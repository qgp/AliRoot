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
//_________________________________________________________________________
// Algorithm Base class to construct PHOS track segments
// Associates EMC and PPSD clusters
// Unfolds the EMC cluster   
//*-- 
//*-- Author: Dmitri Peressounko (RRC Ki & SUBATECH)


// --- ROOT system ---
#include "TGeometry.h"
#include "TFile.h"
#include "TTree.h"

// --- Standard library ---
#include <iostream.h>
#include <stdlib.h>   

// --- AliRoot header files ---
#include "AliRun.h" 
#include "AliPHOSTrackSegmentMaker.h"
#include "AliHeader.h" 

ClassImp( AliPHOSTrackSegmentMaker) 


//____________________________________________________________________________
  AliPHOSTrackSegmentMaker:: AliPHOSTrackSegmentMaker() : TTask("","")
{
  // ctor
  fSplitFile= 0 ; 

}

//____________________________________________________________________________
AliPHOSTrackSegmentMaker::AliPHOSTrackSegmentMaker(const char * headerFile, const char * name): TTask(name, headerFile)
{
  // ctor
  fSplitFile= 0 ; 
}

//____________________________________________________________________________
AliPHOSTrackSegmentMaker::~AliPHOSTrackSegmentMaker()
{
   
      fSplitFile = 0 ;
}

//____________________________________________________________________________
void AliPHOSTrackSegmentMaker::SetSplitFile(const TString splitFileName) const
{
  // Diverts the TrackSegments in a file separate from the Digits file
  

  TDirectory * cwd = gDirectory ;
  TFile * splitFile = gAlice->InitTreeFile("R",splitFileName.Data());
  splitFile->cd() ; 
  gAlice->Write(0, TObject::kOverwrite);

  TTree *treeE  = gAlice->TreeE();
  if (!treeE) {
    cerr << "ERROR: AliPHOSTrackSegmentMaker::SetSplitFile -> No TreeE found "<<endl;
    abort() ;
  }      
  
  // copy TreeE
  AliHeader *header = new AliHeader();
  treeE->SetBranchAddress("Header", &header);
  treeE->SetBranchStatus("*",1);
  TTree *treeENew =  treeE->CloneTree();
  treeENew->Write(0, TObject::kOverwrite);
  
  // copy AliceGeom
  TGeometry *AliceGeom = static_cast<TGeometry*>(cwd->Get("AliceGeom"));
  if (!AliceGeom) {
    cerr << "ERROR: AliPHOSTrackSegmentMaker::SetSplitFile -> AliceGeom was not found in the input file "<<endl;
    abort() ;
  }
  AliceGeom->Write(0, TObject::kOverwrite);
  
  gAlice->MakeTree("R",splitFile);
  cwd->cd() ; 
  cout << "INFO: AliPHOSTrackSegmentMaker::SetSPlitMode -> TrackSegments will be stored in " << splitFileName.Data() << endl ;   
}
