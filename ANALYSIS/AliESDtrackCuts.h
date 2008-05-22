//
//  Class for handling of ESD track cuts.
//
//  The class manages a number of track quality cuts, a
//  track-to-vertex cut and a number of kinematic cuts. Two methods
//  can be used to figure out if an ESD track survives the cuts:
//  AcceptTrack which takes a single AliESDtrack as argument and
//  returns kTRUE/kFALSE or GetAcceptedTracks which takes an AliESD
//  object and returns an TObjArray (of AliESDtracks) with the tracks
//  in the ESD that survived the cuts.
//
//
//  TODO: 
//  - add functionality to save and load cuts
//  - add different ways to make track to vertex cut
//  - add histograms for kinematic cut variables?
//  - upper and lower cuts for all (non-boolean) cuts
//  - update print method
//  - is there a smarter way to manage the cuts?
//  - put comments to each variable
//

#ifndef ALIESDTRACKCUTS_H
#define ALIESDTRACKCUTS_H

#include <TF1.h>
#include <TH2.h>
#include "AliAnalysisCuts.h"

class AliESD;
class AliESDEvent;
class AliESDtrack;
class AliLog;
class TTree;

class AliESDtrackCuts : public AliAnalysisCuts
{
public:
  AliESDtrackCuts(const Char_t* name = "AliESDtrackCuts", const Char_t* title = "");
  virtual ~AliESDtrackCuts();
  Bool_t IsSelected(TObject* obj)
       {return AcceptTrack((AliESDtrack*)obj);}
  Bool_t IsSelected(TList* /*list*/) {return kTRUE;}
  Bool_t AcceptTrack(AliESDtrack* esdTrack);
  TObjArray* GetAcceptedTracks(AliESD* esd);
  Int_t CountAcceptedTracks(AliESD* esd);
  TObjArray* GetAcceptedTracks(AliESDEvent* esd);
  Int_t CountAcceptedTracks(AliESDEvent* esd);

  virtual Long64_t Merge(TCollection* list);
  virtual void Copy(TObject &c) const;
  AliESDtrackCuts(const AliESDtrackCuts& pd);  // Copy Constructor
  AliESDtrackCuts &operator=(const AliESDtrackCuts &c);

  //######################################################
  // track quality cut setters  
  void SetMinNClustersTPC(Int_t min=-1)          {fCutMinNClusterTPC=min;}
  void SetMinNClustersITS(Int_t min=-1)          {fCutMinNClusterITS=min;}
  void SetMaxChi2PerClusterTPC(Float_t max=1e10) {fCutMaxChi2PerClusterTPC=max;}
  void SetMaxChi2PerClusterITS(Float_t max=1e10) {fCutMaxChi2PerClusterITS=max;}
  void SetRequireTPCRefit(Bool_t b=kFALSE)       {fCutRequireTPCRefit=b;}
  void SetRequireITSRefit(Bool_t b=kFALSE)       {fCutRequireITSRefit=b;}
  void SetAcceptKingDaughters(Bool_t b=kFALSE)   {fCutAcceptKinkDaughters=b;}
  void SetMaxCovDiagonalElements(Float_t c1=1e10, Float_t c2=1e10, Float_t c3=1e10, Float_t c4=1e10, Float_t c5=1e10) 
    {fCutMaxC11=c1; fCutMaxC22=c2; fCutMaxC33=c3; fCutMaxC44=c4; fCutMaxC55=c5;}

  // track to vertex cut setters
  void SetMinNsigmaToVertex(Float_t sigma=1e10)       {fCutNsigmaToVertex = sigma;}
  void SetRequireSigmaToVertex(Bool_t b=kTRUE )       {fCutSigmaToVertexRequired = b;}

  // getters
  Float_t GetMinNsigmaToVertex()       { return fCutNsigmaToVertex;}
  Bool_t GetRequireSigmaToVertex( )    { return fCutSigmaToVertexRequired;}

  // track kinmatic cut setters
  void SetPRange(Float_t r1=0, Float_t r2=1e10)       {fPMin=r1;   fPMax=r2;}
  void SetPtRange(Float_t r1=0, Float_t r2=1e10)      {fPtMin=r1;  fPtMax=r2;}
  void SetPxRange(Float_t r1=-1e10, Float_t r2=1e10)  {fPxMin=r1;  fPxMax=r2;}
  void SetPyRange(Float_t r1=-1e10, Float_t r2=1e10)  {fPyMin=r1;  fPyMax=r2;}
  void SetPzRange(Float_t r1=-1e10, Float_t r2=1e10)  {fPzMin=r1;  fPzMax=r2;}
  void SetEtaRange(Float_t r1=-1e10, Float_t r2=1e10) {fEtaMin=r1; fEtaMax=r2;}
  void SetRapRange(Float_t r1=-1e10, Float_t r2=1e10) {fRapMin=r1; fRapMax=r2;}

  //######################################################
  void SetHistogramsOn(Bool_t b=kFALSE) {fHistogramsOn = b;}
  void DefineHistograms(Int_t color=1);
  virtual Bool_t LoadHistograms(const Char_t* dir = 0);
  void SaveHistograms(const Char_t* dir = 0);
  void DrawHistograms();

  Float_t GetSigmaToVertex(AliESDtrack* esdTrack);
  
  static void EnableNeededBranches(TTree* tree);

  // void SaveQualityCuts(Char_t* file)
  // void LoadQualityCuts(Char_t* file)

	TH1* GetDZNormalized(Int_t i) const { return fhDZNormalized[i]; }

protected:
  void Init(); // sets everything to 0

  enum { kNCuts = 21 };

  //######################################################
  // esd track quality cuts
  static const Char_t* fgkCutNames[kNCuts]; //! names of cuts (for internal use)

  Int_t   fCutMinNClusterTPC;         // min number of tpc clusters
  Int_t   fCutMinNClusterITS;         // min number of its clusters

  Float_t fCutMaxChi2PerClusterTPC;   // max tpc fit chi2 per tpc cluster
  Float_t fCutMaxChi2PerClusterITS;   // max its fit chi2 per its cluster

  Float_t fCutMaxC11;                 // max cov. matrix diag. elements (res. y^2)
  Float_t fCutMaxC22;                 // max cov. matrix diag. elements (res. z^2)
  Float_t fCutMaxC33;                 // max cov. matrix diag. elements (res. sin(phi)^2)
  Float_t fCutMaxC44;                 // max cov. matrix diag. elements (res. tan(theta_dip)^2)
  Float_t fCutMaxC55;                 // max cov. matrix diag. elements (res. 1/pt^2)

  Bool_t  fCutAcceptKinkDaughters;    // accepting kink daughters?
  Bool_t  fCutRequireTPCRefit;        // require TPC refit
  Bool_t  fCutRequireITSRefit;        // require ITS refit

  // track to vertex cut
  Float_t fCutNsigmaToVertex;         // max number of estimated sigma from track-to-vertex
  Bool_t  fCutSigmaToVertexRequired;  // cut track if sigma from track-to-vertex could not be calculated

  // esd kinematics cuts
  Float_t fPMin,   fPMax;             // definition of the range of the P
  Float_t fPtMin,  fPtMax;            // definition of the range of the Pt
  Float_t fPxMin,  fPxMax;            // definition of the range of the Px
  Float_t fPyMin,  fPyMax;            // definition of the range of the Py
  Float_t fPzMin,  fPzMax;            // definition of the range of the Pz
  Float_t fEtaMin, fEtaMax;           // definition of the range of the eta
  Float_t fRapMin, fRapMax;           // definition of the range of the y

  //######################################################
  // diagnostics histograms
  Bool_t fHistogramsOn;               // histograms on/off

  TH1F* fhNClustersITS[2];            //->
  TH1F* fhNClustersTPC[2];            //->

  TH1F* fhChi2PerClusterITS[2];       //->
  TH1F* fhChi2PerClusterTPC[2];       //->

  TH1F* fhC11[2];                     //->
  TH1F* fhC22[2];                     //->
  TH1F* fhC33[2];                     //->
  TH1F* fhC44[2];                     //->
  TH1F* fhC55[2];                     //->

  TH1F* fhDXY[2];                     //->
  TH1F* fhDZ[2];                      //->
  TH2F* fhDXYvsDZ[2];                 //->

  TH1F* fhDXYNormalized[2];           //->
  TH1F* fhDZNormalized[2];            //->
  TH2F* fhDXYvsDZNormalized[2];       //->
  TH1F* fhNSigmaToVertex[2];          //->
  
  TH1F* fhPt[2];                      //-> pt of esd tracks
  TH1F* fhEta[2];                     //-> eta of esd tracks

  TF1*  ffDTheoretical;               //-> theoretical distance to vertex normalized (2d gauss)

  TH1F*  fhCutStatistics;             //-> statistics of what cuts the tracks did not survive
  TH2F*  fhCutCorrelation;            //-> 2d statistics plot

  ClassDef(AliESDtrackCuts, 2)
};


#endif
