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

//*-- Author: Rachid Guernane (LPCCFd)

#include "AliMUONTriggerCrate.h"
#include "AliMUONTriggerBoard.h"

ClassImp(AliMUONTriggerCrate)

//___________________________________________
AliMUONTriggerCrate::AliMUONTriggerCrate()
: fNslots(0),
  fNboards(0),
  fBoards(0x0)
{
    // Def Ctor
}

//___________________________________________
AliMUONTriggerCrate::~AliMUONTriggerCrate()
{
  delete fBoards;
}

//___________________________________________
AliMUONTriggerCrate::AliMUONTriggerCrate(const char *name, Int_t n) : TNamed(name,"Regional trigger crate")
{
   fNslots = n;
   fBoards = new TObjArray(fNslots);
}

//___________________________________________
void AliMUONTriggerCrate::AddBoard(AliMUONTriggerBoard *board, Int_t i)
{
   fBoards->AddAt(board,i);
   fNboards++;
}

//___________________________________________
AliMUONTriggerCrate::AliMUONTriggerCrate(const AliMUONTriggerCrate &crate) : TNamed(crate)
{
// Dummy Copy Ctor
   crate.Copy(*this);
}

//___________________________________________
AliMUONTriggerCrate& AliMUONTriggerCrate::operator=(const AliMUONTriggerCrate &rhs)
{
// Assignment optor
   rhs.Copy(*this);
   return (*this);
}

//___________________________________________
void AliMUONTriggerCrate::Copy(TObject&) const
{
   Fatal("Copy","Not implemented!\n");
}
