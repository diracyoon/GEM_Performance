#!/bin/bash

root -l -b<<EOF
gSystem->Load("/home/isyoon/GEM_Performance/Build/libGEM_Performance.so");
.x Run_Discharge_Prob.C()
EOF
