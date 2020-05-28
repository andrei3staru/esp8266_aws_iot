# Connecting ESP8266MOD to AWS IOT

Components needed:
1. ESP8266MOD device - Used for testing: https://smile.amazon.com/gp/product/B07HF44GBT/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1
2. AWS Account
3. Arduino IDE to upload the sketch - https://www.arduino.cc/en/Main/Software
4. SPIFF (Serial Peripheral Interface Flash File System) Arduino IDE package.

## Cost

Free for up to:
* 2,250,000 minutes of connection
* 500,000 messages
* 225,000 Registry or Device Shadow operations
* 250,000 rules triggered and 250,000 actions executed

More details: https://aws.amazon.com/iot-core/pricing/

## Steps

1. Configure Arduino IDE to connect to ESP8266: [Tutorial](https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/)
2. Add SPIFF to Arduino IDE: [Tutorial](https://github.com/esp8266/arduino-esp8266fs-plugin)
3. Follow [AWS guide](https://docs.aws.amazon.com/iot/latest/developerguide/iot-gs.html) to configure a device.
4. When you create the device certificate remember to download the 3 red-circled files. Save those files in the "data" folder. Note the name of the files.
5. Modify the sketch.ino file with your information (look for TODO comment):
  * Add wireless (ssid and password) information
  * AWS endpoint (MQTT broker ID)
  * Topics to publish and subscribe to.
  * Change the Certificate, Private certificate and CA file names (those noted in step 4).
6. Upload the files in the data folder to the ESP8266 file system.
7. Upload the sketch to ESP8266 using Arduino.
8. Use the Test menu in AWS IOT console to subscribe and publish messages.