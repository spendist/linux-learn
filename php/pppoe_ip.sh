#!/bin/bash

ssh admin@192.168.0.254  "ip addr show ppp0|grep 'inet ' "|awk '{ print $2 }'

