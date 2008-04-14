///////////////////////////////////////////////
//  Author: Henrik Tydesjo                   //
//  Preprocessor Class for the SPD           //
//                                           //
///////////////////////////////////////////////

#include "AliITSPreprocessorSPD.h"
#include "AliITSCalibrationSPD.h"
#include "AliITSOnlineCalibrationSPDhandler.h"
#include "AliCDBEntry.h"
#include "AliCDBMetaData.h"
#include "AliShuttleInterface.h"
#include "AliLog.h"
#include <TTimeStamp.h>
#include <TObjString.h>
#include <TSystem.h>
#include <fstream>

/* $Id$ */

ClassImp(AliITSPreprocessorSPD)

//______________________________________________________________________________________________
AliITSPreprocessorSPD::AliITSPreprocessorSPD(AliShuttleInterface* shuttle) :
  AliPreprocessor("SPD", shuttle), fIdList()
{
  // constructor
  AddRunType("DAQ_MIN_TH_SCAN");
  AddRunType("DAQ_MEAN_TH_SCAN");
  AddRunType("DAQ_GEN_DAC_SCAN");
  AddRunType("DAQ_UNIFORMITY_SCAN");
  AddRunType("DAQ_NOISY_PIX_SCAN");
  AddRunType("DAQ_PIX_DELAY_SCAN");
  AddRunType("DAQ_FO_UNIF_SCAN");
  AddRunType("PHYSICS");

  fIdList.SetOwner(kTRUE);
}

//______________________________________________________________________________________________
AliITSPreprocessorSPD::~AliITSPreprocessorSPD()
{
  // destructor
}

//______________________________________________________________________________________________
void AliITSPreprocessorSPD::Initialize(Int_t run, UInt_t startTime,
	UInt_t endTime)
{
  // initialize
  AliPreprocessor::Initialize(run, startTime, endTime);

  AliInfo(Form("\n\tRun %d \n\tStartTime %s \n\tEndTime %s", run,
	       TTimeStamp(startTime).AsString(),
	       TTimeStamp(endTime).AsString()));
}

//______________________________________________________________________________________________
UInt_t AliITSPreprocessorSPD::Process(TMap* /*dcsAliasMap*/)
{
  // Do the actual preprocessing


  // *** GET RUN TYPE ***

  TString runType = GetRunType();


  fIdList.Clear();
  UInt_t nrEqForScan  = 0;
  //  UInt_t nrEqForPhysN = 0;
  //  UInt_t nrEqForPhysD = 0;

  // ******************************************************************************************** //
  // *** GET THE FILE IDs FOR DEBUGGING *** //
  if (runType == "DAQ_MIN_TH_SCAN" || 
      runType == "DAQ_MEAN_TH_SCAN" || 
      runType == "DAQ_GEN_DAC_SCAN" || 
      runType == "DAQ_UNIFORMITY_SCAN" || 
      runType == "DAQ_NOISY_PIX_SCAN" || 
      runType == "DAQ_PIX_DELAY_SCAN" || 
      runType == "DAQ_FO_UNIF_SCAN" || 
      runType == "PHYSICS") {
    TString idListId = "SPD_id_list";
    TList* list = GetFileSources(kDAQ,idListId.Data());
    UInt_t nrIdFiles = 0;
    if (list) {
      TListIter *iter = new TListIter(list);
      while (TObjString* fileNameEntry = (TObjString*) iter->Next()) {
	TString fileName = GetFile(kDAQ, idListId.Data(), fileNameEntry->GetString().Data());
	if (fileName.IsNull()) {
	  Log(Form("GetFile failed to retrieve file %s.",fileNameEntry->GetString().Data()));
	  return 1;
	}
	nrIdFiles++;
	ifstream idFile;
	idFile.open(fileName.Data(), ifstream::in);
	if (idFile.fail()) {
	  Log(Form("Could not open file (%s) for reading.",fileName.Data()));
	  return 1;
	}
	else {
	  while(1) {
	    Char_t id[50];
	    idFile >> id;
	    if (idFile.eof()) break;
	    // Add id to the list;
	    fIdList.AddLast(new TObjString(id));
	  }
	}
	idFile.close();
      }
      delete iter;
    }
    if (nrIdFiles==0) {
      Log("Failed to retrieve any id list file.");
      return 1;
    }
  }


  // ******************************************************************************************** //
  // *** REFERENCE DATA *** //

  // Standalone runs:
  if (runType == "DAQ_MIN_TH_SCAN" ||
      runType == "DAQ_MEAN_TH_SCAN" ||
      runType == "DAQ_GEN_DAC_SCAN" || 
      runType == "DAQ_UNIFORMITY_SCAN" ||
      runType == "DAQ_NOISY_PIX_SCAN" ||
      runType == "DAQ_PIX_DELAY_SCAN" ||
      runType == "DAQ_FO_UNIF_SCAN") {
    // Store the scan container files as reference data (0 or 1 file for each equipment)
    for (UInt_t eq=0; eq<20; eq++) {
      TString id = Form("SPD_ref_scan_%d",eq);
      TList* list = GetFileSources(kDAQ,id.Data()); // (the id should be unique, so always 1 file)
      if (list) {
	TObjString* fileNameEntry = (TObjString*) list->First();
	if (fileNameEntry!=NULL) {
	  nrEqForScan++;
	  TString fileName = GetFile(kDAQ, id, fileNameEntry->GetString().Data());
	  if (fileName.IsNull()) {
	    Log(Form("GetFile failed to retrieve file %s.",fileNameEntry->GetString().Data()));
	    return 1;
	  }
	  if (!RemoveIdFromList(id.Data())) {
	    Log(Form("Warning: Retrieved file with id %s, that was not in the id list!",id.Data()));
	  }
	  TString refCAT = Form("SPD_ref_scan_eq_%d",eq);
	  if (!StoreReferenceFile(fileName.Data(),refCAT.Data())) {
	    Log(Form("Failed to store reference file %s.",fileName.Data()));
	    return 1;
	  }
	}
      }
    }
  }

  // Physics runs (online monitoring):
  if (runType == "PHYSICS") {
    // Store the phys "per run" container files as reference data
    if (!StoreRefFromTarForId("SPD_ref_phys")) return 1;
    // Store the phys "dead" container files as reference data
    if (!StoreRefFromTarForId("SPD_ref_phys_dead")) return 1;
  }


// *** OLD CODE NOT TARED
//    for (UInt_t eq=0; eq<20; eq++) {
//      TString id = Form("SPD_ref_phys_%d",eq);
//      TList* list = GetFileSources(kDAQ,id.Data()); // (the id should be unique, so always 1 file)
//      if (list) {
//	TObjString* fileNameEntry = (TObjString*) list->First();
//	if (fileNameEntry!=NULL) {
//	  nrEqForPhysN++;
//	  TString fileName = GetFile(kDAQ, id, fileNameEntry->GetString().Data());
//	  if (fileName.IsNull()) {
//	    Log(Form("GetFile failed to retrieve file %s.",fileNameEntry->GetString().Data()));
//	    return 1;
//	  }
//	  if (!RemoveIdFromList(id.Data())) {
//	    Log(Form("Warning: Retrieved file with id %s, that was not in the id list!",id.Data()));
//	  }
//	  TString refCAT = Form("SPD_ref_phys_eq_%d",eq);
//	  if (!StoreReferenceFile(fileName.Data(),refCAT.Data())) {
//	    Log(Form("Failed to store reference file %s.",fileName.Data()));
//	    return 1;
//	  }
//	}
//      }
//    }
// ***

// *** OLD CODE NOT TARED
//    // Store the phys "dead" container files as reference data (0 or 1 file for each equipment)
//    for (UInt_t eq=0; eq<20; eq++) {
//      TString id = Form("SPD_ref_phys_dead_%d",eq);
//      TList* list = GetFileSources(kDAQ,id.Data()); // (the id should be unique, so always 1 file)
//      if (list) {
//	TObjString* fileNameEntry = (TObjString*) list->First();
//	if (fileNameEntry!=NULL) {
//	  nrEqForPhysD++;
//	  TString fileName = GetFile(kDAQ, id, fileNameEntry->GetString().Data());
//	  if (fileName.IsNull()) {
//	    Log(Form("GetFile failed to retrieve file %s.",fileNameEntry->GetString().Data()));
//	    return 1;
//	  }
//	  if (!RemoveIdFromList(id.Data())) {
//	    Log(Form("Warning: Retrieved file with id %s, that was not in the id list!",id.Data()));
//	  }
//	  TString refCAT = Form("SPD_ref_phys_dead_eq_%d",eq);
//	  if (!StoreReferenceFile(fileName.Data(),refCAT.Data())) {
//	    Log(Form("Failed to store reference file %s.",fileName.Data()));
//	    return 1;
//	  }
//	}
//      }
//    }
//  }
// ***

  // ******************************************************************************************** //


  // *** NOISY AND DEAD DATA *** //

  // Standalone runs:
  if (runType == "DAQ_NOISY_PIX_SCAN") {
    // Retrieve and unpack tared calibration files from FXS
    TString id = "SPD_scan_noisy";
    TList* list = GetFileSources(kDAQ,id.Data());
    if (list) {
      UInt_t index = 0;
      while (list->At(index)!=NULL) {
	TObjString* fileNameEntry = (TObjString*) list->At(index);
	TString fileName = GetFile(kDAQ, id.Data(), fileNameEntry->GetString().Data());
	if (fileName.IsNull()) {
	  Log(Form("GetFile failed to retrieve file %s.",fileNameEntry->GetString().Data()));
	  return 1;
	}
	if (!RemoveIdFromList("SPD_scan_noisy")) {
	  Log(Form("Warning: Retrieved file with id %s, that was not in the id list!",id.Data()));
	}
	TString command = Form("tar -xf %s",fileName.Data());
	gSystem->Exec(command.Data());
	index++;
      }
    }
    // Create new database entries
    TObjArray* spdEntryNoisy = new TObjArray(240);
    spdEntryNoisy->SetOwner(kTRUE);
    for(UInt_t module=0; module<240; module++){
      AliITSCalibrationSPD* calObj = new AliITSCalibrationSPD();
      spdEntryNoisy->Add(calObj);
    }
    // Add noisy from the copied FXS files
    AliITSOnlineCalibrationSPDhandler* handler = new AliITSOnlineCalibrationSPDhandler();
    TString fileLoc = ".";
    handler->SetFileLocation(fileLoc.Data());
    handler->ReadNoisyFromFiles();
    for (Int_t module=0; module<240; module++) {
      ((AliITSCalibrationSPD*) spdEntryNoisy->At(module)) -> SetNrBad( handler->GetNrNoisy(module) );
      ((AliITSCalibrationSPD*) spdEntryNoisy->At(module)) -> SetBadList( handler->GetNoisyArray(module) );
    }
    delete handler;
    // Store the new calibration objects in OCDB
    Log("Noisy lists (scan) will be stored...");
    AliCDBMetaData metaData;
    metaData.SetBeamPeriod(0);
    metaData.SetResponsible("Henrik Tydesjo");
    metaData.SetComment("Created by SPD PreProcessor");
    // validity for this run until infinity
    if (!Store("Calib", "SPDNoisy", spdEntryNoisy, &metaData, 0, kTRUE)) {
      Log("Failed to store calibration data.");
      return 1;
    }
    Log("Database updated.");
    delete spdEntryNoisy;
  }

  // Physics runs (online monitoring):
  else if (runType == "PHYSICS") {

    // Noisy pixels:
    // Read noisy from previous calibration
    AliCDBEntry* cdbEntry = GetFromOCDB("Calib", "SPDNoisy");
    TObjArray* spdEntryNoisy;
    if(cdbEntry) {
      spdEntryNoisy = (TObjArray*)cdbEntry->GetObject();
      if(!spdEntryNoisy) return 1;
    }
    else {
      Log("Old calibration not found in database. This is required for further processing.");
      return 1;
    }
    AliITSOnlineCalibrationSPDhandler* handOld = new AliITSOnlineCalibrationSPDhandler();
    handOld->ReadNoisyFromCalibObj(spdEntryNoisy);
    // Retrieve and unpack tared calibration files from FXS
    TString idN = "SPD_phys_noisy";
    TList* listN = GetFileSources(kDAQ,idN.Data());
    if (listN) {
      UInt_t index = 0;
      while (listN->At(index)!=NULL) {
	TObjString* fileNameEntry = (TObjString*) listN->At(index);
	TString fileName = GetFile(kDAQ, idN.Data(), fileNameEntry->GetString().Data());
	if (fileName.IsNull()) {
	  Log(Form("GetFile failed to retrieve file %s.",fileNameEntry->GetString().Data()));
	  return 1;
	}
	if (!RemoveIdFromList(idN.Data())) {
	  Log(Form("Warning: Retrieved file with id %s, that was not in the id list!",idN.Data()));
	}
	TString command = Form("tar -xf %s",fileName.Data());
	gSystem->Exec(command.Data());
	index++;
      }
    }
    AliITSOnlineCalibrationSPDhandler* handNew = new AliITSOnlineCalibrationSPDhandler();
    handNew->SetFileLocation(".");
    handNew->ReadNoisyFromFiles();
    // add the new list to the old one
    UInt_t nrNewNoisy = handOld->AddNoisyFrom(handNew);
    // If new noisy pixels were found: Update calibration objects
    if (nrNewNoisy>0) {
      for (Int_t module=0; module<240; module++) {
	((AliITSCalibrationSPD*) spdEntryNoisy->At(module)) -> SetNrBad( handOld->GetNrNoisy(module) );
	((AliITSCalibrationSPD*) spdEntryNoisy->At(module)) -> SetBadList( handOld->GetNoisyArray(module) );
      }
      // Store the new calibration objects in OCDB
      Log("Noisy lists (phys) will be stored...");
      AliCDBMetaData metaData;
      metaData.SetBeamPeriod(0);
      metaData.SetResponsible("Henrik Tydesjo");
      metaData.SetComment("Created by SPD PreProcessor");  
      // validity for this run only
      if (!Store("Calib", "SPDNoisy", spdEntryNoisy, &metaData, 0, kFALSE)) {
	Log("Failed to store calibration data.");
	return 1;
      }
      Log("Database updated.");
    }
    delete handNew;

    // Dead pixels:
    // Retrieve and unpack tared calibration files from FXS
    TString idD = "SPD_phys_dead";
    TList* listD = GetFileSources(kDAQ,idD.Data());
    UInt_t nrPhysDeadFiles = 0;
    if (listD) {
      UInt_t index = 0;
      while (listD->At(index)!=NULL) {
	TObjString* fileNameEntry = (TObjString*) listD->At(index);
	TString fileName = GetFile(kDAQ, idD.Data(), fileNameEntry->GetString().Data());
	if (fileName.IsNull()) {
	  Log(Form("GetFile failed to retrieve file %s.",fileNameEntry->GetString().Data()));
	  return 1;
	}
	nrPhysDeadFiles++;
	if (!RemoveIdFromList("SPD_phys_dead")) {
	  Log(Form("Warning: Retrieved file with id %s, that was not in the id list!",idD.Data()));
	}
	TString command = Form("tar -xf %s",fileName.Data());
	gSystem->Exec(command.Data());
	index++;
      }
    }
    if (nrPhysDeadFiles==0) {
      Log(Form("Could not find files with id %s. Should be present for each run.",idD.Data()));
      return 1;
    }
    // Create new database entries
    TObjArray* spdEntryDead = new TObjArray(240);
    spdEntryDead->SetOwner(kTRUE);
    for(UInt_t module=0; module<240; module++){
      AliITSCalibrationSPD* calObj = new AliITSCalibrationSPD();
      spdEntryDead->Add(calObj);
    }
    // Add dead from the copied FXS files
    handOld->SetFileLocation(".");
    handOld->ReadDeadFromFiles();
    for (Int_t module=0; module<240; module++) {
      ((AliITSCalibrationSPD*) spdEntryDead->At(module)) -> SetNrBad( handOld->GetNrDead(module) );
      ((AliITSCalibrationSPD*) spdEntryDead->At(module)) -> SetBadList( handOld->GetDeadArray(module) );
    }
    delete handOld;
    // Store the new calibration objects in OCDB
    Log("Dead lists (phys) will be stored...");
    AliCDBMetaData metaData;
    metaData.SetBeamPeriod(0);
    metaData.SetResponsible("Henrik Tydesjo");
    metaData.SetComment("Created by SPD PreProcessor");
    // validity for this run only
    if (!Store("Calib", "SPDDead", spdEntryDead, &metaData, 0, kFALSE)) {
      Log("Failed to store calibration data.");
      return 1;
    }
    Log("Database updated.");
    delete spdEntryDead;

  }


  // check that there are no ids left in the list:
  if (fIdList.First()!=NULL) {
    TString logMessage = "";
    TListIter *iter = new TListIter(&fIdList);
    while (TObjString *st = (TObjString*)iter->Next()) {
      logMessage.Append(st->GetString());
      logMessage.Append(" ");
    }
    delete iter;
    Log(Form("Files with the following ids were never retrieved: %s.",logMessage.Data()));
    return 1;
  }

  fIdList.Clear();

  return 0; // 0 means success

}
//_________________________________________________________________________________________
Bool_t AliITSPreprocessorSPD::RemoveIdFromList(const Char_t *id) {
  // removes id from the list of ids
  Bool_t found = kFALSE;
  TListIter *iter = new TListIter(&fIdList);
  while (TObjString *st = (TObjString*)iter->Next()) {
    if (st->GetString().CompareTo(id)==0) {
      fIdList.Remove(st);
      found = kTRUE;
      break;
    }
  }
  delete iter;
  return found;
}
//_________________________________________________________________________________________
Bool_t AliITSPreprocessorSPD::StoreRefFromTarForId(const Char_t *id) {
  // store reference files from tar file for the id given (this is just to not duplicate code)
  TList* list = GetFileSources(kDAQ,id);
  if (list) {
    UInt_t index = 0;
    while (list->At(index)!=NULL) {
      TObjString* fileNameEntry = (TObjString*) list->At(index);
      TString fileName = GetFile(kDAQ, id, fileNameEntry->GetString().Data());
      if (fileName.IsNull()) {
	Log(Form("GetFile failed to retrieve file %s.",fileNameEntry->GetString().Data()));
	return kFALSE;
      }
      if (!RemoveIdFromList(id)) {
	Log(Form("Warning: Retrieved file with id %s, that was not in the id list!",id));
      }
      // get the file names from the tar file
      //      TString pwd = gSystem->pwd();
      //      TString tempFileName = Form("%s/tempTar.txt",pwd.Data());
      TString tempFileName = "tempTar.txt";
      TString command = Form("tar -tf %s > %s",fileName.Data(),tempFileName.Data());
      gSystem->Exec(command.Data());
      TList fList;
      ifstream tempFile;
      tempFile.open(tempFileName.Data(), ifstream::in);
      if (tempFile.fail()) {
	Log(Form("Could not open file (%s) for reading.",tempFileName.Data()));
	return kFALSE;
      }
      else {
	while(1) {
	  Char_t fileN[100];
	  tempFile >> fileN;
	  if (tempFile.eof()) break;
	  fList.AddLast(new TObjString(fileN));
	}
      }
      // close and remove temp file
      tempFile.close();
      command = Form("rm -f %s",tempFileName.Data());
      gSystem->Exec(command.Data());
      // unpack
      command = Form("tar -xf %s",fileName.Data());
      gSystem->Exec(command.Data());
      // store each file
      UInt_t index2 = 0;
      while (fList.At(index2)!=NULL) {
	TString fileName = ((TObjString*)fList.At(index2))->GetString();
	// get eq id
	TString eqStr = fileName.Data();
	UInt_t len = eqStr.Length();
	eqStr.Replace(0,len-7,"",0);
	eqStr.ReplaceAll("_",1,"",0);
	eqStr.ReplaceAll(".root",5,"",0);
	Int_t eqId = eqStr.Atoi();
	if (eqId>=0 && eqId<20) {
	  TString refCAT = Form("%s_eq_%d",id,eqId);
	  if (!StoreReferenceFile(fileName.Data(),refCAT.Data())) {
	    Log(Form("Failed to store reference file %s.",fileName.Data()));
	    return kFALSE;
	  }
	}
	else {
	  Log(Form("Eq ID %d out of bounds for file %s",eqId,fileName.Data()));
	  fList.Clear();
	  return kFALSE;
	}
	index2++;
      }
      fList.Clear();
      index++;
    }
  }
  return kTRUE;
}
