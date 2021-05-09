#!/bin/bash
#
:
# 获取风扇转速，并报告给 domoticz 服务器，协议： mosqtti
#

IDX=29

speed=$(sensors|grep fan1|awk '{ print $2 }')

# Post message to mqtt server, 
echo "speed=$speed"

# { "idx":29,"svalue":"$speed" }

MSG="{ \"idx\":${IDX},\"svalue\":\"${speed}\" }"

mosquitto_pub -h 192.168.0.6 -p 11883 -u spender -P mqttPassw0rd -t domoticz/in -m "$MSG"


