#ifndef AliTPCCALIBTRACKSGAIN_H
#define AliTPCCALIBTRACKSGAIN_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include <TChain.h>
#include <TNamed.h>


#include <TObjArray.h>
#include <TH2D.h>
#include <TVectorD.h>
#include <TMatrixD.h>

#include <iostream>
#include <TH1F.h>
#include <TProfile.h> 
#include <TProfile2D.h> 

using namespace std;

class TTreeSRedirector;
class TObjString;
class TLinearFitter;
/* class TProfile;  */
/* class TProfile2D;  */
class TH1F; 

class AliTPCClusterParam; 
class AliTPCParamSR; 
class AliTPCCalROC;
class AliTPCCalPad;
class AliTPCseed; 
class AliTPCclusterMI; 
class AliTrackPointArray;
class TTreeStream;
class AliTPCcalibTracksCuts;
class AliTPCFitPad;
class TGraph;
class AliTPCCClusterParam;

class AliTPCcalibTracksGain : public TNamed {
public:
   enum {
      kShortPads = 0,
      kMediumPads = 1,
      kLongPads = 2
   };
   enum {
      kSimpleFitter = 0,
      kSqrtFitter = 1,
      kLogFitter = 2
   };
   
  AliTPCcalibTracksGain();
  AliTPCcalibTracksGain(const AliTPCcalibTracksGain& obj);
  AliTPCcalibTracksGain(const char* name, const char* title, AliTPCcalibTracksCuts* cuts, TNamed* debugStreamPrefix = 0, AliTPCcalibTracksGain* prevIter = 0);
  virtual ~AliTPCcalibTracksGain();
  AliTPCcalibTracksGain& operator=(const AliTPCcalibTracksGain& rhs);
  static void     AddInfo(TChain* chain, char* debugStreamPrefix = 0, char* prevIterFileName = 0);
  void            Terminate();
  //
  // Tracks manipulation
  //
  void            Process(AliTPCseed* seed);
  void            AddTrack(AliTPCseed* seed);
  Bool_t          AcceptTrack(AliTPCseed* track);
  void            DumpTrack(AliTPCseed* track);
  Bool_t          GetDedx(AliTPCseed* track, Int_t padType,  Int_t*rows,
			  Int_t &sector, Int_t& npoints, 
			  TVectorD &dedxM, TVectorD &dedxQ, 
			  TVectorD &parY, TVectorD &parZ, TVectorD & meanPos);
  void            AddCluster(AliTPCclusterMI* cluster, Float_t momenta, Float_t mdedx, Int_t padType, Float_t xcenter, TVectorD &dedxQ, TVectorD & dedxM, Float_t fraction, Float_t fraction2, Float_t dedge, TVectorD& parY, TVectorD& parZ, TVectorD& meanPos);
  void            AddTracklet(UInt_t sector, UInt_t padType,TVectorD &dedxQ, TVectorD &dedxM,TVectorD& parY, TVectorD& parZ, TVectorD& meanPos);

  void            AddCluster(AliTPCclusterMI* cluster);

  //
  // Merging of the component
  //
  Long64_t        Merge(TCollection *list);
  void            Add(AliTPCcalibTracksGain* cal);
  //
  // Histogram part
  //
  TH1F  * GetQM(Int_t sector=-1){return (TH1F*)(sector<0 ?  fArrayQM->At(72): fArrayQM->At(sector));}
  TH1F  * GetQT(Int_t sector=-1){return (TH1F*)(sector<0 ?  fArrayQT->At(72): fArrayQT->At(sector));}
  TProfile* GetProfileQM(Int_t sector){return (TProfile*)(sector<0 ? fProfileArrayQM->At(36): fProfileArrayQM->At(sector));}
  TProfile* GetProfileQT(Int_t sector){return (TProfile*)(sector<0 ? fProfileArrayQT->At(36): fProfileArrayQT->At(sector));}
  TProfile2D* GetProfileQM2D(Int_t sector){return (TProfile2D*)(sector<0 ? fProfileArrayQM2D->At(36): fProfileArrayQM2D->At(sector));}
  TProfile2D* GetProfileQT2D(Int_t sector){return (TProfile2D*)(sector<0 ? fProfileArrayQT2D->At(36): fProfileArrayQT2D->At(sector));}
  //
  // Get Derived results - gain maps
  //
  AliTPCCalPad*   CreateFitCalPad(UInt_t fitType, Bool_t undoTransformation = kFALSE, Bool_t normalizeToPadSize = kFALSE);
  AliTPCCalROC*   CreateFitCalROC(UInt_t sector, UInt_t fitType, Bool_t undoTransformation = kFALSE, Bool_t normalizeToPadSize = kFALSE);
  AliTPCCalROC*   CreateFitCalROC(UInt_t sector, UInt_t padType, TVectorD &fitParam, UInt_t fitType, Bool_t undoTransformation = kFALSE, Bool_t normalizeToPadSize = kFALSE);
  AliTPCCalROC*   CreateCombinedCalROC(const AliTPCCalROC* roc1, const AliTPCCalROC* roc2);
  //
  void            Evaluate(Bool_t robust = kFALSE, Double_t frac = -1.);
  void            GetParameters(UInt_t segment, UInt_t padType, UInt_t fitType, TVectorD &fitParam);
  void            GetErrors(UInt_t segment, UInt_t padType, UInt_t fitType, TVectorD &fitError);
  Double_t        GetRedChi2(UInt_t segment, UInt_t padType, UInt_t fitType);
  void            GetCovarianceMatrix(UInt_t segment, UInt_t padType, UInt_t fitType, TMatrixD& covMatrix);
  //
  //
  void            UpdateClusterParam(AliTPCClusterParam *param);
  TGraph *        CreateAmpGraph(Int_t ipad, Bool_t qmax);


  TLinearFitter*  GetFitter(UInt_t segment, UInt_t padType, UInt_t fitType);
public:
  //
  // Helper function
  //
  static Double_t GetPadLength(Double_t lx);
  static Int_t    GetPadType(Double_t lx);  
  static Bool_t   GetRowPad(Double_t lx, Double_t ly, Int_t& row, Int_t& pad); // just for debugging
  //
  //
  TTreeSRedirector* fDebugStream;          //! debug stream for debugging
  AliTPCcalibTracksCuts* fCuts;            // cuts that are used for sieving the tracks used for calibration
  //
  // Simple Profiles and histograms - per chambers + 1 total
  //
  TObjArray*        fArrayQM;                // Qmax normalized
  TObjArray*        fArrayQT;                // Qtot normalized 
  TObjArray*        fProfileArrayQM;         // Qmax normalized  versus local X
  TObjArray*        fProfileArrayQT;         // Qtot normalized  versus local X 
  TObjArray*        fProfileArrayQM2D;       // Qmax normalized  versus local X and phi
  TObjArray*        fProfileArrayQT2D;       // Qtot normalized  versus local X and phi
  //
  // Fitters
  //
  AliTPCFitPad*     fSimpleFitter;         // simple fitter for short pads
  AliTPCFitPad*     fSqrtFitter;           // sqrt fitter for medium pads
  AliTPCFitPad*     fLogFitter;            // log fitter for long pads
  TLinearFitter*    fFitter0M;          // fitting of the atenuation, angular correction, and mean chamber gain
  TLinearFitter*    fFitter1M;          // fitting of the atenuation, angular correction, and mean chamber gain
  TLinearFitter*    fFitter2M;          // fitting of the atenuation, angular correction, and mean chamber gain
  TLinearFitter*    fFitter0T;          // fitting of the atenuation, angular correction, and mean chamber gain
  TLinearFitter*    fFitter1T;          // fitting of the atenuation, angular correction, and mean chamber gain
  TLinearFitter*    fFitter2T;          // fitting of the atenuation, angular correction, and mean chamber gain
  AliTPCFitPad*     fSingleSectorFitter;   // just for debugging
  //
  // Conters
  //
  UInt_t          fTotalTracks;         // just for debugging
  UInt_t          fAcceptedTracks;      // just for debugging
  AliTPCCalPad*   fDebugCalPadRaw;      // just for debugging
  AliTPCCalPad*   fDebugCalPadCorr;     // just for debugging
  UInt_t          fNShortClusters[36];   // number of clusters registered on short pads
  UInt_t          fNMediumClusters[36];  // number of clusters registered on medium pads
  UInt_t          fNLongClusters[36];    // number of clusters registered on long pads
  //
  //
  AliTPCcalibTracksGain* fPrevIter;        // the calibration object in its previous iteration (will not be owned by the new object, don't forget to delete it!)    
  //
  // Setup
  //
   static       AliTPCParamSR* fgTPCparam;              //! helper object for geometry related operations
   static const Double_t       fgkM;                    // value used in the transformation of the charge values for the logarithmic fitter
   static const char*          fgkDebugStreamFileName;  // filename of the debug stream file
   static const Bool_t         fgkUseTotalCharge;       // whether to use the cluster's total or maximum charge

   ClassDef(AliTPCcalibTracksGain, 1);
};

#endif
