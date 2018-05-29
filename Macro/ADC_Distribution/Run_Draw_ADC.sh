#!/bin/bash
path=`pwd`

root -l -b<<EOF
gSystem->Load("libGpad.so");
gSystem->Load("/home/isyoon/GEM_Performance/lib/libGEM_Performance.so");
.x Run_Draw_ADC.C()
EOF
