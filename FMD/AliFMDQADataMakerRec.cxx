/**************************************************************************
 * Copyright(c) 2004, ALICE Experiment at CERN, All rights reserved. *
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
// --- ROOT system ---
#include <iostream>
#include <TClonesArray.h>
#include <TFile.h> 
#include <TH1F.h> 
#include <TH1I.h> 

// --- AliRoot header files ---
#include "AliESDEvent.h"
#include "AliLog.h"
#include "AliFMDQADataMakerRec.h"
#include "AliFMDDigit.h"
#include "AliFMDRecPoint.h"
#include "AliQAChecker.h"
#include "AliESDFMD.h"
#include "AliFMDParameters.h"
#include "AliFMDRawReader.h"
#include "AliRawReader.h"
#include "AliFMDAltroMapping.h"

//_____________________________________________________________________
// This is the class that collects the QA data for the FMD during
// reconstruction.  
//
// The following data types are picked up:
// - rec points
// - esd data
// - raws
// Author : Hans Hjersing Dalsgaard, hans.dalsgaard@cern.ch
//_____________________________________________________________________

ClassImp(AliFMDQADataMakerRec)
#if 0
; // For Emacs - do not delete!
#endif
           
//_____________________________________________________________________
AliFMDQADataMakerRec::AliFMDQADataMakerRec() : 
  AliQADataMakerRec(AliQA::GetDetName(AliQA::kFMD), 
		    "FMD Quality Assurance Data Maker"),
  fDigitsArray("AliFMDDigit", 0),
  fRecPointsArray("AliFMDRecPoint", 1000)
{
  // ctor
 
}

//_____________________________________________________________________
AliFMDQADataMakerRec::AliFMDQADataMakerRec(const AliFMDQADataMakerRec& qadm) 
  : AliQADataMakerRec(AliQA::GetDetName(AliQA::kFMD), 
		      "FMD Quality Assurance Data Maker"),
    fDigitsArray(qadm.fDigitsArray),
    fRecPointsArray(qadm.fRecPointsArray)
{
  // copy ctor 
  // Parameters: 
  //    qadm    Object to copy from
  
}
//_____________________________________________________________________
AliFMDQADataMakerRec& AliFMDQADataMakerRec::operator = (const AliFMDQADataMakerRec& qadm ) 
{
  fDigitsArray = qadm.fDigitsArray;
  fRecPointsArray = qadm.fRecPointsArray;
  
  return *this;
}
//_____________________________________________________________________
AliFMDQADataMakerRec::~AliFMDQADataMakerRec()
{
 
}


//_____________________________________________________________________ 

void 
AliFMDQADataMakerRec::EndOfDetectorCycle(AliQA::TASKINDEX_t task, 
					 TObjArray * list)
{
  // Detector specific actions at end of cycle
  // do the QA checking
  AliLog::Message(5,"FMD: end of detector cycle",
		  "AliFMDQADataMakerRec","AliFMDQADataMakerRec",
		  "AliFMDQADataMakerRec::EndOfDetectorCycle",
		  "AliFMDQADataMakerRec.cxx",95);
  AliQAChecker::Instance()->Run(AliQA::kFMD, task, list);
}

//_____________________________________________________________________ 
void AliFMDQADataMakerRec::InitESDs()
{
  // create Digits histograms in Digits subdir
  TH1F* hEnergyOfESD = new TH1F("hEnergyOfESD","Energy distribution",100,0,3);
  hEnergyOfESD->SetXTitle("Edep/Emip");
  hEnergyOfESD->SetYTitle("Counts");
  Add2ESDsList(hEnergyOfESD, 0);
    
}

//_____________________________________________________________________ 
/*void AliFMDQADataMakerRec::InitDigits()
{
  // create Digits histograms in Digits subdir
  TH1I* hADCCounts      = new TH1I("hADCCounts","Dist of ADC counts",
				   1024,0,1024);
  hADCCounts->SetXTitle("ADC counts");
  hADCCounts->SetYTitle("");
  Add2DigitsList(hADCCounts, 0);
  
}
*/
//_____________________________________________________________________ 
void AliFMDQADataMakerRec::InitRecPoints()
{
  TH1F* hEnergyOfRecpoints = new TH1F("hEnergyOfRecpoints",
				      "Energy Distribution",100,0,3);
  hEnergyOfRecpoints->SetXTitle("Edep/Emip");
  hEnergyOfRecpoints->SetYTitle("");
  Add2RecPointsList(hEnergyOfRecpoints,0);
}

//_____________________________________________________________________ 
void AliFMDQADataMakerRec::InitRaws()
{
  
  TH1I* hADCCounts;
  for(Int_t det = 1; det<=3; det++) {
    Int_t firstring = (det==1 ? 1 : 0);
    for(Int_t iring = firstring;iring<=1;iring++) {
      Char_t ring = (iring == 1 ? 'I' : 'O');
      hADCCounts      = new TH1I(Form("hADCCounts_FMD%d%c",
				      det, ring), "ADC counts",
				 1024,0,1023);
      
      Int_t index1 = GetHalfringIndex(det, ring, 0,1);
      Add2RawsList(hADCCounts, index1,kTRUE);
      
      for(Int_t b = 0; b<=1;b++) {
	
	//Hexadecimal board numbers 0x0, 0x1, 0x10, 0x11;
	UInt_t board = (iring == 1 ? 0 : 1);
	board = board + b*16;
	
	
	hADCCounts      = new TH1I(Form("hADCCounts_FMD%d%c_board%d",
					det, ring, board), "ADC counts",
				   1024,0,1023);
	hADCCounts->SetXTitle("ADC counts");
	hADCCounts->SetYTitle("");
	Int_t index2 = GetHalfringIndex(det, ring, board/16,0);
	Add2RawsList(hADCCounts, index2);

      }
    }
  }
}

//_____________________________________________________________________
void AliFMDQADataMakerRec::MakeESDs(AliESDEvent * esd)
{
  if(!esd) {
    AliError("FMD ESD object not found!!") ; 
    return;
  }
  AliESDFMD* fmd = esd->GetFMDData();
  if (!fmd) return;
  
  for(UShort_t det=1;det<=3;det++) {
    for (UShort_t ir = 0; ir < 2; ir++) {
      Char_t   ring = (ir == 0 ? 'I' : 'O');
      UShort_t nsec = (ir == 0 ? 20  : 40);
      UShort_t nstr = (ir == 0 ? 512 : 256);
      for(UShort_t sec =0; sec < nsec;  sec++)  {
	for(UShort_t strip = 0; strip < nstr; strip++) {
	  Float_t mult = fmd->Multiplicity(det,ring,sec,strip);
	  if(mult == AliESDFMD::kInvalidMult) continue;
	  
	  GetESDsData(0)->Fill(mult);
	}
      }
    }
  }
}

/*
//_____________________________________________________________________
void AliFMDQADataMakerRec::MakeDigits(TClonesArray * digits)
{
  // makes data from Digits  
  if(!digits)  {
    AliError("FMD Digit object not found!!") ;
    return;
  }
  for(Int_t i=0;i<digits->GetEntriesFast();i++) {
    //Raw ADC counts
    AliFMDDigit* digit = static_cast<AliFMDDigit*>(digits->At(i));
    GetDigitsData(0)->Fill(digit->Counts());
  }
}

//_____________________________________________________________________
void AliFMDQADataMakerRec::MakeDigits(TTree * digitTree)
{
  
  fDigitsArray.Clear();
  TBranch*      branch = digitTree->GetBranch("FMD");
  if (!branch) {
    AliWarning("FMD branch in Digit Tree not found") ; 
    return;
  } 
  TClonesArray* digitsAddress = &fDigitsArray;
  branch->SetAddress(&digitsAddress);
  branch->GetEntry(0); 
  MakeDigits(digitsAddress);
}
*/
//_____________________________________________________________________
void AliFMDQADataMakerRec::MakeRaws(AliRawReader* rawReader)
{
 
  AliFMDRawReader fmdReader(rawReader,0);
  TClonesArray* digitsAddress = &fDigitsArray;
  
  rawReader->Reset();
		
  digitsAddress->Clear();
  fmdReader.ReadAdcs(digitsAddress);
  for(Int_t i=0;i<digitsAddress->GetEntriesFast();i++) {
    //Raw ADC counts
    AliFMDDigit*      digit = static_cast<AliFMDDigit*>(digitsAddress->At(i));
    UShort_t          det   = digit->Detector();
    Char_t            ring  = digit->Ring();
    UShort_t          sec   = digit->Sector();
    // UShort_t strip = digit->Strip();
    AliFMDParameters* pars  = AliFMDParameters::Instance();
    Short_t           board = pars->GetAltroMap()->Sector2Board(ring, sec);
    
    Int_t index1 = GetHalfringIndex(det, ring, 0, 1);
    GetRawsData(index1)->Fill(digit->Counts());
    Int_t index2 = GetHalfringIndex(det, ring, board/16,0);
    GetRawsData(index2)->Fill(digit->Counts());
    
  }
}

//_____________________________________________________________________
void AliFMDQADataMakerRec::MakeRecPoints(TTree* clustersTree)
{
  // makes data from RecPoints
  AliFMDParameters* pars = AliFMDParameters::Instance();
  fRecPointsArray.Clear();
  TBranch *fmdbranch = clustersTree->GetBranch("FMD");
  if (!fmdbranch) { 
    AliError("can't get the branch with the FMD recpoints !");
    return;
  }
  
  TClonesArray* RecPointsAddress = &fRecPointsArray;
  
  fmdbranch->SetAddress(&RecPointsAddress);
  fmdbranch->GetEntry(0);
  TIter next(RecPointsAddress) ; 
  AliFMDRecPoint * rp ; 
  while ((rp = static_cast<AliFMDRecPoint*>(next()))) {
    
    GetRecPointsData(0)->Fill(rp->Edep()/pars->GetEdepMip()) ;
  
  }

}

//_____________________________________________________________________ 
void AliFMDQADataMakerRec::StartOfDetectorCycle()
{
  // What 
  // to 
  // do?
}
//_____________________________________________________________________ 
Int_t AliFMDQADataMakerRec::GetHalfringIndex(UShort_t det, 
					     Char_t ring, 
					     UShort_t board, 
					     UShort_t monitor) {
  
  UShort_t iring  =  (ring == 'I' ? 1 : 0);
  
  Int_t index = ( ((det-1) << 3) | (iring << 2) | (board << 1) | (monitor << 0));
  
  return index-2;
  
}

//_____________________________________________________________________ 


//
// EOF
//
