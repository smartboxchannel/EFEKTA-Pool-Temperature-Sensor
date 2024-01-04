# EFEKTA-Pool-Temperature-Sensor

Проект zigbee датчика температуры для бассейна. Проект разработан на soc сс2530 (радиомомдуль E18-MS1PA2-IPX, E18-MS1PA2-PCB, E18-MS1-PCB)

Телеграм чат DIY Devices - https://t.me/diy_devices

Продажа DIY Устройств - https://t.me/diydevmart

## Установка среды разработки IAR

https://github.com/sigma7i/zigbee-wiki/wiki/zigbee-firmware-install

https://zigdevwiki.github.io/Begin/IAR_install/

### Программа для разработки печатных плат - DipTrace

https://diptrace.com/rus/download/download-diptrace/

### Корпус спроектирован в SOLIDWORKS

### Прошивка устройства

Вам понадобится программа SmartRF Flash Programmer версии 1. Так же вам понадобится CC Debugger или Smart RF04E(рекомендуется из-за простоты подключения).

#### Процесс прошивки с помощью SmartRF Flash Programmer v1

Подключите 5 контактов на плате на соответствующие подписанные контакты дебагера. Подключаете дебагер в usb порту компьютера, открываете программу SmartRF Flash Programmer v1. Правее поля в пункте Flash Image нажимаете на иконку «обзор» и выбираете файл прошивки скаченный ранее. В пункте Actions выбираете Erase и ниже нажимаете на кнопку Perform actions. После того как вы очистили чип, выбираете Erase and program том же в пункте Actions и снова нажимаете кнопку Perform actions.

![Flash Programm EFEKTA Pool Temperature Sensor](https://raw.githubusercontent.com/smartboxchannel/EFEKTA-Pool-Temperature-Sensor/main/IMAGES/200.jpg)
