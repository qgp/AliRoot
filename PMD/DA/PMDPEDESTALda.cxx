/*
PMD DA for online calibration

Contact: basanta@iitb.ac.in, Satyajit.Jena@cern.ch
Link: https://twiki.cern.ch/twiki/bin/view/ALICE/DA
Reference Run:/afs/cern.ch/user/s/sjena/public/run83496.raw
Run Type: PEDESTAL
DA Type: LDC
Number of events needed: 1000
Input Files: PMD/Calib/Mapping/Run0_999999999_v0_s0.root
Output Files: pmd_ped.root, to be exported to the DAQ FXS, pedestal230*.ped
Trigger types used: PHYSICS_EVENT

*/
extern "C" {
#include <daqDA.h>
}

#include "event.h"
#include "monitor.h"

#include <Riostream.h>
#include <stdio.h>
#include <stdlib.h>

//AliRoot
#include "AliRawReaderDate.h"
#include "AliPMDCalibPedestal.h"
#include "AliLog.h"
#include "AliCDBManager.h"

//ROOT
#include "TFile.h"
#include "TH1F.h"
#include "TBenchmark.h"
#include "TTree.h"
#include "TROOT.h"
#include "TPluginManager.h"
#include "TSystem.h"

/* Main routine
      Arguments: 
      1- monitoring data source
*/
int main(int argc, char **argv) {
  
    /* magic line from Rene */
    gROOT->GetPluginManager()->AddHandler("TVirtualStreamerInfo",
					  "*",
					  "TStreamerInfo",
					  "RIO",
					  "TStreamerInfo()");

    
    AliPMDCalibPedestal *calibped = new AliPMDCalibPedestal();

    TTree *ped = NULL;

    //    TH1F::AddDirectory(0);
  
      
    // decoding the events
  
    int status;
    
    if (argc!=2) {
	printf("Wrong number of arguments\n");
	return -1;
    }

    
    /* define data source : this is argument 1 */  
    status=monitorSetDataSource( argv[1] );
    if (status!=0) {
	printf("monitorSetDataSource() failed : %s\n",monitorDecodeError(status));
	return -1;
    }
    
    /* declare monitoring program */
    status=monitorDeclareMp( __FILE__ );
    if (status!=0) {
	printf("monitorDeclareMp() failed : %s\n",monitorDecodeError(status));
	return -1;
    }
    
    /* define wait event timeout - 1s max */
    monitorSetNowait();
    monitorSetNoWaitNetworkTimeout(1000);
    
    /* log start of process */
    printf("PMD PED DA - started generating mean and rms of each channel\n");  
    
    /* init some counters */


    struct eventHeaderStruct *event;
    eventTypeType eventT = 0;
    Int_t iev=0;

    // Get run number

    int runNr = 0;
    if (getenv("DATE_RUN_NUMBER")==0) {
      printf("DATE_RUN_NUMBER not properly set.\n");
      //return -1;
      printf("Run Number is set to Zero \n");
    }
    else
      {
	runNr = atoi(getenv("DATE_RUN_NUMBER"));
      }

    if (gSystem->AccessPathName("localOCDB/PMD/Calib/Mapping",kFileExists))
      {
	if (gSystem->mkdir("localOCDB/PMD/Calib/Mapping",kTRUE) != 0)
	  {
	    printf("Failed to create directory: localOCDB/PMD/Calib/Mapping");
	    return -1;
	  }
      }
    status = daqDA_DB_getFile("PMD/Calib/Mapping/Run0_999999999_v0_s0.root","localOCDB/PMD/Calib/Mapping/Run0_999999999_v0_s0.root");
    if (status)
      {
	printf("Failed to get PMD-Mapping file (PMD/Calib/Mapping/Run0_999999999_v0_s0.root) from DAQdetDB, status=%d\n", status);
	return -1;
      }

    // Global initializations
    AliLog::SetGlobalLogLevel(AliLog::kError);
    AliCDBManager *man = AliCDBManager::Instance();
    man->SetDefaultStorage("local://localOCDB");
    man->SetRun(runNr);

    
    /* main loop (infinite) */
    for(;;) {
	
	/* check shutdown condition */
	if (daqDA_checkShutdown()) {break;}
	
	/* get next event (blocking call until timeout) */
	status=monitorGetEventDynamic((void **)&event);
	if (status==MON_ERR_EOF) {
	    printf ("End of File detected\n");
	    break; /* end of monitoring file has been reached */
	}
	
	if (status!=0) {
	    printf("monitorGetEventDynamic() failed : %s\n",monitorDecodeError(status));
	    break;
	}
	
	/* retry if got no event */
	if (event==NULL) {
	    continue;
	}
	
	iev++; 
	
	/* use event - here, just write event id to result file */
	eventT=event->eventType;
	switch (event->eventType){
	    
	    /* START OF RUN */
	    case START_OF_RUN:
		break;
		/* END START OF RUN */
		
		/* END OF RUN */
	    case END_OF_RUN:
		break;
		
	    case PHYSICS_EVENT:

		AliRawReader *rawReader = new AliRawReaderDate((void*)event);
		TObjArray *pmdddlcont = new TObjArray();
		calibped->ProcessEvent(rawReader,pmdddlcont);
		
		delete pmdddlcont;
		pmdddlcont = 0;
		delete rawReader;
		rawReader = 0x0;
	}
	
	/* free resources */
	free(event);
	
	/* exit when last event received, no need to wait for TERM signal */

	// either END_OF_RUN or 1000 events

	if (iev == 1000)
	  {
	    printf("-- 1000 pedestal events processed : terminating --\n");
	    printf("-- eventT is set to END_OF_RUN --\n");
	    eventT = END_OF_RUN;
	    break;
	  }

    }

    printf(" Total number of events processed = %i \n",iev);

    ped = new TTree("ped","PMD Pedestal tree");
    
    if (eventT==END_OF_RUN) {
      printf("EOR event detected\n");
      calibped->Analyse(ped);
    }
    
    TFile * pedRun = new TFile ("PMD_PED.root","RECREATE"); 
    ped->Write();
    pedRun->Close();

    delete ped;
    ped = 0;

    delete calibped;

/* store the pedestal file in database */

    status = daqDA_DB_storeFile("PMD_PED.root","PMD_PED.root");

    if (!status)
      {
	printf("--- PEDESTAL root FILE IS IN THE DAQ DATABASE\n");
      }
    else
      {
	printf("--- Storing the root file into the database failed\n");
      }

/* store the pedestal file on FES */
 
    status = daqDA_FES_storeFile("PMD_PED.root","PMD_PED.root");
    if (status) {
	status = -2;
    }




    return status;
}
