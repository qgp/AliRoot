#ifndef ALIPMDCLUSTERFINDER_H
#define ALIPMDCLUSTERFINDER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
//-----------------------------------------------------//
//                                                     //
//  Date   : August 05 2003                            //
//  This reads the file PMD.digits.root(TreeD),        //
//  calls the Clustering algorithm and stores the      //
//  clustering output in PMD.RecPoints.root(TreeR)     // 
//                                                     //
//-----------------------------------------------------//

class TClonesArray;
class TFile;
class TObjArray;
class TTree;
class TNtuple;

class AliLoader;
class AliRunLoader;
class AliRun;
class AliDetector;
class AliHeader;

class AliPMDdigit;
class AliPMDClustering;
class AliPMDcluster;
class AliPMDrecpoint1;

class AliPMDClusterFinder
{

 public:

  AliPMDClusterFinder();
  virtual ~AliPMDClusterFinder();

  void OpengAliceFile(char * file, Option_t * option);

  void Digits2RecPoints(Int_t ievt);
  void SetCellEdepCut(Float_t ecut);
  void SetDebug(Int_t idebug);
  void AddRecPoint(Float_t * clusdata);
  void ResetCellADC();
  void ResetRecpoint();
  void UnLoad(Option_t * option);

 protected:
  AliRunLoader *fRunLoader; // Pointer to Run Loader
  AliDetector  *fPMD;       // Pointers to Alice detectors & Hits containers
  AliLoader    *fPMDLoader; // Pointer to specific detector loader

  TTree        *fTreeD;     // Digits tree
  TTree        *fTreeR;     // Reconstructed points

  TClonesArray *fDigits;    // List of digits
  TClonesArray *fRecpoints; // List of reconstructed points

  Int_t   fNpoint;          // 
  Int_t   fDetNo;           // Detector Number (0:PRE, 1:CPV)
  Int_t   fDebug;           // Debugging switch (0:NO, 1:YES)
  Float_t fEcut;            // Energy/ADC cut per cell

  static const Int_t fgkRow = 48; // Total number of rows in one unitmodule
  static const Int_t fgkCol = 96; // Total number of cols in one unitmodule
  Double_t fCellADC[fgkRow][fgkCol]; // Array containing individual cell ADC

  ClassDef(AliPMDClusterFinder,2) // To run PMD clustering
};
#endif

