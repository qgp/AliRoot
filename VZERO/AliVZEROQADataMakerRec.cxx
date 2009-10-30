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
#include <TH1F.h> 
#include <TH1I.h> 
#include <TH2I.h> 
#include <TH2D.h> 
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
#include "AliVZEROQADataMakerRec.h"
#include "AliQAChecker.h"
#include "AliRawReader.h"
#include "AliVZERORawStream.h"
#include "AliVZEROdigit.h"
#include "AliVZEROReconstructor.h"
#include "AliVZEROTrending.h"
#include "AliVZEROCalibData.h"
#include "event.h"


ClassImp(AliVZEROQADataMakerRec)
           
//____________________________________________________________________________ 
  AliVZEROQADataMakerRec::AliVZEROQADataMakerRec() : 
	AliQADataMakerRec(AliQAv1::GetDetName(AliQAv1::kVZERO), "VZERO Quality Assurance Data Maker"),
	fCalibData(0x0),
        fEvent(0), 
        fNTotEvents(0), 
        fNSubEvents(0), 
        fTrendingUpdateEvent(0), 
        fNTrendingUpdates(0), 
        fTrendingUpdateTime(0), 
        fCycleStartTime(0), 
        fCycleStopTime(0), 
        fMonitorRate(0.)
    
{
   // Constructor
   
      AliDebug(AliQAv1::GetQADebugLevel(), "Construct VZERO QA Object");
  for(Int_t i=0;i<8;i++){
  	fChargePerRing[i] = 0.;
  	fFlagPerRing[i] = 0.;
  }
   for(Int_t i=0; i<64; i++){  
       fEven[i] = 0;   
       fOdd[i]  = 0;
	   fChargePerChannel[i] = 0.;  
	   fFlagPerChannel[i] = 0.;  
	   fMeanChargePerChannel[i] = 0.;  
	   fMeanFlagPerChannel[i] = 0.;  
  }
  
   for(Int_t i=0; i<128; i++){  
       fADCmean[i] = 0.0;   }	
}

//____________________________________________________________________________ 
  AliVZEROQADataMakerRec::AliVZEROQADataMakerRec(const AliVZEROQADataMakerRec& qadm) :
  AliQADataMakerRec(),
	fCalibData(0x0),
        fEvent(0), 
        fNTotEvents(0), 
        fNSubEvents(0), 
        fTrendingUpdateEvent(0), 
        fNTrendingUpdates(0), 
        fTrendingUpdateTime(0), 
        fCycleStartTime(0), 
        fCycleStopTime(0), 
        fMonitorRate(0.)
  
{
  // Copy constructor 
  
   SetName((const char*)qadm.GetName()) ; 
   SetTitle((const char*)qadm.GetTitle()); 
}

//__________________________________________________________________
AliVZEROQADataMakerRec& AliVZEROQADataMakerRec::operator = (const AliVZEROQADataMakerRec& qadm )
{
  // Equal operator
  
  this->~AliVZEROQADataMakerRec();
  new(this) AliVZEROQADataMakerRec(qadm);
  return *this;
}

//____________________________________________________________________________
AliVZEROCalibData* AliVZEROQADataMakerRec::GetCalibData() const

{
   AliCDBManager *man = AliCDBManager::Instance();

   AliCDBEntry *entry=0;

   entry = man->Get("VZERO/Calib/Data",fRun);
   if(!entry){
	AliWarning("Load of calibration data from default storage failed!");
	AliWarning("Calibration data will be loaded from local storage ($ALICE_ROOT)");
	
	man->SetDefaultStorage("local://$ALICE_ROOT/OCDB");
	entry = man->Get("VZERO/Calib/Data",fRun);
   }
   // Retrieval of data in directory VZERO/Calib/Data:

   AliVZEROCalibData *calibdata = 0;

   if (entry) calibdata = (AliVZEROCalibData*) entry->GetObject();
   if (!calibdata)  AliFatal("No calibration data from calibration database !");

   return calibdata;
}


 
//____________________________________________________________________________ 
void AliVZEROQADataMakerRec::EndOfDetectorCycle(AliQAv1::TASKINDEX_t task, TObjArray ** list)
{
  // Detector specific actions at end of cycle
  // Does the QA checking
  
  AliQAChecker::Instance()->Run(AliQAv1::kVZERO, task, list) ;
  
  if(task == AliQAv1::kRAWS){
	TTimeStamp currentTime;
	fCycleStopTime = currentTime.GetSec();
	if(fCycleStopTime-fCycleStartTime>0.) fMonitorRate = fNTotEvents/(fCycleStopTime-fCycleStartTime);
	//printf("%d event have been monitored -> Monitoring Rate = %f Hz\n",fNTotEvents,fMonitorRate); 
	Bool_t update = kFALSE;
	if(!fTrendingUpdateEvent) {
		update = kTRUE;
	} else if ((TMath::Abs(fTrendingUpdateTime * fMonitorRate - fTrendingUpdateEvent) / fTrendingUpdateEvent) > 0.1){
		update = kTRUE;
	}
	if(update) fTrendingUpdateEvent = (size_t)(fTrendingUpdateTime * fMonitorRate);
	//printf("Update trending information every %d events\n",fTrendingUpdateEvent); 

  }

  for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) {
    if (! IsValidEventSpecie(specie, list)) 
      continue ;
    SetEventSpecie(AliRecoParam::ConvertIndex(specie)) ; 
    if(task == AliQAv1::kRAWS){
  	  int nMaxBin = GetRawsData(kPedestalTimeInt0)->GetNbinsY();
      if(fCurrentCycle%nMaxBin==0) {
        GetRawsData(kPedestalTimeInt0)->Reset();
        GetRawsData(kPedestalTimeInt1)->Reset();
        GetRawsData(kChargeEoITimeInt0)->Reset();
        GetRawsData(kChargeEoITimeInt1)->Reset();
      }
      TH1D* hProj;
      char name[50];
      for(Int_t iChannel=0; iChannel<64; iChannel++) {
        for(Int_t integrator=0;integrator<2;integrator++){
          sprintf(name,"Ped_%d_%d",iChannel,integrator);
          hProj = ((TH2I*)GetRawsData((integrator == 0 ? kPedestalCycleInt0 : kPedestalCycleInt1)))->ProjectionY(name,iChannel+1,iChannel+1);
          ((TH2D*)GetRawsData((integrator == 0 ? kPedestalTimeInt0 : kPedestalTimeInt1)))->Fill((double)iChannel,(double)(fCurrentCycle%nMaxBin),(double)hProj->GetMean());
          delete hProj;

          sprintf(name,"Charge_%d_%d",iChannel,integrator);
          hProj = ((TH2I*)GetRawsData((integrator == 0 ? kChargeEoICycleInt0 : kChargeEoICycleInt1)))->ProjectionY(name,iChannel+1,iChannel+1);
          ((TH2D*)GetRawsData((integrator == 0 ? kChargeEoITimeInt0 : kChargeEoITimeInt1)))->Fill((double)iChannel,(double)(fCurrentCycle%nMaxBin),hProj->GetMean());
          delete hProj;
        }
      }
    } else if (task == AliQAv1::kESDS) {
    }
  }
}

//____________________________________________________________________________ 
void AliVZEROQADataMakerRec::InitESDs()
{
  // Creates histograms to control ESDs
  
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 
	
  TH2D * h2d;
  TH1I * h1i;
  TH1D * h1d;
		
  h1i = new TH1I("H1I_Cell_Multiplicity_V0A", "Cell Multiplicity in V0A;Multiplicity (Nb of Cell);Counts", 35, 0, 35) ;  
  Add2ESDsList(h1i, kCellMultiV0A, !expert, image)  ;  
                                                                                                        
  h1i = new TH1I("H1I_Cell_Multiplicity_V0C", "Cell Multiplicity in V0;Multiplicity (Nb of Cell);Counts", 35, 0, 35) ;  
  Add2ESDsList(h1i, kCellMultiV0C, !expert, image)  ;  
   
  h1d = new TH1D("H1D_MIP_Multiplicity_V0A", "MIP Multiplicity in V0A;Multiplicity (Nb of MIP);Counts", 1000, 0, 1000) ;  
  Add2ESDsList(h1d, kMIPMultiV0A, !expert, image)  ;  
  
  h1d = new TH1D("H1D_MIP_Multiplicity_V0C", "MIP Multiplicity in V0C;Multiplicity (Nb of MIP);Counts", 1000, 0, 1000) ;  
  Add2ESDsList(h1d, kMIPMultiV0C, !expert, image)  ;  

  h2d = new TH2D("H2D_MIP_Multiplicity_Channel", "MIP Multiplicity per Channel;Channel;Multiplicity (Nb of MIP)",64, 0, 64, 100, 0, 100) ;  
  Add2ESDsList(h2d, kMIPMultiChannel, !expert, image)  ;  
  
  h1d = new TH1D("H1D_BBFlag_Counters", "BB Flag Counters;Channel;Counts",64, 0, 64) ;  
  Add2ESDsList(h1d, kBBFlag, !expert, image)  ;  
  
  h1d = new TH1D("H1D_BGFlag_Counters", "BG Flag Counters;Channel;Counts",64, 0, 64) ;  
  Add2ESDsList(h1d, kBGFlag, !expert, image)  ;  
  
  h2d = new TH2D("H2D_Charge_Channel", "ADC Charge per channel;Channel;Charge (ADC counts)",64, 0, 64, 1024, 0, 1024) ;  
  Add2ESDsList(h2d, kChargeChannel, !expert, image)  ;  
  
  h2d = new TH2D("H2D_Time_Channel", "Time per channel;Channel;Time (ns)",64, 0, 64, 820, 0, 410) ;  
  Add2ESDsList(h2d, kTimeChannel, !expert, image)  ;  
  
  h1d = new TH1D("H1D_V0A_Time", "Mean V0A Time;Time (ns);Counts",2048, 0., 409.6);
  Add2ESDsList(h1d,kESDV0ATime, !expert, image); 
  
  h1d = new TH1D("H1D_V0C_Time", "Mean V0C Time;Time (ns);Counts",2048, 0., 409.6);
  Add2ESDsList(h1d,kESDV0CTime, !expert, image); 
  
  h1d = new TH1D("H1D_Diff_Time", "Diff Time V0A - V0C;Diff Time V0A - V0C (ns);Counts",2*2048, -409.6, 409.6);
  Add2ESDsList(h1d,kESDDiffTime, !expert, image); 
	
}

//____________________________________________________________________________ 
 void AliVZEROQADataMakerRec::InitRaws()
 {
   // Creates RAW histograms in Raws subdir

   const Bool_t expert   = kTRUE ; 
   const Bool_t saveCorr = kTRUE ; 
   const Bool_t image    = kTRUE ; 

  char name[50] , title[100];
  const Int_t kNintegrator  =    2;
 
  const Int_t kNTdcTimeBins  = 2048;
  const Int_t kTdcTimeMin    =    0;
  const Int_t kTdcTimeMax    = 4096;
  const Int_t kNTdcWidthBins =  128;
  const Int_t kTdcWidthMin   =    0;
  const Int_t kTdcWidthMax   =  128;
  const Int_t kNChargeBins   = 1024;
  const Int_t kChargeMin     =    0;
  const Int_t kChargeMax     = 1024;
  const Int_t kNChannelBins  =   64;
  const Int_t kChannelMin    =    0;
  const Int_t kChannelMax    =   64;
  const Int_t kNPedestalBins =  200;
  const Int_t kPedestalMin   =    0;
  const Int_t kPedestalMax   =  200;
  const Int_t kTimeMin       =   0;
  const Int_t kTimeMax       = 100;
  const Int_t kNMIPBins      = 200;
  const Int_t kMIPMin        =   0;
  const Int_t kMIPMax        = 200;

  TH2I * h2i;
  TH2D * h2d;
  TH1I * h1i;
  TH1D * h1d;
  AliVZEROTrending * trend;

  int iHisto =0;
 
   // Creation of Cell Multiplicity Histograms
  h1i = new TH1I("H1I_Multiplicity_V0A", "Cell Multiplicity in V0A;# of Cells;Entries", 35, 0, 35) ;  
  Add2RawsList(h1i,kMultiV0A, expert, image, saveCorr);   iHisto++;
  h1i = new TH1I("H1I_Multiplicity_V0C", "Cell Multiplicity in V0C;# of Cells;Entries", 35, 0, 35) ;  
  Add2RawsList(h1i,kMultiV0C, expert, image, saveCorr);   iHisto++;
 
  // Creation of Total Charge Histograms
  h1d = new TH1D("H1D_Charge_V0A", "Total Charge in V0A;Charge [ADC counts];Counts", 2048, 0, 32768) ;  
  Add2RawsList(h1d,kChargeV0A, expert, image, saveCorr);   iHisto++;
  h1d = new TH1D("H1D_Charge_V0C", "Total Charge in V0C;Charge [ADC counts];Counts", 2048, 0, 32768) ;  
  Add2RawsList(h1d,kChargeV0C, expert, image, saveCorr);   iHisto++;
  h1d = new TH1D("H1D_Charge_V0", "Total Charge in V0;Charge [ADC counts];Counts", 2048, 0, 65536) ;  
  Add2RawsList(h1d,kChargeV0, expert, image, saveCorr);   iHisto++;
  
  // Creation of MIP Histograms
  h1d = new TH1D("H1D_MIP_V0A", "Total MIP in V0A;Charge [MIP];Counts", 2*kNMIPBins,kMIPMin ,32*kMIPMax) ;  
  Add2RawsList(h1d,kRawMIPV0A, expert, image, saveCorr);   iHisto++;
  h1d = new TH1D("H1D_MIP_V0C", "Total MIP in V0C;Charge [MIP];Counts", 2*kNMIPBins,kMIPMin ,32*kMIPMax) ;  
  Add2RawsList(h1d,kRawMIPV0C, expert, image, saveCorr);   iHisto++;
  h1d = new TH1D("H1D_MIP_V0", "Total MIP in V0;Charge [MIP];Counts", 2*kNMIPBins,kMIPMin ,32*kMIPMax) ;  
  Add2RawsList(h1d,kRawMIPV0, expert, image, saveCorr);   iHisto++;
  h2d = new TH2D("H2D_MIP_Channel", "Nb of MIP per channel;Channel;# of Mips", kNChannelBins, kChannelMin, kChannelMax,kNMIPBins,kMIPMin ,kMIPMax) ;  
  Add2RawsList(h2d,kRawMIPChannel, expert, !image, !saveCorr);   iHisto++;
  
 
 for(Int_t iInt=0;iInt<kNintegrator;iInt++){
    // Creation of Pedestal histograms 
    sprintf(name,"H2I_Pedestal_Int%d",iInt);
    sprintf(title,"Pedestal (Int%d);Pedestal [ADC counts];Counts",iInt);
    h2i = new TH2I(name, title,kNChannelBins, kChannelMin, kChannelMax,kNPedestalBins,kPedestalMin ,kPedestalMax );
    Add2RawsList(h2i,(iInt == 0 ? kPedestalInt0 : kPedestalInt1), expert, !image, !saveCorr); iHisto++;
	
    // Creation of temporary Pedestal histo used for the mean versus time histogram. This histogram will be reset at the end of each cycle
    sprintf(name,"H2I_Pedestal_CycleInt%d",iInt);
    sprintf(title,"One Cycle Pedestal (Int%d);Pedestal [ADC counts];Counts",iInt);
    h2i = new TH2I(name, title,kNChannelBins, kChannelMin, kChannelMax,kNPedestalBins,kPedestalMin ,kPedestalMax );
    Add2RawsList(h2i,(iInt == 0 ? kPedestalCycleInt0 : kPedestalCycleInt1), expert, !image, !saveCorr); iHisto++;
		
    // Creation of Pedestal versus time graph.
    sprintf(name,"H2D_Pedestal_Time_Int%d",iInt);
    sprintf(title,"Pedestal Versus Time (Int%d);Time [ns];Pedestal [ADC counts]",iInt);
    h2d = new TH2D(name, title,kNChannelBins, kChannelMin, kChannelMax,kTimeMax,kTimeMin ,kTimeMax );
    Add2RawsList(h2d,(iInt == 0 ? kPedestalTimeInt0 : kPedestalTimeInt1), expert, !image, !saveCorr); iHisto++;

   // Creation of Charge EoI histograms 
    sprintf(name,"H2I_ChargeEoI_Int%d",iInt);
    sprintf(title,"Charge EoI (Int%d);Charge [ADC counts];Counts",iInt);
    h2i = new TH2I(name, title,kNChannelBins, kChannelMin, kChannelMax, kNChargeBins, kChargeMin, kChargeMax);
    Add2RawsList(h2i,(iInt == 0 ? kChargeEoIInt0 : kChargeEoIInt1), expert, image, !saveCorr); iHisto++;

   // Creation of temporary Charge EoI histograms used for the mean versus time histogram. This histogram will be reset at the end of each cycle
    sprintf(name,"H2I_ChargeEoI_CycleInt%d",iInt);
    sprintf(title,"One Cycle Charge EoI (Int%d);Charge [ADC counts];Counts",iInt);
    h2i = new TH2I(name, title,kNChannelBins, kChannelMin, kChannelMax, kNChargeBins, kChargeMin, kChargeMax);
    Add2RawsList(h2i,(iInt == 0 ? kChargeEoICycleInt0 : kChargeEoICycleInt1), expert, !image, !saveCorr); iHisto++;
		
    // Creation of Charge EoI versus time graphs
    sprintf(name,"H2D_ChargeEoI_Time_Int%d",iInt);
    sprintf(title,"Charge EoI Versus Time (Int%d);Time [ns];Charge [ADC counts]",iInt);
    h2d = new TH2D(name, title,kNChannelBins, kChannelMin, kChannelMax,kTimeMax,kTimeMin ,kTimeMax );
    Add2RawsList(h2d,(iInt == 0 ? kChargeEoITimeInt0 : kChargeEoITimeInt1), expert, !image, !saveCorr); iHisto++;
    
    sprintf(name,"H2I_ChargeEoI_BB_Int%d",iInt);
    sprintf(title,"Charge EoI w/ BB Flag (Int%d);??;Charge [ADC counts]",iInt);
    h2i = new TH2I(name, title,kNChannelBins, kChannelMin, kChannelMax, kNChargeBins, kChargeMin, kChargeMax);
    Add2RawsList(h2i,(iInt == 0 ? kChargeEoIBBInt0 : kChargeEoIBBInt1), expert, !image, !saveCorr); iHisto++;
    
    sprintf(name,"H2I_ChargeEoI_BG_Int%d",iInt);
    sprintf(title,"Charge EoI w/ BG Flag (Int%d);??;Charge [ADC counts]",iInt);
    h2i = new TH2I(name, title,kNChannelBins, kChannelMin, kChannelMax, kNChargeBins, kChargeMin, kChargeMax);
    Add2RawsList(h2i,(iInt == 0 ?  kChargeEoIBGInt0: kChargeEoIBGInt1), expert, !image, !saveCorr); iHisto++;

    // Creation of Charge versus LHC Clock histograms 
    sprintf(name,"H2D_ChargeVsClock_Int%d",iInt);
    sprintf(title,"Charge Versus LHC-Clock (Int%d);Tine [ns];Charge [ADC counts]",iInt);
    h2d = new TH2D(name, title,kNChannelBins, kChannelMin, kChannelMax,21, -10.5, 10.5 );
    Add2RawsList(h2d,(iInt == 0 ? kChargeVsClockInt0 : kChargeVsClockInt1 ), expert, !image, !saveCorr); iHisto++;
	
    // Creation of Minimum Bias Charge histograms 
    for(Int_t iBB=0;iBB<2;iBB++){
		for(Int_t iBG=0;iBG<2;iBG++){
			sprintf(name,"H2I_ChargeMB_BB%d_BG%d_Int%d",iBB,iBG,iInt);
			sprintf(title,"MB Charge (BB=%d, BG=%d, Int=%d);Charge [ADC counts];Counts",iBB,iBG,iInt);
			h2i = new TH2I(name, title,kNChannelBins, kChannelMin, kChannelMax,kNChargeBins, kChargeMin, kChargeMax);
			int idx;
			if(iInt==0){
				if(iBB==0){
					if(iBG==0) idx = kChargeMBBB0BG0Int0;
					else idx = kChargeMBBB0BG1Int0;
				} else {
					if(iBG==0) idx = kChargeMBBB1BG0Int0;
					else idx = kChargeMBBB1BG1Int0;
				}
			} else {
				if(iBB==0){
					if(iBG==0) idx = kChargeMBBB0BG0Int1;
					else idx = kChargeMBBB0BG1Int1;
				} else {
					if(iBG==0) idx = kChargeMBBB1BG0Int1;
					else idx = kChargeMBBB1BG1Int1;
				}
			}
			Add2RawsList(h2i,idx, expert, !image, !saveCorr); iHisto++;
		}
    }
	
 }
 
     // Creation of Time histograms 
	sprintf(name,"H2I_Width");
	sprintf(title,"HPTDC Width;Width [ns];Counts");
	h2i = new TH2I(name, title,kNChannelBins, kChannelMin, kChannelMax, kNTdcWidthBins, kTdcWidthMin, kTdcWidthMax);
 	Add2RawsList(h2i,kWidth, expert, !image, !saveCorr); iHisto++;

 	sprintf(name,"H2I_Width_BB");
 	sprintf(title,"HPTDC Width w/ BB Flag condition;??;Width [ns]");
 	h2i = new TH2I(name, title,kNChannelBins, kChannelMin, kChannelMax, kNTdcWidthBins, kTdcWidthMin, kTdcWidthMax);
 	Add2RawsList(h2i,kWidthBB, expert, !image, !saveCorr); iHisto++;

 	sprintf(name,"H2I_Width_BG");
 	sprintf(title,"HPTDC Width w/ BG Flag condition??;Width [ns]");
 	h2i = new TH2I(name, title,kNChannelBins, kChannelMin, kChannelMax, kNTdcWidthBins, kTdcWidthMin, kTdcWidthMax);
 	Add2RawsList(h2i,kWidthBG, expert, !image, !saveCorr); iHisto++;

 	sprintf(name,"H2I_HPTDCTime");
 	sprintf(title,"HPTDC Time;??;Width [ns]");
 	h2i = new TH2I(name, title,kNChannelBins, kChannelMin, kChannelMax, kNTdcTimeBins, kTdcTimeMin, kTdcTimeMax);
 	Add2RawsList(h2i,kHPTDCTime, expert, image, !saveCorr); iHisto++;

 	sprintf(name,"H2I_HPTDCTime_BB");
 	sprintf(title,"HPTDC Time w/ BB Flag condition;??;Width [ns]");
 	h2i = new TH2I(name, title,kNChannelBins, kChannelMin, kChannelMax, kNTdcTimeBins, kTdcTimeMin, kTdcTimeMax);
 	Add2RawsList(h2i,kHPTDCTimeBB, expert, !image, !saveCorr); iHisto++;

 	sprintf(name,"H2I_HPTDCTime_BG");
 	sprintf(title,"HPTDC Time w/ BG Flag condition;??;Width [ns]");
 	h2i = new TH2I(name, title,kNChannelBins, kChannelMin, kChannelMax, kNTdcTimeBins, kTdcTimeMin, kTdcTimeMax);
 	Add2RawsList(h2i,kHPTDCTimeBG, expert, !image, !saveCorr); iHisto++;
	
 	sprintf(name,"H1D_V0A_Time");
 	sprintf(title,"V0A Time;Time [ns];Counts");
 	h1d = new TH1D(name, title,kNTdcTimeBins, kTdcTimeMin/10, kTdcTimeMax/10);
 	Add2RawsList(h1d,kV0ATime, expert, !image, saveCorr); iHisto++;
	
 	sprintf(name,"H1D_V0C_Time");
 	sprintf(title,"V0C Time;Time [ns];Counts");
 	h1d = new TH1D(name, title,kNTdcTimeBins, kTdcTimeMin/10, kTdcTimeMax/10);
 	Add2RawsList(h1d,kV0CTime, expert, !image, saveCorr); iHisto++;
	
 	sprintf(name,"H1D_Diff_Time");
 	sprintf(title,"Diff V0A-V0C Time;Time [ns];Counts");
 	h1d = new TH1D(name, title,2*kNTdcTimeBins, -kTdcTimeMax/10, kTdcTimeMax/10);
 	Add2RawsList(h1d,kDiffTime, expert, !image, saveCorr); iHisto++;
	
 	// Creation of Flag versus LHC Clock histograms 
 	sprintf(name,"H2D_BBFlagVsClock");
 	sprintf(title,"BB-Flags Versus LHC-Clock;Time [ns];??");
 	h2d = new TH2D(name, title,kNChannelBins, kChannelMin, kChannelMax,21, -10.5, 10.5 );
 	Add2RawsList(h2d,kBBFlagVsClock, expert, !image, !saveCorr); iHisto++;
	
 	sprintf(name,"H2D_BGFlagVsClock");
 	sprintf(title,"BG-Flags Versus LHC-Clock;Time [ns];??");
 	h2d = new TH2D(name, title,kNChannelBins, kChannelMin, kChannelMax,21, -10.5, 10.5 );
 	Add2RawsList(h2d,kBGFlagVsClock, expert, !image, !saveCorr); iHisto++;
	 
 	sprintf(name,"TREND_MeanChargePerRing");
 	sprintf(title,"Mean Charge per Event and per Ring versus time ");
 	trend = new AliVZEROTrending(name, title);
 	Add2RawsList(trend,kRawMeanChargePerRing, expert, !image, !saveCorr); iHisto++;
	 
 	sprintf(name,"TREND_MeanFlagPerRing");
 	sprintf(title,"Mean Flag per Event and per Ring versus time ");
 	trend = new AliVZEROTrending(name, title);
 	Add2RawsList(trend,kRawMeanFlagPerRing, expert, !image, !saveCorr); iHisto++;
	 
 	sprintf(name,"H1D_DQMFlag");
 	sprintf(title,"Current Flag per Event / Mean Flag per Event ");
 	h1d = new TH1D(name, title, kNChannelBins, kChannelMin, kChannelMax);
	h1d->SetFillColor(29);
	h1d->SetLineWidth(2);
	h1d->GetXaxis()->SetLabelSize(0.06);
    h1d->GetXaxis()->SetNdivisions(808,kFALSE);
	h1d->GetXaxis()->SetBinLabel(4, "V0C");h1d->GetXaxis()->SetBinLabel(5, "R0");
	h1d->GetXaxis()->SetBinLabel(12, "V0C");h1d->GetXaxis()->SetBinLabel(13, "R1");
	h1d->GetXaxis()->SetBinLabel(20, "V0C");h1d->GetXaxis()->SetBinLabel(21, "R2");
	h1d->GetXaxis()->SetBinLabel(28, "V0C");h1d->GetXaxis()->SetBinLabel(29, "R3");
	h1d->GetXaxis()->SetBinLabel(36, "V0A");h1d->GetXaxis()->SetBinLabel(37, "R0");
	h1d->GetXaxis()->SetBinLabel(44, "V0A");h1d->GetXaxis()->SetBinLabel(45, "R1");
	h1d->GetXaxis()->SetBinLabel(52, "V0A");h1d->GetXaxis()->SetBinLabel(53, "R2");
	h1d->GetXaxis()->SetBinLabel(60, "V0A");h1d->GetXaxis()->SetBinLabel(61, "R3");
	h1d->GetXaxis()->CenterTitle();
    h1d->GetXaxis()->SetTitleOffset(0.8);
    h1d->GetXaxis()->SetNdivisions(808,kFALSE);
 	Add2RawsList(h1d,kRawDQMFlag, !expert, image, !saveCorr); iHisto++;
	 
 	sprintf(name,"H1D_DQMCharge");
 	sprintf(title,"Current Charge per Event / Mean Charge per Event ");
 	h1d = new TH1D(name, title, kNChannelBins, kChannelMin, kChannelMax);
	h1d->SetFillColor(29);
	h1d->SetLineWidth(2);
	h1d->GetXaxis()->SetLabelSize(0.06);
    h1d->GetXaxis()->SetNdivisions(808,kFALSE);
	h1d->GetXaxis()->SetBinLabel(4, "V0C");h1d->GetXaxis()->SetBinLabel(5, "R0");
	h1d->GetXaxis()->SetBinLabel(12, "V0C");h1d->GetXaxis()->SetBinLabel(13, "R1");
	h1d->GetXaxis()->SetBinLabel(20, "V0C");h1d->GetXaxis()->SetBinLabel(21, "R2");
	h1d->GetXaxis()->SetBinLabel(28, "V0C");h1d->GetXaxis()->SetBinLabel(29, "R3");
	h1d->GetXaxis()->SetBinLabel(36, "V0A");h1d->GetXaxis()->SetBinLabel(37, "R0");
	h1d->GetXaxis()->SetBinLabel(44, "V0A");h1d->GetXaxis()->SetBinLabel(45, "R1");
	h1d->GetXaxis()->SetBinLabel(52, "V0A");h1d->GetXaxis()->SetBinLabel(53, "R2");
	h1d->GetXaxis()->SetBinLabel(60, "V0A");h1d->GetXaxis()->SetBinLabel(61, "R3");
	h1d->GetXaxis()->CenterTitle();
    h1d->GetXaxis()->SetTitleOffset(0.8);
    h1d->GetXaxis()->SetNdivisions(808,kFALSE);
 	Add2RawsList(h1d,kRawDQMCharge, !expert, image, !saveCorr); iHisto++;
	 
 	AliDebug(AliQAv1::GetQADebugLevel(), Form("%d Histograms has been added to the Raws List",iHisto));
 }

//____________________________________________________________________________ 
void AliVZEROQADataMakerRec::InitDigits()
{
  // create Digits histograms in Digits subdir
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 
  
  char TDCname[100];
  char ADCname[100];
  TH1I *fhDigTDC[64]; 
  TH1I *fhDigADC[64]; 
  char texte[100];
  
  // create Digits histograms in Digits subdir
  TH1I * h0 = new TH1I("hDigitMultiplicity", "Digits multiplicity distribution in VZERO;# of Digits;Entries", 100, 0, 99) ; 
  h0->Sumw2() ;
  Add2DigitsList(h0, 0, !expert, image) ;
  
  for (Int_t i=0; i<64; i++)
    {
    sprintf(TDCname, "hDigitTDC%d", i);
    sprintf(texte,"Digit TDC in cell %d; TDC value;Entries",i);    
    fhDigTDC[i] = new TH1I(TDCname,texte,300,0.,149.);
    
    sprintf(ADCname,"hDigitADC%d",i);
    sprintf(texte,"Digit ADC in cell %d;ADC value;Entries",i);
    fhDigADC[i]= new TH1I(ADCname,texte,1024,0.,1023.);
    
    Add2DigitsList(fhDigTDC[i],i+1, !expert, image);
    Add2DigitsList(fhDigADC[i],i+1+64, !expert, image);  
    }  
}

//____________________________________________________________________________
void AliVZEROQADataMakerRec::MakeDigits()
{
  // makes data from Digits

  GetDigitsData(0)->Fill(fDigitsArray->GetEntriesFast()) ; 
  TIter next(fDigitsArray) ; 
  AliVZEROdigit *VZERODigit ; 
  while ( (VZERODigit = dynamic_cast<AliVZEROdigit *>(next())) ) {
    Int_t   PMNumber  = VZERODigit->PMNumber();         
    GetDigitsData(PMNumber +1)->Fill( VZERODigit->Time()) ;    // in 100 of picoseconds
    GetDigitsData(PMNumber +1+64)->Fill( VZERODigit->ADC()) ;
  }  
}


//____________________________________________________________________________
void AliVZEROQADataMakerRec::MakeDigits(TTree *digitTree)
{
  // makes data from Digit Tree
	
  if ( fDigitsArray ) 
    fDigitsArray->Clear() ; 
  else 
    fDigitsArray = new TClonesArray("AliVZEROdigit", 1000) ; 
  
  TBranch * branch = digitTree->GetBranch("VZERODigit") ;
  if ( ! branch ) {
    AliWarning("VZERO branch in Digit Tree not found") ; 
  } else {
    branch->SetAddress(&fDigitsArray) ;
    branch->GetEntry(0) ; 
    MakeDigits() ; 
  }  
}


//____________________________________________________________________________
void AliVZEROQADataMakerRec::MakeESDs(AliESDEvent * esd)
{
  // Creates QA data from ESDs
  
  UInt_t eventType = esd->GetEventType();

  switch (eventType){
  	case PHYSICS_EVENT:
  	AliESDVZERO *esdVZERO=esd->GetVZEROData();
   
	if (!esdVZERO) break;
		  
    	GetESDsData(kCellMultiV0A)->Fill(esdVZERO->GetNbPMV0A());
    	GetESDsData(kCellMultiV0C)->Fill(esdVZERO->GetNbPMV0C());  
    	GetESDsData(kMIPMultiV0A)->Fill(esdVZERO->GetMTotV0A());
    	GetESDsData(kMIPMultiV0C)->Fill(esdVZERO->GetMTotV0C());  
    	
	Float_t  timeV0A = 0., timeV0C = 0., diffTime;
	Int_t   iTimeV0A = 0, iTimeV0C = 0;
		
	for(Int_t i=0;i<64;i++) {
			GetESDsData(kMIPMultiChannel)->Fill((Float_t) i,(Float_t) esdVZERO->GetMultiplicity(i));
		   	GetESDsData(kChargeChannel)->Fill((Float_t) i,(Float_t) esdVZERO->GetAdc(i));
			if(esdVZERO->GetBBFlag(i)) GetESDsData(kBBFlag)->Fill((Float_t) i);
		  	if(esdVZERO->GetBGFlag(i)) GetESDsData(kBGFlag)->Fill((Float_t) i);

			Float_t time = (Float_t) esdVZERO->GetTime(i)/10.; //Convert in ns:  1 TDC channel = 100ps 
			GetESDsData(kTimeChannel)->Fill((Float_t) i,time);

			if(time>0.){
				if (i<32) {
					iTimeV0C++;
					timeV0C += time;
				}else{
					iTimeV0A++;
					timeV0A += time;
				}
			}
    	}
	if(iTimeV0A>0) timeV0A /= iTimeV0A; 
	else timeV0A = -1.;
	if(iTimeV0C>0) timeV0C /= iTimeV0C;
	else timeV0C = -1.;
	if(timeV0A<0. || timeV0C<0.) diffTime = -10000.;
	else diffTime = timeV0A - timeV0C;
				
	GetESDsData(kESDV0ATime)->Fill(timeV0A);
	GetESDsData(kESDV0CTime)->Fill(timeV0C);
	GetESDsData(kESDDiffTime)->Fill(diffTime);
		
	break;
	}  
  
}

//____________________________________________________________________________
 void AliVZEROQADataMakerRec::MakeRaws(AliRawReader* rawReader)
 {
  // Fills histograms with Raws, computes average ADC values dynamically (pedestal subtracted)
                  
   // Check id histograms already created for this Event Specie
   if ( ! GetRawsData(kPedestalInt0) )
     InitRaws() ;

   rawReader->Reset() ; 
  AliVZERORawStream* rawStream  = new AliVZERORawStream(rawReader); 
 if(!(rawStream->Next())) return;  
 
  eventTypeType eventType = rawReader->GetType();

  Int_t    mulV0A = 0 ; 
  Int_t    mulV0C = 0 ; 
  Double_t timeV0A =0., timeV0C = 0.;
  UInt_t   itimeV0A=0, itimeV0C=0;
  Double_t chargeV0A=0., chargeV0C=0.;
  Double_t mipV0A=0., mipV0C=0.;

  Double_t diffTime=-100000.;

  
  switch (eventType){
       case PHYSICS_EVENT:
  
  		fNTotEvents++;

		if(fNSubEvents++>=fTrendingUpdateEvent && fTrendingUpdateEvent>0) {
			fNSubEvents=0;
			AddTrendingEntry();
		}
       Int_t  iFlag=0;
       Int_t  pedestal;
       Int_t  integrator;
       Bool_t BBFlag;	 
       Bool_t BGFlag;	 
       Float_t time, width;
       Int_t  MBCharge;
	   Float_t charge;
       Int_t  offlineCh;
       TH1D * hProj;

       for(Int_t iChannel=0; iChannel<64; iChannel++) { // BEGIN : Loop over channels
		   
	   offlineCh = rawStream->GetOfflineChannel(iChannel);
		   
	   // Fill Pedestal histograms
	   
           for(Int_t j=15; j<21; j++) {
		       if((rawStream->GetBGFlag(iChannel,j) || rawStream->GetBBFlag(iChannel,j))) iFlag++;
           }

           if(iFlag == 0){ //No Flag found
		       for(Int_t j=15; j<21; j++){
	   		       pedestal= (Int_t) rawStream->GetPedestal(iChannel, j);
	   		       integrator = rawStream->GetIntegratorFlag(iChannel, j);

	   		       GetRawsData((integrator == 0 ? kPedestalInt0 : kPedestalInt1))->Fill(offlineCh,pedestal);
	   		       GetRawsData((integrator == 0 ? kPedestalCycleInt0 : kPedestalCycleInt1))->Fill(offlineCh,pedestal);
		       }
            }

	   // Fill Charge EoI histograms
	   
	   // Look for the maximum in the LHC clock train
           charge = 0;
           Int_t iClock  = 0;
           Float_t iCharge = 0;
           for(Int_t iEvent=0; iEvent<21; iEvent++){
               iCharge = rawStream->GetPedestal(iChannel,iEvent);
               if(iCharge>charge)  {
	 	       charge = iCharge;
	    	       iClock = iEvent;
	           }
           }   // End of maximum searching procedure

           integrator    = rawStream->GetIntegratorFlag(iChannel,iClock);
           BBFlag	 = rawStream->GetBBFlag(iChannel, iClock);
           BGFlag	 = rawStream->GetBGFlag(iChannel,iClock );

           GetRawsData((integrator == 0 ? kChargeEoIInt0 : kChargeEoIInt1))->Fill(offlineCh,charge);
	   if(BBFlag) GetRawsData((integrator == 0 ? kChargeEoIBBInt0 : kChargeEoIBBInt1))->Fill(offlineCh,charge);
           if(BGFlag) GetRawsData((integrator == 0 ? kChargeEoIBGInt0 : kChargeEoIBGInt1))->Fill(offlineCh,charge);

	   hProj = ((TH2I*)GetRawsData((integrator == 0 ? kPedestalInt0 : kPedestalInt1)))->ProjectionY("",offlineCh+1,offlineCh+1);
	   Double_t ped   = hProj->GetMean();
	   Double_t sigma = hProj->GetRMS();
	   delete hProj;

	   Double_t chargeEoI = charge - ped;
		   
	   // Calculation of the number of MIP
	   Double_t mipEoI = chargeEoI * fCalibData->GetMIPperADC(offlineCh);

	   int side = offlineCh/32;
	   int ring = (offlineCh - 32*side) / 8;
	   if(BBFlag) {
	   		fFlagPerRing[side*4 + ring] += 1;
	   		fFlagPerChannel[offlineCh] += 1;
	   }	

	   if(charge<1023 && chargeEoI > 5.*sigma){ 
		   fChargePerRing[side*4 + ring] += chargeEoI;	
	   	   fChargePerChannel[offlineCh] += chargeEoI;
		   ((TH2I*)GetRawsData((integrator == 0 ? kChargeEoICycleInt0 : kChargeEoICycleInt1)))->Fill(offlineCh,chargeEoI);
		   ((TH2D*)GetRawsData(kRawMIPChannel))->Fill(offlineCh,mipEoI);
        	   if(offlineCh<32) {
				   mulV0C++;
				   chargeV0C += chargeEoI;
				   mipV0C += mipEoI;
		   } else {
				   mulV0A++;
				   chargeV0A += chargeEoI;
				   mipV0A += mipEoI;
		   }
	   }

	   // Fill Charge Minimum Bias Histograms
		   
	   int idx;
	   for(Int_t iBunch=0; iBunch<10; iBunch++){
			   integrator = rawStream->GetIntMBFlag(iChannel, iBunch);
			   BBFlag     = rawStream->GetBBMBFlag(iChannel, iBunch);
			   BGFlag     = rawStream->GetBGMBFlag(iChannel, iBunch);
			   MBCharge   = rawStream->GetChargeMB(iChannel, iBunch);

			   if(integrator==0){
				   if(BBFlag==0){
					   if(BGFlag==0) idx = kChargeMBBB0BG0Int0;
					   else idx = kChargeMBBB0BG1Int0;
				   } else {
					   if(BGFlag==0) idx = kChargeMBBB1BG0Int0;
					   else idx = kChargeMBBB1BG1Int0;
				   }
			   } else {
				   if(BBFlag==0){
					   if(BGFlag==0) idx = kChargeMBBB0BG0Int1;
					   else idx = kChargeMBBB0BG1Int1;
				   } else {
					   if(BGFlag==0) idx = kChargeMBBB1BG0Int1;
					   else idx = kChargeMBBB1BG1Int1;
				   }
			   }
			   GetRawsData(idx)->Fill(offlineCh,MBCharge);
          }   

	  // Fill HPTDC Time Histograms

	   BBFlag   = rawStream->GetBBFlag(iChannel, 10);
           BGFlag   = rawStream->GetBGFlag(iChannel, 10);
           time     = rawStream->GetTime(iChannel);
           width    = rawStream->GetWidth(iChannel);

	   if(time>0.){
		      if (offlineCh<32) {
				   itimeV0C++;
				   timeV0C += time;
		      }else{
				   itimeV0A++;
				   timeV0A += time;
		      }
	   }
           GetRawsData(kHPTDCTime)->Fill(offlineCh,time);
           GetRawsData(kWidth)->Fill(offlineCh,width);
           if(BBFlag) {
		  GetRawsData(kHPTDCTimeBB)->Fill(offlineCh,time);
  	          GetRawsData(kWidthBB)->Fill(offlineCh,width);
           }
	   if(BGFlag) {
	          GetRawsData(kHPTDCTimeBG)->Fill(offlineCh,time);
	          GetRawsData(kWidthBG)->Fill(offlineCh,width);
           }

	   // Fill Flag and Charge Versus LHC-Clock histograms
	   
	   for(Int_t iEvent=0; iEvent<21; iEvent++){
               charge = rawStream->GetPedestal(iChannel,iEvent);
               integrator = rawStream->GetIntegratorFlag(iChannel,iEvent);
               BBFlag	  = rawStream->GetBBFlag(iChannel, iEvent);
               BGFlag	  = rawStream->GetBGFlag(iChannel,iEvent );

               ((TH2*) GetRawsData((integrator == 0 ? kChargeVsClockInt0 : kChargeVsClockInt1 )))->Fill(offlineCh,(float)iEvent-10,(float)charge);
               ((TH2*) GetRawsData(kBBFlagVsClock))->Fill(offlineCh,(float)iEvent-10,(float)BBFlag);
               ((TH2*) GetRawsData(kBGFlagVsClock))->Fill(offlineCh,(float)iEvent-10,(float)BGFlag);
           }

       }// END of Loop over channels

	    if(itimeV0A>0) timeV0A /= (itimeV0A * 10); // itimeV0A Channels and divide by 10 to have the result in ns because 1 TDC Channel = 100 ps
	    else timeV0A = -1.;
	    if(itimeV0C>0) timeV0C /= (itimeV0C * 10);
	    else timeV0C = -1.;
	    if(timeV0A<0. || timeV0C<0.) diffTime = -10000.;
	    else diffTime = timeV0A - timeV0C;
		
	    GetRawsData(kV0ATime)->Fill(timeV0A);
	    GetRawsData(kV0CTime)->Fill(timeV0C);
	    GetRawsData(kDiffTime)->Fill(diffTime);
		
	    GetRawsData(kMultiV0A)->Fill(mulV0A);
	    GetRawsData(kMultiV0C)->Fill(mulV0C);

	    GetRawsData(kChargeV0A)->Fill(chargeV0A);
	    GetRawsData(kChargeV0C)->Fill(chargeV0C);
	    GetRawsData(kChargeV0)->Fill(chargeV0A + chargeV0C);
		
	    GetRawsData(kRawMIPV0A)->Fill(mipV0A);
	    GetRawsData(kRawMIPV0C)->Fill(mipV0C);
	    GetRawsData(kRawMIPV0)->Fill(mipV0A + mipV0C);
	    break;
	    
	} // END of SWITCH : EVENT TYPE 
	
	fEvent++; 
	TParameter<double> * p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kMultiV0A)->GetName()))) ; 
	if (p) p->SetVal((double)mulV0A) ; 

	p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kMultiV0C)->GetName()))) ; 
	if (p) p->SetVal((double)mulV0C) ;                     

	p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kChargeV0A)->GetName()))) ; 
	if (p) p->SetVal((double)chargeV0A) ; 

	p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kChargeV0C)->GetName()))) ; 
	if (p) p->SetVal((double)chargeV0C) ;                     

	p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kChargeV0)->GetName()))) ; 
	if (p) p->SetVal((double)(chargeV0A + chargeV0C)) ;                     
	
	p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kRawMIPV0A)->GetName()))) ; 
	if (p) p->SetVal((double)mipV0A) ; 
	
	p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kRawMIPV0C)->GetName()))) ; 
	if (p) p->SetVal((double)mipV0C) ;                     
	
	p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kRawMIPV0)->GetName()))) ; 
	if (p) p->SetVal((double)(mipV0A + mipV0C)) ;                     
	
	p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kV0ATime)->GetName()))) ; 
	if (p) p->SetVal((double)timeV0A) ; 
	
	p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kV0CTime)->GetName()))) ; 
	if (p) p->SetVal((double)timeV0C) ;                     
	
	p = dynamic_cast<TParameter<double>*>(GetParameterList()->FindObject(Form("%s_%s_%s", GetName(), AliQAv1::GetTaskName(AliQAv1::kRAWS).Data(), GetRawsData(kDiffTime)->GetName()))) ; 
	if (p) p->SetVal((double)diffTime) ;                     
	
  	delete rawStream; rawStream = 0x0;      


 }

//____________________________________________________________________________ 
void AliVZEROQADataMakerRec::StartOfDetectorCycle()
{
  // Detector specific actions at start of cycle
  
  // Reset of the histogram used - to have the trend versus time -
 
  fCalibData = GetCalibData();
  
  TH1* h;
  h = GetRawsData(kPedestalCycleInt0);
  if(h) h->Reset();
  h = GetRawsData(kPedestalCycleInt1); 
  if(h) h->Reset();
  h = GetRawsData(kChargeEoICycleInt0);
  if(h) h->Reset();
  h = GetRawsData(kChargeEoICycleInt1);
  if(h) h->Reset();
	
  TTimeStamp currentTime;
  fCycleStartTime = currentTime.GetSec();
 
  fNTotEvents = 0;
}

//-------------------------------------------------------------------------------------------------
void AliVZEROQADataMakerRec::AddTrendingEntry(){   
     //printf("AddTrendingEntry\n");
	fNTrendingUpdates++;
	
	// Normalize to the number of events
	for(int i=0; i<8;i++){
//		fChargePerRing[i] *= TMath::Power(10.,i)/fTrendingUpdateEvent;
//		fFlagPerRing[i] *= TMath::Power(10.,i)/fTrendingUpdateEvent;
		fChargePerRing[i] /= fTrendingUpdateEvent;
		fFlagPerRing[i] /= fTrendingUpdateEvent;
	}
	
	GetRawsData(kRawDQMCharge)->Reset();
	GetRawsData(kRawDQMFlag)->Reset();

	for(int i=0; i<64;i++){
		fChargePerChannel[i] /= fTrendingUpdateEvent;
		fFlagPerChannel[i] /= fTrendingUpdateEvent;
		
		if(fMeanChargePerChannel[i]) GetRawsData(kRawDQMCharge)->Fill(i,fChargePerChannel[i]/fMeanChargePerChannel[i]);
		else GetRawsData(kRawDQMCharge)->Fill(i,0.);
		
		if(fMeanFlagPerChannel[i]) GetRawsData(kRawDQMFlag)->Fill(i,fFlagPerChannel[i]/fMeanFlagPerChannel[i]);
		else GetRawsData(kRawDQMFlag)->Fill(i,0.);
		
		fMeanChargePerChannel[i] = (fMeanChargePerChannel[i] * (fNTrendingUpdates-1) + fChargePerChannel[i]) / fNTrendingUpdates;
		fMeanFlagPerChannel[i] = (fMeanFlagPerChannel[i] * (fNTrendingUpdates-1) + fFlagPerChannel[i]) / fNTrendingUpdates;
		
	}
	
	TTimeStamp currentTime;
	((AliVZEROTrending*)GetRawsData(kRawMeanChargePerRing))->AddEntry(fChargePerRing,  currentTime.GetSec());
	((AliVZEROTrending*)GetRawsData(kRawMeanFlagPerRing))->AddEntry(fFlagPerRing,  currentTime.GetSec());
	//moMeanFlagPerRing->AddEntry(fFlagPerRing,  currentTime.GetSec());
	
	// Put back counters to zero
	for(int i=0; i<8;i++){
		fChargePerRing[i] = 0.;
		fFlagPerRing[i] = 0.;
	}
	for(int i=0; i<64;i++){
		fChargePerChannel[i] = 0.;
		fFlagPerChannel[i] = 0.;
	}
	
}

