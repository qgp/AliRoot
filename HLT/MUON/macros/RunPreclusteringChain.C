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

void RunPreclusteringChain(Int_t dataType = 0, // 0 = raw, 1 = digit
                           const char* dataFile = "raw.root",
                           Int_t nEvent=-1)
{
  /*
   gSystem->SetIncludePath("-I${ALICE_BUILD}/include -I${ALICE_ROOT}/RAW -I${ALICE_ROOT}/MUON -I${ALICE_ROOT}/MUON/mapping -I${ALICE_ROOT}/HLT/BASE -I${ALICE_ROOT}/HLT/BASE/HOMER -I${ALICE_ROOT}/HLT/MUON -I${ALICE_ROOT}/HLT/MUON/macros -I${ALICE_ROOT}/HLT/MUON/OfflineInterface -I${ALICE_ROOT}/HLT/MUON/OnlineAnalysis")
   .x RunPreclusteringChain()
  */

  if (dataType < 0 || dataType > 1) {
    cerr << "ERROR: Wrong data type: 0 = raw, 1 = digit." << endl;
    return;
  }

  AliHLTSystem sys;

  sys.ScanOptions(Form("loglevel=%d",kHLTLogWarning|kHLTLogError|kHLTLogFatal));

  sys.LoadComponentLibraries("libAliHLTUtil.so");
  sys.LoadComponentLibraries("libAliHLTMUON.so");

  // Build the DDL file publishers using AliRawReaderPublisher components.
  TString cmd;
  TString pubName;
  TString sources = "";

  if (dataType == 0) {

    for (Int_t i = 0; i < 20; ++i) {

      pubName.Form("pubDDL%d",i+1);
      sources += pubName + " ";

      Int_t ddlId = 2560+i;
      cmd.Form("-skipempty -minid %d -datatype 'DDL_RAW ' 'MUON'  -dataspec 0x%06x",ddlId,(1<<i));
      new AliHLTConfiguration(pubName.Data(),"AliRawReaderPublisher",NULL,cmd.Data());

    }

  } else {

    pubName = "digits";
    sources += pubName;

    cmd.Form("-datafile %s",dataFile);
    new AliHLTConfiguration(pubName.Data(),"MUONDigitReader",NULL,cmd.Data());

  }

  AliHLTConfiguration digitLoader("digitLoader","MUONDigitLoader",sources.Data(),"-cdbpath local://$ALICE_ROOT/OCDB -run 0");

  AliHLTConfiguration preClustering("preClustering","MUONPreclusterFinder","digitLoader","-cdbpath local://$ALICE_ROOT/OCDB -run 0");

  AliHLTConfiguration clWriting("clWriting","MUONClusterWriter","preClustering","-datafile MUON.RecPoints.root");

  AliHLTConfigurationHandler::Instance()->Print();

  // Build and run the chain's tasks.
  sys.BuildTaskList("clWriting");

  if (dataType == 0) {

    // Setup the raw reader.
    AliRawReader* rawReader = AliRawReader::Create(dataFile);
    if (rawReader == NULL)
    {
      cerr << "ERROR: Could not create raw reader." << endl;
      return;
    }
    if (! rawReader->IsRawReaderValid())
    {
      cerr << "ERROR: Raw reader is not valid." << endl;
      return;
    }

    // get number of events to process
    Int_t nEventMax = rawReader->GetNumberOfEvents();
    if (nEvent < 0 || nEvent > nEventMax) nEvent = nEventMax;

    // connect the rawReader to the publisher
    AliHLTOfflineInterface::SetParamsToComponents(NULL, rawReader);

    rawReader->NextEvent(); // Need to call this once here or we will start at the wrong event.

    // Now step through the events.
    for (int i = 0; i < nEvent; i++)
    {
      // The "(i == lastEvent) ? 1 : 0" part is to indicate a
      // stop run command when we hit the last event.
      sys.Run(1, (i == nEvent-1) ? 1 : 0);
      rawReader->NextEvent();
    }

  } else {

    // get number of events to process
    TFile *file = new TFile(dataFile,"READ");
    if (!file) {
      cerr << "ERROR: Cannot open digit file." << endl;
      return;
    }
    TTree *tree = dynamic_cast<TTree*>(file->Get("TreeD"));
    if (!tree) {
      cerr << "ERROR: Cannot find digit tree \"TreeD\" in file" << endl;
      delete file;
      return;
    }
    Int_t nEventMax = tree->GetEntries();
    if (nEvent < 0 || nEvent > nEventMax) nEvent = nEventMax;
    file->Close();
    delete file;

    // Now step through the events.
    sys.Run(nEvent,1);

  }

}
