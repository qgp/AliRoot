/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors:                                                       *
 *   Philippe Pillot <pillot@subatech.in2p3.fr>                           *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

// $Id$

///
///  @file   AliHLTMUONDigitReaderComponent.cxx
///  @author Philippe Pillot <pillot@subatech.in2p3.fr>
///  @date   24 October 2014
///  @brief  Implementation of digit reader source component for the dimuon HLT.
///

#include <cassert>

// ROOT
#include <TFile.h>
#include <TTree.h>

// HLT MUON
#include "AliHLTMUONDigitReaderComponent.h"
#include "AliHLTMUONConstants.h"

// MUON
#include "AliMUONVDigit.h"
#include "AliMUONVDigitStore.h"


ClassImp(AliHLTMUONDigitReaderComponent)


//_________________________________________________________________________________________________
AliHLTMUONDigitReaderComponent::AliHLTMUONDigitReaderComponent() :
AliHLTDataSource(),
fFile(0x0),
fTree(0x0),
fDigitStore(0x0)
{
  /// Default constructor
}

//_________________________________________________________________________________________________
AliHLTMUONDigitReaderComponent::~AliHLTMUONDigitReaderComponent()
{
  /// Default destructor
}

//_________________________________________________________________________________________________
void AliHLTMUONDigitReaderComponent::CleanMemory()
{
  /// delete allocated memory
  delete fDigitStore;
  fDigitStore = 0x0;
  delete fFile;
  fFile = 0x0;
  fTree = 0x0;
}

//_________________________________________________________________________________________________
const char* AliHLTMUONDigitReaderComponent::GetComponentID()
{
  /// Inherited from AliHLTComponent. Returns the component ID.
  return "MUONDigitReader";
}

//_________________________________________________________________________________________________
AliHLTComponentDataType AliHLTMUONDigitReaderComponent::GetOutputDataType()
{
  /// Inherited from AliHLTComponent. Returns the output data type.
  return AliHLTMUONConstants::DigitStoreDataType();
}

//_________________________________________________________________________________________________
void AliHLTMUONDigitReaderComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
  /// Inherited from AliHLTComponent. Returns an estimate of the expected output data size.
  constBase = sizeof(AliMUONVDigitStore) + 1024*1024;
  inputMultiplier = 0;
}

//_________________________________________________________________________________________________
AliHLTComponent* AliHLTMUONDigitReaderComponent::Spawn()
{
  /// Inherited from AliHLTComponent. Creates a new object instance.
  return new AliHLTMUONDigitReaderComponent;
}

//_________________________________________________________________________________________________
int AliHLTMUONDigitReaderComponent::DoInit(int argc, const char** argv)
{
  /// Inherited from AliHLTComponent.
  /// Parses the command line parameters and initialises the component.
  
  HLTInfo("Initialising dHLT digit reader component.");
  
  const char* fileName = NULL;
  
  for (int i = 0; i < argc; i++) {
    
    // file name
    if (strcmp(argv[i], "-datafile") == 0) {
      
      if (fileName != NULL) HLTWarning("file name was already specified. Will replace previous value given by -datafile.");
      
      if (argc <= i+1) {
        HLTError("The file name was not specified.");
        return -EINVAL;
      }
      
      fileName = argv[++i];
      
    }
    
  }
  
  // prepare reading of digits
  fFile = new TFile(fileName,"READ");
  if (!fFile) {
    HLTError("Cannot open file named %s.",fileName);
    return -ENOENT;
  }
  
  fTree = dynamic_cast<TTree*>(fFile->Get("TreeD"));
  if (!fTree) {
    HLTError("Cannot find digit tree \"TreeD\" in file named %s.",fileName);
    CleanMemory();
    return -EFTYPE;
  }
  
  fDigitStore = AliMUONVDigitStore::Create(*fTree);
  if (!fDigitStore) {
    HLTError("Cannot create digit store.");
    CleanMemory();
    return -EFTYPE;
  }
  
  if (!fDigitStore->Connect(*fTree)) {
    HLTError("Cannot connect digit store to the tree.");
    CleanMemory();
    return -EIO;
  }
  
  return 0;
}

//_________________________________________________________________________________________________
int AliHLTMUONDigitReaderComponent::DoDeinit()
{
  /// Inherited from AliHLTComponent. Performs a cleanup of the component.
  
  HLTInfo("Deinitialising dHLT digit reader component.");
  
  CleanMemory();
  
  return 0;
}

//_________________________________________________________________________________________________
int AliHLTMUONDigitReaderComponent::GetEvent(const AliHLTComponentEventData& evtData,
                                             AliHLTComponentTriggerData& /*trigData*/)
{
  /// Inherited from AliHLTDataSource. Processes the new event data.
  
  assert(fTree != 0x0 && fDigitStore != 0x0);
  
  // skip non physics events (e.g. SOR, EOR)
  if (!IsDataEvent()) return 0;
  
  UInt_t eventnumber = UInt_t(evtData.fEventID);
  if (eventnumber >= fTree->GetEntries()) return -EINVAL;
  
  // load digits
  fDigitStore->Clear();
  fTree->GetEntry(eventnumber);
  
  // register digits
  if (fDigitStore->GetSize() > 0)
    PushBack(fDigitStore, AliHLTMUONConstants::DigitStoreDataType(), 0);
  
  return 0;
}

