// $Id$

/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Authors: Matthias Richter <Matthias.Richter@ift.uib.no>                *
 *          Timm Steinbeck <timm@kip.uni-heidelberg.de>                   *
 *          for The ALICE Off-line Project.                               *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/** @file   AliHLTTRDCalibrationComponent.cxx
    @author Timm Steinbeck, Matthias Richter
    @date   
    @brief  A TRDCalibration processing component for the HLT. */

#if __GNUC__ >= 3
using namespace std;
#endif

#include "AliHLTTRDCalibra.h"
#include "AliHLTTRDCalibrationComponent.h"
#include "AliHLTTRDDefinitions.h"

#include "AliCDBManager.h"
#include "AliTRDtriggerHLT.h"
#include "AliTRDtrigParam.h"
#include "AliRawReaderMemory.h"

#include "TTree.h"
#include "TBranch.h"

#include <cstdlib>
#include <cerrno>
#include <string>

// this is a global object used for automatic component registration, do not use this
AliHLTTRDCalibrationComponent gAliHLTTRDCalibrationComponent;

ClassImp(AliHLTTRDCalibrationComponent)
    
AliHLTTRDCalibrationComponent::AliHLTTRDCalibrationComponent()
{
  fMCMtrigger = 0;

  fOutputPercentage = 100; // By default we copy to the output exactly what we got as input

  fTriggerParDebugLevel = 0;
  fLTUpTcut = 2.3; // GeV/c
  fBField = 0.5; //TESLA


  fStrorageDBpath = "local://$ALICE_ROOT";  
}

AliHLTTRDCalibrationComponent::~AliHLTTRDCalibrationComponent()
{
}

const char* AliHLTTRDCalibrationComponent::GetComponentID()
{
  return "TRDCalibration"; // The ID of this component
}

void AliHLTTRDCalibrationComponent::GetInputDataTypes( vector<AliHLTComponent_DataType>& list)
{
  list.clear(); // We do not have any requirements for our input data type(s).
  list.push_back( AliHLTTRDDefinitions::gkDDLRawDataType );
}

AliHLTComponent_DataType AliHLTTRDCalibrationComponent::GetOutputDataType()
{
  return AliHLTTRDDefinitions::gkClusterDataType;
}

void AliHLTTRDCalibrationComponent::GetOutputDataSize( unsigned long& constBase, double& inputMultiplier )
{
  constBase = 0;
  inputMultiplier = ((double)fOutputPercentage)/100.0;
}


// Spawn function, return new instance of this class
AliHLTComponent* AliHLTTRDCalibrationComponent::Spawn()
{
  return new AliHLTTRDCalibrationComponent;
};

int AliHLTTRDCalibrationComponent::DoInit( int argc, const char** argv )
{
  // perform initialization. We check whether our relative output size is specified in the arguments.
  fOutputPercentage = 100;
  int i = 0;
  char* cpErr;
  while ( i < argc )
    {
      Logging( kHLTLogDebug, "HLT::TRDCalibration::DoInit", "Arguments", "argv[%d] == %s", i, argv[i] );
      if ( !strcmp( argv[i], "output_percentage" ) )
	{
	  if ( i+1>=argc )
	    {
	      Logging(kHLTLogError, "HLT::TRDCalibration::DoInit", "Missing Argument", "Missing output_percentage parameter");
	      return ENOTSUP;
	    }
	  Logging( kHLTLogDebug, "HLT::TRDCalibration::DoInit", "Arguments", "argv[%d+1] == %s", i, argv[i+1] );
	  fOutputPercentage = strtoul( argv[i+1], &cpErr, 0 );
	  if ( *cpErr )
	    {
	      Logging(kHLTLogError, "HLT::TRDCalibration::DoInit", "Wrong Argument", "Cannot convert output_percentage parameter '%s'", argv[i+1] );
	      return EINVAL;
	    }
	  Logging( kHLTLogInfo, "HLT::TRDCalibration::DoInit", "Output percentage set", "Output percentage set to %lu %%", fOutputPercentage );
	  i += 2;
	  continue;
	}

      if ( strcmp( argv[i], "-cdb" ) == 0)
	{
	  if ( i+1 >= argc )
	    {
	      Logging(kHLTLogError, "HLT::TRDCalibration::DoInit", "Missing Argument", "Missing -cdb argument");
	      return ENOTSUP;	      
	    }
	  fStrorageDBpath = argv[i+1];
	  Logging( kHLTLogInfo, "HLT::TRDCalibration::DoInit", "DB storage set", "DB storage is %s", fStrorageDBpath.c_str() );	  
	  i += 2;
	  continue;
	}      

      if ( strcmp( argv[i], "-dbg" ) == 0)
	{
	  if ( i+1 >= argc )
	    {
	      Logging(kHLTLogError, "HLT::TRDCalibration::DoInit", "Missing Argument", "Missing -dbg argument");
	      return ENOTSUP;	      
	    }
	  fTriggerParDebugLevel = strtol( argv[i+1], &cpErr, 10 );
	  if ( *cpErr )
	    {
	      Logging(kHLTLogError, "HLT::TRDCalibration::DoInit", "Wrong Argument", "Cannot convert -dbg parameter '%s'", argv[i+1] );
	      return EINVAL;
	    }
	  Logging( kHLTLogInfo, "HLT::TRDCalibration::DoInit", "Trigger Params", "Debug Level set to %d ", fTriggerParDebugLevel);
	  i += 2;
	  continue;
	}      

      if ( strcmp( argv[i], "-ptcut" ) == 0)
	{
	  if ( i+1 >= argc )
	    {
	      Logging(kHLTLogError, "HLT::TRDCalibration::DoInit", "Missing Argument", "Missing -ptcut argument");
	      return ENOTSUP;	      
	    }
	  fLTUpTcut = strtod( argv[i+1], &cpErr);
	  if ( *cpErr )
	    {
	      Logging(kHLTLogError, "HLT::TRDCalibration::DoInit", "Wrong Argument", "Cannot convert -ptcut parameter '%s'", argv[i+1] );
	      return EINVAL;
	    }
	  Logging( kHLTLogInfo, "HLT::TRDCalibration::DoInit", "Trigger Params", "Pt Cut set to %d GeV/c", fLTUpTcut);
	  i += 2;
	  continue;
	}      

      if ( strcmp( argv[i], "-field" ) == 0)
	{
	  if ( i+1 >= argc )
	    {
	      Logging(kHLTLogError, "HLT::TRDCalibration::DoInit", "Missing Argument", "Missing -field argument");
	      return ENOTSUP;	      
	    }
	  fBField = strtod( argv[i+1], &cpErr);
	  if ( *cpErr )
	    {
	      Logging(kHLTLogError, "HLT::TRDCalibration::DoInit", "Wrong Argument", "Cannot convert -field parameter '%s'", argv[i+1] );
	      return EINVAL;
	    }
	  Logging( kHLTLogInfo, "HLT::TRDCalibration::DoInit", "Trigger Params", "Field set to %d ", fBField);
	  i += 2;
	  continue;
	}      

      Logging(kHLTLogError, "HLT::TRDCalibration::DoInit", "Unknown Option", "Unknown option '%s'", argv[i] );
      return EINVAL;
    }

  cdb = AliCDBManager::Instance();
  if (!cdb)
    {
      Logging(kHLTLogError, "HLT::TRDCalibration::DoInit", "Could not get CDB instance", "cdb 0x%x", cdb);
    }
  else
    {
      cdb->SetRun(0); // THIS HAS TO BE RETRIEVED !!!
      cdb->SetDefaultStorage(fStrorageDBpath.c_str());
      Logging(kHLTLogDebug, "HLT::TRDCalibration::DoInit", "CDB instance", "cdb 0x%x", cdb);
    }

  calibra = AliHLTTRDCalibra::Instance();
  if (!calibra) 
    {
      Logging(kHLTLogError, "HLT::TRDCalibration::DoInit", "Could not get Calibra instance", "calibra 0x%x", calibra);
    }
  else
    {
      // init the histograms for output!
      calibra->SetOn();
      calibra->Init2Dhistos();
      Logging(kHLTLogDebug, "HLT::TRDCalibration::DoInit", "Calibra instance", "calibra 0x%x", calibra);
    }  

  rmem = new AliRawReaderMemory;

  fMCMtriggerParams = new AliTRDtrigParam("TRDMCMtriggerParams", "TRDMCMtriggerParams");
  fMCMtriggerParams->SetDebugLevel(fTriggerParDebugLevel);
  fMCMtriggerParams->SetLtuPtCut(fLTUpTcut);
  fMCMtriggerParams->SetField(fBField);

  fMCMtrigger = new AliTRDtriggerHLT("TRDMCMtrigger", "TRDMCMtrigger");
  fMCMtrigger->SetParameter(fMCMtriggerParams);
  fMCMtrigger->Init();

  return 0;
}

int AliHLTTRDCalibrationComponent::DoDeinit()
{
  Logging( kHLTLogDebug, "HLT::TRDCalibration::DoDeinit", "destruct", "start");

  Logging( kHLTLogDebug, "HLT::TRDCalibration::DoDeinit", "delete", "rmem");
  delete rmem;
  rmem = 0;

  Logging( kHLTLogDebug, "HLT::TRDCalibration::DoDeinit", "delete", "fMCMtriggerParams");
  delete fMCMtriggerParams;
  fMCMtriggerParams = 0;

  Logging( kHLTLogDebug, "HLT::TRDCalibration::DoDeinit", "delete", "fMCMtrigger");
  delete fMCMtrigger;
  fMCMtrigger = 0;

  if (calibra)
    {
      Logging( kHLTLogDebug, "HLT::TRDCalibration::DoDeinit", "destroy", "calibra");
      calibra->Destroy();
      calibra = 0;
    }

  if (cdb)
    {
      Logging( kHLTLogDebug, "HLT::TRDCalibration::DoDeinit", "destroy", "cdb");
      cdb->Destroy();
      cdb = 0;
    }

  Logging( kHLTLogDebug, "HLT::TRDCalibration::DoDeinit", "destruct", "all done!");
  return 0;
}

int AliHLTTRDCalibrationComponent::DoEvent( const AliHLTComponent_EventData& evtData, const AliHLTComponent_BlockData* blocks, 
					    AliHLTComponent_TriggerData& trigData, AliHLTUInt8_t* outputPtr, 
					    AliHLTUInt32_t& size, vector<AliHLTComponent_BlockData>& outputBlocks )
{
  Logging( kHLTLogInfo, "HLT::TRDCalibration::DoEvent", "Output percentage set", "Output percentage set to %lu %%", fOutputPercentage );
  Logging( kHLTLogInfo, "HLT::TRDCalibration::DoEvent", "BLOCKS", "NofBlocks %lu", evtData.fBlockCnt );
  // Process an event
  unsigned long totalSize = 0;
  AliHLTUInt32_t fDblock_Specification = 0;

  // Loop over all input blocks in the event
  for ( unsigned long i = 0; i < evtData.fBlockCnt; i++ )
    {
      char tmp1[14], tmp2[14];
      DataType2Text( blocks[i].fDataType, tmp1 );
      DataType2Text( AliHLTTRDDefinitions::gkDDLRawDataType, tmp2 );      
      Logging( kHLTLogDebug, "HLT::TRDCalibration::DoEvent", "Event received", 
	       "Event 0x%08LX (%Lu) received datatype: %s - required datatype: %s",
	       evtData.fEventID, evtData.fEventID, tmp1, tmp2 );

      if ( blocks[i].fDataType != AliHLTTRDDefinitions::gkDDLRawDataType ) 
	{
 	  Logging (kHLTLogError, "HLT::TRDCalibration::DoEvent", "COMPARE FAILED", "type=%d is type=%d",
 		   blocks[i].fDataType, AliHLTTRDDefinitions::gkDDLRawDataType);
	  continue;
	}
      fDblock_Specification = blocks[i].fSpecification;
      unsigned long blockSize = blocks[i].fSize;
      totalSize += blockSize;
    }

  void *memBufIn = calloc(totalSize, 1);
  AliHLTUInt8_t *pBuf = (AliHLTUInt8_t *)memBufIn;
  if (memBufIn == NULL)
    {
      Logging( kHLTLogError, "HLT::TRDCalibration::DoEvent", "MEMORY", "Unable to allocate %lu bytes", totalSize);
      return -1;
    }

  // Make the memory continuous
  unsigned long copied = 0;
  for ( unsigned long i = 0; i < evtData.fBlockCnt; i++ )
    {
      if ( blocks[i].fDataType != AliHLTTRDDefinitions::gkDDLRawDataType ) 
	continue;

      void *pos = (void*)(pBuf + copied);
      void *copyret = memcpy(pos, blocks[i].fPtr, blocks[i].fSize);
      if (copyret < 0)
	{
	  Logging( kHLTLogError, "HLT::TRDCalibration::DoEvent", "MEMORY", "Unable to copy %lu bytes", blocks[i].fSize);
	  return -1;
	}
      copied += blocks[i].fSize;
    }

  Logging( kHLTLogInfo, "HLT::TRDCalibration::DoEvent", "COPY STATS", "total=%lu copied=%lu", totalSize, copied);

  rmem->Reset();
  rmem->SetMemory((UChar_t*)memBufIn, totalSize);
  //rmem->Reset();
  Bool_t ihead = rmem->ReadHeader();
  if (ihead == kTRUE)
    {
      Logging( kHLTLogInfo, "HLT::TRDCalibration::DoEvent", "HEADER", "Header read successfully");
    }
  else
    {
      Logging( kHLTLogError, "HLT::TRDCalibration::DoEvent", "HEADER", "Header read ERROR");
      //return -1; -- not FATAL
    }

  fMCMtrigger->ResetTree();
  Bool_t ireadD = fMCMtrigger->ReadDigits(rmem);
  if (ireadD == kTRUE)
    {
      Logging( kHLTLogInfo, "HLT::TRDCalibration::DoEvent", "DIGITS", "Digits read successfully");
    }
  else
    {
      Logging( kHLTLogError, "HLT::TRDCalibration::DoEvent", "DIGITS", "Digits read ERROR");
      return -1;
    }

  Bool_t iclustered = fMCMtrigger->MakeTracklets();
  if (iclustered == kTRUE)
    {
      Logging( kHLTLogInfo, "HLT::TRDCalibration::DoEvent", "TRACKLETS", "Tracklets created successfully");
    }
  else
    {
      Logging( kHLTLogError, "HLT::TRDCalibration::DoEvent", "TRACKLETS", "Tracklets creation ERROR");
      return -1;
    }

  free(memBufIn);

  UInt_t memBufOutSize = 0;

  // put the tree into output blocks of TObjArrays
  TTree *fcTree = fMCMtrigger->GetTrackletTree();
  TList *lt = (TList*)fcTree->GetListOfBranches();
  TIter it(lt);
  it.Reset();
  TBranch *tb = 0;
  while ((tb = (TBranch*)it.Next()) != 0)
    {
      TObjArray *detTracklets = 0;
      tb->SetAddress(&detTracklets);
      for (Int_t icb = 0; icb < tb->GetEntries(); icb++)
	{
	  tb->GetEntry(icb);
	  PushBack(detTracklets, AliHLTTRDDefinitions::gkMCMtrackletDataType, fDblock_Specification);
	}
    }

  // add the histograms...
  //gkMCMCalibrationDataType

  //AliHLTTRDCalibra *calibra = AliHLTTRDCalibra::Instance();
  if (!calibra) 
    {
      Logging( kHLTLogError, "HLT::TRDCalibration::DoEvent", "OUTPUT", "Calibra not valid");
    }
  else
    {
      Logging( kHLTLogDebug, "HLT::TRDCalibration::DoEvent", "OUTPUT", "Here we should put the histos...");
//       //   TH2I            *GetCH2d();
//       TH2I *chtmp = calibra->GetCH2d();
//       if (chtmp)
// 	PushBack(chtmp, AliHLTTRDDefinitions::gkMCMcalibrationDataType, fDblock_Specification);
//       else
// 	Logging( kHLTLogError, "HLT::TRDCalibration::DoEvent", "OUTPUT", "Could not calibra->GetCH2d()");
//       //   TProfile2D      *GetPH2d();
//       TProfile2D *phtmp = calibra->GetPH2d();
//       if (phtmp)
// 	PushBack(phtmp, AliHLTTRDDefinitions::gkMCMcalibrationDataType, fDblock_Specification);
//       else
//  	Logging( kHLTLogError, "HLT::TRDCalibration::DoEvent", "OUTPUT", "Could not calibra->GetPH2d()");
//       //   TProfile2D      *GetPRF2d();
//       TProfile2D *prf = calibra->GetPRF2d();
//       if (prf)
// 	PushBack(prf, AliHLTTRDDefinitions::gkMCMcalibrationDataType, fDblock_Specification);      
//       else
// 	Logging( kHLTLogError, "HLT::TRDCalibration::DoEvent", "OUTPUT", "Could not calibra->GetPRF2d()");
    }

  return 0;
}
