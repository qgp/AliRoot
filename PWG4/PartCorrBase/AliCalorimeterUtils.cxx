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
/* $Id:  $ */

//_________________________________________________________________________
// Class utility for Calorimeter specific selection methods                ///
//
//
//
//-- Author: Gustavo Conesa (LPSC-Grenoble) 
//////////////////////////////////////////////////////////////////////////////


// --- ROOT system ---
#include "TGeoManager.h"

//---- ANALYSIS system ----
#include "AliCalorimeterUtils.h"
#include "AliVEvent.h"
#include "AliMCEvent.h"
#include "AliStack.h"
#include "AliAODPWG4Particle.h"
#include "AliVCluster.h"
#include "AliVCaloCells.h"
#include "AliMixedEvent.h"


ClassImp(AliCalorimeterUtils)
  
  
//____________________________________________________________________________
  AliCalorimeterUtils::AliCalorimeterUtils() : 
    TObject(), fDebug(0), 
    fEMCALGeoName("EMCAL_FIRSTYEAR"),fPHOSGeoName("PHOSgeo"), 
    fEMCALGeo(0x0), fPHOSGeo(0x0), 
    fEMCALGeoMatrixSet(kFALSE), fPHOSGeoMatrixSet(kFALSE), 
    fRemoveBadChannels(kFALSE),
    fEMCALBadChannelMap(0x0),fPHOSBadChannelMap(0x0), 
    fNCellsFromEMCALBorder(0), fNCellsFromPHOSBorder(0), 
    fNoEMCALBorderAtEta0(kFALSE),fRecalibration(kFALSE),
    fEMCALRecalibrationFactors(), fPHOSRecalibrationFactors()
{
  //Ctor
  
  //Initialize parameters
  InitParameters();
}

//_________________________________
AliCalorimeterUtils::~AliCalorimeterUtils() {
  //Dtor
  
  //if(fPHOSGeo)  delete fPHOSGeo  ;
  if(fEMCALGeo) delete fEMCALGeo ;
	
  if(fEMCALBadChannelMap) { 
    fEMCALBadChannelMap->Clear();
    delete  fEMCALBadChannelMap;
  }
  if(fPHOSBadChannelMap) { 
    fPHOSBadChannelMap->Clear();
    delete  fPHOSBadChannelMap;
  }
	
  if(fEMCALRecalibrationFactors) { 
    fEMCALRecalibrationFactors->Clear();
    delete  fEMCALRecalibrationFactors;
  }
  if(fPHOSRecalibrationFactors) { 
    fPHOSRecalibrationFactors->Clear();
    delete  fPHOSRecalibrationFactors;
  }
	
}

//_______________________________________________________________
Bool_t AliCalorimeterUtils::CheckCellFiducialRegion(AliVCluster* cluster, AliVCaloCells* cells, AliVEvent * event, Int_t iev) const {

	// Given the list of AbsId of the cluster, get the maximum cell and 
	// check if there are fNCellsFromBorder from the calorimeter border
	
    //If the distance to the border is 0 or negative just exit accept all clusters
	if(cells->GetType()==AliVCaloCells::kEMCALCell && fNCellsFromEMCALBorder <= 0 ) return kTRUE;
	if(cells->GetType()==AliVCaloCells::kPHOSCell  && fNCellsFromPHOSBorder  <= 0 ) return kTRUE;

  Int_t absIdMax	= -1;
	Float_t ampMax  = -1;
	
  AliMixedEvent * mixEvent = dynamic_cast<AliMixedEvent*> (event);
  Int_t nMixedEvents = 0 ; 
  Int_t * cellsCumul = NULL ;
  Int_t numberOfCells = 0 ;  
  if (mixEvent){
    nMixedEvents = mixEvent->GetNumberOfEvents() ; 
    if (cells->GetType()==AliVCaloCells::kEMCALCell) {
      cellsCumul =  mixEvent->GetEMCALCellsCumul() ; 
      numberOfCells = mixEvent->GetNumberOfEMCALCells() ;
    } 
      
    else if (cells->GetType()==AliVCaloCells::kPHOSCell) {
      cellsCumul =  mixEvent->GetPHOSCellsCumul() ; 
      numberOfCells = mixEvent->GetNumberOfPHOSCells() ;
    } 
    
    if(cellsCumul){
      
      Int_t startCell = cellsCumul[iev] ; 
      Int_t endCell   = (iev+1 < nMixedEvents)?cellsCumul[iev+1]:numberOfCells;
      //Find cells with maximum amplitude
      for(Int_t i = 0; i < cluster->GetNCells() ; i++){
        Int_t absId = cluster->GetCellAbsId(i) ;
        for (Int_t j = startCell; j < endCell ;  j++) {
          Short_t cellNumber; 
          Double_t amp ; 
          Double_t time; 
          cells->GetCell(j, cellNumber, amp, time) ; 
          if (absId == cellNumber) {
            if(amp > ampMax){
              ampMax   = amp;
              absIdMax = absId;
            }        
          }
        }
      }//loop on cluster cells
    }// cells cumul available
    else {
      printf("AliCalorimeterUtils::CheckCellFiducialRegion() - CellsCumul is NULL!!!\n");
      abort();
    }
  } else {//Normal SE Events
    for(Int_t i = 0; i < cluster->GetNCells() ; i++){
      Int_t absId = cluster->GetCellAbsId(i) ;
      Float_t amp	= cells->GetCellAmplitude(absId);
      if(amp > ampMax){
        ampMax   = amp;
        absIdMax = absId;
      }
    }
  }
	
	if(fDebug > 1)
		printf("AliCalorimeterUtils::CheckCellFiducialRegion() - Cluster Max AbsId %d, Cell Energy %2.2f, Cluster Energy %2.2f\n", 
		   absIdMax, ampMax, cluster->E());
	
	if(absIdMax==-1) return kFALSE;
	
	//Check if the cell is close to the borders:
	Bool_t okrow = kFALSE;
	Bool_t okcol = kFALSE;

	if(cells->GetType()==AliVCaloCells::kEMCALCell){
	
		Int_t iTower = -1, iIphi = -1, iIeta = -1, iphi = -1, ieta = -1, iSM = -1; 
		fEMCALGeo->GetCellIndex(absIdMax,iSM,iTower,iIphi,iIeta); 
		fEMCALGeo->GetCellPhiEtaIndexInSModule(iSM,iTower,iIphi, iIeta,iphi,ieta);
		if(iSM < 0 || iphi < 0 || ieta < 0 ) {
      Fatal("CheckCellFidutialRegion","Negative value for super module: %d, or cell ieta: %d, or cell iphi: %d, check EMCAL geometry name\n",iSM,ieta,iphi);
    }
      
		//Check rows/phi
		if(iSM < 10){
			if(iphi >= fNCellsFromEMCALBorder && iphi < 24-fNCellsFromEMCALBorder) okrow =kTRUE; 
	    }
		else{
			if(iphi >= fNCellsFromEMCALBorder && iphi < 12-fNCellsFromEMCALBorder) okrow =kTRUE; 
		}
		
		//Check collumns/eta
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
		if(fDebug > 1)
		{
			printf("AliCalorimeterUtils::CheckCellFiducialRegion() - EMCAL Cluster in %d cells fiducial volume: ieta %d, iphi %d, SM %d ?",
				   fNCellsFromEMCALBorder, ieta, iphi, iSM);
			if (okcol && okrow ) printf(" YES \n");
			else  printf(" NO: column ok? %d, row ok? %d \n",okcol,okrow);
		}
	}//EMCAL
	else if(cells->GetType()==AliVCaloCells::kPHOSCell){
		Int_t relId[4];
		Int_t irow = -1, icol = -1;
		fPHOSGeo->AbsToRelNumbering(absIdMax,relId);
		irow = relId[2];
		icol = relId[3];
		//imod = relId[0]-1;
		if(irow >= fNCellsFromPHOSBorder && irow < 64-fNCellsFromPHOSBorder) okrow =kTRUE; 
		if(icol >= fNCellsFromPHOSBorder && icol < 56-fNCellsFromPHOSBorder) okcol =kTRUE; 
		if(fDebug > 1)
		{
			printf("AliCalorimeterUtils::CheckCellFiducialRegion() - PHOS Cluster in %d cells fiducial volume: icol %d, irow %d, Module %d?",
				   fNCellsFromPHOSBorder, icol, irow, relId[0]-1);
			if (okcol && okrow ) printf(" YES \n");
			else  printf(" NO: column ok? %d, row ok? %d \n",okcol,okrow);
		}
	}//PHOS
	
	if (okcol && okrow) return kTRUE; 
	else                return kFALSE;
	
}	

//_________________________________________________________________________________________________________
Bool_t AliCalorimeterUtils::ClusterContainsBadChannel(TString calorimeter,UShort_t* cellList, Int_t nCells){
	// Check that in the cluster cells, there is no bad channel of those stored 
	// in fEMCALBadChannelMap or fPHOSBadChannelMap
	
	if (!fRemoveBadChannels) return kFALSE;
	//printf("fEMCALBadChannelMap %p, fPHOSBadChannelMap %p \n",fEMCALBadChannelMap,fPHOSBadChannelMap);
	if(calorimeter == "EMCAL" && !fEMCALBadChannelMap) return kFALSE;
	if(calorimeter == "PHOS"  && !fPHOSBadChannelMap)  return kFALSE;

	Int_t icol = -1;
	Int_t irow = -1;
	Int_t imod = -1;
	for(Int_t iCell = 0; iCell<nCells; iCell++){
	
		//Get the column and row
		if(calorimeter == "EMCAL"){
			Int_t iTower = -1, iIphi = -1, iIeta = -1; 
			fEMCALGeo->GetCellIndex(cellList[iCell],imod,iTower,iIphi,iIeta); 
			if(fEMCALBadChannelMap->GetEntries() <= imod) continue;
			fEMCALGeo->GetCellPhiEtaIndexInSModule(imod,iTower,iIphi, iIeta,irow,icol);		
      
      if(imod < 0 || irow < 0 || icol < 0 ) {
        Fatal("ClusterContainsBadChannel","Negative value for super module: %d, or cell icol: %d, or cell irow: %d, check EMCAL geometry name\n",imod,icol,irow);
      }
      
			if(GetEMCALChannelStatus(imod, icol, irow))return kTRUE;
		}
		else if(calorimeter=="PHOS"){
			Int_t    relId[4];
			fPHOSGeo->AbsToRelNumbering(cellList[iCell],relId);
			irow = relId[2];
			icol = relId[3];
			imod = relId[0]-1;
			if(fPHOSBadChannelMap->GetEntries() <= imod)continue;
			if(GetPHOSChannelStatus(imod, icol, irow)) return kTRUE;
		}
		else return kFALSE;
		
	}// cell cluster loop

	return kFALSE;

}

//____________________________________________________________________________________________________________________________________________________
Int_t AliCalorimeterUtils::GetModuleNumber(AliAODPWG4Particle * particle, AliVEvent * inputEvent) const
{
	//Get the EMCAL/PHOS module number that corresponds to this particle
	
	Int_t absId = -1;
	if(particle->GetDetector()=="EMCAL"){
		fEMCALGeo->GetAbsCellIdFromEtaPhi(particle->Eta(),particle->Phi(), absId);
		if(fDebug > 2) 
		  printf("AliCalorimeterUtils::GetModuleNumber(PWG4AOD) - EMCAL: cluster eta %f, phi %f, absid %d, SuperModule %d\n",
			 particle->Eta(), particle->Phi()*TMath::RadToDeg(),absId, fEMCALGeo->GetSuperModuleNumber(absId));
		return fEMCALGeo->GetSuperModuleNumber(absId) ;
	}//EMCAL
	else if(particle->GetDetector()=="PHOS"){
    // In case we use the MC reader, the input are TParticles, 
    // in this case use the corresponing method in PHOS Geometry to get the particle.
    if(strcmp(inputEvent->ClassName(), "AliMCEvent") == 0 )
    {
      Int_t mod =-1;
      Double_t z = 0., x=0.;
      TParticle* primary = 0x0;
      AliStack * stack = ((AliMCEvent*)inputEvent)->Stack();
      if(stack) {
        primary = stack->Particle(particle->GetCaloLabel(0));
      }
      else {
        Fatal("GetModuleNumber(PWG4AOD)", "Stack not available, stop!");
      }
      
      if(primary){
        fPHOSGeo->ImpactOnEmc(primary,mod,z,x) ;
      }
      else{
        Fatal("GetModuleNumber(PWG4AOD)", "Primary not available, stop!");
      }
      return mod;
    }
    // Input are ESDs or AODs, get the PHOS module number like this.
    else{
      AliVCluster *cluster = inputEvent->GetCaloCluster(particle->GetCaloLabel(0));
      return GetModuleNumber(cluster);
    }
	}//PHOS
	
	return -1;
}

//____________________________________________________________________________________________________________________________________________________
Int_t AliCalorimeterUtils::GetModuleNumber(AliVCluster * cluster) const
{
	//Get the EMCAL/PHOS module number that corresponds to this cluster
	TLorentzVector lv;
	Double_t v[]={0.,0.,0.}; //not necessary to pass the real vertex.
	cluster->GetMomentum(lv,v);
	Float_t phi = lv.Phi();
	if(phi < 0) phi+=TMath::TwoPi();	
	Int_t absId = -1;
	if(cluster->IsEMCAL()){
		fEMCALGeo->GetAbsCellIdFromEtaPhi(lv.Eta(),phi, absId);
		if(fDebug > 2) 
		  printf("AliCalorimeterUtils::GetModuleNumber() - EMCAL: cluster eta %f, phi %f, absid %d, SuperModule %d\n",
			 lv.Eta(), phi*TMath::RadToDeg(),absId, fEMCALGeo->GetSuperModuleNumber(absId));
		return fEMCALGeo->GetSuperModuleNumber(absId) ;
	}//EMCAL
	else if(cluster->IsPHOS()) {
		Int_t    relId[4];
		if ( cluster->GetNCells() > 0) {
			absId = cluster->GetCellAbsId(0);
			if(fDebug > 2) 
				printf("AliCalorimeterUtils::GetModuleNumber() - PHOS: cluster eta %f, phi %f, e %f, absId %d\n",
					   lv.Eta(), phi*TMath::RadToDeg(), lv.E(), absId);
		}
		else return -1;
		
		if ( absId >= 0) {
			fPHOSGeo->AbsToRelNumbering(absId,relId);
			if(fDebug > 2) 
			  printf("AliCalorimeterUtils::GetModuleNumber() - PHOS: Module %d\n",relId[0]-1);
			return relId[0]-1;
		}
		else return -1;
	}//PHOS
	
	return -1;
}

//_____________________________________________________________________________________________________________
Int_t AliCalorimeterUtils::GetModuleNumberCellIndexes(const Int_t absId, const TString calo, Int_t & icol, Int_t & irow, Int_t & iRCU) const
{
	//Get the EMCAL/PHOS module, columns, row and RCU number that corresponds to this absId
	Int_t imod = -1;
	if ( absId >= 0) {
		if(calo=="EMCAL"){
			Int_t iTower = -1, iIphi = -1, iIeta = -1; 
			fEMCALGeo->GetCellIndex(absId,imod,iTower,iIphi,iIeta); 
			fEMCALGeo->GetCellPhiEtaIndexInSModule(imod,iTower,iIphi, iIeta,irow,icol);
      if(imod < 0 || irow < 0 || icol < 0 ) {
        Fatal("GetModuleNumberCellIndexes()","Negative value for super module: %d, or cell icol: %d, or cell irow: %d, check EMCAL geometry name\n",imod,icol,irow);
      }
      
			//RCU0
			if (0<=irow&&irow<8) iRCU=0; // first cable row
			else if (8<=irow&&irow<16 && 0<=icol&&icol<24) iRCU=0; // first half; 
			//second cable row
			//RCU1
			else if(8<=irow&&irow<16 && 24<=icol&&icol<48) iRCU=1; // second half; 
			//second cable row
			else if(16<=irow&&irow<24) iRCU=1; // third cable row
			
			if (imod%2==1) iRCU = 1 - iRCU; // swap for odd=C side, to allow us to cable both sides the same
			if (iRCU<0) {
				Fatal("GetModuleNumberCellIndexes()","Wrong EMCAL RCU number = %d\n", iRCU);
			}			
			
			return imod ;
		}//EMCAL
		else{//PHOS
			Int_t    relId[4];
			fPHOSGeo->AbsToRelNumbering(absId,relId);
			irow = relId[2];
			icol = relId[3];
			imod = relId[0]-1;
			iRCU= (Int_t)(relId[2]-1)/16 ;
			//Int_t iBranch= (Int_t)(relid[3]-1)/28 ; //0 to 1
			if (iRCU >= 4) {
				Fatal("GetModuleNumberCellIndexes()","Wrong PHOS RCU number = %d\n", iRCU);
			}			
			return imod;
		}//PHOS	
	}
	
	return -1;
}

//_______________________________________________________________
//void AliCalorimeterUtils::Init()
//{
//	//Init reader. Method to be called in AliAnaPartCorrMaker
//	
//	fEMCALBadChannelMap->SetName(Form("EMCALBadMap_%s",fTaskName.Data()));
//	fPHOSBadChannelMap->SetName(Form("PHOSBadMap_%s",fTaskName.Data()));
//}

//_______________________________________________________________
void AliCalorimeterUtils::InitParameters()
{
  //Initialize the parameters of the analysis.
  fEMCALGeoName = "EMCAL_FIRSTYEAR";
  fPHOSGeoName  = "PHOSgeo";
	
  if(gGeoManager) {// geoManager was set
	if(fDebug > 2)printf("AliCalorimeterUtils::InitParameters() - Geometry manager available\n");
	fEMCALGeoMatrixSet = kTRUE;	 
	fPHOSGeoMatrixSet  = kTRUE;	 
  }
  else{
	fEMCALGeoMatrixSet = kFALSE;
	fPHOSGeoMatrixSet  = kFALSE;
  }
		
  fRemoveBadChannels   = kFALSE;
	
  fNCellsFromEMCALBorder   = 0;
  fNCellsFromPHOSBorder    = 0;
  fNoEMCALBorderAtEta0     = kFALSE;
}

//________________________________________________________________
void AliCalorimeterUtils::InitEMCALBadChannelStatusMap(){
  //Init EMCAL bad channels map
   if(fDebug > 0 )printf("AliCalorimeterUtils::InitEMCALBadChannelStatusMap()\n");
  //In order to avoid rewriting the same histograms
  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);

  fEMCALBadChannelMap = new TObjArray(12);
  //TH2F * hTemp = new  TH2I("EMCALBadChannelMap","EMCAL SuperModule bad channel map", 48, 0, 48, 24, 0, 24);
  for (int i = 0; i < 12; i++) {
    fEMCALBadChannelMap->Add(new TH2I(Form("EMCALBadChannelMap_Mod%d",i),Form("EMCALBadChannelMap_Mod%d",i), 48, 0, 48, 24, 0, 24));
    //fEMCALBadChannelMap->Add((TH2I*)hTemp->Clone(Form("EMCALBadChannelMap_Mod%d",i)));
  }

  //delete hTemp;

  fEMCALBadChannelMap->SetOwner(kTRUE);
  fEMCALBadChannelMap->Compress();

  //In order to avoid rewriting the same histograms
  TH1::AddDirectory(oldStatus);		
}

//________________________________________________________________
void AliCalorimeterUtils::InitPHOSBadChannelStatusMap(){
  //Init PHOS bad channels map
  if(fDebug > 0 )printf("AliCalorimeterUtils::InitPHOSBadChannelStatusMap()\n");
  //In order to avoid rewriting the same histograms
  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);

  fPHOSBadChannelMap = new TObjArray(5);	
  for (int i = 0; i < 5; i++)fPHOSBadChannelMap->Add(new TH2I(Form("PHOSBadChannelMap_Mod%d",i),Form("PHOSBadChannelMap_Mod%d",i), 56, 0, 56, 64, 0, 64));

  fPHOSBadChannelMap->SetOwner(kTRUE);
  fPHOSBadChannelMap->Compress();
  
  //In order to avoid rewriting the same histograms
  TH1::AddDirectory(oldStatus);		
}

//________________________________________________________________
void AliCalorimeterUtils::InitEMCALRecalibrationFactors(){
	//Init EMCAL recalibration factors
	if(fDebug > 0 )printf("AliCalorimeterUtils::InitEMCALRecalibrationFactors()\n");
	//In order to avoid rewriting the same histograms
	Bool_t oldStatus = TH1::AddDirectoryStatus();
	TH1::AddDirectory(kFALSE);

	fEMCALRecalibrationFactors = new TObjArray(12);
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
void AliCalorimeterUtils::InitPHOSRecalibrationFactors(){
	//Init EMCAL recalibration factors
	if(fDebug > 0 )printf("AliCalorimeterUtils::InitPHOSRecalibrationFactors()\n");
	//In order to avoid rewriting the same histograms
	Bool_t oldStatus = TH1::AddDirectoryStatus();
	TH1::AddDirectory(kFALSE);

	fPHOSRecalibrationFactors = new TObjArray(5);
	for (int i = 0; i < 5; i++)fPHOSRecalibrationFactors->Add(new TH2F(Form("PHOSRecalFactors_Mod%d",i),Form("PHOSRecalFactors_Mod%d",i), 56, 0, 56, 64, 0, 64));
	//Init the histograms with 1
	for (Int_t m = 0; m < 5; m++) {
		for (Int_t i = 0; i < 56; i++) {
			for (Int_t j = 0; j < 64; j++) {
				SetPHOSChannelRecalibrationFactor(m,i,j,1.);
			}
		}
	}
	fPHOSRecalibrationFactors->SetOwner(kTRUE);
	fPHOSRecalibrationFactors->Compress();
	
	//In order to avoid rewriting the same histograms
	TH1::AddDirectory(oldStatus);		
}


//________________________________________________________________
void AliCalorimeterUtils::InitEMCALGeometry()
{
	//Initialize EMCAL geometry if it did not exist previously
	if (!fEMCALGeo){
		fEMCALGeo = new AliEMCALGeoUtils(fEMCALGeoName);
		if(fDebug > 0){
			printf("AliCalorimeterUtils::InitEMCALGeometry()");
			if (!gGeoManager) printf(" - Careful!, gGeoManager not loaded, load misalign matrices");
			printf("\n");
		}
	}
}

//________________________________________________________________
void AliCalorimeterUtils::InitPHOSGeometry()
{
	//Initialize PHOS geometry if it did not exist previously
	if (!fPHOSGeo){
		fPHOSGeo = new AliPHOSGeoUtils(fPHOSGeoName); 
		if(fDebug > 0){
			printf("AliCalorimeterUtils::InitPHOSGeometry()");
			if (!gGeoManager) printf(" - Careful!, gGeoManager not loaded, load misalign matrices");
			printf("\n");
		}	
	}	
}

//________________________________________________________________
void AliCalorimeterUtils::Print(const Option_t * opt) const
{

  //Print some relevant parameters set for the analysis
  if(! opt)
    return;

  printf("***** Print: %s %s ******\n", GetName(), GetTitle() ) ;
  printf("Remove Clusters with bad channels? %d\n",fRemoveBadChannels);
  printf("Remove Clusters with max cell at less than %d cells from EMCAL border and %d cells from PHOS border\n",
	 fNCellsFromEMCALBorder, fNCellsFromPHOSBorder);
  if(fNoEMCALBorderAtEta0) printf("Do not remove EMCAL clusters at Eta = 0\n");
  printf("Recalibrate Clusters? %d\n",fRecalibration);

  printf("    \n") ;
} 

//________________________________________________________________
Float_t AliCalorimeterUtils::RecalibrateClusterEnergy(AliVCluster * cluster, AliVCaloCells * cells){
	// Recalibrate the cluster energy, considering the recalibration map and the energy of the cells that compose the cluster.

  //Initialize some used variables
	Float_t energy   = 0;
	Int_t absId      = -1;
	Int_t icol = -1, irow = -1, iRCU = -1, module=1;
	Float_t factor = 1, frac = 0;  
  
	if(cells) {
	
	//Get the cluster number of cells and list of absId, check what kind of cluster do we have.
	UShort_t * index    = cluster->GetCellsAbsId() ;
	Double_t * fraction = cluster->GetCellsAmplitudeFraction() ;
	Int_t ncells     = cluster->GetNCells();	
	TString calo     = "EMCAL";
	if(cluster->IsPHOS()) calo = "PHOS";
	
	//Loop on the cells, get the cell amplitude and recalibration factor, multiply and and to the new energy
	for(Int_t icell = 0; icell < ncells; icell++){
		absId = index[icell];
		frac =  fraction[icell];
		if(frac < 1e-3) frac = 1; //in case of EMCAL, this is set as 0, not used.
        module = GetModuleNumberCellIndexes(absId,calo,icol,irow,iRCU);
		if(cluster->IsPHOS()) factor = GetPHOSChannelRecalibrationFactor (module,icol,irow);
		else                  factor = GetEMCALChannelRecalibrationFactor(module,icol,irow);
		if(fDebug>2)
			printf("AliCalorimeterUtils::RecalibrateClusterEnergy() - recalibrate cell: %s, module %d, col %d, row %d, cell fraction %f, recalibration factor %f, cell energy %f\n", 
				calo.Data(),module,icol,irow,frac,factor,cells->GetCellAmplitude(absId));
		
		energy += cells->GetCellAmplitude(absId)*factor*frac;
	}
	
	if(fDebug>1)
		printf("AliCalorimeterUtils::RecalibrateClusterEnergy() - Energy before %f, after %f\n",cluster->E(),energy);
    
	}// cells available
  else{
    Fatal("RecalibrateClusterEnergy()","Cells pointer does not exist!");
  }
  
	return energy;
}

//________________________________________________________________
void AliCalorimeterUtils::SetGeometryTransformationMatrices(AliVEvent* inputEvent) 
{
  //Set the calorimeters transformation matrices
	
  //Get the EMCAL transformation geometry matrices from ESD 
  if (!gGeoManager && fEMCALGeo && !fEMCALGeoMatrixSet) { 
	  if(fDebug > 1) 
		  printf(" AliCalorimeterUtils::SetGeometryTransformationMatrices() - Load EMCAL misalignment matrices. \n");
	  if(!strcmp(inputEvent->GetName(),"AliESDEvent"))  {
			for(Int_t mod=0; mod < (fEMCALGeo->GetEMCGeometry())->GetNumberOfSuperModules(); mod++){ 
				if(inputEvent->GetEMCALMatrix(mod)) {
					//printf("EMCAL: mod %d, matrix %p\n",mod, ((AliESDEvent*)inputEvent)->GetEMCALMatrix(mod));
					fEMCALGeo->SetMisalMatrix(inputEvent->GetEMCALMatrix(mod),mod) ;
					fEMCALGeoMatrixSet = kTRUE;//At least one, so good
				}
			}// loop over super modules	
		}//ESD as input
		else {
			if(fDebug > 1)
				printf("AliCalorimeterUtils::SetGeometryTransformationMatrices() - Setting of EMCAL transformation matrixes for AODs not implemented yet. \n Import geometry.root file\n");
				}//AOD as input
  }//EMCAL geo && no geoManager
	
	//Get the PHOS transformation geometry matrices from ESD 
	if (!gGeoManager && fPHOSGeo && !fPHOSGeoMatrixSet) {
		if(fDebug > 1) 
			printf(" AliCalorimeterUtils::SetGeometryTransformationMatrices() - Load PHOS misalignment matrices. \n");
			if(!strcmp(inputEvent->GetName(),"AliESDEvent"))  {
				for(Int_t mod=0; mod < 5; mod++){ 
					if(inputEvent->GetPHOSMatrix(mod)) {
						//printf("PHOS: mod %d, matrix %p\n",mod, ((AliESDEvent*)inputEvent)->GetPHOSMatrix(mod));
						fPHOSGeo->SetMisalMatrix(inputEvent->GetPHOSMatrix(mod),mod) ;
						fPHOSGeoMatrixSet  = kTRUE; //At least one so good
					}
				}// loop over modules	
			}//ESD as input
			else {
				if(fDebug > 1) 
					printf("AliCalorimeterUtils::SetGeometryTransformationMatrices() - Setting of EMCAL transformation matrixes for AODs not implemented yet. \n Import geometry.root file\n");
					}//AOD as input
	}//PHOS geo	and  geoManager was not set

}

