# SPDX-License-Identifier: Apache-2.0

zephyr_library()
# adc_context.h is a private upstream header
zephyr_library_include_directories(${ZEPHYR_BASE}/drivers/adc)
zephyr_library_sources(adc_at32.c)
