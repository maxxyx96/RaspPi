## motor.py
- Listens to topic "motor/action" for any instructions sent, and sets the GPIO pins respectively.
- If the directions are wrong, just modify the input pins of Motor1A/1B/2A/2B. 
- LF/LB: Left wheel forward / Left wheel backwards
- RF/RB: Right wheel forward / Right wheel backwards
- BF/BB: Both wheels move forwards / Both wheels move backwards
- X: Stop movement from both wheels. 


## dummyControl
- Code used to test working MQTT connection.
