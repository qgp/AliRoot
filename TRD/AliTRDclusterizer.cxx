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
//  TRD cluster finder base class                                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>

#include "AliRun.h"
#include "AliRunLoader.h"
#include "AliLoader.h"

#include "AliTRDclusterizer.h"
#include "AliTRDcluster.h"
#include "AliTRDrecPoint.h"
#include "AliTRDgeometry.h"
#include "AliTRDparameter.h"

ClassImp(AliTRDclusterizer)

//_____________________________________________________________________________
AliTRDclusterizer::AliTRDclusterizer():TNamed()
{
  //
  // AliTRDclusterizer default constructor
  //

  fClusterTree = NULL;
  fRecPoints   = 0;
  fVerbose     = 0;
  fPar         = 0;

}

//_____________________________________________________________________________
AliTRDclusterizer::AliTRDclusterizer(const Text_t* name, const Text_t* title)
                  :TNamed(name, title)
{
  //
  // AliTRDclusterizer default constructor
  //

  fClusterTree = NULL;
  fRecPoints   = 0;
  fVerbose     = 0;
  fPar         = 0;

}

//_____________________________________________________________________________
AliTRDclusterizer::AliTRDclusterizer(const AliTRDclusterizer &c):TNamed(c)
{
  //
  // AliTRDclusterizer copy constructor
  //

  ((AliTRDclusterizer &) c).Copy(*this);

}

//_____________________________________________________________________________
AliTRDclusterizer::~AliTRDclusterizer()
{
  //
  // AliTRDclusterizer destructor
  //

  if (fRecPoints) {
    fRecPoints->Delete();
    delete fRecPoints;
  }
}

//_____________________________________________________________________________
AliTRDclusterizer &AliTRDclusterizer::operator=(const AliTRDclusterizer &c)
{
  //
  // Assignment operator
  //

  if (this != &c) ((AliTRDclusterizer &) c).Copy(*this);
  return *this;

}

//_____________________________________________________________________________
void AliTRDclusterizer::Copy(TObject &c) const
{
  //
  // Copy function
  //

  ((AliTRDclusterizer &) c).fClusterTree = NULL;
  ((AliTRDclusterizer &) c).fRecPoints   = NULL;  
  ((AliTRDclusterizer &) c).fVerbose     = fVerbose;  
  ((AliTRDclusterizer &) c).fPar         = 0;

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::Open(const Char_t *name, Int_t nEvent)
{
  //
  // Opens the AliROOT file. Output and input are in the same file
  //
  TString evfoldname = AliConfig::GetDefaultEventFolderName();
  fRunLoader = AliRunLoader::GetRunLoader(evfoldname);
  if (!fRunLoader)
    fRunLoader = AliRunLoader::Open(name);
  if (!fRunLoader)
   {
     Error("Open","Can not open session for file %s.",name);
     return kFALSE;
   }

  OpenInput(nEvent);
  OpenOutput();
  return kTRUE;
}


//_____________________________________________________________________________
Bool_t AliTRDclusterizer::OpenOutput()
{
  //
  // Open the output file
  //

  TObjArray *ioArray = 0;

  AliLoader* loader = fRunLoader->GetLoader("TRDLoader");
  loader->MakeTree("R");
  fClusterTree = loader->TreeR();
  fClusterTree->Branch("TRDcluster","TObjArray",&ioArray,32000,0);


  return kTRUE;

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::OpenInput(Int_t nEvent)
{
  //
  // Opens a ROOT-file with TRD-hits and reads in the digits-tree
  //

  // Connect the AliRoot file containing Geometry, Kine, and Hits
  if (fRunLoader->GetAliRun() == 0x0) fRunLoader->LoadgAlice();
  gAlice = fRunLoader->GetAliRun();

  if (!(gAlice)) {
    fRunLoader->LoadgAlice();
    gAlice = fRunLoader->GetAliRun();
      if (!(gAlice)) {
        printf("AliTRDclusterizer::OpenInput -- ");
        printf("Could not find AliRun object.\n");
        return kFALSE;
      }
  }

  // Import the Trees for the event nEvent in the file
  fRunLoader->GetEvent(nEvent);
  
  return kTRUE;

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizer::WriteClusters(Int_t det)
{
  //
  // Fills TRDcluster branch in the tree with the clusters 
  // found in detector = det. For det=-1 writes the tree. 
  //

  if ((det < -1) || (det >= AliTRDgeometry::Ndet())) {
    printf("AliTRDclusterizer::WriteClusters -- ");
    printf("Unexpected detector index %d.\n",det);
    return kFALSE;
  }
 

  TBranch *branch = fClusterTree->GetBranch("TRDcluster");
  if (!branch) {
    TObjArray *ioArray = 0;
    branch = fClusterTree->Branch("TRDcluster","TObjArray",&ioArray,32000,0);
  }

  if ((det >= 0) && (det < AliTRDgeometry::Ndet())) {

    Int_t nRecPoints = RecPoints()->GetEntriesFast();
    TObjArray *detRecPoints = new TObjArray(400);

    for (Int_t i = 0; i < nRecPoints; i++) {
      AliTRDcluster *c = (AliTRDcluster *) RecPoints()->UncheckedAt(i);
      if (det == c->GetDetector()) {
        detRecPoints->AddLast(c);
      }
      else {
        printf("AliTRDclusterizer::WriteClusters --");
        printf("Attempt to write a cluster with unexpected detector index\n");
      }
    }

    branch->SetAddress(&detRecPoints);
    fClusterTree->Fill();

    delete detRecPoints;

    return kTRUE;

  }

  if (det == -1) {

    Info("WriteClusters","Writing the cluster tree %s for event %d."
	 ,fClusterTree->GetName(),fRunLoader->GetEventNumber());
    /*
    fClusterTree->Write();
    AliTRDgeometry *geo = fTRD->GetGeometry();
    geo->SetName("TRDgeometry");
    geo->Write();
    fPar->Write();
    */
    AliLoader* loader = fRunLoader->GetLoader("TRDLoader");
    loader->WriteRecPoints("OVERWRITE");
  
    return kTRUE;  

  }
  /*
  AliLoader* loader = fRunLoader->GetLoader("TRDLoader");
  loader->WriteDigits("OVERWRITE");
  */
  printf("AliTRDclusterizer::WriteClusters -- ");
  printf("Unexpected detector index %d.\n",det);
 
  return kFALSE;  
  
}


//_____________________________________________________________________________
AliTRDcluster *  AliTRDclusterizer::AddCluster(Double_t *pos, Int_t det, Double_t amp
				   , Int_t *tracks, Double_t *sig, Int_t iType, Float_t center)
{
  //
  // Add a cluster for the TRD
  //

  AliTRDcluster *c = new AliTRDcluster();

  c->SetDetector(det);
  c->AddTrackIndex(tracks);
  c->SetQ(amp);
  c->SetY(pos[0]);
  c->SetZ(pos[1]);
  c->SetSigmaY2(sig[0]);   
  c->SetSigmaZ2(sig[1]);
  c->SetLocalTimeBin(((Int_t) pos[2]));
  c->SetCenter(center);
  switch (iType) {
  case 0:
    c->Set2pad();
    break;
  case 1:
    c->Set3pad();
    break;
  case 2:
    c->Set4pad();
    break;
  case 3:
    c->Set5pad();
    break;
  case 4:
    c->SetLarge();
    break;
  };

  RecPoints()->Add(c);
  return c;
}
