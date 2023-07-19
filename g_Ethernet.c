/*
 * g_ethernet_lib.c
 *
 *  Created on: 2023. 3. 7.
 *      Author: Jonghun Kim
 */

#include <g_Ethernet.h>

ETH_VLAN_FORMAT *VLANTag;

void R_Ethernet_Initial()
{
    fsp_err_t err = FSP_SUCCESS;

    R_ETHER_Open(&g_ether0_ctrl, &g_ether0_cfg);

    do {
        err = R_ETHER_LinkProcess(&g_ether0_ctrl);
    } while (FSP_SUCCESS != err);

    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_10_PIN_08, BSP_IO_LEVEL_LOW);
}
uint16_t getVLANTag(uint16_t *VLTG)
{
    VLANTag->PCP = (uint16_t)((VLTG[0] << VLAN_PCP_INDEX) & VLAN_PCP_MASK);
    VLANTag->DEI = (uint16_t)((VLTG[1] << VLAN_DEI_INDEX) & VLAN_DEI_MASK);
    VLANTag->VLAN_ID = VLTG[2] & VLAN_ID_MASK;

    return (uint16_t)(VLANTag->PCP | VLANTag->DEI | VLANTag->VLAN_ID);
}
void getEthernetFrame(Eth_TX_Frame *ethtxf, uint8_t *DST, uint8_t *SRC, uint16_t *VLTG)
{
    // Ethernet Frame Destination & Source MAC Address Setting //
    for(uint8_t i = 0; i < ETH_ADDR_SIZE; i++)
    {
        ethtxf->dst_addr[i] = DST[i];
        ethtxf->src_addr[i] = SRC[i];
    }

    // Ethernet Frame VLAN Type Setting: 0x8100 //
    ethtxf->vlan_type[0] = (uint8_t)((VLAN_TYPE >> 8) & 0xFF);
    ethtxf->vlan_type[1] = (uint8_t)(VLAN_TYPE & 0xFF);

    // Ethernet Frame VLAN Security Setting //
    volatile uint16_t VLAN_Tag = getVLANTag(VLTG);
    ethtxf->vlan_tag[0] = (uint8_t)((VLAN_Tag >> 8) & 0xFF);
    ethtxf->vlan_tag[1] = (uint8_t)(VLAN_Tag & 0xFF);

    // Ethernet Frame Type & Length Setting //
    ethtxf->eth_type[0] = (uint8_t)((ETH_PAYLOAD_MAX_SIZE >> 8) & 0xFF);
    ethtxf->eth_type[1] = (uint8_t)(ETH_PAYLOAD_MAX_SIZE & 0xFF);

    // Ethernet Frame Payload Setting //
    memset(ethtxf->payload, 0, sizeof(ethtxf->payload));
}
void setEthernetFrame(Eth_RX_Frame *ethrxf, uint8_t *p_buffer)
{
    // Ethernet Frame Destination & Source MAC Address Setting //
    for(uint8_t i = 0; i < ETH_ADDR_SIZE; i++)
    {
        ethrxf->dst_addr[i] = p_buffer[i];
        ethrxf->src_addr[i] = p_buffer[i + Eth_SrcAddr_1];
    }

    // Define Frame Index Pointer (IEEE 802.3 Ethernet)
    uint32_t fptr = Eth_MacType_2;
    // Ethernet Type/Length (2-bytes)
    ethrxf->eth_type = (uint16_t)(((p_buffer[fptr - 1] << 8) & 0xFF00) | p_buffer[fptr]);

    if (ethrxf->eth_type == 0x8100)
    {
        // VLAN[Virtual LAN] Type: 0x8100 (2-bytes)
        ethrxf->vlan_type = ethrxf->eth_type;

        // Ethernet VLAN Tag Area (Prevent Broadcasting Storm, 2-bytes)
        ethrxf->vlan_tag.PCP = (p_buffer[++fptr] >> 5) & 0x07;
        ethrxf->vlan_tag.DEI = (p_buffer[fptr++] >> 4) & 0x01;
        ethrxf->vlan_tag.VLAN_ID = (uint16_t)(((p_buffer[fptr - 1] << 8) & 0x0F00) | p_buffer[fptr]);

        ethrxf->eth_type = (uint16_t)(((p_buffer[fptr + 1] << 8) & 0xFF00) | p_buffer[fptr + 2]);
        fptr += 2; // Adjust VLAN offset
    }

    // Compatible with "IEEE 802.3 Ethernet" & "IEEE 802.1Q-based Ethernet II Frame"
    memcpy(ethrxf->payload, &p_buffer[fptr + 1], ethrxf->eth_type);
}
