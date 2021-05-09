#!/bin/bash
#
#
IP=$(curl -k -s "http://members.3322.org/dyndns/getip")
echo -e "当前外网IP: $IP \t\c"

### 这个使用 http://ip-api.com/json/8.8.8.8 替换，写法更简单。 doc: https://ip-api.com/docs/api:json
json=$(curl -k -s "http://ip-api.com/json/${IP}")
lat=$(echo $json|jq .lat)
lon=$(echo $json|jq .lon)
city=$(echo $json|jq .city)
echo -e "纬度:$lat\t经度:$lon\t城市:$city\t"
printf "%3.5f" $lat

json=$(curl -G -s "https://api.sunrise-sunset.org/json?lat=$lat&lng=$lon&date=today&formatted=0") 

sunrise=$(echo $json|jq .results.sunrise|sed 's/"//g')

if [ "$sunrise" ]; then
        lsunrise=$(TZ='Asia/Shanghai' date --date $sunrise +"%F %T")
        echo "今日 太阳升起时间: $lsunrise"
else
        echo "Can't get sunrise"
        exit 1
fi
