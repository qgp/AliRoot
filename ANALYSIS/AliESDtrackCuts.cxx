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

/* $Id: AliESDtrackCuts.cxx 24534 2008-03-16 22:22:11Z fca $ */

#include "AliESDtrackCuts.h"

#include <AliESDtrack.h>
#include <AliESDVertex.h>
#include <AliESDEvent.h>
#include <AliLog.h>

#include <TTree.h>
#include <TCanvas.h>
#include <TDirectory.h>

//____________________________________________________________________
ClassImp(AliESDtrackCuts)

// Cut names
const Char_t* AliESDtrackCuts::fgkCutNames[kNCuts] = {
 "require TPC refit",
 "require ITS refit",
 "n clusters TPC",
 "n clusters ITS",
 "#Chi^{2}/clusters TPC",
 "#Chi^{2}/clusters ITS",
 "cov 11",
 "cov 22",
 "cov 33",
 "cov 44",
 "cov 55",
 "trk-to-vtx",
 "trk-to-vtx failed",
 "kink daughters",
 "p",
 "p_{T}",
 "p_{x}",
 "p_{y}",
 "p_{z}",
 "y",
 "eta",
 "trk-to-vtx dca absolute",
 "trk-to-vtx dca xy absolute",
 "trk-to-vtx dca z absolute",
 "SPD cluster requirement",
 "SDD cluster requirement",
 "SSD cluster requirement"
};

//____________________________________________________________________
AliESDtrackCuts::AliESDtrackCuts(const Char_t* name, const Char_t* title) : AliAnalysisCuts(name,title),
  fCutMinNClusterTPC(0),
  fCutMinNClusterITS(0),
  fCutMaxChi2PerClusterTPC(0),
  fCutMaxChi2PerClusterITS(0),
  fCutMaxC11(0),
  fCutMaxC22(0),
  fCutMaxC33(0),
  fCutMaxC44(0),
  fCutMaxC55(0),
  fCutAcceptKinkDaughters(0),
  fCutRequireTPCRefit(0),
  fCutRequireITSRefit(0),
  fCutNsigmaToVertex(0),
  fCutSigmaToVertexRequired(0),
  fCutDCAToVertex(0),
  fCutDCAToVertexXY(0),
  fCutDCAToVertexZ(0),
  fPMin(0),
  fPMax(0),
  fPtMin(0),
  fPtMax(0),
  fPxMin(0),
  fPxMax(0),
  fPyMin(0),
  fPyMax(0),
  fPzMin(0),
  fPzMax(0),
  fEtaMin(0),
  fEtaMax(0),
  fRapMin(0),
  fRapMax(0),
  fHistogramsOn(0),
  ffDTheoretical(0),
  fhCutStatistics(0),         
  fhCutCorrelation(0)
{
  //
  // constructor
  //

  Init();

  //##############################################################################
  // setting default cuts
  SetMinNClustersTPC();
  SetMinNClustersITS();
  SetMaxChi2PerClusterTPC();
  SetMaxChi2PerClusterITS();  				    
  SetMaxCovDiagonalElements();  				    
  SetRequireTPCRefit();
  SetRequireITSRefit();
  SetAcceptKingDaughters();
  SetMaxNsigmaToVertex();
  SetRequireSigmaToVertex();
  SetMaxDCAToVertex();
  SetMaxDCAToVertexXY();
  SetMaxDCAToVertexZ();
  SetPRange();
  SetPtRange();
  SetPxRange();
  SetPyRange();
  SetPzRange();
  SetEtaRange();
  SetRapRange();
  SetClusterRequirementITS(kSPD);
  SetClusterRequirementITS(kSDD);
  SetClusterRequirementITS(kSSD);

  SetHistogramsOn();
}

//_____________________________________________________________________________
AliESDtrackCuts::AliESDtrackCuts(const AliESDtrackCuts &c) : AliAnalysisCuts(c),
  fCutMinNClusterTPC(0),
  fCutMinNClusterITS(0),
  fCutMaxChi2PerClusterTPC(0),
  fCutMaxChi2PerClusterITS(0),
  fCutMaxC11(0),
  fCutMaxC22(0),
  fCutMaxC33(0),
  fCutMaxC44(0),
  fCutMaxC55(0),
  fCutAcceptKinkDaughters(0),
  fCutRequireTPCRefit(0),
  fCutRequireITSRefit(0),
  fCutNsigmaToVertex(0),
  fCutSigmaToVertexRequired(0),
  fCutDCAToVertex(0),
  fCutDCAToVertexXY(0),
  fCutDCAToVertexZ(0),
  fPMin(0),
  fPMax(0),
  fPtMin(0),
  fPtMax(0),
  fPxMin(0),
  fPxMax(0),
  fPyMin(0),
  fPyMax(0),
  fPzMin(0),
  fPzMax(0),
  fEtaMin(0),
  fEtaMax(0),
  fRapMin(0),
  fRapMax(0),
  fHistogramsOn(0),
  ffDTheoretical(0),				     
  fhCutStatistics(0),         
  fhCutCorrelation(0)
{
  //
  // copy constructor
  //

  ((AliESDtrackCuts &) c).Copy(*this);
}

AliESDtrackCuts::~AliESDtrackCuts()
{
  //
  // destructor
  //

  for (Int_t i=0; i<2; i++) {
    
    if (fhNClustersITS[i])
      delete fhNClustersITS[i];            
    if (fhNClustersTPC[i])
      delete fhNClustersTPC[i];                
    if (fhChi2PerClusterITS[i])
      delete fhChi2PerClusterITS[i];       
    if (fhChi2PerClusterTPC[i])
      delete fhChi2PerClusterTPC[i];       
    if (fhC11[i])
      delete fhC11[i];                     
    if (fhC22[i])
      delete fhC22[i];                     
    if (fhC33[i])
      delete fhC33[i];                     
    if (fhC44[i])
      delete fhC44[i];                     
    if (fhC55[i])
    delete fhC55[i];                     
    
    if (fhDXY[i])
      delete fhDXY[i];                     
    if (fhDZ[i])
      delete fhDZ[i];
    if (fhDXYDZ[i])
      delete fhDXYDZ[i];
    if (fhDXYvsDZ[i])
      delete fhDXYvsDZ[i];

    if (fhDXYNormalized[i])
      delete fhDXYNormalized[i];           
    if (fhDZNormalized[i])
      delete fhDZNormalized[i];
    if (fhDXYvsDZNormalized[i])
      delete fhDXYvsDZNormalized[i];
    if (fhNSigmaToVertex[i])
      delete fhNSigmaToVertex[i];
    if (fhPt[i])
      delete fhPt[i];
    if (fhEta[i])
      delete fhEta[i];
  }

  if (ffDTheoretical)
    delete ffDTheoretical;

  if (fhCutStatistics)
    delete fhCutStatistics;             
  if (fhCutCorrelation)
    delete fhCutCorrelation;            
}

void AliESDtrackCuts::Init()
{
  //
  // sets everything to zero
  //

  fCutMinNClusterTPC = 0;
  fCutMinNClusterITS = 0;

  fCutMaxChi2PerClusterTPC = 0;
  fCutMaxChi2PerClusterITS = 0;
  
  for (Int_t i = 0; i < 3; i++)
  	fCutClusterRequirementITS[i] = kOff;

  fCutMaxC11 = 0;
  fCutMaxC22 = 0;
  fCutMaxC33 = 0;
  fCutMaxC44 = 0;
  fCutMaxC55 = 0;

  fCutAcceptKinkDaughters = 0;
  fCutRequireTPCRefit = 0;
  fCutRequireITSRefit = 0;

  fCutNsigmaToVertex = 0;
  fCutSigmaToVertexRequired = 0;
  fCutDCAToVertex = 0;
  fCutDCAToVertexXY = 0;
  fCutDCAToVertexZ = 0;

  fPMin = 0;
  fPMax = 0;
  fPtMin = 0;
  fPtMax = 0;
  fPxMin = 0;
  fPxMax = 0;
  fPyMin = 0;
  fPyMax = 0;
  fPzMin = 0;
  fPzMax = 0;
  fEtaMin = 0;
  fEtaMax = 0;
  fRapMin = 0;
  fRapMax = 0;

  fHistogramsOn = kFALSE;

  for (Int_t i=0; i<2; ++i)
  {
    fhNClustersITS[i] = 0;
    fhNClustersTPC[i] = 0;

    fhChi2PerClusterITS[i] = 0;
    fhChi2PerClusterTPC[i] = 0;

    fhC11[i] = 0;
    fhC22[i] = 0;
    fhC33[i] = 0;
    fhC44[i] = 0;
    fhC55[i] = 0;

    fhDXY[i] = 0;
    fhDZ[i] = 0;
    fhDXYDZ[i] = 0;
    fhDXYvsDZ[i] = 0;

    fhDXYNormalized[i] = 0;
    fhDZNormalized[i] = 0;
    fhDXYvsDZNormalized[i] = 0;
    fhNSigmaToVertex[i] = 0;
    
    fhPt[i] = 0;
    fhEta[i] = 0;
  }
  ffDTheoretical = 0;

  fhCutStatistics = 0;
  fhCutCorrelation = 0;
}

//_____________________________________________________________________________
AliESDtrackCuts &AliESDtrackCuts::operator=(const AliESDtrackCuts &c)
{
  //
  // Assignment operator
  //

  if (this != &c) ((AliESDtrackCuts &) c).Copy(*this);
  return *this;
}

//_____________________________________________________________________________
void AliESDtrackCuts::Copy(TObject &c) const
{
  //
  // Copy function
  //

  AliESDtrackCuts& target = (AliESDtrackCuts &) c;

  target.Init();

  target.fCutMinNClusterTPC = fCutMinNClusterTPC;
  target.fCutMinNClusterITS = fCutMinNClusterITS;

  target.fCutMaxChi2PerClusterTPC = fCutMaxChi2PerClusterTPC;
  target.fCutMaxChi2PerClusterITS = fCutMaxChi2PerClusterITS;

  for (Int_t i = 0; i < 3; i++)
    target.fCutClusterRequirementITS[i] = fCutClusterRequirementITS[i];

  target.fCutMaxC11 = fCutMaxC11;
  target.fCutMaxC22 = fCutMaxC22;
  target.fCutMaxC33 = fCutMaxC33;
  target.fCutMaxC44 = fCutMaxC44;
  target.fCutMaxC55 = fCutMaxC55;

  target.fCutAcceptKinkDaughters = fCutAcceptKinkDaughters;
  target.fCutRequireTPCRefit = fCutRequireTPCRefit;
  target.fCutRequireITSRefit = fCutRequireITSRefit;

  target.fCutNsigmaToVertex = fCutNsigmaToVertex;
  target.fCutSigmaToVertexRequired = fCutSigmaToVertexRequired;
  target.fCutDCAToVertex = fCutDCAToVertex;
  target.fCutDCAToVertexXY = fCutDCAToVertexXY;
  target.fCutDCAToVertexZ = fCutDCAToVertexZ;

  target.fPMin = fPMin;
  target.fPMax = fPMax;
  target.fPtMin = fPtMin;
  target.fPtMax = fPtMax;
  target.fPxMin = fPxMin;
  target.fPxMax = fPxMax;
  target.fPyMin = fPyMin;
  target.fPyMax = fPyMax;
  target.fPzMin = fPzMin;
  target.fPzMax = fPzMax;
  target.fEtaMin = fEtaMin;
  target.fEtaMax = fEtaMax;
  target.fRapMin = fRapMin;
  target.fRapMax = fRapMax;

  target.fHistogramsOn = fHistogramsOn;

  for (Int_t i=0; i<2; ++i)
  {
    if (fhNClustersITS[i]) target.fhNClustersITS[i] = (TH1F*) fhNClustersITS[i]->Clone();
    if (fhNClustersTPC[i]) target.fhNClustersTPC[i] = (TH1F*) fhNClustersTPC[i]->Clone();

    if (fhChi2PerClusterITS[i]) target.fhChi2PerClusterITS[i] = (TH1F*) fhChi2PerClusterITS[i]->Clone();
    if (fhChi2PerClusterTPC[i]) target.fhChi2PerClusterTPC[i] = (TH1F*) fhChi2PerClusterTPC[i]->Clone();

    if (fhC11[i]) target.fhC11[i] = (TH1F*) fhC11[i]->Clone();
    if (fhC22[i]) target.fhC22[i] = (TH1F*) fhC22[i]->Clone();
    if (fhC33[i]) target.fhC33[i] = (TH1F*) fhC33[i]->Clone();
    if (fhC44[i]) target.fhC44[i] = (TH1F*) fhC44[i]->Clone();
    if (fhC55[i]) target.fhC55[i] = (TH1F*) fhC55[i]->Clone();

    if (fhDXY[i]) target.fhDXY[i] = (TH1F*) fhDXY[i]->Clone();
    if (fhDZ[i]) target.fhDZ[i] = (TH1F*) fhDZ[i]->Clone();
    if (fhDXYDZ[i]) target.fhDXYDZ[i] = (TH1F*) fhDXYDZ[i]->Clone();
    if (fhDXYvsDZ[i]) target.fhDXYvsDZ[i] = (TH2F*) fhDXYvsDZ[i]->Clone();

    if (fhDXYNormalized[i]) target.fhDXYNormalized[i] = (TH1F*) fhDXYNormalized[i]->Clone();
    if (fhDZNormalized[i]) target.fhDZNormalized[i] = (TH1F*) fhDZNormalized[i]->Clone();
    if (fhDXYvsDZNormalized[i]) target.fhDXYvsDZNormalized[i] = (TH2F*) fhDXYvsDZNormalized[i]->Clone();
    if (fhNSigmaToVertex[i]) target.fhNSigmaToVertex[i] = (TH1F*) fhNSigmaToVertex[i]->Clone();
    
    if (fhPt[i]) target.fhPt[i] = (TH1F*) fhPt[i]->Clone();
    if (fhEta[i]) target.fhEta[i] = (TH1F*) fhEta[i]->Clone();
  }
  if (ffDTheoretical) target.ffDTheoretical = (TF1*) ffDTheoretical->Clone();

  if (fhCutStatistics) target.fhCutStatistics = (TH1F*) fhCutStatistics->Clone();
  if (fhCutCorrelation) target.fhCutCorrelation = (TH2F*) fhCutCorrelation->Clone();

  TNamed::Copy(c);
}

//_____________________________________________________________________________
Long64_t AliESDtrackCuts::Merge(TCollection* list) {
  // Merge a list of AliESDtrackCuts objects with this (needed for PROOF)
  // Returns the number of merged objects (including this)
  if (!list)
    return 0;
  if (list->IsEmpty())
    return 1;
  if (!fHistogramsOn)
    return 0;
  TIterator* iter = list->MakeIterator();
  TObject* obj;

  // collection of measured and generated histograms
  Int_t count = 0;
  while ((obj = iter->Next())) {

    AliESDtrackCuts* entry = dynamic_cast<AliESDtrackCuts*>(obj);
    if (entry == 0)
      continue;

    if (!entry->fHistogramsOn)
      continue;

    for (Int_t i=0; i<2; i++) {
      
      fhNClustersITS[i]      ->Add(entry->fhNClustersITS[i]     );      
      fhNClustersTPC[i]      ->Add(entry->fhNClustersTPC[i]     ); 
      					  			    
      fhChi2PerClusterITS[i] ->Add(entry->fhChi2PerClusterITS[i]); 
      fhChi2PerClusterTPC[i] ->Add(entry->fhChi2PerClusterTPC[i]); 
      					  			    
      fhC11[i]               ->Add(entry->fhC11[i]              ); 
      fhC22[i]               ->Add(entry->fhC22[i]              ); 
      fhC33[i]               ->Add(entry->fhC33[i]              ); 
      fhC44[i]               ->Add(entry->fhC44[i]              ); 
      fhC55[i]               ->Add(entry->fhC55[i]              ); 
      					  			    
      fhDXY[i]               ->Add(entry->fhDXY[i]              ); 
      fhDZ[i]                ->Add(entry->fhDZ[i]               ); 
      fhDXYDZ[i]             ->Add(entry->fhDXYDZ[i]          );
      fhDXYvsDZ[i]           ->Add(entry->fhDXYvsDZ[i]          );

      fhDXYNormalized[i]     ->Add(entry->fhDXYNormalized[i]    );
      fhDZNormalized[i]      ->Add(entry->fhDZNormalized[i]     );
      fhDXYvsDZNormalized[i] ->Add(entry->fhDXYvsDZNormalized[i]);
      fhNSigmaToVertex[i]    ->Add(entry->fhNSigmaToVertex[i]); 

      fhPt[i]                ->Add(entry->fhPt[i]); 
      fhEta[i]               ->Add(entry->fhEta[i]); 
    }      

    fhCutStatistics  ->Add(entry->fhCutStatistics);        
    fhCutCorrelation ->Add(entry->fhCutCorrelation);      

    count++;
  }
  return count+1;
}


//____________________________________________________________________
Float_t AliESDtrackCuts::GetSigmaToVertex(AliESDtrack* esdTrack)
{
  // Calculates the number of sigma to the vertex.

  Float_t b[2];
  Float_t bRes[2];
  Float_t bCov[3];
  esdTrack->GetImpactParameters(b,bCov);
  
  if (bCov[0]<=0 || bCov[2]<=0) {
    AliDebugClass(1, "Estimated b resolution lower or equal zero!");
    bCov[0]=0; bCov[2]=0;
  }
  bRes[0] = TMath::Sqrt(bCov[0]);
  bRes[1] = TMath::Sqrt(bCov[2]);

  // -----------------------------------
  // How to get to a n-sigma cut?
  //
  // The accumulated statistics from 0 to d is
  //
  // ->  Erf(d/Sqrt(2)) for a 1-dim gauss (d = n_sigma)
  // ->  1 - Exp(-d**2) for a 2-dim gauss (d*d = dx*dx + dy*dy != n_sigma)
  //
  // It means that for a 2-dim gauss: n_sigma(d) = Sqrt(2)*ErfInv(1 - Exp((-x**2)/2)
  // Can this be expressed in a different way?

  if (bRes[0] == 0 || bRes[1] ==0)
    return -1;

  Float_t d = TMath::Sqrt(TMath::Power(b[0]/bRes[0],2) + TMath::Power(b[1]/bRes[1],2));

  // work around precision problem
  // if d is too big, TMath::Exp(...) gets 0, and TMath::ErfInverse(1) that should be infinite, gets 0 :(
  // 1e-15 corresponds to nsigma ~ 7.7
  if (TMath::Exp(-d * d / 2) < 1e-15)
    return 1000;

  Float_t nSigma = TMath::ErfInverse(1 - TMath::Exp(-d * d / 2)) * TMath::Sqrt(2);
  return nSigma;
}

void AliESDtrackCuts::EnableNeededBranches(TTree* tree)
{
  // enables the branches needed by AcceptTrack, for a list see comment of AcceptTrack

  tree->SetBranchStatus("fTracks.fFlags", 1);
  tree->SetBranchStatus("fTracks.fITSncls", 1);
  tree->SetBranchStatus("fTracks.fTPCncls", 1);
  tree->SetBranchStatus("fTracks.fITSchi2", 1);
  tree->SetBranchStatus("fTracks.fTPCchi2", 1);
  tree->SetBranchStatus("fTracks.fC*", 1);
  tree->SetBranchStatus("fTracks.fD", 1);
  tree->SetBranchStatus("fTracks.fZ", 1);
  tree->SetBranchStatus("fTracks.fCdd", 1);
  tree->SetBranchStatus("fTracks.fCdz", 1);
  tree->SetBranchStatus("fTracks.fCzz", 1);
  tree->SetBranchStatus("fTracks.fP*", 1);
  tree->SetBranchStatus("fTracks.fR*", 1);
  tree->SetBranchStatus("fTracks.fKinkIndexes*", 1);
}

//____________________________________________________________________
Bool_t
AliESDtrackCuts::AcceptTrack(AliESDtrack* esdTrack) {
  // 
  // figure out if the tracks survives all the track cuts defined
  //
  // the different quality parameter and kinematic values are first
  // retrieved from the track. then it is found out what cuts the
  // track did not survive and finally the cuts are imposed.

  // this function needs the following branches:
  // fTracks.fFlags
  // fTracks.fITSncls
  // fTracks.fTPCncls
  // fTracks.fITSchi2
  // fTracks.fTPCchi2
  // fTracks.fC   //GetExternalCovariance
  // fTracks.fD   //GetImpactParameters
  // fTracks.fZ   //GetImpactParameters
  // fTracks.fCdd //GetImpactParameters
  // fTracks.fCdz //GetImpactParameters
  // fTracks.fCzz //GetImpactParameters
  // fTracks.fP   //GetPxPyPz
  // fTracks.fR   //GetMass
  // fTracks.fP   //GetMass
  // fTracks.fKinkIndexes


  UInt_t status = esdTrack->GetStatus();

  // getting quality parameters from the ESD track
  Int_t nClustersITS = esdTrack->GetITSclusters(0);
  Int_t nClustersTPC = esdTrack->GetTPCclusters(0);

  Float_t chi2PerClusterITS = -1;
  Float_t chi2PerClusterTPC = -1;
  if (nClustersITS!=0)
    chi2PerClusterITS = esdTrack->GetITSchi2()/Float_t(nClustersITS);
  if (nClustersTPC!=0)
    chi2PerClusterTPC = esdTrack->GetTPCchi2()/Float_t(nClustersTPC);
  Double_t extCov[15];
  esdTrack->GetExternalCovariance(extCov);

  // getting the track to vertex parameters
  Float_t nSigmaToVertex = GetSigmaToVertex(esdTrack);
      
  Float_t b[2];
  Float_t bCov[3];
  esdTrack->GetImpactParameters(b,bCov);
  if (bCov[0]<=0 || bCov[2]<=0) {
    AliDebug(1, "Estimated b resolution lower or equal zero!");
    bCov[0]=0; bCov[2]=0;
  }

  Float_t dcaToVertexXY = b[0];
  Float_t dcaToVertexZ = b[1];

  Float_t dcaToVertex   = TMath::Sqrt(dcaToVertexXY*dcaToVertexXY + dcaToVertexZ*dcaToVertexZ);

  // getting the kinematic variables of the track
  // (assuming the mass is known)
  Double_t p[3];
  esdTrack->GetPxPyPz(p);

  Float_t momentum = TMath::Sqrt(TMath::Power(p[0],2) + TMath::Power(p[1],2) + TMath::Power(p[2],2));
  Float_t pt       = TMath::Sqrt(TMath::Power(p[0],2) + TMath::Power(p[1],2));
  Float_t energy   = TMath::Sqrt(TMath::Power(esdTrack->GetMass(),2) + TMath::Power(momentum,2));


  //y-eta related calculations
  Float_t eta = -100.;
  Float_t y   = -100.;
  if((momentum != TMath::Abs(p[2]))&&(momentum != 0))
    eta = 0.5*TMath::Log((momentum + p[2])/(momentum - p[2]));
  if((energy != TMath::Abs(p[2]))&&(momentum != 0))
    y = 0.5*TMath::Log((energy + p[2])/(energy - p[2]));

  
  //########################################################################
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
  if (extCov[0]  > fCutMaxC11) 
    cuts[6]=kTRUE;  
  if (extCov[2]  > fCutMaxC22) 
    cuts[7]=kTRUE;  
  if (extCov[5]  > fCutMaxC33) 
    cuts[8]=kTRUE;  
  if (extCov[9]  > fCutMaxC44) 
    cuts[9]=kTRUE;  
  if (extCov[14]  > fCutMaxC55) 
    cuts[10]=kTRUE;  
  if (nSigmaToVertex > fCutNsigmaToVertex && fCutSigmaToVertexRequired)
    cuts[11] = kTRUE;
  // if n sigma could not be calculated
  if (nSigmaToVertex<0 && fCutSigmaToVertexRequired)
    cuts[12]=kTRUE;
  if (!fCutAcceptKinkDaughters && esdTrack->GetKinkIndex(0)>0)
    cuts[13]=kTRUE;
  // track kinematics cut
  if((momentum < fPMin) || (momentum > fPMax)) 
    cuts[14]=kTRUE;
  if((pt < fPtMin) || (pt > fPtMax)) 
    cuts[15] = kTRUE;
  if((p[0] < fPxMin) || (p[0] > fPxMax)) 
    cuts[16] = kTRUE;
  if((p[1] < fPyMin) || (p[1] > fPyMax)) 
    cuts[17] = kTRUE;
  if((p[2] < fPzMin) || (p[2] > fPzMax))
    cuts[18] = kTRUE;
  if((eta < fEtaMin) || (eta > fEtaMax))
    cuts[19] = kTRUE;
  if((y < fRapMin) || (y > fRapMax)) 
    cuts[20] = kTRUE;
  if (dcaToVertex > fCutDCAToVertex)
    cuts[21] = kTRUE;
  if (TMath::Abs(dcaToVertexXY) > fCutDCAToVertexXY)
    cuts[22] = kTRUE;
  if (TMath::Abs(dcaToVertexZ) > fCutDCAToVertexZ)
    cuts[23] = kTRUE;
  
  for (Int_t i = 0; i < 3; i++)
    cuts[24+i] = !CheckITSClusterRequirement(fCutClusterRequirementITS[i], esdTrack->HasPointOnITSLayer(i*2), esdTrack->HasPointOnITSLayer(i*2+1));

  Bool_t cut=kFALSE;
  for (Int_t i=0; i<kNCuts; i++) 
    if (cuts[i]) cut = kTRUE;



  //########################################################################
  // filling histograms
  if (fHistogramsOn) {
    fhCutStatistics->Fill(fhCutStatistics->GetBinCenter(fhCutStatistics->GetXaxis()->FindBin("n tracks")));
    if (cut)
      fhCutStatistics->Fill(fhCutStatistics->GetBinCenter(fhCutStatistics->GetXaxis()->FindBin("n cut tracks")));
    
    for (Int_t i=0; i<kNCuts; i++) {
      if (cuts[i])
        fhCutStatistics->Fill(fhCutStatistics->GetBinCenter(fhCutStatistics->GetXaxis()->FindBin(fgkCutNames[i])));

      for (Int_t j=i; j<kNCuts; j++) {
        if (cuts[i] && cuts[j]) {
          Float_t xC = fhCutCorrelation->GetXaxis()->GetBinCenter(fhCutCorrelation->GetXaxis()->FindBin(fgkCutNames[i]));
          Float_t yC = fhCutCorrelation->GetYaxis()->GetBinCenter(fhCutCorrelation->GetYaxis()->FindBin(fgkCutNames[j]));
          fhCutCorrelation->Fill(xC, yC);
        }
      }
    }
  }

  // now we loop over the filling of the histograms twice: once "before" the cut, once "after"
  // the code is not in a function due to too many local variables that would need to be passed

  for (Int_t id = 0; id < 2; id++)
  {
    // id = 0 --> before cut
    // id = 1 --> after cut

    if (fHistogramsOn)
    {
      fhNClustersITS[id]->Fill(nClustersITS);
      fhNClustersTPC[id]->Fill(nClustersTPC);
      fhChi2PerClusterITS[id]->Fill(chi2PerClusterITS);
      fhChi2PerClusterTPC[id]->Fill(chi2PerClusterTPC);

      fhC11[id]->Fill(extCov[0]);
      fhC22[id]->Fill(extCov[2]);
      fhC33[id]->Fill(extCov[5]);
      fhC44[id]->Fill(extCov[9]);
      fhC55[id]->Fill(extCov[14]);

      fhPt[id]->Fill(pt);
      fhEta[id]->Fill(eta);

      Float_t bRes[2];
      bRes[0] = TMath::Sqrt(bCov[0]);
      bRes[1] = TMath::Sqrt(bCov[2]);

      fhDZ[id]->Fill(b[1]);
      fhDXY[id]->Fill(b[0]);
      fhDXYDZ[id]->Fill(dcaToVertex);
      fhDXYvsDZ[id]->Fill(b[1],b[0]);

      if (bRes[0]!=0 && bRes[1]!=0) {
        fhDZNormalized[id]->Fill(b[1]/bRes[1]);
        fhDXYNormalized[id]->Fill(b[0]/bRes[0]);
        fhDXYvsDZNormalized[id]->Fill(b[1]/bRes[1], b[0]/bRes[0]);
        fhNSigmaToVertex[id]->Fill(nSigmaToVertex);
      }
    }

    // cut the track
    if (cut)
      return kFALSE;
  }

  return kTRUE;
}

//____________________________________________________________________
Bool_t AliESDtrackCuts::CheckITSClusterRequirement(ITSClusterRequirement req, Bool_t clusterL1, Bool_t clusterL2)
{
  // checks if the cluster requirement is fullfilled (in this case: return kTRUE)
  
  switch (req)
  {
  	case kOff:        return kTRUE;
  	case kNone:       return !clusterL1 && !clusterL2;
  	case kAny:        return clusterL1 || clusterL2;
  	case kFirst:      return clusterL1;
  	case kOnlyFirst:  return clusterL1 && !clusterL2;
  	case kSecond:     return clusterL2;
  	case kOnlySecond: return clusterL2 && !clusterL1;
  	case kBoth:       return clusterL1 && clusterL2;
  }
  
  return kFALSE;
}

//____________________________________________________________________
AliESDtrack* AliESDtrackCuts::GetTPCOnlyTrack(AliESDEvent* esd, Int_t iTrack)
{
  // creates a TPC only track from the given esd track
  // the track has to be deleted by the user
  //
  // NB. most of the functionality to get a TPC only track from an ESD track is in AliESDtrack, where it should be
  // there are only missing propagations here that are needed for old data
  // this function will therefore become obsolete
  //
  // adapted from code provided by CKB

  if (!esd->GetPrimaryVertexTPC())
    return 0; // No TPC vertex no TPC tracks

  if(!esd->GetPrimaryVertexTPC()->GetStatus())
    return 0; // TPC Vertex is created by default in AliESDEvent, do not use in this case

  AliESDtrack* track = esd->GetTrack(iTrack);
  if (!track)
    return 0;

  AliESDtrack *tpcTrack = new AliESDtrack();

  // This should have been done during the reconstruction
  // fixed by Juri in r26675
  // but recalculate for older data CKB
  Float_t p[2],cov[3];
  track->GetImpactParametersTPC(p,cov);
  if(p[0]==0&&p[1]==0)
    track->RelateToVertexTPC(esd->GetPrimaryVertexTPC(),esd->GetMagneticField(),kVeryBig);
  // BKC

  // only true if we have a tpc track
  if (!track->FillTPCOnlyTrack(*tpcTrack))
  {
    delete tpcTrack;
    return 0;
  }

  // propagate to Vertex
  // not needed for normal reconstructed ESDs...
  // Double_t pTPC[2],covTPC[3];
  // tpcTrack->PropagateToDCA(esd->GetPrimaryVertexTPC(), esd->GetMagneticField(), 10000,  pTPC, covTPC);

  return tpcTrack;
}

//____________________________________________________________________
TObjArray* AliESDtrackCuts::GetAcceptedTracks(AliESDEvent* esd,Bool_t bTPC)
{
  //
  // returns an array of all tracks that pass the cuts
  // or an array of TPC only tracks (propagated to the TPC vertex during reco)
  // tracks that pass the cut

  TObjArray* acceptedTracks = new TObjArray();

  // loop over esd tracks
  for (Int_t iTrack = 0; iTrack < esd->GetNumberOfTracks(); iTrack++) {
    if(bTPC){
      if(!esd->GetPrimaryVertexTPC())return acceptedTracks; // No TPC vertex no TPC tracks
      if(!esd->GetPrimaryVertexTPC()->GetStatus())return acceptedTracks; // No proper TPC vertex, only the default

      AliESDtrack *tpcTrack = GetTPCOnlyTrack(esd, iTrack);
      if (!tpcTrack)
        continue;

      if (AcceptTrack(tpcTrack)) {
        acceptedTracks->Add(tpcTrack);
      }
      else
        delete tpcTrack;
    }
    else
    {
      AliESDtrack* track = esd->GetTrack(iTrack);
      if(AcceptTrack(track))
        acceptedTracks->Add(track);
    }
  } 
  if(bTPC)acceptedTracks->SetOwner(kTRUE);
  return acceptedTracks;
}

//____________________________________________________________________
Int_t AliESDtrackCuts::CountAcceptedTracks(AliESDEvent* esd)
{
  //
  // returns an the number of tracks that pass the cuts
  //

  Int_t count = 0;

  // loop over esd tracks
  for (Int_t iTrack = 0; iTrack < esd->GetNumberOfTracks(); iTrack++) {
    AliESDtrack* track = esd->GetTrack(iTrack);
    if (AcceptTrack(track))
      count++;
  }

  return count;
}

//____________________________________________________________________
 void AliESDtrackCuts::DefineHistograms(Int_t color) {
   // 
   // diagnostics histograms are defined
   // 

   fHistogramsOn=kTRUE;
   
   Bool_t oldStatus = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   
   //###################################################################################
   // defining histograms

   fhCutStatistics = new TH1F("cut_statistics","cut statistics",kNCuts+4,-0.5,kNCuts+3.5);

   fhCutStatistics->GetXaxis()->SetBinLabel(1,"n tracks");
   fhCutStatistics->GetXaxis()->SetBinLabel(2,"n cut tracks");

   fhCutCorrelation = new TH2F("cut_correlation","cut correlation",kNCuts,-0.5,kNCuts-0.5,kNCuts,-0.5,kNCuts-0.5);;
  
   for (Int_t i=0; i<kNCuts; i++) {
     fhCutStatistics->GetXaxis()->SetBinLabel(i+4,fgkCutNames[i]);
     fhCutCorrelation->GetXaxis()->SetBinLabel(i+1,fgkCutNames[i]);
     fhCutCorrelation->GetYaxis()->SetBinLabel(i+1,fgkCutNames[i]);
   }

  fhCutStatistics  ->SetLineColor(color);
  fhCutCorrelation ->SetLineColor(color);
  fhCutStatistics  ->SetLineWidth(2);
  fhCutCorrelation ->SetLineWidth(2);

  for (Int_t i=0; i<2; i++) {
    fhNClustersITS[i]        = new TH1F("nClustersITS"    ,"",8,-0.5,7.5);
    fhNClustersTPC[i]        = new TH1F("nClustersTPC"     ,"",165,-0.5,164.5);
    fhChi2PerClusterITS[i]   = new TH1F("chi2PerClusterITS","",500,0,10);
    fhChi2PerClusterTPC[i]   = new TH1F("chi2PerClusterTPC","",500,0,10);

    fhC11[i]                 = new TH1F("covMatrixDiagonal11","",2000,0,20);
    fhC22[i]                 = new TH1F("covMatrixDiagonal22","",2000,0,20);
    fhC33[i]                 = new TH1F("covMatrixDiagonal33","",1000,0,0.1);
    fhC44[i]                 = new TH1F("covMatrixDiagonal44","",1000,0,0.1);
    fhC55[i]                 = new TH1F("covMatrixDiagonal55","",1000,0,5);

    fhDXY[i]                 = new TH1F("dXY"    ,"",500,-10,10);
    fhDZ[i]                  = new TH1F("dZ"     ,"",500,-10,10);
    fhDXYDZ[i]               = new TH1F("dXYDZ"  ,"",500,0,10);
    fhDXYvsDZ[i]             = new TH2F("dXYvsDZ","",200,-10,10,200,-10,10);

    fhDXYNormalized[i]       = new TH1F("dXYNormalized"    ,"",500,-10,10);
    fhDZNormalized[i]        = new TH1F("dZNormalized"     ,"",500,-10,10);
    fhDXYvsDZNormalized[i]   = new TH2F("dXYvsDZNormalized","",200,-10,10,200,-10,10);

    fhNSigmaToVertex[i]      = new TH1F("nSigmaToVertex","",500,0,10);

    fhPt[i]                  = new TH1F("pt"     ,"p_{T} distribution;p_{T} (GeV/c)",500,0.0,100.0);
    fhEta[i]                 = new TH1F("eta"     ,"#eta distribution;#eta",40,-2.0,2.0);
    
    fhNClustersITS[i]->SetTitle("n ITS clusters");
    fhNClustersTPC[i]->SetTitle("n TPC clusters");
    fhChi2PerClusterITS[i]->SetTitle("#Chi^{2} per ITS cluster");
    fhChi2PerClusterTPC[i]->SetTitle("#Chi^{2} per TPC cluster");

    fhC11[i]->SetTitle("cov 11 : #sigma_{y}^{2} [cm^{2}]");
    fhC22[i]->SetTitle("cov 22 : #sigma_{z}^{2} [cm^{2}]");
    fhC33[i]->SetTitle("cov 33 : #sigma_{sin(#phi)}^{2}");
    fhC44[i]->SetTitle("cov 44 : #sigma_{tan(#theta_{dip})}^{2}");
    fhC55[i]->SetTitle("cov 55 : #sigma_{1/p_{T}}^{2} [(c/GeV)^2]");

    fhDXY[i]->SetTitle("transverse impact parameter");
    fhDZ[i]->SetTitle("longitudinal impact parameter");
    fhDXYDZ[i]->SetTitle("absolute impact parameter;sqrt(dXY**2 + dZ**2) in cm");
    fhDXYvsDZ[i]->SetXTitle("longitudinal impact parameter");
    fhDXYvsDZ[i]->SetYTitle("transverse impact parameter");

    fhDXYNormalized[i]->SetTitle("normalized trans impact par");
    fhDZNormalized[i]->SetTitle("normalized long impact par");
    fhDXYvsDZNormalized[i]->SetTitle("normalized long impact par");
    fhDXYvsDZNormalized[i]->SetYTitle("normalized trans impact par");
    fhNSigmaToVertex[i]->SetTitle("n #sigma to vertex");

    fhNClustersITS[i]->SetLineColor(color);   fhNClustersITS[i]->SetLineWidth(2);
    fhNClustersTPC[i]->SetLineColor(color);   fhNClustersTPC[i]->SetLineWidth(2);
    fhChi2PerClusterITS[i]->SetLineColor(color);   fhChi2PerClusterITS[i]->SetLineWidth(2);
    fhChi2PerClusterTPC[i]->SetLineColor(color);   fhChi2PerClusterTPC[i]->SetLineWidth(2);

    fhC11[i]->SetLineColor(color);   fhC11[i]->SetLineWidth(2);
    fhC22[i]->SetLineColor(color);   fhC22[i]->SetLineWidth(2);
    fhC33[i]->SetLineColor(color);   fhC33[i]->SetLineWidth(2);
    fhC44[i]->SetLineColor(color);   fhC44[i]->SetLineWidth(2);
    fhC55[i]->SetLineColor(color);   fhC55[i]->SetLineWidth(2);

    fhDXY[i]->SetLineColor(color);   fhDXY[i]->SetLineWidth(2);
    fhDZ[i]->SetLineColor(color);    fhDZ[i]->SetLineWidth(2);
    fhDXYDZ[i]->SetLineColor(color); fhDXYDZ[i]->SetLineWidth(2);

    fhDXYNormalized[i]->SetLineColor(color);   fhDXYNormalized[i]->SetLineWidth(2);
    fhDZNormalized[i]->SetLineColor(color);    fhDZNormalized[i]->SetLineWidth(2);
    fhNSigmaToVertex[i]->SetLineColor(color);  fhNSigmaToVertex[i]->SetLineWidth(2); 
  }

  // The number of sigmas to the vertex is per definition gaussian
  ffDTheoretical = new TF1("nSigmaToVertexTheoretical","([0]/2.506628274)*exp(-(x**2)/2)",0,50);
  ffDTheoretical->SetParameter(0,1);

  TH1::AddDirectory(oldStatus);
}

//____________________________________________________________________
Bool_t AliESDtrackCuts::LoadHistograms(const Char_t* dir)
{
  //
  // loads the histograms from a file
  // if dir is empty a directory with the name of this object is taken (like in SaveHistogram)
  //

  if (!dir)
    dir = GetName();

  if (!gDirectory->cd(dir))
    return kFALSE;

  ffDTheoretical = dynamic_cast<TF1*> (gDirectory->Get("nSigmaToVertexTheory"));

  fhCutStatistics = dynamic_cast<TH1F*> (gDirectory->Get("cut_statistics"));
  fhCutCorrelation = dynamic_cast<TH2F*> (gDirectory->Get("cut_correlation"));

  for (Int_t i=0; i<2; i++) {
    if (i==0)
    {
      gDirectory->cd("before_cuts");
    }
    else
      gDirectory->cd("after_cuts");

    fhNClustersITS[i]      = dynamic_cast<TH1F*> (gDirectory->Get("nClustersITS"     ));
    fhNClustersTPC[i]      = dynamic_cast<TH1F*> (gDirectory->Get("nClustersTPC"     ));
    fhChi2PerClusterITS[i] = dynamic_cast<TH1F*> (gDirectory->Get("chi2PerClusterITS"));
    fhChi2PerClusterTPC[i] = dynamic_cast<TH1F*> (gDirectory->Get("chi2PerClusterTPC"));

    fhC11[i] = dynamic_cast<TH1F*> (gDirectory->Get("covMatrixDiagonal11"));
    fhC22[i] = dynamic_cast<TH1F*> (gDirectory->Get("covMatrixDiagonal22"));
    fhC33[i] = dynamic_cast<TH1F*> (gDirectory->Get("covMatrixDiagonal33"));
    fhC44[i] = dynamic_cast<TH1F*> (gDirectory->Get("covMatrixDiagonal44"));
    fhC55[i] = dynamic_cast<TH1F*> (gDirectory->Get("covMatrixDiagonal55"));

    fhDXY[i] =     dynamic_cast<TH1F*> (gDirectory->Get("dXY"    ));
    fhDZ[i] =      dynamic_cast<TH1F*> (gDirectory->Get("dZ"     ));
    fhDXYDZ[i] =   dynamic_cast<TH1F*> (gDirectory->Get("dXYDZ"));
    fhDXYvsDZ[i] = dynamic_cast<TH2F*> (gDirectory->Get("dXYvsDZ"));

    fhDXYNormalized[i] =     dynamic_cast<TH1F*> (gDirectory->Get("dXYNormalized"    ));
    fhDZNormalized[i] =      dynamic_cast<TH1F*> (gDirectory->Get("dZNormalized"     ));
    fhDXYvsDZNormalized[i] = dynamic_cast<TH2F*> (gDirectory->Get("dXYvsDZNormalized"));
    fhNSigmaToVertex[i] = dynamic_cast<TH1F*> (gDirectory->Get("nSigmaToVertex"));

    fhPt[i] = dynamic_cast<TH1F*> (gDirectory->Get("pt"));
    fhEta[i] = dynamic_cast<TH1F*> (gDirectory->Get("eta"));

    gDirectory->cd("../");
  }

  gDirectory->cd("..");

  return kTRUE;
}

//____________________________________________________________________
void AliESDtrackCuts::SaveHistograms(const Char_t* dir) {
  //
  // saves the histograms in a directory (dir)
  //

  if (!fHistogramsOn) {
    AliDebug(0, "Histograms not on - cannot save histograms!!!");
    return;
  }

  if (!dir)
    dir = GetName();

  gDirectory->mkdir(dir);
  gDirectory->cd(dir);

  gDirectory->mkdir("before_cuts");
  gDirectory->mkdir("after_cuts");
 
  // a factor of 2 is needed since n sigma is positive
  ffDTheoretical->SetParameter(0,2*fhNSigmaToVertex[0]->Integral("width"));
  ffDTheoretical->Write("nSigmaToVertexTheory");

  fhCutStatistics->Write();
  fhCutCorrelation->Write();

  for (Int_t i=0; i<2; i++) {
    if (i==0)
      gDirectory->cd("before_cuts");
    else
      gDirectory->cd("after_cuts");

    fhNClustersITS[i]        ->Write();
    fhNClustersTPC[i]        ->Write();
    fhChi2PerClusterITS[i]   ->Write();
    fhChi2PerClusterTPC[i]   ->Write();

    fhC11[i]                 ->Write();
    fhC22[i]                 ->Write();
    fhC33[i]                 ->Write();
    fhC44[i]                 ->Write();
    fhC55[i]                 ->Write();

    fhDXY[i]                 ->Write();
    fhDZ[i]                  ->Write();
    fhDXYDZ[i]               ->Write();
    fhDXYvsDZ[i]             ->Write();

    fhDXYNormalized[i]       ->Write();
    fhDZNormalized[i]        ->Write();
    fhDXYvsDZNormalized[i]   ->Write();
    fhNSigmaToVertex[i]      ->Write();

    fhPt[i]                  ->Write();
    fhEta[i]                 ->Write();
    
    gDirectory->cd("../");
  }

  gDirectory->cd("../");
}

//____________________________________________________________________
void AliESDtrackCuts::DrawHistograms()
{
  // draws some histograms

  TCanvas* canvas1 = new TCanvas(Form("%s_1", GetName()), "Track Quality Results1", 800, 800);
  canvas1->Divide(2, 2);

  canvas1->cd(1);
  fhNClustersTPC[0]->SetStats(kFALSE);
  fhNClustersTPC[0]->Draw();

  canvas1->cd(2);
  fhChi2PerClusterTPC[0]->SetStats(kFALSE);
  fhChi2PerClusterTPC[0]->Draw();

  canvas1->cd(3);
  fhNSigmaToVertex[0]->SetStats(kFALSE);
  fhNSigmaToVertex[0]->GetXaxis()->SetRangeUser(0, 10);
  fhNSigmaToVertex[0]->Draw();

  canvas1->SaveAs(Form("%s_%s.gif", GetName(), canvas1->GetName()));

  TCanvas* canvas2 = new TCanvas(Form("%s_2", GetName()), "Track Quality Results2", 1200, 800);
  canvas2->Divide(3, 2);

  canvas2->cd(1);
  fhC11[0]->SetStats(kFALSE);
  gPad->SetLogy();
  fhC11[0]->Draw();

  canvas2->cd(2);
  fhC22[0]->SetStats(kFALSE);
  gPad->SetLogy();
  fhC22[0]->Draw();

  canvas2->cd(3);
  fhC33[0]->SetStats(kFALSE);
  gPad->SetLogy();
  fhC33[0]->Draw();

  canvas2->cd(4);
  fhC44[0]->SetStats(kFALSE);
  gPad->SetLogy();
  fhC44[0]->Draw();

  canvas2->cd(5);
  fhC55[0]->SetStats(kFALSE);
  gPad->SetLogy();
  fhC55[0]->Draw();

  canvas2->SaveAs(Form("%s_%s.gif", GetName(), canvas2->GetName()));

  TCanvas* canvas3 = new TCanvas(Form("%s_3", GetName()), "Track Quality Results3", 1200, 800);
  canvas3->Divide(3, 2);

  canvas3->cd(1);
  fhDXY[0]->SetStats(kFALSE);
  gPad->SetLogy();
  fhDXY[0]->Draw();

  canvas3->cd(2);
  fhDZ[0]->SetStats(kFALSE);
  gPad->SetLogy();
  fhDZ[0]->Draw();

  canvas3->cd(3);
  fhDXYvsDZ[0]->SetStats(kFALSE);
  gPad->SetLogz();
  gPad->SetRightMargin(0.15);
  fhDXYvsDZ[0]->Draw("COLZ");

  canvas3->cd(4);
  fhDXYNormalized[0]->SetStats(kFALSE);
  gPad->SetLogy();
  fhDXYNormalized[0]->Draw();

  canvas3->cd(5);
  fhDZNormalized[0]->SetStats(kFALSE);
  gPad->SetLogy();
  fhDZNormalized[0]->Draw();

  canvas3->cd(6);
  fhDXYvsDZNormalized[0]->SetStats(kFALSE);
  gPad->SetLogz();
  gPad->SetRightMargin(0.15);
  fhDXYvsDZNormalized[0]->Draw("COLZ");

  canvas3->SaveAs(Form("%s_%s.gif", GetName(), canvas3->GetName()));

  TCanvas* canvas4 = new TCanvas(Form("%s_4", GetName()), "Track Quality Results4", 800, 500);
  canvas4->Divide(2, 1);

  canvas4->cd(1);
  fhCutStatistics->SetStats(kFALSE);
  fhCutStatistics->LabelsOption("v");
  gPad->SetBottomMargin(0.3);
  fhCutStatistics->Draw();

  canvas4->cd(2);
  fhCutCorrelation->SetStats(kFALSE);
  fhCutCorrelation->LabelsOption("v");
  gPad->SetBottomMargin(0.3);
  gPad->SetLeftMargin(0.3);
  fhCutCorrelation->Draw("COLZ");

  canvas4->SaveAs(Form("%s_%s.gif", GetName(), canvas4->GetName()));

  /*canvas->cd(1);
  fhDXYvsDZNormalized[0]->SetStats(kFALSE);
  fhDXYvsDZNormalized[0]->DrawCopy("COLZ");

  canvas->cd(2);
  fhNClustersTPC[0]->SetStats(kFALSE);
  fhNClustersTPC[0]->DrawCopy();

  canvas->cd(3);
  fhChi2PerClusterITS[0]->SetStats(kFALSE);
  fhChi2PerClusterITS[0]->DrawCopy();
  fhChi2PerClusterITS[1]->SetLineColor(2);
  fhChi2PerClusterITS[1]->DrawCopy("SAME");

  canvas->cd(4);
  fhChi2PerClusterTPC[0]->SetStats(kFALSE);
  fhChi2PerClusterTPC[0]->DrawCopy();
  fhChi2PerClusterTPC[1]->SetLineColor(2);
  fhChi2PerClusterTPC[1]->DrawCopy("SAME");*/
}

Float_t AliESDtrackCuts::GetMinNsigmaToVertex() const
{
  // deprecated, please use GetMaxNsigmaToVertex

  Printf("WARNING: AliESDtrackCuts::GetMinNsigmaToVertex is DEPRECATED and will be removed in the next release. Please use GetMaxNsigmaToVertex instead. Renaming was done to improve code readability.");

  return GetMaxNsigmaToVertex();
}

void AliESDtrackCuts::SetMinNsigmaToVertex(Float_t sigma)
{
  // deprecated, will be removed in next release

  Printf("WARNING: AliESDtrackCuts::SetMinNsigmaToVertex is DEPRECATED and will be removed in the next release. Please use SetMaxNsigmaToVertex instead. Renaming was done to improve code readability.");
  
  SetMaxNsigmaToVertex(sigma);
}

void AliESDtrackCuts::SetDCAToVertex(Float_t dist)
{
  // deprecated, will be removed in next release

  Printf("WARNING: AliESDtrackCuts::SetDCAToVertex is DEPRECATED and will be removed in the next release. Please use SetMaxDCAToVertex instead. Renaming was done to improve code readability.");
  
  SetMaxDCAToVertex(dist);
}
  
void AliESDtrackCuts::SetDCAToVertexXY(Float_t dist)
{
  // deprecated, will be removed in next release

  Printf("WARNING: AliESDtrackCuts::SetDCAToVertexXY is DEPRECATED and will be removed in the next release. Please use SetMaxDCAToVertexXY instead. Renaming was done to improve code readability.");
  
  SetMaxDCAToVertexXY(dist);
}
  
void AliESDtrackCuts::SetDCAToVertexZ(Float_t dist)
{
  // deprecated, will be removed in next release

  Printf("WARNING: AliESDtrackCuts::SetDCAToVertexZ is DEPRECATED and will be removed in the next release. Please use SetMaxDCAToVertexZ instead. Renaming was done to improve code readability.");
  
  SetMaxDCAToVertexZ(dist);
}
