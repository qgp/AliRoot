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

/*
$Log$
Revision 1.16.6.1  2002/06/10 15:28:58  hristov
Merged with v3-08-02

Revision 1.18  2002/04/12 12:13:23  cblume
Add Jiris changes

Revision 1.17  2002/03/28 14:59:07  cblume
Coding conventions

Revision 1.18  2002/04/12 12:13:23  cblume
Add Jiris changes

Revision 1.17  2002/03/28 14:59:07  cblume
Coding conventions

Revision 1.16  2002/02/12 11:42:08  cblume
Remove fTree from destructor

Revision 1.15  2002/02/11 14:27:54  cblume
Geometry and hit structure update

Revision 1.14  2001/11/14 10:50:46  cblume
Changes in digits IO. Add merging of summable digits

Revision 1.13  2001/11/06 17:19:41  cblume
Add detailed geometry and simple simulator

Revision 1.12  2001/05/16 14:57:28  alibrary
New files for folders and Stack

Revision 1.11  2001/03/13 09:30:35  cblume
Update of digitization. Moved digit branch definition to AliTRD

Revision 1.10  2001/01/26 19:56:57  hristov
Major upgrade of AliRoot code

Revision 1.9  2000/11/02 09:25:53  cblume
Change also the dictionary to AliTRDdataArray

Revision 1.8  2000/11/01 15:20:13  cblume
Change AliTRDdataArrayI to AliTRDdataArray in MakeBranch()

Revision 1.7  2000/11/01 14:53:20  cblume
Merge with TRD-develop

Revision 1.1.2.5  2000/10/17 02:27:34  cblume
Get rid of global constants

Revision 1.1.2.4  2000/10/15 23:40:01  cblume
Remove AliTRDconst

Revision 1.1.2.3  2000/10/06 16:49:46  cblume
Made Getters const

Revision 1.1.2.2  2000/10/04 16:34:58  cblume
Replace include files by forward declarations

Revision 1.5  2000/06/09 11:10:07  cblume
Compiler warnings and coding conventions, next round

Revision 1.4  2000/06/08 18:32:58  cblume
Make code compliant to coding conventions

Revision 1.3  2000/06/07 16:27:01  cblume
Try to remove compiler warnings on Sun and HP

Revision 1.2  2000/05/08 16:17:27  cblume
Merge TRD-develop

Revision 1.1.2.1  2000/05/08 14:44:01  cblume
Add new class AliTRDdigitsManager

*/

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Manages the digits and the track dictionary in the form of               //
//  AliTRDdataArray objects.                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
 
#include <TROOT.h>
#include <TTree.h>                                                              
#include <TFile.h>

#include "AliRun.h"

#include "AliTRDdigitsManager.h"
#include "AliTRDsegmentArray.h"
#include "AliTRDdataArrayI.h"
#include "AliTRDdigit.h"
#include "AliTRDgeometry.h"
#include "AliTRD.h"

ClassImp(AliTRDdigitsManager)

//_____________________________________________________________________________

  // Number of track dictionary arrays
  const Int_t AliTRDdigitsManager::fgkNDict = kNDict;

//_____________________________________________________________________________
AliTRDdigitsManager::AliTRDdigitsManager():TObject()
{
  //
  // Default constructor
  //

  fIsRaw   = kFALSE;
  fEvent   = 0;
  fDebug   = 0;
  fSDigits = 0;

  fFile    = NULL;
  fTree    = NULL;
  fDigits  = NULL;
  for (Int_t iDict = 0; iDict < kNDict; iDict++) {
    fDictionary[iDict] = NULL;
  }

}

//_____________________________________________________________________________
AliTRDdigitsManager::AliTRDdigitsManager(const AliTRDdigitsManager &m)
{
  //
  // AliTRDdigitsManager copy constructor
  //

  ((AliTRDdigitsManager &) m).Copy(*this);

}

//_____________________________________________________________________________
AliTRDdigitsManager::~AliTRDdigitsManager()
{
  //
  // AliTRDdigitsManager destructor
  //

  if (fFile) {
    fFile->Close();
    delete fFile;
    fFile = NULL;
  }

  if (fDigits) {
    fDigits->Delete();
    delete fDigits;
    fDigits = NULL;
  }

  for (Int_t iDict = 0; iDict < kNDict; iDict++) {
    fDictionary[iDict]->Delete();
    delete fDictionary[iDict];
    fDictionary[iDict] = NULL;
  }

}

//_____________________________________________________________________________
void AliTRDdigitsManager::Copy(TObject &m)
{
  //
  // Copy function
  //

  ((AliTRDdigitsManager &) m).fIsRaw   = fIsRaw;
  ((AliTRDdigitsManager &) m).fEvent   = fEvent;
  ((AliTRDdigitsManager &) m).fDebug   = fDebug;
  ((AliTRDdigitsManager &) m).fSDigits = fSDigits;

  TObject::Copy(m);

}

//_____________________________________________________________________________
void AliTRDdigitsManager::CreateArrays()
{
  //
  // Create the data arrays
  //

  fDigits = new AliTRDsegmentArray("AliTRDdataArrayI",AliTRDgeometry::Ndet());

  for (Int_t iDict = 0; iDict < kNDict; iDict++) {
    fDictionary[iDict] = new AliTRDsegmentArray("AliTRDdataArrayI"
                                               ,AliTRDgeometry::Ndet());
  }

}

//_____________________________________________________________________________
void AliTRDdigitsManager::SetRaw()
{
  //
  // Switch on the raw digits flag
  //

  fIsRaw = kTRUE;

  fDigits->SetBit(AliTRDdigit::RawDigit());
  
}

//_____________________________________________________________________________
Short_t AliTRDdigitsManager::GetDigitAmp(Int_t row, Int_t col,Int_t time
                                       , Int_t det) const
{
  //
  // Returns the amplitude of a digit
  //

  return ((Short_t) GetDigits(det)->GetData(row,col,time));

}
 
//_____________________________________________________________________________
Bool_t AliTRDdigitsManager::Open(const Char_t *file)
{
  //
  // Opens the file for the TRD digits
  //

  fFile = (TFile*) gROOT->GetListOfFiles()->FindObject(file);
  if (!fFile) {
    if (fDebug > 0) {
      printf("<AliTRDdigitsManager::Open> ");
      printf("Open the AliROOT-file %s.\n",file);
    }
    fFile = new TFile(file,"UPDATE");
    if (!fFile) return kFALSE;
  }
  else {
    if (fDebug > 0) {
      printf("<AliTRDdigitsManager::Open> ");
      printf("%s is already open.\n",file);
    }
  }

  return kTRUE;

}

//_____________________________________________________________________________
void AliTRDdigitsManager::MakeTreeAndBranches(TFile *file, Int_t iEvent)
{
  //
  // Creates tree for (s)digits in the specified file
  //

  fEvent = iEvent;
  TDirectory *wd = gDirectory;
  file->cd();
  MakeBranch();
  wd->cd();

}

//_____________________________________________________________________________
Bool_t AliTRDdigitsManager::MakeBranch(const Char_t *file)
{
  //
  // Creates the tree and branches for the digits and the dictionary
  //

  // Create the TRD digits tree
  TTree *tree;
  Char_t treeName[12];
  if (fSDigits) {
    sprintf(treeName,"TreeS%d_TRD",fEvent);
    tree = new TTree(treeName,"TRD SDigits");
  }
  else {
    sprintf(treeName,"TreeD%d_TRD",fEvent);
    tree = new TTree(treeName,"TRD Digits");
  }

  if (fDebug > 0) {
    printf("<AliTRDdigitsManager::MakeBranch> ");
    printf("Creating tree %s\n",treeName);
  }

  return MakeBranch(tree,file);

}

//_____________________________________________________________________________
Bool_t AliTRDdigitsManager::MakeBranch(TTree *tree, const Char_t *file)
{
  //
  // Creates the tree and branches for the digits and the dictionary
  //

  Int_t buffersize = 64000;

  Bool_t status = kTRUE;

  AliTRD *trd = (AliTRD *) gAlice->GetDetector("TRD") ;

  if (tree) {
    fTree = tree;
  }

  // Make the branch for the digits
  if (fDigits) {
    const AliTRDdataArray *kDigits = (AliTRDdataArray *) fDigits->At(0);
    if (kDigits) {
      trd->MakeBranchInTree(fTree,"TRDdigits",kDigits->IsA()->GetName()
                                 ,&kDigits,buffersize,99,file);
      if (fDebug > 0) {
        printf("<AliTRDdigitsManager::MakeBranch> ");
        printf("Making branch TRDdigits\n");
      }
    }
    else {
      status = kFALSE;
    }
  }
  else {
    status = kFALSE;
  }

  // Make the branches for the dictionaries
  for (Int_t iDict = 0; iDict < kNDict; iDict++) {
    Char_t branchname[15];
    sprintf(branchname,"TRDdictionary%d",iDict);
    if (fDictionary[iDict]) {
      const AliTRDdataArray *kDictionary = 
              (AliTRDdataArray *) fDictionary[iDict]->At(0);
      if (kDictionary) {
        trd->MakeBranchInTree(fTree,branchname,kDictionary->IsA()->GetName()
                             ,&kDictionary,buffersize,99,file);
        if (fDebug > 0) {
          printf("<AliTRDdigitsManager::MakeBranch> ");
          printf("Making branch %s\n",branchname);
	}
      }
      else {
        status = kFALSE;
      }
    }
    else {
      status = kFALSE;
    }
  }

  return status;

}

//_____________________________________________________________________________
Bool_t AliTRDdigitsManager::ReadDigits(TTree *tree)
{
  //
  // Reads the digit information from the input file
  //

  Bool_t status = kTRUE;

  if (tree) {

    fTree = tree;

  }
  else {

    // Get the digits tree
    Char_t treeName[12];
    if (fSDigits) {
      sprintf(treeName,"TreeS%d_TRD",fEvent);
    }
    else {
      sprintf(treeName,"TreeD%d_TRD",fEvent);
    }
    if (fFile) {
      fTree = (TTree *) fFile->Get(treeName);
    }
    else {
      fTree = (TTree *) gDirectory->Get(treeName);
    }

    if (!fTree) {
      if (fDebug > 0) {
        printf("<AliTRDdigitsManager::ReadDigits> ");
        printf("Could not find tree %s.\n",treeName);
      }
      return kFALSE;
    }

  }

  if (!fDigits) {
    if (fDebug > 0) {
      printf("<AliTRDdigitsManager::ReadDigits> ");
      printf("Create the data arrays.\n");
    }
    CreateArrays();
  }

  status = fDigits->LoadArray("TRDdigits",fTree);

  for (Int_t iDict = 0; iDict < kNDict; iDict++) {
    Char_t branchname[15];
    sprintf(branchname,"TRDdictionary%d",iDict);
    status = fDictionary[iDict]->LoadArray(branchname,fTree);
  }  

  if (fDigits->TestBit(AliTRDdigit::RawDigit())) {
    fIsRaw = kTRUE;
  }
  else {
    fIsRaw = kFALSE;
  }

  return kTRUE;

}

//_____________________________________________________________________________
Bool_t AliTRDdigitsManager::WriteDigits()
{
  //
  // Writes out the TRD-digits and the dictionaries
  //

  // Store the contents of the segment array in the tree
  if (!fDigits->StoreArray("TRDdigits",fTree)) {
    printf("<AliTRDdigitsManager::WriteDigits> ");
    printf("Error while storing digits in branch TRDdigits\n");
    return kFALSE;
  }
  for (Int_t iDict = 0; iDict < kNDict; iDict++) {
    Char_t branchname[15];
    sprintf(branchname,"TRDdictionary%d",iDict);
    if (!fDictionary[iDict]->StoreArray(branchname,fTree)) {
      printf("<AliTRDdigitsManager::WriteDigits> ");
      printf("Error while storing dictionary in branch %s\n",branchname);
      return kFALSE;
    }
  }

  // Write the new tree to the output file
  //fTree->Write();
  fTree->AutoSave();  // Modification by Jiri

  return kTRUE;

}

//_____________________________________________________________________________
AliTRDdigit *AliTRDdigitsManager::GetDigit(Int_t row, Int_t col
                                         , Int_t time, Int_t det) const
{
  // 
  // Creates a single digit object 
  //

  Int_t digits[4];
  Int_t amp[1];

  digits[0] = det;
  digits[1] = row;
  digits[2] = col;
  digits[3] = time;

  amp[0]    = GetDigits(det)->GetData(row,col,time);
  
  return (new AliTRDdigit(fIsRaw,digits,amp));

}

//_____________________________________________________________________________
Int_t AliTRDdigitsManager::GetTrack(Int_t track
                                  , Int_t row, Int_t col, Int_t time
                                  , Int_t det) const
{
  // 
  // Returns the MC-track numbers from the dictionary.
  //

  if ((track < 0) || (track >= kNDict)) {
    TObject::Error("GetTracks"
                  ,"track %d out of bounds (size: %d, this: 0x%08x)"
                  ,track,kNDict,this);
    return -1;
  }

  // Array contains index+1 to allow data compression
  return (GetDictionary(det,track)->GetData(row,col,time) - 1);

}

//_____________________________________________________________________________
AliTRDdataArrayI *AliTRDdigitsManager::GetDigits(Int_t det) const
{
  //
  // Returns the digits array for one detector
  //

  return (AliTRDdataArrayI *) fDigits->At(det);

}

//_____________________________________________________________________________
AliTRDdataArrayI *AliTRDdigitsManager::GetDictionary(Int_t det, Int_t i) const
{
  //
  // Returns the dictionary for one detector
  //

  return (AliTRDdataArrayI *) fDictionary[i]->At(det);

}

//_____________________________________________________________________________
Int_t AliTRDdigitsManager::GetTrack(Int_t track, AliTRDdigit *Digit) const
{
  // 
  // Returns the MC-track numbers from the dictionary for a given digit
  //

  Int_t row  = Digit->GetRow();
  Int_t col  = Digit->GetCol();
  Int_t time = Digit->GetTime();
  Int_t det  = Digit->GetDetector();

  return GetTrack(track,row,col,time,det);

}

//_____________________________________________________________________________
AliTRDdigitsManager &AliTRDdigitsManager::operator=(const AliTRDdigitsManager &m)
{
  //
  // Assignment operator
  //

  if (this != &m) ((AliTRDdigitsManager &) m).Copy(*this);
  return *this;

}
