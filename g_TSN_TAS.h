/*
 * g_TSN_TAS.h
 *
 *  Created on: 2023. 3. 9.
 *      Author: Jonghun Kim
 */

#include <hal_data.h>
#include <g_Ethernet.h>
#include <g_DataStructure.h>

#ifndef G_TSN_TAS_H_
#define G_TSN_TAS_H_

#define AVB_TRAFFIC_CLASS          2
#define PREEMPTION_OFFSET_SAMPLE   10

#define TAS_GCL_TS_SIZE            4     // GCL 4-steps (Guard Band, Control Traffic, AVB Traffic, BE Traffic)
#define GLOBAL_TIMER_PERIOD        10.0f // Unit: Microsecond
#define TAS_GCL_CYCLE              94    // Timer Unit: 10[us] --> TAS HyperPeriod: 940[us]
//#define TAS_GCL_CYCLE              100    // Timer Unit: 10[us] --> TAS HyperPeriod: 1000[us]
#define ETHERNET_TRANSMIT_DELAY    13     // HAL Function Delay: 130[us]
#define TAS_GCL_GUARD_SIZE         ETHERNET_TRANSMIT_DELAY // Guard Band Size: 123.36[us]

/* Gate Control List */
#define TAS_GCL_BE                 0
#define TAS_GCL_OFFSET             1
#define TAS_GCL_PERIOD             (TAS_GCL_CYCLE - TAS_GCL_GUARD_SIZE) / 3
#define TAS_GCL_GUARD_BAND         TAS_GCL_BE + TAS_GCL_PERIOD                       // 27 * 10[us]
#define TAS_GCL_CONTROL            TAS_GCL_GUARD_BAND + TAS_GCL_GUARD_SIZE           // 27 * 10[us]
#define TAS_GCL_AVB                TAS_GCL_CONTROL + TAS_GCL_PERIOD                  // 27 * 10[us]

#define TAS_GCL_SYNC_COMMAND       0xF3

#define PREEMPTION_MIN             64    // 64-bytes (0.08[us] x 64 = 5.12[us])
#define FRAME_TAIL_OFFSET          16    // FCS(4-bytes) + Inter-frame Gap(12-bytes)
#define FRAME_JITTER_OFFSET        16    // TT Traffic Jitter offset

#define TAS_PREEMPTION_ENABLE      1
#define TAS_PREEMPTION_DISABLE     0

#define TAS_OPTIMIZE_ENABLE        1
#define TAS_OPTIMIZE_DISABLE       0

/* TAG[Time-aware Gate] Control List
 * IDX 7: Highest-priority TAG (Control Traffic)
 * IDX 6: (AVB Traffic-A)
 * IDX 5: (AVB Traffic-B)
 * IDX 4: (BE Traffic-A)
 * IDX 3: (BE Traffic-B)
 * IDX 2: (BE Traffic-C)
 * IDX 1: (BE Traffic-D)
 * IDX 0: (BE Traffic-E) */

#define TAS_GCL_BE_E               0
#define TAS_GCL_BE_D               1
#define TAS_GCL_BE_C               2
#define TAS_GCL_BE_B               3
#define TAS_GCL_BE_A               4
#define TAS_GCL_AVB_B              5
#define TAS_GCL_AVB_A              6
#define TAS_GCL_CT                 7
#define TAS_GCL_GUARD              8
#define TAS_GCL_NONE               9

typedef enum _Gate_Control_Type GC_Type;
typedef enum _Gate_Control_Mode GC_Mode;
typedef enum _Preemption_State preemState;

uint8_t TSN_TAS_Gate_Mode_Select();
uint16_t TSN_TAS_Gate_Control(volatile int mode);
void TSN_TAS_setEthFrame(int length);
preemState TSN_TAS_Preemption();
void TSN_TAS_Optimizing_Preemption(uint8_t p, int time);

typedef enum _Gate_Control_Type {
    close = 0,
    open = 1
} GC_Type;

typedef enum _Gate_Control_Mode {
    Guard_Band = 0,
    Control_Traffic = 1,
    AVB_Traffic = 2,
    BE_Traffic = 3
} GC_Mode;

typedef enum _Preemption_State {
    preemption_fail = 0,
    preemption_success
} preemState;

#endif /* G_TSN_TAS_H_ */
