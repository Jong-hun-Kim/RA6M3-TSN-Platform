/*
 * g_TSN_TAS.c
 *
 *  Created on: 2023. 3. 9.
 *      Author: Jonghun Kim
 */

#include <g_TSN_TAS.h>

extern volatile uint16_t   pSize;
extern volatile Queue_Head head[STRICT_QUEUE_MAX_SIZE];
extern volatile uint32_t   agt_counter;

//extern Eth_TX_Frame         EthTxFrame;
//extern Eth_RX_Frame         EthRxFrame;

Eth_TX_Frame               EthPtFrame;                           // Preemption Transmit Frame Structure
Eth_RX_Frame               EthPrFrame;                           // Preemption Receive Frame Structure

extern volatile uint8_t    TAS_OPTIMIZE_MODE;

uint8_t                    index = 0;
uint8_t                    gmode = 0;
int                        offset = 0;
volatile int               preemptable = 0;
volatile uint32_t          count = 0;
volatile uint32_t          time_conf = 0;

volatile uint32_t          TAS_TIME_GAP = TAS_GCL_CYCLE - TAS_GCL_GUARD_SIZE;

/* Time-sensitive Network TAS[Time-aware-shaper]-based Gate Control List */
GC_Type                    GCL[TAS_GCL_TS_SIZE][STRICT_QUEUE_MAX_SIZE] = {
                            // |                   <VLAN PRI>                   |
                            // |  0  |  1  |  2  |  3  |  4  |   5  |   6  |  7 |
                            // | BE5 | BE4 | BE3 | BE2 | BE1 | AVB2 | AVB1 | CT |
                               {close, close, close, close, close, close, close, close},
                               {close, close, close, close, close, close, close, open},
                               {close, close, close, close, close, open, open, close},
                               {open, open, open, open, open, close, close, close}
};

bool                       readyState = false;
static int                 totalTime[AVB_TRAFFIC_CLASS] = {0};
volatile int               pTable[AVB_TRAFFIC_CLASS][PREEMPTION_OFFSET_SAMPLE] = {0};

uint8_t TSN_TAS_Gate_Mode_Select()
{
    count = agt_counter;

    if (count < TAS_GCL_GUARD_BAND)
        return BE_Traffic;
    else if (count >= TAS_GCL_GUARD_BAND && count < TAS_GCL_CONTROL)
        return Guard_Band;
    else if (count >= TAS_GCL_CONTROL && count < TAS_GCL_AVB)
        return Control_Traffic;
    else
        return AVB_Traffic;
}
uint16_t TSN_TAS_Gate_Control(volatile int mode)
{
    switch (mode)
    {
        case BE_Traffic:
            if(head[TAS_GCL_BE_A].count > 0)
                return TAS_GCL_BE_A;
            else if(head[TAS_GCL_BE_B].count > 0)
                return TAS_GCL_BE_B;
            else if(head[TAS_GCL_BE_C].count > 0)
                return TAS_GCL_BE_C;
            else if(head[TAS_GCL_BE_D].count > 0)
                return TAS_GCL_BE_D;
            else if(head[TAS_GCL_BE_E].count > 0)
                return TAS_GCL_BE_E;
            break;
        case Guard_Band:
            return TAS_GCL_GUARD;
            break;
        case Control_Traffic:
            if(head[TAS_GCL_CT].count > 0)
                return TAS_GCL_CT;
            break;
        case AVB_Traffic: // need algorithm about CBS[Credit-based Shaper]
            if(head[TAS_GCL_AVB_A].count > 0)
                return TAS_GCL_AVB_A;
            else if(head[TAS_GCL_AVB_B].count > 0)
                return TAS_GCL_AVB_B;
            break;
    }
    return TAS_GCL_NONE;
}
void TSN_TAS_setEthFrame(int length)
{
    EthPtFrame.vlan_tag[0] = (uint8_t)((EthPtFrame.vlan_tag[0] & 0x1F) | ((EthPrFrame.vlan_tag.PCP << 5) & 0xE0));

    EthPtFrame.eth_type[0] = (uint8_t)((length >> 8) & 0xFF);
    EthPtFrame.eth_type[1] = (uint8_t)(length & 0xFF);

    memcpy(EthPtFrame.payload, EthPrFrame.payload, (size_t)length - 1);
}
preemState TSN_TAS_Preemption()
{
    time_conf = TAS_GCL_CONTROL - count;
    uint8_t pos = TAS_GCL_NONE;
    float byte_time = ETH_BIT_RATE / 8.0f; // Exactly, this value is 0.08[us] (Bit-rate is 100[Mbps])

    preemptable = (int)((float)time_conf * GLOBAL_TIMER_PERIOD * byte_time);

    if(head[TAS_GCL_AVB_A].count > 0)
        pos = TAS_GCL_AVB_A;
    else if(head[TAS_GCL_AVB_B].count > 0)
        pos = TAS_GCL_AVB_B;
    else
        return preemption_fail;

    if(readyState && TAS_OPTIMIZE_MODE)
        preemptable -= (FRAME_TAIL_OFFSET + FRAME_JITTER_OFFSET);

    if (preemptable > PREEMPTION_MIN)
    {
        EthPrFrame = *(RQ_dequeue(pos, front_queue));

        if (EthPrFrame.eth_type - preemptable < PREEMPTION_MIN)
        {
            RQ_enqueue(&EthPrFrame, first_in_queue);
            return preemption_fail;
        }

        if (preemptable >= EthPrFrame.eth_type)
        {
            preemptable = EthPrFrame.eth_type;
            TSN_TAS_setEthFrame(preemptable);
            pSize = (uint16_t)preemptable;
        }
        else
        {
            pSize = (uint16_t)preemptable;
            TSN_TAS_setEthFrame(preemptable);
            EthPrFrame.eth_type = EthPrFrame.eth_type - (uint16_t)preemptable;
            RQ_enqueue(&EthPrFrame, first_in_queue);
        }
        return preemption_success;
    }

    return preemption_fail;
}
/* TSN History Table (Circular Queue) */
void TSN_TAS_Optimizing_Preemption(uint8_t p, int byte)
{
    if (readyState)
        totalTime[p] -= pTable[p][index];

    pTable[p][index] = byte;
    totalTime[p] += pTable[p][index++];

    if (index >= PREEMPTION_OFFSET_SAMPLE)
    {
        index = 0;
        readyState = true;
    }
}
