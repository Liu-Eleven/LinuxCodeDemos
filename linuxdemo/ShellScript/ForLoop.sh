#!/bin/sh

data="one two three four five six"


for item in $data;
do
echo Item: $item;
done

echo "-----------------------------------------"

for i in {a..z};
do
echo I: $i;
done


echo "-----------------------------------------"

for((i=0;i<10;i++))
{
	echo I: $i;
}