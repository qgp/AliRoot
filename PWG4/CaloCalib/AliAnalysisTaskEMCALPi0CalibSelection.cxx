/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: Boris Polishchuk                                               *
 * Adapted to AOD reading by Gustavo Conesa  *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

//---------------------------------------------------------------------------// 
//                                                                           //
// Fill histograms (one per cell) with two-cluster invariant mass            //
// using calibration coefficients of the previous iteration.                 //
// Histogram for a given cell is filled if the most energy of one cluster    //
// is deposited in this cell and the other cluster could be anywherein EMCAL.//
//                                                                           //
//---------------------------------------------------------------------------//

//#include <cstdlib>
//#include <Riostream.h>
// Root 
#include "TLorentzVector.h"
#include "TRefArray.h"
#include "TList.h"
#include "TH1F.h"
#include <TGeoManager.h>

// AliRoot
#include "AliAnalysisTaskEMCALPi0CalibSelection.h"
#include "AliAODEvent.h"
#include "AliESDEvent.h"
#include "AliEMCALGeometry.h"
#include "AliVCluster.h"
#include "AliVCaloCells.h"
#include "AliEMCALRecoUtils.h"
//#include "AliEMCALAodCluster.h"
//#include "AliEMCALCalibData.h"

ClassImp(AliAnalysisTaskEMCALPi0CalibSelection)


//__________________________________________________
AliAnalysisTaskEMCALPi0CalibSelection::AliAnalysisTaskEMCALPi0CalibSelection(const char* name) :
  AliAnalysisTaskSE(name),fEMCALGeo(0x0),//fCalibData(0x0), 
  fEmin(0.5), fEmax(15.), fAsyCut(1.),fMinNCells(2), fGroupNCells(0),
  fLogWeight(4.5), fSameSM(kFALSE), fOldAOD(kFALSE), fFilteredInput(kFALSE),
  fCorrectClusters(kFALSE), fEMCALGeoName("EMCAL_FIRSTYEAR"), 
  fRecoUtils(new AliEMCALRecoUtils),
  fNbins(300), fMinBin(0.), fMaxBin(300.),fOutputContainer(0x0),
  fHmgg(0x0),           fHmggDifferentSM(0x0), 
  fHOpeningAngle(0x0),  fHOpeningAngleDifferentSM(0x0),  
  fHIncidentAngle(0x0), fHIncidentAngleDifferentSM(0x0),
  fHAsymmetry(0x0),  fHAsymmetryDifferentSM(0x0),  
  fhNEvents(0x0),fCuts(0x0)
{
  //Named constructor which should be used.
  
  for(Int_t iMod=0; iMod < 12; iMod++) {
    for(Int_t iX=0; iX<24; iX++) {
      for(Int_t iZ=0; iZ<48; iZ++) {
        fHmpi0[iMod][iZ][iX]=0;
      }
    } 
  }
  
  for(Int_t iSM=0; iSM<4; iSM++) {
    fHmggSM[iSM]              =0;
    fHmggPairSM[iSM]          =0;
    fHOpeningAngleSM[iSM]     =0;
    fHOpeningAnglePairSM[iSM] =0;
    fHAsymmetrySM[iSM]     =0;
    fHAsymmetryPairSM[iSM] =0;
    fHIncidentAngleSM[iSM]    =0;
    fHIncidentAnglePairSM[iSM]=0;
    fhTowerDecayPhotonHit[iSM] =0;
    fhTowerDecayPhotonEnergy[iSM]=0;
    fhTowerDecayPhotonAsymmetry[iSM]=0;
  }
  
  DefineOutput(1, TList::Class());
  DefineOutput(2, TList::Class());  // will contain cuts or local params

}

//__________________________________________________
AliAnalysisTaskEMCALPi0CalibSelection::~AliAnalysisTaskEMCALPi0CalibSelection()
{
  //Destructor.
  
  if(fOutputContainer){
    fOutputContainer->Delete() ; 
    delete fOutputContainer ;
  }
	
  //if(fCalibData)  delete fCalibData;
  if(fEMCALGeo)   delete fEMCALGeo;
	
  if(fRecoUtils) delete fRecoUtils ;

}

//_____________________________________________________
void AliAnalysisTaskEMCALPi0CalibSelection::LocalInit()
{
	// Local Initialization
	
	// Create cuts/param objects and publish to slot
	const Int_t buffersize = 255;
	char onePar[buffersize] ;
	fCuts = new TList();

	snprintf(onePar,buffersize, "Custer cuts: %2.2f < E < %2.2f GeV; min number of cells %d; Assymetry cut %1.2f", fEmin,fEmax, fMinNCells, fAsyCut) ;
	fCuts->Add(new TObjString(onePar));
	snprintf(onePar,buffersize, "Group %d cells;", fGroupNCells) ;
	fCuts->Add(new TObjString(onePar));
  snprintf(onePar,buffersize, "Cluster maximal cell away from border at least %d cells;", fRecoUtils->GetNumberOfCellsFromEMCALBorder()) ;
	fCuts->Add(new TObjString(onePar));
	snprintf(onePar,buffersize, "Histograms: bins %d; energy range: %2.2f < E < %2.2f GeV;",fNbins,fMinBin,fMaxBin) ;
	fCuts->Add(new TObjString(onePar));
	snprintf(onePar,buffersize, "Switchs: Remove Bad Channels? %d; Use filtered input? %d;  Correct Clusters? %d, Analyze Old AODs? %d, Mass per channel same SM clusters? %d ",
            fRecoUtils->IsBadChannelsRemovalSwitchedOn(),fFilteredInput,fCorrectClusters, fOldAOD, fSameSM) ;
	fCuts->Add(new TObjString(onePar));
	snprintf(onePar,buffersize, "EMCAL Geometry name: < %s >",fEMCALGeoName.Data()) ;
	fCuts->Add(new TObjString(onePar));

	// Post Data
	PostData(2, fCuts);
	
}

//_________________________________________________________________
Int_t AliAnalysisTaskEMCALPi0CalibSelection::GetEMCALClusters(AliVEvent * event, TRefArray *clusters) const
{
  // fills the provided TRefArray with all found emcal clusters
  
  clusters->Clear();
  AliVCluster *cl = 0;
  Bool_t first = kTRUE;
  for (Int_t i = 0; i < event->GetNumberOfCaloClusters(); i++) {
    if ( (cl = event->GetCaloCluster(i)) ) {
      if (IsEMCALCluster(cl)){
        if(first) {
          new (clusters) TRefArray(TProcessID::GetProcessWithUID(cl)); 
          first=kFALSE;
        }
        clusters->Add(cl);
        //printf("IsEMCal cluster %d, E %2.3f Size: %d \n",i,cl->E(),clusters->GetEntriesFast());
      }
    }
  }
  return clusters->GetEntriesFast();
}


//____________________________________________________________________________
Bool_t AliAnalysisTaskEMCALPi0CalibSelection::IsEMCALCluster(AliVCluster* cluster) const {
  // Check if it is a cluster from EMCAL. For old AODs cluster type has
  // different number and need to patch here
  
  if(fOldAOD)
  {
    if (cluster->GetType() == 2) return kTRUE;
    else                         return kFALSE;
  }
  else 
  {
    return cluster->IsEMCAL();
  }
  
}


//__________________________________________________
void AliAnalysisTaskEMCALPi0CalibSelection::UserCreateOutputObjects()
{
  //Create output container, init geometry 
	
  fEMCALGeo =  AliEMCALGeometry::GetInstance(fEMCALGeoName) ;	
  
  fOutputContainer = new TList();
  const Int_t buffersize = 255;
  char hname[buffersize], htitl[buffersize];
  
  for(Int_t iMod=0; iMod < (fEMCALGeo->GetEMCGeometry())->GetNumberOfSuperModules(); iMod++) {
    for(Int_t iRow=0; iRow<AliEMCALGeoParams::fgkEMCALRows; iRow++) {
      for(Int_t iCol=0; iCol<AliEMCALGeoParams::fgkEMCALCols; iCol++) {
        snprintf(hname,buffersize, "%d_%d_%d",iMod,iCol,iRow);
        snprintf(htitl,buffersize, "Two-gamma inv. mass for super mod %d, cell(col,row)=(%d,%d)",iMod,iCol,iRow);
        fHmpi0[iMod][iCol][iRow] = new TH1F(hname,htitl,fNbins,fMinBin,fMaxBin);
        fOutputContainer->Add(fHmpi0[iMod][iCol][iRow]);
      }
    }
  }

  fHmgg = new TH2F("hmgg","2-cluster invariant mass",fNbins,fMinBin,fMaxBin,100,0,10);
  fHmgg->SetXTitle("m_{#gamma #gamma} (MeV/c^{2})");
  fHmgg->SetYTitle("p_{T #gamma #gamma} (GeV/c)");
  fOutputContainer->Add(fHmgg);

  fHmggDifferentSM = new TH2F("hmggDifferentSM","2-cluster invariant mass, different SM",fNbins,fMinBin,fMaxBin,100,0,10);
  fHmggDifferentSM->SetXTitle("m_{#gamma #gamma} (MeV/c^{2})");
  fHmggDifferentSM->SetYTitle("p_{T #gamma #gamma} (GeV/c)");
  fOutputContainer->Add(fHmggDifferentSM);

  fHOpeningAngle = new TH2F("hopang","2-cluster opening angle",100,0.,50.,100,0,10);
  fHOpeningAngle->SetXTitle("#alpha_{#gamma #gamma}");
  fHOpeningAngle->SetYTitle("p_{T #gamma #gamma} (GeV/c)");
  fOutputContainer->Add(fHOpeningAngle);
  
  fHOpeningAngleDifferentSM = new TH2F("hopangDifferentSM","2-cluster opening angle, different SM",100,0,50.,100,0,10);
  fHOpeningAngleDifferentSM->SetXTitle("#alpha_{#gamma #gamma}");
  fHOpeningAngleDifferentSM->SetYTitle("p_{T #gamma #gamma} (GeV/c)");
  fOutputContainer->Add(fHOpeningAngleDifferentSM);
  
  fHIncidentAngle = new TH2F("hinang","#gamma incident angle in SM",100,0.,20.,100,0,10);
  fHIncidentAngle->SetXTitle("#alpha_{#gamma SM center}");
  fHIncidentAngle->SetYTitle("p_{T #gamma} (GeV/c)");
  fOutputContainer->Add(fHIncidentAngle);
  
  fHIncidentAngleDifferentSM = new TH2F("hinangDifferentSM","#gamma incident angle in SM, different SM pair",100,0,20.,100,0,10);
  fHIncidentAngleDifferentSM->SetXTitle("#alpha_{#gamma - SM center}");
  fHIncidentAngleDifferentSM->SetYTitle("p_{T #gamma} (GeV/c)");
  fOutputContainer->Add(fHIncidentAngleDifferentSM);
  
  fHAsymmetry = new TH2F("hasym","2-cluster opening angle",100,0.,1.,100,0,10);
  fHAsymmetry->SetXTitle("a");
  fHAsymmetry->SetYTitle("p_{T #gamma #gamma} (GeV/c)");
  fOutputContainer->Add(fHAsymmetry);
  
  fHAsymmetryDifferentSM = new TH2F("hasymDifferentSM","2-cluster opening angle, different SM",100,0,1.,100,0,10);
  fHAsymmetryDifferentSM->SetXTitle("a");
  fHAsymmetryDifferentSM->SetYTitle("p_{T #gamma #gamma} (GeV/c)");
  fOutputContainer->Add(fHAsymmetryDifferentSM);
  
  
  TString pairname[] = {"A side (0-2)", "C side (1-3)","Row 0 (0-1)", "Row 1 (2-3)"};
  
  for(Int_t iSM=0; iSM<4; iSM++) {
    
    snprintf(hname, buffersize, "hmgg_SM%d",iSM);
    snprintf(htitl, buffersize, "Two-gamma inv. mass for super mod %d",iSM);
    fHmggSM[iSM] = new TH2F(hname,htitl,fNbins,fMinBin,fMaxBin,100,0,10);
    fHmggSM[iSM]->SetXTitle("m_{#gamma #gamma} (MeV/c^{2})");
    fHmggSM[iSM]->SetYTitle("p_{T #gamma #gamma} (GeV/c)");
    fOutputContainer->Add(fHmggSM[iSM]);
    
    snprintf(hname,buffersize, "hmgg_PairSM%d",iSM);
    snprintf(htitl,buffersize, "Two-gamma inv. mass for SM pair: %s",pairname[iSM].Data());
    fHmggPairSM[iSM] = new TH2F(hname,htitl,fNbins,fMinBin,fMaxBin,100,0,10);
    fHmggPairSM[iSM]->SetXTitle("m_{#gamma #gamma} (MeV/c^{2})");
    fHmggPairSM[iSM]->SetYTitle("p_{T #gamma #gamma} (GeV/c)");
    fOutputContainer->Add(fHmggPairSM[iSM]);
    
    
    snprintf(hname, buffersize, "hopang_SM%d",iSM);
    snprintf(htitl, buffersize, "Opening angle for super mod %d",iSM);
    fHOpeningAngleSM[iSM] = new TH2F(hname,htitl,100,0.,50.,100,0,10);
    fHOpeningAngleSM[iSM]->SetXTitle("#alpha_{#gamma #gamma} (deg)");
    fHOpeningAngleSM[iSM]->SetYTitle("p_{T #gamma #gamma} (GeV/c)");
    fOutputContainer->Add(fHOpeningAngleSM[iSM]);
    
    snprintf(hname,buffersize, "hopang_PairSM%d",iSM);
    snprintf(htitl,buffersize, "Opening angle for SM pair: %s",pairname[iSM].Data());
    fHOpeningAnglePairSM[iSM] = new TH2F(hname,htitl,100,0.,50.,100,0,10);
    fHOpeningAnglePairSM[iSM]->SetXTitle("#alpha_{#gamma #gamma} (deg)");
    fHOpeningAnglePairSM[iSM]->SetYTitle("p_{T #gamma #gamma} (GeV/c)");
    fOutputContainer->Add(fHOpeningAnglePairSM[iSM]);    
    
    snprintf(hname, buffersize, "hinang_SM%d",iSM);
    snprintf(htitl, buffersize, "Incident angle for super mod %d",iSM);
    fHIncidentAngleSM[iSM] = new TH2F(hname,htitl,100,0.,20.,100,0,10);
    fHIncidentAngleSM[iSM]->SetXTitle("#alpha_{#gamma - SM center} (deg)");
    fHIncidentAngleSM[iSM]->SetYTitle("p_{T #gamma} (GeV/c)");
    fOutputContainer->Add(fHIncidentAngleSM[iSM]);
    
    snprintf(hname,buffersize, "hinang_PairSM%d",iSM);
    snprintf(htitl,buffersize, "Incident angle for SM pair: %s",pairname[iSM].Data());
    fHIncidentAnglePairSM[iSM] = new TH2F(hname,htitl,100,0.,20.,100,0,10);
    fHIncidentAnglePairSM[iSM]->SetXTitle("#alpha_{#gamma - SM center} (deg)");
    fHIncidentAnglePairSM[iSM]->SetYTitle("p_{T #gamma} (GeV/c)");
    fOutputContainer->Add(fHIncidentAnglePairSM[iSM]);   
    
    snprintf(hname, buffersize, "hasym_SM%d",iSM);
    snprintf(htitl, buffersize, "asymmetry for super mod %d",iSM);
    fHAsymmetrySM[iSM] = new TH2F(hname,htitl,100,0.,1.,100,0,10);
    fHAsymmetrySM[iSM]->SetXTitle("a");
    fHAsymmetrySM[iSM]->SetYTitle("p_{T #gamma #gamma} (GeV/c)");
    fOutputContainer->Add(fHAsymmetrySM[iSM]);
    
    snprintf(hname,buffersize, "hasym_PairSM%d",iSM);
    snprintf(htitl,buffersize, "Asymmetry for SM pair: %s",pairname[iSM].Data());
    fHAsymmetryPairSM[iSM] = new TH2F(hname,htitl,100,0.,1.,100,0,10);
    fHAsymmetryPairSM[iSM]->SetXTitle("a");
    fHAsymmetryPairSM[iSM]->SetYTitle("p_{T #gamma #gamma} (GeV/c)");
    fOutputContainer->Add(fHAsymmetryPairSM[iSM]);    
    
    
    Int_t colmax = 48;
    Int_t rowmax = 24;
    
    fhTowerDecayPhotonHit[iSM]  = new TH2F (Form("hTowerDecPhotonHit_Mod%d",iSM),Form("Entries in grid of cells in Module %d",iSM), 
                                      colmax+2,-1.5,colmax+0.5, rowmax+2,-1.5,rowmax+0.5); 
    fhTowerDecayPhotonHit[iSM]->SetYTitle("row (phi direction)");
    fhTowerDecayPhotonHit[iSM]->SetXTitle("column (eta direction)");
    fOutputContainer->Add(fhTowerDecayPhotonHit[iSM]);
    
    fhTowerDecayPhotonEnergy[iSM]  = new TH2F (Form("hTowerDecPhotonEnergy_Mod%d",iSM),Form("Accumulated energy in grid of cells in Module %d",iSM), 
                                       colmax+2,-1.5,colmax+0.5, rowmax+2,-1.5,rowmax+0.5); 
    fhTowerDecayPhotonEnergy[iSM]->SetYTitle("row (phi direction)");
    fhTowerDecayPhotonEnergy[iSM]->SetXTitle("column (eta direction)");
    fOutputContainer->Add(fhTowerDecayPhotonEnergy[iSM]);
    
    fhTowerDecayPhotonAsymmetry[iSM]  = new TH2F (Form("hTowerDecPhotonAsymmetry_Mod%d",iSM),Form("Accumulated asymmetry in grid of cells in Module %d",iSM), 
                                               colmax+2,-1.5,colmax+0.5, rowmax+2,-1.5,rowmax+0.5); 
    fhTowerDecayPhotonAsymmetry[iSM]->SetYTitle("row (phi direction)");
    fhTowerDecayPhotonAsymmetry[iSM]->SetXTitle("column (eta direction)");
    fOutputContainer->Add(fhTowerDecayPhotonAsymmetry[iSM]);
    
  }  
  
  fhNEvents        = new TH1I("hNEvents", "Number of analyzed events"   , 1 , 0 , 1  ) ;
  fOutputContainer->Add(fhNEvents);
  
  fOutputContainer->SetOwner(kTRUE);
  
//  fCalibData = new AliEMCALCalibData();
		
  PostData(1,fOutputContainer);

}

//__________________________________________________
void AliAnalysisTaskEMCALPi0CalibSelection::UserExec(Option_t* /* option */)
{
  //Analysis per event.
  
  if(fRecoUtils->GetParticleType()!=AliEMCALRecoUtils::kPhoton){
    printf("Wrong particle type for cluster position recalculation! = %d\n", fRecoUtils->GetParticleType());
    abort();
  }
  
  fhNEvents->Fill(0); //Event analyzed
  
  //Get the input event
  AliVEvent* event = 0;
  if(fFilteredInput) event = AODEvent();
  else               event = InputEvent();
  
  if(!event) {
    printf("Input event not available!\n");
    return;
  }

  if(DebugLevel() > 1) 
    printf("AliAnalysisTaskEMCALPi0CalibSelection <<< %s: Event %d >>>\n",event->GetName(), (Int_t)Entry());
  
  
  //Get the primary vertex
  Double_t v[3];
  event->GetPrimaryVertex()->GetXYZ(v) ;
  
  if(DebugLevel() > 1) printf("AliAnalysisTaskEMCALPi0CalibSelection Vertex: (%.3f,%.3f,%.3f)\n",v[0],v[1],v[2]);
  
  //Int_t runNum = aod->GetRunNumber();
  //if(DebugLevel() > 1) printf("Run number: %d\n",runNum);
  
  //FIXME Not need the matrices for the moment MEFIX
  //Get the matrix with geometry information
  //Still not implemented in AOD, just a workaround to be able to work at least with ESDs	
  //  if(!strcmp(InputEvent()->GetName(),"AliAODEvent")) {
  //    if(DebugLevel() > 1) 
  //      printf("AliAnalysisTaskEMCALPi0CalibSelection Use ideal geometry, values geometry matrix not kept in AODs.\n");
  //  }
  //  else{	
  //    if(DebugLevel() > 1) printf("AliAnalysisTaskEMCALPi0CalibSelection Load Misaligned matrices. \n");
  //    AliESDEvent* esd = dynamic_cast<AliESDEvent*>(InputEvent()) ;
  //    if(!esd) {
  //      printf("AliAnalysisTaskEMCALPi0CalibSelection::UserExec() - This event does not contain ESDs?");
  //      return;
  //    }
  //    for(Int_t mod=0; mod < (fEMCALGeo->GetEMCGeometry())->GetNumberOfSuperModules(); mod++){ 
  //      if(esd->GetEMCALMatrix(mod)) fEMCALGeo->SetMisalMatrix(esd->GetEMCALMatrix(mod),mod) ;
  //    }
  //  }
  
  if(DebugLevel() > 1) printf("AliAnalysisTaskEMCALPi0CalibSelection Will use fLogWeight %.3f .\n",fLogWeight);
  Int_t absId1   = -1;
  Int_t iSupMod1 = -1;
  Int_t iphi1    = -1;
  Int_t ieta1    = -1;
  Int_t absId2   = -1;
  Int_t iSupMod2 = -1;
  Int_t iphi2    = -1;
  Int_t ieta2    = -1;
	
  TLorentzVector p1;
  TLorentzVector p2;
  TLorentzVector p12;
  
  //Get the list of clusters
  TRefArray * caloClustersArr  = new TRefArray();
  if(!fOldAOD) event->GetEMCALClusters(caloClustersArr);
  else                GetEMCALClusters(event,caloClustersArr);
  const Int_t kNumberOfEMCALClusters   = caloClustersArr->GetEntries() ;
  if(DebugLevel() > 1) printf("AliAnalysisTaskEMCALPi0CalibSelection - N CaloClusters: %d \n", kNumberOfEMCALClusters);
  
  // Get EMCAL cells
  AliVCaloCells *emCells = event->GetEMCALCells();
  
  // loop over EMCAL clusters
  //----------------------------------------------------------
  // First recalibrate and recalculate energy and position
  Float_t pos[]={0,0,0};
  if(fCorrectClusters){
    for(Int_t iClu=0; iClu<kNumberOfEMCALClusters-1; iClu++) {
      AliVCluster *c1 = (AliVCluster *) caloClustersArr->At(iClu);
      
      if(fRecoUtils->ClusterContainsBadChannel(fEMCALGeo, c1->GetCellsAbsId(), c1->GetNCells())) continue;	
      
      if(DebugLevel() > 2)
      { 
        printf("Std  : i %d, E %f, dispersion %f, m02 %f, m20 %f\n",c1->GetID(),c1->E(),c1->GetDispersion(),c1->GetM02(),c1->GetM20());
        c1->GetPosition(pos);
        printf("Std  : i %d, x %f, y %f, z %f\n",c1->GetID(), pos[0], pos[1], pos[2]);
      }
      
      //Correct cluster energy and position if requested, and not corrected previously, by default Off
      if(fRecoUtils->IsRecalibrationOn())	{
        fRecoUtils->RecalibrateClusterEnergy(fEMCALGeo, c1, emCells);
        fRecoUtils->RecalculateClusterShowerShapeParameters(fEMCALGeo, emCells,c1);
        fRecoUtils->RecalculateClusterPID(c1);
      }
      if(DebugLevel() > 2) 
        printf("Energy: after recalibration %f; ",c1->E());
      
      // Correct Non-Linearity
      c1->SetE(fRecoUtils->CorrectClusterEnergyLinearity(c1));

      if(DebugLevel() > 2) 
        printf("after linearity correction %f\n",c1->E());
      // Recalculate cluster position
      fRecoUtils->RecalculateClusterPosition(fEMCALGeo, emCells,c1);
      if(DebugLevel() > 2)
      { 
        printf("Cor  : i %d, E %f, dispersion %f, m02 %f, m20 %f\n",c1->GetID(),c1->E(),c1->GetDispersion(),c1->GetM02(),c1->GetM20());
        c1->GetPosition(pos);
        printf("Cor  : i %d, x %f, y %f, z %f\n",c1->GetID(), pos[0], pos[1], pos[2]);
      }    
    }    
  }
  
  //----------------------------------------------------------
  //Now the invariant mass analysis with the corrected clusters  
  for(Int_t iClu=0; iClu<kNumberOfEMCALClusters-1; iClu++) {
    
    AliVCluster *c1 = (AliVCluster *) caloClustersArr->At(iClu);
    if(fRecoUtils->ClusterContainsBadChannel(fEMCALGeo, c1->GetCellsAbsId(), c1->GetNCells())) continue;	
    
    Float_t e1i = c1->E();   // cluster energy before correction   
    if(e1i < fEmin) continue;
    else if(e1i > fEmax) continue;
    else if (c1->GetNCells() < fMinNCells) continue; 
    
    if(DebugLevel() > 2)
    { 
      printf("IMA  : i %d, E %f, dispersion %f, m02 %f, m20 %f\n",c1->GetID(),e1i,c1->GetDispersion(),c1->GetM02(),c1->GetM20());
      c1->GetPosition(pos);
      printf("IMA  : i %d, x %f, y %f, z %f\n",c1->GetID(), pos[0], pos[1], pos[2]);
    }
    
    //AliEMCALAodCluster newc1(*((AliAODCaloCluster*)c1));
    //newc1.EvalAllFromRecoUtils(fEMCALGeo,fRecoUtils,emCells);
    //printf("i %d, recal? %d\n",iClu,newc1.IsRecalibrated());
    //clu1.Recalibrate(fCalibData, emCells, fEMCALGeoName);
    //clu1.EvalEnergy();
    //clu1.EvalAll(fLogWeight, fEMCALGeoName);
    
    fRecoUtils->GetMaxEnergyCell(fEMCALGeo, emCells,c1,absId1,iSupMod1,ieta1,iphi1);
    c1->GetMomentum(p1,v);
    //newc1.GetMomentum(p1,v);
    
    // Combine cluster with other clusters and get the invariant mass
    for (Int_t jClu=iClu+1; jClu<kNumberOfEMCALClusters; jClu++) {
      AliAODCaloCluster *c2 = (AliAODCaloCluster *) caloClustersArr->At(jClu);
      //if(c2->IsEqual(c1)) continue;
      if(fRecoUtils->ClusterContainsBadChannel(fEMCALGeo, c2->GetCellsAbsId(), c2->GetNCells())) continue;	
      
      Float_t e2i = c2->E();
      if(e2i < fEmin) continue;
      else if (e2i > fEmax) continue;
      else if (c2->GetNCells() < fMinNCells) continue; 
      
      //AliEMCALAodCluster newc2(*((AliAODCaloCluster*)c2));
      //newc2.EvalAllFromRecoUtils(fEMCALGeo,fRecoUtils,emCells);
      //printf("\t j %d, recal? %d\n",jClu,newc2.IsRecalibrated());
      //clu2.Recalibrate(fCalibData, emCells,fEMCALGeoName);
      //clu2.EvalEnergy();
      //clu2.EvalAll(fLogWeight,fEMCALGeoName);
      
      fRecoUtils->GetMaxEnergyCell(fEMCALGeo, emCells,c2,absId2,iSupMod2,ieta2,iphi2);
      c2->GetMomentum(p2,v);
      //newc2.GetMomentum(p2,v);
      p12 = p1+p2;
      Float_t invmass = p12.M()*1000; 
      //printf("*** mass %f\n",invmass);
      Float_t asym = TMath::Abs(p1.E()-p2.E())/(p1.E()+p2.E());
      //printf("asymmetry %f\n",asym);
      
      if(asym > fAsyCut) continue;
      
      if(invmass < fMaxBin && invmass > fMinBin){
        
        //Check if cluster is in fidutial region, not too close to borders
        Bool_t in1 = fRecoUtils->CheckCellFiducialRegion(fEMCALGeo, c1, emCells);
        Bool_t in2 = fRecoUtils->CheckCellFiducialRegion(fEMCALGeo, c2, emCells);
        
        if(in1 && in2){
          
          fHmgg->Fill(invmass,p12.Pt()); 
          
          if(iSupMod1==iSupMod2) fHmggSM[iSupMod1]->Fill(invmass,p12.Pt()); 
          else                   fHmggDifferentSM ->Fill(invmass,p12.Pt());
          
          if((iSupMod1==0 && iSupMod2==2) || (iSupMod1==2 && iSupMod2==0)) fHmggPairSM[0]->Fill(invmass,p12.Pt()); 
          if((iSupMod1==1 && iSupMod2==3) || (iSupMod1==3 && iSupMod2==1)) fHmggPairSM[1]->Fill(invmass,p12.Pt()); 
          if((iSupMod1==0 && iSupMod2==1) || (iSupMod1==1 && iSupMod2==0)) fHmggPairSM[2]->Fill(invmass,p12.Pt()); 
          if((iSupMod1==2 && iSupMod2==3) || (iSupMod1==3 && iSupMod2==2)) fHmggPairSM[3]->Fill(invmass,p12.Pt()); 
          
          if(invmass > 100. && invmass < 160.){//restrict to clusters really close to pi0 peak
            
            //Opening angle of 2 photons
            Float_t opangle = p1.Angle(p2.Vect())*TMath::RadToDeg();
            //printf("*******>>>>>>>> In PEAK pt %f, angle %f \n",p12.Pt(),opangle);
            
            //Inciden angle of each photon
            Float_t posSM1cen[3]={0.,0.,0.};
            Float_t depth = fRecoUtils->GetDepth(p1.Energy(),fRecoUtils->GetParticleType(),iSupMod1);
            fEMCALGeo->RecalculateTowerPosition(11.5, 23.5, iSupMod1, depth, fRecoUtils->GetMisalTransShiftArray(),fRecoUtils->GetMisalRotShiftArray(),posSM1cen); 
            Float_t posSM2cen[3]={0.,0.,0.}; 
            depth = fRecoUtils->GetDepth(p2.Energy(),fRecoUtils->GetParticleType(),iSupMod2);
            fEMCALGeo->RecalculateTowerPosition(11.5, 23.5, iSupMod2, depth, fRecoUtils->GetMisalTransShiftArray(),fRecoUtils->GetMisalRotShiftArray(),posSM2cen); 
            //printf("SM1 %d pos (%2.3f,%2.3f,%2.3f) \n",iSupMod1,posSM1cen[0],posSM1cen[1],posSM1cen[2]);
            //printf("SM2 %d pos (%2.3f,%2.3f,%2.3f) \n",iSupMod2,posSM2cen[0],posSM2cen[1],posSM2cen[2]);
            
            TVector3 vecSM1cen(posSM1cen[0]-v[0],posSM1cen[1]-v[1],posSM1cen[2]-v[2]); 
            TVector3 vecSM2cen(posSM2cen[0]-v[0],posSM2cen[1]-v[1],posSM2cen[2]-v[2]); 
            Float_t inangle1 = p1.Angle(vecSM1cen)*TMath::RadToDeg();
            Float_t inangle2 = p2.Angle(vecSM2cen)*TMath::RadToDeg();
            //printf("Incident angle: cluster 1 %2.3f; cluster 2 %2.3f\n",inangle1,inangle2);
            
            fHOpeningAngle ->Fill(opangle,p12.Pt()); 
            fHIncidentAngle->Fill(inangle1,p1.Pt()); 
            fHIncidentAngle->Fill(inangle2,p2.Pt()); 
            fHAsymmetry    ->Fill(asym,p12.Pt()); 
            
            if(iSupMod1==iSupMod2) {
              fHOpeningAngleSM[iSupMod1] ->Fill(opangle,p12.Pt());
              fHIncidentAngleSM[iSupMod1]->Fill(inangle1,p1.Pt());
              fHIncidentAngleSM[iSupMod1]->Fill(inangle2,p2.Pt());
              fHAsymmetrySM[iSupMod1]    ->Fill(asym,p12.Pt());
            }
            else{      
              fHOpeningAngleDifferentSM  ->Fill(opangle,p12.Pt());
              fHIncidentAngleDifferentSM ->Fill(inangle1,p1.Pt());
              fHIncidentAngleDifferentSM ->Fill(inangle2,p2.Pt());
              fHAsymmetryDifferentSM     ->Fill(asym,p12.Pt());
            }
            
            if((iSupMod1==0 && iSupMod2==2) || (iSupMod1==2 && iSupMod2==0)) {
              fHOpeningAnglePairSM[0] ->Fill(opangle,p12.Pt()); 
              fHIncidentAnglePairSM[0]->Fill(inangle1,p1.Pt());
              fHIncidentAnglePairSM[0]->Fill(inangle2,p2.Pt());
              fHAsymmetryPairSM[0]    ->Fill(asym,p12.Pt());
              
            } 
            if((iSupMod1==1 && iSupMod2==3) || (iSupMod1==3 && iSupMod2==1)) {
              fHOpeningAnglePairSM[1] ->Fill(opangle,p12.Pt()); 
              fHIncidentAnglePairSM[1]->Fill(inangle1,p1.Pt());
              fHIncidentAnglePairSM[1]->Fill(inangle2,p2.Pt());
              fHAsymmetryPairSM[1]    ->Fill(asym,p12.Pt());
              
            }
            
            if((iSupMod1==0 && iSupMod2==1) || (iSupMod1==1 && iSupMod2==0)) {
              fHOpeningAnglePairSM[2] ->Fill(opangle,p12.Pt()); 
              fHIncidentAnglePairSM[2]->Fill(inangle1,p1.Pt());
              fHIncidentAnglePairSM[2]->Fill(inangle2,p2.Pt());
              fHAsymmetryPairSM[2]    ->Fill(asym,p12.Pt());
              
              
            }
            if((iSupMod1==2 && iSupMod2==3) || (iSupMod1==3 && iSupMod2==2)) {
              fHOpeningAnglePairSM[3] ->Fill(opangle,p12.Pt()); 
              fHIncidentAnglePairSM[3]->Fill(inangle1,p1.Pt());
              fHIncidentAnglePairSM[3]->Fill(inangle2,p2.Pt());
              fHAsymmetryPairSM[3]    ->Fill(asym,p12.Pt());
            }
            
          }// pair in 100 < mass < 160
          
        }//in acceptance cuts
        
        //In case of filling only channels with second cluster in same SM
        if(fSameSM && iSupMod1!=iSupMod2) continue;
        
        if (fGroupNCells == 0){
          fHmpi0[iSupMod1][ieta1][iphi1]->Fill(invmass);
          fHmpi0[iSupMod2][ieta2][iphi2]->Fill(invmass);
          
          if(invmass > 100. && invmass < 160.){//restrict to clusters really close to pi0 peak
            fhTowerDecayPhotonHit      [iSupMod1]->Fill(ieta1,iphi1);
            fhTowerDecayPhotonEnergy   [iSupMod1]->Fill(ieta1,iphi1,p1.E());
            fhTowerDecayPhotonAsymmetry[iSupMod1]->Fill(ieta1,iphi1,asym);
            
            fhTowerDecayPhotonHit      [iSupMod2]->Fill(ieta2,iphi2);
            fhTowerDecayPhotonEnergy   [iSupMod2]->Fill(ieta2,iphi2,p2.E());
            fhTowerDecayPhotonAsymmetry[iSupMod2]->Fill(ieta2,iphi2,asym);
            
          }// pair in mass of pi0
        }	
        else  {
          //printf("Regroup N %d, eta1 %d, phi1 %d, eta2 %d, phi2 %d \n",fGroupNCells, ieta1, iphi1, ieta2, iphi2);
          for (Int_t i = -fGroupNCells; i < fGroupNCells+1; i++) {
            for (Int_t j = -fGroupNCells; j < fGroupNCells+1; j++) {
              //printf("\t i %d, j %d\n",i,j);
              if((ieta1+i >= 0) && (iphi1+j >= 0) && (ieta1+i < 48) && (iphi1+j < 24)){
                //printf("\t \t eta1+i %d, phi1+j %d\n", ieta1+i, iphi1+j);
                fHmpi0[iSupMod1][ieta1+i][iphi1+j]->Fill(invmass);
              }
              if((ieta2+i >= 0) && (iphi2+j >= 0) && (ieta2+i < 48) && (iphi2+j < 24)){
                //printf("\t \t eta2+i %d, phi2+j %d\n", ieta2+i, iphi2+j);
                fHmpi0[iSupMod2][ieta2+i][iphi2+j]->Fill(invmass);
              }
            }// j loop
          }//i loop
        }//group cells
        
        if(DebugLevel() > 1) printf("AliAnalysisTaskEMCALPi0CalibSelection Mass in (SM%d,%d,%d) and  (SM%d,%d,%d): %.3f GeV  E1_i=%f E1_ii=%f  E2_i=%f E2_ii=%f\n",
                                    iSupMod1,iphi1,ieta1,iSupMod2,iphi2,ieta2,p12.M(),e1i,c1->E(),e2i,c2->E());
      }
      
    }
    
  } // end of loop over EMCAL clusters
  
  delete caloClustersArr;
  
  PostData(1,fOutputContainer);
  
}

//_____________________________________________________
void AliAnalysisTaskEMCALPi0CalibSelection::PrintInfo(){
  
  //Print settings
  printf("Cluster cuts: %2.2f < E < %2.2f GeV; min number of cells %d; Assymetry cut %1.2f\n", fEmin,fEmax, fMinNCells, fAsyCut) ;
	printf("Group %d cells\n", fGroupNCells) ;
  printf("Cluster maximal cell away from border at least %d cells\n", fRecoUtils->GetNumberOfCellsFromEMCALBorder()) ;
	printf("Histograms: bins %d; energy range: %2.2f < E < %2.2f GeV\n",fNbins,fMinBin,fMaxBin) ;
	printf("Switchs:\n \t Remove Bad Channels? %d; Use filtered input? %d;  Correct Clusters? %d, \n \t Analyze Old AODs? %d, Mass per channel same SM clusters? %d\n",
           fRecoUtils->IsBadChannelsRemovalSwitchedOn(),fFilteredInput,fCorrectClusters, fOldAOD, fSameSM) ;
	printf("EMCAL Geometry name: < %s >\n",fEMCALGeoName.Data()) ;

  
}

//__________________________________________________
//void AliAnalysisTaskEMCALPi0CalibSelection::SetCalibCorrections(AliEMCALCalibData* const cdata)
//{
//  //Set new correction factors (~1) to calibration coefficients, delete previous.
//
//   if(fCalibData) delete fCalibData;
//   fCalibData = cdata;
//	
//}
