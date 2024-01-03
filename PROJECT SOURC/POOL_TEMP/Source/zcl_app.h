#ifndef ZCL_APP_H
#define ZCL_APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "zcl.h"

#define APP_REPORT_EVT                                                          0x0001
#define APP_READ_SENSORS_EVT                                                    0x0002
#define APP_SAVE_ATTRS_EVT                                                      0x0004
#define APP_SEND_SENSORS_EVT                                                    0x0008
#define APP_REPORT_BATT_EVT                                                     0x0010
#define APP_SEND_BATT_EVT                                                       0x0020
#define APP_REPORT_PER_EVT                                                      0x0040
#define APP_SEND_PER_SENSORS_EVT                                                0x0080
#define APP_LED_EVT                                                             0x0100
#define APP_LED_PERIOD_EVT                                                      0x0200
   
#define NW_APP_CONFIG                                                           0x0401
  
#define R                                                                       ACCESS_CONTROL_READ
#define RW                                                                      (R | ACCESS_CONTROL_WRITE | ACCESS_CONTROL_AUTH_WRITE)
#define RR                                                                      (R | ACCESS_REPORTABLE)
#define RRW                                                                     (R | ACCESS_REPORTABLE | ACCESS_CONTROL_WRITE | ACCESS_CONTROL_AUTH_WRITE)

#define BASIC                                                                   ZCL_CLUSTER_ID_GEN_BASIC
#define POWER_CFG                                                               ZCL_CLUSTER_ID_GEN_POWER_CFG
#define TEMP                                                                    ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT
#define ILLUMINANCE                                                             ZCL_CLUSTER_ID_MS_ILLUMINANCE_MEASUREMENT
 
#define ATTRID_ReportDelay                                                      0x0201      
#define ATTRID_ComparisonPreviousData                                           0x0205   
#define ATTRID_LED_ONOFF                                                        0x0600    
#define ATTRID_LED_F                                                            0x0601    
#define ATTRID_LED_D                                                            0x0602    

#define ZCL_UINT8                                                               ZCL_DATATYPE_UINT8
#define ZCL_UINT16                                                              ZCL_DATATYPE_UINT16
#define ZCL_INT16                                                               ZCL_DATATYPE_INT16
#define ZCL_INT8                                                                ZCL_DATATYPE_INT8
#define ZCL_INT32                                                               ZCL_DATATYPE_INT32
#define ZCL_UINT32                                                              ZCL_DATATYPE_UINT32
#define ZCL_SINGLE                                                              ZCL_DATATYPE_SINGLE_PREC

typedef struct {
    uint32 ReportDelay;
    uint32 LedFreq;
    uint16 LeDuration;
    bool ComparisonPreviousData;
    bool BackLight;
} application_config_t;

extern SimpleDescriptionFormat_t zclApp_FirstEP;
extern uint8 zclApp_BatteryVoltage;
extern uint8 zclApp_BatteryPercentageRemainig;
extern uint32 zclApp_BatteryAlarm;
extern int16 zclApp_Temperature_Sensor_MeasuredValue;
extern uint16 zclApp_IlluminanceSensor_MeasuredValue;
extern uint16 zclApp_IlluminanceSensor_MeasuredValueRawAdc;

extern CONST zclAttrRec_t zclApp_AttrsFirstEP[];
extern CONST uint8 zclApp_AttrsFirstEPCount;
extern application_config_t zclApp_Config;

extern const uint8 zclApp_ManufacturerName[];
extern const uint8 zclApp_ModelId[];
extern const uint8 zclApp_PowerSource;
extern const uint8 zclApp_SWBuildID[];

extern void zclApp_Init(byte task_id);
extern UINT16 zclApp_event_loop(byte task_id, UINT16 events);
void user_delay_ms(uint32_t period);

#ifdef __cplusplus
}
#endif

#endif /* ZCL_APP_H */
