// $Id$
/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 *                                                                        *
 * Primary Authors: Artur Szostak <artursz@iafrica.com>                   *
 *                  for The ALICE HLT Project.                            *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/// @file   AliHLTTriggerDecision.cxx
/// @author Artur Szostak <artursz@iafrica.com>
/// @date   21 Nov 2008
/// @brief  Implementation of the AliHLTTriggerDecision class.
/// 
/// The trigger decision class stores the HLT decision from an AliHLTTrigger component.

#include "AliHLTTriggerDecision.h"
#include "Riostream.h"

ClassImp(AliHLTTriggerDecision)


AliHLTTriggerDecision::AliHLTTriggerDecision() :
  TObject(),
  fName(),
  fDescription(),
  fTriggerDomain()
{
  // Default constructor.
}


AliHLTTriggerDecision::AliHLTTriggerDecision(bool result, const char* name) :
  TObject(),
  fName(name),
  fDescription(),
  fTriggerDomain()
{
  // Constructor specifying the name and result of the trigger decision.
  
  Result(result);
}


AliHLTTriggerDecision::AliHLTTriggerDecision(
    bool result, const char* name,
    const AliHLTTriggerDomain& triggerDomain,
    const char* description
  ) :
  TObject(),
  fName(name),
  fDescription(description),
  fTriggerDomain(triggerDomain)
{
  // Constructor specifying all information fields.
  
  Result(result);
}


AliHLTTriggerDecision::~AliHLTTriggerDecision()
{
  // Default destructor.
}


void AliHLTTriggerDecision::ReadoutList(const AliHLTReadoutList& value)
{
  // Replaces the readout list in the trigger domain with the new value.
  
  AliHLTReadoutList fullReadout = ~ AliHLTReadoutList(0x0);
  fTriggerDomain.Remove(fullReadout);
  fTriggerDomain.Add(value);
}


void AliHLTTriggerDecision::Print(Option_t* option) const
{
  // Prints the contents of the trigger decision.
  
  cout << "Trigger (" << fName.Data() << ") result = " << Result() << endl;
  TString opt(option);
  if (opt.Contains("short")) return;
  cout << "Description = \"" << fDescription.Data() << "\"" << endl;
  fTriggerDomain.Print();
}

void AliHLTTriggerDecision::Copy(TObject &object) const
{
  // copy this to the specified object

  AliHLTTriggerDecision* pDecision=dynamic_cast<AliHLTTriggerDecision*>(&object);
  if (pDecision) {
    // copy members if target is a AliHLTTriggerDecision
    *pDecision=*this;
  }

  // copy the base class
  TObject::Copy(object);
}

TObject *AliHLTTriggerDecision::Clone(const char */*newname*/) const
{
  // create a new clone, classname is ignored

  return new AliHLTTriggerDecision(*this);
}

Option_t *AliHLTTriggerDecision::GetOption() const
{
  // Return the result of the trigger.
  // "0" or "1"
  if (Result()) return "1";
  return "0";
}
