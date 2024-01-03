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
