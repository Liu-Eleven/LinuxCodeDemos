#!/bin/bash

cd qtopia-free-2.2.0

echo yes | ./configure -qte '-embedded -no-xft -qconfig qpe -depths 16,32 -system-jpeg -gif' -qpe '-edition pda -displaysize 272x480' -qt2 '-no-opengl -no-xft' -dqt '-no-xft -thread' 2>&1 | tee ./qtopiaconfig.log

make

make install

