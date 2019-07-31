#!/bin/sh

# $# args number, not include command 

cmd_gs=./_install/bin/gs
cmd_hpijs=./_install/bin/hpijs
lib_viewjpeg=./_install/share/ghostscript/9.04/lib/viewjpeg.ps

if [ $# -lt 3 ]; then
	printf "Usage: %s -dDEVICEWIDTHPOINTS=? -dDEVICEHEIGHTPOINTS=? FileNameJpg\n" $0  
	exit 1
fi

printf "arg1: %s\n" $1
printf "arg2: %s\n" $2
printf "Jpg file name: %s\n" $3


printf "cmd_gs: %s\n" $cmd_gs
printf "cmd_hpijs: %s\n" $cmd_hpijs
printf "lib_viewjpeg: %s\n" $lib_viewjpeg


$cmd_gs -h > help.txt

$cmd_gs $1 $2 -sDEVICE=pdfwrite -o $3.pdf $lib_viewjpeg -c "($3.jpg) viewJPEG"


$cmd_gs -sDEVICE=ijs -sIjsServer=$cmd_hpijs -dIjsUseOutputFD -sDeviceManufacturer="HEWLETT-PACKARD" -sDeviceModel="deskjet 5550" -dNOPAUSE -dSAFER -sOutputFile="/dev/usb/lp0" test.pdf -c quit




