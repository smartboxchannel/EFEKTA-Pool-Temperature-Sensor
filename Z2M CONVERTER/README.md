## External converter for Z2M

To add an external converter you need to perform "2 steps". The first is to upload the converter file itself to your server and the second is to enter information into the configuration about the external converter in zigbee2mqtt. 

### Adding a file to the server.

/UPD
#### 0 option.
If you use Home Assistant to upload files to the server, you can use the File Editor addon (the easiest option).
/

#### 1 option.

There are several options for uploading a device converter to the server. The first is uploading via an ftp client. This can be any client, for example filezilla.

To connect to the server, you must enter the IP address, login and password in the appropriate fields of the program and click the “quick connection” button.

![Z2M CONVERTER EFEKTA Pool Temperature Sensor](https://raw.githubusercontent.com/smartboxchannel/EFEKTA-Pool-Temperature-Sensor/main/IMAGES/f2d8d708a1152f5e37130.png)

If you are not using HassOs, then the path to the desired folder will be /opt/zigbee2mqtt/data, if you are using HassOs, then the path to the desired folder will be /config/zigbee2mqtt

Having gone to the desired folder, simply drag the converter there from the left program window to the right one.

#### option 2

If you use Home Assistant, then you can install the Samba plugin, then you will have a network folder created in you can simply move the desired file through the standard Windows Explorer. Instructions for installing Samba are here http://psenyukov.ru/%D0%B4%D0%B5%D0%BB%D0%B0%D0%B5%D0%BC-%D1%81%D0%B5%D1%82%D0%B5%D0%B2%D1%8B%D0%B5-%D0%BF%D0%B0%D0%BF%D0%BA%D0%B8-%D0%BD%D0%B0-%D1%81%D0%B5%D1%80%D0%B2%D0%B5%D1%80%D0%B5-hassos-home-assistant-%D1%81/.


