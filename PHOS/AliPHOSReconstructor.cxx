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

// --- Standard library ---

// --- AliRoot header files ---
#include "AliLog.h"
#include "AliAltroMapping.h"
#include "AliESDEvent.h"
#include "AliESDCaloCluster.h"
#include "AliESDCaloCells.h"
#include "AliPHOSReconstructor.h"
#include "AliPHOSClusterizerv1.h"
#include "AliPHOSTrackSegmentMakerv1.h"
#include "AliPHOSPIDv1.h"
#include "AliPHOSTracker.h"
#include "AliRawReader.h"
#include "AliPHOSTrigger.h"
#include "AliPHOSGeometry.h"
#include "AliPHOSRecoParam.h"
#include "AliPHOSRecoParamEmc.h"
#include "AliPHOSRecoParamCpv.h"
#include "AliPHOSDigit.h"
#include "AliPHOSTrackSegment.h"
#include "AliPHOSEmcRecPoint.h"
#include "AliPHOSRecParticle.h"
#include "AliPHOSRawDecoder.h"
#include "AliPHOSRawDecoderv1.h"
#include "AliPHOSRawDecoderv2.h"
#include "AliPHOSRawDigiProducer.h"
#include "AliPHOSPulseGenerator.h"

ClassImp(AliPHOSReconstructor)

Bool_t AliPHOSReconstructor::fgDebug = kFALSE ; 
AliPHOSRecoParam* AliPHOSReconstructor::fgkRecoParamEmc =0;  // EMC rec. parameters
AliPHOSRecoParam* AliPHOSReconstructor::fgkRecoParamCpv =0;  // CPV rec. parameters

//____________________________________________________________________________
AliPHOSReconstructor::AliPHOSReconstructor() :
  fGeom(NULL),fClusterizer(NULL),fTSM(NULL),fPID(NULL)
{
  // ctor

  if (!fgkRecoParamEmc) {
    AliWarning("The Reconstruction parameters for EMC nonitialized - Used default one");
    fgkRecoParamEmc = AliPHOSRecoParamEmc::GetEmcDefaultParameters();
  }

  if (!fgkRecoParamCpv) {
    AliWarning("The Reconstruction parameters for CPV nonitialized - Used default one");
    fgkRecoParamCpv = AliPHOSRecoParamCpv::GetCpvDefaultParameters();
  }

  fGeom        = AliPHOSGeometry::GetInstance("IHEP","");
  fClusterizer = new AliPHOSClusterizerv1      (fGeom);
  fTSM         = new AliPHOSTrackSegmentMakerv1(fGeom);
  fPID         = new AliPHOSPIDv1              (fGeom);
}

//____________________________________________________________________________
  AliPHOSReconstructor::~AliPHOSReconstructor()
{
  // dtor
  delete fGeom;
  delete fClusterizer;
  delete fTSM;
  delete fPID;
} 

//____________________________________________________________________________
void AliPHOSReconstructor::Reconstruct(TTree* digitsTree, TTree* clustersTree) const
{
  // 'single-event' local reco method called by AliReconstruction; 
  // Only the clusterization is performed,; the rest of the reconstruction is done in FillESD because the track
  // segment maker needs access to the AliESDEvent object to retrieve the tracks reconstructed by 
  // the global tracking.

  fClusterizer->SetInput(digitsTree);
  fClusterizer->SetOutput(clustersTree);
  if ( Debug() ) 
    fClusterizer->Digits2Clusters("deb all") ; 
  else 
    fClusterizer->Digits2Clusters("") ;
}

//____________________________________________________________________________
void AliPHOSReconstructor::FillESD(TTree* digitsTree, TTree* clustersTree, 
				   AliESDEvent* esd) const
{
  // This method produces PHOS rec-particles,
  // then it creates AliESDtracks out of them and
  // write tracks to the ESD


  // do current event; the loop over events is done by AliReconstruction::Run()
  fTSM->SetESD(esd) ; 
  fTSM->SetInput(clustersTree);
  if ( Debug() ) 
    fTSM->Clusters2TrackSegments("deb all") ;
  else 
    fTSM->Clusters2TrackSegments("") ;
  
  fPID->SetInput(clustersTree, fTSM->GetTrackSegments()) ; 
  fPID->SetESD(esd) ; 
  if ( Debug() ) 
    fPID->TrackSegments2RecParticles("deb all") ;
  else 
    fPID->TrackSegments2RecParticles("") ;


  // This function creates AliESDtracks from AliPHOSRecParticles
  //         and
  // writes them to the ESD

  TClonesArray *recParticles  = fPID->GetRecParticles();
  Int_t nOfRecParticles = recParticles->GetEntriesFast();
  
  esd->SetNumberOfPHOSClusters(nOfRecParticles) ; 
  esd->SetFirstPHOSCluster(esd->GetNumberOfCaloClusters()) ;
  
  AliDebug(2,Form("%d rec. particles, option %s",nOfRecParticles,GetOption()));
  
  // Read digits array

  TBranch *branch = digitsTree->GetBranch("PHOS");
  if (!branch) { 
    AliError("can't get the branch with the PHOS digits !");
    return;
  }
  TClonesArray *digitsArray    = new TClonesArray("AliPHOSDigit",100);
  branch->SetAddress(&digitsArray);
  branch->GetEntry(0);

  // Get the clusters array

  TBranch *emcbranch = clustersTree->GetBranch("PHOSEmcRP");
  if (!emcbranch) { 
    AliError("can't get the branch with the PHOS EMC clusters !");
    return;
  }

  TObjArray *emcRecPoints = new TObjArray(100) ;
  emcbranch->SetAddress(&emcRecPoints);
  emcbranch->GetEntry(0);

  //#########Calculate trigger and set trigger info###########

  AliPHOSTrigger tr ;
  //   tr.SetPatchSize(1);//create 4x4 patches
  tr.SetSimulation(kFALSE);
  tr.Trigger(digitsArray);
  
  Float_t maxAmp2x2  = tr.Get2x2MaxAmplitude();
  Float_t maxAmpnxn  = tr.GetnxnMaxAmplitude();
  Float_t ampOutOfPatch2x2  = tr.Get2x2AmpOutOfPatch() ;
  Float_t ampOutOfPatchnxn  = tr.GetnxnAmpOutOfPatch() ;

  Int_t iSM2x2      = tr.Get2x2SuperModule();
  Int_t iSMnxn      = tr.GetnxnSuperModule();
  Int_t iCrystalPhi2x2 = tr.Get2x2CrystalPhi();
  Int_t iCrystalPhinxn = tr.GetnxnCrystalPhi();
  Int_t iCrystalEta2x2 = tr.Get2x2CrystalEta();
  Int_t iCrystalEtanxn = tr.GetnxnCrystalEta();

  AliDebug(2, Form("Trigger 2x2 max amp %f, out amp %f, SM %d, iphi %d ieta %d",  
		   maxAmp2x2, ampOutOfPatch2x2, iSM2x2,iCrystalPhi2x2, iCrystalEta2x2));
  AliDebug(2, Form("Trigger 4x4 max amp %f , out amp %f, SM %d, iphi %d, ieta %d",
		   maxAmpnxn, ampOutOfPatchnxn, iSMnxn,iCrystalPhinxn, iCrystalEtanxn));

  // Attention! PHOS modules in order to calculate AbsId need to be 1-5 not 0-4 as returns trigger.
  Int_t iRelId2x2 []= {iSM2x2+1,0,iCrystalPhi2x2,iCrystalEta2x2};
  Int_t iAbsId2x2 =-1;
  Int_t iRelIdnxn []= {iSMnxn+1,0,iCrystalPhinxn,iCrystalEtanxn};
  Int_t iAbsIdnxn =-1;
  TVector3    pos2x2(-1,-1,-1);
  TVector3    posnxn(-1,-1,-1);
  fGeom->RelToAbsNumbering(iRelId2x2, iAbsId2x2);
  fGeom->RelToAbsNumbering(iRelIdnxn, iAbsIdnxn);
  fGeom->RelPosInAlice(iAbsId2x2, pos2x2);
  fGeom->RelPosInAlice(iAbsIdnxn, posnxn);

  TArrayF triggerPosition(6);
  triggerPosition[0] = pos2x2(0) ;   
  triggerPosition[1] = pos2x2(1) ;   
  triggerPosition[2] = pos2x2(2) ;  
  triggerPosition[3] = posnxn(0) ;   
  triggerPosition[4] = posnxn(1) ;   
  triggerPosition[5] = posnxn(2) ;  

  TArrayF triggerAmplitudes(4);
  triggerAmplitudes[0] = maxAmp2x2 ;   
  triggerAmplitudes[1] = ampOutOfPatch2x2 ;    
  triggerAmplitudes[2] = maxAmpnxn ;   
  triggerAmplitudes[3] = ampOutOfPatchnxn ;   

  //esd->SetPHOSTriggerCells(triggerPosition);
  esd->AddPHOSTriggerPosition(triggerPosition);
  esd->AddPHOSTriggerAmplitudes(triggerAmplitudes);
  

  //########################################
  //############# Fill CaloCells ###########
  //########################################

  Int_t nDigits = digitsArray->GetEntries();
  Int_t idignew = 0 ;
  AliDebug(1,Form("%d digits",nDigits));

  const Int_t knEMC = fGeom->GetNModules()*fGeom->GetNPhi()*fGeom->GetNZ();
  AliESDCaloCells &phsCells = *(esd->GetPHOSCells());
  phsCells.CreateContainer(nDigits);
  phsCells.SetType(AliESDCaloCells::kPHOSCell);

  // Add to CaloCells only EMC digits with non-zero energy 
  for (Int_t idig = 0 ; idig < nDigits ; idig++) {
    const AliPHOSDigit * dig = (const AliPHOSDigit*)digitsArray->At(idig);
    if(dig->GetId() <= knEMC && dig->GetEnergy() > 0 ){
      //printf("i %d; id %d; amp %f; time %e\n",
      //idignew,dig->GetId(),dig->GetEnergy(), dig->GetTime());
      phsCells.SetCell(idignew,dig->GetId(), dig->GetEnergy(), dig->GetTime());   
      idignew++;
    }
  }
  phsCells.SetNumberOfCells(idignew);
  phsCells.Sort();

  //########################################
  //############## Fill CaloClusters #######
  //########################################

  for (Int_t recpart = 0 ; recpart < nOfRecParticles ; recpart++) {
    AliPHOSRecParticle  *rp    = dynamic_cast<AliPHOSRecParticle*>(recParticles->At(recpart));
    if (Debug()) 
      rp->Print();
    // Get track segment and EMC rec.point associated with this rec.particle
    AliPHOSTrackSegment *ts    = static_cast<AliPHOSTrackSegment *>(fTSM->GetTrackSegments()
								    ->At(rp->GetPHOSTSIndex()));

    AliPHOSEmcRecPoint  *emcRP = static_cast<AliPHOSEmcRecPoint *>(emcRecPoints->At(ts->GetEmcIndex()));
    AliESDCaloCluster   *ec    = new AliESDCaloCluster() ; 
    
    Float_t xyz[3];
    for (Int_t ixyz=0; ixyz<3; ixyz++) 
      xyz[ixyz] = rp->GetPos()[ixyz];
    
    AliDebug(2,Form("Global position xyz=(%f,%f,%f)",xyz[0],xyz[1],xyz[2]));
   
    // Create cell lists

    Int_t     cellMult   = emcRP->GetDigitsMultiplicity();
    Int_t    *digitsList = emcRP->GetDigitsList();
    Float_t  *rpElist    = emcRP->GetEnergiesList() ;
    UShort_t *absIdList  = new UShort_t[cellMult];
    Double_t *fracList   = new Double_t[cellMult];

    for (Int_t iCell=0; iCell<cellMult; iCell++) {
      AliPHOSDigit *digit = static_cast<AliPHOSDigit *>(digitsArray->At(digitsList[iCell]));
      absIdList[iCell] = (UShort_t)(digit->GetId());
      if (digit->GetEnergy() > 0)
 	fracList[iCell] = rpElist[iCell]/digit->GetEnergy();
      else
 	fracList[iCell] = 0;
    }

    //Primaries
    Int_t  primMult  = 0;
    Int_t *primList =  emcRP->GetPrimaries(primMult);

    Float_t energy;
    if (fgkRecoParamEmc->Ecore2ESD())
      energy = emcRP->GetCoreEnergy();
    else
      energy = rp->Energy();

    // fills the ESDCaloCluster
    ec->SetClusterType(AliESDCaloCluster::kPHOSCluster);
    ec->SetPosition(xyz);                       //rec.point position in MARS
    ec->SetE(energy);                           //total or core particle energy
    ec->SetClusterDisp(emcRP->GetDispersion()); //cluster dispersion
    ec->SetPid(rp->GetPID()) ;                  //array of particle identification
    ec->SetM02(emcRP->GetM2x()) ;               //second moment M2x
    ec->SetM20(emcRP->GetM2z()) ;               //second moment M2z
    ec->SetNExMax(emcRP->GetNExMax());          //number of local maxima
    ec->SetEmcCpvDistance(ts->GetCpvDistance("r")); //Only radius, what about separate x,z????
    ec->SetClusterChi2(-1);                     //not yet implemented
    ec->SetTOF(emcRP->GetTime()); //Time of flight

    //Cells contributing to clusters
    ec->SetNCells(cellMult);
    ec->SetCellsAbsId(absIdList);
    ec->SetCellsAmplitudeFraction(fracList);

    //Distance to the nearest bad crystal
    ec->SetDistanceToBadChannel(emcRP->GetDistanceToBadCrystal()); 
  
    //Array of MC indeces
    TArrayI arrayPrim(primMult,primList);
    ec->AddLabels(arrayPrim);

    //Array of tracks uncomment when available in future
    //TArrayS arrayTrackMatched(1);// Only one track, temporal solution.
    //arrayTrackMatched[0]= (Short_t)(matchedTrack[iClust]);
    //ec->AddTracksMatched(arrayTrackMatched);
    
    // add the track to the esd object

    esd->AddCaloCluster(ec);
    delete ec;   
  }
  digitsArray ->Delete();
  delete digitsArray;
  emcRecPoints->Delete();
  delete emcRecPoints;
  recParticles->Delete();
}

//____________________________________________________________________________
AliTracker* AliPHOSReconstructor::CreateTracker() const
{
  // creates the PHOS tracker
  return new AliPHOSTracker();
}

//____________________________________________________________________________
void  AliPHOSReconstructor::ConvertDigits(AliRawReader* rawReader, TTree* digitsTree) const
{
  // Converts raw data to
  // PHOS digits
  // Works on a single-event basis

  rawReader->Reset() ; 

  AliPHOSRawDecoder * dc ;

  const TObjArray* maps = AliPHOSRecoParamEmc::GetMappings();
  if(!maps) AliFatal("Cannot retrieve ALTRO mappings!!");

  AliAltroMapping *mapping[4];
  for(Int_t i = 0; i < 4; i++) {
    mapping[i] = (AliAltroMapping*)maps->At(i);
  }

  if(strcmp(fgkRecoParamEmc->DecoderVersion(),"v1")==0) 
    dc=new AliPHOSRawDecoderv1(rawReader,mapping);
  else 
    if(strcmp(fgkRecoParamEmc->DecoderVersion(),"v2")==0) 
      dc=new AliPHOSRawDecoderv2(rawReader,mapping);
    else
      dc=new AliPHOSRawDecoder(rawReader,mapping);

  dc->SubtractPedestals(fgkRecoParamEmc->SubtractPedestals());
  
  TClonesArray *digits = new TClonesArray("AliPHOSDigit",1);
  digits->SetName("DIGITS");
  Int_t bufsize = 32000;
  digitsTree->Branch("PHOS", &digits, bufsize);

  AliPHOSRawDigiProducer pr(fgkRecoParamEmc,fgkRecoParamCpv);
  pr.MakeDigits(digits,dc);

  delete dc ;

  //!!!!for debug!!!
/*
  Int_t modMax=-111;
  Int_t colMax=-111;
  Int_t rowMax=-111;
  Float_t eMax=-333;
  //!!!for debug!!!

  Int_t relId[4];
  for(Int_t iDigit=0; iDigit<digits->GetEntries(); iDigit++) {
    AliPHOSDigit* digit = (AliPHOSDigit*)digits->At(iDigit);
    if(digit->GetEnergy()>eMax) {
      fGeom->AbsToRelNumbering(digit->GetId(),relId);
      eMax=digit->GetEnergy();
      modMax=relId[0];
      rowMax=relId[2];
      colMax=relId[3];
    }
  }

  AliDebug(1,Form("Digit with max. energy:  modMax %d colMax %d rowMax %d  eMax %f\n\n",
		  modMax,colMax,rowMax,eMax));
*/
  digitsTree->Fill();
  digits->Delete();
  delete digits;
}
