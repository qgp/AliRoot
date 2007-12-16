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
// class for TPC reconstruction                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include "AliTPCReconstructor.h"
#include "AliRunLoader.h"
#include "AliRun.h"
#include "AliRawReader.h"
#include "AliTPCclustererMI.h"
#include "AliTPCtrackerMI.h"
#include "AliTPCpidESD.h"
#include "AliTPCParam.h"
#include "AliTPCParamSR.h"
#include "AliTPCcalibDB.h"

ClassImp(AliTPCReconstructor)


AliTPCRecoParam *    AliTPCReconstructor::fgkRecoParam =0;  // reconstruction parameters
Int_t    AliTPCReconstructor::fgStreamLevel     = 0;        // stream (debug) level


AliTPCReconstructor::AliTPCReconstructor():
AliReconstructor(),
fClusterer(NULL)
{
  //
  // default constructor
  //
  //
  //
 
  AliTPCParam* param = GetTPCParam();
  if (!param) {
    AliWarning("Loading default TPC parameters !");
    param = new AliTPCParamSR;
  }
  fClusterer = new AliTPCclustererMI(param);
}

//_____________________________________________________________________________
AliTPCReconstructor::~AliTPCReconstructor()
{
  if (fgkRecoParam) delete fgkRecoParam;
  if (fClusterer)   delete fClusterer;
}

//_____________________________________________________________________________
void AliTPCReconstructor::Reconstruct(TTree* digitsTree, TTree* clustersTree) const {
  // single event local reconstruction
  // of TPC data
  fClusterer->SetInput(digitsTree);
  fClusterer->SetOutput(clustersTree);
  fClusterer->Digits2Clusters();
}

//_____________________________________________________________________________
void AliTPCReconstructor::Reconstruct(AliRawReader* rawReader, TTree* clustersTree) const {
  // single event local reconstruction
  // of TPC data starting from raw data
  TString option = GetOption();
  if (option.Contains("OldRCUFormat"))
    fClusterer->SetOldRCUFormat(kTRUE);

  fClusterer->SetOutput(clustersTree);
  fClusterer->Digits2Clusters(rawReader);
}

//_____________________________________________________________________________
AliTracker* AliTPCReconstructor::CreateTracker() const
{
// create a TPC tracker

  AliTPCParam* param = GetTPCParam();
  if (!param) {
    AliWarning("Loading default TPC parameters !");
    param = new AliTPCParamSR;
  }
  param->ReadGeoMatrices();
  return new AliTPCtrackerMI(param);
}

//_____________________________________________________________________________
void AliTPCReconstructor::FillESD(TTree */*digitsTree*/, TTree */*clustersTree*/,
				  AliESDEvent* esd) const
{
// make PID

  Double_t parTPC[] = {47., 0.07, 5.};
  AliTPCpidESD tpcPID(parTPC);
  tpcPID.MakePID(esd);
}


//_____________________________________________________________________________
AliTPCParam* AliTPCReconstructor::GetTPCParam() const
{
// get the TPC parameters

  AliTPCParam* param = AliTPCcalibDB::Instance()->GetParameters();

  return param;
}




const AliTPCRecoParam* AliTPCReconstructor::GetRecoParam(){ 
  //
  // Get reconstruction parameters
  //
  
   if (!fgkRecoParam) {
    //
    // 1. try to get reco parameters from OCDB 
    //
    fgkRecoParam = AliTPCcalibDB::Instance()->GetRecoParam(0);
    //Info("","Reconstruction parameters from OCDB used");
    //
    // 2. If not initialized take default
    //
    if (!fgkRecoParam){
      fgkRecoParam = AliTPCRecoParam::GetHighFluxParam();
      //Error("","Default reconstruction parameters  used");
    }
  }

  return fgkRecoParam;
}
