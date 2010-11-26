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

/* $Id: AliEMCALRecoUtils.cxx 33808 2009-07-15 09:48:08Z gconesab $ */

///////////////////////////////////////////////////////////////////////////////
//
// Class AliEMCALRecoUtils
// Some utilities to recalculate the cluster position or energy linearity
//
//
// Author:  Gustavo Conesa (LPSC- Grenoble) 
///////////////////////////////////////////////////////////////////////////////

// --- standard c ---

// standard C++ includes
//#include <Riostream.h>

// ROOT includes
#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoBBox.h>

// STEER includes
#include "AliEMCALRecoUtils.h"
#include "AliEMCALGeometry.h"
#include "AliVCluster.h"
#include "AliVCaloCells.h"
#include "AliVEvent.h"
#include "AliLog.h"
#include "AliEMCALPIDUtils.h"
#include "AliPID.h"
#include "AliESDEvent.h"
#include "AliESDtrack.h"
#include "AliEMCALTrack.h"

ClassImp(AliEMCALRecoUtils)
  
//______________________________________________
AliEMCALRecoUtils::AliEMCALRecoUtils():
  fNonLinearityFunction (kNoCorrection), fParticleType(kPhoton),
  fPosAlgo(kUnchanged),fW0(4.),
  fRecalibration(kFALSE), fEMCALRecalibrationFactors(),
  fRemoveBadChannels(kFALSE), fRecalDistToBadChannels(kFALSE), fEMCALBadChannelMap(),
  fNCellsFromEMCALBorder(0), fNoEMCALBorderAtEta0(kTRUE),
  fMatchedClusterIndex(0x0), fResidualZ(0x0), fResidualR(0x0), fCutR(20), fCutZ(20),
  fCutMinNClusterTPC(0), fCutMinNClusterITS(0), fCutMaxChi2PerClusterTPC(0), fCutMaxChi2PerClusterITS(0),
  fCutRequireTPCRefit(0), fCutRequireITSRefit(0), fCutAcceptKinkDaughters(0),
  fCutMaxDCAToVertexXY(0), fCutMaxDCAToVertexZ(0),fCutDCAToVertex2D(0),
  fPIDUtils()
{
//
  // Constructor.
  // Initialize all constant values which have to be used
  // during Reco algorithm execution
  //
  
  for(Int_t i = 0; i < 15 ; i++) {
      fMisalTransShift[i] = 0.; 
      fMisalRotShift[i] = 0.; 
  }
  for(Int_t i = 0; i < 6  ; i++) fNonLinearityParams[i] = 0.; 
  //For kPi0GammaGamma case, but default is no correction
  fNonLinearityParams[0] = 0.1457/0.1349766/1.038;
  fNonLinearityParams[1] = -0.02024/0.1349766/1.038;
  fNonLinearityParams[2] = 1.046;

  fMatchedClusterIndex = new TArrayI();
  fResidualZ = new TArrayF();
  fResidualR = new TArrayF();
  
  fPIDUtils = new AliEMCALPIDUtils();

  InitTrackCuts();

}

//______________________________________________________________________
AliEMCALRecoUtils::AliEMCALRecoUtils(const AliEMCALRecoUtils & reco) 
: TNamed(reco), fNonLinearityFunction(reco.fNonLinearityFunction), 
  fParticleType(reco.fParticleType), fPosAlgo(reco.fPosAlgo), fW0(reco.fW0), 
  fRecalibration(reco.fRecalibration),fEMCALRecalibrationFactors(reco.fEMCALRecalibrationFactors),
  fRemoveBadChannels(reco.fRemoveBadChannels),fRecalDistToBadChannels(reco.fRecalDistToBadChannels),
  fEMCALBadChannelMap(reco.fEMCALBadChannelMap),
  fNCellsFromEMCALBorder(reco.fNCellsFromEMCALBorder),fNoEMCALBorderAtEta0(reco.fNoEMCALBorderAtEta0),
  fMatchedClusterIndex(reco.fMatchedClusterIndex?new TArrayI(*reco.fMatchedClusterIndex):0x0),
  fResidualZ(reco.fResidualZ?new TArrayF(*reco.fResidualZ):0x0),
  fResidualR(reco.fResidualR?new TArrayF(*reco.fResidualR):0x0),
  fCutR(reco.fCutR),fCutZ(reco.fCutZ),
  fCutMinNClusterTPC(reco.fCutMinNClusterTPC), fCutMinNClusterITS(reco.fCutMinNClusterITS), 
  fCutMaxChi2PerClusterTPC(reco.fCutMaxChi2PerClusterTPC), fCutMaxChi2PerClusterITS(reco.fCutMaxChi2PerClusterITS),
  fCutRequireTPCRefit(reco.fCutRequireTPCRefit), fCutRequireITSRefit(reco.fCutRequireITSRefit),
  fCutAcceptKinkDaughters(reco.fCutAcceptKinkDaughters),
  fCutMaxDCAToVertexXY(reco.fCutMaxDCAToVertexXY), fCutMaxDCAToVertexZ(reco.fCutMaxDCAToVertexZ),fCutDCAToVertex2D(reco.fCutDCAToVertex2D),
  fPIDUtils(reco.fPIDUtils)

{
  //Copy ctor
  
  for(Int_t i = 0; i < 15 ; i++) {
      fMisalRotShift[i] = reco.fMisalRotShift[i]; 
      fMisalTransShift[i] = reco.fMisalTransShift[i]; 
  } 
  for(Int_t i = 0; i < 6  ; i++) fNonLinearityParams[i] = reco.fNonLinearityParams[i]; 

}


//______________________________________________________________________
AliEMCALRecoUtils & AliEMCALRecoUtils::operator = (const AliEMCALRecoUtils & reco) 
{
  //Assignment operator
  
  if(this == &reco)return *this;
  ((TNamed *)this)->operator=(reco);

  fNonLinearityFunction  = reco.fNonLinearityFunction;
  fParticleType          = reco.fParticleType;
  fPosAlgo               = reco.fPosAlgo; 
  fW0                    = reco.fW0;
  fRecalibration         = reco.fRecalibration;
  fEMCALRecalibrationFactors = reco.fEMCALRecalibrationFactors;
  fRemoveBadChannels     = reco.fRemoveBadChannels;
  fRecalDistToBadChannels= reco.fRecalDistToBadChannels;
  fEMCALBadChannelMap    = reco.fEMCALBadChannelMap;
  fNCellsFromEMCALBorder = reco.fNCellsFromEMCALBorder;
  fNoEMCALBorderAtEta0   = reco.fNoEMCALBorderAtEta0;


  for(Int_t i = 0; i < 15 ; i++) {fMisalTransShift[i] = reco.fMisalTransShift[i]; fMisalRotShift[i] = reco.fMisalRotShift[i];}
  for(Int_t i = 0; i < 6  ; i++) fNonLinearityParams[i] = reco.fNonLinearityParams[i]; 
  
  fCutR                  = reco.fCutR;
  fCutZ                  = reco.fCutZ;

  fCutMinNClusterTPC        = reco.fCutMinNClusterTPC;
  fCutMinNClusterITS        = reco.fCutMinNClusterITS; 
  fCutMaxChi2PerClusterTPC  = reco.fCutMaxChi2PerClusterTPC;
  fCutMaxChi2PerClusterITS  = reco.fCutMaxChi2PerClusterITS;
  fCutRequireTPCRefit       = reco.fCutRequireTPCRefit;
  fCutRequireITSRefit       = reco.fCutRequireITSRefit;
  fCutAcceptKinkDaughters   = reco.fCutAcceptKinkDaughters;
  fCutMaxDCAToVertexXY      = reco.fCutMaxDCAToVertexXY;
  fCutMaxDCAToVertexZ       = reco.fCutMaxDCAToVertexZ;
  fCutDCAToVertex2D         = reco.fCutDCAToVertex2D;

  fPIDUtils              = reco.fPIDUtils;
  
  
  if(reco.fResidualR){
    // assign or copy construct
    if(fResidualR){ 
      *fResidualR = *reco.fResidualR;
    }
    else fResidualR = new TArrayF(*reco.fResidualR);
  }
  else{
    if(fResidualR)delete fResidualR;
    fResidualR = 0;
  }
  
  if(reco.fResidualZ){
    // assign or copy construct
    if(fResidualZ){ 
      *fResidualZ = *reco.fResidualZ;
    }
    else fResidualZ = new TArrayF(*reco.fResidualZ);
  }
  else{
    if(fResidualZ)delete fResidualZ;
    fResidualZ = 0;
  }
  
  
  if(reco.fMatchedClusterIndex){
    // assign or copy construct
    if(fMatchedClusterIndex){ 
      *fMatchedClusterIndex = *reco.fMatchedClusterIndex;
    }
    else fMatchedClusterIndex = new TArrayI(*reco.fMatchedClusterIndex);
  }
  else{
    if(fMatchedClusterIndex)delete fMatchedClusterIndex;
    fMatchedClusterIndex = 0;
  }
  
  
  return *this;
}


//__________________________________________________
AliEMCALRecoUtils::~AliEMCALRecoUtils()
{
  //Destructor.
	
	if(fEMCALRecalibrationFactors) { 
		fEMCALRecalibrationFactors->Clear();
		delete  fEMCALRecalibrationFactors;
	}	
  
  if(fEMCALBadChannelMap) { 
		fEMCALBadChannelMap->Clear();
		delete  fEMCALBadChannelMap;
	}
 
  if(fMatchedClusterIndex) {delete fMatchedClusterIndex; fMatchedClusterIndex=0;}
  if(fResidualR)           {delete fResidualR;           fResidualR=0;}
  if(fResidualZ)           {delete fResidualZ;           fResidualZ=0;}

}

//_______________________________________________________________
Bool_t AliEMCALRecoUtils::CheckCellFiducialRegion(AliEMCALGeometry* geom, AliVCluster* cluster, AliVCaloCells* cells) 
{
	// Given the list of AbsId of the cluster, get the maximum cell and 
	// check if there are fNCellsFromBorder from the calorimeter border
	
  //If the distance to the border is 0 or negative just exit accept all clusters
	if(cells->GetType()==AliVCaloCells::kEMCALCell && fNCellsFromEMCALBorder <= 0 ) return kTRUE;
  
  Int_t absIdMax	= -1, iSM =-1, ieta = -1, iphi = -1;
  Bool_t shared = kFALSE;
  GetMaxEnergyCell(geom, cells, cluster, absIdMax,  iSM, ieta, iphi, shared);

  AliDebug(2,Form("Cluster Max AbsId %d, Cell Energy %2.2f, Cluster Energy %2.2f, Ncells from border %d, EMCAL eta=0 %d\n", 
           absIdMax, cells->GetCellAmplitude(absIdMax), cluster->E(), fNCellsFromEMCALBorder, fNoEMCALBorderAtEta0));
	
	if(absIdMax==-1) return kFALSE;
	
	//Check if the cell is close to the borders:
	Bool_t okrow = kFALSE;
	Bool_t okcol = kFALSE;
  
  if(iSM < 0 || iphi < 0 || ieta < 0 ) {
    AliFatal(Form("Negative value for super module: %d, or cell ieta: %d, or cell iphi: %d, check EMCAL geometry name\n",
                  iSM,ieta,iphi));
  }
  
  //Check rows/phi
  if(iSM < 10){
    if(iphi >= fNCellsFromEMCALBorder && iphi < 24-fNCellsFromEMCALBorder) okrow =kTRUE; 
  }
  else{
    if(iphi >= fNCellsFromEMCALBorder && iphi < 12-fNCellsFromEMCALBorder) okrow =kTRUE; 
  }
  
  //Check columns/eta
  if(!fNoEMCALBorderAtEta0){
    if(ieta  > fNCellsFromEMCALBorder && ieta < 48-fNCellsFromEMCALBorder) okcol =kTRUE; 
  }
  else{
    if(iSM%2==0){
      if(ieta >= fNCellsFromEMCALBorder)     okcol = kTRUE;	
    }
    else {
      if(ieta <  48-fNCellsFromEMCALBorder)  okcol = kTRUE;	
    }
  }//eta 0 not checked
    
  AliDebug(2,Form("EMCAL Cluster in %d cells fiducial volume: ieta %d, iphi %d, SM %d:  column? %d, row? %d\nq",
           fNCellsFromEMCALBorder, ieta, iphi, iSM, okcol, okrow));
	
	if (okcol && okrow) {
    //printf("Accept\n");
    return kTRUE;
  }
	else  {
    //printf("Reject\n");
    AliDebug(2,Form("Reject cluster in border, max cell : ieta %d, iphi %d, SM %d\n",ieta, iphi, iSM));
    return kFALSE;
  }
	
}	


//_________________________________________________________________________________________________________
Bool_t AliEMCALRecoUtils::ClusterContainsBadChannel(AliEMCALGeometry* geom, UShort_t* cellList, Int_t nCells){
	// Check that in the cluster cells, there is no bad channel of those stored 
	// in fEMCALBadChannelMap or fPHOSBadChannelMap
	
	if(!fRemoveBadChannels)  return kFALSE;
	if(!fEMCALBadChannelMap) return kFALSE;
	
	Int_t icol = -1;
	Int_t irow = -1;
	Int_t imod = -1;
	for(Int_t iCell = 0; iCell<nCells; iCell++){
		
		//Get the column and row
    Int_t iTower = -1, iIphi = -1, iIeta = -1; 
    geom->GetCellIndex(cellList[iCell],imod,iTower,iIphi,iIeta); 
    if(fEMCALBadChannelMap->GetEntries() <= imod) continue;
    geom->GetCellPhiEtaIndexInSModule(imod,iTower,iIphi, iIeta,irow,icol);			
    if(GetEMCALChannelStatus(imod, icol, irow)){
      AliDebug(2,Form("Cluster with bad channel: SM %d, col %d, row %d\n",imod, icol, irow));
      return kTRUE;
    }
		
	}// cell cluster loop
	
	return kFALSE;
	
}

//__________________________________________________
Float_t AliEMCALRecoUtils::CorrectClusterEnergyLinearity(AliVCluster* cluster){
// Correct cluster energy from non linearity functions
  Float_t energy = cluster->E();
  
  switch (fNonLinearityFunction) {
      
    case kPi0MC:
      //Non-Linearity correction (from MC with function ([0]*exp(-[1]/E))+(([2]/([3]*2.*TMath::Pi())*exp(-(E-[4])^2/(2.*[3]^2)))))
      //Double_t par0 = 1.001;
      //Double_t par1 = -0.01264;
      //Double_t par2 = -0.03632;
      //Double_t par3 = 0.1798;
      //Double_t par4 = -0.522;
       energy /= (fNonLinearityParams[0]*exp(-fNonLinearityParams[1]/energy))+
                  ((fNonLinearityParams[2]/(fNonLinearityParams[3]*2.*TMath::Pi())*
                    exp(-(energy-fNonLinearityParams[4])*(energy-fNonLinearityParams[4])/(2.*fNonLinearityParams[3]*fNonLinearityParams[3]))));
      break;
      
    case kPi0GammaGamma:

      //Non-Linearity correction (from Olga Data with function p0+p1*exp(-p2*E))
      //Double_t par0 = 0.1457;
      //Double_t par1 = -0.02024;
      //Double_t par2 = 1.046;
      energy /= (fNonLinearityParams[0]+fNonLinearityParams[1]*exp(-fNonLinearityParams[2]*energy)); //Olga function
      break;
      
    case kPi0GammaConversion:
      
      //Non-Linearity correction (Nicolas from Dimitri Data with function C*[1-a*exp(-b*E)])
      //Double_t C = 0.139393/0.1349766;
      //Double_t a = 0.0566186;
      //Double_t b = 0.982133;
      energy /= fNonLinearityParams[0]*(1-fNonLinearityParams[1]*exp(-fNonLinearityParams[2]*energy));
      
      break;
      
    case kNoCorrection:
      AliDebug(2,"No correction on the energy\n");
      break;
      
  }
  
  return energy;

}

//__________________________________________________
Float_t  AliEMCALRecoUtils::GetDepth(const Float_t energy, const Int_t iParticle, const Int_t iSM) const 
{
  //Calculate shower depth for a given cluster energy and particle type

  // parameters 
  Float_t x0    = 1.31;
  Float_t ecr   = 8;
  Float_t depth = 0;
  
  switch ( iParticle )
  {
    case kPhoton:
      depth = x0 * (TMath::Log(energy*1000/ ecr) + 0.5); //Multiply energy by 1000 to transform to MeV
      break;
      
    case kElectron:
      depth = x0 * (TMath::Log(energy*1000/ ecr) - 0.5); //Multiply energy by 1000 to transform to MeV
      break;
      
    case kHadron:
      // hadron 
      // boxes anc. here
      if(gGeoManager){
        gGeoManager->cd("ALIC_1/XEN1_1");
        TGeoNode        *geoXEn1    = gGeoManager->GetCurrentNode();
        TGeoNodeMatrix  *geoSM      = dynamic_cast<TGeoNodeMatrix *>(geoXEn1->GetDaughter(iSM));
        if(geoSM){
          TGeoVolume      *geoSMVol   = geoSM->GetVolume(); 
          TGeoShape       *geoSMShape = geoSMVol->GetShape();
          TGeoBBox        *geoBox     = dynamic_cast<TGeoBBox *>(geoSMShape);
          if(geoBox) depth = 0.5 * geoBox->GetDX()*2 ;
          else AliFatal("Null GEANT box");
        }else AliFatal("NULL  GEANT node matrix");
      }
      else{//electron
        depth = x0 * (TMath::Log(energy*1000 / ecr)  - 0.5); //Multiply energy by 1000 to transform to MeV
      }
        
      break;
      
    default://photon
      depth = x0 * (TMath::Log(energy*1000 / ecr) + 0.5); //Multiply energy by 1000 to transform to MeV
  }  
  
  return depth;
  
}

//__________________________________________________
void AliEMCALRecoUtils::GetMaxEnergyCell(AliEMCALGeometry *geom, AliVCaloCells* cells, AliVCluster* clu, 
                                         Int_t & absId,  Int_t& iSupMod, Int_t& ieta, Int_t& iphi, Bool_t &shared)
{
  //For a given CaloCluster gets the absId of the cell 
  //with maximum energy deposit.
  
  Double_t eMax        = -1.;
  Double_t eCell       = -1.;
  Float_t  fraction    = 1.;
  Float_t  recalFactor = 1.;
  Int_t    cellAbsId   = -1 ;

  Int_t iTower  = -1;
  Int_t iIphi   = -1;
  Int_t iIeta   = -1;
  Int_t iSupMod0= -1;
	//printf("---Max?\n");
  for (Int_t iDig=0; iDig< clu->GetNCells(); iDig++) {
    cellAbsId = clu->GetCellAbsId(iDig);
    fraction  = clu->GetCellAmplitudeFraction(iDig);
    //printf("a Cell %d, id, %d, amp %f, fraction %f\n",iDig,cellAbsId,cells->GetCellAmplitude(cellAbsId),fraction);
    if(fraction < 1e-4) fraction = 1.; // in case unfolding is off
    geom->GetCellIndex(cellAbsId,iSupMod,iTower,iIphi,iIeta); 
    geom->GetCellPhiEtaIndexInSModule(iSupMod,iTower,iIphi, iIeta,iphi,ieta);
    if(iDig==0) iSupMod0=iSupMod;
    else if(iSupMod0!=iSupMod) {
      shared = kTRUE;
      //printf("AliEMCALRecoUtils::GetMaxEnergyCell() - SHARED CLUSTER\n");
    }
    if(IsRecalibrationOn()) {
      recalFactor = GetEMCALChannelRecalibrationFactor(iSupMod,ieta,iphi);
    }
    eCell  = cells->GetCellAmplitude(cellAbsId)*fraction*recalFactor;
    //printf("b Cell %d, id, %d, amp %f, fraction %f\n",iDig,cellAbsId,eCell,fraction);
    if(eCell > eMax)  { 
      eMax  = eCell; 
      absId = cellAbsId;
      //printf("\t new max: cell %d, e %f, ecell %f\n",maxId, eMax,eCell);
    }
  }// cell loop
  
  //Get from the absid the supermodule, tower and eta/phi numbers
  geom->GetCellIndex(absId,iSupMod,iTower,iIphi,iIeta); 
  //Gives SuperModule and Tower numbers
  geom->GetCellPhiEtaIndexInSModule(iSupMod,iTower,
                                         iIphi, iIeta,iphi,ieta); 
  //printf("Max id %d, iSM %d, col %d, row %d\n",absId,iSupMod,ieta,iphi);
  //printf("Max end---\n");
  
}

//________________________________________________________________
void AliEMCALRecoUtils::InitEMCALRecalibrationFactors(){
	//Init EMCAL recalibration factors
	AliDebug(2,"AliCalorimeterUtils::InitEMCALRecalibrationFactors()");
	//In order to avoid rewriting the same histograms
	Bool_t oldStatus = TH1::AddDirectoryStatus();
	TH1::AddDirectory(kFALSE);
  
	fEMCALRecalibrationFactors = new TObjArray(10);
	for (int i = 0; i < 12; i++) fEMCALRecalibrationFactors->Add(new TH2F(Form("EMCALRecalFactors_SM%d",i),Form("EMCALRecalFactors_SM%d",i),  48, 0, 48, 24, 0, 24));
	//Init the histograms with 1
	for (Int_t sm = 0; sm < 12; sm++) {
		for (Int_t i = 0; i < 48; i++) {
			for (Int_t j = 0; j < 24; j++) {
				SetEMCALChannelRecalibrationFactor(sm,i,j,1.);
			}
		}
	}
	fEMCALRecalibrationFactors->SetOwner(kTRUE);
	fEMCALRecalibrationFactors->Compress();
	
	//In order to avoid rewriting the same histograms
	TH1::AddDirectory(oldStatus);		
}


//________________________________________________________________
void AliEMCALRecoUtils::InitEMCALBadChannelStatusMap(){
	//Init EMCAL bad channels map
	AliDebug(2,"AliEMCALRecoUtils::InitEMCALBadChannelStatusMap()");
	//In order to avoid rewriting the same histograms
	Bool_t oldStatus = TH1::AddDirectoryStatus();
	TH1::AddDirectory(kFALSE);
	
	fEMCALBadChannelMap = new TObjArray(10);
	//TH2F * hTemp = new  TH2I("EMCALBadChannelMap","EMCAL SuperModule bad channel map", 48, 0, 48, 24, 0, 24);
	for (int i = 0; i < 10; i++) {
		fEMCALBadChannelMap->Add(new TH2I(Form("EMCALBadChannelMap_Mod%d",i),Form("EMCALBadChannelMap_Mod%d",i), 48, 0, 48, 24, 0, 24));
	}
	
	//delete hTemp;
	
	fEMCALBadChannelMap->SetOwner(kTRUE);
	fEMCALBadChannelMap->Compress();
	
	//In order to avoid rewriting the same histograms
	TH1::AddDirectory(oldStatus);		
}

//________________________________________________________________
void AliEMCALRecoUtils::RecalibrateClusterEnergy(AliEMCALGeometry* geom, AliVCluster * cluster, AliVCaloCells * cells){
	// Recalibrate the cluster energy, considering the recalibration map and the energy of the cells that compose the cluster.
	
	//Get the cluster number of cells and list of absId, check what kind of cluster do we have.
	UShort_t * index    = cluster->GetCellsAbsId() ;
	Double_t * fraction = cluster->GetCellsAmplitudeFraction() ;
	Int_t ncells = cluster->GetNCells();
	
	//Initialize some used variables
	Float_t energy = 0;
	Int_t absId    = -1;
  Int_t icol = -1, irow = -1, imod=1;
	Float_t factor = 1, frac = 0;
	
	//Loop on the cells, get the cell amplitude and recalibration factor, multiply and and to the new energy
	for(Int_t icell = 0; icell < ncells; icell++){
		absId = index[icell];
		frac =  fraction[icell];
		if(frac < 1e-5) frac = 1; //in case of EMCAL, this is set as 0 since unfolding is off
		Int_t iTower = -1, iIphi = -1, iIeta = -1; 
		geom->GetCellIndex(absId,imod,iTower,iIphi,iIeta); 
		if(fEMCALRecalibrationFactors->GetEntries() <= imod) continue;
		geom->GetCellPhiEtaIndexInSModule(imod,iTower,iIphi, iIeta,irow,icol);			
		factor = GetEMCALChannelRecalibrationFactor(imod,icol,irow);
    AliDebug(2,Form("AliEMCALRecoUtils::RecalibrateClusterEnergy - recalibrate cell: module %d, col %d, row %d, cell fraction %f,recalibration factor %f, cell energy %f\n",
             imod,icol,irow,frac,factor,cells->GetCellAmplitude(absId)));
		
		energy += cells->GetCellAmplitude(absId)*factor*frac;
	}
	
	
		AliDebug(2,Form("AliEMCALRecoUtils::RecalibrateClusterEnergy - Energy before %f, after %f\n",cluster->E(),energy));
	
	cluster->SetE(energy);
	
}


//__________________________________________________
void AliEMCALRecoUtils::RecalculateClusterPosition(AliEMCALGeometry *geom, AliVCaloCells* cells, AliVCluster* clu)
{
  //For a given CaloCluster recalculates the position for a given set of misalignment shifts and puts it again in the CaloCluster.
  
  if     (fPosAlgo==kPosTowerGlobal) RecalculateClusterPositionFromTowerGlobal( geom, cells, clu);
  else if(fPosAlgo==kPosTowerIndex)  RecalculateClusterPositionFromTowerIndex ( geom, cells, clu);
  else   AliDebug(2,"Algorithm to recalculate position not selected, do nothing.");
  
}  

//__________________________________________________
void AliEMCALRecoUtils::RecalculateClusterPositionFromTowerGlobal(AliEMCALGeometry *geom, AliVCaloCells* cells, AliVCluster* clu)
{
  // For a given CaloCluster recalculates the position for a given set of misalignment shifts and puts it again in the CaloCluster.
  // The algorithm is a copy of what is done in AliEMCALRecPoint
  
  Double_t eCell       = 0.;
  Float_t  fraction    = 1.;
  Float_t  recalFactor = 1.;
  
  Int_t    absId   = -1;
  Int_t    iTower  = -1, iIphi  = -1, iIeta  = -1;
  Int_t    iSupModMax = -1, iSM=-1, iphi   = -1, ieta   = -1;
  Float_t  weight = 0.,  totalWeight=0.;
  Float_t  newPos[3] = {0,0,0};
  Double_t pLocal[3], pGlobal[3];
  Bool_t shared = kFALSE;

  Float_t  clEnergy = clu->E(); //Energy already recalibrated previously
  GetMaxEnergyCell(geom, cells, clu, absId,  iSupModMax, ieta, iphi,shared);
  Double_t depth = GetDepth(clEnergy,fParticleType,iSupModMax) ;
  
  //printf("** Cluster energy %f, ncells %d, depth %f\n",clEnergy,clu->GetNCells(),depth);
  
  for (Int_t iDig=0; iDig< clu->GetNCells(); iDig++) {
    absId = clu->GetCellAbsId(iDig);
    fraction  = clu->GetCellAmplitudeFraction(iDig);
    if(fraction < 1e-4) fraction = 1.; // in case unfolding is off
    geom->GetCellIndex(absId,iSM,iTower,iIphi,iIeta); 
    geom->GetCellPhiEtaIndexInSModule(iSM,iTower,iIphi, iIeta,iphi,ieta);			
    
    if(IsRecalibrationOn()) {
      recalFactor = GetEMCALChannelRecalibrationFactor(iSM,ieta,iphi);
    }
    eCell  = cells->GetCellAmplitude(absId)*fraction*recalFactor;
    
    weight = GetCellWeight(eCell,clEnergy);
    //printf("cell energy %f, weight %f\n",eCell,weight);
    totalWeight += weight;
    geom->RelPosCellInSModule(absId,depth,pLocal[0],pLocal[1],pLocal[2]);
    //printf("pLocal (%f,%f,%f), SM %d, absId %d\n",pLocal[0],pLocal[1],pLocal[2],iSupModMax,absId);
    geom->GetGlobal(pLocal,pGlobal,iSupModMax);
    //printf("pLocal (%f,%f,%f)\n",pGlobal[0],pGlobal[1],pGlobal[2]);

    for(int i=0; i<3; i++ ) newPos[i] += (weight*pGlobal[i]);
    
  }// cell loop
  
  if(totalWeight>0){
    for(int i=0; i<3; i++ )    newPos[i] /= totalWeight;
  }
    
  //Float_t pos[]={0,0,0};
  //clu->GetPosition(pos);
  //printf("OldPos  : %2.3f,%2.3f,%2.3f\n",pos[0],pos[1],pos[2]);
  //printf("NewPos  : %2.3f,%2.3f,%2.3f\n",newPos[0],newPos[1],newPos[2]);
  
	if(iSupModMax > 1) {//sector 1
	  newPos[0] +=fMisalTransShift[3];//-=3.093; 
	  newPos[1] +=fMisalTransShift[4];//+=6.82;
	  newPos[2] +=fMisalTransShift[5];//+=1.635;
    //printf("   +    : %2.3f,%2.3f,%2.3f\n",fMisalTransShift[3],fMisalTransShift[4],fMisalTransShift[5]);

	}
	else {//sector 0
	  newPos[0] +=fMisalTransShift[0];//+=1.134;
	  newPos[1] +=fMisalTransShift[1];//+=8.2;
	  newPos[2] +=fMisalTransShift[2];//+=1.197;
    //printf("   +    : %2.3f,%2.3f,%2.3f\n",fMisalTransShift[0],fMisalTransShift[1],fMisalTransShift[2]);

	}
  //printf("NewPos : %2.3f,%2.3f,%2.3f\n",newPos[0],newPos[1],newPos[2]);

  clu->SetPosition(newPos);
  
}  

//__________________________________________________
void AliEMCALRecoUtils::RecalculateClusterPositionFromTowerIndex(AliEMCALGeometry *geom, AliVCaloCells* cells, AliVCluster* clu)
{
  // For a given CaloCluster recalculates the position for a given set of misalignment shifts and puts it again in the CaloCluster.
  // The algorithm works with the tower indeces, averages the indeces and from them it calculates the global position
  
  Double_t eCell       = 1.;
  Float_t  fraction    = 1.;
  Float_t  recalFactor = 1.;
  
  Int_t absId   = -1;
  Int_t iTower  = -1;
  Int_t iIphi   = -1, iIeta   = -1;
	Int_t iSupMod = -1, iSupModMax = -1;
  Int_t iphi = -1, ieta =-1;
  Bool_t shared = kFALSE;

  Float_t clEnergy = clu->E(); //Energy already recalibrated previously.
  GetMaxEnergyCell(geom, cells, clu, absId,  iSupModMax, ieta, iphi,shared);
  Float_t  depth = GetDepth(clEnergy,fParticleType,iSupMod) ;

  Float_t weight = 0., weightedCol = 0., weightedRow = 0., totalWeight=0.;
  Bool_t areInSameSM = kTRUE; //exclude clusters with cells in different SMs for now
  Int_t startingSM = -1;
  
  for (Int_t iDig=0; iDig< clu->GetNCells(); iDig++) {
    absId = clu->GetCellAbsId(iDig);
    fraction  = clu->GetCellAmplitudeFraction(iDig);
    if(fraction < 1e-4) fraction = 1.; // in case unfolding is off
    geom->GetCellIndex(absId,iSupMod,iTower,iIphi,iIeta); 
    geom->GetCellPhiEtaIndexInSModule(iSupMod,iTower,iIphi, iIeta,iphi,ieta);			
    
    if     (iDig==0)  startingSM = iSupMod;
    else if(iSupMod != startingSM) areInSameSM = kFALSE;

    eCell  = cells->GetCellAmplitude(absId);
    
    if(IsRecalibrationOn()) {
      recalFactor = GetEMCALChannelRecalibrationFactor(iSupMod,ieta,iphi);
    }
    eCell  = cells->GetCellAmplitude(absId)*fraction*recalFactor;
    
    weight = GetCellWeight(eCell,clEnergy);
    if(weight < 0) weight = 0;
    totalWeight += weight;
    weightedCol += ieta*weight;
    weightedRow += iphi*weight;
    
    //printf("Max cell? cell %d, amplitude org %f, fraction %f, recalibration %f, amplitude new %f \n",cellAbsId, cells->GetCellAmplitude(cellAbsId), fraction, recalFactor, eCell) ;
    
    }// cell loop
    
  Float_t xyzNew[]={0.,0.,0.};
  if(areInSameSM == kTRUE) {
    //printf("In Same SM\n");
    weightedCol = weightedCol/totalWeight;
    weightedRow = weightedRow/totalWeight;
    geom->RecalculateTowerPosition(weightedRow, weightedCol, iSupModMax, depth, fMisalTransShift, fMisalRotShift, xyzNew); 
  }
  else {
    //printf("In Different SM\n");
    geom->RecalculateTowerPosition(iphi,        ieta,        iSupModMax, depth, fMisalTransShift, fMisalRotShift, xyzNew); 
  }
  
  clu->SetPosition(xyzNew);
  
}

//____________________________________________________________________________
void AliEMCALRecoUtils::RecalculateClusterDistanceToBadChannel(AliEMCALGeometry * geom, AliVCaloCells* cells, AliVCluster * cluster){           
	
  //re-evaluate distance to bad channel with updated bad map
  
  if(!fRecalDistToBadChannels) return;
  
	//Get channels map of the supermodule where the cluster is.
  Int_t absIdMax	= -1, iSupMod =-1, icolM = -1, irowM = -1;
  Bool_t shared = kFALSE;
  GetMaxEnergyCell(geom, cells, cluster, absIdMax,  iSupMod, icolM, irowM, shared);
  TH2D* hMap  = (TH2D*)fEMCALBadChannelMap->At(iSupMod);

  Int_t dRrow, dRcol;	
	Float_t  minDist = 10000.;
	Float_t  dist    = 0.;
  
  //Loop on tower status map 
	for(Int_t irow = 0; irow < AliEMCALGeoParams::fgkEMCALRows; irow++){
		for(Int_t icol = 0; icol < AliEMCALGeoParams::fgkEMCALCols; icol++){
			//Check if tower is bad.
			if(hMap->GetBinContent(icol,irow)==0) continue;
      //printf("AliEMCALRecoUtils::RecalculateDistanceToBadChannels() - \n \t Bad channel in SM %d, col %d, row %d, \n \t Cluster max in col %d, row %d\n",
      //       iSupMod,icol, irow, icolM,irowM);
      
      dRrow=TMath::Abs(irowM-irow);
      dRcol=TMath::Abs(icolM-icol);
      dist=TMath::Sqrt(dRrow*dRrow+dRcol*dRcol);
			if(dist < minDist){
        //printf("MIN DISTANCE TO BAD %2.2f\n",dist);
        minDist = dist;
      }
      
		}
	}
  
	//In case the cluster is shared by 2 SuperModules, need to check the map of the second Super Module
	if (shared) {
		TH2D* hMap2 = 0;
		Int_t iSupMod2 = -1;
    
		//The only possible combinations are (0,1), (2,3) ... (8,9)
		if(iSupMod%2) iSupMod2 = iSupMod-1;
		else          iSupMod2 = iSupMod+1;
		hMap2  = (TH2D*)fEMCALBadChannelMap->At(iSupMod2);
    
		//Loop on tower status map of second super module
		for(Int_t irow = 0; irow < AliEMCALGeoParams::fgkEMCALRows; irow++){
			for(Int_t icol = 0; icol < AliEMCALGeoParams::fgkEMCALCols; icol++){
				//Check if tower is bad.
				if(hMap2->GetBinContent(icol,irow)==0) continue;
				//printf("AliEMCALRecoUtils::RecalculateDistanceToBadChannels(shared) - \n \t Bad channel in SM %d, col %d, row %d \n \t Cluster max in SM %d, col %d, row %d\n",
          //     iSupMod2,icol, irow,iSupMod,icolM,irowM);
        
        dRrow=TMath::Abs(irow-irowM);
        
        if(iSupMod%2) {
				  dRcol=TMath::Abs(icol-(AliEMCALGeoParams::fgkEMCALCols+icolM));
				}
        else {
          dRcol=TMath::Abs(AliEMCALGeoParams::fgkEMCALCols+icol-icolM);
				}                    
        
				dist=TMath::Sqrt(dRrow*dRrow+dRcol*dRcol);
        if(dist < minDist) minDist = dist;        
        
			}
		}
    
	}// shared cluster in 2 SuperModules
  
  AliDebug(2,Form("Max cluster cell (SM,col,row)=(%d %d %d) - Distance to Bad Channel %2.2f",iSupMod, icolM, irowM, minDist));
  cluster->SetDistanceToBadChannel(minDist);
  
}

//____________________________________________________________________________
void AliEMCALRecoUtils::RecalculateClusterPID(AliVCluster * cluster){           
	
  //re-evaluate identification parameters with bayesian

	if ( cluster->GetM02() != 0)
    fPIDUtils->ComputePID(cluster->E(),cluster->GetM02());
  
  Float_t pidlist[AliPID::kSPECIESN+1];
	for(Int_t i = 0; i < AliPID::kSPECIESN+1; i++) pidlist[i] = fPIDUtils->GetPIDFinal(i);
  
  cluster->SetPID(pidlist);
	
}

//____________________________________________________________________________
void AliEMCALRecoUtils::RecalculateClusterShowerShapeParameters(AliEMCALGeometry * geom, AliVCaloCells* cells, AliVCluster * cluster)
{
  // Calculates new center of gravity in the local EMCAL-module coordinates 
  // and tranfers into global ALICE coordinates
  // Calculates Dispersion and main axis
  
  Int_t nstat  = 0;
  Float_t wtot = 0. ;
  Double_t eCell       = 0.;
  Float_t  fraction    = 1.;
  Float_t  recalFactor = 1.;

  Int_t iSupMod = -1;
  Int_t iTower  = -1;
  Int_t iIphi   = -1;
  Int_t iIeta   = -1;
  Int_t iphi    = -1;
  Int_t ieta    = -1;
  Double_t etai = -1.;
  Double_t phii = -1.;
  
  Double_t w     = 0.;
  Double_t d     = 0.;
  Double_t dxx   = 0.;
  Double_t dzz   = 0.;
  Double_t dxz   = 0.;  
  Double_t xmean = 0.;
  Double_t zmean = 0.;
    
  //Loop on cells
  for(Int_t iDigit=0; iDigit < cluster->GetNCells(); iDigit++) {
    
    //Get from the absid the supermodule, tower and eta/phi numbers
    geom->GetCellIndex(cluster->GetCellAbsId(iDigit),iSupMod,iTower,iIphi,iIeta); 
    geom->GetCellPhiEtaIndexInSModule(iSupMod,iTower,iIphi,iIeta, iphi,ieta);        
    
    //Get the cell energy, if recalibration is on, apply factors
    fraction  = cluster->GetCellAmplitudeFraction(iDigit);
    if(fraction < 1e-4) fraction = 1.; // in case unfolding is off
    if(IsRecalibrationOn()) {
      recalFactor = GetEMCALChannelRecalibrationFactor(iSupMod,ieta,iphi);
    }
    eCell  = cells->GetCellAmplitude(cluster->GetCellAbsId(iDigit))*fraction*recalFactor;
    
    if(cluster->E() > 0 && eCell > 0){
      
      w  = GetCellWeight(eCell,cluster->E());
      
      etai=(Double_t)ieta;
      phii=(Double_t)iphi;		
      if(w > 0.0) {
        wtot += w ;
        nstat++;		        
        //Shower shape
        dxx  += w * etai * etai ;
        xmean+= w * etai ;
        dzz  += w * phii * phii ;
        zmean+= w * phii ; 
        dxz  += w * etai * phii ; 
      }
    }
    else
      AliError(Form("Wrong energy %f and/or amplitude %f\n", eCell, cluster->E()));
  }//cell loop
  
  //Normalize to the weight	
  if (wtot > 0) {
    xmean /= wtot ;
    zmean /= wtot ;
  }
  else
    AliError(Form("Wrong weight %f\n", wtot));
  
  //Calculate dispersion	
  for(Int_t iDigit=0; iDigit < cluster->GetNCells(); iDigit++) {
    
    //Get from the absid the supermodule, tower and eta/phi numbers
    geom->GetCellIndex(cluster->GetCellAbsId(iDigit),iSupMod,iTower,iIphi,iIeta); 
    geom->GetCellPhiEtaIndexInSModule(iSupMod,iTower,iIphi,iIeta, iphi,ieta);
    
    //Get the cell energy, if recalibration is on, apply factors
    fraction  = cluster->GetCellAmplitudeFraction(iDigit);
    if(fraction < 1e-4) fraction = 1.; // in case unfolding is off
    if(IsRecalibrationOn()) {
      recalFactor = GetEMCALChannelRecalibrationFactor(iSupMod,ieta,iphi);
    }
    eCell  = cells->GetCellAmplitude(cluster->GetCellAbsId(iDigit))*fraction*recalFactor;
    
    if(cluster->E() > 0 && eCell > 0){
      
      w  = GetCellWeight(eCell,cluster->E());
      
      etai=(Double_t)ieta;
      phii=(Double_t)iphi;		
      if(w > 0.0)  d +=  w*((etai-xmean)*(etai-xmean)+(phii-zmean)*(phii-zmean)); 
    }
    else
      AliError(Form("Wrong energy %f and/or amplitude %f\n", eCell, cluster->E()));
  }// cell loop
  
  //Normalize to the weigth and set shower shape parameters
  if (wtot > 0 && nstat > 1) {
    d /= wtot ;
    dxx /= wtot ;
    dzz /= wtot ;
    dxz /= wtot ;
    dxx -= xmean * xmean ;
    dzz -= zmean * zmean ;
    dxz -= xmean * zmean ;
    cluster->SetM02(0.5 * (dxx + dzz) + TMath::Sqrt( 0.25 * (dxx - dzz) * (dxx - dzz) + dxz * dxz ));
    cluster->SetM20(0.5 * (dxx + dzz) - TMath::Sqrt( 0.25 * (dxx - dzz) * (dxx - dzz) + dxz * dxz ));
  }
  else{
    d=0. ;
    cluster->SetM20(0.) ;
    cluster->SetM02(0.) ;
  }	
  
  if (d>=0)
    cluster->SetDispersion(TMath::Sqrt(d)) ;
  else    
    cluster->SetDispersion(0) ;
  
}

//__________________________________________________
void AliEMCALRecoUtils::FindMatches(AliVEvent *event)
{
  //This function should be called before the cluster loop
  //Before call this function, please recalculate the cluster positions
  //Given the input event, loop over all the tracks, select the closest cluster as matched with fCutR
  //Store matched cluster indexes and residuals
  //It only works with ESDs, not AODs
  
  fMatchedClusterIndex->Reset();
  fResidualZ->Reset();
  fResidualR->Reset();
  
  fMatchedClusterIndex->Set(100);
  fResidualZ->Set(100);
  fResidualR->Set(100);
  
  Int_t    matched=0;
  Float_t  clsPos[3];
  Double_t trkPos[3];
  for(Int_t itr=0; itr<event->GetNumberOfTracks(); itr++)
  {
    AliESDtrack *track = ((AliESDEvent*)event)->GetTrack(itr);
    if(!track || !IsAccepted(track)) continue;
    
    Float_t dRMax = fCutR, dZMax = fCutZ;
    Int_t index = -1;
    AliEMCALTrack *emctrack = new AliEMCALTrack(*track);
    for(Int_t icl=0; icl<event->GetNumberOfCaloClusters(); icl++)
    {
      AliVCluster *cluster = (AliVCluster*) event->GetCaloCluster(icl);
      if(!cluster->IsEMCAL()) continue;
      cluster->GetPosition(clsPos); //Has been recalculated
      if(!emctrack->PropagateToGlobal(clsPos[0],clsPos[1],clsPos[2],0.,0.) )  continue;
      emctrack->GetXYZ(trkPos);
      Float_t tmpR = TMath::Sqrt( TMath::Power(clsPos[0]-trkPos[0],2)+TMath::Power(clsPos[1]-trkPos[1],2)+TMath::Power(clsPos[2]-trkPos[2],2) );
      Float_t tmpZ = TMath::Abs(clsPos[2]-trkPos[2]);
      
      if(tmpR<dRMax)
	    {
	      dRMax=tmpR;
	      dZMax=tmpZ;
	      index=icl;
	    }
      
    }//cluser loop
    
    if(index>-1)
    {
      fMatchedClusterIndex->AddAt(index,matched);
      fResidualZ->AddAt(dZMax,matched);
      fResidualR->AddAt(dRMax,matched);
      matched++;
    }
    delete emctrack;
  }//track loop
  fMatchedClusterIndex->Set(matched);
  fResidualZ->Set(matched);
  fResidualR->Set(matched);
  
  //printf("Number of matched pairs: %d\n",matched);
}

//__________________________________________________
void AliEMCALRecoUtils::GetMatchedResiduals(Int_t index, Float_t &dR, Float_t &dZ)
{
  //Given a cluster index as in AliESDEvent::GetCaloCluster(index)
  //Get the residuals dR and dZ for this cluster
  //It only works with ESDs, not AODs

  if( FindMatchedPos(index) >= 999 )
  {
    AliDebug(2,"No matched tracks found!\n");
    dR=999.;
    dZ=999.;
    return;
  }
  dR = fResidualR->At(FindMatchedPos(index));
  dZ = fResidualZ->At(FindMatchedPos(index));
}

//__________________________________________________
Bool_t AliEMCALRecoUtils::IsMatched(Int_t index)
{
  //Given a cluster index as in AliESDEvent::GetCaloCluster(index)
  //Returns if cluster has a match
  if(FindMatchedPos(index) < 999) 
    return kTRUE;
  else
    return kFALSE;
}
//__________________________________________________
UInt_t AliEMCALRecoUtils::FindMatchedPos(Int_t index) const
{
  //Given a cluster index as in AliESDEvent::GetCaloCluster(index)
  //Returns the position of the match in the fMatchedClusterIndex array
  Float_t tmpR = fCutR;
  UInt_t pos = 999;

  for(Int_t i=0; i<fMatchedClusterIndex->GetSize(); i++)
    {
      if(fMatchedClusterIndex->At(i)==index && fResidualR->At(i)<tmpR)
	{
	  pos=i;
	  tmpR=fResidualR->At(i);
	}
      AliDebug(3,Form("Matched cluster pos: %d, index: %d, dR: %2.4f, dZ: %2.4f.\n",i,fMatchedClusterIndex->At(i),fResidualR->At(i),fResidualZ->At(i)));
    }
  return pos;
}

Bool_t AliEMCALRecoUtils::IsAccepted(AliESDtrack *esdTrack)
{
  // Given a esd track, return whether the track survive all the cuts

  // The different quality parameter are first
  // retrieved from the track. then it is found out what cuts the
  // track did not survive and finally the cuts are imposed.

  UInt_t status = esdTrack->GetStatus();

  Int_t nClustersITS = esdTrack->GetITSclusters(0);
  Int_t nClustersTPC = esdTrack->GetTPCclusters(0);

  Float_t chi2PerClusterITS = -1;
  Float_t chi2PerClusterTPC = -1;
  if (nClustersITS!=0)
    chi2PerClusterITS = esdTrack->GetITSchi2()/Float_t(nClustersITS);
  if (nClustersTPC!=0) 
    chi2PerClusterTPC = esdTrack->GetTPCchi2()/Float_t(nClustersTPC);


  //DCA cuts
  Float_t MaxDCAToVertexXYPtDep = 0.0182 + 0.0350/TMath::Power(esdTrack->Pt(),1.01); //This expression comes from AliESDtrackCuts::GetStandardITSTPCTrackCuts2010()
  //AliDebug(3,Form("Track pT = %f, DCAtoVertexXY = %f",esdTrack->Pt(),MaxDCAToVertexXYPtDep));
  SetMaxDCAToVertexXY(MaxDCAToVertexXYPtDep); //Set pT dependent DCA cut to vertex in x-y plane


  Float_t b[2];
  Float_t bCov[3];
  esdTrack->GetImpactParameters(b,bCov);
  if (bCov[0]<=0 || bCov[2]<=0) {
    AliDebug(1, "Estimated b resolution lower or equal zero!");
    bCov[0]=0; bCov[2]=0;
  }

  Float_t dcaToVertexXY = b[0];
  Float_t dcaToVertexZ = b[1];
  Float_t dcaToVertex = -1;

  if (fCutDCAToVertex2D)
    dcaToVertex = TMath::Sqrt(dcaToVertexXY*dcaToVertexXY/fCutMaxDCAToVertexXY/fCutMaxDCAToVertexXY + dcaToVertexZ*dcaToVertexZ/fCutMaxDCAToVertexZ/fCutMaxDCAToVertexZ);
  else
    dcaToVertex = TMath::Sqrt(dcaToVertexXY*dcaToVertexXY + dcaToVertexZ*dcaToVertexZ);
    
  // cut the track?
  
  Bool_t cuts[kNCuts];
  for (Int_t i=0; i<kNCuts; i++) cuts[i]=kFALSE;
  
  // track quality cuts
  if (fCutRequireTPCRefit && (status&AliESDtrack::kTPCrefit)==0)
    cuts[0]=kTRUE;
  if (fCutRequireITSRefit && (status&AliESDtrack::kITSrefit)==0)
    cuts[1]=kTRUE;
  if (nClustersTPC<fCutMinNClusterTPC)
    cuts[2]=kTRUE;
  if (nClustersITS<fCutMinNClusterITS) 
    cuts[3]=kTRUE;
  if (chi2PerClusterTPC>fCutMaxChi2PerClusterTPC) 
    cuts[4]=kTRUE; 
  if (chi2PerClusterITS>fCutMaxChi2PerClusterITS) 
    cuts[5]=kTRUE;  
  if (!fCutAcceptKinkDaughters && esdTrack->GetKinkIndex(0)>0)
    cuts[6]=kTRUE;
  if (fCutDCAToVertex2D && dcaToVertex > 1)
    cuts[7] = kTRUE;
  if (!fCutDCAToVertex2D && TMath::Abs(dcaToVertexXY) > fCutMaxDCAToVertexXY)
    cuts[8] = kTRUE;
  if (!fCutDCAToVertex2D && TMath::Abs(dcaToVertexZ) > fCutMaxDCAToVertexZ)
    cuts[9] = kTRUE;

  //Require at least one SPD point + anything else in ITS
  if( (esdTrack->HasPointOnITSLayer(0) || esdTrack->HasPointOnITSLayer(1)) == kFALSE)
    cuts[10] = kTRUE;

  Bool_t cut=kFALSE;
  for (Int_t i=0; i<kNCuts; i++) 
    if (cuts[i]) {cut = kTRUE;}

    // cut the track
  if (cut) 
    return kFALSE;
  else 
    return kTRUE;
}
//__________________________________________________
void AliEMCALRecoUtils::InitTrackCuts()
{
  //Intilize the track cut criteria
  //By default these cuts are set according to AliESDtrackCuts::GetStandardITSTPCTrackCuts2010()
  //Also you can customize the cuts using the setters
  
  //TPC
  SetMinNClustersTPC(70);
  SetMaxChi2PerClusterTPC(4);
  SetAcceptKinkDaughters(kFALSE);
  SetRequireTPCRefit(kTRUE);
  
  //ITS
  SetRequireITSRefit(kTRUE);
  SetMaxDCAToVertexZ(2);
  SetDCAToVertex2D(kFALSE);
}

//__________________________________________________
void AliEMCALRecoUtils::Print(const Option_t *) const 
{
  // Print Parameters
  
  printf("AliEMCALRecoUtils Settings: \n");
  printf("Misalignment shifts\n");
  for(Int_t i=0; i<5; i++) printf("\t sector %d, traslation (x,y,z)=(%f,%f,%f), rotation (x,y,z)=(%f,%f,%f)\n",i, 
                                  fMisalTransShift[i*3],fMisalTransShift[i*3+1],fMisalTransShift[i*3+2],
                                  fMisalRotShift[i*3],  fMisalRotShift[i*3+1],  fMisalRotShift[i*3+2]   );
  printf("Non linearity function %d, parameters:\n", fNonLinearityFunction);
  for(Int_t i=0; i<6; i++) printf("param[%d]=%f\n",i, fNonLinearityParams[i]);
  
  printf("Position Recalculation option %d, Particle Type %d, fW0 %2.2f, Recalibrate Data %d \n",fPosAlgo,fParticleType,fW0, fRecalibration);

  printf("Matching criteria: dR < %2.2f[cm], dZ < %2.2f[cm]\n",fCutR,fCutZ);

  printf("Track cuts: \n");
  printf("TPCRefit = %d, ITSRefit = %d\n",fCutRequireTPCRefit,fCutRequireITSRefit);
  printf("AcceptKinks = %d\n",fCutAcceptKinkDaughters);
  printf("MinNCulsterTPC = %d, MinNClusterITS = %d\n",fCutMinNClusterTPC,fCutMinNClusterITS);
  printf("MaxChi2TPC = %2.2f, MaxChi2ITS = %2.2f\n",fCutMaxChi2PerClusterTPC,fCutMaxChi2PerClusterITS);
  printf("DCSToVertex2D = %d, MaxDCAToVertexXY = %2.2f, MaxDCAToVertexZ = %2.2f\n",fCutDCAToVertex2D,fCutMaxDCAToVertexXY,fCutMaxDCAToVertexZ);

    
}
