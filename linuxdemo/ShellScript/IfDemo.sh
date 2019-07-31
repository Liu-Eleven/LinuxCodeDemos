#!/bin/sh

var=2;

if [ $var -eq 2 ];
then
	echo var=$var
fi

echo "-----------------------------------------"

var=3;
if [ $var -eq 2 ];
then
	echo var=$var
elif [ $var -eq 3 ];
then
	echo var=$var	
else
	echo var=$var		
fi

echo "-----------------------------------------"
