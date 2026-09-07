# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_PINCTRL_AT32_MUX pinctrl_at32_mux.c)
zephyr_library_sources_ifdef(CONFIG_PINCTRL_AT32_IOMAP pinctrl_at32_iomap.c)
