#!/bin/sh

var=10;
echo var=$var

if [ $var -eq 10 ];
then
	echo $var eq 10
fi

echo "-----------------------------------------"

if [ $var -gt 1 ];
then
	echo $var gt 1
fi

echo "-----------------------------------------"

if [ $var -lt 20 ];
then
	echo $var lt 20
fi

echo "-----------------------------------------"

if [ $var -ge 10 ];
then
	echo $var ge 10
fi

echo "-----------------------------------------"

if [ $var -le 20 ];
then
	echo $var le 20
fi

echo "-----------------------------------------"


