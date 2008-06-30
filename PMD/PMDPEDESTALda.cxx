/*
PMD DA for online calibration

contact: basanta@phy.iitb.ac.in
Link:
Reference Run:/afs/cern.ch/user/b/bnandi/public/pedestaldata/run37820.raw
Run Type: STANDALONE
DA Type: LDC
Number of events needed: 1000
Input Files: 
Output Files: pmd_ped.root, to be exported to the DAQ FXS, pedestal230*.ped
Trigger types used: PHYSICS_EVENT

*/
extern "C" {
#include <daqDA.h>
}

#include "event.h"
#include "monitor.h"
//#include "daqDA.h"

#include <Riostream.h>
#include <stdio.h>
#include <stdlib.h>

//AliRoot
#include "AliRawReaderDate.h"
#include "AliPMDCalibPedestal.h"

//ROOT
#include "TFile.h"
#include "TH1F.h"
#include "TBenchmark.h"
#include "TTree.h"
#include "TROOT.h"
#include "TPluginManager.h"



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

    
    AliPMDCalibPedestal calibped;

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
	      //if(iev%100 == 0)printf(" event number = %i \n",iev);

		AliRawReader *rawReader = new AliRawReaderDate((void*)event);
		TObjArray *pmdddlcont = new TObjArray();
		calibped.ProcessEvent(rawReader,pmdddlcont);
		
		delete pmdddlcont;
		pmdddlcont = 0;
		delete rawReader;
		rawReader = 0x0;
	}
	
	/* free resources */
	free(event);
	
	/* exit when last event received, no need to wait for TERM signal */

    }

    printf(" Total number of events processed = %i \n",iev);

    ped = new TTree("ped","PMD Pedestal tree");
    
    if (eventT==END_OF_RUN) {
      printf("EOR event detected\n");
      calibped.Analyse(ped);
    }
    
    TFile * pedRun = new TFile ("PMD_PED.root","RECREATE"); 
    ped->Write();
    pedRun->Close();

    delete ped;
    ped = 0;


/* store the result file on FES */
 
    status = daqDA_FES_storeFile("PMD_PED.root","pedestal");
    if (status) {
	status = -2;
    }

    return status;
}
