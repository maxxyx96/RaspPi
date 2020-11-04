## Server.py
- Listens for data sent by sensortag.py in the topic "sensor/pub" and echoes it out whenever it receives a message. 

## Sensortag.py 
- Same as cc2650.bluepy.py, just that messages received will be published to topic "sensor/pub" 
-Remember to change the macAddress to your sensortag's MAC Address.
-If the device can't be found, try to plug and unplug the sensortag from the power source or follow this steps to "pair" your device:

`bluetoothctl`

`scan on`

- Search for "XX:XX:XX:XX:XX:XX CC2650 Sensortag" and copy the MAC address

`trust <sensortag's MAC address>`

- Try running the code again. If it still doesn't work, unplug and plug your device from the power source
