#!/bin/bash

temp=$(sensors|grep "^temp11"|awk -F':' '{ print $2 }')

echo $temp|sed s'/..$//'
