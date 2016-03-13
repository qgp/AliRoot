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

//_________________________________________________________________________
//  The AliEMCALLoader gets the TClonesArray and TObjArray for reading
//  Hits, Dgits, SDigits and RecPoints. Filling is managed in the GetEvent()
//  method.
//  It also provides acces methods to the calibration and simulation OCDB parameters 
//
//*-- Author: Yves Schutz (SUBATECH) & Dmitri Peressounko (RRC KI & SUBATECH)
//*--         Completely redesigned by Dmitri Peressounko March 2001  
//
//*-- YS June 2001 : renamed the original AliEMCALIndexToObject and make
//*--         systematic usage of TFolders without changing the interface
// 
//*-- Marco van Leeuwen, Jan 2006: complete revision to simplify reading
//*--         and fit better in general ALICE scheme
//*-- GCB: Remove TClonesArrays and TObjArrays data members, they are created elsewhere.
//*--      Provide access to OCDB calibration and simulation parameters.          
//
//////////////////////////////////////////////////////////////////////////////

// --- ROOT system ---
#include "TMath.h"
#include "TTree.h"
// --- Standard library ---

// --- AliRoot header files ---
#include "AliEMCALLoader.h"
#include "AliLog.h"
#include "AliCDBLocal.h"
#include "AliCDBStorage.h"
#include "AliCDBManager.h"
#include "AliCDBEntry.h"

 ClassImp(AliEMCALLoader)
  
const TString         AliEMCALLoader::fgkECARecPointsBranchName("EMCALECARP");//Name for branch with ECA Reconstructed Points
const TString         AliEMCALLoader::fgkECADigitsBranchName("DIGITS");//Name for branch with ECA Digits
const TString         AliEMCALLoader::fgkECASDigitsBranchName("SDIGITS");//Name for branch with ECA SDigits

AliEMCALCalibData*    AliEMCALLoader::fgCalibData = 0; //energy calibration data
AliEMCALCalibTime*    AliEMCALLoader::fgCalibTime = 0; //time calibration data
AliCaloCalibPedestal* AliEMCALLoader::fgCaloPed   = 0; //dead map data
AliEMCALSimParam*     AliEMCALLoader::fgSimParam  = 0; //simulation parameters
AliEMCALRecParam*     AliEMCALLoader::fgRecParam  = 0; //reconstruction parameters

//____________________________________________________________________________ 
AliEMCALLoader::AliEMCALLoader()
: fDebug(0)
{
  //Default constructor for EMCAL Loader Class

}

//____________________________________________________________________________ 
AliEMCALLoader::AliEMCALLoader(const Char_t *detname,const Char_t *eventfoldername)
  : AliLoader(detname,eventfoldername), fDebug(0)
{
  //Specific constructor for EMCAL Loader class

}

//____________________________________________________________________________
AliEMCALLoader::AliEMCALLoader(const Char_t *name, TFolder *topfolder)
  : AliLoader(name,topfolder), fDebug(0)
{
  //Specific constructor for EMCAL Loader class

}

//____________________________________________________________________________ 
AliEMCALLoader::~AliEMCALLoader()
{
  // Disconnect trees and remove arrays
  if (TreeH())
    TreeH()->SetBranchAddress(fDetectorName,0);
//  if (TreeD())
//    TreeD()->SetBranchAddress(fDetectorName,0);
//  if (TreeS())
//    TreeS()->SetBranchAddress(fDetectorName,0);
//  if (TreeR())
//    TreeR()->SetBranchAddress(fgkECARecPointsBranchName,0);
	
	Clean(fgkECASDigitsBranchName);
	Clean(fgkECADigitsBranchName);
	Clean(fgkECARecPointsBranchName);
	
	AliLoader::CleanFolders();
		
}

//____________________________________________________________________________ 
AliEMCALCalibData* AliEMCALLoader::CalibData()
{ 
  // Check if the instance of AliEMCALCalibData exists, if not, create it if 
  // the OCDB is available, and finally return it.
  
  if(!fgCalibData && (AliCDBManager::Instance()->IsDefaultStorageSet()))
    {
      AliCDBEntry *entry = (AliCDBEntry*) 
	AliCDBManager::Instance()->Get("EMCAL/Calib/Data");
      if (entry) fgCalibData =  (AliEMCALCalibData*) entry->GetObject();
    }
  
  if(!fgCalibData)
    AliFatal("Calibration parameters not found in CDB!");
  
  return fgCalibData;
  
}

//____________________________________________________________________________ 
AliEMCALCalibTime* AliEMCALLoader::CalibTime()
{ 
  // Check if the instance of AliEMCALCalibTime exists, if not, create it if 
  // the OCDB is available, and finally return it.
  
  if(!fgCalibTime && (AliCDBManager::Instance()->IsDefaultStorageSet()))
  {
    AliCDBEntry *entry = (AliCDBEntry*) 
    AliCDBManager::Instance()->Get("EMCAL/Calib/Time");
    if (entry) fgCalibTime =  (AliEMCALCalibTime*) entry->GetObject();
  }
  
  if(!fgCalibTime)
    AliFatal("Calibration parameters not found in CDB!");
  
  return fgCalibTime;
  
}


//____________________________________________________________________________ 
AliCaloCalibPedestal* AliEMCALLoader::PedestalData()
{ 
	// Check if the instance of AliCaloCalibPedestal exists, if not, create it if 
	// the OCDB is available, and finally return it.
	
	if(!fgCaloPed && (AliCDBManager::Instance()->IsDefaultStorageSet()))
    {
		AliCDBEntry *entry = (AliCDBEntry*) 
		AliCDBManager::Instance()->Get("EMCAL/Calib/Pedestals");
		if (entry) fgCaloPed =  (AliCaloCalibPedestal*) entry->GetObject();
    }
	
	if(!fgCaloPed)
		AliFatal("Pedestal info not found in CDB!");
	
	return fgCaloPed;
	
}

//____________________________________________________________________________ 
AliEMCALSimParam* AliEMCALLoader::SimulationParameters()
{ 
  // Check if the instance of AliEMCALSimParam exists, if not, create it if 
  // the OCDB is available, and finally return it.
  
  if(!fgSimParam && (AliCDBManager::Instance()->IsDefaultStorageSet()))
    {
      AliCDBEntry *entry = (AliCDBEntry*) 
      AliCDBManager::Instance()->Get("EMCAL/Calib/SimParam");
      if (entry) fgSimParam =  (AliEMCALSimParam*) entry->GetObject();
      
    }
  
  if(!fgSimParam)
    AliFatal("Simulations parameters not found in CDB!");
  
  return fgSimParam;
  
}


//____________________________________________________________________________ 
AliEMCALRecParam* AliEMCALLoader::ReconstructionParameters(Int_t eventType = 0)
{ 
  // Check if the instance of AliEMCALRecParam exists, if not, create it if 
  // the OCDB is available, and finally return it. 
  // The event type must be provided.
  
  if(!fgRecParam && (AliCDBManager::Instance()->IsDefaultStorageSet()))
  {
    AliCDBEntry *entry = (AliCDBEntry*) 
    AliCDBManager::Instance()->Get("EMCAL/Calib/RecoParam");
    if (entry) fgRecParam =  (AliEMCALRecParam*)((TObjArray *) entry->GetObject())->At(eventType);
    
  }
  
  if(!fgRecParam)
    AliFatal("Reconstruction parameters not found in CDB!");
  
  return fgRecParam;
  
}


//____________________________________________________________________________ 
Int_t AliEMCALLoader::GetEvent() 
{
  //Method to load all of the data
  //members of the EMCAL for a given
  //event from the Trees

  AliLoader::GetEvent();  // First call AliLoader to do all the groundwork
  
  // *** Hits ***
  // Hits are now handled directly on the AliEMCALSDigitizer, the only place it is requested.
  // together with AliEveEMCALData
	
  // *** SDigits ***
  // Initialize the SDigits TClonesArray, only if it did not existed before
  MakeSDigitsArray();
  
  TTree *treeS = TreeS();
  if (treeS) {
    TBranch * branchS = treeS->GetBranch(fDetectorName);
    
    // Reset SDigits array and branch
    branchS->ResetAddress();
    TClonesArray* sdigits = const_cast<AliEMCALLoader *>(this)->SDigits();
    if (sdigits) sdigits->Clear("C");
    
    branchS->SetAddress(&sdigits);
    branchS->GetEntry(0);
  }
  
  // *** Digits ***
  // Initialize the Digits TClonesArray, only if it did not existed before
  MakeDigitsArray();
  
  TTree *treeD = TreeD();
  if (treeD) {
    TBranch * branchD = treeD->GetBranch(fDetectorName);
    
    // Reset Digits array and branch
    branchD->ResetAddress();
    TClonesArray* digits = const_cast<AliEMCALLoader *>(this)->Digits();
    if (digits) digits->Clear("C");
    
    branchD->SetAddress(&digits);
    branchD->GetEntry(0);
  }
  
  // *** RecPoints ***  
  // Initialize the RecPoints TObjArray, only if it did not existed before
  MakeRecPointsArray();
  
  TTree *treeR = TreeR();
  if (treeR) {
    TBranch * branchR = treeR->GetBranch(fgkECARecPointsBranchName);
    
    // Reset RecPoints array and branch
    branchR->ResetAddress();
    TObjArray* rp = const_cast<AliEMCALLoader *>(this)->RecPoints();
    if (rp) rp->Clear();
    
    branchR->SetAddress(&rp);
    branchR->GetEntry(0);
  }
  
  return 0;
}

//____________________________________________________________________________
void AliEMCALLoader::MakeSDigitsArray(){
  // Add SDigits array to the data folder
  if (SDigits()) return;
  TClonesArray* sdigits = new TClonesArray("AliEMCALDigit",0);
  sdigits->SetName(fgkECASDigitsBranchName);
  GetDetectorDataFolder()->Add(sdigits);
}

//____________________________________________________________________________
void AliEMCALLoader::MakeDigitsArray(){
  // Add Digits array to the data folder
  if (Digits()) return;
  TClonesArray* digits = new TClonesArray("AliEMCALDigit",0);
  digits->SetName(fgkECADigitsBranchName);
  GetDetectorDataFolder()->Add(digits);
}

//____________________________________________________________________________
void AliEMCALLoader::MakeRecPointsArray(){
  // Add RecPoints array to the data folder
  if (RecPoints()) return;
  TObjArray* rp = new TObjArray(0);
  rp->SetName(fgkECARecPointsBranchName);
  GetDetectorDataFolder()->Add(rp);
}
