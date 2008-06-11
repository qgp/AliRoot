/*
PMD DA for online calibration

contact: basanta@phy.iitb.ac.in
Link:http://www.veccal.ernet.in/~pmd/
Reference run:
Run Type: PHYSICS
DA Type: MON
Number of events needed: 1 million for PB+PB, 200 milion for p+p
Input Files: 
Output Files: pmd_calib.root, to be exported to the DAQ FXS
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
#include "AliPMDCalibGain.h"

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

    AliPMDCalibGain calibgain;

    TTree *ic = NULL;

    //TH1F::AddDirectory(0);
  
    
    // decoding the events
    
    int status;

    if (argc!=2) {
	printf("Wrong number of arguments\n");
	return -1;
    }
    
    /* open result file */
    FILE *fp=NULL;
    fp=fopen("./result.txt","a");
    if (fp==NULL) {
	printf("Failed to open file\n");
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
    printf("DA example case2 monitoring program started\n");  
    
    /* init some counters */
    int nevents_physics=0;
    int nevents_total=0;
    
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
	nevents_total++;
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
		nevents_physics++;
		if(nevents_physics%100 == 0)printf("Physis Events = %d\n",nevents_physics);
		AliRawReader *rawReader = new AliRawReaderDate((void*)event);
		TObjArray *pmdddlcont = new TObjArray();
		calibgain.ProcessEvent(rawReader, pmdddlcont);

		delete pmdddlcont;
		pmdddlcont = 0x0;
		delete rawReader;
		rawReader = 0x0;
		
	}
       
	/* free resources */
	free(event);
	
    }

    /* exit when last event received, no need to wait for TERM signal */
    if (eventT==END_OF_RUN) {
      printf("EOR event detected\n");

      ic = new TTree("ic","PMD Gain tree");
      calibgain.Analyse(ic);
      //break;
    }
    
    //write the Run level file   
    TFile * fileRun = new TFile ("outPMDdaRun.root","RECREATE"); 
    TBenchmark *bench = new TBenchmark();
    bench->Start("PMD");
    bench->Stop("PMD");
    bench->Print("PMD");
    fileRun->Close();
    
    /* write report */
    fprintf(fp,"Run #%s, received %d physics events out of %d\n",getenv("DATE_RUN_NUMBER"),nevents_physics,nevents_total);
    

    TFile * gainRun = new TFile ("PMDGAINS.root","RECREATE"); 
    ic->Write();
    gainRun->Close();
    
    
    delete ic;
    ic = 0;

    /* close result file */
    fclose(fp);

    return status;
}
