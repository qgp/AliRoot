#ifndef ALIHLTTPCCLUSTERS_H
#define ALIHLTTPCCLUSTERS_H

// see delow for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

#include "Rtypes.h"

class AliHLTTPCClusters {

 public:
  AliHLTTPCClusters();
  AliHLTTPCClusters(const AliHLTTPCClusters& src);
  AliHLTTPCClusters& operator=(const AliHLTTPCClusters& src);
  virtual ~AliHLTTPCClusters();

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
  UInt_t fRowNumber;     //row number
  Int_t fFirstPad;       //first pad
  UInt_t fLastPad;       //last pad (aha!!!)
  UInt_t fQMax;          //Max signal in cluster (not the total charge)
  ClassDef(AliHLTTPCClusters,1) //Fast cluster finder data point.
};
#endif //ALIHLTTPCCLUSTERS_H
