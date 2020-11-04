Contents of this folder:

1) RPI.py - Basic GPIO for motors. 
  - Driven with L293D Motor Driver

2) cc2650_blupy.py - Bluetooth LE connection from sensortag to Raspberry Pi / Ubuntu / Linux
  - Remember to change the macAddress to your sensortag's MAC Address. 
  - If the device can't be found, try to plug and unplug the sensortag from the power source or follow this steps to "pair" your device:
    
    `bluetoothctl`
    
    `scan on`
  - Search for "XX:XX:XX:XX:XX:XX CC2650 Sensortag" and copy the MAC address
    
    `trust <sensortag's MAC address>`
  - Try running the code again. If it still doesn't work, unplug and plug your device from the power source
  
  
  MQTT Folder
  - Files that attempt to transport data through the MQTT protocol. 
