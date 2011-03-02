#ifndef ALIANALYSISTASKITSALIGNQA
#define ALIANALYSISTASKITSALIGNQA

/* Copyright(c) 1998-2012, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//*************************************************************************
// Class AliAnalysiTaskITSAlignQA
// AliAnalysisTaskSE to extract from ESD + ESDfriends 
// the track-to-point residuals and dE/dx vs, time for SDD modules
//
// Author: F. Prino, prino@to.infn.it
//*************************************************************************

class TList;
class TH1F;
class TH2F;
class TTree;
class TString;
class AliESDEvent;
class AliESDfriend;
class AliITSTPArrayFit;

#include "AliAnalysisTaskSE.h"

class AliAnalysisTaskITSAlignQA : public AliAnalysisTaskSE {

 public:
  
  AliAnalysisTaskITSAlignQA();
  virtual ~AliAnalysisTaskITSAlignQA();

  virtual void   UserExec(Option_t *option);
  virtual void   UserCreateOutputObjects();
  virtual void   Terminate(Option_t *option);

  void SetDoSPDResiduals(Bool_t opt){
    fDoSPDResiduals=opt;
  }
  void SetDoSDDResiduals(Bool_t opt){
    fDoSDDResiduals=opt;
  }
  void SetDoSSDResiduals(Bool_t opt){
    fDoSSDResiduals=opt;
  }
  void SetDoSDDdEdxCalib(Bool_t opt){
    fDoSDDdEdxCalib=opt;
  }
  void SetDoAllResiduals(){
    fDoSPDResiduals=kTRUE;
    fDoSDDResiduals=kTRUE;
    fDoSSDResiduals=kTRUE;
  }
  void SetDoAll(){
    SetDoAllResiduals();
    fDoSDDdEdxCalib=kTRUE;    
  }

  void SetUseITSstandaloneTracks(Bool_t use){
    fUseITSsaTracks=use;
  }
  void SetMinITSPoints(Int_t minp=3){
    fMinITSpts=minp;
  }
  void SetMinTPCPoints(Int_t minp=70){
    fMinTPCpts=minp;
  }
  void SetMinPt(Float_t minpt=1.0){
    fMinPt=minpt;
  }
  
  void     SetOCDBInfo(UInt_t runNb, const char *location) {
    fRunNb=runNb; 
    fOCDBLocation=location;
  }

  Bool_t   AcceptTrack(AliESDtrack * track);

  void     CreateSPDHistos();
  void     CreateSDDHistos();
  void     CreateSSDHistos();

  void     FitAndFillSPD(Int_t iLayer, const AliTrackPointArray *array, Int_t npts, AliESDtrack * track);
  void     FitAndFillSDD(const AliTrackPointArray *array, Int_t npts, AliESDtrack * track);
  void     FitAndFillSSD(Int_t iLayer, const AliTrackPointArray *array, Int_t npts, AliESDtrack * track);
  void     SetPtBinLimits(Int_t nBins, Double_t* xbins){
    fNPtBins=nBins;
    if(nBins>kMaxPtBins) fNPtBins=kMaxPtBins;
    for(Int_t iBin=0; iBin<=fNPtBins; iBin++) fPtBinLimits[iBin]=xbins[iBin];
  }
  void     LoadGeometryFromOCDB();

 private:
  AliAnalysisTaskITSAlignQA(const AliAnalysisTaskITSAlignQA &source);
  AliAnalysisTaskITSAlignQA& operator=(const AliAnalysisTaskITSAlignQA &source);
  
  enum {kNSPDmods = 240};
  enum {kNSDDmods = 260};
  enum {kNSSDmods = 1698};
  enum {kMaxPtBins = 12};

  TList* fOutput;              //! Histos with residuals
  TH1F*  fHistNEvents;         //! histo with N of events  
  TH1F*  fHistPtAccept;        //! histo of pt distribution of accepted tracks 

  TH2F*  fHistSPDResidX[kNSPDmods];       //! histos of SPD residuals along Xloc vs. Pt
  TH2F*  fHistSPDResidZ[kNSPDmods];       //! histos of SPD residuals along Zloc vs. Pt
  TH2F*  fHistSDDResidX[kNSSDmods];       //! histos of SDD residuals along Xloc vs. Pt
  TH2F*  fHistSDDResidZ[kNSSDmods];       //! histos of SDD residuals along Zloc vs. Pt
  TH2F*  fHistSSDResidX[kNSSDmods];       //! histos of SSD residuals along Xloc vs. Pt
  TH2F*  fHistSSDResidZ[kNSSDmods];       //! histos of SSD residuals along Zloc vs. Pt

  TH2F*  fHistSDDResidXvsX[kNSDDmods];    //! histos of SDD residuals along Xloc vs. Xloc
  TH2F*  fHistSDDResidXvsZ[kNSDDmods];    //! histos of SDD residuals along Xloc vs. Zloc
  TH2F*  fHistSDDResidZvsX[kNSDDmods];    //! histos of SDD residuals along Zloc vs. Xloc
  TH2F*  fHistSDDResidZvsZ[kNSDDmods];    //! histos of SDD residuals along Zloc vs. Zloc
  TH2F*  fHistSDDdEdxvsDrTime[kNSDDmods]; //! histos of SDD dE/dx vs. drift time
  TH1F*  fHistSDDDrTimeAll[kNSDDmods];    //! histos of SDD drift time (all clusters)
  TH1F*  fHistSDDDrTimeExtra[kNSDDmods];  //! histos of SDD drift time (extra clusters)
  TH1F*  fHistSDDDrTimeAttac[kNSDDmods];  //! histos of SDD drift time (attached clusters)
  

  Bool_t   fDoSPDResiduals;   // Flag to enable histos of SPD residuals
  Bool_t   fDoSDDResiduals;   // Flag to enable histos of SDD residuals
  Bool_t   fDoSSDResiduals;   // Flag to enable histos of SSD residuals
  Bool_t   fDoSDDdEdxCalib;   // Flag to enable histos for SDD dE/dx calibration
  Bool_t   fUseITSsaTracks;   // Flag for using standalone ITS tracks
  Int_t    fMinITSpts;        // Minimum number of ITS points per track
  Int_t    fMinTPCpts;        // Minimum number of TPC points per track
  Float_t  fMinPt;            // Minimum pt to accept tracks
  Int_t    fNPtBins;          // number of pt bins
  Double_t fPtBinLimits[kMaxPtBins+1];  // limits of Pt bins

  AliITSTPArrayFit* fFitter;  // Track Point fitter
  Int_t fRunNb;               // Run number
  TString fOCDBLocation;      // OCDB location

  ClassDef(AliAnalysisTaskITSAlignQA,1);
};


#endif
