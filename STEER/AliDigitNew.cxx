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
Revision 1.3  2002/10/29 14:26:49  hristov
Code clean-up (F.Carminati)

Revision 1.2  2000/02/14 11:44:29  fca
New version from Y.Schutz

Revision 1.1  1999/12/17 09:01:14  fca
Y.Schutz new classes for reconstruction

*/

#include "AliDigitNew.h"
 
ClassImp(AliDigitNew)

//_______________________________________________________________________
AliDigitNew::AliDigitNew():
  fAmp(0),
  fId(0),
  fIndexInList(0)
{
  //
  // Default constructor
  //
}

	 
