/*

HMPID DA for online calibration

Contact: Levente.Molnar@ba.infn.it, Giacomo.Volpe@ba.infn.it
Link: http://richpc1.ba.infn.it/~levente/Files4Public/ValidateHmpidDA/
Run Type: PEDESTAL -- but we select on the PHYSICS_EVENTS in th HMPIDda.cxx
DA Type: LDC
Number of events needed: 1000 events
Input Files: Raw pedestal file, no external config file
Output Files: 14 txt files including pedestal values
Trigger types used: PEDESTAL RUN (selecting on PHYSICS_EVENT)

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
#include "AliHMPIDRawStream.h"
#include "AliHMPIDCalib.h"
#include "AliRawReaderDate.h"
#include "AliBitPacking.h"
#include "TMath.h"

//ROOT
#include "TFile.h"
#include "TSystem.h"
#include "TKey.h"
#include "TH2S.h"
#include "TObject.h"
#include "TBenchmark.h"
#include "TMath.h"
#include "TRandom.h"


int main(int argc, char **argv){ 

  int status;

  /* log start of process */
  printf("HMPID DA program started\n");  

  /* check that we got some arguments = list of files */
  if (argc<2) {
    printf("Wrong number of arguments\n");
    return -1;
  }

  /* copy locally a file from daq detector config db
  status=daqDA_DB_getFile("myconfig","./myconfig.txt");
  if (status) {
    printf("Failed to get config file : %d\n",status);
    return -1;
  }
  and possibly use it */

  /* report progress */
  daqDA_progressReport(10);

  
  
  /* define wait event timeout - 1s max */
  monitorSetNowait();
  monitorSetNoWaitNetworkTimeout(1000);

  /* init the pedestal calculation */
  AliHMPIDCalib *pCal=new AliHMPIDCalib();
  
  /* init event counter */
  Int_t iEvtNcal=0;
  ULong_t runNum=0;
  
  int n;
  for (n=1;n<argc;n++) {

    status=monitorSetDataSource( argv[n] );
    if (status!=0) {
      printf("monitorSetDataSource() failed : %s\n",monitorDecodeError(status));
      return -1;
    }

    /* report progress */
    /* in this example, indexed on the number of files */
    daqDA_progressReport(10+80*n/argc);

    for(;;) { // infinite loop 
      
       /* check shutdown condition */
    if (daqDA_checkShutdown()) {break;}
    
      struct eventHeaderStruct *event;
      eventTypeType eventT;

      /* get next event */
      status=monitorGetEventDynamic((void **)&event);
      if (status==MON_ERR_EOF)                                              /* end of monitoring file has been reached */
      {
        printf("End of monitoring file has been reached! \n");
        break;
        }
      
       
      if (status!=0) {
        printf("monitorGetEventDynamic() failed : %s\n",monitorDecodeError(status));
	return -1;
      }

      /* retry if got no event */
      if (event==NULL) {
        //break;
        continue;
      }

      /* use event - here, just write event id to result file */
      eventT=event->eventType;

      if (eventT==PHYSICS_EVENT) {                                                //we use PHYSICS_EVENT for pedestal not CALIBRATION_EVENT
	
        runNum=(unsigned long)event->eventRunNb;                                  //assuming that only one run is processed at a time
             
	iEvtNcal++;
        
	AliRawReader *reader = new AliRawReaderDate((void*)event);
	AliHMPIDRawStream stream(reader);
	
        while(stream.Next())
          {
             for(Int_t iPad=0;iPad<stream.GetNPads();iPad++) {
             pCal->FillPedestal(stream.GetPadArray()[iPad],stream.GetChargeArray()[iPad]);
           }
           
           for(Int_t iddl=0;iddl<AliHMPIDRawStream::kNDDL;iddl++){                                         
              for(Int_t ierr=0; ierr < AliHMPIDRawStream::kSumErr; ierr++) {
                 pCal->FillErrors(iddl,ierr,stream.GetErrors(iddl,ierr));
               }
            }
          
        }//Next()    
        stream.Delete();            

      }// if CALIBRATION_EVENT

      /* exit when last event received, no need to wait for TERM signal */
      if (eventT==END_OF_RUN) {
	printf("EOR event detected\n");
	break;    
    
      } // events loop   

      free(event);
    }

  }//arg

  /* write report */
  printf("HMPID DA processed RUN #%s, with %d calibration events\n",getenv("DATE_RUN_NUMBER"),iEvtNcal);

  if (!iEvtNcal) {
    printf("No calibration events have been read. Exiting\n");
    return -1;
  }

  /* report progress */
  daqDA_progressReport(90);

   
  for(Int_t nDDL=0; nDDL < AliHMPIDCalib::kNDDL; nDDL++) {
    
    /* Calculate pedestal for the given ddl, if there is no ddl go t next */
    if(!pCal->CalcPedestal(runNum,nDDL,Form("./HmpidPedDdl%02i.txt",nDDL),iEvtNcal)) continue;
    if(!pCal->WriteErrors(runNum,nDDL,Form("./HmpidErrorsDdl%02i.txt",nDDL),iEvtNcal)) continue;
    
    /* store the result file on FES */
    /*
    status=daqDA_FES_storeFile(Form("./HmpidPedDdl%02i.txt",nDDL),Form("HMPID_DA_Pedestals_ddl=%02i",nDDL));
    if (status) {
      printf("Failed to export file : %d\n",status);
      return -1;
    }
    */
    status=daqDA_FES_storeFile(Form("./HmpidPedDdl%02i.txt",nDDL),Form("HMPID_DA_Pedestals_ddl=%02i",nDDL));
    if (status) { printf("Failed to export file : %d\n",status); }
    status=daqDA_FES_storeFile(Form("./HmpidErrorsDdl%02i.txt",nDDL),Form("HMPID_DA_Errors_ddl=%02i",nDDL));
    if (status) { printf("Failed to export file : %d\n",status); }
    
  }//nDDL
  delete pCal;
  if (status) return -1;
  
  /* report progress */
  daqDA_progressReport(100);
  

  return status;
}
