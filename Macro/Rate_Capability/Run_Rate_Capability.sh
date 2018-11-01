#!/bin/bash
path=`pwd`

root -l -b<<EOF
gSystem->Load("/home/isyoon/GEM_Performance/Build/libGEM_Performance.so");
.x Run_Rate_Capability.C("${path}")
EOF
