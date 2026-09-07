# SPDX-License-Identifier: Apache-2.0

zephyr_library()
# i2c-priv.h is a private upstream header
zephyr_library_include_directories(${ZEPHYR_BASE}/drivers/i2c)
zephyr_library_sources(i2c_at32.c)
