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
//  Base class for the clusterization algorithm (pure abstract)
//*--
//*-- Author: Yves Schutz  SUBATECH 
// Modif: 
//  August 2002 Yves Schutz: clone PHOS as closely as possible and intoduction
//                           of new  IO (� la PHOS)
//////////////////////////////////////////////////////////////////////////////

// --- ROOT system ---

// --- Standard library ---


// --- AliRoot header files ---
#include "AliRun.h" 
#include "AliEMCALClusterizer.h"

ClassImp(AliEMCALClusterizer)

//____________________________________________________________________________
  AliEMCALClusterizer::AliEMCALClusterizer():TTask("","")
{
  // ctor
  fEventFolderName = "" ;  
}

//____________________________________________________________________________
AliEMCALClusterizer::AliEMCALClusterizer(const TString alirunFileName, const TString eventFolderName):
  TTask("EMCAL"+AliConfig::fgkReconstructionerTaskName, alirunFileName), fEventFolderName(eventFolderName)
{
  // ctor
  
}

//____________________________________________________________________________
AliEMCALClusterizer::~AliEMCALClusterizer()
{
  // dtor
}

