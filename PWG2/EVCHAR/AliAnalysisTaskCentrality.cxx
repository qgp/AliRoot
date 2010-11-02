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

/////////////////////////////////////////////////////////////
//							   //
//	Class to analyze centrality measurements           //
//							   //
/////////////////////////////////////////////////////////////

#include <TTree.h>
#include <TList.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile.h>
#include <TFile.h>
#include <TString.h>
#include <TCanvas.h>

#include "AliAnalysisManager.h"
#include "AliVEvent.h"
#include "AliESD.h"
#include "AliESDEvent.h"
#include "AliESDHeader.h"
#include "AliESDInputHandler.h"
#include "AliESDZDC.h"
#include "AliESDFMD.h"
#include "AliESDVZERO.h"
#include "AliMultiplicity.h"
#include "AliAODHandler.h"
#include "AliAODEvent.h"
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
#include "AliAnalysisTaskCentrality.h"

ClassImp(AliAnalysisTaskCentrality)


//________________________________________________________________________
AliAnalysisTaskCentrality::AliAnalysisTaskCentrality():
  AliAnalysisTaskSE(),
  fDebug(0),
  fAnalysisInput("ESD"),
  fIsMCInput(kFALSE),
  fOutput(0x0),
  hEzdc(0x0),
  hEzem(0x0),
  hNtracks(0x0),
  hNtracklets(0x0),
  hNclusters0(0x0),
  hmultV0(0x0),
  hmultFMD(0x0),
  hEzemvsEzdc(0x0),
  hNtracksvsEzdc(0x0),
  hNtrackletsvsEzdc(0x0),
  hNclusters0vsEzdc(0x0),
  hmultV0vsEzdc(0x0),
  hmultFMDvsEzdc(0x0),
  hNtracksvsEzem(0x0),
  hNtrackletsvsEzem(0x0),
  hNclusters0vsEzem(0x0),
  hmultV0vsEzem(0x0),
  hmultFMDvsEzem(0x0),
  hNtracksvsmultV0(0x0),
  hNtrackletsvsmultV0(0x0),
  hNclusters0vsmultV0(0x0),
  hNtracksvsmultFMD(0x0),
  hNtrackletsvsmultFMD(0x0),
  hNclusters0vsmultFMD(0x0),
  hmultV0vsmultFMD(0x0)
{   
   // Default constructor
}   

//________________________________________________________________________
AliAnalysisTaskCentrality::AliAnalysisTaskCentrality(const char *name):
  AliAnalysisTaskSE(name),
  fDebug(0),
  fAnalysisInput("ESD"),
  fIsMCInput(kFALSE),
  fOutput(0x0),
  hEzdc(0x0),
  hEzem(0x0),
  hNtracks(0x0),
  hNtracklets(0x0),
  hNclusters0(0x0),
  hmultV0(0x0),
  hmultFMD(0x0),
  hEzemvsEzdc(0x0),
  hNtracksvsEzdc(0x0),
  hNtrackletsvsEzdc(0x0),
  hNclusters0vsEzdc(0x0),
  hmultV0vsEzdc(0x0),
  hmultFMDvsEzdc(0x0),
  hNtracksvsEzem(0x0),
  hNtrackletsvsEzem(0x0),
  hNclusters0vsEzem(0x0),
  hmultV0vsEzem(0x0),
  hmultFMDvsEzem(0x0),
  hNtracksvsmultV0(0x0),
  hNtrackletsvsmultV0(0x0),
  hNclusters0vsmultV0(0x0),
  hNtracksvsmultFMD(0x0),
  hNtrackletsvsmultFMD(0x0),
  hNclusters0vsmultFMD(0x0),
  hmultV0vsmultFMD(0x0)
{
  // Default constructor
  
  // Output slot #1 writes into a TList container
  DefineOutput(1, TList::Class()); 

}

//________________________________________________________________________
AliAnalysisTaskCentrality& AliAnalysisTaskCentrality::operator=(const AliAnalysisTaskCentrality& c)
{
  //
  // Assignment operator
  //
  if (this!=&c) {
    AliAnalysisTaskSE::operator=(c);
  }
  return *this;
}

//________________________________________________________________________
AliAnalysisTaskCentrality::AliAnalysisTaskCentrality(const AliAnalysisTaskCentrality& ana):
  AliAnalysisTaskSE(ana),
  fDebug(ana.fDebug),	  
  fAnalysisInput(ana.fDebug),
  fIsMCInput(ana.fIsMCInput),
  fOutput(ana.fOutput),
  hEzdc(ana.hEzdc),
  hEzem(ana.hEzem),
  hNtracks(ana.hNtracks),
  hNtracklets(ana.hNtracklets),
  hNclusters0(ana.hNclusters0),
  hmultV0(ana.hmultV0),
  hmultFMD(ana.hmultFMD),
  hEzemvsEzdc(ana.hEzemvsEzdc),
  hNtracksvsEzdc(ana.hNtracksvsEzdc),
  hNtrackletsvsEzdc(ana.hNtrackletsvsEzdc),
  hNclusters0vsEzdc(ana.hNclusters0vsEzdc),
  hmultV0vsEzdc(ana.hmultV0vsEzdc),
  hmultFMDvsEzdc(ana.hmultFMDvsEzdc),
  hNtracksvsEzem(ana.hNtracksvsEzem),
  hNtrackletsvsEzem(ana.hNtrackletsvsEzem),
  hNclusters0vsEzem(ana.hNclusters0vsEzem),
  hmultV0vsEzem(ana.hmultV0vsEzem),
  hmultFMDvsEzem(ana.hmultFMDvsEzem),
  hNtracksvsmultV0(ana.hNtracksvsmultV0),
  hNtrackletsvsmultV0(ana.hNtrackletsvsmultV0),
  hNclusters0vsmultV0(ana.hNclusters0vsmultV0),
  hNtracksvsmultFMD(ana.hNtracksvsmultFMD),
  hNtrackletsvsmultFMD(ana.hNtrackletsvsmultFMD),
  hNclusters0vsmultFMD(ana.hNclusters0vsmultFMD),
  hmultV0vsmultFMD(ana.hmultV0vsmultFMD)
{
  //
  // Copy Constructor	
  //
}
 
//________________________________________________________________________
 AliAnalysisTaskCentrality::~AliAnalysisTaskCentrality()
 {
   // Destructor
   if(fOutput){
     delete fOutput; fOutput=0;
   } 
 }  

//________________________________________________________________________
void AliAnalysisTaskCentrality::UserCreateOutputObjects()
{  

  // Create the output containers
  if(fDebug>1) printf("AnalysisTaskZDCpp::UserCreateOutputObjects() \n");

  // Several histograms are more conveniently managed in a TList
  fOutput = new TList();
  fOutput->SetOwner();
  fOutput->SetName("OutputHistos");

  hEzdc         = new TH1F("hEzdc","hEzdc",500,0,150);
  hEzem         = new TH1F("hEzem","hEzem",500,0,5);
  hNtracks      = new TH1F("hNtracks","hNtracks",500,0,17000);
  hNtracklets   = new TH1F("hNtracklets","hNtracklets",500,0,10000);
  hNclusters0   = new TH1F("hNclusters0","hNclusters0",500,0,15000);
  hmultV0       = new TH1F("hmultV0","hmultV0",500,0,30000);
  hmultFMD      = new TH1F("hmultFMD","hmultFMD",500,0,24000);

  hEzemvsEzdc         = new TProfile("hEzemvsEzdc","hEzemvsEzdc",500,0,5,"");
  hNtracksvsEzdc      = new TProfile("hNtracksvsEzdc","hNtracksvsEzdc",500,0,17000,"");
  hNtrackletsvsEzdc   = new TProfile("hNtrackletsvsEzdc","hNtrackletsvsEzdc",500,0,10000,"");
  hNclusters0vsEzdc   = new TProfile("hNclusters0vsEzdc","hNclusters0vsEzdc",500,0,15000,"");
  hmultV0vsEzdc       = new TProfile("hmultV0vsEzdc","hmultV0vsEzdc",500,0,30000,"");
  hmultFMDvsEzdc      = new TProfile("hmultFMDvsEzdc","hmultFMDvsEzdc",500,0,24000,"");
  hNtracksvsEzem      = new TProfile("hNtracksvsEzem","hNtracksvsEzem",500,0,17000,"");
  hNtrackletsvsEzem   = new TProfile("hNtrackletsvsEzem","hNtrackletsvsEzem",500,0,10000,"");
  hNclusters0vsEzem   = new TProfile("hNclusters0vsEzem","hNclusters0vsEzem",500,0,15000,"");
  hmultV0vsEzem       = new TProfile("hmultV0vsEzem","hmultV0vsEzem",500,0,30000,"");
  hmultFMDvsEzem      = new TProfile("hmultFMDvsEzem","hmultFMDvsEzem",500,0,24000,"");
  hNtracksvsmultV0    = new TProfile("hNtracksvsmultV0","hNtracksvsmultV0",500,0,17000,"");      
  hNtrackletsvsmultV0 = new TProfile("hNtrackletsvsmultV0","hNtrackletsvsmultV0",500,0,10000,"");    
  hNclusters0vsmultV0 = new TProfile("hNclusters0vsmultV0","hNclusters0vsmultV0",500,0,15000,"");
  hNtracksvsmultFMD   = new TProfile("hNtracksvsmultFMD","hNtracksvsmultFMD",500,0,17000,"");
  hNtrackletsvsmultFMD= new TProfile("hNtrackletsvsmultFMD","hNtrackletsvsmultFMD",500,0,10000,"");
  hNclusters0vsmultFMD= new TProfile("hNclusters0vsmultFMD","hNclusters0vsmultFMD",500,0,15000,"");		   
  hmultV0vsmultFMD    = new TProfile("hmultV0vsmultFMD","hmultV0vsmultFMD",500,0,30000,"");

  hEzdc         ->GetXaxis()->SetTitle("E_{ZDC}[TeV]");
  hEzem         ->GetXaxis()->SetTitle("E_{ZEM}[TeV]");
  hNtracks      ->GetXaxis()->SetTitle("N_{tracks}");
  hNtracklets   ->GetXaxis()->SetTitle("N_{tracklets}");
  hNclusters0   ->GetXaxis()->SetTitle("N_{clusters0}");
  hmultV0       ->GetXaxis()->SetTitle("V0 mult");
  hmultFMD      ->GetXaxis()->SetTitle("FMD mult");
  
  hEzemvsEzdc         ->GetYaxis()->SetTitle("E_{ZDC}[TeV]");
  hNtracksvsEzdc      ->GetYaxis()->SetTitle("E_{ZDC}[TeV]");
  hNtrackletsvsEzdc   ->GetYaxis()->SetTitle("E_{ZDC}[TeV]");
  hNclusters0vsEzdc   ->GetYaxis()->SetTitle("E_{ZDC}[TeV]");
  hmultV0vsEzdc       ->GetYaxis()->SetTitle("E_{ZDC}[TeV]");
  hmultFMDvsEzdc      ->GetYaxis()->SetTitle("E_{ZDC}[TeV]");
  hNtracksvsEzem      ->GetYaxis()->SetTitle("E_{ZEM}[TeV]");
  hNtrackletsvsEzem   ->GetYaxis()->SetTitle("E_{ZEM}[TeV]");
  hNclusters0vsEzem   ->GetYaxis()->SetTitle("E_{ZEM}[TeV]");
  hmultV0vsEzem       ->GetYaxis()->SetTitle("E_{ZEM}[TeV]");
  hmultFMDvsEzem      ->GetYaxis()->SetTitle("E_{ZEM}[TeV]");
  hNtracksvsmultV0    ->GetYaxis()->SetTitle("V0 mult");    
  hNtrackletsvsmultV0 ->GetYaxis()->SetTitle("V0 mult");  
  hNclusters0vsmultV0 ->GetYaxis()->SetTitle("V0 mult");
  hNtracksvsmultFMD   ->GetYaxis()->SetTitle("FMD mult");
  hNtrackletsvsmultFMD->GetYaxis()->SetTitle("FMD mult");
  hNclusters0vsmultFMD->GetYaxis()->SetTitle("FMD mult");
  hmultV0vsmultFMD    ->GetYaxis()->SetTitle("FMD mult");
  
  hEzemvsEzdc         ->GetXaxis()->SetTitle("E_{ZEM}[TeV]");
  hNtracksvsEzdc      ->GetXaxis()->SetTitle("N_{tracks}");
  hNtrackletsvsEzdc   ->GetXaxis()->SetTitle("N_{tracklets}");
  hNclusters0vsEzdc   ->GetXaxis()->SetTitle("N_{clusters0}");
  hmultV0vsEzdc       ->GetXaxis()->SetTitle("V0 mult");
  hmultFMDvsEzdc      ->GetXaxis()->SetTitle("FMD mult");
  hNtracksvsEzem      ->GetXaxis()->SetTitle("N_{tracks}");
  hNtrackletsvsEzem   ->GetXaxis()->SetTitle("N_{tracklets}");
  hNclusters0vsEzem   ->GetXaxis()->SetTitle("N_{clusters0}");
  hmultV0vsEzem       ->GetXaxis()->SetTitle("V0 mult");
  hmultFMDvsEzem      ->GetXaxis()->SetTitle("FMD mult");
  hNtracksvsmultV0    ->GetXaxis()->SetTitle("N_{tracks}");    
  hNtrackletsvsmultV0 ->GetXaxis()->SetTitle("N_{tracklets}");  
  hNclusters0vsmultV0 ->GetXaxis()->SetTitle("N_{clusters0}");
  hNtracksvsmultFMD   ->GetXaxis()->SetTitle("N_{tracks}");
  hNtrackletsvsmultFMD->GetXaxis()->SetTitle("N_{tracklets}");
  hNclusters0vsmultFMD->GetXaxis()->SetTitle("N_{clusters}");
  hmultV0vsmultFMD    ->GetXaxis()->SetTitle("V0 mult");
  
  fOutput->Add(hEzdc);
  fOutput->Add(hEzem);
  fOutput->Add(hNtracks);
  fOutput->Add(hNtracklets);
  fOutput->Add(hNclusters0);
  fOutput->Add(hmultV0);
  fOutput->Add(hmultFMD);

  fOutput->Add(hEzemvsEzdc);
  fOutput->Add(hNtracksvsEzdc);
  fOutput->Add(hNtrackletsvsEzdc);
  fOutput->Add(hNclusters0vsEzdc);
  fOutput->Add(hmultV0vsEzdc);
  fOutput->Add(hmultFMDvsEzdc);
  fOutput->Add(hNtracksvsEzem);
  fOutput->Add(hNtrackletsvsEzem);
  fOutput->Add(hNclusters0vsEzem);
  fOutput->Add(hmultV0vsEzem);
  fOutput->Add(hmultFMDvsEzem);
  fOutput->Add(hNtracksvsmultV0);
  fOutput->Add(hNtrackletsvsmultV0);
  fOutput->Add(hNclusters0vsmultV0);
  fOutput->Add(hNtracksvsmultFMD);
  fOutput->Add(hNtrackletsvsmultFMD);
  fOutput->Add(hNclusters0vsmultFMD);
  fOutput->Add(hmultV0vsmultFMD);
  
  PostData(1, fOutput);

}

//________________________________________________________________________
void AliAnalysisTaskCentrality::UserExec(Option_t */*option*/)
{ 
  // Execute analysis for current event:
  if(fDebug>1) printf(" **** AliAnalysisTaskCentrality::UserExec() \n");
  
  if(fAnalysisInput.CompareTo("ESD")==0){

    AliVEvent* event = InputEvent();
    AliESDEvent* esd = dynamic_cast<AliESDEvent*>(event);
    
      fNev++;

      fNTracks    = event->GetNumberOfTracks();     
      fNPmdTracks = esd->GetNumberOfPmdTracks();     

      AliESDVZERO* esdV0 = esd->GetVZEROData();
      fMultV0A=esdV0->GetMTotV0A();
      fMultV0C=esdV0->GetMTotV0C();

      if(fIsMCInput){

        AliMCEvent* mcEvent = MCEvent();
        if (!mcEvent) {
          printf("   Could not retrieve MC event!!!\n");
          return;
        }

	fNmyTracks_gen = 0;
	AliStack *stack = 0x0; // needed for MC studies
	stack = MCEvent()->Stack();
	for (Int_t iTrack = 0; iTrack < MCEvent()->GetNumberOfTracks(); iTrack++) {
	  //get properties of mc particle
	  AliMCParticle* mcP = (AliMCParticle*) MCEvent()->GetTrack(iTrack);
	  // Primaries only
	  if (!(stack->IsPhysicalPrimary(mcP->Label()))) continue;
	  //charged tracks only
	  if (mcP->Particle()->GetPDG()->Charge() == 0) continue;
	  //same cuts as on ESDtracks
// 	  if(TMath::Abs(mcP->Eta())>0.9)continue;
// 	  if(mcP->Pt()<0.2)continue;
// 	  if(mcP->Pt()>200)continue;

	  fNmyTracks_gen ++;
	} 

        AliGenEventHeader* genHeader = mcEvent->GenEventHeader();
        if(!genHeader){
          printf("  Event generator header not available!!!\n");
	  return;
        }
	
	if(genHeader->InheritsFrom(AliGenHijingEventHeader::Class())){
          fbMC = ((AliGenHijingEventHeader*) genHeader)->ImpactParameter();
          Int_t specNeutronProj = ((AliGenHijingEventHeader*) genHeader)->ProjSpectatorsn();
          Int_t specProtonProj  = ((AliGenHijingEventHeader*) genHeader)->ProjSpectatorsp();
          Int_t specNeutronTarg = ((AliGenHijingEventHeader*) genHeader)->TargSpectatorsn();
          Int_t specProtonTarg  = ((AliGenHijingEventHeader*) genHeader)->TargSpectatorsp();
	  fNpartTargMC = 208.-(specNeutronTarg+specProtonTarg);
	  fNpartProjMC = 208.-(specNeutronProj+specProtonProj);
	  fNNColl   = ((AliGenHijingEventHeader*) genHeader)->NN();
	  fNNwColl  = ((AliGenHijingEventHeader*) genHeader)->NNw();
	  fNwNColl  = ((AliGenHijingEventHeader*) genHeader)->NwN();
	  fNwNwColl = ((AliGenHijingEventHeader*) genHeader)->NwNw();
	}  
	
      }
      
      fBeamEnergy = esd->GetBeamEnergy();

      // ***** Trigger selection
      TString triggerClass = esd->GetFiredTriggerClasses();
      sprintf(fTrigClass,"%s",triggerClass.Data());
          
      const AliESDVertex *vertex = esd->GetPrimaryVertexSPD();
      fxVertex = vertex->GetX();
      fyVertex = vertex->GetY();
      fzVertex = vertex->GetZ();
      if(vertex->IsFromVertexer3D()) fVertexer3d = kTRUE;
      else fVertexer3d = kFALSE;
      Double_t vertex3[3];
      vertex->GetXYZ(vertex3);

      const AliMultiplicity *mult = esd->GetMultiplicity();
      fNTracklets = mult->GetNumberOfTracklets();
     
      for(Int_t ilay=0; ilay<6; ilay++){
        fNClusters[ilay] = mult->GetNumberOfITSClusters(ilay);
      }
      fNSingleClusters = mult->GetNumberOfSingleClusters();

      for(Int_t ilay=0; ilay<2; ilay++){
        fNChips[ilay] = mult->GetNumberOfFiredChips(ilay);
      }


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
	fMultFMDA = totalMultA;
	fMultFMDC = totalMultC;
	

      AliESDZDC *esdZDC = esd->GetESDZDC();
      
      fESDFlag =  esdZDC->GetESDQuality();   
      
      fZNCEnergy = (Float_t) (esdZDC->GetZDCN1Energy());
      fZPCEnergy = (Float_t) (esdZDC->GetZDCP1Energy());
      fZNAEnergy = (Float_t) (esdZDC->GetZDCN2Energy());
      fZPAEnergy = (Float_t) (esdZDC->GetZDCP2Energy());
      fZEM1Energy = (Float_t) (esdZDC->GetZDCEMEnergy(0));
      fZEM2Energy = (Float_t) (esdZDC->GetZDCEMEnergy(1));
            
      fbZDC = esdZDC->GetImpactParameter();
      fNpartZDC = esdZDC->GetZDCParticipants();
      fbZDCA = esdZDC->GetImpactParamSideA();
      fNpartZDCA = esdZDC->GetZDCPartSideA();
      fbZDCC = esdZDC->GetImpactParamSideC();
      fNpartZDCC = esdZDC->GetZDCPartSideC();
      
      const Double_t * towZNC = esdZDC->GetZN1TowerEnergy();
      const Double_t * towZPC = esdZDC->GetZP1TowerEnergy();
      const Double_t * towZNA = esdZDC->GetZN2TowerEnergy();
      const Double_t * towZPA = esdZDC->GetZP2TowerEnergy();

      for(Int_t it=0; it<5; it++){
         fZNCtower[it] = (Float_t) (towZNC[it]);
         fZPCtower[it] = (Float_t) (towZPC[it]);
         fZNAtower[it] = (Float_t) (towZNA[it]); 
         fZPAtower[it] = (Float_t) (towZPA[it]);  
      }

      Double_t xyZNC[2]={-99.,-99.}, xyZNA[2]={-99.,-99.};
      esdZDC->GetZNCentroidInPbPb(fBeamEnergy, xyZNC, xyZNA);
      for(Int_t it=0; it<2; it++){
         fCentrZNC[it] = xyZNC[it];
         fCentrZNA[it] = xyZNA[it];
      }

      // filling histos

      hEzdc         ->Fill((fZNCEnergy+fZPCEnergy+fZNAEnergy+fZPAEnergy)/1000.);
      hEzem         ->Fill(fZEM1Energy+fZEM2Energy);
      hNtracks      ->Fill(fNTracks);
      hNtracklets   ->Fill(fNTracklets);
      hNclusters0   ->Fill(fNClusters[0]);
      hmultV0       ->Fill(fMultV0A+fMultV0C);
      hmultFMD      ->Fill(fMultFMDA+fMultFMDC);
      
      hEzemvsEzdc         ->Fill(fZEM1Energy+fZEM2Energy, (fZNCEnergy+fZPCEnergy+fZNAEnergy+fZPAEnergy)/1000.);
      hNtracksvsEzdc      ->Fill(fNTracks, (fZNCEnergy+fZPCEnergy+fZNAEnergy+fZPAEnergy)/1000.);
      hNtrackletsvsEzdc   ->Fill(fNTracklets,  (fZNCEnergy+fZPCEnergy+fZNAEnergy+fZPAEnergy)/1000.);
      hNclusters0vsEzdc   ->Fill(fNClusters[0],  (fZNCEnergy+fZPCEnergy+fZNAEnergy+fZPAEnergy)/1000.);
      hmultV0vsEzdc       ->Fill(fMultV0A+fMultV0C,  (fZNCEnergy+fZPCEnergy+fZNAEnergy+fZPAEnergy)/1000.);
      hmultFMDvsEzdc      ->Fill(fMultFMDA+fMultFMDC,  (fZNCEnergy+fZPCEnergy+fZNAEnergy+fZPAEnergy)/1000.);
      hNtracksvsEzem      ->Fill(fNTracks, fZEM1Energy+fZEM2Energy);
      hNtrackletsvsEzem   ->Fill(fNTracklets, fZEM1Energy+fZEM2Energy);
      hNclusters0vsEzem   ->Fill(fNClusters[0], fZEM1Energy+fZEM2Energy);
      hmultV0vsEzem       ->Fill(fMultV0A+fMultV0C, fZEM1Energy+fZEM2Energy);
      hmultFMDvsEzem      ->Fill(fMultFMDA+fMultFMDC, fZEM1Energy+fZEM2Energy);
      hNtracksvsmultV0    ->Fill(fNTracks,fMultV0A+fMultV0C);    
      hNtrackletsvsmultV0 ->Fill(fNTracklets,fMultV0A+fMultV0C);    
      hNclusters0vsmultV0 ->Fill(fNClusters[0],fMultV0A+fMultV0C);    
      hNtracksvsmultFMD   ->Fill(fNTracks,fMultFMDA+fMultFMDC);
      hNtrackletsvsmultFMD->Fill(fNTracklets,fMultFMDA+fMultFMDC);
      hNclusters0vsmultFMD->Fill(fNClusters[0],fMultFMDA+fMultFMDC);
      hmultV0vsmultFMD    ->Fill(fMultV0A+fMultV0C,fMultFMDA+fMultFMDC);
      
  }   
  else if(fAnalysisInput.CompareTo("AOD")==0){
    //AliAODEvent *aod =  dynamic_cast<AliAODEvent*> (InputEvent());
    // to be implemented
    printf("  AOD analysis not yet implemented!!!\n\n");
    return;
  }
  PostData(1, fOutput);
}



//________________________________________________________________________
void AliAnalysisTaskCentrality::Terminate(Option_t */*option*/)
{
  // Terminate analysis
}


