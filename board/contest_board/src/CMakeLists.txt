# STM32N647-EVB Board sources - Contest 2026 Team 273

nuttx_add_library(board)

set(SRCS board_init.c board_appinit.c)

if(CONFIG_STM32N6_SPI1 OR CONFIG_STM32N6_SPI2 OR CONFIG_STM32N6_SPI3)
  list(APPEND SRCS stm32_spi.c)
endif()

target_sources(board PRIVATE ${SRCS})
set_property(GLOBAL PROPERTY LD_SCRIPT "${NUTTX_BOARD_DIR}/scripts/ld.script")
