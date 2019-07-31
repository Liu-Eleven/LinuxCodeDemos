#!/bin/sh

source ~/.bashrc
echo "----------------------------------"
echo "please enter your choise:"
echo "(0) mongotest"
echo "(1) mongomaster"
echo "(2) mongozwt179"
echo "(3) mongobjsc115"
echo "(4) mongobjsc102"
echo "(9) Exit Menu"
echo "----------------------------------"
read input
 
case $input in
    0)
    echo mongotest
    sleep 1
    mongotest;;
    1)
    echo mongomaster
    sleep 1
    mongomaster;;
    2)
    echo mongozwt179
    sleep 1
    mongozwt179;;
    3)
    echo mongobjsc115
    sleep 1
    mongobjsc115;;
    4)
    echo mongobjsc102
    sleep 1
    mongobjsc102;;
    9)
    exit;;
esac
