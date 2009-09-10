#ifndef ALITRDCLUSTERIZER_H
#define ALITRDCLUSTERIZER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  TRD cluster finder                                                    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include <TNamed.h>

class TFile;
class TTree;
class TClonesArray;

class AliRunLoader;
class AliRawReader;

class AliTRD;
class AliTRDcluster;

class AliTRDarrayADC;
class AliTRDarraySignal;
class AliTRDdigitsManager;
class AliTRDSignalIndex;
class AliTRDtransform;
class AliTRDCalROC;
class AliTRDReconstructor;
class AliTRDCalSingleChamberStatus;

class AliTRDclusterizer : public TNamed 
{

 public:

  // steering flags
  enum{
    kTrOwner= BIT(13)  //  toggle online tracklets ownership
   ,kClOwner= BIT(14)  //  toggle cluster ownership
   ,kLabels = BIT(15)  //  toggle MC labels for clusters
   ,kHLT    = BIT(16)  //  HLT mode
   ,kLUT    = BIT(17)  //  using look up table for cluster's r-phi position
   ,kGAUS   = BIT(18)  //  using gauss approx. for cluster's r-phi position
  };

  struct MaxStruct
  {
    Int_t       Row;
    Int_t       Col;
    Int_t       Time;
    UChar_t     padStatus;
    Short_t     Signals[3];
    Bool_t      FivePad;
  MaxStruct():Row(-1),Col(0),Time(0),padStatus(0),FivePad(kFALSE)
      {memset(Signals, 0, 3*sizeof(Short_t));}
    MaxStruct &operator=(const MaxStruct &a)
    {Row=a.Row; Col=a.Col; Time=a.Time; padStatus=a.padStatus; FivePad=a.FivePad;
     memcpy(Signals, a.Signals, 3*sizeof(Short_t)); return *this;}
  };
  
  AliTRDclusterizer(const AliTRDReconstructor *const rec = 0x0);
  AliTRDclusterizer(const Text_t* name, const Text_t* title, const AliTRDReconstructor *const rec = 0x0);
  AliTRDclusterizer(const AliTRDclusterizer &c);
  virtual         ~AliTRDclusterizer();
  AliTRDclusterizer &operator=(const AliTRDclusterizer &c);

  void     Copy(TObject &c) const;

  Bool_t   Open(const Char_t *name, Int_t nEvent = 0);
  Bool_t   OpenInput(Int_t nEvent = 0);
  Bool_t   OpenOutput();
  Bool_t   OpenOutput(TTree *const clusterTree);
  Bool_t   OpenTrackletOutput();

  Bool_t   ReadDigits();
  Bool_t   ReadDigits(AliRawReader *rawReader);
  Bool_t   ReadDigits(TTree *digitsTree);

  Bool_t   WriteClusters(Int_t det);
  void     ResetRecPoints();
  virtual TClonesArray    *RecPoints();
  virtual TClonesArray    *TrackletsArray();
  Bool_t   WriteTracklets(Int_t det);

  Bool_t   Raw2Clusters(AliRawReader *rawReader);
  Bool_t   Raw2ClustersChamber(AliRawReader *rawReader);

  Bool_t   MakeClusters();
  Bool_t   MakeClusters(Int_t det);

  Bool_t   AddLabels();
  Bool_t   SetUseLabels(const Bool_t kset) { SetBit(kLabels, kset); return TestBit(kLabels);  } // should we assign labels to clusters
  void     SetRawVersion(const Int_t iver) { fRawVersion = iver; } // set the expected raw data version
  void             SetReconstructor(const AliTRDReconstructor *rec) {fReconstructor = rec;}
  static UChar_t   GetStatus(Short_t &signal);
  Int_t            GetAddedClusters() const {return fNoOfClusters;}

  Bool_t   IsClustersOwner() const {return TestBit(kClOwner);}
  virtual void     SetClustersOwner(Bool_t own=kTRUE) {SetBit(kClOwner, own); if(!own) {fRecPoints = 0x0; fNoOfClusters=0;} }
  void     SetTrackletsOwner(Bool_t own=kTRUE) {SetBit(kTrOwner, own); if(!own) {fTracklets = 0x0; } }

protected:

  void             DeConvExp (const Double_t *const source, Double_t *const target
			     ,const Int_t nTimeTotal, const Int_t nexp);
  void             TailCancelation();

  Float_t  Unfold(Double_t eps, Int_t layer, const Double_t *const padSignal) const;
  
  void             SetPadStatus(const UChar_t status, UChar_t &encoding) const;
  UChar_t          GetPadStatus(UChar_t encoding) const;
  Int_t            GetCorruption(UChar_t encoding) const;

  Bool_t           IsMaximum(const MaxStruct &Max, UChar_t &padStatus, Short_t *const Signals);       //for const correctness reasons not const parameters are given separately
  Bool_t           FivePadCluster(MaxStruct &ThisMax, MaxStruct &NeighbourMax);
  void             CreateCluster(const MaxStruct &Max); 
  inline void      CalcAdditionalInfo(const MaxStruct &Max, Short_t *const signals, Int_t &nPadCount);
  virtual void     AddClusterToArray(AliTRDcluster* cluster);
  virtual void     AddTrackletsToArray();

  const AliTRDReconstructor *fReconstructor; //! reconstructor
  AliRunLoader        *fRunLoader;           //! Run Loader
  TTree               *fClusterTree;         //! Tree with the cluster
  TClonesArray        *fRecPoints;           //! Array of clusters
  TClonesArray        *fTracklets;           //! Array of online tracklets

  TTree               *fTrackletTree;        //! Tree for tracklets

  AliTRDdigitsManager *fDigitsManager;       //! TRD digits manager

  UInt_t              **fTrackletContainer;  //! tracklet container

  Int_t                fRawVersion;          //  Expected raw version of the data - default is 2

  AliTRDtransform     *fTransform;           //! Transforms the reconstructed space points

  AliTRDarrayADC      *fDigits;               // Array holding the digits
  AliTRDSignalIndex   *fIndexes;              // Array holding the indexes to the digits
  Float_t              fADCthresh;            // ADC thresholds: There is no ADC threshold anymore, and simParam should not be used in clusterizer. KO
  Float_t              fMaxThresh;            // Threshold value for the maximum
  Float_t              fSigThresh;            // Threshold value for the digit signal
  Float_t              fMinMaxCutSigma;       // Threshold value for the maximum (cut noise)
  Float_t              fMinLeftRightCutSigma; // Threshold value for the sum pad (cut noise)
  Int_t                fLayer;                // Current layer of the detector
  Int_t                fDet;                  // Current detecor
  UShort_t             fVolid;                // Volume ID
  Int_t                fColMax;               // Number of Colums in one detector
  Int_t                fTimeTotal;            // Number of time bins
  AliTRDCalROC        *fCalGainFactorROC;     // Calibration object with pad wise values for the gain factors
  Float_t              fCalGainFactorDetValue;// Calibration value for chamber wise noise
  AliTRDCalROC        *fCalNoiseROC;          // Calibration object with pad wise values for the noise
  Float_t              fCalNoiseDetValue;     // Calibration value for chamber wise noise
  AliTRDCalSingleChamberStatus *fCalPadStatusROC; // Calibration object with the pad status
  Int_t                fClusterROC;           // The index to the first cluster of a given ROC
  Int_t                firstClusterROC;       // The number of cluster in a given ROC
  Int_t                fNoOfClusters;         // Number of Clusters already processed and still owned by the clusterizer

  ClassDef(AliTRDclusterizer,6)               //  TRD clusterfinder

};

#endif
