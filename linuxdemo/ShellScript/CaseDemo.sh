#!/bin/sh

echo "Is it morning? Please answer yes or no"
read timeofday

case "$timeofday" in
	yes)	echo "Good Morning";;
	no)	echo "Good Afternoon";;
	y)	echo "Good morning";;
	n)	echo "Good Afternoon";;
	*)	echo "Sorry, answer not recognized";;
esac

read timeofday_ex

case "$timeofday_ex" in
	yes | y | Yes | YES)	echo "Good Morning";;
	no | n | No | NO)	echo "Good Afternoon";;
	*)	echo "Sorry, answer not recognized";;
esac

exit 0

