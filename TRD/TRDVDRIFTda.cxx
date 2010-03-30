/*

Contact: r.bailhache@gsi.de
Link:
Reference run: 25909
Run Type: PHYSICS STANDALONE
DA Type: MON
Number of events needed: many
Input Files:  TRD raw files
Output Files: trdCalibrationv.root
Trigger types used: PHYSICS_EVENT

*/

#define RESULT_FILE  "trdVdrift.root"
#define FILE_ID "VDRIFT"


extern "C" {
#include <daqDA.h>
}

#include "event.h"
#include "monitor.h"
#include <stdio.h>
#include <stdlib.h>


//
//Root includes
//
#include <TObjArray.h>
#include <TString.h>
#include <TVectorF.h>
#include <TDirectory.h>
#include <TSystem.h>
#include <TFile.h>
#include "TROOT.h"
#include "TPluginManager.h"
//
//AliRoot includes
//
#include "AliRawReader.h"
#include "AliRawReaderDate.h"
#include "AliTRDrawStream.h"
#include "AliTRDrawStreamBase.h"


//
// TRD calibration algorithm includes
//
#include "AliTRDCalibraFillHisto.h"




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
  printf("TRD DA VDRIFT started\n");  


  /* init some counters */
  int neventsphysics=0;
  int neventstotal=0;

  //Instance of AliCDBManager: needed by AliTRDrawStream
  //AliCDBManager *man = AliCDBManager::Instance();
  //man->SetDefaultStorage("local://$ALICE_ROOT/OCDB");
  //man->SetRun(0);
  //Instance of AliTRDCalibraFillHisto
  AliTRDCalibraFillHisto *calibra      = AliTRDCalibraFillHisto::Instance();
  calibra->SetNumberClustersProcent(0.9);
  // everythings are okey for vdrift
  Bool_t passvdrift  = kTRUE;    // if timebin okey
  Int_t  nbvdrift    = 0;     // number of events with entries for vdrift

   // setting
  // AliTRDrawStream::SetNoDebug();
  AliTRDrawStream::DisableSkipData();
  AliTRDrawStream::SetNoErrorWarning();
  //AliTRDrawStream::SetSubtractBaseline(0); 
  //AliTRDrawStream::SetForceCleanDataOnly();
  //AliTRDrawStream::AllowCorruptedData();
  //AliTRDrawStream::DisableStackNumberChecker();
  //AliTRDrawStream::DisableStackLinkNumberChecker();
  //AliTRDrawStream::SetSkipCDH();
  //AliTRDrawStream::SetExtraWordsFix();
  //AliTRDrawStream::EnableDebugStream();
  //AliTRDrawStream::SetDumpHead(320);
  //AliTRDrawStream::SetDumpHead(80);

  
  /* main loop (infinite) */
  for(;;) {
  //for(Int_t k = 0; k < 20; k++) {
    struct eventHeaderStruct *event;
    eventTypeType eventT;

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

    if( ((Int_t)neventstotal)%1000 == 0 ) printf(" event number %d (physic event number %d) will be processed\n",(Int_t) neventstotal,(Int_t) neventsphysics);  

    /* use event - here, just write event id to result file */
    eventT=event->eventType;
    
    //
    // vdrift calibration: run only for physics events
    //
    if ((eventT==PHYSICS_EVENT) && (passvdrift)) {
      //if (eventT==PHYSICS_EVENT) {
      AliRawReader *rawReader = new AliRawReaderDate((void*)event);
      rawReader->Select("TRD");
      AliTRDrawStream *trdRawStream = new AliTRDrawStream((AliRawReader *) rawReader);
      Int_t result = calibra->ProcessEventDAQ((AliTRDrawStreamBase *) trdRawStream,(Bool_t)nbvdrift);
      if(!result) passvdrift = kFALSE;
      else nbvdrift += (Int_t) result/2;
             
      delete trdRawStream;
      delete rawReader;
    
    } 
   
    if(eventT==PHYSICS_EVENT){
 
      neventsphysics++;
     
    }
    
    neventstotal++;


    /* free resources */
    free(event);
    
    /* exit when last event received, no need to wait for TERM signal */
    if (eventT==END_OF_RUN) {
      printf("EOR event detected\n");
      break;
    }
  }


  /* report progress */
  printf("%d events processed and %d used\n",neventstotal,nbvdrift);
  
  //
  // Write a local file and put it on the FX 
  //
  TFile  *fileTRD     = new TFile(RESULT_FILE,"recreate");
 
  if((nbvdrift > 0) && passvdrift){
    //Double_t *stat = calibra->StatH((TH2 *)(calibra->GetPH2d()),1);
    //if((stat[6] < 0.20) && (stat[5] > 3000.0)) {
    // write the histogram in any case to see if problems
    calibra->GetPH2d()->Write();
    //}
  }
  fileTRD->Close();
  status=0;
  // Export the file in any case to see if problems
  if(daqDA_FES_storeFile(RESULT_FILE,FILE_ID)) status = -2;
  
  delete fileTRD;  

  return status;
}

