#!/bin/bash

root -l -b<<EOF
gSystem->Load("/home/isyoon/GEM_Performance/lib/libGEM_Performance.so");
.x Run_Gain.C()
EOF
