// #define DO_DEBUG_UART
#define OUTDOOR_LONG_RANGE

#define TC_LINKKEY_JOIN
#define NV_INIT
#define NV_RESTORE
#define HAL_LCD FALSE
#define TP2_LEGACY_ZC
#define NWK_AUTO_POLL
#define MULTICAST_ENABLED                                                       FALSE
#define ZCL_READ
#define ZCL_WRITE
#define ZCL_BASIC
#define ZCL_IDENTIFY
#define ZCL_REPORTING_DEVICE
#define ZSTACK_DEVICE_BUILD                                                     (DEVICE_BUILD_ENDDEVICE)

#define DISABLE_GREENPOWER_BASIC_PROXY
#define BDB_FINDING_BINDING_CAPABILITY_ENABLED                                  1

#define ISR_KEYINTERRUPT
#define HAL_BUZZER                                                              FALSE
#define HAL_I2C                                                                 FALSE

#define POWER_SAVING

#define FACTORY_RESET_BY_LONG_PRESS_PORT                                        0x04 //port2
#define APP_COMMISSIONING_BY_LONG_PRESS                                         TRUE
#define APP_COMMISSIONING_BY_LONG_PRESS_PORT                                    0x04 //port2

#ifdef DO_DEBUG_UART
#define HAL_UART                                                                TRUE
#define HAL_UART_DMA                                                            1
#define INT_HEAP_LEN                                                            (2685 - 0x4B - 0xBB)
#endif

#define DS18B20_PORT                                                            1
#define DS18B20_PIN                                                             4

#define TSENS_SBIT                                                              P1_4
#define TSENS_BV                                                                BV(4)
#define TSENS_DIR                                                               P1DIR

#define LUMOISITY_PORT                                                          0
#define LUMOISITY_PIN                                                           5

#define HAL_KEY_P2_INPUT_PINS                                                   BV(0)

#ifdef OUTDOOR_LONG_RANGE
#define HAL_PA_LNA_CC2592
#define APP_TX_POWER                                                            TX_PWR_PLUS_19
#endif

#include "hal_board_cfg.h"
#include "stdint.h"
#include "stddef.h"