#!/bin/sh
# $Id$

CURDIR=`pwd`
OUTDIR=testlong_out

rm -fr $OUTDIR
mkdir $OUTDIR
cp .rootrc rootlogon.C $OUTDIR
cd $OUTDIR

FULLPATH="$CURDIR/$OUTDIR"
# Minimum number of events to have enough stat. for invariant mass fit
# 10000 is ok, 20000 is really fine
NEVENTS=10000
SEED=1234567

CDBDIRECTORY="$ALICE_ROOT/MUON/CDB/Default";
CDB="local://$CDBDIRECTORY";

if [ ! -d $CDBDIRECTORY"/MUON" ]; then

echo "Generating Condition Database in directory $CDBDIRECTORY. This may take a while, so please be patient..."

aliroot -b >& testGenerateCalibrations.out << EOF
.L $ALICE_ROOT/MUON/MUONCDB.C+
gRandom->SetSeed($SEED);
generateCalibrations("$CDB",true);
.q
EOF

else

echo "Condition Database found in directory $CDBDIRECTORY. Will use it if needed."

fi

echo "Running simulation  ..."

aliroot -b >& testSim.out << EOF  
// Uncoment following lines to run simulation with local residual mis-alignment
// (generated via MUONGenerateGeometryData.C macro)
// AliCDBManager* man = AliCDBManager::Instance();
// man->SetDefaultStorage("local://$ALICE_ROOT");
// man->SetSpecificStorage("MUON","local://$ALICE_ROOT/MUON/ResMisAlignCDB");
gRandom->SetSeed($SEED);
AliCDBManager::Instance()->SetDefaultStorage("$CDB");
AliSimulation MuonSim("$ALICE_ROOT/MUON/Config.C");
MuonSim.Run($NEVENTS); 
.q
EOF

echo "Running reconstruction  ..."

aliroot -b >& testReco.out << EOF 
gRandom->SetSeed($SEED);
AliCDBManager::Instance()->SetDefaultStorage("$CDB");
AliReconstruction MuonRec("galice.root"); 
MuonRec.SetRunTracking("");
MuonRec.SetRunVertexFinder(kFALSE);
MuonRec.SetRunLocalReconstruction("MUON");
MuonRec.SetFillESD("MUON");
MuonRec.Run(); 
.q
EOF

echo "Running Trigger efficiency  ..."

aliroot -b >& testTriggerResults.out << EOF
.L $ALICE_ROOT/MUON/MUONTriggerEfficiency.C+
MUONTriggerEfficiency();
.q
EOF

echo "Running efficiency  ..."

aliroot -b >& testEfficiency.out << EOF 
.L $ALICE_ROOT/MUON/MUONefficiency.C+
// no argument assumes Upsilon but MUONefficiency(443) handles Jpsi
MUONefficiency();
.q
EOF


aliroot -b >& testResults.out << EOF 
// no argument assumes Upsilon but MUONplotefficiency(443) handles Jpsi
.x $ALICE_ROOT/MUON/MUONplotefficiency.C
.q
EOF

more  testSim.out | grep 'RunSimulation: Execution time:'  > testTime.out
more  testSim.out | grep 'RunSDigitization: Execution time:'  >> testTime.out
more  testSim.out | grep 'RunDigitization: Execution time:'  >> testTime.out 

more  testReco.out | grep 'RunLocalReconstruction: Execution time for MUON'  >> testTime.out
more  testReco.out | grep 'Execution time for filling ESD ' >> testTime.out

rm gphysi.dat
rm *.root
rm testSim.out
rm testReco.out
rm *.eps

echo "Finished"  
echo "... see results in testlong_out"

cd $CURDIR
