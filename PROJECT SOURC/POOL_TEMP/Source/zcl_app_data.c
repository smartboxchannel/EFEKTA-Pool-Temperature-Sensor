#include "AF.h"
#include "OSAL.h"
#include "ZComDef.h"
#include "ZDConfig.h"
#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ms.h"
#include "zcl_ha.h"
#include "zcl_app.h"
#include "battery.h"

#define APP_DEVICE_VERSION                                                      2
#define APP_FLAGS                                                               0
#define APP_HWVERSION                                                           1
#define APP_ZCLVERSION                                                          1

#define DEFAULT_SET_CPD                                                         TRUE
#ifdef OUTDOOR_LONG_RANGE
#define APP_REPORT_DELAY                                                        120
#else
#define APP_REPORT_DELAY                                                        30
#endif
#define DEFAULT_SET_LED                                                         TRUE
#define APP_LED_BLINK_FREQ                                                      10
#define APP_LED_BLINK_DUR                                                       50

const uint16 zclApp_clusterRevision_all = 0x0001;
const uint8 zclApp_HWRevision = APP_HWVERSION;
const uint8 zclApp_ZCLVersion = APP_ZCLVERSION;
const uint8 zclApp_ApplicationVersion = 3;
const uint8 zclApp_StackVersion = 4;

int16 zclApp_Temperature_Sensor_MeasuredValue = 0;
uint16 zclApp_HumiditySensor_MeasuredValue = 0;
uint16 zclApp_IlluminanceSensor_MeasuredValue = 0;
uint16 zclApp_IlluminanceSensor_MeasuredValueRawAdc = 0;

const uint8 zclApp_ManufacturerName[] = {9, 'E', 'f', 'e', 'k', 't', 'a', 'L', 'a', 'b'};
const uint8 zclApp_ModelId[] = {29, 'P', 'o', 'o', 'l', '_', 'T', 'e', 'm', 'p', 'e', 'r', 'a', 't', 'u', 'r', 'e', '_', 's', 'm', 'a', 'r', 't', '_', 's', 'e', 'n', 's', 'o', 'r'};
const uint8 zclApp_SWBuildID[] = {5, '0', '.', '0', '.', '1'};
const uint8 zclApp_DateCode[] = { 12, '2', '0', '2', '3', '0', '6', '1', '5', ' ', '6', '4', '3'};
const uint8 zclApp_PowerSource = POWER_SOURCE_BATTERY;

application_config_t zclApp_Config = {
    .ReportDelay = APP_REPORT_DELAY,
    .LedFreq = APP_LED_BLINK_FREQ,
    .LeDuration = APP_LED_BLINK_DUR,
    .ComparisonPreviousData = DEFAULT_SET_CPD,
    .BackLight = DEFAULT_SET_LED
};

CONST zclAttrRec_t zclApp_AttrsFirstEP[] = {
    {BASIC, {ATTRID_BASIC_ZCL_VERSION, ZCL_UINT8, R, (void *)&zclApp_ZCLVersion}},
    {BASIC, {ATTRID_BASIC_APPL_VERSION, ZCL_UINT8, R, (void *)&zclApp_ApplicationVersion}},
    {BASIC, {ATTRID_BASIC_STACK_VERSION, ZCL_UINT8, R, (void *)&zclApp_StackVersion}},
    {BASIC, {ATTRID_BASIC_HW_VERSION, ZCL_UINT8, R, (void *)&zclApp_HWRevision}},
    {BASIC, {ATTRID_BASIC_MANUFACTURER_NAME, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_ManufacturerName}},
    {BASIC, {ATTRID_BASIC_MODEL_ID, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_ModelId}},
    {BASIC, {ATTRID_BASIC_DATE_CODE, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_DateCode}},
    {BASIC, {ATTRID_BASIC_POWER_SOURCE, ZCL_DATATYPE_ENUM8, R, (void *)&zclApp_PowerSource}},
    {BASIC, {ATTRID_BASIC_SW_BUILD_ID, ZCL_DATATYPE_CHAR_STR, R, (void *)&zclApp_SWBuildID}},
    {BASIC, {ATTRID_CLUSTER_REVISION, ZCL_DATATYPE_UINT16, R, (void *)&zclApp_clusterRevision_all}},
    
    {BASIC, {ATTRID_ReportDelay, ZCL_UINT16, RW, (void *)&zclApp_Config.ReportDelay}},
    {BASIC, {ATTRID_ComparisonPreviousData, ZCL_DATATYPE_BOOLEAN, RW, (void *)&zclApp_Config.ComparisonPreviousData}},
    {BASIC, {ATTRID_LED_ONOFF, ZCL_DATATYPE_BOOLEAN, RW, (void *)&zclApp_Config.BackLight}},
    {BASIC, {ATTRID_LED_F, ZCL_UINT16, RW, (void *)&zclApp_Config.LedFreq}},
    {BASIC, {ATTRID_LED_D, ZCL_UINT16, RW, (void *)&zclApp_Config.LeDuration}},
    
    {POWER_CFG, {ATTRID_POWER_CFG_BATTERY_VOLTAGE, ZCL_UINT8, R, (void *)&zclBattery_Voltage}},
    {POWER_CFG, {ATTRID_POWER_CFG_BATTERY_PERCENTAGE_REMAINING, ZCL_UINT8, R, (void *)&zclBattery_PercentageRemainig}},
    {POWER_CFG, {ATTRID_POWER_CFG_BAT_ALARM_STATE, ZCL_DATATYPE_BITMAP32, R, (void *)&zclApp_BatteryAlarm}},
    
    {TEMP, {ATTRID_MS_TEMPERATURE_MEASURED_VALUE, ZCL_INT16, R, (void *)&zclApp_Temperature_Sensor_MeasuredValue}},
    {ILLUMINANCE, {ATTRID_MS_ILLUMINANCE_MEASURED_VALUE, ZCL_UINT16, R, (void *)&zclApp_IlluminanceSensor_MeasuredValue}}
};
     
uint8 CONST zclApp_AttrsFirstEPCount = (sizeof(zclApp_AttrsFirstEP) / sizeof(zclApp_AttrsFirstEP[0]));
const cId_t zclApp_InClusterList[] = {ZCL_CLUSTER_ID_GEN_BASIC, POWER_CFG, TEMP, ILLUMINANCE};
#define APP_MAX_INCLUSTERS (sizeof(zclApp_InClusterList) / sizeof(zclApp_InClusterList[0]))
const cId_t zclApp_OutClusterListFirstEP[] = {POWER_CFG, TEMP};
#define APP_MAX_OUTCLUSTERS_FIRST_EP (sizeof(zclApp_OutClusterListFirstEP) / sizeof(zclApp_OutClusterListFirstEP[0]))

SimpleDescriptionFormat_t zclApp_FirstEP = {
    1,
    ZCL_HA_PROFILE_ID,
    ZCL_HA_DEVICEID_SIMPLE_SENSOR,
    APP_DEVICE_VERSION,
    APP_FLAGS,
    APP_MAX_INCLUSTERS,
    (cId_t *)zclApp_InClusterList,
    APP_MAX_OUTCLUSTERS_FIRST_EP,
    (cId_t *)zclApp_OutClusterListFirstEP
};

