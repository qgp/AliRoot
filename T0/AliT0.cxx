/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  T0 (T-Zero) Detector                                            //
//  This class contains the base procedures for the T0     //
//  detector                                                                 //
//                                                                           //
//Begin_Html
/*
<img src="gif/AliT0Class.gif">
</pre>
<br clear=left>
<font size=+2 color=red>
<p>The responsible person for this module is
<a href="mailto:Alla.Maevskaia@cern.ch">Alla Maevskaia</a>.
</font>
<pre>
*/
//End_Html
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TClonesArray.h"

#include "AliLoader.h"
#include "AliLog.h"
#include "AliLog.h"
#include "AliMC.h"
#include "AliRun.h"
#include "AliT0.h"
#include "AliT0Digitizer.h"
#include "AliT0RawData.h"
#include "AliT0RecPoint.h"
#include "AliT0digit.h"
#include "AliT0hit.h"

ClassImp(AliT0)

  //static  AliT0digit *digits; 

//_____________________________________________________________________________
AliT0::AliT0()
  : AliDetector(), fIdSens(0), fDigits(NULL), fRecPoints(NULL)
{
  //
  // Default constructor for class AliT0
  //
  fIshunt   = 1;
  fHits     = 0;
  fDigits   = 0;
  fRecPoints = 0;
}
 
//_____________________________________________________________________________
AliT0::AliT0(const char *name, const char *title)
  : AliDetector(name,title), fIdSens(0), fDigits(new AliT0digit()), fRecPoints(new AliT0RecPoint())
{
  //
  // Standard constructor for T0 Detector
  //

  
  //
  // Initialise Hit array
  fHits       = new TClonesArray("AliT0hit",  405);
  gAlice->GetMCApp()->AddHitList(fHits);
  //  fDigits    = new AliT0digit();
  //  fRecPoints = new AliT0RecPoint();
  fIshunt     =  1;
  //  fIdSens   =  0;
  //PH  SetMarkerColor(kRed);
}

//_____________________________________________________________________________
AliT0::~AliT0() {
  
  //destructor
  if (fHits) {
    fHits->Delete();
    delete fHits;
  }
  /*
  if (fDigits) {
    fDigits->Delete();
    delete fDigits;
    cout<<" delete fDigits; "<<endl;
  }
  if (fRecPoints) {
   fRecPoints ->Delete();
    delete fRecPoints;
    cout<<" delete fRecPoints; "<<endl;
  }
  */ 
}

//_____________________________________________________________________________
void AliT0::AddHit(Int_t track, Int_t *vol, Float_t *hits)
{
  //
  // Add a T0 hit
  //
  TClonesArray &lhits = *fHits;
  new(lhits[fNhits++]) AliT0hit(fIshunt,track,vol,hits);
}


//_____________________________________________________________________________

void AliT0::AddDigit(Int_t besttimeright, Int_t besttimeleft, Int_t meantime, 
		     Int_t timediff, Int_t sumMult, Int_t refpoint,
			TArrayI *timeCFD, TArrayI *qt0, TArrayI *timeLED, TArrayI *qt1)
{
  
  //  Add a T0 digit to the list.
 //
  
  if (!fDigits) {
    fDigits = new AliT0digit();
  }
  fDigits-> SetTimeBestA(besttimeright);
  fDigits->SetTimeBestC(besttimeleft);
  fDigits-> SetMeanTime(meantime);
  fDigits-> SetDiffTime(timediff);
  fDigits-> SetSumMult(sumMult);
  fDigits->SetTimeCFD(*timeCFD);
  fDigits->SetTimeLED(*timeLED);
  fDigits->SetQT0(*qt0);
  fDigits->SetQT1(*qt1);
  fDigits->SetRefPoint(refpoint);
}

//-------------------------------------------------------------------------
void AliT0::Init()
{
  //
  // Initialis the T0 after it has been built
  Int_t i;
  //
  if(AliLog::GetGlobalDebugLevel()>0) {
    printf("\n%s: ",ClassName());
    for(i=0;i<35;i++) printf("*");
    printf(" T0_INIT ");
    for(i=0;i<35;i++) printf("*");
    printf("\n%s: ",ClassName());
    //
    // Here the T0 initialisation code (if any!)
    for(i=0;i<80;i++) printf("*");
    printf("\n");
  }
}

//---------------------------------------------------------------------------
void AliT0::MakeBranch(Option_t* option)
{
  //
// Create Tree branches for the T0.

 // Options:
  //
  //    H          Make a branch of TClonesArray of AliT0Hit's
  //    D          Make a branch of TClonesArray of AliT0Digit's
  //
  //    R         Make a branch of  AliT0RecPoints
  //
  char branchname[20];
  // sprintf(branchname,"%s",GetName());
  strncpy(branchname, GetName(), 20);
  const char *cH = strstr(option,"H");
  const char *cD = strstr(option,"D");
  const char *cR = strstr(option,"R");
  const char *cS = strstr(option,"S");

    if (cH && fLoader->TreeH())
  {
     if (fHits == 0x0) fHits  = new TClonesArray("AliT0hit",  405);
     AliDetector::MakeBranch(option);
  } 
    
    
  if (cD && fLoader->TreeD())
    {
      if (fDigits == 0x0) fDigits  = new AliT0digit();
      //     MakeBranchInTree(fLoader->TreeD(), branchname,
      //		       &fDigits, 405, 0);
      fLoader->TreeD()->Branch(branchname,"AliT0digit",&fDigits);
      //   fLoader->TreeD()->Print();
    } 
  if (cR && fLoader->TreeR())
    {
      if (fRecPoints == 0x0) fRecPoints  = new AliT0RecPoint();
      MakeBranchInTree(fLoader->TreeR(), branchname, &fRecPoints);
    } 
  if (cS && fLoader->TreeS())
    {
      if (fDigits == 0x0) fDigits  = new AliT0digit();
      //     MakeBranchInTree(fLoader->TreeD(), branchname,
      //		       &fDigits, 405, 0);
      fLoader->TreeS()->Branch(branchname,"AliT0digit",&fDigits);
      //   fLoader->TreeD()->Print();
    } 
  
}    

//_____________________________________________________________________________
void AliT0::ResetHits()
{
  //
  //reset hits
  //
  AliDetector::ResetHits();
  
}
//____________________________________________________________________
void AliT0::ResetDigits()
{
  //
  // Reset number of digits and the digits array for this detector
  //
  if (fDigits) fDigits->Clear();
}

//_____________________________________________________________________________
void AliT0::SetTreeAddress()
{

  TTree    *treeH = fLoader->TreeH();
  
  if (treeH)
    {
      if (fHits == 0x0) fHits  = new TClonesArray("AliT0hit",  405);
    }
    
  AliDetector::SetTreeAddress();
  TTree *treeD = fLoader->TreeD();
  if (treeD) {
    if (fDigits == 0x0)  fDigits  = new AliT0digit();
    TBranch* branch = treeD->GetBranch ("T0");
    if (branch) branch->SetAddress(&fDigits);
  }

  TTree *treeR = fLoader->TreeR();
  if (treeR) {
    if (fRecPoints == 0x0) fRecPoints  = new  AliT0RecPoint()  ;
    TBranch* branch = treeR->GetBranch ("T0");
    if (branch) branch->SetAddress(&fRecPoints);
  }
  // SDigitizer for Federico
  TTree *treeS = fLoader->TreeS();
  if (treeS) {
    if (fDigits == 0x0)  fDigits  = new AliT0digit();
    TBranch* branch = treeS->GetBranch ("T0");
    if (branch) branch->SetAddress(&fDigits);
  }
 
}


//_____________________________________________________________________________
void AliT0::MakeBranchInTreeD(TTree *treeD, const char *file)
{
    //
    // Create TreeD branches for the FMD
    //
    const Int_t kBufferSize = 4000;
    char branchname[20];
   strncpy(branchname, GetName(), 20);
   //   sprintf(branchname,"%s",GetName());
    if(treeD)
     {
       MakeBranchInTree(treeD,  branchname,&fDigits, kBufferSize, file);
     }
}

//_____________________________________________________________________________
AliDigitizer* AliT0::CreateDigitizer(AliRunDigitizer* manager) const
{
  return new AliT0Digitizer(manager);
}
//____________________________________________________________________________
void AliT0::Digits2Raw()
{
//
// Starting from the T0 digits, writes the Raw Data objects
//
//  AliT0Loader* pStartLoader = (AliT0Loader*)fLoader;
  fLoader ->LoadDigits("read");
  TTree* treeD = fLoader->TreeD();
  if (!treeD) {
    AliError("no digits tree");
    return;
  }
  if (fDigits == 0x0)  fDigits  = new AliT0digit();
  
  TBranch *branch = treeD->GetBranch("T0");
  if (branch) {
    branch->SetAddress(&fDigits);
  }else{
    AliError("Branch T0 DIGIT not found");
    exit(111);
  } 
  AliT0RawData rawWriter;
  rawWriter.SetVerbose(0);
  
  AliDebug(2,Form(" Formatting raw data for T0 "));
  branch->GetEntry(0);
  //  rawWriter.RawDataT0(treeD->GetBranch("T0"));
  rawWriter.RawDataT0(fDigits);
  
  
  fLoader->UnloadDigits();
  
}

//____________________________________________________________________________
void AliT0::Raw2Digits(AliRawReader *rawReader,TTree* digitsTree)
{

 //T0 raw data-> digits conversion
 // reconstruct time information from raw data
 // cout<<"  AliT0::Raw2Digits(AliRawReader *rawReader,TTree* digitsTree) "<<
  // rawReader<<" "<<digitsTree<<endl;

 
  //  AliT0RawReader myrawreader(rawReader,digitsTree);
   AliT0RawReader myrawreader(rawReader);
   if (!myrawreader.Next())
     AliDebug(1,Form(" no raw data found!! %i", myrawreader.Next()));
   Int_t allData[110][5];
   for (Int_t i=0; i<110; i++) {
     allData[i][0]=myrawreader.GetData(i,0);
   }


   fDigits = new AliT0digit();
   digitsTree->Branch("T0","AliT0digit",&fDigits);
   
   
   TArrayI *timeLED = new TArrayI(24);
   TArrayI * timeCFD = new TArrayI(24);
   TArrayI *chargeQT0 = new TArrayI(24);
   TArrayI *chargeQT1 = new TArrayI(24);
   
   for (Int_t in=0; in<24; in++)
     {
       timeLED->AddAt(allData[in+1][0],in);
       timeCFD->AddAt(allData[in+25][0],in);
       chargeQT0->AddAt(allData[in+55][0],in);
       chargeQT1->AddAt(allData[in+79][0],in);
       AliDebug(2, Form(" readed Raw %i %i %i %i %i", in, timeLED->At(in),timeCFD->At(in),chargeQT0->At(in),chargeQT1->At(in)));
     }
  
   fDigits->SetTimeCFD(*timeCFD);
   fDigits->SetQT0(*chargeQT1);

   fDigits->SetTimeLED(*timeLED);
   fDigits->SetQT1(*chargeQT1);

   fDigits->SetMeanTime(allData[49][0]);
   fDigits->SetDiffTime(allData[50][0]);
   fDigits->SetTimeBestA(allData[51][0]);
   fDigits->SetTimeBestC(allData[52][0]);
   digitsTree->Fill();
   fDigits->Write();
 
   delete timeCFD ;
   delete chargeQT0;
   delete timeLED ;
   delete chargeQT1;


}
