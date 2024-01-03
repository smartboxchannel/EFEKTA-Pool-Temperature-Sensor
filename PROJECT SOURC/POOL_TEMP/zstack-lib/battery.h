#ifndef _BATTERY_H
#define _BATTERY_H
#define ATTRID_POWER_CFG_BATTERY_VOLTAGE_RAW_ADC                                0x0200

extern uint8 zclBattery_Voltage;
extern uint8 zclBattery_PercentageRemainig;
extern uint16 getBatteryVoltage(void);
extern uint8 getBatteryVoltageZCL(uint16 millivolts);
extern uint8 getBatteryRemainingPercentageZCL(uint16 millivolts);
extern uint8 getBatteryRemainingPercentageZCLCR2032(uint16 volt16);
extern void zclBattery_Read(void);
#endif