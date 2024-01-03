#include "AF.h"
#include "OSAL.h"
#include "OSAL_Clock.h"
#include "OSAL_PwrMgr.h"
#include "ZComDef.h"
#include "ZDApp.h"
#include "ZDNwkMgr.h"
#include "ZDObject.h"
#include "math.h"
#include "nwk_util.h"
#include "zcl.h"
#include "zcl_app.h"
#include "zcl_diagnostic.h"
#include "zcl_general.h"
#include "zcl_ms.h"
#include "bdb.h"
#include "bdb_interface.h"
#include "gp_interface.h"
#include "OnBoard.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"

#include "hal_adc.h"
#include "hal_drivers.h"
#include "hal_key.h"
#include "hal_led.h"
#include "battery.h"
#include "commissioning.h"
#include "factory_reset.h"
#include "utils.h"
#include "ds18b20.h"

#define HAL_KEY_CODE_RELEASE_KEY HAL_KEY_CODE_NOKEY
#define myround(x) ((int)((x)+0.5))

#define POWER_ON_SENSORS()                                                                                                                 \
    do {                                                                                                                                   \
        HAL_TURN_ON_LED4();                                                                                                                \
        IO_PUD_PORT(DS18B20_PORT, IO_PUP);                                                                                                 \
    } while (0)
#define POWER_OFF_SENSORS()                                                                                                                \
    do {                                                                                                                                   \
        HAL_TURN_OFF_LED4();                                                                                                               \
        IO_PUD_PORT(DS18B20_PORT, IO_PDN);                                                                                                 \
    } while (0)

extern bool requestNewTrustCenterLinkKey;
byte zclApp_TaskID;

static bool initOut = false;
static bool pushBut = false;
static uint8 currentSensorsReadingPhase = 0;
static uint8 currentSensorsSendingPhase = 0;
static uint8 currentSensorsSendingPeriodicPhase = 0;
static uint8 currentSensorsSendingMRPhase = 0;
static int16 sendInitReportCount = 0;
static int16 temp_old = 0;
static int16 temp_tr = 15;
static uint16 lumi_old = 0;
static uint16 lumi_tr = 50;

uint8 SeqNum = 0;
afAddrType_t inderect_DstAddr = {.addrMode = (afAddrMode_t)AddrNotPresent, .endPoint = 0, .addr.shortAddr = 0};

static void zclApp_BasicResetCB(void);
static void zclApp_RestoreAttributesFromNV(void);
static void zclApp_SaveAttributesToNV(void);
static ZStatus_t zclApp_ReadWriteAuthCB(afAddrType_t *srcAddr, zclAttrRec_t *pAttr, uint8 oper);
static void zclApp_HandleKeys(byte shift, byte keys);
static void zclApp_Report(void);
static void zclApp_ReportPeriodic(void);
static void zclApp_ReportBatt(void);
static void zclBattery_Report(void);
static void zclApp_ReadSensors(void);
static void zclApp_SendSensors(void);
static void zclApp_SendPeriodicSensors(void);
static void zclApp_SendMandatoryReport(void);
static void zclConfigReport(void);
static void zclApp_ReadDS18B20(void);
static void zclApp_ReadLum(void);
static void zclApp_SendTemp(void);
static void zclApp_SendLum(void);
static void zclApp_ReportTemp(void);
static void zclApp_ReportLumosity(void);
static void zclApp_LedOn(void);
static void zclApp_LedOff(void);

static zclGeneral_AppCallbacks_t zclApp_CmdCallbacks = {
    zclApp_BasicResetCB, // Basic Cluster Reset command
    NULL, // Identify Trigger Effect command
    NULL, // On/Off cluster commands
    NULL, // On/Off cluster enhanced command Off with Effect
    NULL, // On/Off cluster enhanced command On with Recall Global Scene
    NULL, // On/Off cluster enhanced command On with Timed Off
    NULL, // RSSI Location command
    NULL  // RSSI Location Response command
};

void zclApp_Init(byte task_id) {
    zclApp_RestoreAttributesFromNV();
  
    HAL_TURN_ON_LED1();
    user_delay_ms(1500);
    HAL_TURN_OFF_LED1();
    IO_IMODE_PORT_PIN(LUMOISITY_PORT, LUMOISITY_PIN, IO_TRI);
    POWER_OFF_SENSORS();
    
    requestNewTrustCenterLinkKey = FALSE;
    zclApp_TaskID = task_id;
    zclGeneral_RegisterCmdCallbacks(1, &zclApp_CmdCallbacks);
    zcl_registerAttrList(zclApp_FirstEP.EndPoint, zclApp_AttrsFirstEPCount, zclApp_AttrsFirstEP);
    bdb_RegisterSimpleDescriptor(&zclApp_FirstEP);
    zcl_registerReadWriteCB(zclApp_FirstEP.EndPoint, NULL, zclApp_ReadWriteAuthCB);
    zcl_registerForMsg(zclApp_TaskID);
    RegisterForKeys(zclApp_TaskID);
    
    zclApp_ReadSensors();
    osal_start_timerEx(zclApp_TaskID, APP_REPORT_EVT, 15000);
    osal_start_reload_timer(zclApp_TaskID, APP_REPORT_PER_EVT, 1800101);
    osal_start_reload_timer(zclApp_TaskID, APP_REPORT_BATT_EVT, 10600202);
    osal_start_timerEx(zclApp_TaskID, APP_LED_PERIOD_EVT, zclApp_Config.LedFreq*1000);
}

uint16 zclApp_event_loop(uint8 task_id, uint16 events) {
   afIncomingMSGPacket_t *MSGpkt;

    (void)task_id;
    if (events & SYS_EVENT_MSG) {
        while ((MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(zclApp_TaskID))) {
            switch (MSGpkt->hdr.event) {
            case KEY_CHANGE:
                zclApp_HandleKeys(((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys);
                break;
            case ZCL_INCOMING_MSG:
                if (((zclIncomingMsg_t *)MSGpkt)->attrCmd) {
                    osal_mem_free(((zclIncomingMsg_t *)MSGpkt)->attrCmd);
                }
                break;

            default:
                break;
            }
            // Release the memory
            osal_msg_deallocate((uint8 *)MSGpkt);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
    if (events & APP_REPORT_EVT) {
        if(zclApp_Config.ReportDelay < 20){
          zclApp_Config.ReportDelay = 20;
          pushBut = true;
        }
        if(zclApp_Config.ReportDelay > 1800){
          zclApp_Config.ReportDelay = 1800;
          pushBut = true;
        }
        
        if (initOut == false){
          sendInitReportCount++;
          if(sendInitReportCount  >=  5){
            osal_start_timerEx(zclApp_TaskID, APP_REPORT_EVT, zclApp_Config.ReportDelay*1000); 
            initOut = true;
          }else{
            osal_start_timerEx(zclApp_TaskID, APP_REPORT_EVT, 10000);
          }
          pushBut = true;
          zclApp_Report();
        }else{
          osal_start_timerEx(zclApp_TaskID, APP_REPORT_EVT, zclApp_Config.ReportDelay*1000); 
         zclApp_Report(); 
        }
        return (events ^ APP_REPORT_EVT);
    }

    if (events & APP_READ_SENSORS_EVT) {
        zclApp_ReadSensors();
        return (events ^ APP_READ_SENSORS_EVT);
    }
    if (events & APP_REPORT_BATT_EVT) {
        zclApp_ReportBatt();
        return (events ^ APP_REPORT_BATT_EVT);
    }
    if (events & APP_REPORT_PER_EVT) {
        zclApp_ReportPeriodic();
        return (events ^ APP_REPORT_PER_EVT);
    }
    if (events & APP_SEND_PER_SENSORS_EVT) {
        zclApp_SendPeriodicSensors();
        return (events ^ APP_SEND_PER_SENSORS_EVT);
    }
    if (events & APP_SEND_BATT_EVT) {
        zclApp_SendMandatoryReport();
        return (events ^ APP_SEND_BATT_EVT);
    }
    if (events & APP_SEND_SENSORS_EVT) {
        zclApp_SendSensors();
        return (events ^APP_SEND_SENSORS_EVT);
    }
    if (events & APP_SAVE_ATTRS_EVT) {
        zclApp_SaveAttributesToNV();
        return (events ^ APP_SAVE_ATTRS_EVT);
    }
    if (events & APP_LED_PERIOD_EVT) {
          osal_start_timerEx(zclApp_TaskID, APP_LED_PERIOD_EVT, zclApp_Config.LedFreq*1000); 
          zclApp_LedOn();
        return (events ^ APP_LED_PERIOD_EVT);
    }
    if (events & APP_LED_EVT) {
           zclApp_LedOff();
        return (events ^ APP_LED_EVT);
    }
    return 0;
}

static void zclApp_LedOn(void){
   if(zclApp_Config.BackLight == 1){
if(zclApp_Temperature_Sensor_MeasuredValue <= 1900){
    HAL_TURN_ON_LED1();// BLUE
  }else if(zclApp_Temperature_Sensor_MeasuredValue > 1900 && zclApp_Temperature_Sensor_MeasuredValue <= 2200){
    HAL_TURN_ON_LED2();//GREEN
    HAL_TURN_ON_LED1();// BLUE
  }else if(zclApp_Temperature_Sensor_MeasuredValue > 2200 && zclApp_Temperature_Sensor_MeasuredValue <= 2500){
    HAL_TURN_ON_LED2();//GREEN
  }else if(zclApp_Temperature_Sensor_MeasuredValue > 2500 && zclApp_Temperature_Sensor_MeasuredValue <= 2800){
    HAL_TURN_ON_LED3(); //RED
    HAL_TURN_ON_LED2();//GREEN
  }else if(zclApp_Temperature_Sensor_MeasuredValue > 2800 && zclApp_Temperature_Sensor_MeasuredValue <= 3500){
    HAL_TURN_ON_LED3(); //RED
    HAL_TURN_ON_LED1();// BLUE
  }else if(zclApp_Temperature_Sensor_MeasuredValue > 3500){
    HAL_TURN_ON_LED3(); //RED
  }
  }else{
   HAL_TURN_OFF_LED3(); //RED
   HAL_TURN_OFF_LED2();//GREEN
   HAL_TURN_OFF_LED1();// BLUE 
  } 
   osal_start_timerEx(zclApp_TaskID, APP_LED_EVT, zclApp_Config.LeDuration);
}

static void zclApp_LedOff(void){
   HAL_TURN_OFF_LED1(); //RED
   HAL_TURN_OFF_LED2();//GREEN
   HAL_TURN_OFF_LED3();// BLUE 
}

static void zclApp_HandleKeys(byte portAndAction, byte keyCode) {
    
#if APP_COMMISSIONING_BY_LONG_PRESS == TRUE
    if (bdbAttributes.bdbNodeIsOnANetwork == 1) {
      zclFactoryResetter_HandleKeys(portAndAction, keyCode);
    }
#else
    zclFactoryResetter_HandleKeys(portAndAction, keyCode);
#endif
    zclCommissioning_HandleKeys(portAndAction, keyCode);
    
    if (portAndAction & HAL_KEY_PRESS) {
     
    }   
    if (portAndAction & HAL_KEY_RELEASE) {
        pushBut = true;
        HAL_TURN_OFF_LED1();
        zclApp_Report();
    } 
}

static void zclApp_BasicResetCB(void) {
    zclApp_SaveAttributesToNV();
}

static ZStatus_t zclApp_ReadWriteAuthCB(afAddrType_t *srcAddr, zclAttrRec_t *pAttr, uint8 oper) {
    osal_start_timerEx(zclApp_TaskID, APP_SAVE_ATTRS_EVT, 2000);
    return ZSuccess;
}

static void zclApp_SaveAttributesToNV(void) {
    uint8 writeStatus = osal_nv_write(NW_APP_CONFIG, 0, sizeof(application_config_t), &zclApp_Config);
}

static void zclApp_RestoreAttributesFromNV(void) {
    uint8 status = osal_nv_item_init(NW_APP_CONFIG, sizeof(application_config_t), NULL);
    if (status == NV_ITEM_UNINIT) {
        uint8 writeStatus = osal_nv_write(NW_APP_CONFIG, 0, sizeof(application_config_t), &zclApp_Config);
    }
    if (status == ZSUCCESS) {
        osal_nv_read(NW_APP_CONFIG, 0, sizeof(application_config_t), &zclApp_Config);
    }
}

static void zclApp_ReadSensors(void) {
    switch (currentSensorsReadingPhase++) {  
     case 0:
       if(pushBut){
         HAL_TURN_ON_LED1();
         zclBattery_Read();
         HAL_TURN_OFF_LED1();
       }
       POWER_ON_SENSORS();
       zclApp_ReadDS18B20();
      break;   
      
     case 1:
       zclApp_ReadLum();
       POWER_OFF_SENSORS();
      break;   
      
     default:
        currentSensorsReadingPhase = 0;
        osal_start_timerEx(zclApp_TaskID, APP_SEND_SENSORS_EVT, 200);
        break;
    }
    if (currentSensorsReadingPhase != 0) {
        osal_start_timerEx(zclApp_TaskID, APP_READ_SENSORS_EVT, 30);
    }
}      
           
static void zclApp_SendSensors(void) {
    switch (currentSensorsSendingPhase++) {  
    case 0:       
        zclApp_SendTemp();
        break;
        
    case 1:       
        zclApp_SendLum();
        break;
        
    default:
        currentSensorsSendingPhase = 0;
         if(pushBut){
           osal_start_timerEx(zclApp_TaskID, APP_SEND_BATT_EVT, 200);
        }
        break;
    }
    if (currentSensorsSendingPhase != 0) {
        osal_start_timerEx(zclApp_TaskID, APP_SEND_SENSORS_EVT, 40);
    }
}


static void zclApp_SendPeriodicSensors(void) {
    switch (currentSensorsSendingPeriodicPhase++) {  
    case 0:       
        zclApp_ReportTemp();
        break;
        
    case 1:       
        zclApp_ReportLumosity();
        break;
        
    default:
        currentSensorsSendingPeriodicPhase = 0;
        break;
    }
    if (currentSensorsSendingPeriodicPhase != 0) {
        osal_start_timerEx(zclApp_TaskID, APP_SEND_PER_SENSORS_EVT, 50);
    }
}



static void zclApp_SendMandatoryReport(void) {
    switch (currentSensorsSendingMRPhase++) {  
    case 0:  
        if(!pushBut){
        zclBattery_Read();
      }
        zclBattery_Report();
        break;
        
    case 1:       
        zclConfigReport();
        break;
        
    default:
        currentSensorsSendingMRPhase = 0;
        if(pushBut){
        pushBut = false;
        }
        break;
    }
    if (currentSensorsSendingMRPhase != 0) {
        osal_start_timerEx(zclApp_TaskID, APP_SEND_BATT_EVT, 50);
    }
}


static void zclApp_ReadDS18B20(void) {
  int16 tempT = (int16)(readTemperature()*100.0);
    if (tempT != 1) {
        zclApp_Temperature_Sensor_MeasuredValue = tempT;
    } else {
        //add an attribute for reading and transmitting sensor reading errors
    }
}

static void zclApp_ReadLum(void) {
    zclApp_IlluminanceSensor_MeasuredValueRawAdc = adcReadSampled(LUMOISITY_PIN, HAL_ADC_RESOLUTION_14, HAL_ADC_REF_AVDD, 5)*5;
    zclApp_IlluminanceSensor_MeasuredValue = zclApp_IlluminanceSensor_MeasuredValueRawAdc;
}

static void _delay_us(uint16 microSecs) {
    while (microSecs--) {
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
    }
}

void user_delay_ms(uint32_t period) { _delay_us(1000 * period); }

static void zclApp_Report(void) { 
  osal_start_timerEx(zclApp_TaskID, APP_READ_SENSORS_EVT, 100); 
}

static void zclApp_ReportPeriodic(void) { 
  osal_start_timerEx(zclApp_TaskID, APP_SEND_PER_SENSORS_EVT, 150); 
}

static void zclApp_ReportBatt(void) { 
  osal_start_timerEx(zclApp_TaskID, APP_SEND_BATT_EVT, 200); 
}

static void zclApp_SendTemp(void){
   if(!pushBut){
     if(zclApp_Config.ComparisonPreviousData == true){
    if (abs(zclApp_Temperature_Sensor_MeasuredValue - temp_old) >= temp_tr) {
        temp_old = zclApp_Temperature_Sensor_MeasuredValue;
        zclApp_ReportTemp();
    }
     }else{
        temp_old = zclApp_Temperature_Sensor_MeasuredValue;
        zclApp_ReportTemp();
     }
   }else{
     temp_old = zclApp_Temperature_Sensor_MeasuredValue;
     zclApp_ReportTemp();
   }
}

static void zclApp_SendLum(void){
  if(!pushBut){
     if(zclApp_Config.ComparisonPreviousData == true){
    if (abs(zclApp_IlluminanceSensor_MeasuredValue - lumi_old) >= lumi_tr) {
        lumi_old = zclApp_IlluminanceSensor_MeasuredValue;
        zclApp_ReportLumosity();
    }
     }else{
        lumi_old = zclApp_IlluminanceSensor_MeasuredValue;
        zclApp_ReportLumosity();
     }
   }else{
     lumi_old = zclApp_IlluminanceSensor_MeasuredValue;
     zclApp_ReportLumosity();
   }
}

static void zclApp_ReportTemp(void) { 
  if(bdbAttributes.bdbNodeIsOnANetwork == 1){
     const uint8 NUM_ATTRIBUTES = 1;
     zclReportCmd_t *pReportCmd;
     pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
     if (pReportCmd != NULL) {
       pReportCmd->numAttr = NUM_ATTRIBUTES;
       pReportCmd->attrList[0].attrID = ATTRID_MS_TEMPERATURE_MEASURED_VALUE;
       pReportCmd->attrList[0].dataType = ZCL_INT16;
       pReportCmd->attrList[0].attrData = (void *)(&zclApp_Temperature_Sensor_MeasuredValue);

       zcl_SendReportCmd(zclApp_FirstEP.EndPoint, &inderect_DstAddr, TEMP, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, TRUE, SeqNum++);
     }
    osal_mem_free(pReportCmd);
  }
}

static void zclApp_ReportLumosity(void) { 
     if(bdbAttributes.bdbNodeIsOnANetwork == 1){
     const uint8 NUM_ATTRIBUTES = 1;
     zclReportCmd_t *pReportCmd;
     pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
     if (pReportCmd != NULL) {
       pReportCmd->numAttr = NUM_ATTRIBUTES;
       pReportCmd->attrList[0].attrID = ATTRID_MS_ILLUMINANCE_MEASURED_VALUE;
       pReportCmd->attrList[0].dataType = ZCL_UINT16;
       pReportCmd->attrList[0].attrData = (void *)(&zclApp_IlluminanceSensor_MeasuredValue);

       zcl_SendReportCmd(zclApp_FirstEP.EndPoint, &inderect_DstAddr, ILLUMINANCE, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, TRUE, SeqNum++);
     }
    osal_mem_free(pReportCmd);
     }
}

static void zclConfigReport(void) {
if(bdbAttributes.bdbNodeIsOnANetwork == 1){
     const uint8 NUM_ATTRIBUTES = 5;
     zclReportCmd_t *pReportCmd;
     pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
     if (pReportCmd != NULL) {
       pReportCmd->numAttr = NUM_ATTRIBUTES;
       
       pReportCmd->attrList[0].attrID = ATTRID_ReportDelay;
       pReportCmd->attrList[0].dataType = ZCL_UINT16;
       pReportCmd->attrList[0].attrData = (void *)(&zclApp_Config.ReportDelay);
       
       pReportCmd->attrList[1].attrID = ATTRID_ComparisonPreviousData;
       pReportCmd->attrList[1].dataType = ZCL_DATATYPE_BOOLEAN;
       pReportCmd->attrList[1].attrData = (void *)(&zclApp_Config.ComparisonPreviousData);
       
       pReportCmd->attrList[2].attrID = ATTRID_LED_ONOFF;
       pReportCmd->attrList[2].dataType = ZCL_DATATYPE_BOOLEAN;
       pReportCmd->attrList[2].attrData = (void *)(&zclApp_Config.BackLight);
       
       pReportCmd->attrList[3].attrID = ATTRID_LED_F;
       pReportCmd->attrList[3].dataType = ZCL_UINT16;
       pReportCmd->attrList[3].attrData = (void *)(&zclApp_Config.LedFreq);
       
       pReportCmd->attrList[4].attrID = ATTRID_LED_D;
       pReportCmd->attrList[4].dataType = ZCL_UINT16;
       pReportCmd->attrList[4].attrData = (void *)(&zclApp_Config.LeDuration);

       zcl_SendReportCmd(zclApp_FirstEP.EndPoint, &inderect_DstAddr, BASIC, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, TRUE, SeqNum++);
     }
    osal_mem_free(pReportCmd);
}
}

void zclBattery_Report(void) {
if(bdbAttributes.bdbNodeIsOnANetwork == 1){
    const uint8 NUM_ATTRIBUTES = 3;
    zclReportCmd_t *pReportCmd;
    pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
    if (pReportCmd != NULL) {
        pReportCmd->numAttr = NUM_ATTRIBUTES;
        
        pReportCmd->attrList[0].attrID = ATTRID_POWER_CFG_BAT_ALARM_STATE;
        pReportCmd->attrList[0].dataType = ZCL_DATATYPE_BITMAP32;
        pReportCmd->attrList[0].attrData = (void *)(&zclApp_BatteryAlarm);

        pReportCmd->attrList[1].attrID = ATTRID_POWER_CFG_BATTERY_VOLTAGE;
        pReportCmd->attrList[1].dataType = ZCL_DATATYPE_UINT8;
        pReportCmd->attrList[1].attrData = (void *)(&zclBattery_Voltage);

        pReportCmd->attrList[2].attrID = ATTRID_POWER_CFG_BATTERY_PERCENTAGE_REMAINING;
        pReportCmd->attrList[2].dataType = ZCL_DATATYPE_UINT8;
        pReportCmd->attrList[2].attrData = (void *)(&zclBattery_PercentageRemainig);
        
        zcl_SendReportCmd(zclApp_FirstEP.EndPoint, &inderect_DstAddr, POWER_CFG, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, TRUE, SeqNum++);
    }
    osal_mem_free(pReportCmd);
}
}