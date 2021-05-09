#!/bin/bash
#
# https://api.sunrise-sunset.org/json?lat=36.7201600&lng=-4.4203400&formatted=0
# Document: https://api.sunrise-sunset.org

# 获取成都本地日出时间
#curl -G -s "https://api.sunrise-sunset.org/json?lat=30.67&lng=104.07&date=today&formatted=0" | jq .
#curl -G -s "http://restapi.amap.com/v3/geocode/geo?key=389880a06e3f893ea46036f030c94700&s=rsv3&city=35&address=陕西省合阳县坊镇北伏蒙"|jq .
#curl -G -s "https://api.sunrise-sunset.org/json?lat=35.209289&lng=110.331137&date=today&formatted=0" | jq .

Address="成都市锦城大道108号"

#
# Get location 
location=$(curl -G -s "http://restapi.amap.com/v3/geocode/geo?key=389880a06e3f893ea46036f030c94700&s=rsv3&city=35&address=${Address}"|jq '.geocodes' |grep location|awk -F':' '{ print $2 }')
if [ "$location" ]; then
	echo $location
	longitude=$(echo $location|sed 's/"//g' |awk -F',' '{ print $1 }')
	latitude=$(echo $location|sed 's/"//g' |awk -F',' '{ print $2 }')
else
	echo "Can't get location from Address!"
	exit 1
fi

#
# Get sunrise
	##echo $latitude
	##echo $longitude
sunInfo=$(curl -G -s "https://api.sunrise-sunset.org/json?lat=$latitude&lng=$longitude&date=today&formatted=0" | jq .results)
if [ "$sunInfo" ]; then
	sunrise=$(echo $sunInfo| jq .sunrise|sed 's/"//g')
	sunset=$(echo $sunInfo| jq .sunset|sed 's/"//g')
	# echo $sunrise 
	lsunrise=$(TZ='Asia/Shanghai' date --date $sunrise +"%F %T")

	echo "$Address 今日 太阳升起时间: $lsunrise"

else
	echo "Can't get sunrise"
	exit 1
fi
	
