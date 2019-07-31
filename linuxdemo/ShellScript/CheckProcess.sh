#!/bin/sh

#monitor C600, restart it when the progress is down.

app="C600"
#echo $app

while true
do
	count=`ps | grep C600 | grep -v grep | wc -l`
	#echo $count
	if [ $count -lt 1 ]; then
		echo "C600 has dropped!"
		echo "Now, restart."
		cd /toncen
		/toncen/C600 -qws -fn SIMHEI -nomouse &
		#/toncen/C600 -qws -fn SIMHEI -nomouse -font unifont &
	fi
	sleep 1
done
