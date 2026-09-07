# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources(flash_at32.c)
zephyr_library_sources_ifdef(CONFIG_DT_HAS_ARTERY_AT32_NV_FLASH_V1_ENABLED flash_at32_v1.c)
