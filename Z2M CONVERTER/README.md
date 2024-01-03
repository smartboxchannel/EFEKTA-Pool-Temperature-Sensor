## External converter for Z2M

Для добавления внешнего конвертера необходимо выполнить «2 шага». Первый — загрузить сам файл конвертера на ваш сервер, а второй — внести информацию в файл конфигурации о внешнем конвертере в zigbee2mqtt.

To add an external converter you need to perform "2 steps". The first is to upload the converter file itself to your server and the second is to enter information into the configuration about the external converter in zigbee2mqtt. 

### Adding a file to the server.

/UPD
#### 0 option.
Если вы используете Home Assistant то загрузить файлы на сервер можно воспользовавшись аддоном File editor (самый простой вариант).

#### 0 option.
If you use Home Assistant to upload files to the server, you can use the File Editor addon (the easiest option).
/

#### 1 option.

Для загрузки конвертера для устройсва на сервер УД есть несколько вариантов. Первое это загрузка через ftp клиент. Это может быть любой клиент, например filezilla.

Для подключения к серверу необходимо ввести ip адрес, логин и пароль в соответствуюшие поля в программе и нажать на кнопку - "быстрое подключение"

There are several options for uploading a device converter to the server. The first is uploading via an ftp client. This can be any client, for example filezilla.

To connect to the server, you must enter the IP address, login and password in the appropriate fields of the program and click the “quick connection” button.

![Z2M CONVERTER EFEKTA Pool Temperature Sensor](https://raw.githubusercontent.com/smartboxchannel/EFEKTA-Pool-Temperature-Sensor/main/IMAGES/f2d8d708a1152f5e37130.png)

Если вы не используете HassOs то путь к нужной папке будет /opt/zigbee2mqtt/data, если вы используете HassOs то путь к нужной папке будет /config/zigbee2mqtt

Перейдя в нужную папку просто перетащите туда конвертер из левого окна программы в правое.

If you are not using HassOs, then the path to the desired folder will be /opt/zigbee2mqtt/data, if you are using HassOs, then the path to the desired folder will be /config/zigbee2mqtt

Having gone to the desired folder, simply drag the converter there from the left program window to the right one.

#### option 2

Если вы используете Home Assistant то вы можете установить плагин Самба, тогда у вас появится сетевая папка в которую вы можете просто перетащить нужный файл через стандартный проводник в Windows. Инструкция по установке Самба находится здесь http://psenyukov.ru/%D0%B4%D0%B5%D0%BB%D0%B0%D0%B5%D0%BC-%D1%81%D0%B5%D1%82%D0%B5%D0%B2%D1%8B%D0%B5-%D0%BF%D0%B0%D0%BF%D0%BA%D0%B8-%D0%BD%D0%B0-%D1%81%D0%B5%D1%80%D0%B2%D0%B5%D1%80%D0%B5-hassos-home-assistant-%D1%81/.

If you use Home Assistant, then you can install the Samba plugin, then you will have a network folder created in you can simply move the desired file through the standard Windows Explorer. Instructions for installing Samba are here http://psenyukov.ru/%D0%B4%D0%B5%D0%BB%D0%B0%D0%B5%D0%BC-%D1%81%D0%B5%D1%82%D0%B5%D0%B2%D1%8B%D0%B5-%D0%BF%D0%B0%D0%BF%D0%BA%D0%B8-%D0%BD%D0%B0-%D1%81%D0%B5%D1%80%D0%B2%D0%B5%D1%80%D0%B5-hassos-home-assistant-%D1%81/.

### Adding information about the converter.

После загрузки файла (это самая трудная часть обычно :)), нужно прописать данные о внешнем конвертере в конфигурацию zigbee2mqtt. Для этого вам нужно перейти в вэбинтерфейс zigbee2mqtt и перейти в раздел - 'настройки".

далее найти подраздел - внешние конвертеры и перейти в него.

After downloading the file (this is usually the hardest part :)), you need to register the data in the external converter in the zigbee2mqtt configuration. To do this, you need to go to the zigbee2mqtt interface and go to the “Settings” section.

Next, find the section - External converters and go to it.

![Z2M CONVERTER EFEKTA Pool Temperature Sensor](https://raw.githubusercontent.com/smartboxchannel/EFEKTA-Pool-Temperature-Sensor/main/IMAGES/50e441c9f0153789f9023.png)

Далеее нужно спустится вниз страницы и нажать на кнопку "+", появися новое окно для ввода информации, туда нужно добавить название вашего внешнего конвертера с расширением. например Pool_Temperature_smart_sensor.js. После этого нажать на кнопку 'Submit" и далее на кнопку - "Перезагрузить" вверху страницы.

Next, you need to scroll down the page and click the “+” button, a new window will appear for entering information, where you need to add the name of your external converter with extensions. for example Pool_Temperature_smart_sensor.js. After that, click on the “Submit” button and then on the “Reload” button at the top of the page.

![Z2M CONVERTER EFEKTA Pool Temperature Sensor](https://raw.githubusercontent.com/smartboxchannel/EFEKTA-Pool-Temperature-Sensor/main/IMAGES/b3489a93950a8c93bf425.png)



