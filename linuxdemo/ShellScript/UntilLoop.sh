#!/bin/sh

x=0

until [ $x -eq 9 ];
do
	let x++
	echo $x;
	sleep 1
done
