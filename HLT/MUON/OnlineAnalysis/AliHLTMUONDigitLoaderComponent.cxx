/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * All rights reserved.                                                   *
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
///  @file   AliHLTMUONDigitLoaderComponent.cxx
///  @author Hugo Pereira Da Costa <pereira@hep.saclay.cea.fr>
///  @date   March 08 2016
///  @brief  load digits from raw data
///

// HLT MUON
#include "AliHLTMUONDigitLoaderComponent.h"
#include "AliHLTMUONConstants.h"
#include "AliHLTMUONUtils.h"
#include "AliHLTCDHWrapper.h"

#include "AliCDBManager.h"
#include "AliCodeTimer.h"
#include "AliMpConstants.h"
#include "AliMpDDLStore.h"
#include "AliMpDEIterator.h"
#include "AliMpSegmentation.h"
#include "AliMpVSegmentation.h"
#include "AliMUONCDB.h"
#include "AliMUONConstants.h"
#include "AliMUONVDigit.h"

ClassImp(AliHLTMUONDigitLoaderComponent)

//_________________________________________________________________________________________________
AliHLTMUONDigitLoaderComponent::AliHLTMUONDigitLoaderComponent() :
    fBadEvent(kFALSE),
    fDigitblock(0x0)
{
  /// Default constructor
  fRawDecoder.GetHandler().SetParent(this);
}

//_________________________________________________________________________________________________
AliHLTMUONDigitLoaderComponent::~AliHLTMUONDigitLoaderComponent()
{
  /// Default destructor
}

//_________________________________________________________________________________________________
const char* AliHLTMUONDigitLoaderComponent::GetComponentID()
{
  /// Inherited from AliHLTComponent. Returns the component ID.
  return "MUONDigitLoader";
}

//_________________________________________________________________________________________________
void AliHLTMUONDigitLoaderComponent::GetInputDataTypes(AliHLTComponentDataTypeList& list)
{
  /// Inherited from AliHLTComponent. Returns the list of expected input data types.
  list.clear();
  list.push_back(AliHLTMUONConstants::DDLRawDataType());
}

//_________________________________________________________________________________________________
AliHLTComponentDataType AliHLTMUONDigitLoaderComponent::GetOutputDataType()
{
  /// Inherited from AliHLTComponent. Returns the output data type.
  return AliHLTMUONConstants::DigitBlockDataType();
}

//_________________________________________________________________________________________________
void AliHLTMUONDigitLoaderComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
  /// Inherited from AliHLTComponent. Returns an estimate of the expected output data size.
  constBase = sizeof(AliHLTMUONDigitsBlockStruct) + 1064008*sizeof(AliHLTMUONDigitStruct);
  inputMultiplier = 0;
}

//_________________________________________________________________________________________________
AliHLTComponent* AliHLTMUONDigitLoaderComponent::Spawn()
{
  /// Inherited from AliHLTComponent. Creates a new object instance.
  return new AliHLTMUONDigitLoaderComponent();
}

//_________________________________________________________________________________________________
int AliHLTMUONDigitLoaderComponent::DoInit(int argc, const char** argv)
{
  /// Inherited from AliHLTComponent.
  /// Parses the command line parameters and initialises the component.

  HLTInfo("Initialising dHLT digit loader component component.");

  AliCodeTimer::Instance()->Reset();

  const char* cdbPath = NULL;
  Int_t run = -1;

  for (int i = 0; i < argc; i++)
  {

    // get the CDB path
    if (strcmp(argv[i], "-cdbpath") == 0) {

      if (cdbPath != NULL) HLTWarning("CDB path was already specified. Will replace previous value given by -cdbpath.");

      if (argc <= i+1) {
        HLTError("The CDB path was not specified.");
        return -EINVAL;
      }

      cdbPath = argv[i+1];
      i++;
      continue;

    }

    // get the run number
    if (strcmp(argv[i], "-run") == 0) {

      if (run != -1) HLTWarning("Run number was already specified. Will replace previous value given by -run.");
      if (argc <= i+1)
      {
        HLTError("The run number was not specified.");
        return -EINVAL;
      }

      char* cpErr = NULL;
      run = Int_t( strtol(argv[i+1], &cpErr, 0) );
      if (cpErr == NULL or *cpErr != '\0' or run < 0)
      {
        HLTError("Cannot convert '%s' to a valid run number. Expected a positive integer value.", argv[i+1]);
        return -EINVAL;
      }

      i++;
      continue;

    }

  }

  AliCDBManager* cdbManager = AliCDBManager::Instance();
  if (cdbManager == NULL)
  {
    HLTError("CDB manager instance does not exist.");
    return -EIO;
  }

  // Set the CDB path
  if (cdbPath == NULL)
  {

    HLTError("The CDB path was not set.");
    return -EINVAL;

  } else {

    cdbManager->SetDefaultStorage(cdbPath);
    if( !cdbManager->IsDefaultStorageSet() )
    {

      HLTError("Invalid CDB path: %s.", cdbPath);
      return -EINVAL;

    }

  }

  // Set the run number
  if (run == -1)
  {

    HLTError("The run number was not set.");
    return -EINVAL;

  } else {

    cdbManager->SetRun(run);

  }

  // Load the MUON mapping from CDB and create the internal mapping
  // MUON mapping also needed for raw data decoding
  if( !AliMUONCDB::LoadMapping() ) return -EIO;
  CreateMapping();

  AliCodeTimer::Instance()->Reset();

  return 0;

}

//_________________________________________________________________________________________________
int AliHLTMUONDigitLoaderComponent::DoDeinit()
{
  /// Inherited from AliHLTComponent. Performs a cleanup of the component.
  HLTInfo("Deinitialising dHLT digit loader component.");
  AliCodeTimer::Instance()->Print();

  return 0;
}

//_________________________________________________________________________________________________
int AliHLTMUONDigitLoaderComponent::DoEvent(
  const AliHLTComponentEventData& /*evtData*/,
  const AliHLTComponentBlockData* /*blocks*/,
  AliHLTComponentTriggerData& /*trigData*/,
  AliHLTUInt8_t* outputPtr,
  AliHLTUInt32_t& size,
  AliHLTComponentBlockDataList& /*outputBlocks*/)
{

  /// Inherited from AliHLTProcessor. Processes the new event data.
  AliCodeTimerAutoGeneral("",0);

  // skip non physics events (e.g. SOR, EOR)
  if (!IsDataEvent()) return 0;

  fBadEvent = kFALSE;

  // create digit block
  int status = CreateDigitBlock( outputPtr, size, fDigitblock );
  if( status < 0 ) return status;

  // loop over blocks of the data type "DDL_RAW"
  AliCodeTimerStartGeneral("LoadDigitsFromRaw");
  const AliHLTComponentBlockData* pBlock = GetFirstInputBlock(AliHLTMUONConstants::DDLRawDataType());
  while (pBlock)
  {

    // get the pointer to the begining and the size of the raw data block (after the CDH)
    AliHLTCDHWrapper cdh(pBlock->fPtr);
    AliHLTUInt32_t headerSize = cdh.GetHeaderSize();
    AliHLTUInt32_t totalDDLSize = pBlock->fSize;
    AliHLTUInt32_t ddlRawDataSize = totalDDLSize - headerSize;
    AliHLTUInt32_t *buffer = reinterpret_cast<AliHLTUInt32_t*>(pBlock->fPtr) + headerSize/sizeof(AliHLTUInt32_t);

    // decode the block to get the fired pads
    fRawDecoder.Decode(buffer,ddlRawDataSize);
    if( fBadEvent )
    {

      HLTError("Error found while decoding the raw data block.");
      #if __APPLE__
      return -EFTYPE;
      #else
      return -ENODATA;
      #endif
    }

    pBlock = GetNextInputBlock();

  }

  // push data
  // TODO: could check here whether at least one digit was added
  const AliHLTUInt32_t bytesUsed = fDigitblock->BytesUsed();
  status = PushBack( outputPtr, bytesUsed, AliHLTMUONConstants::DigitBlockDataType(), 0);

  // cleanup
  delete fDigitblock;
  fDigitblock = 0x0;

  AliCodeTimerStopGeneral("LoadDigitsFromRaw");
  return status;

}


//_________________________________________________________________________________________________
void AliHLTMUONDigitLoaderComponent::CreateMapping()
{
  fDEPlaneType.Clear();

  // loop over chambers
  for( Int_t iCh = 0; iCh < AliMUONConstants::NTrackingCh(); ++iCh )
  {

    // loop over detector element
    AliMpDEIterator iterator;
    for( iterator.First(iCh); !iterator.IsDone(); iterator.Next() )
    {

      // get detector element
      const Int_t deId( iterator.CurrentDEId() );

      // get segmentation for cathode 0
      const AliMpVSegmentation* segmentation = AliMpSegmentation::Instance()->GetMpSegmentation( deId, AliMp::kCath0 );

      // store corresponding plane type
      fDEPlaneType.Add( deId, segmentation->PlaneType() );

    }

  }

}

//_________________________________________________________________________________________________
int AliHLTMUONDigitLoaderComponent::CreateDigitBlock(
  AliHLTUInt8_t *outputPtr,
  AliHLTUInt32_t size,
  AliHLTMUONDigitsBlockWriter *&digitblock ) const
{

  /// Create a digits data block.
  digitblock = new AliHLTMUONDigitsBlockWriter(outputPtr, size);
  if (!digitblock->InitCommonHeader())
  {

    HLTError("The buffer is too small to store a new digit block.");
    return -ENOBUFS;

  }

  return 0;

}

//_________________________________________________________________________________________________
void AliHLTMUONDigitLoaderComponent::RawDecoderHandler::OnData(UInt_t data, bool /*parityError*/)
{

  // decode the raw data word
  UShort_t manuId;
  UChar_t manuChannel;
  UShort_t adc;
  UnpackADC(data, manuId, manuChannel, adc);

  if (adc <= 0) return;

  // get plane index
  const AliMp::PlaneType planeType = (manuId & AliMpConstants::ManuMask(AliMp::kNonBendingPlane)) ? AliMp::kNonBendingPlane : AliMp::kBendingPlane;

  // get cathode index
  const Int_t iCath = ( planeType == fParent->fDEPlaneType.GetValue( fDetElemId ) ) ? 0:1;

  // get pad unique id
  const UInt_t padId = AliMUONVDigit::BuildUniqueID(fDetElemId, manuId, manuChannel, iCath );

  // add new digit
  AliHLTMUONDigitStruct* digit = fParent->fDigitblock->AddEntry();
  digit->fId = padId;
  digit->fIndex = 0;
  digit->fADC = adc;

  std::cerr << *digit << endl;

}

//_________________________________________________________________________________________________
void AliHLTMUONDigitLoaderComponent::RawDecoderHandler::OnNewBusPatch(const AliMUONBusPatchHeaderStruct* header, const void* /*data*/)
{
  // get the detection element ID containing this bus patch
  fDetElemId = AliMpDDLStore::Instance()->GetDEfromBus(header->fBusPatchId);
}

//_________________________________________________________________________________________________
void AliHLTMUONDigitLoaderComponent::RawDecoderHandler::OnError(ErrorCode /*error*/, const void* /*location*/)
{
  fParent->fBadEvent = kTRUE;
}

