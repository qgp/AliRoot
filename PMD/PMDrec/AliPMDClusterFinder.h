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
class TTree;

class AliLoader;
class AliRunLoader;
class AliRawReader;
class AliPMDCalibData;
class AliPMDPedestal;
class AliPMDHotData;
class AliPMDNoiseCut;
class AliPMDddlinfoData;
class AliPMDRecoParam;

class AliPMDClusterFinder : public TObject
{

 public:

  AliPMDClusterFinder();
  AliPMDClusterFinder(AliRunLoader* runLoader);
  AliPMDClusterFinder(const AliPMDClusterFinder &finder);  // copy constructor
  AliPMDClusterFinder &operator=(const AliPMDClusterFinder &finder); // assignment op
  virtual ~AliPMDClusterFinder();

  void Digits2RecPoints(TTree *digitsTree, TTree *clustersTree,Int_t gRecoMode);
  void Digits2RecPoints(AliRawReader *rawReader, TTree *clustersTree,Int_t gRecoMode);

  void AddRecPoint(Int_t idet, Int_t ismn, Float_t * clusdata);
  void AddRecHit(Int_t celldataX, Int_t celldataY, Int_t celldataTr,
		 Int_t celldataPid, Float_t celldataAdc);
  void ResetCellADC();
  void ResetRecpoint();
  void ResetRechit();
  void Load();
  void LoadClusters();
  void UnLoad();
  void UnLoadClusters();

  AliPMDCalibData    *GetCalibGain() const;
  AliPMDPedestal     *GetCalibPed() const;
  AliPMDHotData      *GetCalibHot() const;
  AliPMDNoiseCut     *GetNoiseCut() const;
  AliPMDddlinfoData  *GetDdlinfoData() const;

 protected:
  AliRunLoader *fRunLoader; // Pointer to Run Loader
  AliLoader    *fPMDLoader; // Pointer to specific detector loader

  AliPMDCalibData    *fCalibGain;  //! Gain calibration data
  AliPMDPedestal     *fCalibPed;   //! Pedestal calibration data
  AliPMDHotData      *fCalibHot;   //! Hot data
  AliPMDNoiseCut     *fNoiseCut;   //! Noise cut
  AliPMDddlinfoData  *fDdlinfo;    //! ddl info data

  const AliPMDRecoParam *fRecoParam; // reconstruction parameter

  TTree        *fTreeD;     // Digits tree
  TTree        *fTreeR;     // Reconstructed points

  TClonesArray *fDigits;    // List of digits
  TClonesArray *fRecpoints; // List of reconstructed points
  TClonesArray *fRechits;   // List of cells associated with rec points

  Int_t   fNpoint;          // 
  Int_t   fNhit;            // 
  Int_t   fDetNo;           // Detector Number (0:PRE, 1:CPV)


  static const Int_t fgkRow = 48; // Total number of rows in one unitmodule
  static const Int_t fgkCol = 96; // Total number of cols in one unitmodule
  Double_t fCellADC[fgkRow][fgkCol]; // Array containing individual cell ADC
  Int_t    fCellTrack[fgkRow][fgkCol]; // Array containing individual cell tr
  Int_t    fCellPid[fgkRow][fgkCol]; // Array containing individual cell pid

  ClassDef(AliPMDClusterFinder,19) // 19 by satyajit
};
#endif

