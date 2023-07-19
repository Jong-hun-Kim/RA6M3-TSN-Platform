/*
 * g_ethernet_lib.h
 *
 *  Created on: 2023. 3. 7.
 *      Author: Jonghun Kim
 */

#include <hal_data.h>

#ifndef G_ETHERNET_LIB_H_
#define G_ETHERNET_LIB_H_

#define ETH_RX_EVENT_NUMBER             7
#define ETH_PREAMBLE_SIZE               7
#define ETH_SFD_SIZE                    1
#define ETH_ADDR_SIZE                   6
#define ETH_TYPE_SIZE                   2
#define ETH_PAYLOAD_MAX_SIZE            1500
#define ETH_CRC_SIZE                    4
#define ETH_INTERFRAME_GAP              12

#define VLAN_TYPE                       0x8100
#define VLAN_PCP_INDEX                  13
#define VLAN_PCP_MASK                   0xE000
#define VLAN_DEI_INDEX                  12
#define VLAN_DEI_MASK                   0x1000
#define VLAN_ID_MASK                    0x0FFF

#define ETH_PHYSICAL_SIZE               (ETH_PREAMBLE_SIZE + ETH_SFD_SIZE + ETH_INTERFRAME_GAP)
#define ETH_HEADER_SIZE                 (ETH_ADDR_SIZE * 2 + ETH_TYPE_SIZE * 3)
#define DATA_LINK_SIZE                  (ETH_HEADER_SIZE + ETH_PAYLOAD_MAX_SIZE)
#define TOTAL_FRAME_SIZE                (DATA_LINK_SIZE + ETH_PHYSICAL_SIZE + ETH_CRC_SIZE)

#define ETHERC_ECMR_RTM                 0x00000004

#define ETH_BIT_RATE                    100 // Mbit-per-second Unit

typedef struct _Eth_TX_Frame Eth_TX_Frame;
typedef struct _Eth_RX_Frame Eth_RX_Frame;

void R_Ethernet_Initial();
uint16_t getVLANTag(uint16_t *VLTG);
void getEthernetFrame(Eth_TX_Frame *ethtxf, uint8_t *DST, uint8_t *SRC, uint16_t *VLTG);
void setEthernetFrame(Eth_RX_Frame *ethrxf, uint8_t *p_buffer);

enum Eth_FrameNo
{
    Eth_DstAddr_1 = 0,
    Eth_DstAddr_2,
    Eth_DstAddr_3,
    Eth_DstAddr_4,
    Eth_DstAddr_5,
    Eth_DstAddr_6,
    Eth_SrcAddr_1,
    Eth_SrcAddr_2,
    Eth_SrcAddr_3,
    Eth_SrcAddr_4,
    Eth_SrcAddr_5,  // 10
    Eth_SrcAddr_6,
    Eth_MacType_1,
    Eth_MacType_2,
    Eth_Data_Start, // Ethernet Data Field Delimiter (18)
};

typedef struct _Eth_VLAN_Format
{
    uint16_t        PCP;            /* [15:13] VLAN Priority Code Point (3-bit) */
    uint16_t        DEI;            /* [12:12] VLAN Drop Eligible Indicator (1-bit) */
    uint16_t        VLAN_ID;        /* [11: 0] VLAN Identifier (12-bit) */
} ETH_VLAN_FORMAT;

typedef struct _Eth_TX_Frame {
    volatile uint8_t         dst_addr[ETH_ADDR_SIZE];
    volatile uint8_t         src_addr[ETH_ADDR_SIZE];

    volatile uint8_t         vlan_type[ETH_TYPE_SIZE];
    volatile uint8_t         vlan_tag[ETH_TYPE_SIZE];

    volatile uint8_t         eth_type[ETH_TYPE_SIZE];
    uint8_t                  payload[ETH_PAYLOAD_MAX_SIZE];
} Eth_TX_Frame;

typedef struct _Eth_RX_Frame {
    volatile uint8_t         dst_addr[ETH_ADDR_SIZE];
    volatile uint8_t         src_addr[ETH_ADDR_SIZE];

    volatile uint16_t        vlan_type;
    volatile ETH_VLAN_FORMAT vlan_tag;

    volatile uint16_t        eth_type;
    uint8_t                  payload[ETH_PAYLOAD_MAX_SIZE];
} Eth_RX_Frame;

#endif /* G_ETHERNET_LIB_H_ */
