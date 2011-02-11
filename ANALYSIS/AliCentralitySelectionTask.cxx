/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, All rights reserved. *
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

//*****************************************************
//   Class AliCentralitySelectionTask
//   Class to analyze determine centrality            
//   author: Alberica Toia
//*****************************************************

#include "AliCentralitySelectionTask.h"

#include <TTree.h>
#include <TList.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TProfile.h>
#include <TFile.h>
#include <TObjString.h>
#include <TString.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TDirectory.h>
#include <TSystem.h>
#include <iostream>

#include "AliAnalysisManager.h"
#include "AliVEvent.h"
#include "AliESD.h"
#include "AliESDEvent.h"
#include "AliESDHeader.h"
#include "AliESDInputHandler.h"
#include "AliESDZDC.h"
#include "AliESDFMD.h"
#include "AliESDVZERO.h"
#include "AliCentrality.h"
#include "AliESDtrackCuts.h"
#include "AliMultiplicity.h"
#include "AliAODHandler.h"
#include "AliAODEvent.h"
#include "AliESDVertex.h"
#include "AliAODVertex.h"
#include "AliAODMCHeader.h"
#include "AliMCEvent.h"
#include "AliMCEventHandler.h"
#include "AliMCParticle.h"
#include "AliStack.h"
#include "AliHeader.h"
#include "AliAODMCParticle.h"
#include "AliAnalysisTaskSE.h"
#include "AliGenEventHeader.h"
#include "AliGenHijingEventHeader.h"
#include "AliPhysicsSelectionTask.h"
#include "AliPhysicsSelection.h"
#include "AliBackgroundSelection.h"
#include "AliESDUtils.h"

ClassImp(AliCentralitySelectionTask)


//________________________________________________________________________
AliCentralitySelectionTask::AliCentralitySelectionTask():
AliAnalysisTaskSE(),
  fDebug(0),
  fAnalysisInput("ESD"),
  fIsMCInput(kFALSE),
  fFile(0),
  fFile2(0),
  fCurrentRun(-1),
  fRunNo(-1),
  fLowRunN(0),
  fHighRunN(0),
  fUseScaling(0),
  fTrackCuts(0),
  fZVCut(10),
  fOutliersCut(5),
  fQuality(0),
  fCentV0M(0),
  fCentFMD(0),
  fCentTRK(0),
  fCentTKL(0),
  fCentCL0(0),
  fCentCL1(0),
  fCentV0MvsFMD(0),
  fCentTKLvsV0M(0),
  fCentZEMvsZDC(0),
  fHtempV0M(0),
  fHtempFMD(0),
  fHtempTRK(0),
  fHtempTKL(0),
  fHtempCL0(0),
  fHtempCL1(0),
  fHtempV0MvsFMD(0),
  fHtempTKLvsV0M(0),
  fHtempZEMvsZDC(0),
  fOutputList(0),
  fHOutCentV0M     (0),
  fHOutCentFMD     (0),
  fHOutCentTRK     (0),
  fHOutCentTKL     (0),
  fHOutCentCL0     (0),
  fHOutCentCL1     (0),
  fHOutCentV0MvsFMD(0),
  fHOutCentTKLvsV0M(0),
  fHOutCentZEMvsZDC(0),
  fHOutMultV0M(0),
  fHOutMultV0R(0),
  fHOutMultFMD(0),
  fHOutMultTRK(0),
  fHOutMultTKL(0),
  fHOutMultCL0(0),
  fHOutMultCL1(0),
  fHOutMultV0MvsZDC(0),
  fHOutMultZEMvsZDC(0),
  fHOutMultV0MvsCL1(0),
  fHOutMultV0MvsTRK(0),
  fHOutMultTRKvsCL1(0),
  fHOutCentV0M_qual1(0),
  fHOutCentTRK_qual1(0),
  fHOutCentCL1_qual1(0),
  fHOutCentV0M_qual2(0),
  fHOutCentTRK_qual2(0),
  fHOutCentCL1_qual2(0),
  fHOutQuality(0),
  fHOutVertex(0)
{   
  // Default constructor
  AliInfo("Centrality Selection enabled.");
  fLowRunN =137161;
  fHighRunN=139173;
  for (Int_t i=0; i<(fHighRunN-fLowRunN); i++) {
    V0MScaleFactor[i]=0.0;
    SPDScaleFactor[i]=0.0;
    TPCScaleFactor[i]=0.0;
    V0MScaleFactorMC[i]=0.0;
  }
  fUseScaling=kTRUE;
}   

//________________________________________________________________________
AliCentralitySelectionTask::AliCentralitySelectionTask(const char *name):
  AliAnalysisTaskSE(name),
  fDebug(0),
  fAnalysisInput("ESD"),
  fIsMCInput(kFALSE),
  fFile(0),
  fFile2(0),
  fCurrentRun(-1),
  fRunNo(-1),
  fLowRunN(0),
  fHighRunN(0),
  fUseScaling(0),
  fTrackCuts(0),
  fZVCut(10),
  fOutliersCut(5),
  fQuality(0),
  fCentV0M(0),
  fCentFMD(0),
  fCentTRK(0),
  fCentTKL(0),
  fCentCL0(0),
  fCentCL1(0),
  fCentV0MvsFMD(0),
  fCentTKLvsV0M(0),
  fCentZEMvsZDC(0),
  fHtempV0M(0),
  fHtempFMD(0),
  fHtempTRK(0),
  fHtempTKL(0),
  fHtempCL0(0),
  fHtempCL1(0),
  fHtempV0MvsFMD(0),
  fHtempTKLvsV0M(0),
  fHtempZEMvsZDC(0),
  fOutputList(0),
  fHOutCentV0M     (0),
  fHOutCentFMD     (0),
  fHOutCentTRK     (0),
  fHOutCentTKL     (0),
  fHOutCentCL0     (0),
  fHOutCentCL1     (0),
  fHOutCentV0MvsFMD(0),
  fHOutCentTKLvsV0M(0),
  fHOutCentZEMvsZDC(0),
  fHOutMultV0M(0),
  fHOutMultV0R(0),
  fHOutMultFMD(0),
  fHOutMultTRK(0),
  fHOutMultTKL(0),
  fHOutMultCL0(0),
  fHOutMultCL1(0),
  fHOutMultV0MvsZDC(0),
  fHOutMultZEMvsZDC(0),
  fHOutMultV0MvsCL1(0),
  fHOutMultV0MvsTRK(0),
  fHOutMultTRKvsCL1(0),
  fHOutCentV0M_qual1(0),
  fHOutCentTRK_qual1(0),
  fHOutCentCL1_qual1(0),
  fHOutCentV0M_qual2(0),
  fHOutCentTRK_qual2(0),
  fHOutCentCL1_qual2(0),
  fHOutQuality(0),
  fHOutVertex(0)
{
  // Default constructor
  AliInfo("Centrality Selection enabled.");
  DefineOutput(1, TList::Class());
  for (Int_t i=0; i<(fHighRunN-fLowRunN); i++) {
    V0MScaleFactor[i]=0.0;
    SPDScaleFactor[i]=0.0;
    TPCScaleFactor[i]=0.0;
    V0MScaleFactorMC[i]=0.0;
  }
  fUseScaling=kTRUE;
}

//________________________________________________________________________
AliCentralitySelectionTask& AliCentralitySelectionTask::operator=(const AliCentralitySelectionTask& c)
{
  // Assignment operator
  if (this!=&c) {
    AliAnalysisTaskSE::operator=(c);
  }
  return *this;
}

//________________________________________________________________________
AliCentralitySelectionTask::AliCentralitySelectionTask(const AliCentralitySelectionTask& ana):
  AliAnalysisTaskSE(ana),
  fDebug(ana.fDebug),	  
  fAnalysisInput(ana.fDebug),
  fIsMCInput(ana.fIsMCInput),
  fFile(ana.fFile),
  fFile2(ana.fFile2),
  fCurrentRun(ana.fCurrentRun),
  fRunNo(ana.fRunNo),
  fLowRunN(ana.fLowRunN),
  fHighRunN(ana.fHighRunN),
  fUseScaling(ana.fUseScaling),
  fTrackCuts(ana.fTrackCuts),
  fZVCut(ana.fZVCut),
  fOutliersCut(ana.fOutliersCut),
  fQuality(ana.fQuality),
  fCentV0M(ana.fCentV0M),
  fCentFMD(ana.fCentFMD),
  fCentTRK(ana.fCentTRK),
  fCentTKL(ana.fCentTKL),
  fCentCL0(ana.fCentCL0),
  fCentCL1(ana.fCentCL1),
  fCentV0MvsFMD(ana.fCentV0MvsFMD),
  fCentTKLvsV0M(ana.fCentTKLvsV0M),
  fCentZEMvsZDC(ana.fCentZEMvsZDC),
  fHtempV0M(ana.fHtempV0M),
  fHtempFMD(ana.fHtempFMD),
  fHtempTRK(ana.fHtempTRK),
  fHtempTKL(ana.fHtempTKL),
  fHtempCL0(ana.fHtempCL0),
  fHtempCL1(ana.fHtempCL1),
  fHtempV0MvsFMD(ana.fHtempV0MvsFMD),
  fHtempTKLvsV0M(ana.fHtempTKLvsV0M),
  fHtempZEMvsZDC(ana.fHtempZEMvsZDC),
  fOutputList(ana.fOutputList),
  fHOutCentV0M     (ana.fHOutCentV0M     ),
  fHOutCentFMD     (ana.fHOutCentFMD     ),
  fHOutCentTRK     (ana.fHOutCentTRK     ),
  fHOutCentTKL     (ana.fHOutCentTKL     ),
  fHOutCentCL0     (ana.fHOutCentCL0     ),
  fHOutCentCL1     (ana.fHOutCentCL1     ),
  fHOutCentV0MvsFMD(ana.fHOutCentV0MvsFMD),
  fHOutCentTKLvsV0M(ana.fHOutCentTKLvsV0M),
  fHOutCentZEMvsZDC(ana.fHOutCentZEMvsZDC),
  fHOutMultV0M(ana.fHOutMultV0M),
  fHOutMultV0R(ana.fHOutMultV0R),
  fHOutMultFMD(ana.fHOutMultFMD),
  fHOutMultTRK(ana.fHOutMultTRK),
  fHOutMultTKL(ana.fHOutMultTKL),
  fHOutMultCL0(ana.fHOutMultCL0),
  fHOutMultCL1(ana.fHOutMultCL1),
  fHOutMultV0MvsZDC(ana.fHOutMultV0MvsZDC),
  fHOutMultZEMvsZDC(ana.fHOutMultZEMvsZDC),
  fHOutMultV0MvsCL1(ana.fHOutMultV0MvsCL1),
  fHOutMultV0MvsTRK(ana.fHOutMultV0MvsTRK),
  fHOutMultTRKvsCL1(ana.fHOutMultTRKvsCL1),
  fHOutCentV0M_qual1(ana.fHOutCentV0M_qual1),
  fHOutCentTRK_qual1(ana.fHOutCentTRK_qual1),
  fHOutCentCL1_qual1(ana.fHOutCentCL1_qual1),
  fHOutCentV0M_qual2(ana.fHOutCentV0M_qual2),
  fHOutCentTRK_qual2(ana.fHOutCentTRK_qual2),
  fHOutCentCL1_qual2(ana.fHOutCentCL1_qual2),
  fHOutQuality(ana.fHOutQuality),
  fHOutVertex(ana.fHOutVertex)
{
  // Copy Constructor	
}
 
//________________________________________________________________________
AliCentralitySelectionTask::~AliCentralitySelectionTask()
{
  // Destructor  
  if (fOutputList && !AliAnalysisManager::GetAnalysisManager()->IsProofMode()) delete fOutputList;
  if (fTrackCuts) delete fTrackCuts;
}  

//________________________________________________________________________
void AliCentralitySelectionTask::UserCreateOutputObjects()
{  
  // Create the output containers
  if(fDebug>1) printf("AnalysisCentralitySelectionTask::UserCreateOutputObjects() \n");
  AliLog::SetClassDebugLevel("AliCentralitySelectionTask", AliLog::kInfo);

  fOutputList = new TList();
  fOutputList->SetOwner();
  fHOutCentV0M     = new TH1F("fHOutCentV0M","fHOutCentV0M; Centrality V0",501,0,101);
  fHOutCentFMD     = new TH1F("fHOutCentFMD","fHOutCentFMD; Centrality FMD",501,0,101);
  fHOutCentTRK     = new TH1F("fHOutCentTRK","fHOutCentTRK; Centrality TPC",501,0,101);
  fHOutCentTKL     = new TH1F("fHOutCentTKL","fHOutCentTKL; Centrality tracklets",501,0,101);
  fHOutCentCL0     = new TH1F("fHOutCentCL0","fHOutCentCL0; Centrality SPD inner",501,0,101);
  fHOutCentCL1     = new TH1F("fHOutCentCL1","fHOutCentCL1; Centrality SPD outer",501,0,101);
  fHOutCentV0MvsFMD= new TH1F("fHOutCentV0MvsFMD","fHOutCentV0MvsFMD; Centrality V0 vs FMD",501,0,101);
  fHOutCentTKLvsV0M= new TH1F("fHOutCentTKLvsV0M","fHOutCentTKLvsV0M; Centrality tracklets vs V0",501,0,101);
  fHOutCentZEMvsZDC= new TH1F("fHOutCentZEMvsZDC","fHOutCentZEMvsZDC; Centrality ZEM vs ZDC",501,0,101);

  fHOutMultV0M = new TH1F("fHOutMultV0M","fHOutMultV0M; Multiplicity V0",25000,0,25000);
  fHOutMultV0R = new TH1F("fHOutMultV0R","fHOutMultV0R; Multiplicity V0",25000,0,25000);
  fHOutMultFMD = new TH1F("fHOutMultFMD","fHOutMultFMD; Multiplicity FMD",24000,0,24000);
  fHOutMultTRK = new TH1F("fHOutMultTRK","fHOutMultTRK; Multiplicity TPC",4000,0,4000);
  fHOutMultTKL = new TH1F("fHOutMultTKL","fHOutMultTKL; Multiplicity tracklets",5000,0,5000);
  fHOutMultCL0 = new TH1F("fHOutMultCL0","fHOutMultCL0; Multiplicity SPD inner",7000,0,7000);
  fHOutMultCL1 = new TH1F("fHOutMultCL1","fHOutMultCL1; Multiplicity SPD outer",7000,0,7000);
  fHOutMultV0MvsZDC = new TH2F("fHOutMultV0MvsZDC","fHOutMultV0MvsZDC; Multiplicity V0; Energy ZDC",500,0,25000,500,0,6000);
  fHOutMultZEMvsZDC = new TH2F("fHOutMultZEMvsZDC","fHOutMultZEMvsZDC; Energy ZEM; Energy ZDC",500,0,2500,500,0,6000);
  fHOutMultV0MvsCL1 = new TH2F("fHOutMultV0MvsCL1","fHOutMultV0MvsCL1; Multiplicity V0; Multiplicity SPD outer",2500,0,25000,700,0,7000);
  fHOutMultV0MvsTRK = new TH2F("fHOutMultV0MvsTRK","fHOutMultV0MvsTRK; Multiplicity V0; Multiplicity TPC",2500,0,25000,400,0,4000);
  fHOutMultTRKvsCL1 = new TH2F("fHOutMultTRKvsCL1","fHOutMultTRKvsCL1; Multiplicity TPC; Multiplicity SPD outer",400,0,4000,700,0,7000);

  fHOutCentV0M_qual1 = new TH1F("fHOutCentV0M_qual1","fHOutCentV0M_qual1; Centrality V0",501,0,100);
  fHOutCentTRK_qual1 = new TH1F("fHOutCentTRK_qual1","fHOutCentTRK_qual1; Centrality TPC",501,0,100);
  fHOutCentCL1_qual1 = new TH1F("fHOutCentCL1_qual1","fHOutCentCL1_qual1; Centrality SPD outer",501,0,100);

  fHOutCentV0M_qual2 = new TH1F("fHOutCentV0M_qual2","fHOutCentV0M_qual2; Centrality V0",101,-0.1,100.1);
  fHOutCentTRK_qual2 = new TH1F("fHOutCentTRK_qual2","fHOutCentTRK_qual2; Centrality TPC",101,-0.1,100.1);
  fHOutCentCL1_qual2 = new TH1F("fHOutCentCL1_qual2","fHOutCentCL1_qual2; Centrality SPD outer",101,-0.1,100.1);

  fHOutQuality = new TH1F("fHOutQuality", "fHOutQuality", 10,-0.5,9.5);
  fHOutVertex  = new TH1F("fHOutVertex", "fHOutVertex", 100,-20,20);

  fOutputList->Add(  fHOutCentV0M     );
  fOutputList->Add(  fHOutCentFMD     );
  fOutputList->Add(  fHOutCentTRK     );
  fOutputList->Add(  fHOutCentTKL     );
  fOutputList->Add(  fHOutCentCL0     );
  fOutputList->Add(  fHOutCentCL1     );
  fOutputList->Add(  fHOutCentV0MvsFMD);
  fOutputList->Add(  fHOutCentTKLvsV0M);
  fOutputList->Add(  fHOutCentZEMvsZDC);
  fOutputList->Add(  fHOutMultV0M); 
  fOutputList->Add(  fHOutMultV0R); 
  fOutputList->Add(  fHOutMultFMD); 
  fOutputList->Add(  fHOutMultTRK); 
  fOutputList->Add(  fHOutMultTKL); 
  fOutputList->Add(  fHOutMultCL0); 
  fOutputList->Add(  fHOutMultCL1); 
  fOutputList->Add(  fHOutMultV0MvsZDC);
  fOutputList->Add(  fHOutMultZEMvsZDC);
  fOutputList->Add(  fHOutMultV0MvsCL1);
  fOutputList->Add(  fHOutMultV0MvsTRK);
  fOutputList->Add(  fHOutMultTRKvsCL1);
  fOutputList->Add(  fHOutCentV0M_qual1 );
  fOutputList->Add(  fHOutCentTRK_qual1 );
  fOutputList->Add(  fHOutCentCL1_qual1 );                   
  fOutputList->Add(  fHOutCentV0M_qual2 );
  fOutputList->Add(  fHOutCentTRK_qual2 );
  fOutputList->Add(  fHOutCentCL1_qual2 );
  fOutputList->Add(  fHOutQuality );
  fOutputList->Add(  fHOutVertex );


  fTrackCuts = AliESDtrackCuts::GetStandardTPCOnlyTrackCuts();

  PostData(1, fOutputList); 

  MyInitScaleFactor();
  if (fIsMCInput) MyInitScaleFactorMC();
}

//________________________________________________________________________
void AliCentralitySelectionTask::UserExec(Option_t */*option*/)
{ 
  // Execute analysis for current event:
  if(fDebug>1) printf(" **** AliCentralitySelectionTask::UserExec() \n");
  
  Float_t  zncEnergy = 0.;          //  ZNC Energy
  Float_t  zpcEnergy = 0.;          //  ZPC Energy
  Float_t  znaEnergy = 0.;          //  ZNA Energy
  Float_t  zpaEnergy = 0.;          //  ZPA Energy
  Float_t  zem1Energy = 0.;         //  ZEM1 Energy
  Float_t  zem2Energy = 0.;         //  ZEM2 Energy
  
  Int_t    nTracks = 0;             //  no. tracks
  Int_t    nTracklets = 0;          //  no. tracklets
  Int_t    nClusters[6] = {0};      //  no. clusters on 6 ITS layers
  Int_t    nChips[2];               //  no. chips on 2 SPD layers
  Float_t  spdCorr =0;              //  corrected spd2 multiplicity

  Float_t  multV0A  = 0;            //  multiplicity from V0 reco side A
  Float_t  multV0C  = 0;            //  multiplicity from V0 reco side C
  Float_t  multFMDA = 0;            //  multiplicity from FMD on detector A
  Float_t  multFMDC = 0;            //  multiplicity from FMD on detector C

  Short_t v0Corr = 0;               // corrected V0 multiplicity
  Short_t v0CorrResc = 0;           // corrected and rescaled V0 multiplicity

  Float_t zvtx =0;                  // z-vertex SPD
 
  AliCentrality *esdCent = 0;

  if(fAnalysisInput.CompareTo("ESD")==0){

    AliVEvent* event = InputEvent();
    AliESDEvent* esd = dynamic_cast<AliESDEvent*>(event);
    if (!esd) {
	AliError("No ESD Event");
	return;
    }
    
    if (fRunNo<=0) {
      if (SetupRun(esd)<0)
         AliFatal("Centrality File not available for this run");
    }

    esdCent = esd->GetCentrality();

    // ***** V0 info    
    AliESDVZERO* esdV0 = esd->GetVZEROData();
    multV0A=esdV0->GetMTotV0A();
    multV0C=esdV0->GetMTotV0C();

    Float_t v0CorrR;
    v0Corr = (Short_t)AliESDUtils::GetCorrV0(esd,v0CorrR);
    v0CorrResc = (Short_t)v0CorrR;

    // ***** Vertex Info
    const AliESDVertex* vtxESD = esd->GetPrimaryVertexSPD();
    zvtx        = vtxESD->GetZ(); 

    // ***** CB info (tracklets, clusters, chips)
    //nTracks    = event->GetNumberOfTracks();     
    nTracks    = fTrackCuts ? (Short_t)fTrackCuts->GetReferenceMultiplicity(esd,kTRUE):-1;

    const AliMultiplicity *mult = esd->GetMultiplicity();

    nTracklets = mult->GetNumberOfTracklets();

    for(Int_t ilay=0; ilay<6; ilay++){
      nClusters[ilay] = mult->GetNumberOfITSClusters(ilay);
    }

    for(Int_t ilay=0; ilay<2; ilay++){
      nChips[ilay] = mult->GetNumberOfFiredChips(ilay);
    }

    spdCorr = AliESDUtils::GetCorrSPD2(nClusters[1],zvtx);    

    // ***** FMD info
    AliESDFMD *fmd = esd->GetFMDData();
    Float_t totalMultA = 0;
    Float_t totalMultC = 0;
    const Float_t fFMDLowCut = 0.4;
    
    for(UShort_t det=1;det<=3;det++) {
      Int_t nRings = (det==1 ? 1 : 2);
      for (UShort_t ir = 0; ir < nRings; ir++) {	  
	Char_t   ring = (ir == 0 ? 'I' : 'O');
	UShort_t nsec = (ir == 0 ? 20  : 40);
	UShort_t nstr = (ir == 0 ? 512 : 256);
	for(UShort_t sec =0; sec < nsec;  sec++)  {
	  for(UShort_t strip = 0; strip < nstr; strip++) {
	    
	    Float_t FMDmult = fmd->Multiplicity(det,ring,sec,strip);
	    if(FMDmult == 0 || FMDmult == AliESDFMD::kInvalidMult) continue;
	    
	    Float_t nParticles=0;
	    
	    if(FMDmult > fFMDLowCut) {
	      nParticles = 1.;
	    }
	    
	    if (det<3) totalMultA = totalMultA + nParticles;
	    else totalMultC = totalMultC + nParticles;
	    
	  }
	}
      }
    }
    multFMDA = totalMultA;
    multFMDC = totalMultC;
    
    // ***** ZDC info
    AliESDZDC *esdZDC = esd->GetESDZDC();
    zncEnergy = (Float_t) (esdZDC->GetZDCN1Energy())/8.;
    zpcEnergy = (Float_t) (esdZDC->GetZDCP1Energy())/8.;
    znaEnergy = (Float_t) (esdZDC->GetZDCN2Energy())/8.;
    zpaEnergy = (Float_t) (esdZDC->GetZDCP2Energy())/8.;
    zem1Energy = (Float_t) (esdZDC->GetZDCEMEnergy(0))/8.;
    zem2Energy = (Float_t) (esdZDC->GetZDCEMEnergy(1))/8.;
    
  }   
  else if(fAnalysisInput.CompareTo("AOD")==0){
    //AliAODEvent *aod =  dynamic_cast<AliAODEvent*> (InputEvent());
    // to be implemented
    printf("  AOD analysis not yet implemented!!!\n\n");
    return;
  } 


  // ***** Scaling
  if (fUseScaling) {
    Float_t temp_scalefactorV0M = MyGetScaleFactor(fCurrentRun,0);
    Float_t temp_scalefactorSPD = MyGetScaleFactor(fCurrentRun,1);
    Float_t temp_scalefactorTPC = MyGetScaleFactor(fCurrentRun,2);
    v0Corr  = Short_t(v0Corr / temp_scalefactorV0M);
    spdCorr = spdCorr / temp_scalefactorSPD;
    nTracks = Int_t(nTracks / temp_scalefactorTPC);
  }
  // ***** Scaling for MC
  if (fIsMCInput) {
    Float_t temp_scalefactorV0M = MyGetScaleFactorMC(fCurrentRun);
    v0Corr  = Short_t((multV0A+multV0C)  * temp_scalefactorV0M);
  }

  // ***** Centrality Selection
  if(fHtempV0M) fCentV0M = fHtempV0M->GetBinContent(fHtempV0M->FindBin((v0Corr)));
  if(fHtempFMD) fCentFMD = fHtempFMD->GetBinContent(fHtempFMD->FindBin((multFMDA+multFMDC)));
  if(fHtempTRK) fCentTRK = fHtempTRK->GetBinContent(fHtempTRK->FindBin(nTracks));
  if(fHtempTKL) fCentTKL = fHtempTKL->GetBinContent(fHtempTKL->FindBin(nTracklets));
  if(fHtempCL0) fCentCL0 = fHtempCL0->GetBinContent(fHtempCL0->FindBin(nClusters[0]));
  if(fHtempCL1) fCentCL1 = fHtempCL1->GetBinContent(fHtempCL1->FindBin(spdCorr));
  
  if(fHtempV0MvsFMD) fCentV0MvsFMD = fHtempV0MvsFMD->GetBinContent(fHtempV0MvsFMD->FindBin((multV0A+multV0C)));
  if(fHtempTKLvsV0M) fCentTKLvsV0M = fHtempTKLvsV0M->GetBinContent(fHtempTKLvsV0M->FindBin(nTracklets));
  if(fHtempZEMvsZDC) fCentZEMvsZDC = fHtempZEMvsZDC->GetBinContent(fHtempZEMvsZDC->FindBin((zem1Energy+zem2Energy)/1000.));


  // ***** Cleaning
  fQuality=0;
  fZVCut=10;
  fOutliersCut=6;
  
  // ***** vertex
  if (TMath::Abs(zvtx)>fZVCut) fQuality += 1;   

  // ***** outliers
  // **** V0 vs SPD
  printf("AnalysisCentralitySelectionTask::centrality is %f\n",fCentV0M);

  if (IsOutlierV0MSPD(spdCorr, v0Corr, int(fCentV0M))) fQuality  += 2;
  // ***** V0 vs TPC
  if (IsOutlierV0MTPC(nTracks, v0Corr, int(fCentV0M))) fQuality  += 4;
  // ***** V0 vs ZDC
  if (IsOutlierV0MZDC((zncEnergy+znaEnergy+zpcEnergy+zpaEnergy), v0Corr)) fQuality  += 8;

  if (esdCent) {
      esdCent->SetQuality(fQuality);
      esdCent->SetCentralityV0M(fCentV0M);
      esdCent->SetCentralityFMD(fCentFMD);
      esdCent->SetCentralityTRK(fCentTRK);
      esdCent->SetCentralityTKL(fCentTKL);
      esdCent->SetCentralityCL0(fCentCL0);
      esdCent->SetCentralityCL1(fCentCL1);
      esdCent->SetCentralityV0MvsFMD(fCentV0MvsFMD);
      esdCent->SetCentralityTKLvsV0M(fCentTKLvsV0M);
      esdCent->SetCentralityZEMvsZDC(fCentZEMvsZDC);
  }

  fHOutQuality->Fill(fQuality);
  fHOutVertex->Fill(zvtx);

  fHOutMultV0MvsZDC->Fill(v0Corr,(zncEnergy+znaEnergy+zpcEnergy+zpaEnergy));
  fHOutMultZEMvsZDC->Fill((zem1Energy+zem2Energy),(zncEnergy+znaEnergy+zpcEnergy+zpaEnergy));
  fHOutMultV0MvsCL1->Fill(v0Corr,spdCorr);
  fHOutMultV0MvsTRK->Fill(v0Corr,nTracks);
  
  if (fQuality==0) {  
    fHOutCentV0M->Fill(fCentV0M);
    fHOutCentFMD->Fill(fCentFMD);
    fHOutCentTRK->Fill(fCentTRK);
    fHOutCentTKL->Fill(fCentTKL);
    fHOutCentCL0->Fill(fCentCL0);
    fHOutCentCL1->Fill(fCentCL1);
    fHOutCentV0MvsFMD->Fill(fCentV0MvsFMD);
    fHOutCentTKLvsV0M->Fill(fCentTKLvsV0M);
    fHOutCentZEMvsZDC->Fill(fCentZEMvsZDC);
    fHOutMultV0M->Fill(v0Corr);
    fHOutMultV0R->Fill(multV0A+multV0C);
    fHOutMultFMD->Fill((multFMDA+multFMDC));
    fHOutMultTRK->Fill(nTracks);
    fHOutMultTKL->Fill(nTracklets);
    fHOutMultCL0->Fill(nClusters[0]);
    fHOutMultCL1->Fill(spdCorr);
    fHOutMultTRKvsCL1->Fill(nTracks,spdCorr);
  } else if (fQuality ==1) {
    fHOutCentV0M_qual1->Fill(fCentV0M);
    fHOutCentTRK_qual1->Fill(fCentTRK);
    fHOutCentCL1_qual1->Fill(fCentCL1);
  } else {
    fHOutCentV0M_qual2->Fill(fCentV0M);
    fHOutCentTRK_qual2->Fill(fCentTRK);
    fHOutCentCL1_qual2->Fill(fCentCL1);
  }

  PostData(1, fOutputList); 
}

//________________________________________________________________________
void AliCentralitySelectionTask::ReadCentralityHistos(TString fCentfilename) 
{
  //  Read centrality histograms
  TDirectory *owd = gDirectory;
  // Check if the file is present
  TString path = gSystem->ExpandPathName(fCentfilename.Data());
  if (gSystem->AccessPathName(path)) {
     AliError(Form("File %s does not exist", path.Data()));
     return;
  }
  fFile  = TFile::Open(fCentfilename);
  owd->cd();
  fHtempV0M  = (TH1F*) (fFile->Get("hmultV0_percentile"));
  fHtempFMD  = (TH1F*) (fFile->Get("hmultFMD_percentile"));
  fHtempTRK  = (TH1F*) (fFile->Get("hNtracks_percentile"));
  fHtempTKL  = (TH1F*) (fFile->Get("hNtracklets_percentile"));
  fHtempCL0  = (TH1F*) (fFile->Get("hNclusters0_percentile"));
  fHtempCL1  = (TH1F*) (fFile->Get("hNclusters1_percentile"));

  if (!fHtempV0M) AliWarning(Form("Calibration for V0M does not exist in %s", path.Data()));
  if (!fHtempFMD) AliWarning(Form("Calibration for FMD does not exist in %s", path.Data()));
  if (!fHtempTRK) AliWarning(Form("Calibration for TRK does not exist in %s", path.Data()));
  if (!fHtempTKL) AliWarning(Form("Calibration for TKL does not exist in %s", path.Data()));
  if (!fHtempCL0) AliWarning(Form("Calibration for CL0 does not exist in %s", path.Data()));
  if (!fHtempCL1) AliWarning(Form("Calibration for CL1 does not exist in %s", path.Data()));
  
  owd->cd();
}  

//________________________________________________________________________
void AliCentralitySelectionTask::ReadCentralityHistos2(TString fCentfilename2) 
{
  //  Read centrality histograms
  TDirectory *owd = gDirectory;
  TString path = gSystem->ExpandPathName(fCentfilename2.Data());
  if (gSystem->AccessPathName(path)) {
     AliError(Form("File %s does not exist", path.Data()));
     return;
  }   
  fFile2  = TFile::Open(fCentfilename2);
  owd->cd();
  fHtempV0MvsFMD =  (TH1F*) (fFile2->Get("hmultV0vsmultFMD_all_percentile"));
  fHtempTKLvsV0M  = (TH1F*) (fFile2->Get("hNtrackletsvsmultV0_all_percentile"));
  fHtempZEMvsZDC  = (TH1F*) (fFile2->Get("hEzemvsEzdc_all_percentile"));

  if (!fHtempV0MvsFMD) AliWarning(Form("Calibration for V0MvsFMD does not exist in %s", path.Data()));
  if (!fHtempTKLvsV0M) AliWarning(Form("Calibration for TKLvsV0M does not exist in %s", path.Data()));
  if (!fHtempZEMvsZDC) AliWarning(Form("Calibration for ZEMvsZDC does not exist in %s", path.Data()));
  
  owd->cd();
}

//________________________________________________________________________
void AliCentralitySelectionTask::Terminate(Option_t */*option*/)
{
  // Terminate analysis
  if (fFile && fFile->IsOpen())
    fFile->Close();  
  if (fFile2 && fFile2->IsOpen())
    fFile2->Close();  
}
//________________________________________________________________________
Int_t AliCentralitySelectionTask::SetupRun(AliESDEvent* esd)
{
  // Setup files for run

  if (!esd)
    return -1;

  // check if something to be done
  if (fCurrentRun == esd->GetRunNumber())
    return 0;
  else
    fCurrentRun = esd->GetRunNumber();
  
  AliInfo(Form("Setup Centrality Selection for run %d\n",fCurrentRun));

  // CHANGE HERE FOR RUN RANGES
  if ( fCurrentRun <= 137165 ) fRunNo = 137161;
  else fRunNo = 137366;
  // CHANGE HERE FOR RUN RANGES
  
  TString fileName(Form("%s/COMMON/CENTRALITY/data/AliCentralityBy1D_%d.root", AliAnalysisManager::GetOADBPath(), fRunNo));
  TString fileName2(Form("%s/COMMON/CENTRALITY/data/AliCentralityByFunction_%d.root", AliAnalysisManager::GetOADBPath(), fRunNo));
  
  AliInfo(Form("Centrality Selection for run %d is initialized with %s", fCurrentRun, fileName.Data()));
  ReadCentralityHistos(fileName.Data());
  ReadCentralityHistos2(fileName2.Data());
  if (!fFile && !fFile2) {
     AliFatal(Form("Run %d not known to centrality selection!", fCurrentRun));       
     return -1;
  }   
  return 0;
}

//________________________________________________________________________
Bool_t AliCentralitySelectionTask::IsOutlierV0MSPD(Float_t spd, Float_t v0, Int_t cent)
{
  TF1 *V0MSPDfun = new TF1("V0MSPDfun","-0.143789+ 0.288874*x",0,25000);
  Float_t SPDsigma[100]={231.483, 189.446, 183.359, 179.923, 174.229, 170.309, 165.021, 
			 160.84, 159.33, 154.453, 151.644, 148.337, 145.215, 142.353, 
			 139.351, 136, 133.838, 129.885, 127.36, 125.032, 122.21, 120.3, 
			 117.766, 114.77, 113.1, 110.268, 107.463, 105.293, 102.845, 
			 100.835, 98.9632, 97.3287, 93.6887, 92.1066, 89.3224, 87.8382, 
			 86.04, 83.6431, 81.9655, 80.0491, 77.8208, 76.4716, 74.2165, 
			 72.2752, 70.4875, 68.9414, 66.8622, 65.022, 63.5134, 61.8228, 
			 59.7166, 58.5008, 56.2789, 54.9615, 53.386, 51.2165, 49.4842, 
			 48.259, 47.1129, 45.3115, 43.8486, 41.9207, 40.5754, 39.3872, 
			 38.1897, 36.5401, 35.1283, 33.9702, 32.6429, 31.3612, 29.5876, 
			 28.9319, 27.564, 26.0443, 25.2836, 23.9753, 22.8936, 21.5665, 
			 20.7048, 19.8016, 18.7095, 18.1144, 17.2095, 16.602, 16.3233, 
			 15.7185, 15.3006, 14.7432, 14.4174, 14.0805, 13.7638, 13.7638, 
			 13.7638, 13.7638, 13.7638, 13.7638, 13.7638, 13.7638, 13.7638, 18.0803};

  if ( TMath::Abs(spd-V0MSPDfun->Eval(v0)) > fOutliersCut*SPDsigma[cent] ) 
    return kTRUE;
  else 
    return kFALSE;
}

//________________________________________________________________________
Bool_t AliCentralitySelectionTask::IsOutlierV0MTPC(Int_t tracks, Float_t v0, Int_t cent)
{
  TF1 *V0MTPCfun = new TF1("V0MTPCfun","-0.540691+0.128358*x",0,25000);
  Float_t TPCsigma[100]={106.439, 89.2834, 86.7568, 85.3641, 83.379, 81.6093, 79.3189, 
			 78.0616, 77.2167, 75.0021, 73.9957, 72.0926, 71.0442, 69.8395, 
			 68.1169, 66.6676, 66.0038, 64.2284, 63.3845, 61.7439, 60.642, 
			 59.5383, 58.3696, 57.0227, 56.0619, 54.7108, 53.8382, 52.3398, 
			 51.5297, 49.9488, 49.1126, 48.208, 46.8566, 45.7724, 44.7829, 
			 43.8726, 42.7499, 41.9307, 40.6874, 39.9619, 38.5534, 38.0884, 
			 36.6141, 35.7482, 34.8804, 34.1769, 33.1278, 32.3435, 31.4783, 
			 30.2587, 29.4741, 28.8575, 27.9298, 26.9752, 26.1675, 25.1234, 
			 24.4702, 23.6843, 22.9764, 21.8579, 21.2924, 20.3241, 19.8296, 
			 19.2465, 18.4474, 17.7216, 16.8956, 16.342, 15.626, 15.0329, 
			 14.3911, 13.9301, 13.254, 12.6745, 12.2436, 11.7776, 11.1795, 
			 10.673, 10.27, 9.95646, 9.50939, 9.26162, 8.95315, 8.73439, 
			 8.67375, 8.43029, 8.34818, 8.33484, 8.40709, 8.3974, 8.32814, 
			 8.32814, 8.32814, 8.32814, 8.32814, 8.32814, 8.32814, 8.32814, 8.32814, 12.351};

  if ( TMath::Abs(tracks-V0MTPCfun->Eval(v0)) > fOutliersCut*TPCsigma[cent] ) 
    return kTRUE;
  else 
    return kFALSE;
}

//________________________________________________________________________
Bool_t AliCentralitySelectionTask::IsOutlierV0MZDC(Float_t zdc, Float_t v0)
{
  TF1 *fun1 = new TF1("fun1","6350-0.26*x",0,25000);
  TF1 *fun2 = new TF1("fun2","5580",0,25000);

  if ( (zdc > fun1->Eval(v0)) || (zdc > fun2->Eval(v0)) )
    return kTRUE;
  else 
    return kFALSE;
}

//________________________________________________________________________  
Float_t AliCentralitySelectionTask::MyGetScaleFactor(Int_t runnumber, Int_t flag) 
{
  if (! (runnumber >= fLowRunN && runnumber <=fHighRunN)) {
    cout << "MyGetScaleFactor error in run number range " << runnumber << endl;
    return 0.0;
  }

  Float_t scalefactor=0.0;
  if (flag==0)
    scalefactor = V0MScaleFactor[runnumber - fLowRunN]; // subtracting reference offset index
  else if (flag==1)
    scalefactor = SPDScaleFactor[runnumber - fLowRunN]; // subtracting reference offset index
  else if (flag==2)
    scalefactor = TPCScaleFactor[runnumber - fLowRunN]; // subtracting reference offset index

  return scalefactor;

}

//________________________________________________________________________  
Float_t AliCentralitySelectionTask::MyGetScaleFactorMC(Int_t runnumber) 
{
  if (! (runnumber >= fLowRunN && runnumber <=fHighRunN)) {
    cout << "MyGetScaleFactor error in run number range " << runnumber << endl;
    return 0.0;
  }

  Float_t scalefactor= V0MScaleFactorMC[runnumber - fLowRunN]; // subtracting reference offset index
  return scalefactor;

}

//________________________________________________________________________  
void AliCentralitySelectionTask::MyInitScaleFactor () 
{
  for (int i=0; i<(fHighRunN-fLowRunN); i++) V0MScaleFactor[i] = 0.0;
  for (int i=0; i<(fHighRunN-fLowRunN); i++) SPDScaleFactor[i] = 0.0;
  for (int i=0; i<(fHighRunN-fLowRunN); i++) TPCScaleFactor[i] = 0.0;
  
  // scale factors determined from <V0 charge> on a run-by-run basis
  V0MScaleFactor[0] = 0.956841;
  V0MScaleFactor[1] = 0.958274;
  V0MScaleFactor[204] = 1.0046;
  V0MScaleFactor[205] = 0.983535;
  V0MScaleFactor[269] = 0.988185;
  V0MScaleFactor[270] = 0.983351;
  V0MScaleFactor[271] = 0.989013;
  V0MScaleFactor[273] = 0.990056;
  V0MScaleFactor[278] = 0.974438;
  V0MScaleFactor[279] = 0.981572;
  V0MScaleFactor[280] = 0.989316;
  V0MScaleFactor[282] = 0.98345;
  V0MScaleFactor[378] = 0.993647;
  V0MScaleFactor[380] = 0.994758;
  V0MScaleFactor[383] = 0.989569;
  V0MScaleFactor[388] = 0.993119;
  V0MScaleFactor[434] = 0.989583;
  V0MScaleFactor[447] = 0.990377;
  V0MScaleFactor[477] = 0.990176;
  V0MScaleFactor[478] = 0.98723;
  V0MScaleFactor[524] = 1.00403;
  V0MScaleFactor[525] = 0.994376;
  V0MScaleFactor[530] = 0.99759;
  V0MScaleFactor[532] = 1.01031;
  V0MScaleFactor[590] = 0.996216;
  V0MScaleFactor[591] = 0.994205;
  V0MScaleFactor[682] = 0.998479;
  V0MScaleFactor[687] = 1.00078;
  V0MScaleFactor[989] = 1.00515;
  V0MScaleFactor[993] = 1.00094;
  V0MScaleFactor[1029] = 0.986596;
  V0MScaleFactor[1036] = 0.972226;
  V0MScaleFactor[1039] = 0.960358;
  V0MScaleFactor[1040] = 0.970023;
  V0MScaleFactor[1064] = 1.00575;
  V0MScaleFactor[1235] = 1.00471;
  V0MScaleFactor[1277] = 1.00611;
  V0MScaleFactor[1278] = 1.00976;
  V0MScaleFactor[1308] = 1.00771;
  V0MScaleFactor[1372] = 1.01622;
  V0MScaleFactor[1417] = 1.01305;
  V0MScaleFactor[1418] = 1.00388;
  V0MScaleFactor[1421] = 1.00673;
  V0MScaleFactor[1422] = 1.00916;
  V0MScaleFactor[1460] = 1.0092;
  V0MScaleFactor[1463] = 1.00728;
  V0MScaleFactor[1476] = 1.01655;
  V0MScaleFactor[1477] = 1.00672;
  V0MScaleFactor[1491] = 0.983339;
  V0MScaleFactor[1492] = 1.00754;
  V0MScaleFactor[1501] = 1.00608;
  V0MScaleFactor[1505] = 1.01227;
  V0MScaleFactor[1569] = 0.99907;
  V0MScaleFactor[1571] = 0.995696;
  V0MScaleFactor[1876] = 1.00559;
  V0MScaleFactor[1877] = 1.00631;
  V0MScaleFactor[1944] = 1.01512;
  V0MScaleFactor[1946] = 0.998727;
  V0MScaleFactor[2011] = 1.00701;
  
  SPDScaleFactor[0] = 1.00211;
  SPDScaleFactor[1] = 1.00067;
  SPDScaleFactor[204] = 1.02268;
  SPDScaleFactor[205] = 0.994902;
  SPDScaleFactor[269] = 1.00215;
  SPDScaleFactor[270] = 0.993421;
  SPDScaleFactor[271] = 1.00129;
  SPDScaleFactor[273] = 1.00242;
  SPDScaleFactor[278] = 0.984762;
  SPDScaleFactor[279] = 0.994355;
  SPDScaleFactor[280] = 1.00073;
  SPDScaleFactor[282] = 0.995889;
  SPDScaleFactor[378] = 0.994532;
  SPDScaleFactor[380] = 0.998307;
  SPDScaleFactor[383] = 0.994052;
  SPDScaleFactor[388] = 0.993224;
  SPDScaleFactor[434] = 0.993279;
  SPDScaleFactor[447] = 0.992494;
  SPDScaleFactor[477] = 0.992678;
  SPDScaleFactor[478] = 0.996563;
  SPDScaleFactor[524] = 1.01116;
  SPDScaleFactor[525] = 0.993108;
  SPDScaleFactor[530] = 0.997574;
  SPDScaleFactor[532] = 1.01829;
  SPDScaleFactor[590] = 0.999438;
  SPDScaleFactor[591] = 0.995849;
  SPDScaleFactor[682] = 0.999227;
  SPDScaleFactor[687] = 1.00575;
  SPDScaleFactor[989] = 0.99877;
  SPDScaleFactor[993] = 0.999682;
  SPDScaleFactor[1029] = 0.978198;
  SPDScaleFactor[1036] = 0.964178;
  SPDScaleFactor[1039] = 0.959439;
  SPDScaleFactor[1040] = 0.956945;
  SPDScaleFactor[1064] = 0.994434;
  SPDScaleFactor[1235] = 1.0016;
  SPDScaleFactor[1277] = 1.00153;
  SPDScaleFactor[1278] = 1.00698;
  SPDScaleFactor[1308] = 1.00554;
  SPDScaleFactor[1372] = 1.0123;
  SPDScaleFactor[1417] = 1.011;
  SPDScaleFactor[1418] = 1.00143;
  SPDScaleFactor[1421] = 1.00486;
  SPDScaleFactor[1422] = 1.00646;
  SPDScaleFactor[1460] = 1.00515;
  SPDScaleFactor[1463] = 1.00485;
  SPDScaleFactor[1476] = 1.01215;
  SPDScaleFactor[1477] = 1.00419;
  SPDScaleFactor[1491] = 0.983327;
  SPDScaleFactor[1492] = 1.00529;
  SPDScaleFactor[1501] = 1.00367;
  SPDScaleFactor[1505] = 1.01045;
  SPDScaleFactor[1569] = 0.996374;
  SPDScaleFactor[1571] = 0.988827;
  SPDScaleFactor[1876] = 1.00354;
  SPDScaleFactor[1877] = 1.00397;
  SPDScaleFactor[1944] = 1.01138;
  SPDScaleFactor[1946] = 0.996641;
  SPDScaleFactor[2011] = 1.00357;

  TPCScaleFactor[0] = 1.00434;
  TPCScaleFactor[1] = 1.0056;
  TPCScaleFactor[204] = 1.02185;
  TPCScaleFactor[205] = 1.0036;
  TPCScaleFactor[269] = 1.00607;
  TPCScaleFactor[270] = 1.00183;
  TPCScaleFactor[271] = 1.00693;
  TPCScaleFactor[273] = 1.00746;
  TPCScaleFactor[278] = 0.990524;
  TPCScaleFactor[279] = 0.998582;
  TPCScaleFactor[280] = 1.00618;
  TPCScaleFactor[282] = 1.00088;
  TPCScaleFactor[378] = 1.00598;
  TPCScaleFactor[380] = 1.00658;
  TPCScaleFactor[383] = 1.00144;
  TPCScaleFactor[388] = 1.00279;
  TPCScaleFactor[434] = 1.00122;
  TPCScaleFactor[447] = 1.002;
  TPCScaleFactor[477] = 0.997818;
  TPCScaleFactor[478] = 0.994583;
  TPCScaleFactor[524] = 1.01508;
  TPCScaleFactor[525] = 1.00218;
  TPCScaleFactor[530] = 1.00569;
  TPCScaleFactor[532] = 1.01789;
  TPCScaleFactor[590] = 1.00739;
  TPCScaleFactor[591] = 1.00462;
  TPCScaleFactor[682] = 1.00502;
  TPCScaleFactor[687] = 1.00943;
  TPCScaleFactor[989] = 1.00438;
  TPCScaleFactor[993] = 0.996701;
  TPCScaleFactor[1029] = 0.978641;
  TPCScaleFactor[1036] = 0.968906;
  TPCScaleFactor[1039] = 0.954311;
  TPCScaleFactor[1040] = 0.958764;
  TPCScaleFactor[1064] = 0.997899;
  TPCScaleFactor[1235] = 0.992;
  TPCScaleFactor[1277] = 0.992635;
  TPCScaleFactor[1278] = 1.00207;
  TPCScaleFactor[1308] = 1.00126;
  TPCScaleFactor[1372] = 1.00324;
  TPCScaleFactor[1417] = 1.00042;
  TPCScaleFactor[1418] = 0.978881;
  TPCScaleFactor[1421] = 0.999818;
  TPCScaleFactor[1422] = 1.00109;
  TPCScaleFactor[1460] = 0.99935;
  TPCScaleFactor[1463] = 0.998531;
  TPCScaleFactor[1476] = 0.999125;
  TPCScaleFactor[1477] = 0.998479;
  TPCScaleFactor[1491] = 0.9775;
  TPCScaleFactor[1492] = 0.999095;
  TPCScaleFactor[1501] = 0.998197;
  TPCScaleFactor[1505] = 1.00413;
  TPCScaleFactor[1569] = 0.990916;
  TPCScaleFactor[1571] = 0.987241;
  TPCScaleFactor[1876] = 1.00048;
  TPCScaleFactor[1877] = 1.00057;
  TPCScaleFactor[1944] = 1.00588;
  TPCScaleFactor[1946] = 0.991503;
  TPCScaleFactor[2011] = 1.00057;

  // set all missing values to the value of the run before it ....
  for (int i=0; i<(fHighRunN-fLowRunN); i++) {    
    if (V0MScaleFactor[i] == 0.0) {     
      if (i==0) {
	V0MScaleFactor[i] = 1.00;
      } else {
	// search for last run number with non-zero value
	for (int j=i-1;j>=0;j--) {
	  if (V0MScaleFactor[j] != 0.0) {
	    V0MScaleFactor[i] = V0MScaleFactor[j];
	    break;
	  }
	}
      }
    }
  } // end loop over checking all run-numbers 

  for (int i=0; i<(fHighRunN-fLowRunN); i++) {    
    if (SPDScaleFactor[i] == 0.0) {     
      if (i==0) {
	SPDScaleFactor[i] = 1.00;
      } else {
	for (int j=i-1;j>=0;j--) {
	  if (SPDScaleFactor[j] != 0.0) {
	    SPDScaleFactor[i] = SPDScaleFactor[j];
	    break;
	  }
	}
      }
    }
  } 

  for (int i=0; i<(fHighRunN-fLowRunN); i++) {    
    if (TPCScaleFactor[i] == 0.0) {     
      if (i==0) {
	TPCScaleFactor[i] = 1.00;
      } else {
	for (int j=i-1;j>=0;j--) {
	  if (TPCScaleFactor[j] != 0.0) {
	    TPCScaleFactor[i] = TPCScaleFactor[j];
	    break;
	  }
	}
      }
    }
  } 
  

  //    for (int i=0; i<(fHighRunN-fLowRunN); i++) cout << "Scale Factor = " << V0MScaleFactor[i] << " for Run " << i+fLowRunN << endl;
  
  return;

}


//________________________________________________________________________  
void AliCentralitySelectionTask::MyInitScaleFactorMC() 
{
  for (int i=0; i<(fHighRunN-fLowRunN); i++) V0MScaleFactorMC[i] = 0.0;
  // scale factors determined from <V0 charge> on a run-by-run basis
  V0MScaleFactor[0] = 0.75108;
  // set all missing values to the value of the run before it ....
  for (int i=0; i<(fHighRunN-fLowRunN); i++) {    
    if (V0MScaleFactorMC[i] == 0.0) {     
      if (i==0) {
	V0MScaleFactorMC[i] = 1.00;
      } else {
	// search for last run number with non-zero value
	for (int j=i-1;j>=0;j--) {
	  if (V0MScaleFactorMC[j] != 0.0) {
	    V0MScaleFactorMC[i] = V0MScaleFactorMC[j];
	    break;
	  }
	}
      }
    }
  } // end loop over checking all run-numbers 


  //    for (int i=0; i<(fHighRunN-fLowRunN); i++) cout << "Scale Factor = " << V0MScaleFactorMC[i] << " for Run " << i+fLowRunN << endl;
  
  return;

}

