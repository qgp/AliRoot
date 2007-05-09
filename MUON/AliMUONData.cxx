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

/// \class AliMUONData
///
/// Class containing MUON data: hits, digits, rawclusters, globaltrigger, localtrigger, etc ..
/// The classe makes the lik between the MUON data lists and the event trees from loaders
///
/// \author Gines Martinez, Subatech,  September 2003
///

#include "AliMUONData.h"
#include "AliMUONDataIterator.h"
#include "AliMUONConstants.h"
#include "AliMUONHit.h"
#include "AliMUONDigit.h"
#include "AliMUONGlobalTrigger.h"
#include "AliMUONLocalTrigger.h"
#include "AliMUONRegionalTrigger.h"
#include "AliMUONTriggerCrateStore.h"
#include "AliMUONTriggerCircuit.h"
#include "AliMUONGeometryTransformer.h"
#include "AliMUONRawCluster.h"

// This is from rec, classes in base should not depend on rec !!!
#include "AliMUONTrack.h"
#include "AliMUONTriggerTrack.h"

#include "AliRunLoader.h"
#include "AliStack.h"
#include "AliLog.h"

#include <TString.h>
#include <TParticle.h>
#include <TNtuple.h>
#include <Riostream.h>
#include <TFile.h>

/// \cond CLASSIMP
ClassImp(AliMUONData)
/// \endcond
 
//_____________________________________________________________________________
  AliMUONData::AliMUONData():
    TNamed(),
    fRunLoader(0x0),
    fLoader(0x0),
    fHits(0x0),
    fDigits(0x0),
    fSDigits(0x0),
    fRawClusters(0x0),
    fGlobalTrigger(0x0),
    fLocalTrigger(0x0),
    fRegionalTrigger(0x0),
    fRecTracks(0x0),
    fRecTriggerTracks(0x0),
    fNhits(0),
    fNdigits(0x0),
    fNSdigits(0x0),
    fNrawclusters(0x0),
    fNglobaltrigger(0),
    fNlocaltrigger(0),
    fNregionaltrigger(0),
    fNrectracks(0),
    fNrectriggertracks(0),
    fSplitLevel(0),
    fCurrentEvent(-1)
{
/// Default constructor
}
//_____________________________________________________________________________
AliMUONData::AliMUONData(AliLoader * loader, const char* name, const char* title):
  TNamed(name,title),
    fRunLoader(0x0),
    fLoader(loader),
    fHits(0x0),
    fDigits(0x0),
    fSDigits(0x0),
    fRawClusters(0x0),
    fGlobalTrigger(0x0),
    fLocalTrigger(0x0),
    fRegionalTrigger(0x0),
    fRecTracks(0x0),
    fRecTriggerTracks(0x0),
    fNhits(0),
    fNdigits(0x0),
    fNSdigits(0x0),
    fNrawclusters(0x0),
    fNglobaltrigger(0),
    fNlocaltrigger(0),
    fNregionaltrigger(0),
    fNrectracks(0),
    fNrectriggertracks(0),
    fSplitLevel(0),
    fCurrentEvent(-1)
{
/// Standard constructor
}

//_____________________________________________________________________________
AliMUONData::AliMUONData(const char* galiceFile):
  TNamed("MUON", "MUON"),
    fRunLoader(0x0),
    fLoader(0x0),
    fHits(0x0),
    fDigits(0x0),
    fSDigits(0x0),
    fRawClusters(0x0),
    fGlobalTrigger(0x0),
    fLocalTrigger(0x0),
    fRegionalTrigger(0x0),
    fRecTracks(0x0),
    fRecTriggerTracks(0x0),
    fNhits(0),
    fNdigits(0x0),
    fNSdigits(0x0),
    fNrawclusters(0x0),
    fNglobaltrigger(0),
    fNlocaltrigger(0),
    fNregionaltrigger(0),
    fNrectracks(0),
    fNrectriggertracks(0),
    fSplitLevel(0),
    fCurrentEvent(-1)
{
/// Constructor for loading data from gAlice file

  fRunLoader = AliRunLoader::Open(galiceFile, "MUONFolder", "READ");
  if (!fRunLoader) {
    AliError(Form("Error opening %s file \n", galiceFile));
    return;
  }  

  fLoader = fRunLoader->GetLoader("MUONLoader");
  if ( ! fLoader ) {
    AliError(Form("Could get MUONLoader"));
    return;
  }  
}

//_____________________________________________________________________________
AliMUONData::~AliMUONData()
{
/// Destructor for AliMUONData
  if (fHits) {
    fHits->Delete();
    delete fHits;
  }
  
  if (fDigits) {
    fDigits->Delete();
    delete fDigits;
  }
  if (fSDigits) {
    fSDigits->Delete();
    delete fSDigits;
  }
  if (fRawClusters) {
    fRawClusters->Delete();
    delete fRawClusters;
  }
  if (fGlobalTrigger){
    fGlobalTrigger->Delete();
    delete fGlobalTrigger;
  }  
  if (fRegionalTrigger){
    fRegionalTrigger->Delete();
    delete fRegionalTrigger;
  }
  if (fLocalTrigger){
    fLocalTrigger->Delete();
    delete fLocalTrigger;
  }
  if (fRecTracks){
    fRecTracks->Delete();
    delete fRecTracks;
  }
  if (fRecTriggerTracks){
    fRecTriggerTracks->Delete();
    delete fRecTriggerTracks;
  }

  if (fRunLoader) {
    fRunLoader->UnloadAll();
    delete fRunLoader;
  }  
}
//____________________________________________________________________________
void AliMUONData::AddHit(Int_t fIshunt, Int_t track, Int_t detElemId, 
			 Int_t idpart, Float_t X, Float_t Y, Float_t Z, 
			 Float_t tof, Float_t momentum, Float_t theta, 
			 Float_t phi, Float_t length, Float_t destep,
			 Float_t Xref,Float_t Yref,Float_t Zref)
{
 /// Add new hit to the hit list

  TClonesArray &lhits = *fHits;
  new(lhits[fNhits++]) AliMUONHit(fIshunt, track, detElemId, 
				  idpart, X, Y, Z, 
				  tof, momentum, theta, 
				  phi, length, destep,
				  Xref,Yref,Zref);
}
//_____________________________________________________________________________
void AliMUONData::AddDigit(Int_t id, const AliMUONDigit& digit)
{
/// Add a MUON digit to the list of Digits of the detection plane id

  TClonesArray &ldigits = * Digits(id) ; 
  new(ldigits[fNdigits[id]++]) AliMUONDigit(digit);
}
//_____________________________________________________________________________
void AliMUONData::AddSDigit(Int_t id, const AliMUONDigit& Sdigit)
{
/// Add a MUON Sdigit to the list of SDigits of the detection plane id

  TClonesArray &lSdigits = * SDigits(id) ; 
  new(lSdigits[fNSdigits[id]++]) AliMUONDigit(Sdigit);
}

//_____________________________________________________________________________
void AliMUONData::AddGlobalTrigger(const AliMUONGlobalTrigger& trigger )
{
/// Add a MUON Global Trigger to the list (only one GlobalTrigger per event !);

  TClonesArray &globalTrigger = *fGlobalTrigger;
  new(globalTrigger[fNglobaltrigger++]) AliMUONGlobalTrigger(trigger);
}

//____________________________________________________________________________
void AliMUONData::AddRegionalTrigger(const  AliMUONRegionalTrigger& trigger)
{
/// add a MUON regional Trigger to the list
  TClonesArray &regionalTrigger = *fRegionalTrigger;
  new(regionalTrigger[fNregionaltrigger++]) AliMUONRegionalTrigger(trigger);
}
//____________________________________________________________________________
void AliMUONData::AddLocalTrigger(const  AliMUONLocalTrigger& trigger)
{
/// add a MUON Local Trigger to the list

  TClonesArray &localTrigger = *fLocalTrigger;
  new(localTrigger[fNlocaltrigger++]) AliMUONLocalTrigger(trigger);
}

//_____________________________________________________________________________
void AliMUONData::AddRawCluster(Int_t id, const AliMUONRawCluster& c)
{
/// Add a MUON rawcluster to the list in the detection plane id

  TClonesArray &lrawcl = *((TClonesArray*) fRawClusters->At(id));
  new(lrawcl[fNrawclusters[id]++]) AliMUONRawCluster(c);
}
//_____________________________________________________________________________
void AliMUONData::AddRecTrack(const AliMUONTrack& track)
{
/// Add a MUON rectrack

  TClonesArray &lrectracks = *fRecTracks;
  new(lrectracks[fNrectracks++]) AliMUONTrack(track);
}
//_____________________________________________________________________________
void AliMUONData::AddRecTriggerTrack(const AliMUONTriggerTrack& triggertrack)
{
/// Add a MUON triggerrectrack

  TClonesArray &lrectriggertracks = *fRecTriggerTracks;  
  new(lrectriggertracks[fNrectriggertracks++]) AliMUONTriggerTrack(triggertrack);
}
//____________________________________________________________________________
TClonesArray*  AliMUONData::Digits(Int_t DetectionPlane) const
{
/// Getting List of Digits

  if (fDigits)
    return ( (TClonesArray*) fDigits->At(DetectionPlane) );
  else
    return NULL;
}
//____________________________________________________________________________
TClonesArray*  AliMUONData::SDigits(Int_t DetectionPlane) const
{
/// Getting List of SDigits

  if (fSDigits)
    return ( (TClonesArray*) fSDigits->At(DetectionPlane) );
  else
    return NULL;
}
//____________________________________________________________________________
Bool_t   AliMUONData::IsRawClusterBranchesInTree()
{
/// Checking if there are RawCluster Branches In TreeR

  if (TreeR()==0x0) {
    AliError("No treeR in memory");
    return kFALSE;
  }
  else {
     char branchname[30];
     sprintf(branchname,"%sRawClusters1",GetName());
     TBranch * branch = 0x0;
     branch = TreeR()->GetBranch(branchname);
     if (branch)  return kTRUE;
     else return kFALSE;    
  }
}
//____________________________________________________________________________
Bool_t   AliMUONData::IsDigitsBranchesInTree()
{
/// Checking if there are RawCluster Branches In TreeR

  if (TreeD()==0x0) {
    AliError("No treeD in memory");
    return kFALSE;
  }
  else {
     char branchname[30];
     sprintf(branchname,"%sDigits1",GetName());
     TBranch * branch = 0x0;
     branch = TreeD()->GetBranch(branchname);
     if (branch)  return kTRUE;
     else return kFALSE;    
  }
}
//____________________________________________________________________________
Bool_t   AliMUONData::IsTriggerBranchesInTree()
{
/// Checking if there are Trigger Branches In TreeR
 if (TreeR()==0x0) {
    AliError("No treeR in memory");
    return kFALSE;
  }
  else {
     char branchname[30];
     sprintf(branchname,"%sLocalTrigger",GetName());
     TBranch * branch = 0x0;
     branch = TreeR()->GetBranch(branchname);
     if (branch)  return kTRUE;
     else return kFALSE;    
  }
}
//____________________________________________________________________________
Bool_t   AliMUONData::IsTriggerBranchesInTreeD()
{
/// Checking if there are Trigger Branches In TreeR
 if (TreeD()==0x0) {
    AliError("No treeD in memory");
    return kFALSE;
  }
  else {
     char branchname[30];
     sprintf(branchname,"%sLocalTrigger",GetName());
     TBranch * branch = 0x0;
     branch = TreeD()->GetBranch(branchname);
     if (branch)  return kTRUE;
     else return kFALSE;    
  }
}

//____________________________________________________________________________
Bool_t   AliMUONData::IsTrackBranchesInTree()
{
/// Checking if there are Track Branches In TreeT
  if (TreeT()==0x0) {
    AliError("No treeT in memory");
    return kFALSE;
  }
  else {
     char branchname[30];
     sprintf(branchname,"%sTrack",GetName());
     TBranch * branch = 0x0;
     branch = TreeT()->GetBranch(branchname);
     if (branch)  return kTRUE;
     else return kFALSE;    
  }
}
//____________________________________________________________________________
Bool_t   AliMUONData::IsTriggerTrackBranchesInTree()
{
/// Checking if there are TriggerTrack Branches In TreeT
  if (TreeT()==0x0) {
    AliError("No treeT in memory");
    return kFALSE;
  }
  else {
     char branchname[30];
     sprintf(branchname,"%sTriggerTrack",GetName());
     TBranch * branch = 0x0;
     branch = TreeT()->GetBranch(branchname);
     if (branch)  return kTRUE;
     else return kFALSE;    
  }
}
//____________________________________________________________________________
void AliMUONData::Fill(Option_t* option)
{
/// Method to fill the trees
  const char *cH   = strstr(option,"H");
  const char *cD   = strstr(option,"D");   // Digits branches in TreeD
  const char *cS   = strstr(option,"S");   // SDigits branches in TreeS
  const char *cRC  = strstr(option,"RC");  // RawCluster branches in TreeR
  const char *cGLT = strstr(option,"GLT"); // Global and Local Trigger branches in TreeD
  const char *cTC = strstr(option,"TC");   // global and local Trigger branches Copy in TreeR
  const char *cRT  = strstr(option,"RT");  // Reconstructed Track in TreeT
  const char *cRL = strstr(option,"RL");   // Reconstructed Trigger Track in TreeT

  //const char *cRP  = strstr(option,"RP");  // Reconstructed Particle in TreeP
  
  char branchname[30];
  TBranch * branch = 0x0;

  // Filling TreeH
  if ( TreeH() && cH ) 
  {
    TreeH()->Fill();
  }  
 
  // Filling TreeD

  if ( TreeD() && cD && cGLT )
  {
    // Writing digits and (global+local) trigger at once.
    TreeD()->Fill();
  }
  else
  {
    if ( TreeD() && cD ) 
    {
      if ( IsTriggerBranchesInTreeD() ) 
      {
        for (int i=0; i<AliMUONConstants::NCh(); i++) 
        {
          sprintf(branchname,"%sDigits%d",GetName(),i+1);
          branch = TreeD()->GetBranch(branchname);
          branch->Fill();
        }
      } 
      else
      {
        TreeD()->Fill();
      }
    }
    
    if ( TreeD() && cGLT ) 
    {
      if ( IsDigitsBranchesInTree() ) 
      {
        sprintf(branchname,"%sLocalTrigger",GetName());
        branch = TreeD()->GetBranch(branchname); 
        branch->Fill();
	sprintf(branchname,"%sRegionalTrigger",GetName());
        branch = TreeD()->GetBranch(branchname);
        branch->Fill();
        sprintf(branchname,"%sGlobalTrigger",GetName());
        branch = TreeD()->GetBranch(branchname);
        branch->Fill();

      } 
      else
      {
        TreeD()->Fill();
      }
    }
  } // end of TreeD() handling.

  // Filling TreeS
  if ( TreeS() && cS) 
  {
    TreeS()->Fill();
  }

  // Filling TreeR
  
  if ( TreeR() && cRC && cTC )
  {
    TreeR()->Fill();
  }
  else
  {  
    if ( TreeR()  && cRC ) 
    {
      if ( IsTriggerBranchesInTree() ) 
      {
      // Branch per branch filling
        for (int i=0; i<AliMUONConstants::NTrackingCh(); i++) 
        {
          sprintf(branchname,"%sRawClusters%d",GetName(),i+1);
          branch = TreeR()->GetBranch(branchname);
          branch->Fill();
        }
      }
      else  
      {
        TreeR()->Fill();
      }
    }
    
    if ( TreeR()  && cTC) 
    {
      if (IsRawClusterBranchesInTree()) 
      {
        // Branch per branch filling
        sprintf(branchname,"%sLocalTrigger",GetName());
        branch = TreeR()->GetBranch(branchname); 
        branch->Fill();
	sprintf(branchname,"%sRegionalTrigger",GetName());
        branch = TreeR()->GetBranch(branchname); 
        branch->Fill();
        sprintf(branchname,"%sGlobalTrigger",GetName());
        branch = TreeR()->GetBranch(branchname);
        branch->Fill();
      }
      else
      {
        TreeR()->Fill();
      }
    }
  }

  // Filling TreeT
  
  if ( TreeT() && cRT && cRL )
  {
    TreeT()->Fill();
  }
  else
  {
    if ( TreeT() && cRT ) 
    {
      if (IsTriggerTrackBranchesInTree()) 
      {
        sprintf(branchname,"%sTrack",GetName());  
        branch = TreeT()->GetBranch(branchname);
        branch->Fill();
      }
      else 
      {
        TreeT()->Fill();
      }
    }

    if ( TreeT() && cRL ) 
    {
      if (IsTrackBranchesInTree()) 
      {
        sprintf(branchname,"%sTriggerTrack",GetName());  
        branch = TreeT()->GetBranch(branchname);
        branch->Fill();
      }    
      else 
      {
        TreeT()->Fill();
      }
    }
  }
}

//_____________________________________________________________________________
void AliMUONData::MakeBranch(Option_t* option)
{
/// Create Tree branches for the MUON.

  const Int_t kBufferSize = 4000;
  char branchname[30];
  
  //Setting Data Container
  SetDataContainer(option);  

  const char *cH   = strstr(option,"H");
  const char *cD   = strstr(option,"D");   // Digits branches in TreeD
  const char *cS   = strstr(option,"S");   // Digits branches in TreeS
  const char *cRC  = strstr(option,"RC");  // RawCluster branches in TreeR
  const char *cGLT = strstr(option,"GLT"); // Global and Local Trigger branches in TreeD
  const char *cTC  = strstr(option,"TC");   // global and local Trigger branches Copy in TreeR
  const char *cRT  = strstr(option,"RT");  // Reconstructed Track in TreeT
  const char *cRL  = strstr(option,"RL");  // Reconstructed Trigger Track in TreeT
                                           //const char *cRP  = strstr(option,"RP");  // Reconstructed Particle in TreeP
  
  TBranch * branch = 0x0;
  
  // Creating Branches for Hits
  if (TreeH() && cH) {
    sprintf(branchname,"%sHits",GetName());  
    branch = TreeH()->GetBranch(branchname);
    if (branch) {  
      AliInfo(Form("MakeBranch","Branch %s is already in tree.",branchname));
      return ;
    }
    branch = TreeH()->Branch(branchname,&fHits,kBufferSize);
    //Info("MakeBranch","Making Branch %s for hits \n",branchname);
  }  
  
  //Creating Branches for Digits
  TTree* treeD = 0x0;
  if ( cD || cGLT )
  {
    treeD = TreeD();
  }

  if ( treeD && cD ) 
  {
    // one branch for digits per chamber
    for (Int_t iDetectionPlane=0; iDetectionPlane<AliMUONConstants::NCh() ;iDetectionPlane++) 
    {
      sprintf(branchname,"%sDigits%d",GetName(),iDetectionPlane+1);
      branch = treeD->GetBranch(branchname);
      if (branch) 
      {  
        AliInfo(Form("Branch %s is already in tree.",branchname));
        return;
      }
      TClonesArray * digits = Digits(iDetectionPlane); 
      branch = treeD->Branch(branchname, &digits, kBufferSize,1);
    }
  }
  
  if ( treeD && cGLT ) 
  {
    //
    // one branch for global trigger
    //
    sprintf(branchname,"%sGlobalTrigger",GetName());
    branch = treeD->GetBranch(branchname);
    if (branch) 
    {  
      AliInfo(Form("Branch GlobalTrigger is already in treeD."));
      return ;
    }
    branch = treeD->Branch(branchname, &fGlobalTrigger, kBufferSize);

  //
    // one branch for regional trigger
    //  
    sprintf(branchname,"%sRegionalTrigger",GetName());
    branch = 0x0;
    branch = treeD->GetBranch(branchname);
    if (branch) 
    {  
      AliInfo(Form("Branch RegionalTrigger is already in treeD."));
      return;
    }
    branch = treeD->Branch(branchname, &fRegionalTrigger, kBufferSize);
  

    //
    // one branch for local trigger
    //  
    sprintf(branchname,"%sLocalTrigger",GetName());
    branch = 0x0;
    branch = treeD->GetBranch(branchname);
    if (branch) 
    {  
      AliInfo(Form("Branch LocalTrigger is already in treeD."));
      return;
    }
    branch = treeD->Branch(branchname, &fLocalTrigger, kBufferSize);
  }
    
  //Creating Branches for SDigits
  if (TreeS() && cS ) {
    // one branch for Sdigits per chamber
    for (Int_t iDetectionPlane=0; iDetectionPlane<AliMUONConstants::NCh() ;iDetectionPlane++) {
      sprintf(branchname,"%sSDigits%d",GetName(),iDetectionPlane+1);
      branch = 0x0;
      branch = TreeS()->GetBranch(branchname);
      if (branch) {  
        AliInfo(Form("Branch %s is already in tree.",branchname));
        return;
      }
      TClonesArray * sdigits = SDigits(iDetectionPlane); 
      branch = TreeS()->Branch(branchname, &sdigits, kBufferSize,1);
      //Info("MakeBranch","Making Branch %s for sdigits in detection plane %d\n",branchname,iDetectionPlane+1);
    }
  }
  
  if (TreeR() && cRC ) {
    //  one branch for raw clusters per tracking detection plane
    //        
    Int_t i; 
    for (i=0; i<AliMUONConstants::NTrackingCh() ;i++) {
      sprintf(branchname,"%sRawClusters%d",GetName(),i+1);	
      branch = 0x0;
      branch = TreeR()->GetBranch(branchname);
      if (branch) {  
        AliInfo(Form("Branch %s is already in tree.",branchname));
        return;
      }
      branch = TreeR()->Branch(branchname, &((*fRawClusters)[i]),kBufferSize);
      //Info("MakeBranch","Making Branch %s for rawcluster in detection plane %d\n",branchname,i+1);
    }
  }
  
  if (TreeR() && cTC ) {
    //
    // one branch for global trigger
    //
    sprintf(branchname,"%sGlobalTrigger",GetName());
    branch = 0x0;
    branch = TreeR()->GetBranch(branchname);
    if (branch) {  
      AliInfo(Form("Branch GlobalTrigger is already in treeR."));
      return ;
    }
    branch = TreeR()->Branch(branchname, &fGlobalTrigger, kBufferSize);
    //Info("MakeBranch", "Making Branch %s for Global Trigger\n",branchname);

  //
    // one branch for regional trigger
    //  
    sprintf(branchname,"%sRegionalTrigger",GetName());
    branch = 0x0;
    branch = TreeR()->GetBranch(branchname);
    if (branch) {  
      AliInfo(Form("Branch RegionalTrigger is already in treeR."));
      return;
    }
    branch = TreeR()->Branch(branchname, &fRegionalTrigger, kBufferSize);
     
    //
    // one branch for local trigger
    //  
    sprintf(branchname,"%sLocalTrigger",GetName());
    branch = 0x0;
    branch = TreeR()->GetBranch(branchname);
    if (branch) {  
      AliInfo(Form("Branch LocalTrigger is already in treeR."));
      return;
    }
    branch = TreeR()->Branch(branchname, &fLocalTrigger, kBufferSize);
    //Info("MakeBranch", "Making Branch %s for Global Trigger\n",branchname);  
  }
  
  if (TreeT() && cRT ) {
    sprintf(branchname,"%sTrack",GetName());  
    branch = TreeT()->GetBranch(branchname);
    if (branch) {  
      AliInfo(Form("Branch %s is already in tree.",GetName()));
      return ;
    }
    branch = TreeT()->Branch(branchname,&fRecTracks,kBufferSize);
    //Info("MakeBranch","Making Branch %s for tracks \n",branchname);
  }  
  // trigger tracks
  if (TreeT() && cRL ) {
    sprintf(branchname,"%sTriggerTrack",GetName());  
    branch = TreeT()->GetBranch(branchname);
    if (branch) {  
      AliInfo(Form("Branch %s is already in tree.",GetName()));
      return ;
    }
    branch = TreeT()->Branch(branchname,&fRecTriggerTracks,kBufferSize);
    //Info("MakeBranch","Making Branch %s for trigger tracks \n",branchname);
  }  
}
//____________________________________________________________________________
TClonesArray*  AliMUONData::RawClusters(Int_t DetectionPlane)
{
/// Getting Raw Clusters

  if (fRawClusters) 
    return ( (TClonesArray*) fRawClusters->At(DetectionPlane) );
  else
    return NULL;
}

//____________________________________________________________________________
TClonesArray*  
AliMUONData::LocalTrigger() const
{
/// Getting local trigger

  return fLocalTrigger;
}

//____________________________________________________________________________
TClonesArray*  
AliMUONData::RegionalTrigger() const
{
/// Getting regional trigger

  return fRegionalTrigger;
}

//____________________________________________________________________________
Int_t          
AliMUONData::GetNtracks() const      
{
/// Get number of entries in hits three

  Int_t ntrk = 0;
  if (fLoader && fLoader->TreeH())
    ntrk = (Int_t) fLoader->TreeH()->GetEntries();
  return ntrk;
}

//____________________________________________________________________________
void
AliMUONData::GetDigits() const 
{
/// Load the digits from TreeD for the current event.

  Int_t event = fLoader->GetRunLoader()->GetEventNumber();
  if ( fCurrentEvent != event )
  {
    if (fLoader->TreeD()) {
      fLoader->TreeD()->GetEvent(0);
      fCurrentEvent = event;
    }
  }
}

//____________________________________________________________________________
TClonesArray*  
AliMUONData::GlobalTrigger() const
{
/// Return the global trigger 

  return fGlobalTrigger;
}

//____________________________________________________________________________
void AliMUONData::ResetDigits()
{
/// Reset number of digits and the digits array for this detector

    if (fDigits == 0x0) return;
    for ( int i=0;i<AliMUONConstants::NCh();i++ ) {
      if ((*fDigits)[i])    ((TClonesArray*)fDigits->At(i))->Clear("C");
      if (fNdigits)  fNdigits[i]=0;
    }
}
//____________________________________________________________________________
void AliMUONData::ResetSDigits()
{
/// Reset number of Sdigits and the Sdigits array for this detector

    if (fSDigits == 0x0) return;
    for ( int i=0;i<AliMUONConstants::NCh();i++ ) {
      if ((*fSDigits)[i])    ((TClonesArray*)fSDigits->At(i))->Clear();
      if (fNSdigits)  fNSdigits[i]=0;
    }
}
//______________________________________________________________________________
void AliMUONData::ResetHits()
{
/// Reset number of clusters and the cluster array for this detector

  fNhits   = 0;
  if (fHits) fHits->Clear();
}
//_______________________________________________________________________________
void AliMUONData::ResetRawClusters()
{
/// Reset number of raw clusters and the raw clust array for this detector

  for ( int i=0;i<AliMUONConstants::NTrackingCh();i++ ) {
    if ((*fRawClusters)[i])    ((TClonesArray*)fRawClusters->At(i))->Clear();
    if (fNrawclusters)  fNrawclusters[i]=0;
  }
}
//_______________________________________________________________________________
void AliMUONData::ResetTrigger()
{
/// Reset Local and Global Trigger 

  fNglobaltrigger = 0;
  if (fGlobalTrigger) fGlobalTrigger->Clear();
  fNregionaltrigger = 0;
  if (fRegionalTrigger) fRegionalTrigger->Clear();
  fNlocaltrigger = 0;
  if (fLocalTrigger) fLocalTrigger->Clear();

}
//____________________________________________________________________________
void AliMUONData::ResetRecTracks()
{
/// Reset tracks information

  fNrectracks = 0;
  if (fRecTracks) fRecTracks->Delete(); // necessary to delete in case of memory allocation
}
//____________________________________________________________________________
void AliMUONData::ResetRecTriggerTracks()
{
/// Reset tracks information

  fNrectriggertracks = 0;
  if (fRecTriggerTracks) fRecTriggerTracks->Delete(); // necessary to delete in case of memory allocation
}
//____________________________________________________________________________
void AliMUONData::SetDataContainer(Option_t* option)
{
/// Setting data containers of muon data
  const char *cH   = strstr(option,"H");
  const char *cD   = strstr(option,"D");   // Digits
  const char *cS   = strstr(option,"S");   // SDigits
  const char *cRC  = strstr(option,"RC");  // RawCluster
  const char *cGLT = strstr(option,"GLT"); // Global and Local Trigger
  const char *cTC = strstr(option,"TC");   // global and local Trigger 
  const char *cRT  = strstr(option,"RT");  // Reconstructed Tracks
  const char *cRL  = strstr(option,"RL");  // Reconstructed Trigger Tracks
                                           //const char *cRP  = strstr(option,"RP");  // Reconstructed Particles  
  AliDebug(1,Form("option=%s",option));
  //
  // Clones array for hits
  if ( cH ) {
    if (fHits == 0x0) {
      fHits     = new TClonesArray("AliMUONHit",1000);
    }
    ResetHits();
  }
  
  //
  // ObjArray of ClonesArrays for Digits
  if ( cD ) {      
    if (fDigits == 0x0 ) {
      fDigits = new TObjArray(AliMUONConstants::NCh());
      fNdigits= new Int_t[AliMUONConstants::NCh()];
      for (Int_t i=0; i<AliMUONConstants::NCh() ;i++) {
	TClonesArray * tca = new TClonesArray("AliMUONDigit",10000);
	tca->SetOwner();
        fDigits->AddAt(tca,i); 
        fNdigits[i]=0;
      }
    } 
    else {
      AliDebug(1,Form("fDigits already there = %p",fSDigits));
    }
    ResetDigits();
  }

  //
  // ClonesArrays for Trigger
  if ( cGLT ) { 
    if (fLocalTrigger == 0x0) {
      fLocalTrigger  = new TClonesArray("AliMUONLocalTrigger",234);
    }
    if (fRegionalTrigger == 0x0) {
      fRegionalTrigger  = new TClonesArray("AliMUONRegionalTrigger",16);
    }
    if (fGlobalTrigger== 0x0) {
      fGlobalTrigger = new TClonesArray("AliMUONGlobalTrigger",1); 
    }
    ResetTrigger();
  }
    
  //
  // Container for Sdigits
  if (cS) {
    if (fSDigits == 0x0) { 
      AliDebug(1,"Creating fSDigits TObjArray");
      fSDigits = new TObjArray(AliMUONConstants::NCh());
      fNSdigits= new Int_t[AliMUONConstants::NCh()];
      for (Int_t i=0; i<AliMUONConstants::NCh() ;i++) {
	TClonesArray* a = new TClonesArray("AliMUONDigit",10000);
	a->SetOwner();
	fSDigits->AddAt(a,i);
	AliDebug(1,Form("fSDigits[%d]=%p",i,a));
        fNSdigits[i]=0;
      }
    }
    else {
      AliDebug(1,Form("fSDigits already there = %p",fSDigits));
    }
    ResetSDigits();
  }
  
  //
  // Containers for rawclusters, globaltrigger and local trigger tree
  if (cRC ) {
    if (fRawClusters == 0x0) {
      fRawClusters = new TObjArray(AliMUONConstants::NTrackingCh());
      fNrawclusters= new Int_t[AliMUONConstants::NTrackingCh()];
      for (Int_t i=0; i<AliMUONConstants::NTrackingCh();i++) {
	TClonesArray* tca = new TClonesArray("AliMUONRawCluster",10000);
	tca->SetOwner();
        fRawClusters->AddAt(tca,i); 
        fNrawclusters[i]=0;
      }
    }
    // ResetRawClusters(); 
    // It breaks the correct functioning of the combined reconstruction (AZ)
    
  }
  if (cTC ) {
    if (fLocalTrigger == 0x0) {
      fLocalTrigger  = new TClonesArray("AliMUONLocalTrigger",234);
    }
   if (fRegionalTrigger == 0x0) {
      fRegionalTrigger  = new TClonesArray("AliMUONRegionalTrigger",16);
    }
    if (fGlobalTrigger== 0x0) {
      fGlobalTrigger = new TClonesArray("AliMUONGlobalTrigger",1); 
    }
    // ResetTrigger(); 
    // This is not necessary here since trigger info ins copied from digits info on flight to RecPoint output
  }

  //
  // Containers for rectracks and rectrigger tracks
  if ( cRT ) {
    if (fRecTracks == 0x0)  {
      fRecTracks  = new TClonesArray("AliMUONTrack",100);
    }
    ResetRecTracks();
  }
  if (cRL) {
    if (fRecTriggerTracks == 0x0 && cRL)  {
      fRecTriggerTracks  = new TClonesArray("AliMUONTriggerTrack",100);
    }
    ResetRecTriggerTracks();
  }  
}

//____________________________________________________________________________
void AliMUONData::SetTreeAddress(Option_t* option)
{
  // Setting Data containers
  SetDataContainer(option);

/// Setting Addresses to the events trees

  const char *cH   = strstr(option,"H");
  const char *cD   = strstr(option,"D");   // Digits branches in TreeD
  const char *cS   = strstr(option,"S");   // SDigits branches in TreeS
  const char *cRC  = strstr(option,"RC");  // RawCluster branches in TreeR
  const char *cGLT = strstr(option,"GLT"); // Global and Local Trigger branches in TreeD
  const char *cTC = strstr(option,"TC");   // global and local Trigger branches Copy in TreeR
  const char *cRT  = strstr(option,"RT");  // Reconstructed Track in TreeT
  const char *cRL  = strstr(option,"RL");  // Reconstructed Trigger Track in TreeT
                                           //const char *cRP  = strstr(option,"RP");  // Reconstructed Particle in TreeP
  
  // Set branch address for the Hits, Digits, RawClusters, GlobalTrigger and LocalTrigger Tree.
  char branchname[30];
  TBranch * branch = 0x0;
  
  AliDebug(1,Form("option=%s",option));
  //
  // Branch address for hit tree
  if (TreeH() && fHits && cH) {
    sprintf(branchname,"%sHits",GetName());  
    branch = TreeH()->GetBranch(branchname);
    if (branch) {
      //      Info("SetTreeAddress","(%s) Setting for Hits",GetName());
      branch->SetAddress(&fHits);
    }
    else { //can be invoked before branch creation
      //AliWarning(Form("(%s) Failed for Hits. Can not find branch in tree.",GetName()));
    }
  }
  
  //
  // Branch address for digit tree
  if (TreeD() && fDigits && cD) {
    for (int i=0; i<AliMUONConstants::NCh(); i++) {
      sprintf(branchname,"%sDigits%d",GetName(),i+1);
      if (fDigits) {
        branch = TreeD()->GetBranch(branchname);
        TClonesArray * digits = Digits(i);
        if (branch) {
          branch->SetAddress( &digits );
        }
        else AliWarning(Form("(%s) Failed for Digits Detection plane %d. Can not find branch in tree.",GetName(),i));
      }
    }
  }
  if ( TreeD()  && fLocalTrigger && cGLT) {
    sprintf(branchname,"%sLocalTrigger",GetName());
    branch = TreeD()->GetBranch(branchname);
    if (branch) branch->SetAddress(&fLocalTrigger);
    else AliWarning(Form("(%s) Failed for LocalTrigger. Can not find branch in treeD.",GetName()));
  }
 if ( TreeD()  && fRegionalTrigger && cGLT) {
    sprintf(branchname,"%sRegionalTrigger",GetName());
    branch = TreeD()->GetBranch(branchname);
    if (branch) branch->SetAddress(&fRegionalTrigger);
    else AliWarning(Form("(%s) Failed for RegionalTrigger. Can not find branch in treeD.",GetName()));
  }
  if ( TreeD() && fGlobalTrigger && cGLT) {
    sprintf(branchname,"%sGlobalTrigger",GetName());
    branch = TreeD()->GetBranch(branchname);
    if (branch) branch->SetAddress(&fGlobalTrigger);
    else AliWarning(Form("(%s) Failed for GlobalTrigger. Can not find branch in treeD.",GetName()));
  }
  
  //
  // Branch address for Sdigit tree
  if (TreeS() && fSDigits && cS) {
    AliDebug(1,"Setting branch addresses");
    for (int i=0; i<AliMUONConstants::NCh(); i++) {
      sprintf(branchname,"%sSDigits%d",GetName(),i+1);
      if (fSDigits) {
        AliDebug(1,Form("TreeS=%p for ich=%d branchname=%s",
                        TreeS(),i,branchname));
        branch = TreeS()->GetBranch(branchname);
        TClonesArray * sdigits = SDigits(i);
        if (branch) branch->SetAddress( &sdigits );
        else AliWarning(Form("(%s) Failed for SDigits Detection plane %d. Can not find branch in tree.",GetName(),i));
      }
    }
  }
  
  //
  // Branch address for rawclusters, globaltrigger and local trigger tree
  if ( TreeR()  && fRawClusters && cRC && !strstr(cRC,"RCC")) {
    for (int i=0; i<AliMUONConstants::NTrackingCh(); i++) {
      sprintf(branchname,"%sRawClusters%d",GetName(),i+1);
      if (fRawClusters) {
        branch = TreeR()->GetBranch(branchname);
        if (branch) branch->SetAddress( &((*fRawClusters)[i]) );
        else AliWarning(Form("(%s) Failed for RawClusters Detection plane %d. Can not find branch in tree.",GetName(),i));
      }
    }
  }
  if ( TreeR()  && fLocalTrigger && cTC) {
    sprintf(branchname,"%sLocalTrigger",GetName());
    branch = TreeR()->GetBranch(branchname);
    if (branch) branch->SetAddress(&fLocalTrigger);
    else AliWarning(Form("(%s) Failed for LocalTrigger. Can not find branch in treeR.",GetName()));
  }
  if ( TreeR()  && fRegionalTrigger && cTC) {
    sprintf(branchname,"%sRegionalTrigger",GetName());
    branch = TreeR()->GetBranch(branchname);
    if (branch) branch->SetAddress(&fRegionalTrigger);
    else AliWarning(Form("(%s) Failed for RegionalTrigger. Can not find branch in treeR.",GetName()));
  }
  if ( TreeR() && fGlobalTrigger && cTC) {
    sprintf(branchname,"%sGlobalTrigger",GetName());
    branch = TreeR()->GetBranch(branchname);
    if (branch) branch->SetAddress(&fGlobalTrigger);
    else AliWarning(Form("(%s) Failed for GlobalTrigger. Can not find branch in treeR.",GetName()));
  }

  // Rec Trakcs
  if ( TreeT() && fRecTracks && cRT ) {
    sprintf(branchname,"%sTrack",GetName());  
    branch = TreeT()->GetBranch(branchname);
    if (branch) branch->SetAddress(&fRecTracks);
    else AliWarning(Form("(%s) Failed for Tracks. Can not find branch in tree.",GetName()));
  }
  // Trigger tracks
  if ( TreeT() && fRecTriggerTracks && cRL ) {
    sprintf(branchname,"%sTriggerTrack",GetName());  
    branch = TreeT()->GetBranch(branchname);
    if (branch) branch->SetAddress(&fRecTriggerTracks);
    else AliWarning(Form("(%s) Failed for Trigger Tracks. Can not find branch in tree.",GetName()));
  }
}

//_____________________________________________________________________________
void
AliMUONData::Print(Option_t* opt) const
{
/// Dump object on screen

  TString options(opt);
  options.ToUpper();
  
  if ( options.Contains("D") )
  {
    for ( Int_t ich = 0; ich < AliMUONConstants::NCh(); ++ich)
    {
      TClonesArray* digits = Digits(ich);
      Int_t ndigits = digits->GetEntriesFast();
      for ( Int_t id = 0; id < ndigits; ++id )
      {
        AliMUONDigit* digit = 
          static_cast<AliMUONDigit*>(digits->UncheckedAt(id));
        digit->Print();
      }
    }
  }
  
  if ( options.Contains("S") )
  {
    for ( Int_t ich = 0; ich < AliMUONConstants::NCh(); ++ich)
    {
      TClonesArray* digits = SDigits(ich);
      Int_t ndigits = digits->GetEntriesFast();
      for ( Int_t id = 0; id < ndigits; ++id )
      {
        AliMUONDigit* digit = 
        static_cast<AliMUONDigit*>(digits->UncheckedAt(id));
        digit->Print();
      }
    }
  }
  
}

//_____________________________________________________________________________
void 
AliMUONData::DumpKine(Int_t event2Check)
{
/// Dump kinematics

  fRunLoader->LoadKinematics("READ");

  Int_t nevents = fRunLoader->GetNumberOfEvents();
  for (Int_t ievent=0; ievent<nevents; ievent++) {  // Event loop
    if ( event2Check != 0 ) ievent=event2Check;

    // Getting event ievent
    fRunLoader->GetEvent(ievent); 

    // Stack of particle for this event
    AliStack* stack = fRunLoader->Stack();

    Int_t nparticles = (Int_t) fRunLoader->Stack()->GetNtrack();
    printf(">>> Event %d, Number of particles is %d \n", ievent, nparticles);

    for (Int_t iparticle=0; iparticle<nparticles; iparticle++) {
      stack->Particle(iparticle)->Print("");  
    }
    if (event2Check!=0) ievent=nevents;
  }
  fRunLoader->UnloadKinematics();
}


//_____________________________________________________________________________
void 
AliMUONData::DumpHits(Int_t event2Check, Option_t* opt)
{
/// Dump hits

  fLoader->LoadHits("READ");

  // Event loop
  Int_t nevents = fRunLoader->GetNumberOfEvents();
  for (Int_t ievent=0; ievent<nevents; ievent++) {
    if (event2Check!=0) ievent=event2Check;
    printf(">>> Event %d \n",ievent);

    // Getting event ievent
    fRunLoader->GetEvent(ievent); 
    SetTreeAddress("H");

    // Track loop
    Int_t ntracks = (Int_t) GetNtracks();
    for (Int_t itrack=0; itrack<ntracks; itrack++) {
      //Getting List of Hits of Track itrack
      GetTrack(itrack);

      Int_t nhits = (Int_t) Hits()->GetEntriesFast();
      printf(">>> Track %d, Number of hits %d \n",itrack,nhits);
      for (Int_t ihit=0; ihit<nhits; ihit++) {
	AliMUONHit* mHit = static_cast<AliMUONHit*>(Hits()->At(ihit));
	mHit->Print(opt);
      }
      ResetHits();
    }
    if (event2Check!=0) ievent=nevents;
  }
  fLoader->UnloadHits();
}

//_____________________________________________________________________________
void 
AliMUONData::DumpDigits(Int_t event2Check, Option_t* opt)
{
/// Dump digits

  fLoader->LoadDigits("READ");
  
  // Event loop
  Int_t firstEvent = 0;
  Int_t lastEvent = fRunLoader->GetNumberOfEvents()-1;
  if ( event2Check != 0 ) {
    firstEvent = event2Check;
    lastEvent = event2Check;
  }  
  
  for ( Int_t ievent = firstEvent; ievent <= lastEvent; ++ievent ) {
    printf(">>> Event %d \n",ievent);
    fRunLoader->GetEvent(ievent);

    AliMUONDataIterator it(this, "digit", AliMUONDataIterator::kTrackingChambers);
    AliMUONDigit* digit;
 
     while ( ( digit = (AliMUONDigit*)it.Next() ) )
     {
       digit->Print(opt);
     }
  } 
  fLoader->UnloadDigits();
}

//_____________________________________________________________________________
void 
AliMUONData::DumpSDigits(Int_t event2Check, Option_t* opt)
{
/// Dump SDigits

  fLoader->LoadSDigits("READ");
  
  // Event loop
  Int_t nevents = fRunLoader->GetNumberOfEvents();
  for (Int_t ievent=0; ievent<nevents; ievent++) {
    if (event2Check!=0) ievent=event2Check;
    printf(">>> Event %d \n",ievent);

    // Getting event ievent
    fRunLoader->GetEvent(ievent);
    SetTreeAddress("S");
    GetSDigits();

    // Loop on chambers
    Int_t nchambers = AliMUONConstants::NCh(); ;
    for (Int_t ichamber=0; ichamber<nchambers; ichamber++) {
      TClonesArray* digits = SDigits(ichamber);

      // Loop on Sdigits
      Int_t ndigits = (Int_t)digits->GetEntriesFast();
      for (Int_t idigit=0; idigit<ndigits; idigit++) {
        AliMUONDigit* mDigit = static_cast<AliMUONDigit*>(digits->At(idigit));
        mDigit->Print(opt);
      }
    }
    ResetSDigits();
    if (event2Check!=0) ievent=nevents;
  }
  fLoader->UnloadSDigits();
}

//_____________________________________________________________________________
void 
AliMUONData::DumpRecPoints(Int_t event2Check, Option_t* opt) 
{
/// Dump rec points

  fLoader->LoadRecPoints("READ");

  // Event loop
  Int_t nevents = fRunLoader->GetNumberOfEvents();
  for (Int_t ievent=0; ievent<nevents; ievent++) {
    if (event2Check!=0) ievent=event2Check;
    printf(">>> Event %d \n",ievent);

    // Getting event ievent
    fRunLoader->GetEvent(ievent);
    Int_t nchambers = AliMUONConstants::NTrackingCh();
    SetTreeAddress("RC,TC"); 
    GetRawClusters();

    // Loop on chambers
    for (Int_t ichamber=0; ichamber<nchambers; ichamber++) {
      char branchname[30];    
      sprintf(branchname,"MUONRawClusters%d",ichamber+1);
      //printf(">>>  branchname %s\n",branchname);

      // Loop on rec points
      Int_t nrecpoints = (Int_t) RawClusters(ichamber)->GetEntriesFast();
      // printf(">>> Chamber %2d, Number of recpoints = %6d \n",ichamber+1, nrecpoints);
      for (Int_t irecpoint=0; irecpoint<nrecpoints; irecpoint++) {
	AliMUONRawCluster* mRecPoint = static_cast<AliMUONRawCluster*>(RawClusters(ichamber)->At(irecpoint));
	mRecPoint->Print(opt);
      }
    }
    ResetRawClusters();
    if (event2Check!=0) ievent=nevents;
  }
  fLoader->UnloadRecPoints();
}


//_____________________________________________________________________________
void 
AliMUONData::DumpRecTrigger(Int_t event2Check, 
                            Int_t write, Bool_t readFromRP)
{
/// Reads and dumps trigger objects from MUON.RecPoints.root

  TClonesArray * globalTrigger;
  TClonesArray * localTrigger;
  
  // Do NOT print out all the info if the loop runs over all events 
  Int_t printout = (event2Check == 0 ) ? 0 : 1 ;  

  // Book a ntuple for more detailled studies
  TNtuple *tupleGlo = new TNtuple("TgtupleGlo","Global Trigger Ntuple","ev:global:slpt:shpt:uplpt:uphpt:lplpt:lplpt");
  TNtuple *tupleLoc = new TNtuple("TgtupleLoc","Local Trigger Ntuple","ev:LoCircuit:LoStripX:LoDev:StripY:LoLpt:LoHpt:y11:y21:x11");

  // counters
  Int_t sLowpt=0,sHighpt=0;
  Int_t uSLowpt=0,uSHighpt=0;
  Int_t lSLowpt=0,lSHighpt=0;

  AliMUONTriggerCrateStore* crateManager = new AliMUONTriggerCrateStore();   
  crateManager->ReadFromFile();

  AliMUONGeometryTransformer* transformer = new AliMUONGeometryTransformer(kFALSE);
  transformer->ReadGeometryData("volpath.dat", "geometry.root");

  TClonesArray*  triggerCircuit = new TClonesArray("AliMUONTriggerCircuit", 234);

  for (Int_t i = 0; i < AliMUONConstants::NTriggerCircuit(); i++)  {
      AliMUONTriggerCircuit* c = new AliMUONTriggerCircuit();
      c->SetTransformer(transformer);
      c->Init(i,*crateManager);
      TClonesArray& circuit = *triggerCircuit;
      new(circuit[circuit.GetEntriesFast()])AliMUONTriggerCircuit(*c);
      delete c;
  }
  
  Char_t fileName[30];
  if (!readFromRP) {
      AliInfoStream() << " reading from digits \n";
      fLoader->LoadDigits("READ");
      sprintf(fileName,"TriggerCheckFromDigits.root");
  } else {
      AliInfoStream() << " reading from RecPoints \n";
      fLoader->LoadRecPoints("READ");
      sprintf(fileName,"TriggerCheckFromRP.root");
  }

  
  AliMUONGlobalTrigger *gloTrg(0x0);
  AliMUONLocalTrigger *locTrg(0x0);

  Int_t nevents = fRunLoader->GetNumberOfEvents();
  for (Int_t ievent=0; ievent<nevents; ievent++) {
    if (event2Check!=0) ievent=event2Check;
    if (ievent%100==0 || event2Check) 
      AliInfoStream() << "Processing event " << ievent << endl;
    fRunLoader->GetEvent(ievent);
    
    if (!readFromRP) {
	SetTreeAddress("D,GLT"); 
	GetTriggerD();
    } else {    
	SetTreeAddress("RC,TC"); 
	GetTrigger();
    }

    globalTrigger = GlobalTrigger();
    localTrigger = LocalTrigger();
    
    Int_t nglobals = (Int_t) globalTrigger->GetEntriesFast(); // should be 1
    Int_t nlocals  = (Int_t) localTrigger->GetEntriesFast(); // up to 234
    if (printout) printf("###################################################\n");
    if (printout) printf("event %d nglobal %d nlocal %d \n",ievent,nglobals,nlocals);

    for (Int_t iglobal=0; iglobal<nglobals; iglobal++) { // Global Trigger
      gloTrg = static_cast<AliMUONGlobalTrigger*>(globalTrigger->At(iglobal));
      
      sLowpt+=gloTrg->SingleLpt() ;
      sHighpt+=gloTrg->SingleHpt() ;
      uSLowpt+=gloTrg->PairUnlikeLpt(); 
      uSHighpt+=gloTrg->PairUnlikeHpt();
      lSLowpt+=gloTrg->PairLikeLpt(); 
      lSHighpt+=gloTrg->PairLikeHpt();
      
      if (printout) gloTrg->Print("full");

    } // end of loop on Global Trigger

    for (Int_t ilocal=0; ilocal<nlocals; ilocal++) { // Local Trigger
      locTrg = static_cast<AliMUONLocalTrigger*>(localTrigger->At(ilocal));

      Bool_t xTrig=kFALSE;
      Bool_t yTrig=kFALSE;

      if ( locTrg->LoSdev()==1 && locTrg->LoDev()==0 && 
	   locTrg->LoStripX()==0) xTrig=kFALSE; // no trigger in X
      else xTrig=kTRUE;                         // trigger in X
      if (locTrg->LoTrigY()==1 && 
	  locTrg->LoStripY()==15 ) yTrig = kFALSE; // no trigger in Y
      else yTrig = kTRUE;                          // trigger in Y

      if (xTrig && yTrig) { // make Trigger Track if trigger in X and Y
	  
	  if (printout) locTrg->Print("full");
	  
	  AliMUONTriggerCircuit* circuit = (AliMUONTriggerCircuit*)triggerCircuit->At(locTrg->LoCircuit()-1); 
	  
	  tupleLoc->Fill(ievent,locTrg->LoCircuit(),locTrg->LoStripX(),locTrg->LoDev(),locTrg->LoStripY(),locTrg->LoLpt(),locTrg->LoHpt(),circuit->GetY11Pos(locTrg->LoStripX()),circuit->GetY21Pos(locTrg->LoStripX()+locTrg->LoDev()+1),circuit->GetX11Pos(locTrg->LoStripY()));
      }
      
    } // end of loop on Local Trigger
    
    // fill ntuple
    tupleGlo->Fill(ievent,nglobals,gloTrg->SingleLpt(),gloTrg->SingleHpt(),gloTrg->PairUnlikeLpt(),gloTrg->PairUnlikeHpt(),gloTrg->PairLikeLpt(),gloTrg->PairLikeHpt());
    
    ResetTrigger();
    if (event2Check!=0) ievent=nevents;
  } // end loop on event  
  
  // Print out summary if loop ran over all event
  if (!event2Check){

    printf("\n");
    printf("=============================================\n");
    printf("================  SUMMARY  ==================\n");
    printf("\n");
    printf("Total number of events processed %d \n", (event2Check==0) ? nevents : 1);
    printf("\n");
    printf(" Global Trigger output       Low pt  High pt\n");
    printf(" number of Single           :\t");
    printf("%i\t%i\t",sLowpt,sHighpt);
    printf("\n");
    printf(" number of UnlikeSign pair  :\t"); 
    printf("%i\t%i\t",uSLowpt,uSHighpt);
    printf("\n");
    printf(" number of LikeSign pair    :\t");  
    printf("%i\t%i\t",lSLowpt,lSHighpt);
    printf("\n");
    printf("=============================================\n");
    fflush(stdout);
  }
  
  if (write){
      TFile *myFile = new TFile(fileName, "RECREATE");
      tupleGlo->Write();
      tupleLoc->Write();
      myFile->Close();
  }

  fLoader->UnloadRecPoints();

  delete crateManager;
  delete transformer;
  delete triggerCircuit;
  
}
