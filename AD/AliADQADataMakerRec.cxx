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


//  Produces the data needed to calculate the quality assurance 
//  All data must be mergeable objects
//  Handles ESDs and Raws
//  Histos defined will be used for Raw Data control and monitoring

// --- ROOT system ---
#include <TClonesArray.h>
#include <TFile.h> 
#include <TF1.h> 
#include <TH1F.h> 
#include <TH1I.h> 
#include <TH2I.h> 
#include <TH2F.h> 
#include <TGraph.h> 
#include <TParameter.h>
#include <TTimeStamp.h>

// --- Standard library ---

// --- AliRoot header files ---
#include "AliESDEvent.h"
#include "AliLog.h"
#include "AliCDBManager.h"
#include "AliCDBStorage.h"
#include "AliCDBEntry.h"
#include "AliADQADataMakerRec.h"
#include "AliQAChecker.h"
#include "AliRawReader.h"
#include "AliADRawStream.h"
#include "AliADdigit.h"
#include "AliADConst.h"
#include "AliADReconstructor.h"
//#include "AliADTrending.h"
#include "AliADCalibData.h"
#include "AliCTPTimeParams.h"
#include "event.h"

ClassImp(AliADQADataMakerRec)
           
//____________________________________________________________________________ 
AliADQADataMakerRec::AliADQADataMakerRec() : 
AliQADataMakerRec(AliQAv1::GetDetName(AliQAv1::kAD), "AD Quality Assurance Data Maker"),
  fCalibData(0x0),
  fTrendingUpdateTime(0), 
  fCycleStartTime(0), 
  fCycleStopTime(0),
  fTimeSlewing(0)
    
{
  // Constructor
   
  AliDebug(AliQAv1::GetQADebugLevel(), "Construct AD QA Object");

  for(Int_t i=0; i<16; i++){  
    fEven[i] = 0;   
    fOdd[i]  = 0;
  }
  
  for(Int_t i=0; i<32; i++){  
    fADCmean[i] = 0.0;   }	
}

//____________________________________________________________________________ 
AliADQADataMakerRec::AliADQADataMakerRec(const AliADQADataMakerRec& qadm) :
  AliQADataMakerRec(),
  fCalibData(0x0),
  fTrendingUpdateTime(0), 
  fCycleStartTime(0), 
  fCycleStopTime(0),
  fTimeSlewing(0)
  
{
  // Copy constructor 
  
  SetName((const char*)qadm.GetName()) ; 
  SetTitle((const char*)qadm.GetTitle()); 
}

//__________________________________________________________________
AliADQADataMakerRec& AliADQADataMakerRec::operator = (const AliADQADataMakerRec& qadm )
{
  // Equal operator
  
  this->~AliADQADataMakerRec();
  new(this) AliADQADataMakerRec(qadm);
  return *this;
}

//____________________________________________________________________________
AliADCalibData* AliADQADataMakerRec::GetCalibData() const

{
  AliCDBManager *man = AliCDBManager::Instance();

  AliCDBEntry *entry=0;

  entry = man->Get("AD/Calib/Data",fRun);
  if(!entry){
    AliWarning("Load of calibration data from default storage failed!");
    AliWarning("Calibration data will be loaded from local storage ($ALICE_ROOT)");
	
    man->SetDefaultStorage("local://$ALICE_ROOT/OCDB");
    entry = man->Get("AD/Calib/Data",fRun);
  }
  // Retrieval of data in directory AD/Calib/Data:

  AliADCalibData *calibdata = 0;

  if (entry) calibdata = (AliADCalibData*) entry->GetObject();
  if (!calibdata)  AliFatal("No calibration data from calibration database !");

  return calibdata;
}
//____________________________________________________________________________ 
void AliADQADataMakerRec::StartOfDetectorCycle()
{
  // Detector specific actions at start of cycle
  
  // Reset of the histogram used - to have the trend versus time -
 
  fCalibData = GetCalibData();
 
  AliCDBEntry *entry = AliCDBManager::Instance()->Get("GRP/CTP/CTPtiming");
  if (!entry) AliFatal("CTP timing parameters are not found in OCDB !");
  AliCTPTimeParams *ctpParams = (AliCTPTimeParams*)entry->GetObject();
  Float_t l1Delay = (Float_t)ctpParams->GetDelayL1L0()*25.0;

  AliCDBEntry *entry1 = AliCDBManager::Instance()->Get("GRP/CTP/TimeAlign");
  if (!entry1) AliFatal("CTP time-alignment is not found in OCDB !");
  AliCTPTimeParams *ctpTimeAlign = (AliCTPTimeParams*)entry1->GetObject();
  l1Delay += ((Float_t)ctpTimeAlign->GetDelayL1L0()*25.0);
  /*/
  AliCDBEntry *entry2 = AliCDBManager::Instance()->Get("AD/Calib/TimeDelays");
  if (!entry2) AliFatal("AD time delays are not found in OCDB !");
  TH1F *delays = (TH1F*)entry2->GetObject();

  AliCDBEntry *entry3 = AliCDBManager::Instance()->Get("AD/Calib/TimeSlewing");
  if (!entry3) AliFatal("AD time slewing function is not found in OCDB !");
  fTimeSlewing = (TF1*)entry3->GetObject();
  /*/

  for(Int_t i = 0 ; i < 16; ++i) {
    //Int_t board = AliADCalibData::GetBoardNumber(i);
    fTimeOffset[i] = (
		      //	((Float_t)fCalibData->GetTriggerCountOffset(board) -
		      //	(Float_t)fCalibData->GetRollOver(board))*25.0 +
		      //     fCalibData->GetTimeOffset(i) -
		      //     l1Delay+
		      //delays->GetBinContent(i+1)//+
		      //      kADOffset
		      0
		      );
    //		      AliInfo(Form(" fTimeOffset[%d] = %f  kADoffset %f",i,fTimeOffset[i],kADOffset));
  }

 
 
  
 	
  TTimeStamp currentTime;
  fCycleStartTime = currentTime.GetSec();
 
  //  fNTotEvents = 0;
}
//____________________________________________________________________________ 
void AliADQADataMakerRec::EndOfDetectorCycle(AliQAv1::TASKINDEX_t task, TObjArray ** list)
{
  // Detector specific actions at end of cycle
  // Does the QA checking
  ResetEventTrigClasses();
  //
  
  
  if(task == AliQAv1::kRAWS){
    TTimeStamp currentTime;
    fCycleStopTime = currentTime.GetSec();
  }

  for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) {
    if (! IsValidEventSpecie(specie, list)) continue ;
    SetEventSpecie(AliRecoParam::ConvertIndex(specie));
    if(task == AliQAv1::kRAWS) {
    } else if (task == AliQAv1::kESDS) {
    }
  }
  AliQAChecker::Instance()->Run(AliQAv1::kAD, task, list) ;
}

//____________________________________________________________________________ 
void AliADQADataMakerRec::InitESDs()
{
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 

  TH1I * h0 = new TH1I("H1I_Cell_Multiplicity_ADA", "Cell Multiplicity in ADA;Multiplicity (Nb of Cell);Counts", 35, 0, 35) ;  
  Add2ESDsList(h0, kCellMultiADA, !expert, image)  ;  
                                                                                                        
  TH1I * h1 = new TH1I("H1I_Cell_Multiplicity_ADC", "Cell Multiplicity in AD;Multiplicity (Nb of Cell);Counts", 35, 0, 35) ;  
  Add2ESDsList(h1, kCellMultiADC, !expert, image)  ;  
  
  TH1F * h2 = new TH1F("H1D_BBFlag_Counters", "BB Flag Counters;Channel;Counts",16, 0, 16) ;  
  Add2ESDsList(h2, kBBFlag, !expert, image)  ;  
  
  TH1F * h3 = new TH1F("H1D_BGFlag_Counters", "BG Flag Counters;Channel;Counts",16, 0, 16) ;  
  Add2ESDsList(h3, kBGFlag, !expert, image)  ;  
  
  TH2F * h4 = new TH2F("H2D_Charge_Channel", "ADC Charge per channel;Channel;Charge (ADC counts)",16, 0, 16, 1024, 0, 1024) ;  
  Add2ESDsList(h4, kChargeChannel, !expert, image)  ;  
  
  TH2F * h5 = new TH2F("H2D_Time_Channel", "Time per channel;Channel;Time (ns)",16, 0, 16, 400, -100, 100) ;  
  Add2ESDsList(h5, kTimeChannel, !expert, image)  ;  
  
  TH1F * h6 = new TH1F("H1D_ADA_Time", "Mean ADA Time;Time (ns);Counts",1000, -100., 100.);
  Add2ESDsList(h6,kESDADATime, !expert, image); 
  
  TH1F * h7 = new TH1F("H1D_ADC_Time", "Mean ADC Time;Time (ns);Counts",1000, -100., 100.);
  Add2ESDsList(h7,kESDADCTime, !expert, image); 
  
  TH1F * h8 = new TH1F("H1D_Diff_Time", "Diff Time ADA - ADC;Diff Time ADA - ADC (ns);Counts",1000, -200., 200.);
  Add2ESDsList(h8,kESDDiffTime, !expert, image); 
  //
  ClonePerTrigClass(AliQAv1::kESDS); // this should be the last line	
}

//____________________________________________________________________________ 
void AliADQADataMakerRec::InitDigits()
{
// create Digits histograms in Digits subdir
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 

  // create Digits histograms in Digits subdir
  TH1I * h0 = new TH1I("hDigitMultiplicity", "Digits multiplicity distribution in AD;# of Digits;Entries", 17,-0.5,16.5) ; 
  h0->Sumw2() ;
  Add2DigitsList(h0, 0, !expert, image) ;
     
  TH2D * h1 = new TH2D("hDigitLeadingTimePerPM", "Leading time distribution per PM in AD;PM number;Leading Time [ns]",16,0,16, 1000, 200, 300); 
  h1->Sumw2() ;
  Add2DigitsList(h1, 1, !expert, image) ; 
  
  TH2D * h2 = new TH2D("hDigitTimeWidthPerPM", "Time width distribution per PM in AD;PM number;Time width [ns]",16,0,16, 1000, 0, 100); 
  h2->Sumw2() ;
  Add2DigitsList(h2, 2, !expert, image) ;
  
  TH2I * h3 = new TH2I("hDigitChargePerClockPerPM", "Charge array per PM in AD;PM number; Clock",16,0,16,21, -10.5, 10.5);
  h3->Sumw2();
  Add2DigitsList(h3, 3, !expert, image) ;
  
  TH1I * h4 = new TH1I("hDigitBBflagsAD","Number of BB flags in AD; # of BB flags; Entries",17,-0.5,16.5);
  h4->Sumw2();
  Add2DigitsList(h4, 4, !expert, image) ;
  
  TH1I * h5 = new TH1I("hDigitBBflagsADA","Number of BB flags in ADA; # of BB flags; Entries",9,-0.5,8.5);
  h5->Sumw2();
  Add2DigitsList(h5, 5, !expert, image) ;
  
  TH1I * h6 = new TH1I("hDigitBBflagsADC","Number of BB flags in ADC; # of BB flags; Entries",9,-0.5,8.5);
  h6->Sumw2();
  Add2DigitsList(h6, 6, !expert, image) ;
  
  TH2D * h7 = new TH2D("hDigitTotalChargePerPM", "Total Charge per PM in AD;PM number; Charge [ADC counts]",16,0,16,10000,0,10000);
  h7->Sumw2();
  Add2DigitsList(h7, 7, !expert, image) ;
  
  TH2I * h8 = new TH2I("hDigitMaxChargeClockPerPM", "Clock with maximum charge per PM in AD;PM number; Clock ",16,0,16,21, -10.5, 10.5);
  h8->Sumw2();
  Add2DigitsList(h8, 8, !expert, image) ;
   
  //
  ClonePerTrigClass(AliQAv1::kDIGITS); // this should be the last line
}

//____________________________________________________________________________
void AliADQADataMakerRec::MakeDigits()
{
 // makes data from Digits

  FillDigitsData(0,fDigitsArray->GetEntriesFast()) ; 
  TIter next(fDigitsArray) ; 
    AliADdigit *ADDigit ; 
    Int_t nBBflagsADA = 0;
    Int_t nBBflagsADC = 0;
    
    while ( (ADDigit = dynamic_cast<AliADdigit *>(next())) ) {
         Int_t totCharge = 0;
         Int_t   PMNumber  = ADDigit->PMNumber();
	 if(PMNumber<8 && ADDigit->GetBBflag()) nBBflagsADC++;
	 if(PMNumber>7 && ADDigit->GetBBflag()) nBBflagsADA++;
	 
	 Short_t adc[21];
	 for(Int_t iClock=0; iClock<21; iClock++) { 
	 adc[iClock]= ADDigit->ChargeADC(iClock);
	 FillDigitsData(3, PMNumber,(float)iClock-10,(float)adc[iClock]);
	 totCharge += adc[iClock];
	 }
	    
         FillDigitsData(1,PMNumber,ADDigit->Time()); 
	 FillDigitsData(2,PMNumber,ADDigit->Width());
	 FillDigitsData(7,PMNumber,totCharge);
	 FillDigitsData(8,PMNumber,TMath::LocMax(21,adc)-10); 
	 
    }
    FillDigitsData(4,nBBflagsADA+nBBflagsADC);
    FillDigitsData(5,nBBflagsADA);
    FillDigitsData(6,nBBflagsADC);  
}

//____________________________________________________________________________
void AliADQADataMakerRec::MakeDigits(TTree* digitTree)
{
  // makes data from Digit Tree
	
  if (fDigitsArray)
    fDigitsArray->Clear() ; 
  else 
    fDigitsArray = new TClonesArray("AliADdigit", 1000) ; 

    TBranch * branch = digitTree->GetBranch("ADDigit") ;
    if ( ! branch ) {
         AliWarning("AD branch in Digit Tree not found") ; 
    } else {
         branch->SetAddress(&fDigitsArray) ;
         branch->GetEntry(0) ; 
         MakeDigits() ; 
    }  
    //
    IncEvCountCycleDigits();
    IncEvCountTotalDigits();
    //    
}


//____________________________________________________________________________
void AliADQADataMakerRec::MakeESDs(AliESDEvent* esd)
{
// Creates QA data from ESDs
  
  UInt_t eventType = esd->GetEventType();

  switch (eventType){
  case PHYSICS_EVENT:
    AliESDAD *esdAD=esd->GetADData();
   
    if (!esdAD) break;
		  
    FillESDsData(kCellMultiADA,esdAD->GetNbPMADA());
    FillESDsData(kCellMultiADC,esdAD->GetNbPMADC());   
    	
    for(Int_t i=0;i<16;i++) {
      FillESDsData(kChargeChannel,(Float_t) i,(Float_t) esdAD->GetAdc(i));
      if (i < 8) {
	if(esdAD->BBTriggerADC(i)) FillESDsData(kBBFlag,(Float_t) i);
	if(esdAD->BGTriggerADC(i)) FillESDsData(kBGFlag,(Float_t) i);
      }
      else {
	if(esdAD->BBTriggerADA(i-8)) FillESDsData(kBBFlag,(Float_t) i);  
	if(esdAD->BGTriggerADA(i-8)) FillESDsData(kBGFlag,(Float_t) i);
      }		  	
      Float_t time = (Float_t) esdAD->GetTime(i);
      FillESDsData(kTimeChannel,(Float_t) i,time);
    }
				
    Float_t timeADA = esdAD->GetADATime();
    Float_t timeADC = esdAD->GetADCTime();
    Float_t diffTime;

    if(timeADA<-1024.+1.e-6 || timeADC<-1024.+1.e-6) diffTime = -1024.;
    else diffTime = timeADA - timeADC;

    FillESDsData(kESDADATime,timeADA);
    FillESDsData(kESDADCTime,timeADC);
    FillESDsData(kESDDiffTime,diffTime);
		
    break;
  }  
  //
  IncEvCountCycleESDs();
  IncEvCountTotalESDs();  
  // 
}

//____________________________________________________________________________ 
void AliADQADataMakerRec::InitRaws()
{
  // Creates RAW histograms in Raws subdir

  const Bool_t expert   = kTRUE ; 
  const Bool_t saveCorr = kTRUE ; 
  const Bool_t image    = kTRUE ; 

  const Int_t kNintegrator  =    2;
 
  const Int_t kNTdcTimeBins  = 1280;
  const Float_t kTdcTimeMin    =    0.;
  const Float_t kTdcTimeMax    = 75.;
    const Int_t kNTdcWidthBins =  256;
  const Float_t kTdcWidthMin   =    0;
  const Float_t kTdcWidthMax   =  200.;
  const Int_t kNChargeBins   = 1024;
  const Float_t kChargeMin     =    0;
  const Float_t kChargeMax     = 1024;
  const Int_t kNChannelBins  =   16;
  const Float_t kChannelMin    =    0;
  const Float_t kChannelMax    =   16;
  const Int_t kNPedestalBins =  200;
  const Float_t kPedestalMin   =    0;
  const Float_t kPedestalMax   =  200; 
  const Int_t kNPairBins  =   8;
  const Float_t kPairMin    =    0;
  const Float_t kPairMax    =   8;

  TH2I * h2i;
  TH2F * h2d;
  TH1I * h1i;
  TH1F * h1d;

  int iHisto =0;

  // Creation of Cell Multiplicity Histograms
  h1i = new TH1I("H1I_Multiplicity_ADA", "Number of fired cells in ADA;# of Cells;Entries", 10, 0, 10) ;  
  Add2RawsList(h1i,kMultiADA, !expert, image, saveCorr);   iHisto++;
  h1i = new TH1I("H1I_Multiplicity_ADC", "Number of fired cells in ADC;# of Cells;Entries", 10, 0, 10) ;  
  Add2RawsList(h1i,kMultiADC, !expert, image, saveCorr);   iHisto++;
 
  // Creation of Total Charge Histograms
  h1d = new TH1F("H1D_Charge_ADA", "Total Charge in ADA;Charge [ADC counts];Counts", 4000, 0, 30000) ;  
  Add2RawsList(h1d,kChargeADA, !expert, image, saveCorr);   iHisto++;
  h1d = new TH1F("H1D_Charge_ADC", "Total Charge in ADC;Charge [ADC counts];Counts", 4000, 0, 50000) ;  
  Add2RawsList(h1d,kChargeADC, !expert, image, saveCorr);   iHisto++;
  h1d = new TH1F("H1D_Charge_AD", "Total Charge in AD;Charge [ADC counts];Counts", 4000, 0, 80000) ;  
  Add2RawsList(h1d,kChargeAD, !expert,  image, saveCorr);   iHisto++;
   

  // Creation of Charge EoI histogram 
  h2d = new TH2F("H2D_ChargeEoI", "Charge Event of Interest;Channel Number;Charge [ADC counts]"
		 ,kNChannelBins, kChannelMin, kChannelMax, kNChargeBins, kChargeMin, 2.*kChargeMax);
  Add2RawsList(h2d,kChargeEoI, !expert, image, saveCorr); iHisto++;

  for(Int_t iInt=0;iInt<kNintegrator;iInt++){
    // Creation of Pedestal histograms 
    h2i = new TH2I(Form("H2I_Pedestal_Int%d",iInt), Form("Pedestal (Int%d);Channel;Pedestal [ADC counts]",iInt)
		   ,kNChannelBins, kChannelMin, kChannelMax,kNPedestalBins,kPedestalMin ,kPedestalMax );
    Add2RawsList(h2i,(iInt == 0 ? kPedestalInt0 : kPedestalInt1), !expert, image, saveCorr); iHisto++;
	

    // Creation of Charge EoI histograms 
    h2i = new TH2I(Form("H2I_ChargeEoI_Int%d",iInt), Form("Charge EoI (Int%d);Channel;Charge [ADC counts]",iInt)
		   ,kNChannelBins, kChannelMin, kChannelMax, kNChargeBins, kChargeMin, kChargeMax);
    Add2RawsList(h2i,(iInt == 0 ? kChargeEoIInt0 : kChargeEoIInt1), !expert, image, saveCorr); iHisto++;
    
  }	
  
  // Creation of Time histograms 
  h2i = new TH2I("H2I_Width", "HPTDC Width;Channel;Width [ns]",kNChannelBins, kChannelMin, kChannelMax, kNTdcWidthBins, kTdcWidthMin, kTdcWidthMax);
  Add2RawsList(h2i,kWidth, !expert, image, saveCorr); iHisto++;
  
  h2i = new TH2I("H2I_Width_BB", "HPTDC Width w/ BB Flag condition;Channel;Width [ns]",kNChannelBins, kChannelMin, kChannelMax, kNTdcWidthBins, kTdcWidthMin, kTdcWidthMax);
  Add2RawsList(h2i,kWidthBB, !expert, image, saveCorr); iHisto++;

  h2i = new TH2I("H2I_Width_BG", "HPTDC Width w/ BG Flag condition;Channel;Width [ns]",kNChannelBins, kChannelMin, kChannelMax, kNTdcWidthBins, kTdcWidthMin, kTdcWidthMax);
  Add2RawsList(h2i,kWidthBG, !expert, image, saveCorr); iHisto++;

  h2i = new TH2I("H2I_HPTDCTime", "HPTDC Time;Channel;Leading Time [ns]",kNChannelBins, kChannelMin, kChannelMax, kNTdcTimeBins, kTdcTimeMin, kTdcTimeMax);
  Add2RawsList(h2i,kHPTDCTime, !expert, image, saveCorr); iHisto++;
  
  h2i = new TH2I("H2I_HPTDCTime_BB", "HPTDC Time w/ BB Flag condition;Channel;Leading Time [ns]",kNChannelBins, kChannelMin, kChannelMax, kNTdcTimeBins, kTdcTimeMin, kTdcTimeMax);
  Add2RawsList(h2i,kHPTDCTimeBB, !expert, image, !saveCorr); iHisto++;

  h2i = new TH2I("H2I_HPTDCTime_BG", "HPTDC Time w/ BG Flag condition;Channel;Leading Time [ns]",kNChannelBins, kChannelMin, kChannelMax, kNTdcTimeBins, kTdcTimeMin, kTdcTimeMax);
  Add2RawsList(h2i,kHPTDCTimeBG, !expert, image, !saveCorr); iHisto++;
	
  h1d = new TH1F("H1D_ADA_Time", "ADA Time;Time [ns];Counts",kNTdcTimeBins, kTdcTimeMin, kTdcTimeMax);
  Add2RawsList(h1d,kADATime, !expert, image, saveCorr); iHisto++;
	
  h1d = new TH1F("H1D_ADC_Time", "ADC Time;Time [ns];Counts",kNTdcTimeBins, kTdcTimeMin, kTdcTimeMax);
  Add2RawsList(h1d,kADCTime, !expert, image, saveCorr); iHisto++;
	
  h1d = new TH1F("H1D_Diff_Time","Diff ADA-ADC Time;Time [ns];Counts",kNTdcTimeBins, -50., 50.);
  Add2RawsList(h1d,kDiffTime, !expert, image, saveCorr); iHisto++;

  h2d = new TH2F("H2D_TimeADA_ADC", "Mean Time in ADC versus ADA;Time ADA [ns];Time ADC [ns]", kNTdcTimeBins/8, kTdcTimeMin,kTdcTimeMax,kNTdcTimeBins/8, kTdcTimeMin,kTdcTimeMax) ;  
  Add2RawsList(h2d,kTimeADAADC, !expert, image, saveCorr);   iHisto++;
  
  //Creation of pair coincidence histograms
  h1i = new TH1I("H1I_MultiCoincidence_ADA", "Number of coincidences in ADA;# of Coincidences;Entries", 5, 0, 5) ;  
  Add2RawsList(h1i,kNCoincADA, !expert, image, saveCorr);   iHisto++;
  h1i = new TH1I("H1I_MultiCoincidence_ADC", "Number of coincidences in ADC;# of Coincidences;Entries", 5, 0, 5) ;  
  Add2RawsList(h1i,kNCoincADC, !expert, image, saveCorr);   iHisto++;
  
  h2d = new TH2F("H2D_Pair_Diff_Time","Diff Pair Time;Pair number;Time [ns]",kNPairBins, kPairMin, kPairMax,kNTdcTimeBins, -50., 50.);
  Add2RawsList(h2d,kPairDiffTime, !expert, image, saveCorr); iHisto++;
  
  h2d = new TH2F("H2D_Pair_Diff_Charge","Diff Pair Charge;Pair number;Charge [ADC counts]",kNPairBins, kPairMin, kPairMax, 2*kNChargeBins, -kChargeMax, kChargeMax);
  Add2RawsList(h2d,kPairDiffCharge, !expert, image, saveCorr); iHisto++;

  //Creation of Clock histograms
  h2d = new TH2F("H2D_BBFlagVsClock", "BB-Flags Versus LHC-Clock;Channel;LHC Clocks",kNChannelBins, kChannelMin, kChannelMax,21, -10.5, 10.5 );
  Add2RawsList(h2d,kBBFlagVsClock, !expert, image, saveCorr); iHisto++;
	
  h2d = new TH2F("H2D_BGFlagVsClock", "BG-Flags Versus LHC-Clock;Channel;LHC Clocks",kNChannelBins, kChannelMin, kChannelMax,21, -10.5, 10.5 );
  Add2RawsList(h2d,kBGFlagVsClock, !expert, image, saveCorr); iHisto++;

  for(Int_t iInt=0;iInt<kNintegrator;iInt++){
  	h2d = new TH2F(Form("H2D_ChargeVsClock_Int%d",iInt), Form("Charge Versus LHC-Clock (Int%d);Channel;LHCClock;Charge [ADC counts]",iInt),kNChannelBins, kChannelMin, kChannelMax,21, -10.5, 10.5 );
  	Add2RawsList(h2d,(iInt == 0 ? kChargeVsClockInt0 : kChargeVsClockInt1 ), expert, !image, !saveCorr); iHisto++;
	}
  
  h2d = new TH2F("H2D_BBFlagPerChannel", "BB-Flags Versus Channel;Channel;BB Flags Count",kNChannelBins, kChannelMin, kChannelMax,22,-0.5,21.5);
  Add2RawsList(h2d,kBBFlagsPerChannel, !expert, image, saveCorr); iHisto++;
  
  h2d = new TH2F("H2D_BGFlagPerChannel", "BG-Flags Versus Channel;Channel;BG Flags Count",kNChannelBins, kChannelMin, kChannelMax,22,-0.5,21.5);
  Add2RawsList(h2d,kBGFlagsPerChannel, !expert, image, saveCorr); iHisto++;
  
  AliDebug(AliQAv1::GetQADebugLevel(), Form("%d Histograms has been added to the Raws List",iHisto));
  //
  ClonePerTrigClass(AliQAv1::kRAWS); // this should be the last line
}

//____________________________________________________________________________
void AliADQADataMakerRec::MakeRaws(AliRawReader* rawReader)
{
  // Fills histograms with Raws, computes average ADC values dynamically (pedestal subtracted)
                  
					  
  // Check id histograms already created for this Event Specie
  if ( ! GetRawsData(kPedestalInt0) )
    InitRaws() ;

  rawReader->Reset() ; 
  AliADRawStream* rawStream  = new AliADRawStream(rawReader); 
  if(!(rawStream->Next())) return;  
 
  eventTypeType eventType = rawReader->GetType();

  Int_t    mulADA = 0 ; 
  Int_t    mulADC = 0 ; 
  Double_t timeADA =0., timeADC = 0.;
  Double_t weightADA =0., weightADC = 0.;
  UInt_t   itimeADA=0, itimeADC=0;
  Double_t chargeADA=0., chargeADC=0.;

  Double_t diffTime=-100000.;
  
  Int_t	   pmulADA = 0;
  Int_t	   pmulADC = 0;
  Double_t pDiffTime =-100000.;

  
  switch (eventType){
  case PHYSICS_EVENT:
  
    //    fNTotEvents++; // Use framework counters instead

    Int_t  iFlag=0;
    Int_t  pedestal;
    Int_t  integrator[16];
    Bool_t flagBB[16];	 
    Bool_t flagBG[16];	 
    Float_t charge;
    Int_t  offlineCh;
    Float_t adc[16], time[16], width[16], timeCorr[16]; 
    Int_t  iPair=0;

    for(Int_t iChannel=0; iChannel<16; iChannel++) { // BEGIN : Loop over channels
		   
      offlineCh = kOfflineChannel[iChannel];
		   
      // Fill Pedestal histograms
	   
      for(Int_t j=15; j<21; j++) {
	if((rawStream->GetBGFlag(iChannel,j) || rawStream->GetBBFlag(iChannel,j))) iFlag++;
      }

      if(iFlag == 0){ //No Flag found
	for(Int_t j=15; j<21; j++){
	  pedestal= (Int_t) rawStream->GetPedestal(iChannel, j);
	  integrator[offlineCh] = rawStream->GetIntegratorFlag(iChannel, j);

	  FillRawsData((integrator[offlineCh] == 0 ? kPedestalInt0 : kPedestalInt1),offlineCh,pedestal);
	}
      }

      // Fill Charge EoI histograms
	   
      adc[offlineCh]    = 0.0;

      // Search for the maximum charge in the train of 21 LHC clocks 
      // regardless of the integrator which has been operated:
      Float_t maxadc = 0;
      Int_t imax = -1;
      Float_t adcPedSub[21];
      for(Int_t iClock=0; iClock<21; iClock++){
	Bool_t iIntegrator = rawStream->GetIntegratorFlag(iChannel,iClock);
	Int_t k = offlineCh+16*iIntegrator;

	adcPedSub[iClock] = rawStream->GetPedestal(iChannel,iClock) - fCalibData->GetPedestal(k);
	//				if(adcPedSub[iClock] <= GetRecoParam()->GetNSigmaPed()*fCalibData->GetSigma(k)) {
	if(adcPedSub[iClock] <= 2.*fCalibData->GetSigma(k)) {
	  adcPedSub[iClock] = 0;
	  continue;
	}
	//		   		if(iClock < GetRecoParam()->GetStartClock() || iClock > GetRecoParam()->GetEndClock()) continue;
	if(iClock < 8 || iClock > 12) continue;
	if(adcPedSub[iClock] > maxadc) {
	  maxadc = adcPedSub[iClock];
	  imax   = iClock;
	}
      }
      //printf(Form("Channel %d (online), %d (offline)\n",iChannel,j)); 
      if (imax != -1) {
	//		   		Int_t start = imax - GetRecoParam()->GetNPreClocks();
	Int_t start = imax - 2;
	if (start < 0) start = 0;
	//		   		Int_t end = imax + GetRecoParam()->GetNPostClocks();
	Int_t end = imax + 1;
	if (end > 20) end = 20;
	for(Int_t iClock = start; iClock <= end; iClock++) {
	  adc[offlineCh] += adcPedSub[iClock];
	}
      }
	
		
      Int_t iClock  = imax;
      charge = rawStream->GetPedestal(iChannel,iClock); // Charge at the maximum 

      integrator[offlineCh]    = rawStream->GetIntegratorFlag(iChannel,iClock);
      flagBB[offlineCh]	 = rawStream->GetBBFlag(iChannel, iClock);
      flagBG[offlineCh]	 = rawStream->GetBGFlag(iChannel,iClock );
      Int_t board = AliADCalibData::GetBoardNumber(offlineCh);
      time[offlineCh] = rawStream->GetTime(iChannel)*fCalibData->GetTimeResolution(board);
      width[offlineCh] = rawStream->GetWidth(iChannel)*fCalibData->GetWidthResolution(board);

      if (time[offlineCh] >= 1e-6) FillRawsData(kChargeEoI,offlineCh,adc[offlineCh]);

      FillRawsData((integrator[offlineCh] == 0 ? kChargeEoIInt0 : kChargeEoIInt1),offlineCh,charge);

      Float_t sigma = fCalibData->GetSigma(offlineCh+16*integrator[offlineCh]);
		  
      if((adc[offlineCh] > 2.*sigma) && !(time[offlineCh] <1.e-6)){ 
	if(offlineCh<8) {
	  mulADC++;
	  chargeADC += adc[offlineCh];
	  
	} else {
	  mulADA++;
	  chargeADA += adc[offlineCh];
	}
      }
		   
  
      // Fill HPTDC Time Histograms
      //timeCorr[offlineCh] = CorrectLeadingTime(offlineCh,time[offlineCh],adc[offlineCh]);
      timeCorr[offlineCh] = time[offlineCh];

      //const Float_t p1 = 2.50; // photostatistics term in the time resolution
      //const Float_t p2 = 3.00; // sleewing related term in the time resolution
      if(timeCorr[offlineCh]>-1024 + 1.e-6){
	//Float_t nphe = adc[offlineCh]*kChargePerADC/(fCalibData->GetGain(offlineCh)*TMath::Qe());
	Float_t timeErr = 1;
	/*/
	if (nphe>1.e-6) timeErr = TMath::Sqrt(kIntTimeRes*kIntTimeRes+
					      p1*p1/nphe+
					      p2*p2*(fTimeSlewing->GetParameter(0)*fTimeSlewing->GetParameter(1))*(fTimeSlewing->GetParameter(0)*fTimeSlewing->GetParameter(1))*
					      TMath::Power(adc[offlineCh]/fCalibData->GetCalibDiscriThr(offlineCh,kTRUE),2.*(fTimeSlewing->GetParameter(1)-1.))/
					      (fCalibData->GetCalibDiscriThr(offlineCh,kTRUE)*fCalibData->GetCalibDiscriThr(offlineCh,kTRUE)));/*/

	if (timeErr>1.e-6) {
	  if (offlineCh<8) {
	    itimeADC++;
	    timeADC += timeCorr[offlineCh]/(timeErr*timeErr);
	    weightADC += 1./(timeErr*timeErr);
	  }else{
	    itimeADA++;
	    timeADA += timeCorr[offlineCh]/(timeErr*timeErr);
	    weightADA += 1./(timeErr*timeErr);
	  }
	}
      }
      
      // Fill Flag and Charge Versus LHC-Clock histograms
      Int_t nbbFlag = 0;
      Int_t nbgFlag = 0;
      
      for(Int_t iEvent=0; iEvent<21; iEvent++){
	charge = rawStream->GetPedestal(iChannel,iEvent);
	Int_t intgr = rawStream->GetIntegratorFlag(iChannel,iEvent);
	Bool_t bbFlag	  = rawStream->GetBBFlag(iChannel, iEvent);
	Bool_t bgFlag	  = rawStream->GetBGFlag(iChannel,iEvent );
	if(bbFlag) nbbFlag++;
	if(bgFlag) nbgFlag++;
	
	FillRawsData((intgr == 0 ? kChargeVsClockInt0 : kChargeVsClockInt1 ), offlineCh,(float)iEvent-10,(float)charge);
	FillRawsData(kBBFlagVsClock, offlineCh,(float)iEvent-10,(float)bbFlag);
	FillRawsData(kBGFlagVsClock, offlineCh,(float)iEvent-10,(float)bgFlag);
	
      }
      FillRawsData(kBBFlagsPerChannel, offlineCh,nbbFlag);
      FillRawsData(kBGFlagsPerChannel, offlineCh,nbgFlag);
      
      FillRawsData(kHPTDCTime,offlineCh,timeCorr[offlineCh]);
      FillRawsData(kWidth,offlineCh,width[offlineCh]);
      //if(flagBB[offlineCh]) {
      if(nbbFlag > 0){
	FillRawsData(kHPTDCTimeBB,offlineCh,timeCorr[offlineCh]);
	FillRawsData(kWidthBB,offlineCh,width[offlineCh]);
      }
      //if(flagBG[offlineCh]) {
      if(nbgFlag > 0){
	FillRawsData(kHPTDCTimeBG,offlineCh,timeCorr[offlineCh]);
	FillRawsData(kWidthBG,offlineCh,width[offlineCh]);
      }
      

    }// END of Loop over channels
    
    for(Int_t iChannel=0; iChannel<4; iChannel++) {//Loop over pairs ADC
    	offlineCh = kOfflineChannel[iChannel];
	Float_t sigma = fCalibData->GetSigma(offlineCh+16*integrator[offlineCh]);
	Float_t sigma4 = fCalibData->GetSigma(offlineCh+4+16*integrator[offlineCh]);		
    	if( ((adc[offlineCh] > 2.*sigma) && !(time[offlineCh] <1.e-6)) && ((adc[offlineCh+4] > 2.*sigma4) && !(time[offlineCh+4] <1.e-6)) ){ 
		pmulADC++;
		if(timeCorr[offlineCh]<-1024.+1.e-6 || timeCorr[offlineCh+4]<-1024.+1.e-6) pDiffTime = -1024.;
		else pDiffTime = timeCorr[offlineCh+4] - timeCorr[offlineCh]; 
		FillRawsData(kPairDiffTime,iPair,pDiffTime);
		}
	FillRawsData(kPairDiffCharge,iPair,TMath::Abs(adc[offlineCh]-adc[offlineCh+4]));
	iPair++;
	}
    for(Int_t iChannel=8; iChannel<12; iChannel++) {//Loop over pairs ADA
    	offlineCh = kOfflineChannel[iChannel];
	Float_t sigma = fCalibData->GetSigma(offlineCh+16*integrator[offlineCh]);
	Float_t sigma4 = fCalibData->GetSigma(offlineCh+4+16*integrator[offlineCh]);
    	if( ((adc[offlineCh] > 2.*sigma) && !(time[offlineCh] <1.e-6)) && ((adc[offlineCh+4] > 2.*sigma4) && !(time[offlineCh+4] <1.e-6)) ){ 
		pmulADA++;
		if(timeCorr[offlineCh]<-1024.+1.e-6 || timeCorr[offlineCh+4]<-1024.+1.e-6) pDiffTime = -1024.;
		else pDiffTime = timeCorr[offlineCh+4] - timeCorr[offlineCh]; 
		FillRawsData(kPairDiffTime,iPair,pDiffTime);
		}
	FillRawsData(kPairDiffCharge,iPair,TMath::Abs(adc[offlineCh]-adc[offlineCh+4]));
	iPair++;	
	}
    FillRawsData(kNCoincADA,pmulADA);
    FillRawsData(kNCoincADC,pmulADC);
	
	
    if(weightADA>1.e-6) timeADA /= weightADA; 
    else timeADA = -1024.;
    if(weightADC>1.e-6) timeADC /= weightADC;
    else timeADC = -1024.;
    if(timeADA<-1024.+1.e-6 || timeADC<-1024.+1.e-6) diffTime = -1024.;
    else diffTime = timeADA - timeADC;
    

		
    FillRawsData(kADATime,timeADA);
    FillRawsData(kADCTime,timeADC);
    FillRawsData(kDiffTime,diffTime);
    FillRawsData(kTimeADAADC,timeADA,timeADC);

    FillRawsData(kMultiADA,mulADA);
    FillRawsData(kMultiADC,mulADC);

    FillRawsData(kChargeADA,chargeADA);
    FillRawsData(kChargeADC,chargeADC);
    FillRawsData(kChargeAD,chargeADA + chargeADC);
	    
    break;
  } // END of SWITCH : EVENT TYPE 
	
  TParameter<double> * p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kMultiADA)->GetName()))) ; 
  if (p) p->SetVal((double)mulADA) ; 

  p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kMultiADC)->GetName()))) ; 
  if (p) p->SetVal((double)mulADC) ;                     

  p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kChargeADA)->GetName()))) ; 
  if (p) p->SetVal((double)chargeADA) ; 

  p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kChargeADC)->GetName()))) ; 
  if (p) p->SetVal((double)chargeADC) ;                     

  p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kChargeAD)->GetName()))) ; 
  if (p) p->SetVal((double)(chargeADA + chargeADC)) ;                     
	                   	
  p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kADATime)->GetName()))) ; 
  if (p) p->SetVal((double)timeADA) ; 
	
  p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kADCTime)->GetName()))) ; 
  if (p) p->SetVal((double)timeADC) ;                     
	
  p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kDiffTime)->GetName()))) ; 
  if (p) p->SetVal((double)diffTime) ;                     
	
  delete rawStream; rawStream = 0x0;      
  //
  IncEvCountCycleRaws();
  IncEvCountTotalRaws();
  //
}

//____________________________________________________________________________ 
Float_t AliADQADataMakerRec::CorrectLeadingTime(Int_t /*i*/, Float_t time, Float_t /*adc*/) const
{
  // Correct the leading time
  // for slewing effect and
  // misalignment of the channels
  if (time < 1e-6) return -1024;

  // Channel alignment and general offset subtraction
  //  time -= fTimeOffset[i];
  //AliInfo(Form("time-offset %f", time));

  // In case of pathological signals
  //if (adc < 1e-6) return time;

  // Slewing correction
  //Float_t thr = fCalibData->GetCalibDiscriThr(i,kTRUE);
  //AliInfo(Form("adc %f thr %f dtime %f ", adc,thr,fTimeSlewing->Eval(adc/thr)));
  //time -= fTimeSlewing->Eval(adc/thr);

  return time;
}

