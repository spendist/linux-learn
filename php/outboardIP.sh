#!/bin/bash

outboardIP=$(dig A +short gateway.spender.men)

echo "outboard IP = $outboardIP"
echo -n "Change /etc/used4.conf, "
sed -r -i -e 's/([0-9]{1,3}\.){3}[0-9]{1,3}\b'/"$outboardIP"/g /etc/used4.conf
if [ $? -eq 0 ]; then
	echo "Suceess!"
else
	echo "Sorry!"
fi
