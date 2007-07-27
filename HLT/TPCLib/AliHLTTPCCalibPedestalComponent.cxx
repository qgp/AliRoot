/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 *                                                                        *
 * Primary Authors: Jochen Thaeder <thaeder@kip.uni-heidelberg.de>        *
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

/** @file   AliHLTTPCCalibPedestalComponent.cxx
    @author Jochen Thaeder
    @date   
    @brief  A pedestal calibration component for the TPC.
*/

#if __GNUC__>= 3
using namespace std;
#endif

#include "AliHLTTPCLogging.h"
#include "AliHLTTPCTransform.h"

#include "AliHLTTPCCalibPedestalComponent.h"

#include "AliRawDataHeader.h"
#include "AliRawReaderMemory.h"
#include "AliTPCRawStream.h"

#include "AliTPCCalibPedestal.h"

#include <stdlib.h>
#include <errno.h>
#include "TString.h"

// this is a global object used for automatic component registration, do not use this
AliHLTTPCCalibPedestalComponent gAliHLTTPCCalibPedestalComponent;

ClassImp(AliHLTTPCCalibPedestalComponent)

AliHLTTPCCalibPedestalComponent::AliHLTTPCCalibPedestalComponent()
  :
  fRawReader(NULL),
  fRawStream(NULL),
  fCalibPedestal(NULL),
  fRCUFormat(kFALSE),
  fMinPatch(5),
  fMaxPatch(0),
  fSpecification(0) ,
  fEnableAnalysis(kFALSE) {
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTTPCCalibPedestalComponent::AliHLTTPCCalibPedestalComponent(const AliHLTTPCCalibPedestalComponent&)
  :
  fRawReader(NULL),
  fRawStream(NULL),
  fCalibPedestal(NULL),
  fRCUFormat(kFALSE),
  fMinPatch(5),
  fMaxPatch(0),
  fSpecification(0),
  fEnableAnalysis(kFALSE) {
  // see header file for class documentation

  HLTFatal("copy constructor untested");
}

AliHLTTPCCalibPedestalComponent& AliHLTTPCCalibPedestalComponent::operator=(const AliHLTTPCCalibPedestalComponent&) { 
  // see header file for class documentation

  HLTFatal("assignment operator untested");
  return *this;
}	

AliHLTTPCCalibPedestalComponent::~AliHLTTPCCalibPedestalComponent() {
  // see header file for class documentation
}

// Public functions to implement AliHLTComponent's interface.
// These functions are required for the registration process

const char* AliHLTTPCCalibPedestalComponent::GetComponentID() {
  // see header file for class documentation

  return "TPCCalibPedestal";
}

void AliHLTTPCCalibPedestalComponent::GetInputDataTypes( vector<AliHLTComponentDataType>& list) {
  // see header file for class documentation

  list.clear(); 
  list.push_back( AliHLTTPCDefinitions::fgkDDLPackedRawDataType );
}

AliHLTComponentDataType AliHLTTPCCalibPedestalComponent::GetOutputDataType() {
  // see header file for class documentation

  return AliHLTTPCDefinitions::fgkCalibPedestalDataType;
}

void AliHLTTPCCalibPedestalComponent::GetOutputDataSize( unsigned long& constBase, double& inputMultiplier ) {
  // see header file for class documentation

  // XXX TODO: Find more realistic values.  
  constBase = 0;
  inputMultiplier = (2.0);
}

AliHLTComponent* AliHLTTPCCalibPedestalComponent::Spawn() {
  // see header file for class documentation

  return new AliHLTTPCCalibPedestalComponent();
}  


Int_t AliHLTTPCCalibPedestalComponent::ScanArgument( Int_t argc, const char** argv ) {
  // see header file for class documentation

  Int_t iResult = 0;
  TString argument = "";
  TString parameter = "";

  if ( !argc ) 
    return -EINVAL;

  argument = argv[iResult];
  
  if ( argument.IsNull() ) 
    return -EINVAL;

  // -rcuformat
  if ( argument.CompareTo("-rcuformat") == 0 ) {

    if ( ++iResult >= argc  ) {
      iResult = -EPROTO;
    }
    else {
      parameter = argv[1];
      if ( parameter.CompareTo("old") == 0 ) {
	fRCUFormat = kTRUE;
        HLTInfo( "RCU Format is set to old." );
      }
      else if ( parameter.CompareTo("new") == 0 ) {
	fRCUFormat = kFALSE;
        HLTInfo( "RCU Format is set to new." );
      }
      else {
	HLTError( "Cannot convert rcu format specifier '%s'.", argv[1] );
	iResult = -EPROTO;
      }
    } 
  }
  else if ( argument.CompareTo("-enableanalysis") == 0 ) {
    HLTInfo( "Analysis before shipping data to FXS enabled." );
    fEnableAnalysis = kTRUE;
  }
  else {
    iResult = -EINVAL;
  }
      
  return iResult;
}

Int_t AliHLTTPCCalibPedestalComponent::InitCalibration() {
  // see header file for class documentation
    
  // ** Create pedestal calibration
  if ( fCalibPedestal )
    return EINPROGRESS;
  
  fCalibPedestal = new AliTPCCalibPedestal();

  // **  Create AliRoot Memory Reader
  if (fRawReader)
    return EINPROGRESS;

#if defined(HAVE_ALIRAWDATA) && defined(HAVE_ALITPCRAWSTREAM_H) 
  fRawReader = new AliRawReaderMemory();
#else
  HLTFatal("AliRawReader  not available - check your build");
  return -ENODEV;
#endif

  return 0;
}

Int_t AliHLTTPCCalibPedestalComponent::DeinitCalibration() {
  // see header file for class documentation

  if ( fRawReader )
    delete fRawReader;
  fRawReader = NULL;

  if ( fCalibPedestal )
    delete fCalibPedestal;
  fCalibPedestal = NULL;

  return 0;
}

/*
 * --- setter for rcuformat need in AliTPCCalibPedestal class
 */
Int_t AliHLTTPCCalibPedestalComponent::ProcessCalibration( const AliHLTComponentEventData& evtData, AliHLTComponentTriggerData& trigData ) {
  // see header file for class documentation
  
  const AliHLTComponentBlockData* iter = NULL;

  AliHLTUInt8_t slice, patch;
  Int_t DDLid = 0;
    
  // ** Loop over all input blocks and specify which data format should be read - only select Raw Data
  iter = GetFirstInputBlock( AliHLTTPCDefinitions::fgkDDLPackedRawDataType );
  
  while ( iter != NULL ) {
    
    // ** Print Debug output which data format was received
    char tmp1[14], tmp2[14];
    DataType2Text( iter->fDataType, tmp1 );
    DataType2Text( AliHLTTPCDefinitions::fgkDDLPackedRawDataType, tmp2 );

    HLTDebug ( "Event received - Event 0x%08LX (%Lu) received datatype: %s - required datatype: %s", 
	       evtData.fEventID, evtData.fEventID, tmp1, tmp2 );

    // ** Get DDL ID in order to tell the memory reader which slice/patch to use
    slice = AliHLTTPCDefinitions::GetMinSliceNr( *iter );
    patch = AliHLTTPCDefinitions::GetMinPatchNr( *iter );

    if (patch < 2) DDLid = 768 + (2 * slice) + patch;
    else DDLid = 838 + (4*slice) + patch;

    HLTDebug ( "Input Raw Data - Slice/Patch: %d/%d - EquipmentID : %d.", slice, patch, DDLid );

    // ** Get min and max patch, used for output specification
    if ( patch < fMinPatch ) fMinPatch =  patch;
    if ( patch > fMaxPatch ) fMaxPatch =  patch;

    // ** Init TPCRawStream
    fRawReader->SetMemory( reinterpret_cast<UChar_t*>( iter->fPtr ), iter->fSize );
    fRawReader->SetEquipmentID(DDLid);

    fRawStream = new AliTPCRawStream( fRawReader );
    fRawStream->SetOldRCUFormat( fRCUFormat );

    // ** Process actual Pedestal Calibration - Fill histograms
    fCalibPedestal->ProcessEvent( fRawStream );
  
    // ** Delete TPCRawStream
    if ( fRawStream )
      delete fRawStream;
    fRawStream = NULL;    

    // ** Get next input block, with the same specification as defined in GetFirstInputBlock()
    iter = GetNextInputBlock();

  } //  while ( iter != NULL ) {
    
  // ** Get output specification
  fSpecification = AliHLTTPCDefinitions::EncodeDataSpecification( slice, slice, fMinPatch, fMaxPatch );

  // ** PushBack data to shared memory ... 
  PushBack( (TObject*) fCalibPedestal, AliHLTTPCDefinitions::fgkCalibPedestalDataType, fSpecification);
  
  return 0;
} // Int_t AliHLTTPCCalibPedestalComponent::ProcessCalibration( const AliHLTComponentEventData& evtData, AliHLTComponentTriggerData& trigData ) {


Int_t AliHLTTPCCalibPedestalComponent::ShipDataToFXS( const AliHLTComponentEventData& evtData, AliHLTComponentTriggerData& trigData ) {
  // see header file for class documentation
    
  if ( fEnableAnalysis )
    fCalibPedestal->Analyse();
  
  // ** PushBack data to FXS ...
  PushToFXS( (TObject*) fCalibPedestal, "TPC", "Pedestal" ) ;
  
  return 0;
} // Int_t AliHLTTPCCalibPedestalComponent::ShipDataToFXS( const AliHLTComponentEventData& evtData, AliHLTComponentTriggerData& trigData ) {


