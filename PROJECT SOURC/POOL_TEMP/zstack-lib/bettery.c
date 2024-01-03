#include "battery.h"
#include "hal_adc.h"
#include "utils.h"
#include "OSAL.h"
#include "zcl.h"
#include "zcl_general.h"
#include "bdb_interface.h"

#define MULTI (float)0.443
#define VOLTAGE_MIN 2.0
#define VOLTAGE_MAX 3.3

#ifndef ZCL_BATTERY_REPORT_REPORT_CONVERTER
#define ZCL_BATTERY_REPORT_REPORT_CONVERTER(millivolts) getBatteryRemainingPercentageZCLCR2032(millivolts)
#endif

uint8 zclBattery_Voltage = 0xff;
uint8 zclBattery_PercentageRemainig = 0xff;
uint16 zclBattery_RawAdc = 0xff;
uint32 zclApp_BatteryAlarm = 0x00000000;

uint8 getBatteryVoltageZCL(uint16 millivolts) {
    uint8 volt8 = (uint8)(millivolts / 100);
    if ((millivolts - (volt8 * 100)) > 50) {
        return volt8 + 1;
    } else {
        return volt8;
    }
}

uint16 getBatteryVoltage(void) {
    HalAdcSetReference(HAL_ADC_REF_125V);
    zclBattery_RawAdc = adcReadSampled(HAL_ADC_CHANNEL_VDD, HAL_ADC_RESOLUTION_14, HAL_ADC_REF_125V, 10);
    return (uint16)(zclBattery_RawAdc * MULTI);
}

uint8 getBatteryRemainingPercentageZCL(uint16 millivolts) { return (uint8)mapRange(VOLTAGE_MIN, VOLTAGE_MAX, 0.0, 200.0, millivolts); }

uint8 getBatteryRemainingPercentageZCLCR2032(uint16 volt16) {
    float battery_level;
    if (volt16 >= 3200) {
        battery_level = 100;
    } else if (volt16 > 2600) {
        battery_level = 100 - ((3200 - volt16) * 70) / 600;
    } else if (volt16 > 2200) {
        battery_level = 30 - ((2600 - volt16) * 30) / 400;
    } else {
        battery_level = 0;
    }
    return (uint8)(battery_level * 2);
}

void zclBattery_Read(void) {
    uint16 millivolts = getBatteryVoltage();
    zclBattery_Voltage = getBatteryVoltageZCL(millivolts);
    zclBattery_PercentageRemainig = ZCL_BATTERY_REPORT_REPORT_CONVERTER(millivolts);
    if(zclBattery_PercentageRemainig/2 <= 5){
      zclApp_BatteryAlarm = 0x00000001;
    }else{
      zclApp_BatteryAlarm = 0x00000000;
    }
}