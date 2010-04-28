// $Id$
//**************************************************************************
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//*                                                                        *
//* Primary Authors: Svein Lindal <svein.lindal@gmail.com>                 *
//*                  for The ALICE HLT Project.                            *
//*                                                                        *
//* Permission to use, copy, modify and distribute this software and its   *
//* documentation strictly for non-commercial purposes is hereby granted   *
//* without fee, provided that the above copyright notice appears in all   *
//* copies and that both the copyright notice and this permission notice   *
//* appear in the supporting documentation. The authors make no claims     *
//* about the suitability of this software for any purpose. It is          *
//* provided "as is" without express or implied warranty.                  *
//**************************************************************************

/// @file   AliHLTTriggerEmcalClusterEnergy.cxx
/// @author Svein Lindal <slindal@fys.uio.no>
/// @date   2009-08-17
/// @brief  HLT energy threshold trigger for EMCAL
///      

// see header file for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt


#include "AliHLTTriggerEmcalClusterEnergy.h"
#include "AliESDEvent.h"
#include "AliESDCaloCluster.h"
#include "AliHLTTriggerDecision.h"
#include "AliHLTDomainEntry.h"


AliHLTTriggerEmcalClusterEnergy gEmcalClusterEnergyTrigger;


/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTTriggerEmcalClusterEnergy)

AliHLTTriggerEmcalClusterEnergy::AliHLTTriggerEmcalClusterEnergy()  : 
AliHLTTriggerCaloClusterEnergy("EMCAL")
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlts

  fgkOCDBEntry = "HLT/ConfigHLT/EmcalClusterEnergyTrigger";
  fgkInputDataType = kAliHLTDataTypeCaloCluster | kAliHLTDataOriginEMCAL;
}

AliHLTTriggerEmcalClusterEnergy::~AliHLTTriggerEmcalClusterEnergy() {
  // see header file for class documentation
}

const char* AliHLTTriggerEmcalClusterEnergy::GetTriggerName() const {
  // see header file for class documentation
  return "EmcalClusterEnergyTrigger";
}

AliHLTComponent* AliHLTTriggerEmcalClusterEnergy::Spawn() {
  // see header file for class documentation
  return new AliHLTTriggerEmcalClusterEnergy;
}

Int_t AliHLTTriggerEmcalClusterEnergy::GetClustersFromEsd( const AliESDEvent * esd, TRefArray * clustersRefs ){
  return esd->GetEMCALClusters(clustersRefs);
}
