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
// class for running the reconstruction                                      //
//                                                                           //
// Clusters and tracks are created for all detectors and all events by       //
// typing:                                                                   //
//                                                                           //
//   AliReconstruction rec;                                                  //
//   rec.Run();                                                              //
//                                                                           //
// The Run method returns kTRUE in case of successful execution.             //
// The name of the galice file can be changed from the default               //
// "galice.root" by passing it as argument to the AliReconstruction          //
// constructor or by                                                         //
//                                                                           //
//   rec.SetGAliceFile("...");                                               //
//                                                                           //
// The reconstruction can be switched on or off for individual detectors by  //
//                                                                           //
//   rec.SetRunReconstruction("...");                                        //
//                                                                           //
// The argument is a (case sensitive) string with the names of the           //
// detectors separated by a space. The special string "ALL" selects all      //
// available detectors. This is the default.                                 //
//                                                                           //
// The tracking in ITS, TPC and TRD and the creation of ESD tracks can be    //
// switched off by                                                           //
//                                                                           //
//   rec.SetRunTracking(kFALSE);                                             //
//                                                                           //
// The filling of additional ESD information can be steered by               //
//                                                                           //
//   rec.SetFillESD("...");                                                  //
//                                                                           //
// Again, the string specifies the list of detectors. The default is "ALL".  //
//                                                                           //
// The reconstruction requires digits as input. For the creation of digits   //
// have a look at the class AliSimulation.                                   //
//                                                                           //
// For debug purposes the method SetCheckPointLevel can be used. If the      //
// argument is greater than 0, files with ESD events will be written after   //
// selected steps of the reconstruction for each event:                      //
//   level 1: after tracking and after filling of ESD (final)                //
//   level 2: in addition after each tracking step                           //
//   level 3: in addition after the filling of ESD for each detector         //
// If a final check point file exists for an event, this event will be       //
// skipped in the reconstruction. The tracking and the filling of ESD for    //
// a detector will be skipped as well, if the corresponding check point      //
// file exists. The ESD event will then be loaded from the file instead.     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include "AliReconstruction.h"
#include "AliRunLoader.h"
#include "AliRun.h"
#include "AliModule.h"
#include "AliDetector.h"
#include "AliTracker.h"
#include "AliESD.h"
#include "AliESDVertex.h"
#include "AliVertexer.h"
#include "AliHeader.h"
#include "AliGenEventHeader.h"
#include "AliESDpid.h"
#include "AliMagF.h"
#include <TArrayF.h>
#include <TSystem.h>
#include <TROOT.h>


ClassImp(AliReconstruction)


//_____________________________________________________________________________
AliReconstruction::AliReconstruction(const char* gAliceFilename,
				     const char* name, const char* title) :
  TNamed(name, title),

  fRunReconstruction("ALL"),
  fRunVertexFinder(kTRUE),
  fRunTracking(kTRUE),
  fFillESD("ALL"),
  fGAliceFileName(gAliceFilename),
  fStopOnError(kFALSE),
  fCheckPointLevel(0),

  fRunLoader(NULL),
  fITSLoader(NULL),
  fITSVertexer(NULL),
  fITSTracker(NULL),
  fTPCLoader(NULL),
  fTPCTracker(NULL),
  fTRDLoader(NULL),
  fTRDTracker(NULL),
  fTOFLoader(NULL),
  fTOFTracker(NULL)
{
// create reconstruction object with default parameters

}

//_____________________________________________________________________________
AliReconstruction::AliReconstruction(const AliReconstruction& rec) :
  TNamed(rec),

  fRunReconstruction(rec.fRunReconstruction),
  fRunVertexFinder(rec.fRunVertexFinder),
  fRunTracking(rec.fRunTracking),
  fFillESD(rec.fFillESD),
  fGAliceFileName(rec.fGAliceFileName),
  fStopOnError(rec.fStopOnError),
  fCheckPointLevel(0),

  fRunLoader(NULL),
  fITSLoader(NULL),
  fITSVertexer(NULL),
  fITSTracker(NULL),
  fTPCLoader(NULL),
  fTPCTracker(NULL),
  fTRDLoader(NULL),
  fTRDTracker(NULL),
  fTOFLoader(NULL),
  fTOFTracker(NULL)
{
// copy constructor

}

//_____________________________________________________________________________
AliReconstruction& AliReconstruction::operator = (const AliReconstruction& rec)
{
// assignment operator

  this->~AliReconstruction();
  new(this) AliReconstruction(rec);
  return *this;
}

//_____________________________________________________________________________
AliReconstruction::~AliReconstruction()
{
// clean up

  CleanUp();
}


//_____________________________________________________________________________
void AliReconstruction::SetGAliceFile(const char* fileName)
{
// set the name of the galice file

  fGAliceFileName = fileName;
}


//_____________________________________________________________________________
Bool_t AliReconstruction::Run()
{
// run the reconstruction

  // open the run loader
  fRunLoader = AliRunLoader::Open(fGAliceFileName.Data());
  if (!fRunLoader) {
    Error("Run", "no run loader found in file %s", 
	  fGAliceFileName.Data());
    CleanUp();
    return kFALSE;
  }
  fRunLoader->LoadgAlice();
  AliRun* aliRun = fRunLoader->GetAliRun();
  if (!aliRun) {
    Error("Run", "no gAlice object found in file %s", 
	  fGAliceFileName.Data());
    CleanUp();
    return kFALSE;
  }
  gAlice = aliRun;

  // local reconstruction
  if (!fRunReconstruction.IsNull()) {
    if (!RunReconstruction(fRunReconstruction)) {
      if (fStopOnError) {CleanUp(); return kFALSE;}
    }
  }
  if (!fRunVertexFinder && !fRunTracking && fFillESD.IsNull()) return kTRUE;

  // get vertexer
  if (fRunVertexFinder && !CreateVertexer()) {
    if (fStopOnError) {
      CleanUp(); 
      return kFALSE;
    }
  }

  // get loaders and trackers
  if (fRunTracking && !CreateTrackers()) {
    if (fStopOnError) {
      CleanUp(); 
      return kFALSE;
    }      
  }

  // create the ESD output file and tree
  TFile* file = TFile::Open("AliESDs.root", "RECREATE");
  if (!file->IsOpen()) {
    Error("Run", "opening AliESDs.root failed");
    if (fStopOnError) {CleanUp(file); return kFALSE;}    
  }
  AliESD* esd = new AliESD;
  TTree* tree = new TTree("esdTree", "Tree with ESD objects");
  tree->Branch("ESD", "AliESD", &esd);
  delete esd;
  gROOT->cd();

  // loop over events
  for (Int_t iEvent = 0; iEvent < fRunLoader->GetNumberOfEvents(); iEvent++) {
    Info("Run", "processing event %d", iEvent);
    fRunLoader->GetEvent(iEvent);

    char fileName[256];
    sprintf(fileName, "ESD_%d.%d_final.root", 
	    aliRun->GetRunNumber(), aliRun->GetEvNumber());
    if (!gSystem->AccessPathName(fileName)) continue;

    esd = new AliESD;
    esd->SetRunNumber(aliRun->GetRunNumber());
    esd->SetEventNumber(aliRun->GetEvNumber());
    esd->SetMagneticField(aliRun->Field()->SolenoidField());

    // vertex finder
    if (fRunVertexFinder) {
      if (!ReadESD(esd, "vertex")) {
	if (!RunVertexFinder(esd)) {
	  if (fStopOnError) {CleanUp(file); return kFALSE;}
	}
	if (fCheckPointLevel > 0) WriteESD(esd, "vertex");
      }
    }

    // barrel tracking
    if (fRunTracking) {
      if (!ReadESD(esd, "tracking")) {
	if (!RunTracking(esd)) {
	  if (fStopOnError) {CleanUp(file); return kFALSE;}
	}
	if (fCheckPointLevel > 0) WriteESD(esd, "tracking");
      }
    }

    // fill ESD
    if (!fFillESD.IsNull()) {
      if (!FillESD(esd, fFillESD)) {
	if (fStopOnError) {CleanUp(file); return kFALSE;}
      }
    }

    // combined PID
    AliESDpid::MakePID(esd);
    if (fCheckPointLevel > 1) WriteESD(esd, "PID");

    // write ESD
    tree->Fill();

    if (fCheckPointLevel > 0) WriteESD(esd, "final");
    delete esd;
  }

  file->cd();
  tree->Write();
  CleanUp(file);

  return kTRUE;
}


//_____________________________________________________________________________
Bool_t AliReconstruction::RunReconstruction(const TString& detectors)
{
// run the reconstruction

  TStopwatch stopwatch;
  stopwatch.Start();

  TString detStr = detectors;
  TObjArray* detArray = fRunLoader->GetAliRun()->Detectors();
  for (Int_t iDet = 0; iDet < detArray->GetEntriesFast(); iDet++) {
    AliModule* det = (AliModule*) detArray->At(iDet);
    if (!det || !det->IsActive()) continue;
    if (IsSelected(det->GetName(), detStr)) {
      Info("RunReconstruction", "running reconstruction for %s", 
	   det->GetName());
      TStopwatch stopwatchDet;
      stopwatchDet.Start();
      det->Reconstruct();
      Info("RunReconstruction", "execution time for %s:", det->GetName());
      stopwatchDet.Print();
    }
  }

  if ((detStr.CompareTo("ALL") != 0) && !detStr.IsNull()) {
    Error("RunReconstruction", "the following detectors were not found: %s", 
	  detStr.Data());
    if (fStopOnError) return kFALSE;
  }

  Info("RunReconstruction", "execution time:");
  stopwatch.Print();

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::RunVertexFinder(AliESD*& esd)
{
// run the barrel tracking

  TStopwatch stopwatch;
  stopwatch.Start();

  AliESDVertex* vertex = NULL;
  Double_t vtxPos[3] = {0, 0, 0};
  Double_t vtxErr[3] = {0.07, 0.07, 0.1};
  TArrayF mcVertex(3); 
  if (fRunLoader->GetHeader() && fRunLoader->GetHeader()->GenEventHeader()) {
    fRunLoader->GetHeader()->GenEventHeader()->PrimaryVertex(mcVertex);
    for (Int_t i = 0; i < 3; i++) vtxPos[i] = mcVertex[i];
  }

  if (fITSVertexer) {
    Info("RunVertexFinder", "running the ITS vertex finder");
    fITSVertexer->SetDebug(1);
    vertex = fITSVertexer->FindVertexForCurrentEvent(fRunLoader->GetEventNumber());
    if(!vertex){
      Warning("RunVertexFinder","Vertex not found \n");
      vertex = new AliESDVertex();
    }
    else {
      vertex->SetTruePos(vtxPos);  // store also the vertex from MC
    }

  } else {
    Info("RunVertexFinder", "getting the primary vertex from MC");
    vertex = new AliESDVertex(vtxPos, vtxErr);
  }

  if (vertex) {
    vertex->GetXYZ(vtxPos);
    vertex->GetSigmaXYZ(vtxErr);
  } else {
    Warning("RunVertexFinder", "no vertex reconstructed");
    vertex = new AliESDVertex(vtxPos, vtxErr);
  }
  esd->SetVertex(vertex);
  if (fITSTracker) fITSTracker->SetVertex(vtxPos, vtxErr);
  if (fTPCTracker) fTPCTracker->SetVertex(vtxPos, vtxErr);
  if (fTRDTracker) fTRDTracker->SetVertex(vtxPos, vtxErr);
  delete vertex;

  Info("RunVertexFinder", "execution time:");
  stopwatch.Print();

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::RunTracking(AliESD*& esd)
{
// run the barrel tracking

  TStopwatch stopwatch;
  stopwatch.Start();

  if (!fTPCTracker) {
    Error("RunTracking", "no TPC tracker");
    return kFALSE;
  }

  // TPC tracking
  Info("RunTracking", "TPC tracking");
  fTPCLoader->LoadRecPoints("read");
  TTree* tpcTree = fTPCLoader->TreeR();
  if (!tpcTree) {
    Error("RunTracking", "Can't get the TPC cluster tree");
    return kFALSE;
  }
  fTPCTracker->LoadClusters(tpcTree);
  if (fTPCTracker->Clusters2Tracks(esd) != 0) {
    Error("RunTracking", "TPC Clusters2Tracks failed");
    return kFALSE;
  }
  if (fCheckPointLevel > 1) WriteESD(esd, "TPC.tracking");

  if (!fITSTracker) {
    Warning("RunTracking", "no ITS tracker");
  } else {

    fRunLoader->GetAliRun()->GetDetector("TPC")->FillESD(esd); // preliminary
    AliESDpid::MakePID(esd);                  // PID for the ITS tracker

    // ITS tracking
    Info("RunTracking", "ITS tracking");
    fITSLoader->LoadRecPoints("read");
    TTree* itsTree = fITSLoader->TreeR();
    if (!itsTree) {
      Error("RunTracking", "Can't get the ITS cluster tree");
      return kFALSE;
    }
    fITSTracker->LoadClusters(itsTree);
    if (fITSTracker->Clusters2Tracks(esd) != 0) {
      Error("RunTracking", "ITS Clusters2Tracks failed");
      return kFALSE;
    }
    if (fCheckPointLevel > 1) WriteESD(esd, "ITS.tracking");

    if (!fTRDTracker) {
      Warning("RunTracking", "no TRD tracker");
    } else {
      // ITS back propagation
      Info("RunTracking", "ITS back propagation");
      if (fITSTracker->PropagateBack(esd) != 0) {
	Error("RunTracking", "ITS backward propagation failed");
	return kFALSE;
      }
      if (fCheckPointLevel > 1) WriteESD(esd, "ITS.back");

      // TPC back propagation
      Info("RunTracking", "TPC back propagation");
      if (fTPCTracker->PropagateBack(esd) != 0) {
	Error("RunTracking", "TPC backward propagation failed");
	return kFALSE;
      }
      if (fCheckPointLevel > 1) WriteESD(esd, "TPC.back");

      // TRD back propagation
      Info("RunTracking", "TRD back propagation");
      fTRDLoader->LoadRecPoints("read");
      TTree* trdTree = fTRDLoader->TreeR();
      if (!trdTree) {
	Error("RunTracking", "Can't get the TRD cluster tree");
	return kFALSE;
      }
      fTRDTracker->LoadClusters(trdTree);
      if (fTRDTracker->PropagateBack(esd) != 0) {
	Error("RunTracking", "TRD backward propagation failed");
	return kFALSE;
      }
      if (fCheckPointLevel > 1) WriteESD(esd, "TRD.back");

      if (!fTOFTracker) {
	Warning("RunTracking", "no TOF tracker");
      } else {
	// TOF back propagation
	Info("RunTracking", "TOF back propagation");
	fTOFLoader->LoadDigits("read");
	TTree* tofTree = fTOFLoader->TreeD();
	if (!tofTree) {
	  Error("RunTracking", "Can't get the TOF digits tree");
	  return kFALSE;
	}
	fTOFTracker->LoadClusters(tofTree);
	if (fTOFTracker->PropagateBack(esd) != 0) {
	  Error("RunTracking", "TOF backward propagation failed");
	  return kFALSE;
	}
	if (fCheckPointLevel > 1) WriteESD(esd, "TOF.back");
	fTOFTracker->UnloadClusters();
	fTOFLoader->UnloadDigits();
      }

      // TRD inward refit
      Info("RunTracking", "TRD inward refit");
      if (fTRDTracker->RefitInward(esd) != 0) {
	Error("RunTracking", "TRD inward refit failed");
	return kFALSE;
      }
      if (fCheckPointLevel > 1) WriteESD(esd, "TRD.refit");
      fTRDTracker->UnloadClusters();
      fTRDLoader->UnloadRecPoints();
    
      // TPC inward refit
      Info("RunTracking", "TPC inward refit");
      if (fTPCTracker->RefitInward(esd) != 0) {
	Error("RunTracking", "TPC inward refit failed");
	return kFALSE;
      }
      if (fCheckPointLevel > 1) WriteESD(esd, "TPC.refit");
    
      // ITS inward refit
      Info("RunTracking", "ITS inward refit");
      if (fITSTracker->RefitInward(esd) != 0) {
	Error("RunTracking", "ITS inward refit failed");
	return kFALSE;
      }
      if (fCheckPointLevel > 1) WriteESD(esd, "ITS.refit");

    }  // if TRD tracker
    fITSTracker->UnloadClusters();
    fITSLoader->UnloadRecPoints();

  }  // if ITS tracker
  fTPCTracker->UnloadClusters();
  fTPCLoader->UnloadRecPoints();

  Info("RunTracking", "execution time:");
  stopwatch.Print();

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::FillESD(AliESD*& esd, const TString& detectors)
{
// fill the event summary data

  TStopwatch stopwatch;
  stopwatch.Start();

  TString detStr = detectors;
  TObjArray* detArray = fRunLoader->GetAliRun()->Detectors();
  for (Int_t iDet = 0; iDet < detArray->GetEntriesFast(); iDet++) {
    AliModule* det = (AliModule*) detArray->At(iDet);
    if (!det || !det->IsActive()) continue;
    if (IsSelected(det->GetName(), detStr)) {
      if (!ReadESD(esd, det->GetName())) {
	Info("FillESD", "filling ESD for %s", 
	     det->GetName());
	det->FillESD(esd);
	if (fCheckPointLevel > 2) WriteESD(esd, det->GetName());
      }
    }
  }

  if ((detStr.CompareTo("ALL") != 0) && !detStr.IsNull()) {
    Error("FillESD", "the following detectors were not found: %s", 
	  detStr.Data());
    if (fStopOnError) return kFALSE;
  }

  Info("FillESD", "execution time:");
  stopwatch.Print();

  return kTRUE;
}


//_____________________________________________________________________________
Bool_t AliReconstruction::IsSelected(TString detName, TString& detectors) const
{
// check whether detName is contained in detectors
// if yes, it is removed from detectors

  // check if all detectors are selected
  if ((detectors.CompareTo("ALL") == 0) ||
      detectors.BeginsWith("ALL ") ||
      detectors.EndsWith(" ALL") ||
      detectors.Contains(" ALL ")) {
    detectors = "ALL";
    return kTRUE;
  }

  // search for the given detector
  Bool_t result = kFALSE;
  if ((detectors.CompareTo(detName) == 0) ||
      detectors.BeginsWith(detName+" ") ||
      detectors.EndsWith(" "+detName) ||
      detectors.Contains(" "+detName+" ")) {
    detectors.ReplaceAll(detName, "");
    result = kTRUE;
  }

  // clean up the detectors string
  while (detectors.Contains("  ")) detectors.ReplaceAll("  ", " ");
  while (detectors.BeginsWith(" ")) detectors.Remove(0, 1);
  while (detectors.EndsWith(" ")) detectors.Remove(detectors.Length()-1, 1);

  return result;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::CreateVertexer()
{
// create the vertexer

  fITSVertexer = NULL;
  AliRun* aliRun = fRunLoader->GetAliRun();
  if (aliRun->GetDetector("ITS")) {
    fITSVertexer = aliRun->GetDetector("ITS")->CreateVertexer();
  }
  if (!fITSVertexer) {
    Warning("CreateVertexer", "couldn't create a vertexer for ITS");
    if (fStopOnError) return kFALSE;
  }

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliReconstruction::CreateTrackers()
{
// get the loaders and create the trackers

  AliRun* aliRun = fRunLoader->GetAliRun();

  fITSTracker = NULL;
  fITSLoader = fRunLoader->GetLoader("ITSLoader");
  if (!fITSLoader) {
    Warning("CreateTrackers", "no ITS loader found");
    if (fStopOnError) return kFALSE;
  } else {
    if (aliRun->GetDetector("ITS")) {
      fITSTracker = aliRun->GetDetector("ITS")->CreateTracker();
    }
    if (!fITSTracker) {
      Warning("CreateTrackers", "couldn't create a tracker for ITS");
      if (fStopOnError) return kFALSE;
    }
  }
    
  fTPCTracker = NULL;
  fTPCLoader = fRunLoader->GetLoader("TPCLoader");
  if (!fTPCLoader) {
    Error("CreateTrackers", "no TPC loader found");
    if (fStopOnError) return kFALSE;
  } else {
    if (aliRun->GetDetector("TPC")) {
      fTPCTracker = aliRun->GetDetector("TPC")->CreateTracker();
    }
    if (!fTPCTracker) {
      Error("CreateTrackers", "couldn't create a tracker for TPC");
      if (fStopOnError) return kFALSE;
    }
  }
    
  fTRDTracker = NULL;
  fTRDLoader = fRunLoader->GetLoader("TRDLoader");
  if (!fTRDLoader) {
    Warning("CreateTrackers", "no TRD loader found");
    if (fStopOnError) return kFALSE;
  } else {
    if (aliRun->GetDetector("TRD")) {
      fTRDTracker = aliRun->GetDetector("TRD")->CreateTracker();
    }
    if (!fTRDTracker) {
      Warning("CreateTrackers", "couldn't create a tracker for TRD");
      if (fStopOnError) return kFALSE;
    }
  }
    
  fTOFTracker = NULL;
  fTOFLoader = fRunLoader->GetLoader("TOFLoader");
  if (!fTOFLoader) {
    Warning("CreateTrackers", "no TOF loader found");
    if (fStopOnError) return kFALSE;
  } else {
    if (aliRun->GetDetector("TOF")) {
      fTOFTracker = aliRun->GetDetector("TOF")->CreateTracker();
    }
    if (!fTOFTracker) {
      Warning("CreateTrackers", "couldn't create a tracker for TOF");
      if (fStopOnError) return kFALSE;
    }
  }

  return kTRUE;
}

//_____________________________________________________________________________
void AliReconstruction::CleanUp(TFile* file)
{
// delete trackers and the run loader and close and delete the file

  delete fITSVertexer;
  fITSVertexer = NULL;
  delete fITSTracker;
  fITSTracker = NULL;
  delete fTPCTracker;
  fTPCTracker = NULL;
  delete fTRDTracker;
  fTRDTracker = NULL;
  delete fTOFTracker;
  fTOFTracker = NULL;

  delete fRunLoader;
  fRunLoader = NULL;

  if (file) {
    file->Close();
    delete file;
  }
}


//_____________________________________________________________________________
Bool_t AliReconstruction::ReadESD(AliESD*& esd, const char* recStep) const
{
// read the ESD event from a file

  if (!esd) return kFALSE;
  char fileName[256];
  sprintf(fileName, "ESD_%d.%d_%s.root", 
	  esd->GetRunNumber(), esd->GetEventNumber(), recStep);
  if (gSystem->AccessPathName(fileName)) return kFALSE;

  Info("ReadESD", "reading ESD from file %s", fileName);
  TFile* file = TFile::Open(fileName);
  if (!file || !file->IsOpen()) {
    Error("ReadESD", "opening %s failed", fileName);
    delete file;
    return kFALSE;
  }

  gROOT->cd();
  delete esd;
  esd = (AliESD*) file->Get("ESD");
  file->Close();
  delete file;
  return kTRUE;
}

//_____________________________________________________________________________
void AliReconstruction::WriteESD(AliESD* esd, const char* recStep) const
{
// write the ESD event to a file

  if (!esd) return;
  char fileName[256];
  sprintf(fileName, "ESD_%d.%d_%s.root", 
	  esd->GetRunNumber(), esd->GetEventNumber(), recStep);

  Info("WriteESD", "writing ESD to file %s", fileName);
  TFile* file = TFile::Open(fileName, "recreate");
  if (!file || !file->IsOpen()) {
    Error("WriteESD", "opening %s failed", fileName);
  } else {
    esd->Write("ESD");
    file->Close();
  }
  delete file;
}
