/*
 * g_DeviceDriver.h
 *
 *  Created on: 2023. 3. 9.
 *      Author: Jonghun Kim
 */

#include <hal_data.h>

#ifndef G_DEVICEDRIVER_H_
#define G_DEVICEDRIVER_H_

void R_IRQ_Initial_Setting();
void R_AGT_Initial_Setting();
void pDevice_Driver();

static const enum _irq_number {
    EXTERNAL_INTERRUPT_11 = 11,
    EXTERNAL_INTERRUPT_12,
    EXTERNAL_INTERRUPT_13,
    EXTERNAL_INTERRUPT_14
} irq_number;

#endif /* G_DEVICEDRIVER_H_ */
