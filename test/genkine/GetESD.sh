#!/bin/sh
rm -rf */*.root */*.log */*.dat
cd ./gen
aliroot -b -q rungen.C\(5\) 2>&1 | tee gen.log
chmod a-w *.root
cd ../sim
aliroot -b -q sim.C\(5\) 2>&1 | tee sim.log
aliroot -b -q rec.C      2>&1 | tee rec.log


