#! /bin/tcsh -f
#
# Prepare the environment
#
mkdir $HOME/TestSuite ; cd $HOME/TestSuite
cp $ALICE_ROOT/PHOS/grunTestSuite.C . 
cp $ALICE_ROOT/PHOS/ConfigTestSuite.C . 
#
#begin data
set MINHITS=0
set MAXHITS=132
#end data
#
echo PHOS Test Suite run on `date` > TestSuite.log
echo "=======================================================================" >> TestSuite.log
echo AliROOT  version `which aliroot` >> TestSuite.log
echo Root     version `which root` >> TestSuite.log
echo "=======================================================================" >> TestSuite.log
#
# SIMULATION
#
echo "    **** SIMULATION **** " >> TestSuite.log
aliroot -b -q grunTestSuite.C\(1\) >>& TestSuite.log 
echo '{ '  > tempo.C
echo " Int_t minhits  = $MINHITS ;"  >> tempo.C 
echo " Int_t maxhits = $MAXHITS ;"  >> tempo.C 
echo ' AliPHOSGetter * gime = AliPHOSGetter::Instance("galice.root") ;'  >> tempo.C
echo ' TH1F * hitmul = new TH1F("hitmul", "PHOS Test Suite", 100, 0., 200.) ; ' >> tempo.C
echo ' Int_t max = gime->MaxEvent() ; ' >> tempo.C
echo ' Int_t evt = 0 ; ' >> tempo.C
echo ' for ( evt = 0 ; evt < max ; evt++ ) { ' >> tempo.C
echo '  gime->Event(evt) ; ' >> tempo.C
echo '  hitmul->Fill(gime->Hits()->GetEntries()) ; ' >> tempo.C
echo ' } ' >> tempo.C
echo ' TF1 * gaus = new TF1("gaus", "gaus", 0., 200.) ; ' >> tempo.C 
echo ' hitmul->Fit(gaus) ; ' >> tempo.C 
echo ' if( gaus->GetParameter(1) > maxhits ||  gaus->GetParameter(1) < minhits ) ' >> tempo.C  
echo '  printf("ERRORSIM 1") ; ' >> tempo.C
echo ' else ' >> tempo.C
echo '  printf("ERRORSIM 0") ; ' >> tempo.C
echo '} '  >> tempo.C
aliroot -b -q tempo.C >>& TestSuite.log 
set ERRORSIM = `cat TestSuite.log | grep ERRORSIM | awk '{print $2}'`
if($ERRORSIM) then
  echo ERROR
  uuencode  TestSuite.log  TestSuite.log | mail -s 'PHOS INSTALLATION ERROR' yves.schutz@cern.ch
  exit(0)
endif 
rm tempo.C 
#
# RECONSTRUCTION
#
#rm PHOS.SDigits.root
#rm PHOS.Digits.root
#rm PHOS.RecPoints.root
#rm PHOS.Tracks.root
#rm PHOS.RecParticles.root
echo "    **** RECONSTRUCTION **** " >> TestSuite.log
aliroot -b >>& TestSuite.log <<EOF
 AliPHOSSDigitizer sd("galice.root") ; 
 sd.ExecuteTask("deb all") ; 
 AliPHOSDigitizer d("galice.root") ; 
 d.ExecuteTask("deb all") ; 
 AliPHOSClusterizerv1 cl("galice.root") ; 
 cl.ExecuteTask("deb all") ; 
 AliPHOSTrackSegmentMakerv1 ts("galice.root") ; 
 ts.ExecuteTask("deb all") ; 
 AliPHOSPIDv1 pd("galice.root") ; 
 pd.ExecuteTask("deb all") ; 
 .q
EOF
