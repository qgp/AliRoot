// Author:  Mihai Niculesu 2013

/**************************************************************************
 * Copyright(c) 1998-2013, ALICE Experiment at CERN, all rights reserved. *)
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

#include "AliOnlineReconstruction.h"
#include "AliOnlineReconstructionUtil.h"
#include "AliStorageEventManager.h"

#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TTimeStamp.h>

#include <signal.h>
#include <iostream>

using namespace std;

bool gQuit = false;
void GotSignal(int){gQuit = true;}

AliOnlineReconstruction::AliOnlineReconstruction(int run) :
  fRun(run),
  fDataSource(""),
  fSettings(0),
  fAliReco(new AliReconstruction()),
  fCDBmanager(AliCDBManager::Instance())
{
  // make sure that destructor is called when kill signal comes
  struct sigaction sa;
  memset(&sa,0,sizeof(sa));
  sa.sa_handler = GotSignal;
  sigfillset(&sa.sa_mask);
  sigaction(SIGINT,&sa,NULL);

  printf("CDB Lock is %s\n",AliCDBManager::Instance()->GetLock() ? "ON":"OFF");

  fSettings.ReadFile(AliOnlineReconstructionUtil::GetPathToServerConf(), kEnvUser);
  StartOfRun();
  cout<<"after startofrun"<<endl;
}

AliOnlineReconstruction::~AliOnlineReconstruction()
{
  cout<<"AliOnlineReconstruction -- destructor called...";
  if(fAliReco)
    {
      //	fAliReco->SlaveTerminate();
      //	fAliReco->Terminate(); 
      //	delete fAliReco;fAliReco=0;
    }
  if(fCDBmanager){fCDBmanager->Destroy();fCDBmanager=0;}
  cout<<"OK"<<endl;
}

void AliOnlineReconstruction::StartOfRun()
{
  if(strcmp("local",fSettings.GetValue("data.source", DEFAULT_DATA_SOURCE))==0)
    {
      cout<<"Starting Reco for run "<<fRun<<endl;
      fDataSource = Form("mem://%s/run%d", gSystem->Getenv("ONLINERECO_RAWFILES_DIR"), fRun);
    }
  else if(strcmp(fSettings.GetValue("data.source", DEFAULT_DATA_SOURCE),"run")==0)
    {
      cout<<"Starting Reco for GDCs active in current run:"<<fRun<<endl;
      fDataSource = fSettings.GetValue("data.online.source", DEFAULT_DATA_ONLINE_SOURCE);
    }
  else{cout<<"\n\nWrong data source. Quitting\n\n"<<endl;}

  TString recoBaseDir = fSettings.GetValue("server.saveRecoDir",DEFAULT_SERVER_SAVE_RECO_DIR);
  cout<<"Reco base dir:"<<recoBaseDir<<endl;

  // Create directories and logfile
  TString logFile = Form("%s/log/run%d.log",recoBaseDir.Data(),fRun);
  Info("DoStart","Reconstruction log will be written to %s",logFile.Data());
  if( gSystem->RedirectOutput(logFile.Data(),"w")!=0)
    {
      printf(Form("AliRecoServer::StartReconstruction [] Error while trying to redirect output to [%s]. Exiting...", logFile.Data()) );
      return;
    }
  gSystem->cd(recoBaseDir.Data());
  // gSystem->cd("/");
  cout<<"\n\nRetriving GRP\n\n"<<endl;
  TString gdcs;
  if (RetrieveGRP(gdcs) <= 0 || gdcs.IsNull()){return;}
  //gSystem->cd(recoBaseDir.Data());
  gSystem->Exec(Form("rm -fr run%d;mkdir run%d",fRun,fRun));
  gSystem->cd(Form("run%d",fRun));

  SetupReco();
  cout<<"\n\nStarting reconstruction loop\n\n"<<endl;
  ReconstructionLoop();
}

int AliOnlineReconstruction::RetrieveGRP(TString &gdc)
{
	// Retrieve GRP entry for given run from aldaqdb.
	TString dbHost = fSettings.GetValue("logbook.host", DEFAULT_LOGBOOK_HOST);
	Int_t   dbPort =  fSettings.GetValue("logbook.port", DEFAULT_LOGBOOK_PORT);
	TString dbName =  fSettings.GetValue("logbook.db", DEFAULT_LOGBOOK_DB);
	TString user =  fSettings.GetValue("logbook.user", DEFAULT_LOGBOOK_USER);
	TString password = fSettings.GetValue("logbook.pass", DEFAULT_LOGBOOK_PASS);
	TString cdbPath = fSettings.GetValue("cdb.defaultStorage", DEFAULT_CDB_STORAGE);

	//	cdbPath = gSystem->pwd();
	cout<<"CDB path for GRP:"<<cdbPath<<endl;

	Int_t ret=AliGRPPreprocessor::ReceivePromptRecoParameters(fRun, dbHost.Data(),
								  dbPort, dbName.Data(),
								  user.Data(), password.Data(),
								  Form("%s",cdbPath.Data()),
								  gdc);

	if(ret>0) Info("RetrieveGRP","Last run of the same type is: %d",ret);
	else if(ret==0) Warning("RetrieveGRP","No previous run of the same type found");
	else if(ret<0) Error("Retrieve","Error code while retrieving GRP parameters returned: %d",ret);
	return(ret);
}

void AliOnlineReconstruction::SetupReco()
{
	printf(Form("=========================[local://%s/]===========\n",gSystem->pwd()));

	/* Settings CDB */
	cout<<"\n\nSetting CDB manager parameters\n\n"<<endl;
	fCDBmanager->SetRun(fRun);
	fCDBmanager->SetDefaultStorage(fSettings.GetValue("cdb.defaultStorage", DEFAULT_CDB_STORAGE));
	fCDBmanager->SetSpecificStorage(fSettings.GetValue( "cdb.specificStoragePath1", DEFAULT_CDB_SPEC_STORAGE_PATH1),  
				    fSettings.GetValue( "cdb.specificStorageValue1", DEFAULT_CDB_SPEC_STORAGE_VALUE1));
	fCDBmanager->SetSpecificStorage(fSettings.GetValue( "cdb.specificStoragePath2", DEFAULT_CDB_SPEC_STORAGE_PATH2),  
				    fSettings.GetValue( "cdb.specificStorageValue2", DEFAULT_CDB_SPEC_STORAGE_VALUE2));
	fCDBmanager->SetSpecificStorage(fSettings.GetValue( "cdb.specificStoragePath3", DEFAULT_CDB_SPEC_STORAGE_PATH3),  
				    fSettings.GetValue( "cdb.specificStorageValue3", DEFAULT_CDB_SPEC_STORAGE_VALUE3));
	/* Reconstruction settings */  

	// QA options
	cout<<"\n\nSetting AliReconstruction parameters\n\n"<<endl;
	fAliReco->SetRunQA(fSettings.GetValue("qa.runDetectors",DEFAULT_QA_RUN));
	fAliReco->SetRunGlobalQA(fSettings.GetValue("qa.runGlobal",DEFAULT_QA_RUN_GLOBAL));
	fAliReco->SetQARefDefaultStorage(fSettings.GetValue("qa.defaultStorage",DEFAULT_QAREF_STORAGE)) ;
	fAliReco->SetRunPlaneEff(fSettings.GetValue("reco.runPlaneEff",DEFAULT_RECO_RUN_PLANE_EFF));

	// AliReconstruction settings
	fAliReco->SetWriteESDfriend(fSettings.GetValue( "reco.writeESDfriend",DEFAULT_RECO_WRITE_ESDF));
	fAliReco->SetWriteAlignmentData(fSettings.GetValue( "reco.writeAlignment",DEFAULT_RECO_WRITE_ALIGN));
	fAliReco->SetInput(fDataSource.Data()); // reconstruct data from this input
	fAliReco->SetRunReconstruction(fSettings.GetValue( "reco.detectors", DEFAULT_RECO_DETECTORS));
	fAliReco->SetUseTrackingErrorsForAlignment("ITS"); //-- !should be set from conf file!
	fAliReco->SetCleanESD(fSettings.GetValue( "reco.cleanESD",DEFAULT_RECO_CLEAN_ESD));

	// init reco for given run
 	fAliReco->InitRun(fDataSource.Data());
}

void AliOnlineReconstruction::ReconstructionLoop()
{
  cout<<"\n\nCreating sockets\n\n"<<endl;
	AliStorageEventManager *eventManager = AliStorageEventManager::GetEventManagerInstance();
	eventManager->CreateSocket(EVENTS_SERVER_PUB);
	eventManager->CreateSocket(XML_PUB);
	eventManager->CreateSocket(ITS_POINTS_PUB);

	cout<<"\n\nStarting reconstruction\n\n"<<endl;
	fAliReco->Begin(NULL);
	if (fAliReco->GetAbort() != TSelector::kContinue) return;
	fAliReco->SlaveBegin(NULL);
	if (fAliReco->GetAbort() != TSelector::kContinue) return;
	cout<<"\n\nStarting loop over events\n\n"<<endl;

	TString recoBaseDir = fSettings.GetValue("server.saveRecoDir",DEFAULT_SERVER_SAVE_RECO_DIR);


	//******* The loop over events
	Int_t iEvent = 0;
	AliESDEvent* event;
	struct recPointsStruct *files;
	//	while (fAliReco->HasNextEventAfter(iEvent) && !gQuit)
	while (!gQuit)
	{
	  if(fAliReco->HasNextEventAfter(iEvent))
	    {
	      // remove files for previous event: (needed to send RecPoints:
	      /*
	      gSystem->cd(recoBaseDir.Data());
	      gSystem->Exec(Form("rm -fr run%d;mkdir run%d",fRun,fRun));
	      gSystem->cd(Form("run%d",fRun));
	      */
	      
	      if (!fAliReco->HasEnoughResources(iEvent)) break;
	      cout<<"\n\nProcessing event:"<<iEvent<<endl<<endl;
	      Bool_t status = fAliReco->ProcessEvent(iEvent);
      
	      if (status){
		event = fAliReco->GetESDEvent();
		eventManager->Send(event,EVENTS_SERVER_PUB);
		eventManager->SendAsXml(event,XML_PUB);

		// sending RecPoints:
		/*
		cout<<"loading file"<<endl;
		files->files[0] = TFile::Open("./ITS.RecPoints.root");
		files->files[1] = TFile::Open("./TOF.RecPoints.root");
		files->files[2] = TFile::Open("./galice.root");
		files->files[3] = NULL;
		files->files[4] = NULL;
		files->files[5] = NULL;
		files->files[6] = NULL;
		files->files[7] = NULL;
		files->files[8] = NULL;
		files->files[9] = NULL;


		cout<<"sending files"<<endl;
		eventManager->Send(files,ITS_POINTS_PUB);
		cout<<"files sent"<<endl;
	
		for(int i=0;i<10;i++)
		  {
		    if(files->files[i])
		      {
			files->files[i]->Close();
			delete files->files[i];files->files[i]=0;
			cout<<"file deleted"<<endl;
		      }
		  }
		*/

		//Saving ESD to file:
		/*
		  TFile *file = new TFile(Form("/local/storedFiles/AliESDs.root_%d",iEvent),"recreate");
		  TTree* tree= new TTree("esdTree", "esdTree");
		  event->WriteToTree(tree);
		  tree-> Fill();
		  tree->Write();
		  file->Close();
		*/
	      }
	      else{
		cout<<"Event server -- aborting"<<endl;
		fAliReco->Abort("ProcessEvent",TSelector::kAbortFile);
	      }
	      cout<<"clean"<<endl;
	      fAliReco->CleanProcessedEvent();
	      cout<<"iEvent++"<<endl;
	      iEvent++;
	    }
	  else
	    {
	      cout<<"No event after!"<<endl;
	    }
	}
	cout<<"after while"<<endl;
	//	fAliReco->SlaveTerminate();
	//if (fAliReco->GetAbort() != TSelector::kContinue) return;
	//fAliReco->Terminate();
	//if (fAliReco->GetAbort() != TSelector::kContinue) return; 
}
