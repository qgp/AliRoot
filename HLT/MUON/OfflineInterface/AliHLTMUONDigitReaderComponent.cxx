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
#include <TIterator.h>

// HLT MUON
#include "AliHLTMUONDigitReaderComponent.h"
#include "AliHLTMUONConstants.h"
#include "AliHLTMUONDigitsBlockStruct.h"

// MUON
#include "AliMUONVDigit.h"
#include "AliMUONVDigitStore.h"


ClassImp(AliHLTMUONDigitReaderComponent)


//_________________________________________________________________________________________________
AliHLTMUONDigitReaderComponent::AliHLTMUONDigitReaderComponent() :
AliHLTDataSource(),
fFile(0x0),
fTree(0x0),
fDigitStore(0x0),
fPerDE(kFALSE)
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
  return AliHLTMUONConstants::DigitBlockDataType();
}

//_________________________________________________________________________________________________
void AliHLTMUONDigitReaderComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
  /// Inherited from AliHLTComponent. Returns an estimate of the expected output data size.
  if (fPerDE) constBase = 156*sizeof(AliHLTMUONDigitsBlockStruct) + 1064008*sizeof(AliHLTMUONDigitStruct);
  else constBase = sizeof(AliHLTMUONDigitsBlockStruct) + 1064008*sizeof(AliHLTMUONDigitStruct);
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
    
    // split per DE
    if (strcmp(argv[i], "-perDE") == 0) fPerDE = kTRUE;
    
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
                                             AliHLTComponentTriggerData& /*trigData*/,
                                             AliHLTUInt8_t* outputPtr, AliHLTUInt32_t& size,
                                             AliHLTComponentBlockDataList& /*outputBlocks*/)
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
  if (fDigitStore->GetSize() < 1) return 0;
  
  // register digits with positive charge
  int status(0);
  TIterator *digits(0x0);
  AliMUONVDigit *digit(0x0);
  AliHLTMUONDigitsBlockWriter *digitblock(0x0);
  if (fPerDE) { // one block per dE
    
    digits = fDigitStore->CreateTrackerIterator();
    while ((digit = static_cast<AliMUONVDigit*>(digits->Next())) && digit->Charge() <= 0) continue;
    Int_t currentDE = digit ? digit->DetElemId() : -1;
    AliHLTUInt32_t totalBytesUsed(0);
    while (digit) {
      
      // create a digits data block for the new DE
      if (!digitblock)
        if ((status = CreateDigitBlock(outputPtr+totalBytesUsed, size-totalBytesUsed, digitblock)) < 0) break;
      
      // add the digit to the current block
      if ((status = AddDigit(*digit, *digitblock)) < 0) break;
      
      // get the next digit and register the current block if we are done with this DE
      while ((digit = static_cast<AliMUONVDigit*>(digits->Next())) && digit->Charge() <= 0) continue;
      if (!digit || digit->DetElemId() != currentDE) {
        AliHLTUInt32_t bytesUsed(digitblock->BytesUsed());
        if ((status = PushBack(outputPtr+totalBytesUsed, bytesUsed, AliHLTMUONConstants::DigitBlockDataType(), currentDE)) < 0) break;
        totalBytesUsed += bytesUsed;
        currentDE = digit ? digit->DetElemId() : -1;
        delete digitblock;
        digitblock = 0x0;
      }
      
    }
    
  } else { // all in the same block
    
    // create a digits data block
    status = CreateDigitBlock(outputPtr, size, digitblock);
    
    if (status >= 0) {
      
      digits = fDigitStore->CreateIterator();
      while ((digit = static_cast<AliMUONVDigit*>(digits->Next()))) {
        
        if (digit->Charge() <= 0) continue;
        
        // add the digit to the block
        if ((status = AddDigit(*digit, *digitblock)) < 0) break;
        
      }
      
      // register the block if not empty
      if (status >= 0 && digitblock->Nentries() > 0) {
        AliHLTUInt32_t bytesUsed(digitblock->BytesUsed());
        status = PushBack(outputPtr, bytesUsed, AliHLTMUONConstants::DigitBlockDataType(), 0);
      }
      
    }
    
  }
  delete digitblock;
  delete digits;
  
  return status;
}

//_________________________________________________________________________________________________
int AliHLTMUONDigitReaderComponent::CreateDigitBlock(AliHLTUInt8_t *outputPtr, AliHLTUInt32_t size,
                                                     AliHLTMUONDigitsBlockWriter *&digitblock)
{
  /// Create a digits data block.
  
  digitblock = new AliHLTMUONDigitsBlockWriter(outputPtr, size);
  if (!digitblock->InitCommonHeader()) {
    HLTError("The buffer is too small to store a new digit block.");
    return -ENOBUFS;
  }
  
  return 0;
}

//_________________________________________________________________________________________________
int AliHLTMUONDigitReaderComponent::AddDigit(AliMUONVDigit &digit, AliHLTMUONDigitsBlockWriter &digitblock)
{
  /// Add a digit to the block.
  
  AliHLTMUONDigitStruct *digit2 = digitblock.AddEntry();
  if (!digit2) {
    HLTError("The buffer is too small to store a new digit.");
    return -ENOBUFS;
  }
  digit2->fId = digit.GetUniqueID();
  digit2->fIndex = 0;
  digit2->fADC = static_cast<AliHLTUInt16_t>(digit.ADC());
  
  return 0;
}

