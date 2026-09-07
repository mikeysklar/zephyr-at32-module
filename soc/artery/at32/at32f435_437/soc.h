/*
 * Copyright (c) 2024, Maxjta
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SOC_ARM_ARTERY_AT32F435_437_SOC_H_
#define _SOC_ARM_ARTERY_AT32F435_437_SOC_H_

#ifndef _ASMLANGUAGE

/*
 * at32f435_437.h unconditionally pulls in the whole standard peripheral
 * library, including at32f435_437_usb.h, which declares
 * usb_disconnect(otg_global_type *). Zephyr includes soc.h from
 * cmsis_core_m.h, so that declaration reaches every file that includes
 * <zephyr/kernel.h> and collides with application code using the same name
 * (CircuitPython's supervisor/usb.h). The F435 USB controller is driven by
 * udc_dwc2, not by the library's USB code, so hide the declaration here.
 */
#define usb_disconnect at32_hal_usb_disconnect
#include <at32f435_437.h>
#undef usb_disconnect

#endif /* _ASMLANGUAGE */

#endif /* _SOC_ARM_ARTERY_AT32F435_437_SOC_H_ */
