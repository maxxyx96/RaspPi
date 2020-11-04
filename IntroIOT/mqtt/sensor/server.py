import time
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish

Broker = "localhost"

sub_topic = "sensor/pub"    # receive messages on this topic
pub_topic = "sensor/sub"               # send messages to this topic


# mqtt section

# when connecting to mqtt do this;

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe(sub_topic)

# when receiving a mqtt message do this;

def on_message(client, userdata, msg):
    message = str(msg.payload)
    print(msg.topic+" "+message)

# to send a message

def publish_mqtt(sensor_data):
    mqttc = mqtt.Client()
    mqttc.connect(Broker, 1883)

def on_publish(mosq, obj, mid):
    print("mid: " + str(mid))


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(Broker, 1883, 60)
client.loop_forever()
