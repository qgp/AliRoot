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
//////////////////////////////////////////////////////////////////////////////

// --- ROOT system ---
#include "TGeometry.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TTree.h"

// --- Standard library ---
#include <stdlib.h>   

// --- AliRoot header files ---
#include "AliRun.h" 
#include "AliPHOSClusterizer.h"
#include "AliHeader.h" 
#include "AliPHOSGetter.h"
#include "AliPHOSSDigitizer.h"
#include "AliPHOSDigitizer.h"

ClassImp(AliPHOSClusterizer)

//____________________________________________________________________________
  AliPHOSClusterizer::AliPHOSClusterizer():TTask("","")
{
  // ctor

  fEventFolderName = "" ; 
}

//____________________________________________________________________________
AliPHOSClusterizer::AliPHOSClusterizer(const TString alirunFileName, const TString eventFolderName):
  TTask("PHOS"+AliConfig::fgkReconstructionerTaskName, alirunFileName), fEventFolderName(eventFolderName)
{
  // ctor
 
}

//____________________________________________________________________________
AliPHOSClusterizer::~AliPHOSClusterizer()
{
  // dtor
         
}


