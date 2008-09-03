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

//
//  Base Class
//  Produces the data needed to calculate the quality assurance. 
//  All data must be mergeable objects.
//  Y. Schutz CERN July 2007
//

// --- ROOT system ---
#include <TSystem.h> 
#include <TFile.h>
#include <TList.h> 
#include <TTree.h>
#include <TClonesArray.h>
#include <TParameter.h>

// --- Standard library ---

// --- AliRoot header files ---
#include "AliLog.h"
#include "AliQADataMaker.h"
#include "AliQAChecker.h"
#include "AliESDEvent.h"
#include "AliRawReader.h"

ClassImp(AliQADataMaker)
             
//____________________________________________________________________________ 
AliQADataMaker::AliQADataMaker(const char * name, const char * title) : 
  TNamed(name, title), 
  fOutput(0x0),
  fDetectorDir(0x0),
  fDetectorDirName(""), 
  fCurrentCycle(0), 
  fCycle(9999999), 
  fCycleCounter(0), 
  fWriteExpert(kFALSE),
  fParameterList(0x0), 
  fRun(0)
{
  // ctor
  fDetectorDirName = GetName() ; 
}

//____________________________________________________________________________ 
AliQADataMaker::AliQADataMaker(const AliQADataMaker& qadm) :
  TNamed(qadm.GetName(), qadm.GetTitle()),
  fOutput(qadm.fOutput),
  fDetectorDir(qadm.fDetectorDir),
  fDetectorDirName(qadm.fDetectorDirName),
  fCurrentCycle(qadm.fCurrentCycle), 
  fCycle(qadm.fCycle), 
  fCycleCounter(qadm.fCycleCounter), 
  fWriteExpert(qadm.fWriteExpert),
  fParameterList(qadm.fParameterList),  
  fRun(qadm.fRun)
{
  //copy ctor
  fDetectorDirName = GetName() ; 
}

//____________________________________________________________________________
Int_t AliQADataMaker::Add2List(TH1 * hist, const Int_t index, TObjArray * list, const Bool_t expert, const Bool_t saveForCorr) 
{ 
	// Set histograms memory resident and add to the list
	// Maximm allowed is 10000
        TString className(hist->ClassName()) ;
        if( ! className.BeginsWith("T") ) {
	        AliError(Form("QA data Object must be a generic ROOT object and not %s", className.Data())) ; 
//	        return -1 ;
	}
	if ( index > 10000 ) {
		AliError("Max number of authorized QA objects is 10000") ; 
		return -1 ; 
	} else {
		hist->SetDirectory(0) ; 
    
    if (expert) 
      hist->SetBit(AliQA::GetExpertBit()) ; 
		
    list->AddAtAndExpand(hist, index) ; 
    char * name = Form("%s_%s", list->GetName(), hist->GetName()) ;  
    TParameter<double> * p = new TParameter<double>(name, 9999.9999) ;
    if(saveForCorr) {  
      if ( ! fParameterList )
        fParameterList = new TList() ; 
      fParameterList->Add(p) ;
    }
    return list->GetLast() ;
  }
}

//____________________________________________________________________________
void AliQADataMaker::DefaultEndOfDetectorCycle(AliQA::TASKINDEX_t task) 
{
	// this method must be oveloaded by detectors
	// sets the QA result to Fatal
	AliQA::Instance(AliQA::GetDetIndex(GetName())) ;
	AliQA * qa = AliQA::Instance(task) ;
	qa->Set(AliQA::kFATAL) ; 
	AliQA::GetQAResultFile()->cd() ; 
	qa->Write(AliQA::GetQAName(), kWriteDelete) ;   
	AliQA::GetQAResultFile()->Close() ; 
}

//____________________________________________________________________________ 
void AliQADataMaker::Finish() const 
{ 
	// write to the output File
	if (fOutput) 
		fOutput->Close() ; 
} 

//____________________________________________________________________________ 
TObject * AliQADataMaker::GetData(TObjArray * list, const Int_t index)  
{ 
	// Returns the QA object at index. Limit is 100. 
	if (list) {
		if ( index > 10000 ) {
			AliError("Max number of authorized QA objects is 10000") ; 
			return NULL ; 
		} else {
			return list->At(index) ; 
		} 	
	} else {
		AliError("Data list is NULL !!") ; 
		return NULL ; 		
	}
}
