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
Revision 1.4  2002/10/29 14:26:49  hristov
Code clean-up (F.Carminati)

Revision 1.3  2000/12/21 15:30:18  fca
Correcting coding convention violations

Revision 1.2  2000/12/01 08:40:48  alibrary
Correction of a small bug - sRandom can be used now

Revision 1.1  2000/11/30 07:12:48  alibrary
Introducing new Rndm and QA classes

*/

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TFile.h"
#include "TError.h"
#include "TRandom3.h"
#include "TSystem.h"

#include "AliRndm.h"

ClassImp(AliRndm)

//_______________________________________________________________________
AliRndm::AliRndm():
  fRandom(0)
{
  // 
  // Default ctor
  //
  SetRandom();
}

//_______________________________________________________________________
AliRndm::AliRndm(const AliRndm& rn):
  fRandom(0)
{
  //
  // Copy constructor
  //
  rn.Copy(*this);
}

//_______________________________________________________________________
void AliRndm::Copy(AliRndm&) const
{
  ::Fatal("Copy","Not implemented\n");
}


//_____________________________________________________________________________
void AliRndm::Rndm(Float_t* array, const Int_t size) const
{
  //
  // Return an array of n random numbers uniformly distributed 
  // between 0 and 1 not included
  //
  for(Int_t i=0; i<size; i++) 
#ifdef CKNONE
    array[i]=fRandom->Rndm();
#else
    do array[i]=fRandom->Rndm(); while(0>=array[i] || array[i]>=1);
#endif
}

//_____________________________________________________________________________
void AliRndm::ReadRandom(const char *filename)
{
  //
  // Reads saved random generator status from filename
  //
  char *fntmp = gSystem->ExpandPathName(filename);
  TFile *file = new TFile(fntmp,"r");
  delete [] fntmp;
  if(!file) {
    printf("AliRndm:: Could not open file %s\n",filename);
  } else {
    if(!fRandom) fRandom = new TRandom();
    fRandom->Read("Random");
    file->Close();
    delete file;
  }
}

//_____________________________________________________________________________
void AliRndm::WriteRandom(const char *filename) const
{
  //
  // Writes random generator status to filename
  //
  char *fntmp = gSystem->ExpandPathName(filename);
  TFile *file = new TFile(fntmp,"new");
  delete [] fntmp;
  if(!file) {
    printf("AliRndm:: Could not open file %s\n",filename);
  } else {
    fRandom->Write();
    file->Close();
    delete file;
  }
}
