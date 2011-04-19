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

//_________________________________________________________________________
//--
//-- Yves Schutz (SUBATECH) 
// Reconstruction class. Redesigned from the old AliReconstructionner class and 
// derived from STEER/AliReconstructor. 
// 


// --- ROOT system ---
#include <TClonesArray.h>
#include "TGeoManager.h"
#include "TGeoMatrix.h"

// --- Standard library ---

// --- AliRoot header files ---
#include "AliEMCALReconstructor.h"

#include "AliCodeTimer.h"
#include "AliCaloCalibPedestal.h"
#include "AliEMCALCalibData.h"
#include "AliESDEvent.h"
#include "AliESDCaloCluster.h"
#include "AliESDCaloCells.h"
#include "AliESDtrack.h"
#include "AliEMCALLoader.h"
#include "AliEMCALRawUtils.h"
#include "AliEMCALDigit.h"
#include "AliEMCALClusterizerv1.h"
#include "AliEMCALClusterizerNxN.h"
#include "AliEMCALRecPoint.h"
#include "AliEMCALPID.h"
#include "AliEMCALTrigger.h"
#include "AliRawReader.h"
#include "AliCDBEntry.h"
#include "AliCDBManager.h"
#include "AliEMCALGeometry.h"
#include "AliEMCAL.h"
#include "AliESDVZERO.h"
#include "AliCDBManager.h"
#include "AliRunLoader.h"
#include "AliRun.h"
#include "AliEMCALTriggerData.h"
#include "AliEMCALTriggerElectronics.h"
#include "AliEMCALTriggerDCSConfigDB.h"
#include "AliEMCALTriggerDCSConfig.h"
#include "AliEMCALTriggerData.h"
#include "AliEMCALTriggerRawDigit.h"
#include "AliEMCALTriggerPatch.h"
#include "AliEMCALTriggerTypes.h"

ClassImp(AliEMCALReconstructor) 
  
 const AliEMCALRecParam*     AliEMCALReconstructor::fgkRecParam        = 0;   // EMCAL rec. parameters
AliEMCALRawUtils*           AliEMCALReconstructor::fgRawUtils         = 0;   // EMCAL raw utilities class
AliEMCALClusterizer*        AliEMCALReconstructor::fgClusterizer      = 0;   // EMCAL clusterizer class
TClonesArray*               AliEMCALReconstructor::fgDigitsArr        = 0;   // list of digits, to be used multiple times
TObjArray*                  AliEMCALReconstructor::fgClustersArr      = 0;   // list of clusters, to be used multiple times
TClonesArray*               AliEMCALReconstructor::fgTriggerDigits    = 0;   // list of trigger digits, to be used multiple times
AliEMCALTriggerElectronics* AliEMCALReconstructor::fgTriggerProcessor = 0x0;
//____________________________________________________________________________
AliEMCALReconstructor::AliEMCALReconstructor() 
  : fGeom(0),fCalibData(0),fPedestalData(0),fTriggerData(0x0) 
{
  // ctor
  
  fgRawUtils = new AliEMCALRawUtils;
  
  //To make sure we match with the geometry in a simulation file,
  //let's try to get it first.  If not, take the default geometry
  AliRunLoader *rl = AliRunLoader::Instance();
  if (rl->GetAliRun()){
    AliEMCAL * emcal = dynamic_cast<AliEMCAL*>(rl->GetAliRun()->GetDetector("EMCAL"));
    if(emcal) fGeom = emcal->GetGeometry();
  }
  
  if(!fGeom) {
    AliInfo(Form("Using default geometry in reconstruction"));
    fGeom =  AliEMCALGeometry::GetInstance(AliEMCALGeometry::GetDefaultGeometryName());
  }
  
  //Get calibration parameters	
  if(!fCalibData)
    {
      AliCDBEntry *entry = (AliCDBEntry*) 
	AliCDBManager::Instance()->Get("EMCAL/Calib/Data");
      if (entry) fCalibData =  (AliEMCALCalibData*) entry->GetObject();
    }
  
  if(!fCalibData)
    AliFatal("Calibration parameters not found in CDB!");
  
  //Get calibration parameters	
  if(!fPedestalData)
    {
      AliCDBEntry *entry = (AliCDBEntry*) 
	AliCDBManager::Instance()->Get("EMCAL/Calib/Pedestals");
      if (entry) fPedestalData =  (AliCaloCalibPedestal*) entry->GetObject();
    }
  
  if(!fPedestalData)
    AliFatal("Dead map not found in CDB!");
  
  if(!fGeom) AliFatal(Form("Could not get geometry!"));
  
  AliEMCALTriggerDCSConfigDB* dcsConfigDB = AliEMCALTriggerDCSConfigDB::Instance();
  
  const AliEMCALTriggerDCSConfig* dcsConfig = dcsConfigDB->GetTriggerDCSConfig();
  
  if (!dcsConfig) AliFatal("No Trigger DCS Configuration from OCDB!");
  fgTriggerProcessor = new AliEMCALTriggerElectronics( dcsConfig );
  
  fTriggerData = new AliEMCALTriggerData();
  
  //Init temporary list of digits
  fgDigitsArr     = new TClonesArray("AliEMCALDigit",1000);
  fgClustersArr   = new TObjArray(1000);
  fgTriggerDigits = new TClonesArray("AliEMCALTriggerRawDigit",1000);	
} 

//____________________________________________________________________________
AliEMCALReconstructor::~AliEMCALReconstructor()
{
  // dtor
  
  if(fGeom)              delete fGeom;
  
  //No need to delete, recovered from OCDB
  //if(fCalibData)         delete fCalibData;
  //if(fPedestalData)      delete fPedestalData;
  
  if(fgDigitsArr){
    fgDigitsArr->Clear("C");
    delete fgDigitsArr; 
  }
  
  if(fgClustersArr){
    fgClustersArr->Clear();
    delete fgClustersArr; 
  }
  
  if(fgTriggerDigits){
    fgTriggerDigits->Clear();
    delete fgTriggerDigits; 
  }
  
  if(fgRawUtils)         delete fgRawUtils;
  if(fgClusterizer)      delete fgClusterizer;
  if(fgTriggerProcessor) delete fgTriggerProcessor;
  
  AliCodeTimer::Instance()->Print();
} 

//____________________________________________________________________________                                  
void AliEMCALReconstructor::InitClusterizer() const
{
  //Init the clusterizer with geometry and calibration pointers, avoid doing it twice.                          
  Int_t clusterizerType = -1;
  Int_t eventType = -1;
  if(GetRecParam()) {
    clusterizerType = GetRecParam()->GetClusterizerFlag();
    eventType       = GetRecParam()->GetEventSpecie();
  }
  else{
    AliCDBEntry *entry = (AliCDBEntry*)
      AliCDBManager::Instance()->Get("EMCAL/Calib/RecoParam");
    //Get The reco param for the default event specie                                                           
    if (entry) {
      AliEMCALRecParam *recParam  = (AliEMCALRecParam*)((TObjArray *) entry->GetObject())->At(0);
      if(recParam) clusterizerType = recParam->GetClusterizerFlag(); 
    }
  }
  
  //Check if clusterizer previously set corresponds to what is needed for this event type                       
  if(fgClusterizer){
    if(eventType!=AliRecoParam::kCalib){
      //printf("ReCreate clusterizer? Clusterizer set <%d>, Clusterizer in use <%s>\n",
      //     clusterizerType, fgClusterizer->Version());
      
      if     (clusterizerType == AliEMCALRecParam::kClusterizerv1 && !strcmp(fgClusterizer->Version(),"clu-v1")) return;
      
      else if(clusterizerType == AliEMCALRecParam::kClusterizerNxN && !strcmp(fgClusterizer->Version(),"clu-NxN")) return;
      
      //Need to create new clusterizer, the one set previously is not the correct one     
      delete fgClusterizer;
    }
    else return;
  }
  
  if (clusterizerType  == AliEMCALRecParam::kClusterizerv1)
    {
      fgClusterizer = new AliEMCALClusterizerv1(fGeom, fCalibData,fPedestalData);
    }
  else
    {
      fgClusterizer = new AliEMCALClusterizerNxN(fGeom, fCalibData,fPedestalData);
    }
}

//____________________________________________________________________________
void AliEMCALReconstructor::Reconstruct(TTree* digitsTree, TTree* clustersTree) const
{
  // method called by AliReconstruction; 
  // Only the clusterization is performed,; the rest of the reconstruction is done in FillESD because the track
  // segment maker needs access to the AliESD object to retrieve the tracks reconstructed by 
  // the global tracking.
  // Works on the current event.
  
  AliCodeTimerAuto("",0)
    
  //Get input digits and put them in fgDigitsArr, clear the list before 
  ReadDigitsArrayFromTree(digitsTree);
  
  InitClusterizer();
  
  fgClusterizer->InitParameters();
  fgClusterizer->SetOutput(clustersTree);
  
  //Skip clusterization of LED events
  if (GetRecParam()->GetEventSpecie()!=AliRecoParam::kCalib){
    
    if(fgDigitsArr && fgDigitsArr->GetEntries()) {
      
      fgClusterizer->SetInput(digitsTree);
      
      //fgClusterizer->Digits2Clusters("deb all") ; //For debugging
      fgClusterizer->Digits2Clusters("");
      
      fgClusterizer->Clear();
      
    }//digits array exists and has somethind
  }//not a LED event
  
  clustersTree->Fill();	
}

//____________________________________________________________________________
void AliEMCALReconstructor::ConvertDigits(AliRawReader* rawReader, TTree* digitsTree) const
  
{
  // Conversion from raw data to
  // EMCAL digits.
  // Works on a single-event basis
  
  rawReader->Reset() ; 
  
  fTriggerData->SetMode(1);	
  
  if(fgDigitsArr) fgDigitsArr->Clear("C");
  
  TClonesArray *digitsTrg = new TClonesArray("AliEMCALTriggerRawDigit", 32 * 96);
  
  Int_t bufsize = 32000;
  digitsTree->Branch("EMCAL", &fgDigitsArr, bufsize);
  digitsTree->Branch("EMTRG", &digitsTrg, bufsize);
  
  //Skip calibration events do the rest
  Bool_t doFit = kTRUE;
  if ( !(GetRecParam()->FitLEDEvents()) && GetRecParam()->GetEventSpecie()==AliRecoParam::kCalib) doFit = kFALSE;
  if (doFit){
    //must be done here because, in constructor, option is not yet known
    fgRawUtils->SetOption(GetOption());
    
    //  fgRawUtils->SetRawFormatHighLowGainFactor(GetRecParam()->GetHighLowGainFactor());
    
    //   fgRawUtils->SetRawFormatOrder(GetRecParam()->GetOrderParameter());
    //    fgRawUtils->SetRawFormatTau(GetRecParam()->GetTau());
    fgRawUtils->SetNoiseThreshold(GetRecParam()->GetNoiseThreshold());
    fgRawUtils->SetNPedSamples(GetRecParam()->GetNPedSamples());
    fgRawUtils->SetRemoveBadChannels(GetRecParam()->GetRemoveBadChannels());
    fgRawUtils->SetFittingAlgorithm(GetRecParam()->GetFittingAlgorithm());
    fgRawUtils->SetFALTROUsage(GetRecParam()->UseFALTRO());
    
    //fgRawUtils->SetTimeMin(GetRecParam()->GetTimeMin());
    //fgRawUtils->SetTimeMax(GetRecParam()->GetTimeMax());
    
    //  fgRawUtils->SetTimeMin(-99999 );
    //  fgRawUtils->SetTimeMax( 99999 );
    
    fgRawUtils->Raw2Digits(rawReader,fgDigitsArr,fPedestalData,digitsTrg,fTriggerData);
    
  }//skip calibration event
  else{
    AliDebug(1," Calibration Event, skip!");
  }
  
  digitsTree->Fill();
  digitsTrg->Delete();
  delete digitsTrg;
  
}


//____________________________________________________________________________
void AliEMCALReconstructor::FillESD(TTree* digitsTree, TTree* clustersTree, 
				    AliESDEvent* esd) const
{
  // Called by AliReconstruct after Reconstruct() and global tracking and vertexing 
  // and V0 
  // Works on the current event
  // printf(" ## AliEMCALReconstructor::FillESD() is started ### \n ");
  //return;
  
  //########################################
  // Trigger
  //########################################
  
  static int saveOnce = 0;
  
  Int_t v0M[2] = {0, 0};
  
  AliESDVZERO* esdV0 = esd->GetVZEROData();
  
  if (esdV0) 
    {
      for (Int_t i = 0; i < 32; i++)
	{
	  v0M[0] += (Int_t)esdV0->GetAdcV0C(i);
	  v0M[1] += (Int_t)esdV0->GetAdcV0A(i);
	}
    }
  else
    {
      AliWarning("Cannot retrieve V0 ESD! Run w/ null V0 charges");
    }
  
  if (fgTriggerDigits) fgTriggerDigits->Clear();
  
  TBranch *branchtrg = digitsTree->GetBranch("EMTRG");
  
  if (!branchtrg) 
    { 
      AliError("Can't get the branch with the EMCAL trigger digits!");
      return;
    }
  
  branchtrg->SetAddress(&fgTriggerDigits);
  branchtrg->GetEntry(0);
  
  // Note: fgTriggerProcessor reset done at the end of this method
  fgTriggerProcessor->Digits2Trigger(fgTriggerDigits, v0M, fTriggerData);
  
  // Fill ESD
  AliESDCaloTrigger* trgESD = esd->GetCaloTrigger("EMCAL");
  
  if (trgESD)
    {
      trgESD->Allocate(fgTriggerDigits->GetEntriesFast());
      
      for (Int_t i = 0; i < fgTriggerDigits->GetEntriesFast(); i++)
	{	  
	  AliEMCALTriggerRawDigit* rdig = (AliEMCALTriggerRawDigit*)fgTriggerDigits->At(i);
	  
	  Int_t px, py;
	  if (fGeom->GetPositionInEMCALFromAbsFastORIndex(rdig->GetId(), px, py))
	    {
	      Int_t a = -1, t = -1, times[10]; 
	      
	      rdig->GetMaximum(a, t);
	      rdig->GetL0Times(times);
	      
	      trgESD->Add(px, py, a, t, times, rdig->GetNL0Times(), rdig->GetL1TimeSum(), rdig->GetTriggerBits());
	    }
	}
      
      trgESD->SetL1Threshold(0, fTriggerData->GetL1GammaThreshold());
      
      trgESD->SetL1Threshold(1, fTriggerData->GetL1JetThreshold()  );
      
      Int_t v0[2];
      fTriggerData->GetL1V0(v0);
      
      trgESD->SetL1V0(v0);	
      trgESD->SetL1FrameMask(fTriggerData->GetL1FrameMask());            
      
      if (!saveOnce && fTriggerData->GetL1DataDecoded()) 
	{
	  int type[8] = {0};
	  fTriggerData->GetL1TriggerType(type);
	  
	  esd->SetCaloTriggerType(type);
	  
	  saveOnce = 1;
	}
    }
  
  // Resetting
  fTriggerData->Reset();
  
  //########################################
  //##############Fill CaloCells###############
  //########################################
  
  //Get input digits and put them in fgDigitsArr, clear the list before 
  ReadDigitsArrayFromTree(digitsTree);
  
  Int_t nDigits = fgDigitsArr->GetEntries(), idignew = 0 ;
  AliDebug(1,Form("%d digits",nDigits));
  
  AliESDCaloCells &emcCells = *(esd->GetEMCALCells());
  emcCells.CreateContainer(nDigits);
  emcCells.SetType(AliVCaloCells::kEMCALCell);
  Float_t energy = 0;
  for (Int_t idig = 0 ; idig < nDigits ; idig++) {
    const AliEMCALDigit * dig = (const AliEMCALDigit*)fgDigitsArr->At(idig);
    if(dig->GetAmplitude() > 0 ){
      energy = fgClusterizer->Calibrate(dig->GetAmplitude(),dig->GetTime(),dig->GetId()); //TimeR or Time?
      if(energy > 0){ //Digits tagged as bad (dead, hot, not alive) are set to 0 in calibrate, remove them	
        emcCells.SetCell(idignew,dig->GetId(),energy, dig->GetTime());   
        idignew++;
      }
    }
  }
  emcCells.SetNumberOfCells(idignew);
  emcCells.Sort();
  
  //------------------------------------------------------------
  //-----------------CLUSTERS-----------------------------
  //------------------------------------------------------------
  clustersTree->SetBranchStatus("*",0); //disable all branches
  clustersTree->SetBranchStatus("EMCALECARP",1); //Enable only the branch we need
  if(fgClustersArr) fgClustersArr->Clear();
  TBranch *branch = clustersTree->GetBranch("EMCALECARP");
  branch->SetAddress(&fgClustersArr);
  branch->GetEntry(0);
  //clustersTree->GetEvent(0);
  
  Int_t nClusters = fgClustersArr->GetEntries(),  nClustersNew=0;
  AliDebug(1,Form("%d clusters",nClusters));
  
  //######################################################
  //#######################TRACK MATCHING###############
  //######################################################
  //Fill list of integers, each one is index of track to which the cluster belongs.
  
  // step 1 - initialize array of matched track indexes
  Int_t *matchedTrack = new Int_t[nClusters];
  for (Int_t iclus = 0; iclus < nClusters; iclus++)
    matchedTrack[iclus] = -1;  // neg. index --> no matched track
  
  // step 2, change the flag for all matched clusters found in tracks
  Int_t iemcalMatch = -1;
  Int_t endtpc = esd->GetNumberOfTracks();
  for (Int_t itrack = 0; itrack < endtpc; itrack++) {
    AliESDtrack * track = esd->GetTrack(itrack) ; // retrieve track
    iemcalMatch = track->GetEMCALcluster();
    if(iemcalMatch >= 0) matchedTrack[iemcalMatch] = itrack;
  } 
  
  //########################################
  //##############Fill CaloClusters#############
  //########################################
  for (Int_t iClust = 0 ; iClust < nClusters ; iClust++) {
    const AliEMCALRecPoint * clust = (const AliEMCALRecPoint*)fgClustersArr->At(iClust);
    //if(clust->GetClusterType()== AliVCluster::kEMCALClusterv1) nRP++; else nPC++;
    // clust->Print(); //For debugging
    // Get information from EMCAL reconstruction points
    Float_t xyz[3];
    TVector3 gpos;
    clust->GetGlobalPosition(gpos);
    for (Int_t ixyz=0; ixyz<3; ixyz++)
      xyz[ixyz] = gpos[ixyz];
    Float_t elipAxis[2];
    clust->GetElipsAxis(elipAxis);
    //Create digits lists
    Int_t cellMult = clust->GetMultiplicity();
    //TArrayS digiList(digitMult);
    Float_t *amplFloat = clust->GetEnergiesList();
    Int_t   *digitInts = clust->GetAbsId();
    TArrayS absIdList(cellMult);
    TArrayD fracList(cellMult);
    
    Int_t newCellMult = 0;
    for (Int_t iCell=0; iCell<cellMult; iCell++) {
      if (amplFloat[iCell] > 0) {
        absIdList[newCellMult] = (UShort_t)(digitInts[iCell]);
        //Calculate Fraction
        if(emcCells.GetCellAmplitude(digitInts[iCell])>0 && GetRecParam()->GetUnfold()){
          fracList[newCellMult] = amplFloat[iCell]/(emcCells.GetCellAmplitude(digitInts[iCell]));//get cell calibration value 
          
        }
        else{
          fracList[newCellMult] = 0; 
        }
        newCellMult++;
      }
    }
    
    absIdList.Set(newCellMult);
    fracList.Set(newCellMult);
    
    if(newCellMult > 0) { // accept cluster if it has some digit
      nClustersNew++;
      //Primaries
      Int_t  parentMult  = 0;
      Int_t *parentList =  clust->GetParents(parentMult);
      // fills the ESDCaloCluster
      AliESDCaloCluster * ec = new AliESDCaloCluster() ;
      ec->SetType(AliVCluster::kEMCALClusterv1);
      ec->SetPosition(xyz);
      ec->SetE(clust->GetEnergy());
      
      //Distance to the nearest bad crystal
      ec->SetDistanceToBadChannel(clust->GetDistanceToBadTower()); 
      
      ec->SetNCells(newCellMult);
      //Change type of list from short to ushort
      UShort_t *newAbsIdList  = new UShort_t[newCellMult];
      Double_t *newFracList   = new Double_t[newCellMult];
      for(Int_t i = 0; i < newCellMult ; i++) {
        newAbsIdList[i]=absIdList[i];
        newFracList[i] =fracList[i];
      }
      ec->SetCellsAbsId(newAbsIdList);
      ec->SetCellsAmplitudeFraction(newFracList);
      ec->SetDispersion(clust->GetDispersion());
      ec->SetChi2(-1); //not yet implemented
      ec->SetM02(elipAxis[0]*elipAxis[0]) ;
      ec->SetM20(elipAxis[1]*elipAxis[1]) ;
      ec->SetTOF(clust->GetTime()) ; //time-of-fligh
      ec->SetNExMax(clust->GetNExMax());          //number of local maxima
      TArrayI arrayTrackMatched(1);// Only one track, temporal solution.
      arrayTrackMatched[0]= matchedTrack[iClust];
      ec->AddTracksMatched(arrayTrackMatched);
      
      TArrayI arrayParents(parentMult,parentList);
      ec->AddLabels(arrayParents);
      
      // add the cluster to the esd object
      esd->AddCaloCluster(ec);
      delete ec;
      delete [] newAbsIdList ;
      delete [] newFracList ;
    }
  } // cycle on clusters
  
  delete [] matchedTrack;
  
  //Fill ESDCaloCluster with PID weights
  AliEMCALPID *pid = new AliEMCALPID;
  //pid->SetPrintInfo(kTRUE);
  pid->SetReconstructor(kTRUE);
  pid->RunPID(esd);
  delete pid;
  
  //Store EMCAL misalignment matrixes
  FillMisalMatrixes(esd) ;
  
}

//==================================================================================
void AliEMCALReconstructor::FillMisalMatrixes(AliESDEvent* esd)const{
  //Store EMCAL matrixes in ESD Header
  
  //Check, if matrixes was already stored
  for(Int_t sm = 0 ; sm < fGeom->GetNumberOfSuperModules(); sm++){
    if(esd->GetEMCALMatrix(sm)!=0)
      return ;
  }
  
  //Create and store matrixes
  if(!gGeoManager){
    AliError("Can not store misal. matrixes: no gGeoManager! \n") ;
    return ;
  }
  //Note, that owner of copied marixes will be header
  const Int_t bufsize = 255;
  char path[bufsize] ;
  TGeoHMatrix * m = 0x0;
  for(Int_t sm = 0; sm < fGeom->GetNumberOfSuperModules(); sm++){
    snprintf(path,bufsize,"/ALIC_1/XEN1_1/SMOD_%d",sm+1) ; //In Geometry modules numbered 1,2,.,5
    if(sm >= 10) snprintf(path,bufsize,"/ALIC_1/XEN1_1/SM10_%d",sm-10+1) ;
    
    if (gGeoManager->CheckPath(path)){
      gGeoManager->cd(path);
      m = gGeoManager->GetCurrentMatrix() ;
      //			printf("================================================= \n");
      //			printf("AliEMCALReconstructor::FixMisalMatrixes(), sm %d, \n",sm);
      //			m->Print("");
      esd->SetEMCALMatrix(new TGeoHMatrix(*m),sm) ;
      //			printf("================================================= \n");
    }
    else{
      esd->SetEMCALMatrix(NULL,sm) ;
    }
  }
}

//__________________________________________________________________________
void AliEMCALReconstructor::ReadDigitsArrayFromTree(TTree *digitsTree) const
{
  // Read the digits from the input tree
  // See AliEMCALClusterizer::SetInput(TTree *digitsTree);    
  
  // Clear previous digits in the list
  if(fgDigitsArr){ 
    fgDigitsArr->Clear("C");
  }
  else{
    // It should not happen, but just in case ...
    fgDigitsArr = new TClonesArray("AliEMCALDigit",100); 
  }
  
  // Read the digits from the input tree
  TBranch *branch = digitsTree->GetBranch("EMCAL");
  if (!branch) { 
    AliError("can't get the branch with the EMCAL digits !");
    return;
  }  
  
  branch->SetAddress(&fgDigitsArr);
  branch->GetEntry(0);
}


