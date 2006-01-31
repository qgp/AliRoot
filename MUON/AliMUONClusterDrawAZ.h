#ifndef ALIMUONCLUSTERDRAWAZ_H
#define ALIMUONCLUSTERDRAWAZ_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/// \ingroup rec
/// \class AliMUONClusterDrawAZ
/// \brief Cluster drawing object for AZ cluster finder in MUON arm of ALICE

#include "AliMUONClusterDrawAZ.h"

class TH2D;
class AliMUONData;
class AliMUONPixel;
class AliMUONClusterFinderAZ;

class AliMUONClusterDrawAZ : public TObject 
{
public:
  AliMUONClusterDrawAZ(); // default constructor
  AliMUONClusterDrawAZ(AliMUONClusterFinderAZ *clusFinder); // Constructor
  virtual ~AliMUONClusterDrawAZ(); // Destructor

  void     DrawCluster(); // draw precluster
  void     AdjustHist(Double_t *xylim, const AliMUONPixel *pixPtr);
  void     DrawHist(const char* canvas, TH2D *hist); // draw histogram in canvas
  Int_t    Next(); // commands for drawing
  Bool_t   FindEvCh(Int_t nev, Int_t ch); // find requested event and chamber
  void     FillMuon(Int_t nfit, const Double_t *parOk, const Double_t *errOk); // fill muon info
  void     ResetMuon() { fxyMu[0][6] = fxyMu[1][6] = 9999; } // reset muons

protected:
  AliMUONClusterDrawAZ(const AliMUONClusterDrawAZ& rhs);
  AliMUONClusterDrawAZ& operator=(const AliMUONClusterDrawAZ& rhs);

private:
  AliMUONData *fData; //! pointer to muon data container
  AliMUONClusterFinderAZ* fFind; //! pointer to ClusterFinder
  TH2D*      fHist[4]; // ! histograms
  Int_t      fnMu; // ! number of muons passing thru the selected area
  Double_t   fxyMu[2][7]; // ! muon information
  Int_t      fEvent; // ! current event
  Int_t      fChamber; //! current chamber
  Int_t      fDebug; // ! debug level

  // Functions

  void   Init(); // initialization
  void   ModifyHistos(); // modify histograms
  void   DrawHits(); // draw simulated and reconstructed hits

ClassDef(AliMUONClusterDrawAZ,0) // cluster drawing for MUON arm of ALICE
};

#endif
