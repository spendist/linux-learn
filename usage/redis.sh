#!/bin/bash

PATH=$PATH:/opt/vc/bin
Host=`hostname -s`
T=`/opt/vc/bin/vcgencmd measure_temp`
echo "$Host $T"
