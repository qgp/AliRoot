// @(#) $Id: AliHLTTPCCAStartHitsFinder.cxx 27042 2008-07-02 12:06:02Z richterm $
// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: David Rohr <drohr@kip.uni-heidelberg.de>				*
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************

#include "AliHLTTPCCAStartHitsSorter.h"
#include "AliHLTTPCCATracker.h"

GPUd() void AliHLTTPCCAStartHitsSorter::Thread
( int /*nBlocks*/, int nThreads, int iBlock, int iThread, int iSync,
  AliHLTTPCCASharedMemory &s, AliHLTTPCCATracker &tracker )
{
  if ( iSync == 0 ) {
    if ( iThread == 0 ) {
#ifdef HLTCA_GPU_SORT_STARTHITS_2
	  const int tmpNRowsEvenBlock = (tracker.Param().NRows() - 4) / 2;
	  const int tmpNRows = (tracker.Param().NRows() - 4 + (iBlock & 1)) / 2;
	  int nRows = (iBlock / 2 == 14 ? (tmpNRows - (tmpNRows / 15) * 14) : (tmpNRows / 15)) * 2;
	  int nStartRow = (tmpNRows / 15) * (iBlock / 2) * 2 + 1 + (iBlock & 1);
	  int StartOffset = 0;
	  if (iBlock & 1)
	  {
		for (int ir = 1;ir < tracker.Param().NRows() - 3;ir+=2) StartOffset += tracker.RowStartHitCountOffset()[ir].x;
		if (iBlock == 1) *tracker.NTracklets() += NextMultipleOf<32>(StartOffset) - StartOffset;
		for (int i = StartOffset;i < NextMultipleOf<32>(StartOffset);i++)
			tracker.TrackletStartHits()[i].Set(0, 0);
		StartOffset = NextMultipleOf<32>(StartOffset);
	  }
	  for (int ir = 1 + (iBlock & 1);ir < nStartRow;ir += 2)
	  {
		  StartOffset += tracker.RowStartHitCountOffset()[ir].x;
	  }
#else
	  const int tmpNRows = tracker.Param().NRows() - 4;
	  int nRows = iBlock == 29 ? (tmpNRows - (tmpNRows / 30) * 29) : (tmpNRows / 30);
	  int nStartRow = (tmpNRows / 30) * iBlock + 1;
      int StartOffset = 0;
      for (int ir = 1;ir < nStartRow;ir++)
	  {
		StartOffset += tracker.RowStartHitCountOffset()[ir].x;
	  }
#endif
	  s.fStartOffset = StartOffset;
	  s.fNRows = nRows;
      s.fStartRow = nStartRow;
    }
  } else if ( iSync == 1 ) {
	int StartOffset = s.fStartOffset;
#ifdef HLTCA_GPU_SORT_STARTHITS_2
    for (int ir = 0;ir < s.fNRows;ir+=2)
#else
    for (int ir = 0;ir < s.fNRows;ir++)
#endif
	{
		AliHLTTPCCAHitId *const startHits = tracker.TrackletStartHits();
		AliHLTTPCCAHitId *const tmpStartHits = tracker.TrackletTmpStartHits();
		const int tmpLen = tracker.RowStartHitCountOffset()[ir + s.fStartRow].x;			//Length of hits in row stored by StartHitsFinder
		const int tmpOffset = tracker.RowStartHitCountOffset()[ir + s.fStartRow].y;			//Offset of first hit in row of unsorted array by StartHitsFinder
		if (iThread == 0)
			tracker.RowStartHitCountOffset()[ir + s.fStartRow].y = StartOffset;				//Store New Offset Value of sorted array

		for (int j = iThread;j < tmpLen;j += nThreads)
		{
			startHits[StartOffset + j] = tmpStartHits[tmpOffset + j];
		}
		StartOffset += tmpLen;
    }
  }
}

