// $Id$

//**************************************************************************
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//*                                                                        *
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *
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

/** @file   AliHLTReconstructor.cxx
    @author Matthias Richter
    @date   
    @brief  Binding class for HLT reconstruction in AliRoot. */

#include <TSystem.h>
#include <TObjString.h>
#include "TFile.h"
#include "TTree.h"
#include "TObject.h"
#include "TObjArray.h"
#include "TClass.h"
#include "TStreamerInfo.h"
#include "AliHLTReconstructor.h"
#include "AliLog.h"
#include "AliRawReader.h"
#include "AliESDEvent.h"
#include "AliHLTSystem.h"
#include "AliHLTOUTRawReader.h"
#include "AliHLTOUTDigitReader.h"
#include "AliHLTEsdManager.h"
#include "AliHLTPluginBase.h"
#include "AliHLTMisc.h"
#include "AliCDBManager.h"
#include "AliCDBEntry.h"

class AliCDBEntry;

ClassImp(AliHLTReconstructor)

AliHLTReconstructor::AliHLTReconstructor()
  : 
  AliReconstructor(),
  fFctProcessHLTOUT(NULL),
  fpEsdManager(NULL),
  fpPluginBase(new AliHLTPluginBase)
  , fFlags(0)
{ 
  //constructor
}

AliHLTReconstructor::AliHLTReconstructor(const char* options)
  : 
  AliReconstructor(),
  fFctProcessHLTOUT(NULL),
  fpEsdManager(NULL),
  fpPluginBase(new AliHLTPluginBase)
  , fFlags(0)
{ 
  //constructor
  if (options) Init(options);
}

AliHLTReconstructor::~AliHLTReconstructor()
{ 
  //destructor

  if (fpPluginBase) {
  AliHLTSystem* pSystem=fpPluginBase->GetInstance();
  if (pSystem) {
    AliDebug(0, Form("terminate HLT system: status %#x", pSystem->GetStatusFlags()));
    if (pSystem->CheckStatus(AliHLTSystem::kStarted)) {
      // send specific 'event' to execute the stop sequence
      pSystem->Reconstruct(0, NULL, NULL);
    }
  }
  delete fpPluginBase;
  }
  fpPluginBase=NULL;

  if (fpEsdManager) AliHLTEsdManager::Delete(fpEsdManager);
  fpEsdManager=NULL;
}

void AliHLTReconstructor::Init(const char* options)
{
  // init the reconstructor
  SetOption(options);
  Init();
}

void AliHLTReconstructor::Init()
{
  // init the reconstructor
  if (!fpPluginBase) {
    AliError("internal memory error: can not get AliHLTSystem instance from plugin");
    return;
  }

  AliHLTSystem* pSystem=fpPluginBase->GetInstance();
  if (!pSystem) {
    AliError("can not create AliHLTSystem object");
    return;
  }
  if (pSystem->CheckStatus(AliHLTSystem::kError)) {
    AliError("HLT system in error state");
    return;
  }

  TString esdManagerOptions;

  // the options scan has been moved to AliHLTSystem, the old code
  // here is kept to be able to run an older version of the HLT code
  // with newer AliRoot versions.
  TString libs("");
  TString option = GetOption();
  TObjArray* pTokens=option.Tokenize(" ");
  option="";
  if (pTokens) {
    int iEntries=pTokens->GetEntries();
    for (int i=0; i<iEntries; i++) {
      TString token=(((TObjString*)pTokens->At(i))->GetString());
      if (token.Contains("loglevel=")) {
	TString param=token.ReplaceAll("loglevel=", "");
	if (param.IsDigit()) {
	  pSystem->SetGlobalLoggingLevel((AliHLTComponentLogSeverity)param.Atoi());
	} else if (param.BeginsWith("0x") &&
		   param.Replace(0,2,"",0).IsHex()) {
	  int severity=0;
	  sscanf(param.Data(),"%x", &severity);
	  pSystem->SetGlobalLoggingLevel((AliHLTComponentLogSeverity)severity);
	} else {
	  AliWarning("wrong parameter for option \'loglevel=\', (hex) number expected");
	}
      } else if (token.Contains("alilog=off")) {
	pSystem->SwitchAliLog(0);
      } else if (token.CompareTo("ignore-hltout")==0) {
	fFlags|=kAliHLTReconstructorIgnoreHLTOUT;
      } else if (token.Contains("esdmanager=")) {
	token.ReplaceAll("esdmanager=", "");
	token.ReplaceAll(","," ");
	token.ReplaceAll("'","");
	esdManagerOptions=token;
      } else if (token.BeginsWith("lib") && token.EndsWith(".so")) {
	libs+=token;
	libs+=" ";
      } else {
	if (option.Length()>0) option+=" ";
	option+=token;
      }
    }
    delete pTokens;
  }

  if (!libs.IsNull() &&
      (!pSystem->CheckStatus(AliHLTSystem::kLibrariesLoaded)) &&
      (pSystem->LoadComponentLibraries(libs.Data())<0)) {
    AliError("error while loading HLT libraries");
    return;
  }

  if (!pSystem->CheckStatus(AliHLTSystem::kReady)) {
    typedef int (*AliHLTSystemSetOptions)(AliHLTSystem* pInstance, const char* options);
    gSystem->Load("libHLTinterface.so");
    AliHLTSystemSetOptions pFunc=(AliHLTSystemSetOptions)(gSystem->DynFindSymbol("libHLTinterface.so", "AliHLTSystemSetOptions"));
    if (pFunc) {
      if ((pFunc)(pSystem, option.Data())<0) {
      AliError("error setting options for HLT system");
      return;	
      }
    } else if (option.Length()>0) {
      AliError(Form("version of HLT system does not support the options \'%s\'", option.Data()));
      return;
    }
    if ((pSystem->Configure())<0) {
      AliError("error during HLT system configuration");
      return;
    }
  }

  gSystem->Load("libHLTinterface.so");
  fFctProcessHLTOUT=(void (*)())gSystem->DynFindSymbol("libHLTinterface.so", "AliHLTSystemProcessHLTOUT");

  fpEsdManager=AliHLTEsdManager::New();
  fpEsdManager->SetOption(esdManagerOptions.Data());

  InitStreamerInfos();
}

const char* AliHLTReconstructor::fgkCalibStreamerInfoEntry="HLT/Calib/StreamerInfo";

int AliHLTReconstructor::InitStreamerInfos()
{
  // init streamer infos for HLT reconstruction
  // Root schema evolution is not enabled for AliHLTMessage and all streamed objects.
  // Objects in the raw data payload rely on the availability of the correct stream info.
  // The relevant streamer info for a specific run is stored in the OCDB.
  // The method evaluates the following entries:
  // - HLT/Calib/StreamerInfo

  // to be activated later, this is supposed to go as patch into the v4-17-Release branch
  // which doe snot have the AliHLTMisc implementation
  //AliCDBEntry* pEntry=AliHLTMisc::Instance().LoadOCDBEntry(fgkCalibStreamerInfoEntry);
  AliCDBEntry* pEntry=AliCDBManager::Instance()->Get(fgkCalibStreamerInfoEntry);
  TObject* pObject=NULL;
  //if (pEntry && (pObject=AliHLTMisc::Instance().ExtractObject(pEntry))!=NULL)
  if (pEntry && (pObject=pEntry->GetObject())!=NULL)
    {
    TObjArray* pSchemas=dynamic_cast<TObjArray*>(pObject);
    if (pSchemas) {
      for (int i=0; i<pSchemas->GetEntriesFast(); i++) {
	if (pSchemas->At(i)) {
	  TStreamerInfo* pSchema=dynamic_cast<TStreamerInfo*>(pSchemas->At(i));
	  if (pSchema) {
	    int version=pSchema->GetClassVersion();
	    TClass* pClass=TClass::GetClass(pSchema->GetName());
	    if (pClass) {
	      if (pClass->GetClassVersion()==version) {
		AliDebug(0,Form("skipping schema definition %d version %d to class %s as this is the native version", i, version, pSchema->GetName()));
		continue;
	      }
	      TObjArray* pInfos=pClass->GetStreamerInfos();
	      if (pInfos /*&& version<pInfos->GetEntriesFast()*/) {
		if (pInfos->At(version)==NULL) {
		  TStreamerInfo* pClone=(TStreamerInfo*)pSchema->Clone();
		  if (pClone) {
		    pClone->SetClass(pClass);
		    pClone->BuildOld();
		    pInfos->AddAtAndExpand(pClone, version);
		    AliDebug(0,Form("adding schema definition %d version %d to class %s", i, version, pSchema->GetName()));
		  } else {
		    AliError(Form("skipping schema definition %d (%s), unable to create clone object", i, pSchema->GetName()));
		  }
		} else {
		  TStreamerInfo* pInfo=dynamic_cast<TStreamerInfo*>(pInfos->At(version));
		  if (pInfo && pInfo->GetClassVersion()==version) {
		    AliDebug(0,Form("schema definition %d version %d already available in class %s", i, version, pSchema->GetName()));
		  } else {
		    AliError(Form("can not verify version for already existing schema definition %d (%s) version %d: version of existing definition is %d", i, pSchema->GetName(), version, pInfo?pInfo->GetClassVersion():-1));
		  }
		}
	      } else {
		AliError(Form("skipping schema definition %d (%s), unable to set version %d in info array of size %d", i, pSchema->GetName(), version, pInfos?pInfos->GetEntriesFast():-1));
	      }
	    } else {
	      AliError(Form("skipping schema definition %d (%s), unable to find class", i, pSchema->GetName()));
	    }
	  } else {
	    AliError(Form("skipping schema definition %d, not of TStreamerInfo", i));
	  }
	}
      }
    } else {
      AliError(Form("internal mismatch in OCDB entry %s: wrong class type", fgkCalibStreamerInfoEntry));
    }
  } else {
    AliWarning(Form("missing HLT reco data (%s), skipping initialization of streamer info for TObjects in HLT raw data payload", fgkCalibStreamerInfoEntry));
  }
  return 0;
}

void AliHLTReconstructor::Reconstruct(AliRawReader* rawReader, TTree* /*clustersTree*/) const 
{
  // reconstruction of real data without writing of ESD
  // For each event, HLT reconstruction chains can be executed and
  // added to the existing HLTOUT data
  // The HLTOUT data is finally processed in FillESD
  if (!fpPluginBase) {
    AliError("internal memory error: can not get AliHLTSystem instance from plugin");
    return;
  }

  int iResult=0;
  AliHLTSystem* pSystem=fpPluginBase->GetInstance();

  if (pSystem) {
    if (pSystem->CheckStatus(AliHLTSystem::kError)) {
      AliError("HLT system in error state");
      return;
    }
    if (!pSystem->CheckStatus(AliHLTSystem::kReady)) {
      AliError("HLT system in wrong state");
      return;
    }
    if ((iResult=pSystem->Reconstruct(1, NULL, rawReader))>=0) {
    }
  }
}

void AliHLTReconstructor::FillESD(AliRawReader* rawReader, TTree* /*clustersTree*/, 
				  AliESDEvent* esd) const
{
  // reconstruct real data and fill ESD
  if (!rawReader || !esd) {
    AliError("missing raw reader or esd object");
    return;
  }

  if (!fpPluginBase) {
    AliError("internal memory error: can not get AliHLTSystem instance from plugin");
    return;
  }

  AliHLTSystem* pSystem=fpPluginBase->GetInstance();

  if (pSystem) {
    if (pSystem->CheckStatus(AliHLTSystem::kError)) {
      AliError("HLT system in error state");
      return;
    }
    if (!pSystem->CheckStatus(AliHLTSystem::kReady)) {
      AliError("HLT system in wrong state");
      return;
    }
    pSystem->FillESD(-1, NULL, esd);

    AliRawReader* input=NULL;
    if ((fFlags&kAliHLTReconstructorIgnoreHLTOUT) == 0 ) {
      input=rawReader;
    }
    AliHLTOUTRawReader* pHLTOUT=new AliHLTOUTRawReader(input, esd->GetEventNumberInFile(), fpEsdManager);
    if (pHLTOUT) {
      ProcessHLTOUT(pHLTOUT, esd);
      delete pHLTOUT;
    } else {
      AliError("error creating HLTOUT handler");
    }
  }
}

void AliHLTReconstructor::Reconstruct(TTree* /*digitsTree*/, TTree* /*clustersTree*/) const
{
  // reconstruct simulated data

  // all reconstruction has been moved to FillESD
  //AliReconstructor::Reconstruct(digitsTree,clustersTree);
  AliInfo("running digit data reconstruction");
}

void AliHLTReconstructor::FillESD(TTree* /*digitsTree*/, TTree* /*clustersTree*/, AliESDEvent* esd) const
{
  // reconstruct simulated data and fill ESD

  // later this is the place to extract the simulated HLT data
  // for now it's only an user failure condition as he tries to run HLT reconstruction
  // on simulated data 
  TString option = GetOption();
  if (!option.IsNull() && 
      (option.Contains("config=") || option.Contains("chains="))) {
    AliWarning(Form("HLT reconstruction can be run embedded into Alireconstruction from\n"
		    "raw data (real or simulated)). Reconstruction of of digit data takes\n"
		    "place in AliSimulation, appropriate input conversion is needed.\n"
		    "Consider running embedded into AliSimulation."
		    "        /***  run macro *****************************************/\n"
		    "        AliSimulation sim;\n"
		    "        sim.SetRunHLT(\"%s\");\n"
		    "        sim.SetRunGeneration(kFALSE);\n"
		    "        sim.SetMakeDigits(\"\");\n"
		    "        sim.SetMakeSDigits(\"\");\n"
		    "        sim.SetMakeDigitsFromHits(\"\");\n"
		    "        sim.Run();\n"
		    "        /*********************************************************/", option.Data()));
  }
  if (!fpPluginBase) {
    AliError("internal memory error: can not get AliHLTSystem instance from plugin");
    return;
  }

  AliHLTSystem* pSystem=fpPluginBase->GetInstance();
  if (pSystem) {
    if (pSystem->CheckStatus(AliHLTSystem::kError)) {
      AliError("HLT system in error state");
      return;
    }
    if (!pSystem->CheckStatus(AliHLTSystem::kReady)) {
      AliError("HLT system in wrong state");
      return;
    }

    AliHLTOUTDigitReader* pHLTOUT=new AliHLTOUTDigitReader(esd->GetEventNumberInFile(), fpEsdManager);
    if (pHLTOUT) {
      ProcessHLTOUT(pHLTOUT, esd);
      delete pHLTOUT;
    } else {
      AliError("error creating HLTOUT handler");
    }
  }
}

void AliHLTReconstructor::ProcessHLTOUT(AliHLTOUT* pHLTOUT, AliESDEvent* esd, bool bVerbose) const
{
  // treatment of simulated or real HLTOUT data
  if (!pHLTOUT) return;
  if (!fpPluginBase) {
    AliError("internal memory error: can not get AliHLTSystem instance from plugin");
    return;
  }

  AliHLTSystem* pSystem=fpPluginBase->GetInstance();
  if (!pSystem) {
    AliError("error getting HLT system instance");
    return;
  }

  if (pHLTOUT->Init()<0) {
    AliError("error : initialization of HLTOUT handler failed");
    return;
  }

  if (bVerbose)
    PrintHLTOUTContent(pHLTOUT);

  if (fFctProcessHLTOUT) {
    typedef int (*AliHLTSystemProcessHLTOUT)(AliHLTSystem* pInstance, AliHLTOUT* pHLTOUT, AliESDEvent* esd);
    AliHLTSystemProcessHLTOUT pFunc=(AliHLTSystemProcessHLTOUT)fFctProcessHLTOUT;
    if ((pFunc)(pSystem, pHLTOUT, esd)<0) {
      AliError("error processing HLTOUT");
    }
  }
  pHLTOUT->Reset();
}

void AliHLTReconstructor::ProcessHLTOUT(const char* digitFile, AliESDEvent* pEsd) const
{
  // debugging/helper function to examine simulated data
  if (!digitFile) return;

  // read the number of events
  TFile f(digitFile);
  if (f.IsZombie()) return;
  TTree* pTree=NULL;
  f.GetObject("rawhltout", pTree);
  if (!pTree) {
    AliWarning(Form("can not find tree rawhltout in file %s", digitFile));
    return ;
  }
  int nofEvents=pTree->GetEntries();
  f.Close();
  //delete pTree; OF COURSE NOT! its an object in the file
  pTree=NULL;

  for (int event=0; event<nofEvents; event++) {
    AliHLTOUTDigitReader* pHLTOUT=new AliHLTOUTDigitReader(event, fpEsdManager, digitFile);
    if (pHLTOUT) {
      AliInfo(Form("event %d", event));
      ProcessHLTOUT(pHLTOUT, pEsd, true);
      delete pHLTOUT;
    } else {
      AliError("error creating HLTOUT handler");
    }
  }
}

void AliHLTReconstructor::ProcessHLTOUT(AliRawReader* pRawReader, AliESDEvent* pEsd) const
{
  // debugging/helper function to examine simulated or real HLTOUT data
  if (!pRawReader) return;

  pRawReader->RewindEvents();
  for (int event=0; pRawReader->NextEvent(); event++) {
    AliHLTOUTRawReader* pHLTOUT=new AliHLTOUTRawReader(pRawReader, event, fpEsdManager);
    if (pHLTOUT) {
      AliInfo(Form("event %d", event));
      // the two event fields contain: period - orbit - bunch crossing counter
      //        id[0]               id[1]
      // |32                0|32                0|
      //
      // |      28 bit    |       24 bit     | 12|
      //        period          orbit         bcc
      AliHLTUInt64_t eventId=0;
      const UInt_t* rawreaderEventId=pRawReader->GetEventId();
      if (rawreaderEventId) {
	eventId=rawreaderEventId[0];
	eventId=eventId<<32;
	eventId|=rawreaderEventId[1];
      }
      AliInfo(Form("Event Id from rawreader:\t 0x%016llx", eventId));
      ProcessHLTOUT(pHLTOUT, pEsd, true);
      delete pHLTOUT;
    } else {
      AliError("error creating HLTOUT handler");
    }
  }
}

void AliHLTReconstructor::PrintHLTOUTContent(AliHLTOUT* pHLTOUT) const
{
  // print the block specifications of the HLTOUT data blocks
  if (!pHLTOUT) return;
  int iResult=0;

  AliInfo(Form("Event Id from hltout:\t 0x%016llx", pHLTOUT->EventId()));
  for (iResult=pHLTOUT->SelectFirstDataBlock();
       iResult>=0;
       iResult=pHLTOUT->SelectNextDataBlock()) {
    AliHLTComponentDataType dt=kAliHLTVoidDataType;
    AliHLTUInt32_t spec=kAliHLTVoidDataSpec;
    pHLTOUT->GetDataBlockDescription(dt, spec);
    const AliHLTUInt8_t* pBuffer=NULL;
    AliHLTUInt32_t size=0;
    if (pHLTOUT->GetDataBuffer(pBuffer, size)>=0) {
      pHLTOUT->ReleaseDataBuffer(pBuffer);
      pBuffer=NULL; // just a dummy
    }
    AliInfo(Form("   %s  0x%x: size %d", AliHLTComponent::DataType2Text(dt).c_str(), spec, size));
  }
}
