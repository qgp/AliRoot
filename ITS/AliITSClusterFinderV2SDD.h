#ifndef ALIITSCLUSTERFINDERV2SDD_H
#define ALIITSCLUSTERFINDERV2SDD_H
//--------------------------------------------------------------
//                       ITS clusterer V2 for SDD
//
//   This can be a "wrapping" for the V1 cluster finding classes
//   if compiled with uncommented "#define V1" line 
//   in the AliITSclustererV2.cxx file.
//
//   Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//--------------------------------------------------------------
#include "AliITSClusterFinderV2.h"

class TClonesArray;
class AliRawReader;
class AliITSRawStream;

class AliITSClusterFinderV2SDD : public AliITSClusterFinderV2 {
public:
  AliITSClusterFinderV2SDD(AliITSgeom* geom);
  virtual ~AliITSClusterFinderV2SDD(){;}
  virtual void FindRawClusters(Int_t mod);
  virtual void RawdataToClusters(AliRawReader* rawReader,TClonesArray** clusters);
 protected:

  void FindClustersSDD(TClonesArray *digits);
  void FindClustersSDD(AliBin* bins[2], Int_t nMaxBin, Int_t nMaxZ,
		       TClonesArray *dig, TClonesArray *clusters=0x0);

  void FindClustersSDD(AliITSRawStream* input,TClonesArray** clusters);

  Int_t fNySDD;           //number of "pixels" in Y
  Int_t fNzSDD;           //number of "pixels" in Z
  Float_t fYpitchSDD;     //"pixel size" in Y (drift direction)
  Float_t fZpitchSDD;     //"pixel sizes" in Z
  Float_t fHwSDD;         //half width of the SDD detector
  Float_t fHlSDD;         //half length of the SDD detector
  Float_t fYoffSDD;       //some delay in the drift channel   

  ClassDef(AliITSClusterFinderV2SDD,1)  // ITS cluster finder V2 for SDD
};

#endif
