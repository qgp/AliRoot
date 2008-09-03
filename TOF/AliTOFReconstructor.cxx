/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// class for TOF reconstruction                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include "TObjArray.h"
#include "TString.h"

#include "AliLog.h"
#include "AliRawReader.h"

#include "AliTOFClusterFinder.h"
#include "AliTOFClusterFinderV1.h"
#include "AliTOFcalib.h"
#include "AliTOFtrackerMI.h"
#include "AliTOFtracker.h"
#include "AliTOFtrackerV1.h"
#include "AliTOFReconstructor.h"

class TTree;

class AliESDEvent;

ClassImp(AliTOFReconstructor)

 //____________________________________________________________________
AliTOFReconstructor::AliTOFReconstructor() 
  : AliReconstructor(),
    fTOFcalib(0)
{
//
// ctor
//
  
  //Retrieving the TOF calibration info  
  fTOFcalib    = new AliTOFcalib();
  fTOFcalib->CreateCalObjects();

  if(!fTOFcalib->ReadParOnlineDelayFromCDB("TOF/Calib",-1)) {AliFatal("Exiting, no CDB object found!!!");exit(0);}  
  if(!fTOFcalib->ReadParOnlineStatusFromCDB("TOF/Calib",-1)) {AliFatal("Exiting, no CDB object found!!!");exit(0);}  

  if(!fTOFcalib->ReadParOfflineFromCDB("TOF/Calib",-1)) {AliFatal("Exiting, no CDB object found!!!");exit(0);}  
}

//------------------------------------------------------------------------
AliTOFReconstructor::AliTOFReconstructor(const AliTOFReconstructor &source)
  : AliReconstructor(),
    fTOFcalib(0)
{
//
// copy ctor
//
  this->fTOFcalib=source.fTOFcalib;
}

//------------------------------------------------------------------------
AliTOFReconstructor & AliTOFReconstructor::operator=(const AliTOFReconstructor &source)
{
//
// assignment op.
//
  this->fTOFcalib=source.fTOFcalib;
  return *this;
}
//_____________________________________________________________________________
AliTOFReconstructor::~AliTOFReconstructor() 
{
//
// dtor
//
  delete fTOFcalib;
}

//_____________________________________________________________________________
void AliTOFReconstructor::Reconstruct(AliRawReader *rawReader,
                                      TTree *clustersTree) const
{
  //
  // reconstruct clusters from Raw Data
  //

  TString selectedClusterFinder = GetOption();
  // use V1 cluster finder if selected
  if (selectedClusterFinder.Contains("NCF")) {
    static AliTOFClusterFinderV1 tofClus(fTOFcalib);
    tofClus.Digits2RecPoints(rawReader, clustersTree);
  }
  else {
    static AliTOFClusterFinder tofClus(fTOFcalib);
    tofClus.Digits2RecPoints(rawReader, clustersTree);
  }

}

//_____________________________________________________________________________
void AliTOFReconstructor::Reconstruct(TTree *digitsTree,
                                      TTree *clustersTree) const
{
  //
  // reconstruct clusters from digits
  //

  AliDebug(2,Form("Global Event loop mode: Creating Recpoints from Digits Tree")); 

  TString selectedClusterFinder = GetOption();
  // use V1 cluster finder if selected
  if (selectedClusterFinder.Contains("NCF")) {
    static AliTOFClusterFinderV1 tofClus(fTOFcalib);
    tofClus.Digits2RecPoints(digitsTree, clustersTree);
  }
  else {
    static AliTOFClusterFinder tofClus(fTOFcalib);
    tofClus.Digits2RecPoints(digitsTree, clustersTree);
  }

}
//_____________________________________________________________________________
  void AliTOFReconstructor::ConvertDigits(AliRawReader* reader, TTree* digitsTree) const
{
// reconstruct clusters from digits

  AliDebug(2,Form("Global Event loop mode: Converting Raw Data to a Digits Tree")); 

  TString selectedClusterFinder = GetOption();
  // use V1 cluster finder if selected
  if (selectedClusterFinder.Contains("NCF")) {
    static AliTOFClusterFinderV1 tofClus(fTOFcalib);
    tofClus.Raw2Digits(reader, digitsTree);
  }
  else {
    static AliTOFClusterFinder tofClus(fTOFcalib);
    tofClus.Raw2Digits(reader, digitsTree);
  }

}

//_____________________________________________________________________________
AliTracker* AliTOFReconstructor::CreateTracker() const
{

  // 
  // create a TOF tracker using 
  // TOF Reco Param collected by STEER
  //

  TString selectedTracker = GetOption();
 
  AliTracker *tracker;
  // use MI tracker if selected
  if (selectedTracker.Contains("MI")) {
    tracker = new AliTOFtrackerMI();
  }
  // use V1 tracker if selected
  if (selectedTracker.Contains("V1")) {
    tracker =  new AliTOFtrackerV1();
  }
  else {
    tracker = new AliTOFtracker();
  }
  return tracker;

}
