#!/bin/csh
if ($#argv < 1) then
 echo "usage simrun.sh RunNumber"
 exit()
endif
if ( ! -e $WORK ) then 
 setenv $WORK ./
endif
cd $WORK
if ( ! -e QATest ) then 
 mkdir QATest
endif    
cd QATest
rm -Rf DB* *.root *.C *.log data/*
ln -si $ALICE_ROOT/test/QA/Config.C Config.C
ln -si $ALICE_ROOT/test/QA/sim.C sim.C
ln -si $ALICE_ROOT/test/QA/simqa.C simqa.C
ln -si $ALICE_ROOT/test/QA/rec.C rec.C
ln -si $ALICE_ROOT/test/QA/recqa.C recqa.C
ln -si $ALICE_ROOT/test/QA/DB.tgz DB.tgz
root -b -q $ALICE_ROOT/test/QA/simrun.C --run $1
cd $WORK/QATest/data
#ln -s ../geometry.root
ln -s ../raw.root
ln -s ../DB 
ln -si $ALICE_ROOT/test/QA/recraw.C recraw.C
aliroot -b -q recraw.C  > recraw.log 
cp  $ALICE_ROOT/test/QA/rawqa.C .
alienaliroot -b > rawqa.log << EOF
.x  $ALICE_ROOT/test/QA/rootlogon.C
.L rawqa.C++
rawqa(21950, 10)
EOF
rm -f rawqa.C
exit
