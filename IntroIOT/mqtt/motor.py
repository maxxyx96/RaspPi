import RPi.GPIO as GPIO
from time import sleep
import paho.mqtt.client as mqtt

Motor1A = 13
Motor1B = 11
Motor2A = 16
Motor2B = 18

Broker = "localhost"
sub_topic = "motor/action"    # receive messages on this topic

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe(sub_topic)

# when receiving a mqtt message do this;

def on_message(client, userdata, msg):
    cmd = str(msg.payload)
    print(msg.topic+"" + cmd)
    #Move motors according to values on message
    if cmd == "LF":
        GPIO.output(Motor1A, GPIO.HIGH)
        GPIO.output(Motor1B, GPIO.LOW)
    if cmd == "LB":
        GPIO.output(Motor1A, GPIO.LOW)
        GPIO.output(Motor1B, GPIO.HIGH)
    if cmd == "RF":
        GPIO.output(Motor2A, GPIO.LOW)
        GPIO.output(Motor2B, GPIO.HIGH)
    if cmd == "RB":
        GPIO.output(Motor2A, GPIO.HIGH)
        GPIO.output(Motor2B, GPIO.LOW)
    if cmd == "BF":
        GPIO.output(Motor1A, GPIO.HIGH)
        GPIO.output(Motor1B, GPIO.LOW)
        GPIO.output(Motor2A, GPIO.LOW)
        GPIO.output(Motor2B, GPIO.HIGH)
    if cmd == "BB":
        GPIO.output(Motor1A, GPIO.LOW)
        GPIO.output(Motor1B, GPIO.HIGH)
        GPIO.output(Motor2A, GPIO.HIGH)
        GPIO.output(Motor2B, GPIO.LOW)
    if cmd == "X":
        GPIO.output(Motor1A, GPIO.LOW)
        GPIO.output(Motor1B, GPIO.LOW)
        GPIO.output(Motor2A, GPIO.LOW)
        GPIO.output(Motor2B, GPIO.LOW)

def main():

    #Set up motors
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(Motor1A, GPIO.OUT)
    GPIO.setup(Motor1B, GPIO.OUT)
    GPIO.setup(Motor2A, GPIO.OUT)
    GPIO.setup(Motor2B, GPIO.OUT)

    #Set up MQTT
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(Broker, 1883, 60)
    client.loop_forever()

if __name__ == "__main__":
    main()
