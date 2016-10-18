#include <TFile.h>
#include <TTree.h>
#include "AliCDBManager.h"
#include "AliCDBStorage.h"
#include "AliHLTSystem.h"
#include "AliHLTConfiguration.h"
#include "AliLog.h"
#include "AliHLTOfflineInterface.h"
#include "AliRawReader.h"
#include "AliHLTMUONConstants.h"
#include "AliHLTConfigurationHandler.h"
#include "AliHLTDataTypes.h"
#include "AliHLTPluginBase.h"

///
/// Macro to prepare the preclustering chain
/// to be fed to recraw-local.C
///

void preClusteringChain()
{
  AliHLTSystem* pHLT = AliHLTPluginBase::GetInstance();

  pHLT->ScanOptions(Form("loglevel=%d",kHLTLogWarning|kHLTLogError|kHLTLogFatal));

  pHLT->LoadComponentLibraries("libAliHLTUtil.so");
  pHLT->LoadComponentLibraries("libAliHLTMUON.so");

  // Build the DDL file publishers using AliRawReaderPublisher components.
  TString cmd;
  TString pubName;
  TString sources = "";


  for (Int_t i = 0; i < 20; ++i) {

    pubName.Form("pubDDL%d",i+1);
    sources += pubName + " ";

    Int_t ddlId = 2560+i;
    cmd.Form("-skipempty -minid %d -datatype 'DDL_RAW ' 'MUON'  -dataspec 0x%06x",ddlId,(1<<i));
    new AliHLTConfiguration(pubName.Data(),"AliRawReaderPublisher",NULL,cmd.Data());
  }

  AliHLTConfiguration digitLoader("digitLoader","MUONDigitLoader",sources.Data(),"");

  AliHLTConfiguration preClustering("preClustering","MUONPreclusterFinder","digitLoader","");

  AliHLTConfiguration clWriting("clWriting","MUONClusterWriter","preClustering","-datafile MUON.RecPoints.root");
}
