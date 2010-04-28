#ifndef ALITOFTRACKERV1_H
#define ALITOFTRACKERV1_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//----------------------------------------------------------------------//
//                                                                      //
// AliTOFtrackerV1 Class                                                //
// Task: Perform association of the ESD tracks to TOF Clusters          //
// and Update ESD track with associated TOF Cluster parameters          //
//                                                                      //
// -- Authors : S. Arcelli, C. Zampolli (Bologna University and INFN)   //
// -- Contacts: Annalisa.De.Caro@cern.ch                                //
// --         : Chiara.Zampolli@bo.infn.it                              //
// --         : Silvia.Arcelli@bo.infn.it                               //
//                                                                      //
//----------------------------------------------------------------------//

#include "AliTracker.h"

class TClonesArray;
class TObjArray;
class TH1F;
class TH2F;

class AliESDEvent;

class AliTOFcluster;
class AliTOFRecoParam;
class AliESDpid;

class AliTOFtrackerV1 : public AliTracker {

public:

 AliTOFtrackerV1(); 

 virtual ~AliTOFtrackerV1();
 virtual void GetPidSettings(AliESDpid *esdPID);
 virtual Int_t Clusters2Tracks(AliESDEvent* /*event*/) {return -1;};
 virtual Int_t PropagateBack(AliESDEvent* event);
 virtual Int_t RefitInward(AliESDEvent* /*event*/) {return -1;};
 virtual Int_t LoadClusters(TTree * cTree); // Load Clusters
 virtual void  UnloadClusters();// UnLoad Clusters
 virtual AliCluster *GetCluster(Int_t index) const
   {if (index==-1 || index >= fN) return NULL;
   return (AliCluster *) fClusters[index];};
 Bool_t GetTrackPoint(Int_t index, AliTrackPoint& p) const;
 void InitCheckHists();
 void SaveCheckHists();
 void FillClusterArray(TObjArray* arr) const;

private:

 enum {kMaxCluster=77777}; //maximal number of the TOF clusters

 AliTOFtrackerV1(const AliTOFtrackerV1 &t); //Copy Ctor 
 AliTOFtrackerV1& operator=(const AliTOFtrackerV1 &source); // ass. op.

 Int_t FindClusterIndex(Double_t z) const; // Returns cluster index 
 void  MatchTracks(); // Matching Algorithm 
 void  CollectESD(); // Select starting Set for Matching 
 Float_t  GetTimeZerofromTOF(AliESDEvent* /*event*/) const; // T0 from TOF
 Float_t  GetTimeZerofromT0(const AliESDEvent * const event) const; // T0 from T0
 Float_t  CorrectTimeWalk(Float_t dist,Float_t tof) const; // Time Walk correction

 const AliTOFRecoParam* fkRecoParam;    // Pointer to TOF Recon. Pars
 AliTOFcluster *fClusters[kMaxCluster]; // pointers to the TOF clusters

 Int_t fN;              // Number of Clusters
 Int_t fNseeds;         // Number of track seeds  
 Int_t fNseedsTOF;      // TPC BP tracks
 Int_t fngoodmatch;     // Correctly matched  tracks
 Int_t fnbadmatch;      // Wrongly matched tracks
 Int_t fnunmatch;       // Unmatched tracks
 Int_t fnmatch;         // Total matched tracks
 
 TClonesArray* fTracks; //! pointer to the TClonesArray with TOF tracks
 TObjArray* fSeeds;  //! pointer to the TObjArray with ESD tracks
 //Digits/Reco QA histos

 TH2F * fHDigClusMap; //Digits QA, Cluster Map 
 TH1F * fHDigNClus;   //Digits QA, # of clusters on TOF/event
 TH1F * fHDigClusTime;//Digits QA, Cluster Time (ns)
 TH1F * fHDigClusToT; //Digits QA, Cluster ToT (ns)
 TH1F * fHRecNClus; //Reco QA, cluster occupancy in search window
 TH1F * fHRecChi2;//Reco QA, track-best TOF cluster chi2
 TH1F * fHRecDistZ;//Reco QA, track-TOF cluster closest distance (cm)
 TH2F * fHRecSigYVsP;//Reco QA, track error in Y at TOF inner surface (cm)
 TH2F * fHRecSigZVsP; //Reco QA, track error in Z at TOF inner surface (cm)
 TH2F * fHRecSigYVsPWin;//Reco QA, search window size in Y (cm)
 TH2F * fHRecSigZVsPWin;//Reco QA, search window size in X (cm)

 ClassDef(AliTOFtrackerV1, 3) // TOF tracker 
};

#endif
