//-*- Mode: C++ -*-
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef ALIHLTTPCGMMERGER_H
#define ALIHLTTPCGMMERGER_H

#include "AliHLTTPCCADef.h"
#include "AliHLTTPCCAParam.h"
#include "AliHLTTPCGMBorderTrack.h"
#include "AliHLTTPCGMSliceTrack.h"

#if !defined(HLTCA_GPUCODE)
#include <iostream>
#endif //HLTCA_GPUCODE

class AliHLTTPCCASliceTrack;
class AliHLTTPCCASliceOutput;
class AliHLTTPCGMCluster;
class AliHLTTPCGMTrackParam;
class AliHLTTPCGMMergedTrack;

/**
 * @class AliHLTTPCGMMerger
 *
 */
class AliHLTTPCGMMerger
{
  
public:
  
  AliHLTTPCGMMerger();
  ~AliHLTTPCGMMerger();
  
  void SetSliceParam( const AliHLTTPCCAParam &v ) { fSliceParam = v; }
  
  void Clear();
  void SetSliceData( int index, const AliHLTTPCCASliceOutput *SliceData );
  bool Reconstruct();
  
  Int_t NOutputTracks() const { return fNOutputTracks; }
  const AliHLTTPCGMMergedTrack * OutputTracks() const { return fOutputTracks; }
  const UInt_t * OutputClusterIds() const { return fOutputClusterIds; }
   
  
  const AliHLTTPCCAParam &SliceParam() const { return fSliceParam; }
  
private:
  
  AliHLTTPCGMMerger( const AliHLTTPCGMMerger& );

  const AliHLTTPCGMMerger &operator=( const AliHLTTPCGMMerger& ) const;
  
  void MakeBorderTracks( int iSlice, int iBorder, AliHLTTPCGMBorderTrack B[], int &nB );

  void MergeBorderTracks( int iSlice1, AliHLTTPCGMBorderTrack B1[],  int N1,
			  int iSlice2, AliHLTTPCGMBorderTrack B2[],  int N2 );
  
  static bool CompareTrackParts( const AliHLTTPCGMSliceTrack *t1, const AliHLTTPCGMSliceTrack *t2 ){
    return (t1->X() > t2->X() );
  }

  void ClearMemory();
  bool AllocateMemory();
  void UnpackSlices();
  void MergeWithingSlices();
  void MergeSlices();
  void CollectMergedTracks();
  void Refit();
  
  static const int fgkNSlices = 36;       //* N slices
  int fNextSliceInd[fgkNSlices];
  int fPrevSliceInd[fgkNSlices];
  
  AliHLTTPCCAParam fSliceParam;           //* slice parameters (geometry, calibr, etc.)
  const AliHLTTPCCASliceOutput *fkSlices[fgkNSlices]; //* array of input slice tracks

  Int_t fNOutputTracks;
  AliHLTTPCGMMergedTrack *fOutputTracks;       //* array of output merged tracks
  UInt_t * fOutputClusterIds;
  
  AliHLTTPCGMSliceTrack *fSliceTrackInfos; //* additional information for slice tracks
  int fSliceTrackInfoStart[fgkNSlices];   //* slice starting index in fTrackInfos array;
  int fSliceNTrackInfos[fgkNSlices];      //* N of slice track infos in fTrackInfos array;
  int fMaxSliceTracks;      // max N tracks in one slice
  float *fClusterX;         // cluster X
  float *fClusterY;         // cluster Y
  float *fClusterZ;         // cluster Z
  UInt_t *fClusterRowType;  // cluster row type
  float *fClusterAngle;     // angle    
  AliHLTTPCGMBorderTrack *fBorderMemory; // memory for border tracks
  AliHLTTPCGMBorderTrack::Range *fBorderRangeMemory; // memory for border tracks
};

#endif //ALIHLTTPCCAMERGER_H
