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

// 
//  This class analyses TPC cosmics data from the ESD and the ESDfriend
//
//  Authors: Jan.Fiete.Grosse-Oetringhaus@cern.ch, Claus.Jorgensen@cern.ch
//

#include "AliROCESDAnalysisSelector.h"

#include <AliLog.h>
#include <AliESD.h>
#include <AliESDfriend.h>
#include <../TPC/AliTPCclusterMI.h>
#include <../TPC/AliTPCseed.h>

#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TObjArray.h>
#include <TTimeStamp.h>

#include "TPC/AliTPCClusterHistograms.h"

extern TSystem* gSystem;

ClassImp(AliROCESDAnalysisSelector)

AliROCESDAnalysisSelector::AliROCESDAnalysisSelector() :
  AliSelector(),
  fESDfriend(0),
  fObjectsToSave(0),
  fMinNumberOfRowsIsTrack(0)
{
  //
  // Constructor. Initialization of pointers
  //
  fMinNumberOfRowsIsTrack = 5;

  fObjectsToSave = new TObjArray();
  
  for (Int_t i=0; i<kTPCHists; i++)
    fClusterHistograms[i] = 0;
}

AliROCESDAnalysisSelector::~AliROCESDAnalysisSelector()
{
  //
  // Destructor
  //
}

void AliROCESDAnalysisSelector::SlaveBegin(TTree* tree)
{
  //
  
  AliSelector::SlaveBegin(tree);
} 

void AliROCESDAnalysisSelector::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the branch addresses of the tree
  // will be set. It is normaly not necessary to make changes to the
  // generated code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running with PROOF.

  AliSelector::Init(tree);
  
  printf("Init called %p\n", (void*) fESDfriend);

  // Set branch address
  if (tree) 
  {
    tree->SetBranchAddress("ESDfriend", &fESDfriend);
  
    tree->SetBranchStatus("*", 0);
    tree->SetBranchStatus("fTracks.*", 1);
    tree->SetBranchStatus("fTimeStamp", 1);
    //tree->SetBranchStatus("fTracks.fCalibContainer", 0);
  }

  if (fESDfriend != 0)
    AliDebug(AliLog::kInfo, "INFO: Found ESDfriend branch in chain.");
}

Bool_t AliROCESDAnalysisSelector::Process(Long64_t entry)
{
  //
  // Implement your analysis here. Do not forget to call the parent class Process by
  // if (AliSelector::Process(entry) == kFALSE)
  //   return kFALSE;
  //

  if (AliSelector::Process(entry) == kFALSE)
    return kFALSE;

  // Check prerequisites
  if (!fESD)
  {
    AliDebug(AliLog::kError, "ESD branch not available");
    return kFALSE;
  }

  // Check prerequisites
  if (!fESDfriend)
  {
    AliDebug(AliLog::kError, "ESDfriend branch not available");
    return kFALSE;
  }

  if (fESD->GetNumberOfTracks() != fESDfriend->GetNumberOfTracks())
  {
    AliDebug(AliLog::kError, Form("Event %lld: Number of tracks differ between ESD (%d) and ESDfriend (%d)! Skipping event!\n", entry, fESD->GetNumberOfTracks(), fESDfriend->GetNumberOfTracks()));
    return kFALSE;
  }  

  fESD->SetESDfriend(fESDfriend);

  Int_t flag = ProcessEvent(entry, kFALSE);
  if (flag == 1)
    ProcessEvent(entry, kTRUE);

  // TODO This should not be needed, the TTree::GetEntry() should take care of this, maybe because it has a reference member, to be analyzed
  // if the ESDfriend is not deleted we get a major memory leak
  // here the esdfriend seems to be also deleted, very weird behaviour....

  delete fESD;
  fESD = 0;
  
  //delete fESDfriend;
  //fESDfriend = 0;

  return kTRUE;
}

Int_t AliROCESDAnalysisSelector::ProcessEvent(Long64_t entry, Bool_t detailedHistogram)
{
  //
  // Looping over tracks and clusters in event and filling histograms 
  //
  // - if detailedHistogram = kTRUE special histograms are saved (in fObjectsToSave)
  // - the method returns 
  //   1 : if a "flash" is detected something special in this event
  //   

  // save maximum 50 objects
  if (detailedHistogram) 
    if (fObjectsToSave->GetEntries() > 50) 
      return 0;
      
  // for saving single events
  AliTPCClusterHistograms* clusterHistograms[kTPCSectors];
  for (Int_t i=0; i<kTPCSectors; i++) 
    clusterHistograms[i] = 0;  

  Bool_t intToReturn = 0;

  Int_t nTracks = fESD->GetNumberOfTracks();
  
  Int_t nSkippedSeeds = 0;
  //Int_t nSkippedTracks = 0;

  // for "flash" detection
  Int_t nClusters = 0;
  Float_t clusterQtotSumVsTime[250];  
  for (Int_t z=0; z<250; z++)
    clusterQtotSumVsTime[z] = 0;
  
  // loop over esd tracks
  for (Int_t t=0; t<nTracks; t++)
  {
    AliESDtrack* esdTrack = dynamic_cast<AliESDtrack*> (fESD->GetTrack(t));
    if (!esdTrack)
    {
      AliDebug(AliLog::kError, Form("ERROR: Could not retrieve track %d.", t));
      continue;
    }
    
    AliESDfriendTrack* friendtrack = const_cast<AliESDfriendTrack*> (dynamic_cast<const AliESDfriendTrack*> (esdTrack->GetFriendTrack()));
    if (!friendtrack)
    {
      AliDebug(AliLog::kError, Form("ERROR: Could not retrieve friend of track %d.", t));
      continue;
    }
    
    const AliTPCseed* seed = dynamic_cast<const AliTPCseed*> (friendtrack->GetCalibObject(0));
    if (!seed)
    {
      AliDebug(AliLog::kDebug, Form("ERROR: Could not retrieve seed of track %d.", t));
      nSkippedSeeds++;
      continue;
    }
    
    /*if (!AcceptTrack(seed, fMinNumberOfRowsIsTrack))
    {
      AliDebug(AliLog::kDebug, Form("INFO: Rejected track %d.", t));
      nSkippedTracks++;
      continue;
    }*/
    
    for (Int_t clusterID = 0; clusterID < 160; clusterID++)
    {
      AliTPCclusterMI* cluster = seed->GetClusterPointer(clusterID);
      if (!cluster) 
        continue;
      
      Int_t detector = cluster->GetDetector();
      
      if (detector < 0 || detector >= kTPCSectors) 
      {
        AliDebug(AliLog::kDebug, Form("We found a cluster from invalid sector %d", detector));
        continue;
      }

      if (!detailedHistogram) {
        // TODO: find a clever way to handle the time      
    	Int_t time = 0;
    	
    	if (fESD->GetTimeStamp()>1160000000)
    	  time = fESD->GetTimeStamp();      
    	
    	if (!fClusterHistograms[detector])
    	  fClusterHistograms[detector] = new AliTPCClusterHistograms(detector,"",time,time+5*60*60);
    	
    	if (!fClusterHistograms[detector+kTPCSectors])
    	  fClusterHistograms[detector+kTPCSectors] = new AliTPCClusterHistograms(detector,"",time,time+5*60*60, kTRUE);
    	
    	fClusterHistograms[detector]->FillCluster(cluster, time);
    	fClusterHistograms[detector+kTPCSectors]->FillCluster(cluster, time);
    	
    	Int_t z = Int_t(cluster->GetZ()); 
    	if (z>=0 && z<250) {
    	  nClusters++;
    	  clusterQtotSumVsTime[z] += cluster->GetQ();
    	}
      } // end of if !detailedHistograms
      else {
    	// if we need the detailed histograms for this event
    	if (!clusterHistograms[detector])
    	  clusterHistograms[detector] = new AliTPCClusterHistograms(detector, Form("flash_entry%d", entry));
    	
    	clusterHistograms[detector]->FillCluster(cluster);
      }
    }
    
    for (Int_t i=0; i<kTPCHists; i++) 
      if (fClusterHistograms[i]) 
        fClusterHistograms[i]->FillTrack(seed);

  }
  
  // check if there's a very large q deposit ("flash")
  if (!detailedHistogram) {
    for (Int_t z=0; z<250; z++) {
      if (clusterQtotSumVsTime[z] > 150000) {
      	printf(Form("  \n   -> Entry %lld sum of clusters at time %d is %f, ESD timestamp: %s (%d) \n \n", entry, z, clusterQtotSumVsTime[z], TTimeStamp(fESD->GetTimeStamp()).AsString(), fESD->GetTimeStamp()));
       	intToReturn = 1;
      }
    }
  }
  else {
    for (Int_t i=0; i< kTPCSectors; i++) {
      if (clusterHistograms[i]) {
        fObjectsToSave->Add(clusterHistograms[i]);
      }
    }    
  }

//   if (nSkippedSeeds > 0)
//     printf("WARNING: The seed was not found for %d out of %d tracks.\n", nSkippedSeeds, nTracks);
//   if (nSkippedTracks > 0)
//     printf("INFO: Rejected %d out of %d tracks.\n", nSkippedTracks, nTracks);
   
  return intToReturn;
}

Bool_t AliROCESDAnalysisSelector::AcceptTrack(const AliTPCseed* track, Int_t minRowsIncluded) {
  //
  // check if the track should be accepted.
  //
  const Int_t   kMinClusters = 5;
  const Float_t kMinRatio    = 0.75;
  const Float_t kMax1pt      = 0.5;

  Int_t  nRowsUsedByTracks = 0;
  Bool_t rowIncluded[96];
  
  Float_t totalQtot = 0;
  Int_t   nClusters = 0;

  for(Int_t r=0; r<96; r++) 
    rowIncluded[r] = kFALSE;
  
  for (Int_t clusterID = 0; clusterID < 160; clusterID++) {
    AliTPCclusterMI* cluster = track->GetClusterPointer(clusterID);
    
    if (!cluster) 
      continue;
    
    Float_t qTot =   cluster->GetQ();    
    
    nClusters++;
    totalQtot += qTot;

    if (!rowIncluded[cluster->GetRow()]) {
      nRowsUsedByTracks++;
      rowIncluded[cluster->GetRow()] = kTRUE;
    }
  }

  Float_t meanQtot = totalQtot/nClusters;

  if (meanQtot<70)
    return kFALSE;

  if (nRowsUsedByTracks < minRowsIncluded)
    return kFALSE;
  
  //  printf(Form("    TRACK: n clusters = %d,  n pad rows = %d \n",track->GetNumberOfClusters(), nRowsUsedByTracks));
  
  if (track->GetNumberOfClusters()<kMinClusters) return kFALSE;
  Float_t ratio = track->GetNumberOfClusters()/(track->GetNFoundable()+1.);
  if (ratio<kMinRatio) return kFALSE;
  Float_t mpt = track->GetSigned1Pt();
  if (TMath::Abs(mpt)>kMax1pt) return kFALSE;

  //if (TMath::Abs(track->GetZ())>240.) return kFALSE;
  //if (TMath::Abs(track->GetZ())<10.) return kFALSE;
  //if (TMath::Abs(track->GetTgl())>0.03) return kFALSE;
  
  return kTRUE;
}

void AliROCESDAnalysisSelector::SlaveTerminate()
{
  //
  
  if (fOutput)
  {
    for (Int_t i=0; i<kTPCHists; i++)
      if (fClusterHistograms[i])
        fOutput->Add(fClusterHistograms[i]);
  }
} 

void AliROCESDAnalysisSelector::Terminate()
{
  // 
  // read the objects from the output list and write them to a file
  // the filename is modified by the object comment passed in the tree info or input list
  //

  if (fOutput)
  {  
    fOutput->Print();
        
    for (Int_t i=0; i<kTPCSectors; i++)
      fClusterHistograms[i] = dynamic_cast<AliTPCClusterHistograms*> (fOutput->FindObject(AliTPCClusterHistograms::FormDetectorName(i, kFALSE)));
    for (Int_t i=0; i<kTPCSectors; i++)
      fClusterHistograms[kTPCSectors+i] = dynamic_cast<AliTPCClusterHistograms*> (fOutput->FindObject(AliTPCClusterHistograms::FormDetectorName(i, kTRUE)));
  }
  
  TNamed* comment = 0;
  if (fTree && fTree->GetUserInfo())
    comment = dynamic_cast<TNamed*>(fTree->GetUserInfo()->FindObject("comment"));
  if (!comment && fInput)
    comment = dynamic_cast<TNamed*>(fInput->FindObject("comment"));

  if (comment)
  {
    AliDebug(AliLog::kInfo, Form("INFO: Found comment in input list: %s", comment->GetTitle()));
  }
  else
    return;

  TFile* file = TFile::Open(Form("rocESD_%s.root",comment->GetTitle()), "RECREATE");

  for (Int_t i=0; i<kTPCHists; i++)
    if (fClusterHistograms[i]) {
      fClusterHistograms[i]->SaveHistograms();
     // TCanvas* c = fClusterHistograms[i]->DrawHistograms(comment->GetTitle());
			//TString dir;
			//dir.Form("WWW/%s", comment->GetTitle(), c->GetName());
			//gSystem->mkdir(dir, kTRUE);
      //c->SaveAs(Form("%s/plots_%s_%s.eps",dir.Data(),comment->GetTitle(),c->GetName()));
      //c->SaveAs(Form("%s/plots_%s_%s.gif",dir.Data(),comment->GetTitle(),c->GetName()));

      //c->Close();
      //delete c;
    }

  gDirectory->mkdir("saved_objects");
  gDirectory->cd("saved_objects");

  for (Int_t i=0; i<fObjectsToSave->GetSize(); i++) {
    if (fObjectsToSave->At(i)) {
      AliTPCClusterHistograms* clusterHistograms = dynamic_cast<AliTPCClusterHistograms*> (fObjectsToSave->At(i));
      if (clusterHistograms)
	clusterHistograms->SaveHistograms();
      else
	fObjectsToSave->At(i)->Write();
    }
  }

  gDirectory->cd("../");


  file->Close();
}
