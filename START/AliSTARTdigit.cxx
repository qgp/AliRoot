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
Revision 1.3  2000/07/13 16:41:29  fca
New START corrected for coding conventions

Revision 1.2  2000/03/24 17:40:35  alla
New AliSTART

*/ 
#include <iostream.h>
#include <fstream.h>

#include "AliRun.h"
#include "AliSTART.h"
#include "AliSTARTdigit.h"

ClassImp(AliSTARTdigit)

AliSTARTdigit::AliSTARTdigit(Int_t Timeav, Int_t Timediff)
{
  //
  // Create START digit
  //     The creator for the AliSTARTdigit class. This routine fills the
  // AliSTARTdigit data members from the array digits. The array of track
  // numbers are passed to the AliDigit creator. The order of the elements
  // in the digits array are 
  // fTimeAverage = digits[0], fTimeDiff = digits[2].
  //
  Timeav = fTimeAverage;
  Timediff = fTimeDiff;

}
