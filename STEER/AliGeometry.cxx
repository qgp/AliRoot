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
Revision 1.4  2002/05/31 08:04:10  morsch
Impact methode purely virtual.

Revision 1.3  2002/05/30 15:02:31  morsch
Impact method added. (G. Martinez)

Revision 1.2  2000/07/12 08:56:25  fca
Coding convention correction and warning removal

Revision 1.1  1999/12/17 09:01:14  fca
Y.Schutz new classes for reconstruction

*/

//-*-C++-*-
//_________________________________________________________________________
// AliGeometry base class is pABC
//*-- Author : Yves Schutz  SUBATECH 
//////////////////////////////////////////////////////////////////////////////

// --- ROOT system ---

// --- Standard library ---

// --- AliRoot header files ---

#include "AliGeometry.h"

ClassImp(AliGeometry)

//____________________________________________________________________________
AliGeometry::AliGeometry()
{
  //
  // ctor
  //
}

//____________________________________________________________________________
AliGeometry::~AliGeometry()
{
  // dtor
}
//____________________________________________________________________________
