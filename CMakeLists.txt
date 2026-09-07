# SPDX-License-Identifier: Apache-2.0

zephyr_include_directories(include)

add_subdirectory(drivers)

# The udc_dwc2 vendor quirk header is pulled in by upstream udc_dwc2.h once
# patches/zephyr-udc-dwc2-artery-quirk.patch is applied.
zephyr_include_directories_ifdef(CONFIG_UDC_DWC2 drivers/usb/udc)
