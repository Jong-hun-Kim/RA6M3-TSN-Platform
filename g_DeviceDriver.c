/*
 * g_pDevice_Driver.c
 *
 *  Created on: 2023. 3. 9.
 *      Author: Jonghun Kim
 */

#include <g_DeviceDriver.h>


void R_IRQ_Initial_Setting()
{
    /* Interrupt ICU Setting */
    R_ICU_ExternalIrqOpen(&g_external_irq11_ctrl, &g_external_irq11_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq12_ctrl, &g_external_irq12_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq13_ctrl, &g_external_irq13_cfg);
    R_ICU_ExternalIrqOpen(&g_external_irq14_ctrl, &g_external_irq14_cfg);

    /* Interrupt Set Enable Register Setting (Enable) */
    R_ICU_ExternalIrqEnable(&g_external_irq11_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq12_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq13_ctrl);
    R_ICU_ExternalIrqEnable(&g_external_irq14_ctrl);
}

void R_AGT_Initial_Setting()
{
    /* AGT0 Initial Setting */
    R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg);

    /* AGT0 Counting Start */
    R_AGT_Start(&g_timer0_ctrl);
}
void pDevice_Driver()
{
    R_IRQ_Initial_Setting();
    R_AGT_Initial_Setting();
}
