# zephyr-at32-module

Out-of-tree Zephyr module for the Artery AT32F435/437 and the `at_start_f435`
board (AT32F435ZMT7), built against upstream-style Zephyr 4.4.99 instead of
the ArteryTek fork.

## Origin

Everything here is lifted from the ArteryTek Zephyr fork:

- https://github.com/ArteryTek/zephyr, branch `artery-v1.1-branch`,
  commit `456548bdfa9` (Zephyr 4.3.99 base)

The Artery standard peripheral library is **not** in this repo. It stays in
the `hal_at32` west module from the same fork
(`modules/hal/at32`, Apache-2.0) and is passed as a second extra module.

Verified against adafruit/zephyr `52dc937c7` (4.4.99, the tree used by
CircuitPython's `zephyr-cp` port) with Zephyr SDK 1.0.1.

## Layout

```
zephyr/module.yml              board_root, soc_root, dts_root, module_ext_root
modules/modules.cmake          module extension root (see hal_at32 below)
modules/hal_at32/              CMake + Kconfig glue for the hal_at32 module
boards/artery/at_start_f435/   board
soc/artery/at32/               family Kconfig/soc.yml, common/, at32f435_437/
dts/arm/artery/at32f435_437/   at32f435_437.dtsi, at32f435xmt7.dtsi, at32f435zmt7.dtsi
dts/bindings/                  artery,at32-*.yaml, vendor-prefixes.txt
include/zephyr/                dt-bindings (clock, reset, dma) and driver headers
drivers/                       one zephyr_library per driver, selected by devicetree
patches/                       the one upstream patch (USB)
```

### hal_at32 glue

`hal_at32`'s `zephyr/module.yml` declares `cmake-ext: True` and
`kconfig-ext: True`, i.e. it expects its CMake/Kconfig to live in
`ZEPHYR_BASE/modules/hal_at32/`. Upstream has no such directory. This module
declares `module_ext_root: .` and ships `modules/modules.cmake` plus
`modules/hal_at32/{CMakeLists.txt,Kconfig}` (copied from the fork's
`modules/hal_at32/`), which is the mechanism Zephyr provides for exactly this.

## Included

| Area | Files | Notes |
|---|---|---|
| clock_control | `clock_control_at32.c` | `artery,at32-cctl` |
| gpio | `gpio_at32.c` | |
| pinctrl | `pinctrl_at32_mux.c`, `pinctrl_at32_iomap.c` | F435 uses mux |
| reset | `reset_at32.c` | `artery,at32-rctl` |
| interrupt_controller | `intc_exint_at32.c` | |
| serial | `usart_at32.c` | console |
| flash | `flash_at32.c`, `flash_at32_v1.c` | `artery,at32-nv-flash-v1` |
| dma | `dma_at32.c` | |
| adc | `adc_at32.c` | |
| pwm | `pwm_at32.c` | |
| i2c | `i2c_at32.c` | |
| spi | `spi_at32.c` | |
| counter | `counter_at32_timer.c` | |
| usb | `udc_dwc2_artery_otgfs.h` | quirks for upstream `udc_dwc2`, needs the patch below |

Every driver is `default y` and `depends on DT_HAS_ARTERY_AT32_*_ENABLED`,
so an application needs no extra Kconfig beyond enabling the subsystem
(`CONFIG_SPI=y` etc.).

## Not included

- Watchdog (`wdt_wdt_at32.c`), I2S (`i2s_at32.c`), CAN (`can_at32_bxcan.c`):
  the F435 dtsi in the fork has no nodes for them, so they can never be
  selected or compile-tested from this board. Bindings are shipped anyway.
- `udc_at32.c` / `usb_dc_at32.c`: for other AT32 series (F435 uses `udc_dwc2`).
- Other AT32 series (F402/405, F403A/407, F423, F45x) and their boards.
- `hal_at32` itself (see above).

## Required upstream patch (USB only)

Upstream `drivers/usb/udc/udc_dwc2.h` includes vendor quirk headers from a
hard-coded list. `patches/zephyr-udc-dwc2-artery-quirk.patch` adds

```c
#if DT_HAS_COMPAT_STATUS_OKAY(artery_at32_otg_dwc2)
#include "udc_dwc2_artery_otgfs.h"
#endif
```

to that list. The module adds `drivers/usb/udc` to the include path when
`CONFIG_UDC_DWC2` is set, so the header resolves. Apply with

```
cd $ZEPHYR_BASE && git apply /path/to/zephyr-at32-module/patches/zephyr-udc-dwc2-artery-quirk.patch
```

Non-USB builds (blinky, hello_world, flash_shell) do not need the patch.

## Renames relative to the fork

Done mechanically, once:

- vendor prefix `at,` -> `artery,` in compatibles and vendor properties
  (`artery,prescaler`, `artery,adc-prescaler`, `artery,mem2mem`, ...)
- `DT_DRV_COMPAT at_at32_x` -> `artery_at32_x`,
  `DT_HAS_AT_AT32_*_ENABLED` -> `DT_HAS_ARTERY_AT32_*_ENABLED`
- `boards/at/` -> `boards/artery/`, `soc/at/` -> `soc/artery/`,
  `dts/arm/at/` -> `dts/arm/artery/`, board `vendor: artery`,
  board compatible `artery,at_start_f435`
- `dts/bindings/vendor-prefixes.txt` with `artery` (upstream has none;
  Zephyr merges a vendor-prefixes.txt from every `dts_root`)

## Deviations from the fork (4.3.99 -> 4.4.99)

- `board.yml`: added `full_name`, required by the 4.4 board schema.
- `at_start_f435_defconfig`: trimmed to reset/gpio/serial/console. The fork
  also set `CONFIG_DMA=y`, `CONFIG_ADC_AT32_DMA=y` and the legacy
  `CONFIG_USB_DEVICE_VID`; the latter two do not exist in a build that does
  not enable ADC / the old USB stack and fail the Kconfig stage.
- `soc/.../at32f435_437/linker.xx` dropped: unused, and it points at the
  pre-hwmv2 `arch/arm/aarch32` path. The series CMakeLists already selects
  `include/zephyr/arch/arm/cortex_m/scripts/linker.ld`.
- `spi_at32.c`: `<zephyr/drivers/spi/rtio.h>` became the private
  `drivers/spi/spi_rtio.h` in 4.4; include changed and the directory added to
  the library include path.
- `adc_at32.c` / `i2c_at32.c`: unchanged, but `adc_context.h` and
  `i2c-priv.h` are private upstream headers, so the module adds
  `${ZEPHYR_BASE}/drivers/{adc,i2c}` to those libraries' include paths.
- USB quirk block moved from the fork's monolithic
  `udc_dwc2_vendor_quirks.h` into a per-vendor header using the 4.4 API
  (`dwc2_get_base(dev)` instead of `config->base`). Logic is unchanged.
- The hal_at32 glue is provided through `module_ext_root` (see above)
  instead of living in `ZEPHYR_BASE/modules/hal_at32`.

No driver source needed any other change for 4.4.

## Building

```
source <venv with west>/bin/activate
export ZEPHYR_SDK_INSTALL_DIR=/path/to/zephyr-sdk-1.0.1
cd <west workspace containing zephyr 4.4.99>
west build -p always -b at_start_f435 -d build/blinky zephyr/samples/basic/blinky -- \
  -DZEPHYR_EXTRA_MODULES="/path/to/zephyr-at32-module;/path/to/hal_at32"
```

Without a west workspace, pass `-DZEPHYR_BASE` and an explicit
`-DZEPHYR_MODULES="<this module>;<hal_at32>;<modules/hal/cmsis>;<modules/hal/cmsis_6>"`
to cmake.

The four `No SOURCES given to Zephyr library: drivers__gpio` (and
clock_control, reset, serial) CMake warnings are upstream's own empty driver
libraries and are expected when only out-of-tree drivers are selected.

## Verified builds

| Sample | Flash | RAM | Tree |
|---|---|---|---|
| samples/basic/blinky | 16092 B | 4208 B | adafruit/zephyr 52dc937c7 (CircuitPython zephyr-cp workspace) |
| samples/hello_world | 15564 B | 4208 B | adafruit/zephyr 52dc937c7 |
| samples/drivers/flash_shell | 71932 B | 44120 B | adafruit/zephyr 52dc937c7, exercises flash driver |
| blinky + CONFIG_SPI/I2C/PWM/ADC/DMA/COUNTER, ADC_AT32_DMA, SPI_AT32_DMA | 29516 B | 7664 B | adafruit/zephyr 52dc937c7, all tier-2 drivers compiled |
| samples/subsys/usb/cdc_acm | 61472 B | 17952 B | scratch zephyr 52dc937c7 + patches/zephyr-udc-dwc2-artery-quirk.patch |

All builds: SDK 1.0.1, `-DZEPHYR_EXTRA_MODULES=<module>;<hal_at32>`. Nothing has been
flashed to hardware yet.

The fork's pwm, counter and spi sources emit a few `defined but not used`
warnings; they are untouched.
