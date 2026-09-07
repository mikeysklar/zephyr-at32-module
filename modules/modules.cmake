# SPDX-License-Identifier: Apache-2.0
#
# Module extension root: provides the CMake and Kconfig glue for the hal_at32
# west module (its zephyr/module.yml declares cmake-ext/kconfig-ext, which the
# ArteryTek Zephyr fork satisfies from ZEPHYR_BASE/modules/hal_at32).

file(GLOB cmake_modules "${CMAKE_CURRENT_LIST_DIR}/*/CMakeLists.txt")

foreach(module ${cmake_modules})
  get_filename_component(module_dir  ${module} DIRECTORY)
  get_filename_component(module_name ${module_dir} NAME)
  zephyr_string(SANITIZE TOUPPER MODULE_NAME_UPPER ${module_name})

  set_ifndef(ZEPHYR_${MODULE_NAME_UPPER}_CMAKE_DIR ${module_dir})
endforeach()

file(GLOB kconfig_modules "${CMAKE_CURRENT_LIST_DIR}/*/Kconfig")

foreach(module ${kconfig_modules})
  get_filename_component(module_dir  ${module} DIRECTORY)
  get_filename_component(module_name ${module_dir} NAME)
  zephyr_string(SANITIZE TOUPPER MODULE_NAME_UPPER ${module_name})

  set_ifndef(ZEPHYR_${MODULE_NAME_UPPER}_KCONFIG ${module_dir}/Kconfig)
endforeach()
