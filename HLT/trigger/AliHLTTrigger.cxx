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

/// @file   AliHLTTrigger.h
/// @author Artur Szostak <artursz@iafrica.com>
/// @date   12 Aug 2008
/// @brief  Implementation of the AliHLTTrigger base component class.
///
/// The AliHLTTrigger class is the base class from which all HLT trigger components
/// should be derived.

#include "AliHLTTrigger.h"
#include "AliHLTTriggerDecision.h"
#include "AliHLTReadoutList.h"
#include "AliHLTTriggerDomain.h"
#include "AliHLTDomainEntry.h"

ClassImp(AliHLTTrigger)


AliHLTTrigger::AliHLTTrigger() :
	AliHLTProcessor(),
	fEventData(NULL),
	fTriggerData(NULL),
	fDecisionMade(false),
	fClearInfo(true),
	fTriggerEventResult(0),
	fDescription(),
	fReadoutList(),
	fTriggerDomain()
{
  // Default constructor sets pointers to NULL.
}


AliHLTTrigger::~AliHLTTrigger()
{
  // Default destructor.
}


void AliHLTTrigger::GetInputDataTypes(AliHLTComponentDataTypeList& list) const
{
  // Returns the kAliHLTAnyDataType type as input.
  list.push_back(kAliHLTAnyDataType);
}


void AliHLTTrigger::GetOutputDataTypes(AliHLTComponentDataTypeList& list) const
{
  // Returns the kAliHLTDataTypeTriggerDecision type as output.
  list.push_back(kAliHLTDataTypeTriggerDecision);
}


void AliHLTTrigger::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
  // Returns output data size estimate.
  // See header file for more details.

  // Matthias 2009-07-03 this is presumably to small as the streamed object might be
  // bigger. This is actually the case in root v5-24-00
  // Just take 2x the size of the object
  constBase = 2*sizeof(AliHLTTriggerDecision);
  inputMultiplier = 1;
}


int AliHLTTrigger::DoEvent(const AliHLTComponentEventData& evtData, AliHLTComponentTriggerData& trigData)
{
  // Sets the pointers to the evtData and trigData, then calls the DoTrigger to
  // execute the actual trigger algorithm.

  fEventData = &evtData;
  fTriggerData = &trigData;
  fDecisionMade = false;
  fTriggerEventResult = 0;
  // Reset the description, readout list and trigger domain used by TriggerEvent
  // if requested to do so.
  if (fClearInfo)
  {
    fDescription = "";
    fReadoutList.Clear();
    fTriggerDomain.Clear();
  }
  
  int result = DoTrigger();
  if (result != 0) return result;
  
  // Fill in a default decision of false if none was made.
  if (not fDecisionMade)
  {
    TriggerEvent(false);
  }

  // Cleanup
  fEventData = NULL;
  fTriggerData = NULL;
  return fTriggerEventResult;
}


int AliHLTTrigger::TriggerEvent(bool value)
{
  // Sets the trigger decision for the current event.
  // See header file for more details.
  
  if (fTriggerEventResult != 0) return fTriggerEventResult;  // Do not do anything if a previous call failed.
  AliHLTTriggerDecision triggerResult(value, GetTriggerName(), fTriggerDomain, fDescription);
  // Append the readout list if it contains anything.
  triggerResult.TriggerDomain().Add(fReadoutList);
  return TriggerEvent(&triggerResult, kAliHLTDataTypeTObject|kAliHLTDataOriginOut);
}


int AliHLTTrigger::TriggerEvent(
    AliHLTTriggerDecision* result, const AliHLTComponentDataType& type,
    AliHLTUInt32_t spec
  )
{
  // Sets a custom trigger decision for the current event.
  // See header file for more details.
  
  if (fTriggerEventResult != 0) return fTriggerEventResult;  // Do not do anything if a previous call failed.
  fTriggerEventResult = PushBack(result, type, spec);
  if (fTriggerEventResult == 0) {
    fTriggerEventResult = PushBack(result->ReadoutList().Buffer(), result->ReadoutList().BufferSize(), kAliHLTDataTypeReadoutList);
  }
  
  if (fTriggerEventResult == 0) fDecisionMade = true;
  return fTriggerEventResult;
}


void AliHLTTrigger::GetInputDataTypes(AliHLTComponentDataTypeList& list)
{
  // Calls the const version of this method.
  
  // Assign to const temporary variable to make sure we call the constant version
  // of the GetOutputDataTypes method.
  const AliHLTTrigger* t = this;
  t->GetInputDataTypes(list);
}


int AliHLTTrigger::GetOutputDataTypes(AliHLTComponentDataTypeList& list)
{
  // Calls the const version of this method.
  
  // Assign to const temporary variable to make sure we call the constant version
  // of the GetOutputDataTypes method.
  const AliHLTTrigger* t = this;
  t->GetOutputDataTypes(list);
  list.push_back(kAliHLTDataTypeReadoutList);
  return list.size();
}

int AliHLTTrigger::CreateEventDoneReadoutFilter(const AliHLTTriggerDomain& domain, unsigned type)
{
  // add a readout filter to the EventDoneData
  int iResult=0;
  unsigned nofEntries=domain.GetNofEntries();
  // we need:
  //   1 word eventually for the monitor event command
  //   1 word for the readout filter command
  //   1 word for the readout filter size
  // 4*n words for the filter list
  if ((iResult=ReserveEventDoneData((nofEntries*4 + 3) * sizeof(AliHLTUInt32_t)))<0) return iResult;
  AliHLTUInt32_t eddbuffer[4];
  if (type==4) {
    // in the case of the monitoring filter we also add the monitor event command
    eddbuffer[0]=5;
    if ((iResult=PushEventDoneData(eddbuffer[0]))<0) return iResult;
  }

  // now the readout list command and the block count
  eddbuffer[0]=type;
  if ((iResult=PushEventDoneData(eddbuffer[0]))<0) return iResult;

  // find the valid entries
  unsigned block=0;
  vector<const AliHLTDomainEntry*> entries;
  for (block=0; block<nofEntries; block++) {
    // skip all DAQ readout entries as they are handled by the readout list
    if (domain[block]==AliHLTDomainEntry(kAliHLTDataTypeDAQRDOUT)) continue;
    if (domain[block].Exclusive()) {
      HLTWarning("exclusive trigger domain entries are currently not handled, skipping entry %s", domain[block].AsString().Data());
      continue;
    }
    entries.push_back(&(domain[block]));
  }
  eddbuffer[0]=entries.size();
  if ((iResult=PushEventDoneData(eddbuffer[0]))<0) return iResult;

  for (vector<const AliHLTDomainEntry*>::iterator entry=entries.begin();
       entry!=entries.end(); entry++) {
    (*entry)->AsBinary(eddbuffer);
    for (int n=0; n<4; n++)
      if ((iResult=PushEventDoneData(eddbuffer[n]))<0) return iResult;
  }
  return iResult;
}
