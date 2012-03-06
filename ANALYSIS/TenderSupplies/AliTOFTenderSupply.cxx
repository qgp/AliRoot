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


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// TOF tender: - load updated calibrations (for TOF and T0)
//             - set tofHeader if missing in ESDs (2010 data)
//             - re-apply PID 
//             
// Contacts: Pietro.Antonioli@bo.infn.it                                     //
//           Francesco.Noferini@bo.infn.it                                   //
///////////////////////////////////////////////////////////////////////////////
#include <TMath.h>
#include <TRandom.h>
#include <AliLog.h>
#include <AliESDEvent.h>
#include <AliESDtrack.h>
#include <AliESDInputHandler.h>
#include <AliAnalysisManager.h>
#include <AliESDpid.h>
#include <AliTender.h>

#include <AliTOFcalib.h>
#include <AliTOFT0maker.h>

#include <AliCDBManager.h>
#include <AliCDBEntry.h>
#include <AliT0CalibSeasonTimeShift.h>

#include "AliTOFTenderSupply.h"

ClassImp(AliTOFTenderSupply)

Float_t AliTOFTenderSupply::fgT0Aresolution = 75.;
Float_t AliTOFTenderSupply::fgT0Cresolution = 65.;

AliTOFTenderSupply::AliTOFTenderSupply() :
  AliTenderSupply(),
  fESDpid(0x0),
  fIsMC(kFALSE),
  fTimeZeroType(AliESDpid::kBest_T0),
  fCorrectExpTimes(kTRUE),
  fLHC10dPatch(kFALSE),
  fT0DetectorAdjust(kFALSE),
  fDebugLevel(0),
  fAutomaticSettings(kTRUE),
  fTOFCalib(0x0),
  fTOFT0maker(0x0),
  fTOFres(100.),
  fT0IntercalibrationShift(0)


{
  //
  // default ctor
  //
  fT0shift[0] = 0;
  fT0shift[1] = 0;
  fT0shift[2] = 0;
  fT0shift[3] = 0;
}

//_____________________________________________________
AliTOFTenderSupply::AliTOFTenderSupply(const char *name, const AliTender *tender) :
  AliTenderSupply(name,tender),
  fESDpid(0x0),
  fIsMC(kFALSE),
  fTimeZeroType(AliESDpid::kBest_T0),
  fCorrectExpTimes(kTRUE),
  fLHC10dPatch(kFALSE),
  fT0DetectorAdjust(kFALSE),
  fDebugLevel(0),
  fAutomaticSettings(kTRUE),
  fTOFCalib(0x0),
  fTOFT0maker(0x0),
  fTOFres(100.),
  fT0IntercalibrationShift(0)
 
{
  //
  // named ctor
  //

  fT0shift[0] = 0;
  fT0shift[1] = 0;
  fT0shift[2] = 0;
  fT0shift[3] = 0;
}

//_____________________________________________________
void AliTOFTenderSupply::Init()
{

  Bool_t tenderUnsupported = kFALSE;
  // Initialise TOF tender (this is called at each detected run change)
  AliLog::SetClassDebugLevel("AliTOFTenderSupply",10); 

  // Setup PID object, check for MC, set AliTOFcalib and TOFT0 maker conf
  Int_t run = fTender->GetRun();
  if (run == 0) return;                // to skip first init, when we don't have yet a run number

  if (fAutomaticSettings) {
    if (run<114737) {
      tenderUnsupported = kTRUE;
    }
    else if (run>=114737&&run<=117223) {      //period="LHC10B";
      fCorrectExpTimes=kTRUE;
      fLHC10dPatch=kFALSE;
      fTOFres=100.;
      fTimeZeroType=AliESDpid::kTOF_T0;
      fT0IntercalibrationShift = 0;
      fT0DetectorAdjust=kTRUE;
    }
    else if (run>=118503&&run<=121040) { //period="LHC10C";
      fCorrectExpTimes=kTRUE;
      fLHC10dPatch=kFALSE;
      fTOFres=100.;
      fTimeZeroType=AliESDpid::kTOF_T0;
      fT0IntercalibrationShift = 0;
      fT0DetectorAdjust=kFALSE;
    }
    else if (run>=122195&&run<=126437) { //period="LHC10D";
      fCorrectExpTimes=kFALSE;
      fLHC10dPatch=kTRUE;
      fTOFres=100.;
      fTimeZeroType=AliESDpid::kBest_T0;
      fT0IntercalibrationShift = 0;
      fT0DetectorAdjust=kTRUE;
    }
    else if (run>=127719&&run<=130850) { //period="LHC10E";
      fCorrectExpTimes=kFALSE;
      fLHC10dPatch=kFALSE;
      fTOFres=100.;
      fTimeZeroType=AliESDpid::kBest_T0;
      fT0IntercalibrationShift = 30.;
      fT0DetectorAdjust=kTRUE;
    }
    else if (run>=133004&&run<=135029) { //period="LHC10F";
      fCorrectExpTimes=kFALSE;
      fLHC10dPatch=kFALSE;
      fTOFres=100.;
      fTimeZeroType=AliESDpid::kBest_T0;
      fT0IntercalibrationShift = 0.;
      fT0DetectorAdjust=kTRUE;
      AliWarning("TOF tender not supported for LHC10F period!! Settings are just a guess!!");
    }
    else if (run>=135654&&run<=136377) { //period="LHC10G";
      fCorrectExpTimes=kFALSE;
      fLHC10dPatch=kFALSE;
      fTOFres=100.;
      fTimeZeroType=AliESDpid::kBest_T0;
      fT0IntercalibrationShift = 0.;
      fT0DetectorAdjust=kTRUE;
      AliWarning("TOF tender not supported for LHC10G period!! Settings are just a guess!!");
    }
    else if (run>=136851&&run<=139517) { //period="LHC10H" - pass2;
      fCorrectExpTimes=kFALSE;
      fLHC10dPatch=kFALSE;                
      fTOFres=90.;
      fTimeZeroType=AliESDpid::kTOF_T0;
      fT0IntercalibrationShift = 0.;
      fT0DetectorAdjust=kTRUE;
    }
    else if (run>=139699) {              //period="LHC11A";
      /*
      fCorrectExpTimes=kFALSE;
      fLHC10dPatch=kFALSE;
      fTOFres=100.;
      fTimeZeroType=AliESDpid::kBest_T0;
      fT0IntercalibrationShift = 0.;
      fT0DetectorAdjust=kFALSE;
      AliWarning("TOF tender not supported for LHC11A period!! Settings are just a guess!!");
      */
      AliError("TOF tender not supported for 2011 data!!!!!");
      tenderUnsupported = kTRUE;
    }
  }

  if (tenderUnsupported) {
    AliInfo(" |---------------------------------------------------------------------------|");
    AliInfo(" |                                                                           |");
    AliInfo(Form(" |  TOF tender is not supported for run %d                               |",run));
    AliInfo(" | You cannot use TOF tender for this run, your results can be spoiled       |");
    AliInfo(" | Check TOF tender usage for run/periods at:                                |");
    AliInfo(" |  https://twiki.cern.ch/twiki/bin/view/ALICE/TOF.                          |");
    AliInfo(" |---------------------------------------------------------------------------|");
    AliInfo(" ");
    AliFatal(" ------- TOF tender not to be used in this run, issuing FATAL error -------- ");
  }

  // Check if another detector already created the esd pid object
  // if not we create it and set it to the ESD input handler
  fESDpid=fTender->GetESDhandler()->GetESDpid();
  if (!fESDpid) {
    fESDpid=new AliESDpid;
    fTender->GetESDhandler()->SetESDpid(fESDpid);
  }

  // Even if the user didn't set fIsMC, we force it on if we find the MC handler 
  AliAnalysisManager *mgr=AliAnalysisManager::GetAnalysisManager();
  if (mgr->GetMCtruthEventHandler() && !(fIsMC) ) {
    AliWarning("This ESD is MC, fIsMC found OFF: fIsMC turned ON");
    fIsMC=kTRUE;
  }

  // Configure TOF calibration class
  if (!fTOFCalib)fTOFCalib=new AliTOFcalib();  // create if needed
  fTOFCalib->SetRemoveMeanT0(!(fIsMC));        // must be kFALSE on MC (default is kTRUE)
  fTOFCalib->SetCalibrateTOFsignal(!(fIsMC));  // must be kFALSE on MC (no new calibration) (default is kTRUE)
  fTOFCalib->SetCorrectTExp(fCorrectExpTimes); // apply a fine tuning on the expected times at low momenta
  

  // Configure TOFT0 maker class
  //  if (!fTOFT0maker) fTOFT0maker = new AliTOFT0maker(fESDpid,fTOFCalib); // create if needed
  if (!fTOFT0maker) fTOFT0maker = new AliTOFT0maker(fESDpid); // without passing AliTOFCalib it uses the diamond
  fTOFT0maker->SetTimeResolution(fTOFres);     // set TOF resolution for the PID
  

  AliInfo("|******************************************************|");
  AliInfo(Form("|    Alice TOF Tender Initialisation (Run %d)  |",fTender->GetRun()));
  AliInfo("|    Settings:                                         |");
  AliInfo(Form("|    Correct Exp Times              :  %d               |",fCorrectExpTimes));
  AliInfo(Form("|    LHC10d patch                   :  %d               |",fLHC10dPatch));
  AliInfo(Form("|    TOF resolution for TOFT0 maker :  %5.2f (ps)     |",fTOFres));
  AliInfo(Form("|    timeZero selection             :  %d               |",fTimeZeroType));
  AliInfo(Form("|    MC flag                        :  %d               |",fIsMC));
  AliInfo(Form("|    T0 detector offsets applied    :  %d               |",fT0DetectorAdjust));
  AliInfo(Form("|    TOF/T0 intecalibration shift   :  %5.2f (ps)     |",fT0IntercalibrationShift));
  AliInfo("|******************************************************|");


}

//_____________________________________________________
void AliTOFTenderSupply::ProcessEvent()
{
  //
  // Use updated calibrations for TOF and T0, reapply PID information
  // For MC: timeZero sampling and additional smearing for T0

  if (fDebugLevel > 1) AliInfo("process event");

  AliESDEvent *event=fTender->GetEvent();
  if (!event) return;
  if (fDebugLevel > 1) AliInfo("event read");


    
  if (fTender->RunChanged()){ 

    Init();            

    fTOFCalib->Init(fTender->GetRun());
    
    if(event->GetT0TOF()){ // read T0 detector correction from OCDB
      // OCDB instance
      if (fT0DetectorAdjust) {
	AliCDBManager* ocdbMan = AliCDBManager::Instance();
	ocdbMan->SetRun(fTender->GetRun());    
	AliCDBEntry *entry = ocdbMan->Get("T0/Calib/TimeAdjust/");
	if(entry) {
	  AliT0CalibSeasonTimeShift *clb = (AliT0CalibSeasonTimeShift*) entry->GetObject();
	  Float_t *t0means= clb->GetT0Means();
	  //      Float_t *t0sigmas = clb->GetT0Sigmas();
	  fT0shift[0] = t0means[0] + fT0IntercalibrationShift;
	  fT0shift[1] = t0means[1] + fT0IntercalibrationShift;
	  fT0shift[2] = t0means[2] + fT0IntercalibrationShift;
	  fT0shift[3] = t0means[3] + fT0IntercalibrationShift;
	} else {
	  for (Int_t i=0;i<4;i++) fT0shift[i]=0;
	  AliWarning("TofTender no T0 entry found T0shift set to 0");
	}
      } else {
	for (Int_t i=0;i<4;i++) fT0shift[i]=0;
      }
    }
  }


  fTOFCalib->CalibrateESD(event);   //recalculate TOF signal (no harm for MC, see settings inside init)
  if (fLHC10dPatch && !(fIsMC)) RecomputeTExp(event);

  Double_t startTime = 0.;
  if(fIsMC) startTime = fTOFCalib->TuneForMC(event,fTOFres);

  if (fDebugLevel > 1) Printf(" TofTender: startTime %f",startTime);
  if (fDebugLevel > 1) Printf(" TofTender: T0 time (orig) %f %f %f",event->GetT0TOF(0),event->GetT0TOF(1),event->GetT0TOF(2));
  // event by event TO detector treatment  
  if(event->GetT0TOF()){   // protection: we adjust T0 only if it is there....

    if (event->GetT0TOF(0) == 0) event->SetT0TOF(0, 9999999.); // in case no information we set to unknown
    if (event->GetT0TOF(1) == 0) event->SetT0TOF(1, 99999.);
    if (event->GetT0TOF(2) == 0) event->SetT0TOF(2, 99999.);

    if(!fIsMC){   // data: apply shifts to align around Zero
      event->SetT0TOF(0,event->GetT0TOF(0) - fT0shift[0]);
      event->SetT0TOF(1,event->GetT0TOF(1) - fT0shift[1]);
      event->SetT0TOF(2,event->GetT0TOF(2) - fT0shift[2]);
    } else {
      // MC: add smearing for realistic T0A and T0C resolution
      Double_t defResolutionT0A = 33.;   // in future we will get this from ESDrun data structure or via OCDB
      Double_t defResolutionT0C = 30.;   // for the moment we don't trust them
      if ( (fgT0Aresolution > defResolutionT0A) && (event->GetT0TOF(1)<90000.) ) { // add smearing only if signal is there
	Double_t addedSmearingT0A = TMath::Sqrt(fgT0Aresolution*fgT0Aresolution - defResolutionT0A*defResolutionT0A);
        Double_t smearingT0A = gRandom->Gaus(0.,addedSmearingT0A);
	event->SetT0TOF(1,event->GetT0TOF(1) + smearingT0A);
      }
      if ( (fgT0Cresolution > defResolutionT0C) && (event->GetT0TOF(2)<90000.) ) { // add smearing only if signal is there
	Double_t addedSmearingT0C = TMath::Sqrt(fgT0Cresolution*fgT0Cresolution - defResolutionT0C*defResolutionT0C);
	Double_t smearingT0C = gRandom->Gaus(0.,addedSmearingT0C);
        event->SetT0TOF(2,event->GetT0TOF(2) + smearingT0C);
      }
      if (event->GetT0TOF(0)<90000.) { // we recompute the AND only if it is already there...
	Double_t smearedT0AC = (event->GetT0TOF(1)+event->GetT0TOF(2))/2.;
	event->SetT0TOF(0,smearedT0AC); 
      }
      if (fDebugLevel > 1) Printf(" TofTender: T0 time (postSmear) %f %f %f",event->GetT0TOF(0),event->GetT0TOF(1),event->GetT0TOF(2));
      // add finally the timeZero offset also to the T0 detector information
      event->SetT0TOF(0,event->GetT0TOF(0) + startTime);
      event->SetT0TOF(1,event->GetT0TOF(1) + startTime);
      event->SetT0TOF(2,event->GetT0TOF(2) + startTime);  
      if (fDebugLevel > 1) Printf(" TofTender: T0 time (postStart) %f %f %f",event->GetT0TOF(0),event->GetT0TOF(1),event->GetT0TOF(2));
    }
    // after shifts adjust (data) or smearing+offset (MC) we 'clean' to default if signals not there 
    if(event->GetT0TOF(0) > 900000) event->SetT0TOF(0, 999999.);
    if(event->GetT0TOF(1) > 90000)  event->SetT0TOF(1, 99999.);
    if(event->GetT0TOF(2) > 90000)  event->SetT0TOF(2, 99999.);
  }
  if (fDebugLevel > 1) Printf(" TofTender: T0 time (FINAL) %f %f %f",event->GetT0TOF(0),event->GetT0TOF(1),event->GetT0TOF(2));
  
  //compute timeZero of the event via TOF-TO
  fTOFT0maker->ComputeT0TOF(event);
  fTOFT0maker->WriteInESD(event);

  // recalculate PID probabilities
  fESDpid->SetTOFResponse(event, (AliESDpid::EStartTimeType_t)fTimeZeroType);

  // this is for safety, especially if the user doesn't attach a PID tender after TOF tender  
  Int_t ntracks=event->GetNumberOfTracks();
  for(Int_t itrack = 0; itrack < ntracks; itrack++){
    fESDpid->MakeTOFPID(event->GetTrack(itrack),0);   
  }
  
  
}


//_____________________________________________________
void AliTOFTenderSupply::RecomputeTExp(AliESDEvent *event) const
{
  /*
   * calibrate TExp
   */

  
  /* loop over tracks */
  AliESDtrack *track = NULL;
  for (Int_t itrk = 0; itrk < event->GetNumberOfTracks(); itrk++) {
    /* get track and calibrate */
    track = event->GetTrack(itrk);
    RecomputeTExp(track);
  }
  
}

//_____________________________________________________
void AliTOFTenderSupply::RecomputeTExp(AliESDtrack *track) const
{
  /*** 
       THIS METHOD IS BASED ON THEORETICAL EXPECTED TIME COMPUTED
       USING AVERAGE MOMENTUM BETWEEN INNER/OUTER TRACK PARAMS 
       IT IS A ROUGH APPROXIMATION APPLIED TO FIX LHC10d-pass2 DATA
       WHERE A WRONG GEOMETRY (FULL TRD) WAS INSERTED
  ***/

  Double_t texp[AliPID::kSPECIES];
  if (!track || !(track->GetStatus() & AliESDtrack::kTOFout)) return;


  /* get track params */
  Float_t l = track->GetIntegratedLength();
  Float_t p = track->P();
  if (track->GetInnerParam() && track->GetOuterParam()) {
    Float_t pin = track->GetInnerParam()->P();
    Float_t pout = track->GetOuterParam()->P();
    p = 0.5 * (pin + pout);
  }
  /* loop over particle types and compute expected time */
  for (Int_t ipart = 0; ipart < AliPID::kSPECIES; ipart++)
    texp[ipart] = GetExpTimeTh(AliPID::ParticleMass(ipart), p, l) - 37.; 
  // 37 is a final semiempirical offset to further adjust (calibrations were
  // done with "standard" integratedTimes)
  /* set integrated times */
  track->SetIntegratedTimes(texp);

}


