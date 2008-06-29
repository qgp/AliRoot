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
//...
//  Checks the quality assurance for ACORDE. 
//  Default implementation
//  Authors:
//	Mario Rodriguez Cahuantzi <mrodrigu@mail.cern.ch> (FCFM-BUAP) 
//	Luciano Diaz Gonzalez <luciano.diaz@nucleares.unam.mx> (ICN-UNAM)
//	Arturo Fernandez <afernan@mail.cern.ch> (FCFM-BUAP)
//...

// --- ROOT system ---
#include <TClass.h>
#include <TH1F.h> 
#include <TH1I.h> 
#include <TIterator.h> 
#include <TKey.h> 
#include <TFile.h> 

// --- Standard library ---

// --- AliRoot header files ---
#include "AliLog.h"
#include "AliQA.h"
#include "AliQAChecker.h"
#include "AliACORDEQAChecker.h"

ClassImp(AliACORDEQAChecker)

//__________________________________________________________________
AliACORDEQAChecker& AliACORDEQAChecker::operator = (const AliACORDEQAChecker& qac )
{
  // Equal operator.
  this->~AliACORDEQAChecker();
  new(this) AliACORDEQAChecker(qac);
  return *this;
}
