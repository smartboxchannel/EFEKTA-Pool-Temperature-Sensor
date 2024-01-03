#include "ds18b20.h"
#include "OnBoard.h"

#define DS18B20_SKIP_ROM                                                        0xCC
#define DS18B20_CONVERT_T                                                       0x44
#define DS18B20_READ_SCRATCHPAD                                                 0xBE
#define DS18B20_WRITE_SCRATCHPAD                                                0x4E

#define DS18B20_TEMP_9_BIT                                                      0x1F
#define DS18B20_TEMP_10_BIT                                                     0x3F
#define DS18B20_TEMP_11_BIT                                                     0x5F
#define DS18B20_TEMP_12_BIT                                                     0x7F

#ifndef DS18B20_RESOLUTION
#define DS18B20_RESOLUTION                                                      DS18B20_TEMP_12_BIT
#endif

#ifndef DS18B20_RETRY_COUNT
#define DS18B20_RETRY_COUNT                                                     10
#endif

#define MAX_CONVERSION_TIME                                                     (750 * 1.2)

#define DS18B20_RETRY_DELAY                                                     ((uint16) (MAX_CONVERSION_TIME / DS18B20_RETRY_COUNT))

static void _delay_us(uint16);
static void _delay_ms(uint16);
static void ds18b20_send(uint8);
static uint8 ds18b20_read(void);
static void ds18b20_send_byte(int8);
static uint8 ds18b20_read_byte(void);
static uint8 ds18b20_Reset(void);
static void ds18b20_GroudPins(void);
static void ds18b20_setResolution(uint8 resolution);

static void _delay_us(uint16 microSecs) {
    while (microSecs--)
    {
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

static void _delay_ms(uint16 milliSecs) {
    while (milliSecs--) {
        _delay_us(1000);
    }
}

static void ds18b20_send(uint8 bit) {
    TSENS_SBIT = 1;
    TSENS_DIR |= TSENS_BV; // output
    TSENS_SBIT = 0;
    if (bit != 0)
        _delay_us(8);
    else
        _delay_us(80);
    TSENS_SBIT = 1;
    if (bit != 0)
        _delay_us(80);
    else
        _delay_us(2);
}

static uint8 ds18b20_read(void) {
    TSENS_SBIT = 1;
    TSENS_DIR |= TSENS_BV; // output
    TSENS_SBIT = 0;
    _delay_us(2);
    TSENS_DIR &= ~TSENS_BV; // input
    _delay_us(5);
    uint8 i = TSENS_SBIT;
    _delay_us(60);
    return i;
}

static void ds18b20_send_byte(int8 data) {
    uint8 i, x;
    for (i = 0; i < 8; i++) {
        x = data >> i;
        x &= 0x01;
        ds18b20_send(x);
    }
}

static uint8 ds18b20_read_byte(void) {
    uint8 i;
    uint8 data = 0;
    for (i = 0; i < 8; i++) {
        if (ds18b20_read())
            data |= 0x01 << i;
    }
    return (data);
}

static uint8 ds18b20_Reset(void) {
    TSENS_DIR |= TSENS_BV; // output
    TSENS_SBIT = 0;
    _delay_us(500);
    TSENS_DIR &= ~TSENS_BV; // input
    _delay_us(70);
    uint8 i = TSENS_SBIT;
    _delay_us(200);
    TSENS_SBIT = 1;
    TSENS_DIR |= TSENS_BV; // output
    _delay_us(500);
    return i;
}

static void ds18b20_GroudPins(void) {
    TSENS_DIR &= ~TSENS_BV; // input
}

static void ds18b20_setResolution(uint8 resolution) {
    ds18b20_Reset();
    ds18b20_send_byte(DS18B20_SKIP_ROM);
    ds18b20_send_byte(DS18B20_WRITE_SCRATCHPAD);

    ds18b20_send_byte(0);
    ds18b20_send_byte(100);
    ds18b20_send_byte(resolution);
    ds18b20_Reset();
}

float readTemperature(void) {

    char temp1, temp2;
    uint8 retry_count = DS18B20_RETRY_COUNT;
    ds18b20_setResolution(DS18B20_RESOLUTION);
    ds18b20_Reset();

    ds18b20_send_byte(DS18B20_SKIP_ROM);
    ds18b20_send_byte(DS18B20_CONVERT_T);

    while (retry_count) {
        _delay_ms(DS18B20_RETRY_DELAY);
        ds18b20_Reset();
        ds18b20_send_byte(DS18B20_SKIP_ROM);
        ds18b20_send_byte(DS18B20_READ_SCRATCHPAD);
        temp1 = ds18b20_read_byte();
        temp2 = ds18b20_read_byte();
        ds18b20_Reset();

        if (temp1 == 0xff && temp2 == 0xff) {
            ds18b20_GroudPins();
            return 1;
        }
        if (temp1 == 0x50 && temp2 == 0x05) {
            retry_count--;
            continue;
        }

        ds18b20_GroudPins();
        float temp=0;
        temp=(float)(temp1+(temp2*256))/16;
        return temp;
    }
    ds18b20_GroudPins();
    return 1;
}