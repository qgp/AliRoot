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
// high level filter algorithm for TPC using a hough transformation          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <TStopwatch.h>

#include "AliHLTStandardIncludes.h"
#include "AliHLTLogging.h"
#include "AliHLTTransform.h"
#include "AliHLTHough.h"
#include "AliLog.h"
#include <AliHLTITSclusterer.h>
#include <AliHLTITSVertexerZ.h>
#include <AliHLTITStracker.h>

#include "AliHoughFilter.h"

#include "AliRawReaderRoot.h"
#include <AliMagF.h>
#include <AliMagFMaps.h>
#include <AliKalmanTrack.h>
#include <AliITSgeom.h>
#include <AliESDVertex.h>

ClassImp(AliHoughFilter)

//_____________________________________________________________________________
AliHoughFilter::AliHoughFilter():
fPtmin(0),
fITSgeom(NULL)
{
// default constructor

  // Init debug level
  AliHLTLog::fgLevel = AliHLTLog::kError;
  if (AliDebugLevel() > 0) AliHLTLog::fgLevel = AliHLTLog::kWarning;
  if (AliDebugLevel() > 1) AliHLTLog::fgLevel = AliHLTLog::kInformational;
  if (AliDebugLevel() > 2) AliHLTLog::fgLevel = AliHLTLog::kDebug;

  // Init TPC HLT geometry
  const char *path = gSystem->Getenv("ALICE_ROOT");
  Char_t pathname[1024];
  strcpy(pathname,path);
  strcat(pathname,"/HLT/src");
  if (!AliHLTTransform::Init(pathname, kFALSE))
    AliError("HLT initialization failed!");

  // Init magnetic field
  AliMagF* field = new AliMagFMaps("Maps","Maps", 2, 1., 10., AliMagFMaps::k5kG);
  AliTracker::SetFieldMap(field,kTRUE);
  fPtmin = 0.1*AliHLTTransform::GetSolenoidField();

  // Init ITS geometry
  fITSgeom = new AliITSgeom();
  fITSgeom->ReadNewFile("$ALICE_ROOT/ITS/ITSgeometry_vPPRasymmFMD.det");
  if (!fITSgeom)
    AliError("ITS geometry not found!");

  // Init PID
  AliPID pid;
}

//_____________________________________________________________________________
Bool_t AliHoughFilter::Filter(AliRawVEvent* event, AliESDEvent* esd)
{
  // Run fast online reconstruction
  // based on the HLT tracking algorithms

  TStopwatch globaltimer;
  globaltimer.Start();

  TStopwatch timer;
  timer.Start();

  TTree *treeITSclusters = new TTree("TreeL3ITSclusters"," "); //make a tree
  treeITSclusters->SetDirectory(0);

  RunITSclusterer(event,treeITSclusters);
  RunITSvertexer(esd,treeITSclusters);
  RunTPCtracking(event,esd);
  RunITStracking(esd,treeITSclusters);
  delete treeITSclusters;

  AliInfo(Form("Event filter has finished in %f seconds\n\n\n\n\n\n",globaltimer.RealTime()));

  return kTRUE;
}

//_____________________________________________________________________________
void AliHoughFilter::RunITSclusterer(AliRawVEvent* event, TTree *treeClusters)
{
  // Run ITS Clusterer
  // The clusters are stored in a tree

  TStopwatch timer;
  timer.Start();

  AliHLTITSclusterer clusterer(0);
  AliRawReader *itsrawreader=new AliRawReaderRoot(event);
  clusterer.Digits2Clusters(itsrawreader,treeClusters);
  delete itsrawreader;
  AliInfo(Form("ITS clusterer has finished in %f seconds\n",timer.RealTime()));

}


//_____________________________________________________________________________
void AliHoughFilter::RunITSvertexer(AliESDEvent* esd, TTree *treeClusters)
{
  // Run SPD vertexerZ
  // Store the result in the ESD

  TStopwatch timer;
  timer.Start();

  AliHLTITSVertexerZ vertexer;
  AliESDVertex *vertex = vertexer.FindVertexForCurrentEvent(fITSgeom,treeClusters);
  esd->SetVertex(vertex);
  AliInfo(Form("ITS vertexer has finished in %f seconds\n",timer.RealTime()));

}

//_____________________________________________________________________________
void AliHoughFilter::RunTPCtracking(AliRawVEvent* event, AliESDEvent* esd)
{
  // Run hough transform tracking in TPC
  // The z of the vertex is taken from the ESD
  // The result of the tracking is stored in the ESD
  TStopwatch timer;
  timer.Start();

  const AliESDVertex *vertex = esd->GetVertex();
  Float_t zvertex = vertex->GetZv();

  AliHLTHough *hough1 = new AliHLTHough();
    
  hough1->SetThreshold(4);
  hough1->CalcTransformerParams(fPtmin);
  hough1->SetPeakThreshold(70,-1);
  hough1->Init(100,4,event,zvertex);
  hough1->SetAddHistograms();

  AliHLTHough *hough2 = new AliHLTHough();
  
  hough2->SetThreshold(4);
  hough2->CalcTransformerParams(fPtmin);
  hough2->SetPeakThreshold(70,-1);
  hough2->Init(100,4,event,zvertex);
  hough2->SetAddHistograms();

  Int_t nglobaltracks = 0;
  /* In case we run HLT code in 2 threads */
  hough1->StartProcessInThread(0,17);
  hough2->StartProcessInThread(18,35);

  if(hough1->WaitForThreadFinish())
    ::Fatal("AliHLTHough::WaitForThreadFinish"," Can not join the required thread! ");
  if(hough2->WaitForThreadFinish())
    ::Fatal("AliHLTHough::WaitForThreadFinish"," Can not join the required thread! ");

    /* In case we run HLT code in the main thread
    for(Int_t slice=0; slice<=17; slice++)
      {
	hough1->ReadData(slice,0);
	hough1->Transform();
	hough1->AddAllHistogramsRows();
	hough1->FindTrackCandidatesRow();
	hough1->AddTracks();
      }
    for(Int_t slice=18; slice<=35; slice++)
      {
	hough2->ReadData(slice,0);
	hough2->Transform();
	hough2->AddAllHistogramsRows();
	hough2->FindTrackCandidatesRow();
	hough2->AddTracks();
      }
    */

  nglobaltracks += hough1->FillESD(esd);
  nglobaltracks += hough2->FillESD(esd);

    /* In case we want to debug the ESD
    gSystem->MakeDirectory("hough1");
    hough1->WriteTracks("./hough1");
    gSystem->MakeDirectory("hough2");
    hough2->WriteTracks("./hough2");
    */

  delete hough1;
  delete hough2;

  AliInfo(Form("\nHough Transformer has found %d TPC tracks in %f seconds\n\n", nglobaltracks,timer.RealTime()));

}

//_____________________________________________________________________________
void AliHoughFilter::RunITStracking(AliESDEvent* esd, TTree *treeClusters)
{
  // Run the ITS tracker
  // The tracks from the HT TPC tracking are used as seeds
  // The prologated tracks are updated in the ESD
  TStopwatch timer;
  timer.Start();

  Double_t vtxPos[3];
  Double_t vtxErr[3]={0.005,0.005,0.010};
  const AliESDVertex *vertex = esd->GetVertex();
  vertex->GetXYZ(vtxPos);

  AliHLTITStracker itsTracker(0);
  itsTracker.SetVertex(vtxPos,vtxErr);

  itsTracker.LoadClusters(treeClusters);
  itsTracker.Clusters2Tracks(esd);
  itsTracker.UnloadClusters();
  AliInfo(Form("ITS tracker has finished in %f seconds\n",timer.RealTime()));

}
