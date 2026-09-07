# SPDX-License-Identifier: Apache-2.0

zephyr_library()
# spi_rtio.h is a private upstream header (was <zephyr/drivers/spi/rtio.h> in 4.3)
zephyr_library_include_directories(${ZEPHYR_BASE}/drivers/spi)
zephyr_library_sources(spi_at32.c)
