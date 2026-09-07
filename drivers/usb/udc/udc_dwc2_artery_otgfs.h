/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * Copyright (c) 2025 Maxjta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Vendor quirks for the Artery AT32 OTG_FS DWC2 controller.
 *
 * Lifted from ArteryTek/zephyr artery-v1.1-branch @ 456548bdfa9
 * drivers/usb/udc/udc_dwc2_vendor_quirks.h and adapted to the Zephyr 4.4
 * per-vendor quirk header layout (dwc2_get_base() instead of config->base).
 */

#ifndef ZEPHYR_DRIVERS_USB_UDC_DWC2_ARTERY_OTGFS_H
#define ZEPHYR_DRIVERS_USB_UDC_DWC2_ARTERY_OTGFS_H

#include <zephyr/sys/sys_io.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/clock_control/at32_clock_control.h>
#include <usb_dwc2_hw.h>

struct usb_dw_at32_clk {
	uint16_t clkid;
};

static inline int at32_usb_otg_enable_clk(const struct usb_dw_at32_clk *clk)
{
	(void)clock_control_on(AT32_CLOCK_CONTROLLER,
			       (clock_control_subsys_t)&clk->clkid);
	return 0;
}

static inline int at32_usb_otg_enable_phy(const struct device *dev)
{
	struct usb_dwc2_reg *const base = dwc2_get_base(dev);
	mem_addr_t ggpio_reg = (mem_addr_t)&base->ggpio;
	uint32_t ghwcfg2;

	ghwcfg2 = sys_read32((mem_addr_t)&base->ghwcfg2);

	if (usb_dwc2_get_ghwcfg2_hsphytype(ghwcfg2) ==
	    USB_DWC2_GHWCFG2_HSPHYTYPE_UTMIPLUSULPI) {
		sys_clear_bits((mem_addr_t)&base->gusbcfg,
			       USB_DWC2_GUSBCFG_ULPI_UTMI_SEL_ULPI);
	}

	sys_set_bits(ggpio_reg, USB_DWC2_GGPIO_STM32_PWRDWN | USB_DWC2_GGPIO_STM32_VBDEN);

	return 0;
}

static inline int at32_usb_otg_disable_phy(const struct device *dev)
{
	struct usb_dwc2_reg *const base = dwc2_get_base(dev);
	mem_addr_t ggpio_reg = (mem_addr_t)&base->ggpio;

	sys_clear_bits(ggpio_reg, USB_DWC2_GGPIO_STM32_PWRDWN | USB_DWC2_GGPIO_STM32_VBDEN);

	return 0;
}

static inline int at32_usb_init_caps(const struct device *dev)
{
	const struct udc_dwc2_config *const config = dev->config;
	struct udc_data *data = dev->data;

	if (usb_dwc2_get_ghwcfg2_hsphytype(config->ghwcfg2) != 0) {
		data->caps.hs = true;
	}

	return 0;
}

#define QUIRK_ARTERY_OTGFS_DEFINE(n)						\
	static const struct usb_dw_at32_clk at32_clk_##n = {			\
		.clkid = DT_INST_CLOCKS_CELL(n, id),				\
	};									\
										\
	static int at32_usb_otg_enable_clk_##n(const struct device *dev)	\
	{									\
		return at32_usb_otg_enable_clk(&at32_clk_##n);			\
	}									\
										\
	const struct dwc2_vendor_quirks dwc2_vendor_quirks_##n = {		\
		.pre_enable = at32_usb_otg_enable_clk_##n,			\
		.post_enable = at32_usb_otg_enable_phy,				\
		.disable = at32_usb_otg_disable_phy,				\
		.caps = at32_usb_init_caps,					\
		.irq_clear = NULL,						\
	};

DT_INST_FOREACH_STATUS_OKAY(QUIRK_ARTERY_OTGFS_DEFINE)

#endif /* ZEPHYR_DRIVERS_USB_UDC_DWC2_ARTERY_OTGFS_H */
