#!/bin/sh
# $Id$

CURDIR=`pwd`
OUTDIR=testlong_out

rm -fr $OUTDIR
mkdir $OUTDIR
cp .rootrc $OUTDIR
cd $OUTDIR

echo "Running simulation  ..."

aliroot -b >& testSim.out << EOF  
AliSimulation MuonSim
MuonSim.SetConfigFile("$ALICE_ROOT/MUON/Config.C")
// Minimum number of events to have enough stat. for invariant mass fit
// 10000 is ok, 20000 is really fine
MuonSim.Run(10000) 
.q
EOF

echo "Running reconstruction  ..."

aliroot -b >& testReco.out << EOF 
TPluginManager* pluginManager = gROOT->GetPluginManager();
pluginManager->AddHandler("AliReconstructor", "MUON","AliMUONReconstructor", "MUON","AliMUONReconstructor()")
AliReconstruction MuonRec("galice.root") 
MuonRec.SetRunTracking("")
MuonRec.SetRunVertexFinder(kFALSE)
MuonRec.SetRunLocalReconstruction("MUON")
MuonRec.SetFillESD("MUON")
MuonRec.Run() 
.q
EOF

echo "Running efficiency  ..."

aliroot -b >& testEfficiency.out << EOF 
.x ../MUONefficiency.C
.q
EOF

aliroot -b >& testResults.out << EOF 
.x ../MUONplotefficiency.C
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
