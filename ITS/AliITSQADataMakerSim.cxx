/**************************************************************************
 * Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
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

//  *************************************************************
//  Checks the quality assurance 
//  by comparing with reference data
//  contained in a DB
//  -------------------------------------------------------------
//  W. Ferrarese + P. Cerello Feb 2008
//  INFN Torino

// --- ROOT system ---
#include <TTree.h>
#include <TMath.h>
// --- Standard library ---

// --- AliRoot header files ---
#include "AliITSQADataMakerSim.h"
#include "AliITSQASPDDataMakerSim.h"
#include "AliITSQASDDDataMakerSim.h"
#include "AliITSQASSDDataMakerSim.h"
#include "AliLog.h"
#include "AliQAv1.h"
#include "AliQAChecker.h"
#include "AliITSQAChecker.h"
#include "AliRawReader.h"

ClassImp(AliITSQADataMakerSim)

//____________________________________________________________________________ 
AliITSQADataMakerSim::AliITSQADataMakerSim(Short_t subDet) :
AliQADataMakerSim(AliQAv1::GetDetName(AliQAv1::kITS), "ITS Quality Assurance Data Maker"),
fSubDetector(subDet),
fSPDDataMaker(NULL),
fSDDDataMaker(NULL),
fSSDDataMaker(NULL)
{
  //ctor used to discriminate OnLine-Offline analysis
  if(fSubDetector < 0 || fSubDetector > 3) {
	AliError("Error: fSubDetector number out of range; return\n");
  }

  // Initialization for RAW data 
  if(fSubDetector == 0 || fSubDetector == 1) {
    AliDebug(AliQAv1::GetQADebugLevel(),"AliITSQADM::Create SPD DataMakerSim\n");
	fSPDDataMaker = new AliITSQASPDDataMakerSim(this);
  }
  if(fSubDetector == 0 || fSubDetector == 2) {
    AliDebug(AliQAv1::GetQADebugLevel(),"AliITSQADM::Create SDD DataMakerSim\n");
	//printf("AliITSQADM::Create SDD DataMakerSim\n");		    
	fSDDDataMaker = new AliITSQASDDDataMakerSim(this);
  }
  if(fSubDetector == 0 || fSubDetector == 3) {
    AliDebug(AliQAv1::GetQADebugLevel(),"AliITSQADM::Create SSD DataMakerSim\n");
	fSSDDataMaker = new AliITSQASSDDataMakerSim(this);
  }
}

//____________________________________________________________________________ 
AliITSQADataMakerSim::~AliITSQADataMakerSim(){
  // destructor
  if(fSPDDataMaker)delete fSPDDataMaker;
  if(fSDDDataMaker)delete fSDDDataMaker;
  if(fSSDDataMaker)delete fSSDDataMaker;
}

//____________________________________________________________________________ 
AliITSQADataMakerSim::AliITSQADataMakerSim(const AliITSQADataMakerSim& qadm) :
AliQADataMakerSim(),
fSubDetector(qadm.fSubDetector),
fSPDDataMaker(NULL),
fSDDDataMaker(NULL),
fSSDDataMaker(NULL)
{
  //copy ctor 
  SetName((const char*)qadm.GetName()) ; 
  SetTitle((const char*)qadm.GetTitle());
}

//__________________________________________________________________
AliITSQADataMakerSim& AliITSQADataMakerSim::operator = (const AliITSQADataMakerSim& qac )
{
  // Equal operator.
  this->~AliITSQADataMakerSim();
  new(this) AliITSQADataMakerSim(qac);
  return *this;
}

//____________________________________________________________________________ 
void AliITSQADataMakerSim::StartOfDetectorCycle()
{
  //Detector specific actions at start of cycle
  AliDebug(AliQAv1::GetQADebugLevel(),"AliITSQADM::Start of ITS Cycle\n");

  if(fSubDetector == 0 || fSubDetector == 1) fSPDDataMaker->StartOfDetectorCycle();
  if(fSubDetector == 0 || fSubDetector == 2) fSDDDataMaker->StartOfDetectorCycle();
  if(fSubDetector == 0 || fSubDetector == 3) fSSDDataMaker->StartOfDetectorCycle();
}

//____________________________________________________________________________ 
void AliITSQADataMakerSim::EndOfDetectorCycle(AliQAv1::TASKINDEX_t task, TObjArray** list)
{
  // launch the QA checking
  for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) {
    if (! IsValidEventSpecie(specie, list) )
      continue ; 
    AliDebug(AliQAv1::GetQADebugLevel(),"AliITSDM instantiates checker with Run(AliQAv1::kITS, task, list)\n"); 
    if(fSubDetector == 0 || fSubDetector == 1) fSPDDataMaker->EndOfDetectorCycle(task, list[specie]);
    if(fSubDetector == 0 || fSubDetector == 2) fSDDDataMaker->EndOfDetectorCycle(task, list[specie]);
    if(fSubDetector == 0 || fSubDetector == 3) fSSDDataMaker->EndOfDetectorCycle(task, list[specie]);
  
  
    AliQAChecker *qac = AliQAChecker::Instance();
    AliITSQAChecker *qacb = (AliITSQAChecker *) qac->GetDetQAChecker(0);
    Int_t subdet=GetSubDet();
    qacb->SetSubDet(subdet);

    if(subdet== 0 ){
      qacb->SetTaskOffset(fSPDDataMaker->GetOffset(task),fSDDDataMaker->GetOffset(task),fSSDDataMaker->GetOffset(task)); //Setting the offset for the QAChecker list		
    }
    else
      if(subdet!=0){
	Int_t offset=GetDetTaskOffset(subdet, task);
	qacb->SetDetTaskOffset(subdet,offset);
      }
    qac->Run( AliQAv1::kITS , task, list); 
  }
}

//____________________________________________________________________________ 
void AliITSQADataMakerSim::InitDigits()
{  
  // Initialization for RAW data 
	if(fSubDetector == 0 || fSubDetector == 1) {
	  AliDebug(AliQAv1::GetQADebugLevel(),"AliITSQADM:: SPD InitDigits\n");

	  fSPDDataMaker->InitDigits();
	}
	if(fSubDetector == 0 || fSubDetector == 2) {
 	  AliDebug(AliQAv1::GetQADebugLevel(),"AliITSQADM:: SDD InitDigits\n");

	  fSDDDataMaker->SetOffset(AliQAv1::kDIGITS, fDigitsQAList[AliRecoParam::AConvert(fEventSpecie)]->GetEntries(),AliRecoParam::AConvert(fEventSpecie));
	  fSDDDataMaker->InitDigits();
	}
	if(fSubDetector == 0 || fSubDetector == 3) {
	  AliDebug(AliQAv1::GetQADebugLevel(),"AliITSQADM:: SSD InitDigits\n");

	  fSSDDataMaker->SetOffset(AliQAv1::kDIGITS, fDigitsQAList[AliRecoParam::AConvert(fEventSpecie)]->GetEntries(),AliRecoParam::AConvert(fEventSpecie));
	  fSSDDataMaker->InitDigits();
	}
}

//____________________________________________________________________________
void AliITSQADataMakerSim::MakeDigits()
{ 
  // Fill QA for digits   
  if(fSubDetector == 0 || fSubDetector == 1) 
    fSPDDataMaker->MakeDigits() ; 

  
  if(fSubDetector == 0 || fSubDetector == 2) 
    fSDDDataMaker->MakeDigits() ; 

  if(fSubDetector == 0 || fSubDetector == 3) fSSDDataMaker->MakeDigits();
}

//____________________________________________________________________________
void AliITSQADataMakerSim::MakeDigits(TTree * digits)
{ 
  // Fill QA for digits   
  if(fSubDetector == 0 || fSubDetector == 1) 
    fSPDDataMaker->MakeDigits(digits) ; 

  if(fSubDetector == 0 || fSubDetector == 2)
    fSDDDataMaker->MakeDigits(digits) ; 

  if(fSubDetector == 0 || fSubDetector == 3) fSSDDataMaker->MakeDigits(digits);
}

//____________________________________________________________________________ 
void AliITSQADataMakerSim::InitSDigits()
{
  // Initialization for RECPOINTS
  if(fSubDetector == 0 || fSubDetector == 1) {
    AliDebug(AliQAv1::GetQADebugLevel(),"AliITSQADM:: SPD InitSDigits\n");

    fSPDDataMaker->InitSDigits();
  }
  if(fSubDetector == 0 || fSubDetector == 2){ 
    AliDebug(AliQAv1::GetQADebugLevel(),"AliITSQADM:: SDD InitSDigits\n");

    fSDDDataMaker->SetOffset(AliQAv1::kSDIGITS, fSDigitsQAList [AliRecoParam::AConvert(fEventSpecie)]->GetEntries(),AliRecoParam::AConvert(fEventSpecie));
    fSDDDataMaker->InitSDigits();
  }
  if(fSubDetector == 0 || fSubDetector == 3) {
    AliDebug(AliQAv1::GetQADebugLevel(),"AliITSQADM:: SSD InitSDigits\n");

    fSSDDataMaker->SetOffset(AliQAv1::kSDIGITS, fSDigitsQAList [AliRecoParam::AConvert(fEventSpecie)]->GetEntries(),AliRecoParam::AConvert(fEventSpecie));
    fSSDDataMaker->InitSDigits();
  }
}

//____________________________________________________________________________ 
void AliITSQADataMakerSim::MakeSDigits()
{
  // Fill QA for recpoints
  if(fSubDetector == 0 || fSubDetector == 1)
    fSPDDataMaker->MakeSDigits() ; 

  
  if(fSubDetector == 0 || fSubDetector == 2) 
    fSDDDataMaker->MakeSDigits() ; 


  if(fSubDetector == 0 || fSubDetector == 3) fSSDDataMaker->MakeSDigits();
}

//____________________________________________________________________________ 
void AliITSQADataMakerSim::MakeSDigits(TTree * sdigits)
{
  // Fill QA for recpoints
  if(fSubDetector == 0 || fSubDetector == 1){
    fSPDDataMaker->MakeSDigits(sdigits) ; 
 }
  
  if(fSubDetector == 0 || fSubDetector == 2){
    fSDDDataMaker->MakeSDigits(sdigits) ; 
  }

  if(fSubDetector == 0 || fSubDetector == 3) fSSDDataMaker->MakeSDigits(sdigits);
}

//____________________________________________________________________________ 
void AliITSQADataMakerSim::InitHits()
{
  // Initialization for hits
  if(fSubDetector == 0 || fSubDetector == 1) {
    AliDebug(AliQAv1::GetQADebugLevel(),"AliITSQADM:: SPD InitHits\n");
    fSPDDataMaker->InitHits();
  }
  if(fSubDetector == 0 || fSubDetector == 2) {
    AliDebug(AliQAv1::GetQADebugLevel(),"AliITSQADM:: SDD InitHits\n");

    fSDDDataMaker->SetOffset(AliQAv1::kHITS, fHitsQAList[AliRecoParam::AConvert(fEventSpecie)]->GetEntries(),AliRecoParam::AConvert(fEventSpecie));
    fSDDDataMaker->InitHits();
  }
  if(fSubDetector == 0 || fSubDetector == 3) {
    AliDebug(AliQAv1::GetQADebugLevel(),"AliITSQADM:: SSD InitHits\n");

    fSSDDataMaker->SetOffset(AliQAv1::kHITS, fHitsQAList[AliRecoParam::AConvert(fEventSpecie)]->GetEntries(),AliRecoParam::AConvert(fEventSpecie));
    fSSDDataMaker->InitHits();
  }
}

//____________________________________________________________________________ 
void AliITSQADataMakerSim::MakeHits()
{
  // Fill QA for hits
  if(fSubDetector == 0 || fSubDetector == 1) {
    fSPDDataMaker->MakeHits() ; 
    }
  
  if(fSubDetector == 0 || fSubDetector == 2) {
    fSDDDataMaker->MakeHits() ; 
  }

  if(fSubDetector == 0 || fSubDetector == 3) fSSDDataMaker->MakeHits();
}

//____________________________________________________________________________ 
void AliITSQADataMakerSim::MakeHits(TTree * hits)
{
  // Fill QA for hits
  if(fSubDetector == 0 || fSubDetector == 1) {
    fSPDDataMaker->MakeHits(hits) ; 
   }
  if(fSubDetector == 0 || fSubDetector == 2) {
    fSDDDataMaker->MakeHits(hits) ; 
  }

  if(fSubDetector == 0 || fSubDetector == 3) fSSDDataMaker->MakeHits(hits);
}

//_________________________________________________________________
Int_t AliITSQADataMakerSim::GetDetTaskOffset(Int_t subdet,AliQAv1::TASKINDEX_t task)
{
  switch(subdet)
    {

      Int_t offset;
    case 1:
      offset=fSPDDataMaker->GetOffset(task);
      return offset;
      break;
    case 2:
      offset=fSDDDataMaker->GetOffset(task);
      return offset;
      break;
    case 3:
      offset=fSSDDataMaker->GetOffset(task);
      return offset;
      break;
    default:
      AliWarning("No specific subdetector (SPD, SDD, SSD) selected!! Offset set to zero \n");
      offset=0;
      return offset;
      break;
    }
  //return offset;
}
