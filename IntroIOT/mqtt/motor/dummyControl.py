from bluepy.btle import UUID, Peripheral, AssignedNumbers
import time
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import struct

Broker = "localhost"
pub_topic = "motor/action"

# when connecting to mqtt do this;

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

# when receiving a mqtt message do this;
def on_message(client, userdata, msg):
    message = str(msg.payload)
    print(msg.topic+" "+message)

def on_publish(mosq, obj, mid):
    print("mid: " + str(mid))

def main():

    #Set up MQTT
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(Broker, 1883, 60)
    client.loop_start()
    count = 0
    while True:
        if count > 0 and count <= 1000:
            client.publish(pub_topic, "BF")
        elif count > 1000 and count <= 2000:
            client.publish(pub_topic, "X")
        count += 1;
if __name__ == "__main__":
    main()
