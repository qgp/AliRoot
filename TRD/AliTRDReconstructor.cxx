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
// Class for TRD reconstruction                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <TObjString.h>
#include <TObjArray.h>
#include <TTreeStream.h>
#include <TDirectory.h>

#include "AliRawReader.h"

#include "AliTRDReconstructor.h"
#include "AliTRDclusterizer.h"
#include "AliTRDrawData.h"
#include "AliTRDrawStreamBase.h"
#include "AliTRDdigitsManager.h"
#include "AliTRDtrackerV1.h"

#define SETFLG(n,f) ((n) |= f)
#define CLRFLG(n,f) ((n) &= ~f)

ClassImp(AliTRDReconstructor)

TClonesArray *AliTRDReconstructor::fgClusters = NULL;
TClonesArray *AliTRDReconstructor::fgTracklets = NULL;
AliTRDdigitsParam *AliTRDReconstructor::fgDigitsParam = NULL;
Char_t const * AliTRDReconstructor::fgSteerNames[kNsteer] = {
  "DigitsConversion       "
 ,"Write Clusters         "
 ,"Write Online Tracklets "
 ,"Stand Alone Tracking   "
 ,"HLT Mode               "
 ,"Process Online Tracklets"
 ,"Debug Streaming        "
};
Char_t const * AliTRDReconstructor::fgSteerFlags[kNsteer] = {
  "dc"// digits conversion [false]
 ,"cw"// write clusters [true]
 ,"tw"// write online tracklets [false]
 ,"sa"// track seeding (stand alone tracking) [true]
 ,"hlt"// HLT reconstruction [false]
 ,"tp"// also use online tracklets for reconstruction [false]
 ,"deb"// Write debug stream [false]
};
Char_t const * AliTRDReconstructor::fgTaskNames[AliTRDrecoParam::kTRDreconstructionTasks] = {
  "Clusterizer"
 ,"Tracker"
 ,"PID"
};
Char_t const * AliTRDReconstructor::fgTaskFlags[AliTRDrecoParam::kTRDreconstructionTasks] = {
  "cl"
 ,"tr"
 ,"pd"
};

//_____________________________________________________________________________
AliTRDReconstructor::AliTRDReconstructor()
  :AliReconstructor()
  ,fSteerParam(0)
  ,fClusterizer(NULL)
{
  // setting default "ON" steering parameters
  // owner of debug streamers 
  SETFLG(fSteerParam, kOwner);
  // write clusters [cw]
  SETFLG(fSteerParam, kWriteClusters);
  // track seeding (stand alone tracking) [sa]
  SETFLG(fSteerParam, kSeeding);

  memset(fDebugStream, 0, sizeof(TTreeSRedirector *) * AliTRDrecoParam::kTRDreconstructionTasks);
}

//_____________________________________________________________________________
AliTRDReconstructor::~AliTRDReconstructor()
{
  //
  // Destructor
  //

  if(fgDigitsParam){
    delete fgDigitsParam;
    fgDigitsParam = NULL;
  }
  if(fgClusters) {
    fgClusters->Delete();
    delete fgClusters;
    fgClusters = NULL;
  }
  if(fgTracklets) {
    fgTracklets->Delete();
    delete fgTracklets;
    fgTracklets = NULL;
  }
  if(fSteerParam&kOwner){
    for(Int_t itask = 0; itask < AliTRDrecoParam::kTRDreconstructionTasks; itask++)
      if(fDebugStream[itask]) delete fDebugStream[itask];
  }
  if(fClusterizer){
    delete fClusterizer;
    fClusterizer = NULL;
  }
}


//_____________________________________________________________________________
void AliTRDReconstructor::Init(){
  //
  // Init Options
  //
  SetOption(GetOption());
  Options(fSteerParam);

  if(!fClusterizer){
    fClusterizer = new AliTRDclusterizer(fgTaskNames[AliTRDrecoParam::kClusterizer], fgTaskNames[AliTRDrecoParam::kClusterizer]);
    fClusterizer->SetReconstructor(this);
  }
  
  // Make Debug Streams when Debug Streaming
  if(IsDebugStreaming()){
    for(Int_t task = 0; task < AliTRDrecoParam::kTRDreconstructionTasks; task++){
      TDirectory *savedir = gDirectory;
      fDebugStream[task] = new TTreeSRedirector(Form("TRD.Debug%s.root", fgTaskNames[task]));
      savedir->cd();
      SETFLG(fSteerParam, kOwner);
    }
  }
}

//_____________________________________________________________________________
void AliTRDReconstructor::ConvertDigits(AliRawReader *rawReader
              , TTree *digitsTree) const
{
  //
  // Convert raw data digits into digit objects in a root tree
  //

  //AliInfo("Convert raw data digits into digit objects [RawReader -> Digit TTree]");

  AliTRDrawData rawData;
  rawReader->Reset();
  rawReader->Select("TRD");
  rawData.OpenOutput();
  AliTRDrawStreamBase::SetRawStreamVersion(GetRecoParam()->GetRawStreamVersion()->Data());
  AliTRDdigitsManager *manager = rawData.Raw2Digits(rawReader);
  manager->MakeBranch(digitsTree);
  manager->WriteDigits();
  delete manager;

}

//_____________________________________________________________________________
void AliTRDReconstructor::Reconstruct(AliRawReader *rawReader
                                    , TTree *clusterTree) const
{
  //
  // Reconstruct clusters
  //

  //AliInfo("Reconstruct TRD clusters from RAW data [RawReader -> Cluster TTree]");


  rawReader->Reset();
  rawReader->Select("TRD");
  AliTRDrawStreamBase::SetRawStreamVersion(GetRecoParam()->GetRawStreamVersion()->Data());

  if(!fClusterizer){
    AliFatal("Clusterizer not available!");
    return;
  }

  fClusterizer->ResetRecPoints();

  fClusterizer->OpenOutput(clusterTree);
  fClusterizer->OpenTrackletOutput();
  fClusterizer->SetUseLabels(kFALSE);
  fClusterizer->Raw2ClustersChamber(rawReader);
  
  if(IsWritingClusters()) return;

  // take over ownership of clusters
  fgClusters = fClusterizer->RecPoints();
  fClusterizer->SetClustersOwner(kFALSE);

  // take over ownership of online tracklets
  fgTracklets = fClusterizer->TrackletsArray();
  fClusterizer->SetTrackletsOwner(kFALSE);
}

//_____________________________________________________________________________
void AliTRDReconstructor::Reconstruct(TTree *digitsTree
                                    , TTree *clusterTree) const
{
  //
  // Reconstruct clusters
  //

  //AliInfo("Reconstruct TRD clusters from Digits [Digit TTree -> Cluster TTree]");
  
  AliTRDclusterizer clusterer(fgTaskNames[AliTRDrecoParam::kClusterizer], fgTaskNames[AliTRDrecoParam::kClusterizer]);
  clusterer.SetReconstructor(this);
  clusterer.OpenOutput(clusterTree);
  clusterer.ReadDigits(digitsTree);
  clusterer.MakeClusters();

  if(IsWritingClusters()) return;

  // take over ownership of clusters
  fgClusters = clusterer.RecPoints();
  clusterer.SetClustersOwner(kFALSE);

  // take over ownership of online tracklets
  fgTracklets = clusterer.TrackletsArray();
  clusterer.SetTrackletsOwner(kFALSE);
}

//_____________________________________________________________________________
AliTracker *AliTRDReconstructor::CreateTracker() const
{
  //
  // Create a TRD tracker
  //

  //return new AliTRDtracker(NULL);
  AliTRDtrackerV1 *tracker = new AliTRDtrackerV1();
  tracker->SetReconstructor(this);
  return tracker;

}

//_____________________________________________________________________________
void AliTRDReconstructor::FillESD(TTree* /*digitsTree*/
        , TTree* /*clusterTree*/
        , AliESDEvent* /*esd*/) const
{
  //
  // Fill ESD
  //

}

//_____________________________________________________________________________
void AliTRDReconstructor::SetDigitsParam(AliTRDdigitsParam const *par)
{ 
  if(fgDigitsParam) (*fgDigitsParam) = (*par);
  else fgDigitsParam = new AliTRDdigitsParam(*par);
}


//_____________________________________________________________________________
void AliTRDReconstructor::SetOption(Option_t *opt)
{
  //
  // Read option string into the steer param.
  //

  AliReconstructor::SetOption(opt);

  TString s(opt);
  TObjArray *opar = s.Tokenize(",");
  for(Int_t ipar=0; ipar<opar->GetEntriesFast(); ipar++){
    Bool_t processed = kFALSE;
    TString sopt(((TObjString*)(*opar)[ipar])->String());
    for(Int_t iopt=0; iopt<kNsteer; iopt++){
      if(!sopt.Contains(fgSteerFlags[iopt])) continue;
      SETFLG(fSteerParam, BIT(iopt));
      if(sopt.Contains("!")) CLRFLG(fSteerParam, BIT(iopt));
      processed = kTRUE;
      break;	
    }
    if(processed) continue;

    AliWarning(Form("Unknown option flag %s.", sopt.Data()));
  }
}

//_____________________________________________________________________________
void AliTRDReconstructor::Options(UInt_t steer)
{
  //
  // Print the options
  //

  for(Int_t iopt=0; iopt<kNsteer; iopt++){
    AliDebugGeneral("AliTRDReconstructor", 1, Form(" %s[%s]%s", fgSteerNames[iopt], fgSteerFlags[iopt], steer ?(((steer>>iopt)&1)?" : ON":" : OFF"):""));
  }
}

