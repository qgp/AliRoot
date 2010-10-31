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
/* History of cvs commits:
 *
 * $Log$
 * Revision 1.1  2006/12/07 16:32:16  gustavo
 * First shuttle code, online calibration histograms producer, EMCAL preprocessor
 * 
 *
*/
///////////////////////////////////////////////////////////////////////////////
// Class AliEMCALCalibHistoProducer accumulating histograms
// with amplitudes per EMCAL channel
// It is intended to run at DAQ computers (LDC, GDC, HLT or MOOD)
// and it fills the histograms with amplitudes per channel.
// Usage example see in EMCAL/macros/Shuttle/AliEMCALCalibHistoProducer.C
//
// Author: Boris Polichtchouk, 4 October 2006
// Adapted for EMCAL by Gustavo Conesa Balbastre, October 2006
///////////////////////////////////////////////////////////////////////////////

#include "TH1.h"
#include "TFile.h"
#include "TProfile.h"


#include "AliLog.h"
#include "AliRawReader.h"
#include "AliCaloRawStreamV3.h"
#include "AliEMCALCalibHistoProducer.h"

ClassImp(AliEMCALCalibHistoProducer)

//-----------------------------------------------------------------------------
AliEMCALCalibHistoProducer::AliEMCALCalibHistoProducer(AliRawReader* rawReader) : 
  TObject(),fRawReader(rawReader),fHistoFile(0),fHistoFileName("calibEmcHisto.root"),
  fUpdatingRate(100), fNSuperModules(12),  fNCellsEta (48),   
  fNCellsPhi(24),  fNCellsPhiHalfSM(12)
{
  // Constructor

  for(Int_t ism=0; ism<fNSuperModules; ism++) {
    fAmpProf[ism] = 0;
    fSMInstalled[ism]=kTRUE;
    for(Int_t icol=0; icol<fNCellsEta; icol++) 
      for(Int_t irow=0; irow<fNCellsPhi; irow++) 
	  fAmpHisto[ism][icol][irow]=0;
  }

}
//-----------------------------------------------------------------------------
AliEMCALCalibHistoProducer::AliEMCALCalibHistoProducer() : 
  fRawReader(0x0),fHistoFile(0),fHistoFileName(""),
  fUpdatingRate(0), fNSuperModules(12),  fNCellsEta (48),   
  fNCellsPhi(24),  fNCellsPhiHalfSM(12)
{
  // default Constructor

  for(Int_t ism=0; ism<fNSuperModules; ism++) {
    fAmpProf[ism] = 0;
    fSMInstalled[ism]=kTRUE;
    for(Int_t icol=0; icol<fNCellsEta; icol++) 
      for(Int_t irow=0; irow<fNCellsPhi; irow++) 
	  fAmpHisto[ism][icol][irow]=0;
  }

}

//-----------------------------------------------------------------------------
AliEMCALCalibHistoProducer::AliEMCALCalibHistoProducer(const AliEMCALCalibHistoProducer & copy) :
  TObject(copy),fRawReader((AliRawReader*)copy. fRawReader->Clone()),
  fHistoFile((TFile*)copy.fHistoFile->Clone()),fHistoFileName(copy.fHistoFileName),
  fUpdatingRate(copy.fUpdatingRate),
  fNSuperModules(copy.fNSuperModules), fNCellsEta (copy.fNCellsEta), 
  fNCellsPhi(copy.fNCellsPhi), fNCellsPhiHalfSM(copy.fNCellsPhiHalfSM)
{
  //copy constructor

 for(Int_t ism=0; ism<fNSuperModules; ism++) {
    fAmpProf[ism] = copy. fAmpProf[ism];
    fSMInstalled[ism]= copy.fSMInstalled[ism];
    for(Int_t icol=0; icol<fNCellsEta; icol++) 
      for(Int_t irow=0; irow<fNCellsPhi; irow++) 
	  fAmpHisto[ism][icol][irow]= copy.fAmpHisto[ism][icol][irow];
  }

}

//-----------------------------------------------------------------------------
AliEMCALCalibHistoProducer::~AliEMCALCalibHistoProducer()
{
  // Destructor
  if(fHistoFile) {
    fHistoFile->Close();
    delete fHistoFile;
  }
}

//------------------------------------------------------------------------------
//
AliEMCALCalibHistoProducer& AliEMCALCalibHistoProducer::operator=(const AliEMCALCalibHistoProducer& copy)
{
	//
	// Assignment operator.
	// Besides copying all parameters, duplicates all collections.	
	//
                if (&copy == this) return *this;
	TObject::operator=(copy);
	fHistoFileName = copy.fHistoFileName;
	fUpdatingRate = copy.fUpdatingRate;
	fNSuperModules = copy.fNSuperModules;
	fNCellsEta = copy.fNCellsEta;
	fNCellsPhi = copy.fNCellsPhi;
	fNCellsPhiHalfSM = copy.fNCellsPhiHalfSM;
	
	fRawReader  = (AliRawReader*)copy. fRawReader->Clone();
	fHistoFile      = (TFile*)copy.fHistoFile->Clone();

	for(Int_t ism=0; ism<fNSuperModules; ism++) {
	  fAmpProf[ism] = copy. fAmpProf[ism];
	  fSMInstalled[ism]= copy.fSMInstalled[ism];
	  for(Int_t icol=0; icol<fNCellsEta; icol++) 
	    for(Int_t irow=0; irow<fNCellsPhi; irow++) 
	      fAmpHisto[ism][icol][irow]= copy.fAmpHisto[ism][icol][irow];
	}

	return (*this);
}
//-----------------------------------------------------------------------------
void AliEMCALCalibHistoProducer::Init()
{
  // initializes input data stream supplied by rawReader
  // Checks existence of histograms which might have been left
  // from the previous runs to continue their filling
  fHistoFile =  new TFile(fHistoFileName,"update");
  const Int_t buffersize = 128;
  char hname[buffersize];
  Int_t nRow =  fNCellsPhi ;

  for(Int_t supermodule=0; supermodule<fNSuperModules; supermodule++) {
    //Check installed supermodules
    if(fSMInstalled[supermodule]==kFALSE) continue;
    //Check created profiles
    snprintf(hname,buffersize,"mod%d",supermodule);
    TProfile* prof = (TProfile*)fHistoFile->Get(hname);
    if(prof)
      fAmpProf[supermodule]=prof;
    
    //Check created histograms
    if(supermodule > 10) nRow = fNCellsPhiHalfSM ; //Supermodules 11 and 12 are half supermodules
    for(Int_t column=0; column<fNCellsEta; column++) {
      for(Int_t row=0; row<nRow; row++) {
        snprintf(hname,buffersize,"mod%dcol%drow%d",supermodule,column,row);
        TH1F* hist = (TH1F*)fHistoFile->Get(hname);
        if(hist) 
          fAmpHisto[supermodule][column][row]=hist;
      }
    }
  }
  
}
//-----------------------------------------------------------------------------
void AliEMCALCalibHistoProducer::Run()
{
  // Reads raw data stream and fills amplitude histograms
  // The histograms are written to file every fUpdatingRate events
  //Also fills profiles to study the stability of supermodules during runs.

  Init();
  
//   TH1F* gHighGain = 0;
//   TH1F* gLowGain = 0;
  const Int_t buffersize=128;
  char hname[buffersize];
  Int_t iEvent = 0;
  Int_t runNum = 0;
  Int_t nProfFreq = 1000; //Number of events with which a bin of the TProfile if filled
  Int_t nEvtBins = 1000; //Total number of the profile survey bins.

  AliCaloRawStreamV3 in(fRawReader,"EMCAL");

  // Read raw data event by event

  while (fRawReader->NextEvent()) {
    runNum = fRawReader->GetRunNumber();

    while (in.NextDDL()) {
      while (in.NextChannel()) {

	if(fSMInstalled[in.GetModule()]==kFALSE) continue;

	// loop over samples
	int nsamples = 0;
	Int_t maxSample = 0;
	while (in.NextBunch()) {
	  const UShort_t *sig = in.GetSignals();
	  nsamples += in.GetBunchLength();
	  for (Int_t i = 0; i < in.GetBunchLength(); i++) {
	    if (sig[i] > maxSample) maxSample = sig[i];
	  }
	} // bunches

	if (nsamples > 0) { // this check is needed for when we have zero-supp. on, but not sparse readout

	// indices
	Int_t mod = in.GetModule();
	Int_t col = in.GetColumn();
	Int_t row = in.GetRow();
	Int_t evtbin = iEvent/nProfFreq;
	Bool_t HighGainFlag = in.IsHighGain();
	
	//Check if histogram/profile already exist, if not create it.
	if(!fAmpHisto[mod][col][row]) {
	  snprintf(hname,buffersize,"mod%dcol%drow%d",mod,col,row);
	  fAmpHisto[mod][col][row] = new TH1F(hname,hname,1024,-0.5,1023.);
	}
	if(!fAmpProf[mod]) {
	  snprintf(hname,buffersize,"mod%d",mod);
	  fAmpProf[mod] = new TProfile(hname,hname,nEvtBins,0.,nEvtBins);
	}
		
	//Fill histogram/profile 
	if(HighGainFlag) {
	  fAmpHisto[mod][col][row]->Fill(maxSample);
	  fAmpProf[mod]->Fill(evtbin, maxSample);
	}

      } // nsamples>0 check, some data found for this channel; not only trailer/header

      } // channels
    } // DDL's

    // update histograms in local file every 100th event
    if(iEvent%fUpdatingRate == 0) {
      AliInfo(Form("Updating histo file, event %d, run %d\n",iEvent,runNum));
      UpdateHistoFile();
    } 
    iEvent++;
  }

  UpdateHistoFile(); 
  AliInfo(Form("%d events of run %d processed.",iEvent,runNum));
}

//-----------------------------------------------------------------------------
void AliEMCALCalibHistoProducer::UpdateHistoFile()
{
  // Write histograms to file

  if(!fHistoFile) return;
  if(!fHistoFile->IsOpen()) return;

  TH1F* hist=0;
  TProfile* prof =0;
 
  Int_t nRow =  fNCellsPhi ;
  for(Int_t supermodule=0; supermodule<fNSuperModules; supermodule++) {
    
    prof = fAmpProf[supermodule]; 
    if(prof) prof->Write(prof->GetName(),TObject::kWriteDelete);
    
    if(supermodule > 10)  nRow = fNCellsPhiHalfSM ; //Supermodules 11 and 12 are half supermodules
    for(Int_t column=0; column<fNCellsEta; column++) {
      for(Int_t row=0; row<nRow; row++) {
	hist = fAmpHisto[supermodule][column][row]; 
	if(hist) hist->Write(hist->GetName(),TObject::kWriteDelete);
      }
    }
  }
  
}
