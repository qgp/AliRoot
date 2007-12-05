// @(#) $Id$
// Original: AliHLTClustFinderNew.h,v 1.13 2004/06/18 10:55:26 loizides 

#ifndef AliHLTTPC_CLUSTERFINDER
#define AliHLTTPC_CLUSTERFINDER
/* This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

/** @file   AliHLTTPCClusterFinder.h
    @author Anders Vestbo, Constantin Loizides, Jochen Thaeder
	    Kenneth Aamodt kenneth.aamodt@student.uib.no
    @date   
    @brief  Cluster Finder for the TPC
*/

// see below for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt


#include "AliHLTLogging.h"
#include "AliHLTTPCPadArray.h"
class AliHLTTPCSpacePointData;
class AliHLTTPCDigitReader;

class AliHLTTPCClusterFinder : public AliHLTLogging {

 public:
  struct AliClusterData
  {
    UInt_t fTotalCharge;   //tot charge of cluster
    UInt_t fPad;           //pad value
    UInt_t fTime;          //time value
    ULong64_t fPad2;       //for error in XY direction
    ULong64_t fTime2;      //for error in Z  direction
    UInt_t fMean;          //mean in time
    UInt_t fFlags;         //different flags
    UInt_t fChargeFalling; //for deconvolution
    UInt_t fLastCharge;    //for deconvolution
    UInt_t fLastMergedPad; //dont merge twice per pad
    Int_t fRow;             //row value
  };
  typedef struct AliClusterData AliClusterData; //!

  /** standard constructor */
  AliHLTTPCClusterFinder();
  /** destructor */
  virtual ~AliHLTTPCClusterFinder();

  void Read(void* ptr,unsigned long size);

  void InitSlice(Int_t slice,Int_t patch,Int_t firstrow, Int_t lastrow,Int_t maxpoints);
  void InitSlice(Int_t slice,Int_t patch,Int_t maxpoints);
  void ProcessDigits();

  void SetOutputArray(AliHLTTPCSpacePointData *pt);
  void WriteClusters(Int_t n_clusters,AliClusterData *list);

  void SetXYError(Float_t f) {fXYErr=f;}
  void SetZError(Float_t f) {fZErr=f;}
  void SetDeconv(Bool_t f) {fDeconvPad=f; fDeconvTime=f;}
  void SetThreshold(UInt_t i) {fThreshold=i;}
  void SetOccupancyLimit(Float_t f) {fOccupancyLimit=f;}
  void SetSignalThreshold(Int_t i) {fSignalThreshold=i;}
  void SetNSigmaThreshold(Double_t d) {fNSigmaThreshold=d;}
  void SetMatchWidth(UInt_t i) {fMatch=i;}
  void SetSTDOutput(Bool_t f=kFALSE) {fStdout=f;}  
  void SetCalcErr(Bool_t f=kTRUE) {fCalcerr=f;}
  void SetRawSP(Bool_t f=kFALSE) {fRawSP=f;}
  void SetReader(AliHLTTPCDigitReader* f){fDigitReader = f;}
  Int_t GetNumberOfClusters() const {return fNClusters;}

  //----------------------------------Methods for the new unsorted way of reading data ----------
  void SetPadArray(AliHLTTPCPadArray *padArray);
  void ReadDataUnsorted(void* ptr,unsigned long size);
  void FindClusters();
  Int_t GetActivePads(AliHLTTPCPadArray::AliHLTTPCActivePads* activePads,Int_t maxActivePads);
  void WriteClusters(Int_t nclusters,AliHLTTPCClusters *list);
  void SetUnsorted(Int_t unsorted){fUnsorted=unsorted;}

 private: 
  /** copy constructor prohibited */
  AliHLTTPCClusterFinder(const AliHLTTPCClusterFinder&);
  /** assignment operator prohibited */
  AliHLTTPCClusterFinder& operator=(const AliHLTTPCClusterFinder&);

  AliHLTTPCSpacePointData *fSpacePointData; //! array of space points
  AliHLTTPCDigitReader *fDigitReader;       //! reader instance

  UChar_t* fPtr;   //! pointer to packed block
  unsigned long fSize; //packed block size
  Bool_t fDeconvTime; //deconv in time direction
  Bool_t fDeconvPad;  //deconv in pad direction
  Bool_t fStdout;     //have print out in write clusters
  Bool_t fCalcerr;    //calculate centroid sigmas
  Bool_t fRawSP;      //store centroids in raw system


  Int_t fFirstRow;       //first row
  Int_t fLastRow;        //last row
  Int_t fCurrentRow;     //current active row
  Int_t fCurrentSlice;   //current slice
  Int_t fCurrentPatch;   //current patch
  Int_t fMatch;          //size of match
  UInt_t fThreshold;     //threshold for clusters
  /** threshold for zero suppression (applied per bin) */
  Int_t fSignalThreshold;// see above
  /** threshold for zero suppression 2007 December run */
  Double_t fNSigmaThreshold; // see above
  Int_t fNClusters;      //number of found clusters
  Int_t fMaxNClusters;   //max. number of clusters
  Float_t fXYErr;        //fixed error in XY
  Float_t fZErr;         //fixed error in Z
  
  Float_t fOccupancyLimit; // Occupancy Limit

  AliHLTTPCPadArray * fPadArray; //! transient

  Int_t fUnsorted;       // enable for processing of unsorted digit data

  /** list of active pads if PadArray is not used */
  vector<AliHLTTPCPadArray::AliHLTTPCActivePads> fActivePads; //!transient

#ifdef do_mc
  void GetTrackID(Int_t pad,Int_t time,Int_t *trackID);
#endif
  
  ClassDef(AliHLTTPCClusterFinder,3) //Fast cluster finder
};
#endif
