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
// Generator for vertices taken from a file                                  //
//                                                                           //
// The file name of the galice file is passed as argument to the             //
// constructor. If a second argument is given, this determines the number    //
// of events for which the same vertex is used.                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include "AliVertexGenFile.h"
#include "AliHeader.h"
#include "AliGenEventHeader.h"
#include <TFile.h>
#include <TTree.h>
#include <TArrayF.h>


ClassImp(AliVertexGenFile)


//_____________________________________________________________________________
AliVertexGenFile::AliVertexGenFile() :
  fFile(NULL),
  fTree(NULL),
  fHeader(NULL),
  fEventsPerEntry(0),
  fEvent(0)
{
// default constructor: initialize data members

}

//_____________________________________________________________________________
AliVertexGenFile::AliVertexGenFile(const char* fileName, 
				   Int_t eventsPerEntry) :
  fFile(NULL),
  fTree(NULL),
  fHeader(NULL),
  fEventsPerEntry(eventsPerEntry),
  fEvent(0)
{
// main constructor:
// fileName is the name of the galice file containing the vertices
// eventsPerEntry is the number of events for which the same vertex is used

  TDirectory* dir = gDirectory;

  fFile = TFile::Open(fileName);
  if (!fFile) {
    Error("AliVertexGenFile", "could not open file %s", fileName);
    return;
  }
  fTree = (TTree*) fFile->Get("TE");
  if (!fTree) {
    Error("AliVertexGenFile", "not header tree found in file %s", fileName);
    dir->cd();
    return;
  }
  fHeader = new AliHeader;
  fTree->SetBranchAddress("Header", &fHeader);

  dir->cd();
}

//_____________________________________________________________________________
AliVertexGenFile::~AliVertexGenFile()
{
// clean up

  fFile->Close();
  delete fFile;
  delete fHeader;
}


//_____________________________________________________________________________
TVector3 AliVertexGenFile::GetVertex()
{
// get the vertex from the event header tree

  Int_t entry = fEvent++ / fEventsPerEntry;
  if (fTree->GetEntry(entry) <= 0) {
    Error("GetVertex", "error loading entry %d", entry);
    return TVector3(0,0,0);
  }

  if (!fHeader->GenEventHeader()) {
    Error("GetVertex", "no generator event header");
    return TVector3(0,0,0);
  }

  TArrayF vertex(3);
  fHeader->GenEventHeader()->PrimaryVertex(vertex);
  return TVector3(vertex[0], vertex[1], vertex[2]);
}


