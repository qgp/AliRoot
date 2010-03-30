/*

TOF DA for online calibration

Contact: Chiara.Zampolli@bo.infn.it
         Roberto.Preghenella@bo.infn.it

Run Type: PHYSICS
DA Type: MON
Number of events needed:
Input Files: no input
Output Files: TOFdaHits.root
Event types used: PHYSICS_EVENT

*/

#define FILE_HITS "TOFdaHits.root"

// DATE
#include "event.h"
#include "monitor.h"
#include "daqDA.h"

#include <stdio.h>
#include <stdlib.h>

//AliRoot
#include "TROOT.h"
#include "AliTOFRawStream.h"
#include "AliRawReaderDate.h"
#include "AliRawReader.h"
#include "AliDAQ.h"
#include "AliTOFHitData.h"
#include "AliTOFHitDataBuffer.h"
#include "AliTOFDaConfigHandler.h"
#include "AliTOFHitField.h"
#include "AliTOFcalibHisto.h"
#include "AliLog.h"

//ROOT
#include "TFile.h"
#include "TKey.h"
#include "TH2S.h"
#include "TObject.h"
#include "TMath.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TPluginManager.h"
#include "TSAXParser.h"
#include "TTree.h"

/* Main routine
      Arguments: 
      1- monitoring data source
*/
int 
main(int argc, char **argv) 
{
  
  /* magic line from Rene */
  gROOT->GetPluginManager()->AddHandler("TVirtualStreamerInfo",
					"*",
					"TStreamerInfo",
					"RIO",
					"TStreamerInfo()");
  

  /* log start of process */
  printf("TOF DA started\n");  
  
  /* check that we got some arguments = list of files */
  if (argc!=2) {
    printf("Wrong number of arguments\n");
    return -1;
  }

  /*
   * CONFIG
   */
  
  /* retrieve config file */
  int getConfigFile = daqDA_DB_getFile("TOFPhysicsConfig.xml","TOFPhysicsConfig.xml");
  if (getConfigFile != 0){
    printf("Failed to retrieve config file from DB! returning...\n");
    return -1;
  }
  /* parse config file */
  AliTOFDaConfigHandler* tofHandler = new AliTOFDaConfigHandler();
  TSAXParser *parser = new TSAXParser();
  parser->ConnectToHandler("AliTOFDaConfigHandler", tofHandler);  
  if (parser->ParseFile("./TOFPhysicsConfig.xml") != 0) {
    printf("Failed parsing config file! retunring... \n");
    return -1;
  }
  /* setup config params */
  Int_t meanMultiplicity = tofHandler->GetMeanMultiplicity(); /* average expected TOF multiplicity */
  Int_t maxHits = tofHandler->GetMaxHits(); /* max number of hits to be collected */
  printf("current settings:\n");
  printf(" - meanMultiplicity = %d\n", meanMultiplicity);
  printf(" - maxHits          = %d\n", maxHits);
  /* constants */
  const Int_t nChannels = 157248;
  Int_t noiseCheckTrigger = 10; /* first noise check after 10 events */
	 Float_t meanChannelRate = (Float_t)meanMultiplicity / (Float_t)nChannels; /* average expected channel rate (hits/event) */
  Float_t noiseThreshold = 10. * meanChannelRate; /* noise threshold (hits/event) */
  Int_t minNoiseHits = 10; /* min number of channel hits to check noise */
  /* counters and flags */
  Int_t nEvents, totHits;
  Int_t nChHits[nChannels];
  Bool_t noiseFlag[nChannels];
  /* variables */
  Int_t nhits, ddl, slot, trm, chain, tdc, channel, indexEO, index, timebin, totbin, deltaBC, l0l1latency;
  Float_t noiseHitThreshold;

  /*
   * INIT 
   */

  /* init counters and flags */
  nEvents = 0;
  totHits = 0;
  for (Int_t ich = 0; ich < nChannels; ich++) {
    nChHits[ich] = 0;
    noiseFlag[ich] = 0;
  }

  /* TOF raw data handling */
  AliTOFRawStream *rawStream = new AliTOFRawStream();
  AliTOFHitDataBuffer *pdb = NULL;
  AliTOFHitData *hit = NULL;
  /* calib histo */
  AliTOFcalibHisto calibHisto;
  calibHisto.LoadCalibHisto();
  
  /* open output file */
  TFile *fileOut = new TFile(FILE_HITS, "RECREATE"); 
  /* create hit field data structure */
  AliTOFHitField *hitField = new AliTOFHitField();
  /* create temporary tree */
  TTree *tempTree = new TTree("tempTree", "temporary tree");
  tempTree->Branch("hit", "AliTOFHitField", &hitField);
  /* create output tree */
  TTree *outTree = new TTree("hitTree", "hit tree");
  outTree->Branch("hit", "AliTOFHitField", &hitField);

  /*
   * ONLINE MONITOR
   */

  AliLog::SetGlobalLogLevel(AliLog::kFatal);
  struct eventHeaderStruct *event;
  int ret;
  /* define data source : this is argument 1 */  
  ret = monitorSetDataSource(argv[1]);
  if (ret != 0) {
    printf("monitorSetDataSource() failed : %s\n",monitorDecodeError(ret));
    return -1;
  }
  /* declare monitoring program */
  ret = monitorDeclareMp("tofDA");
  if (ret != 0) {
    printf("monitorDeclareMp() failed : %s\n",monitorDecodeError(ret));
    return -1;
  }
  /* define wait event timeout - 1s max */
  monitorSetNowait();
  monitorSetNoWaitNetworkTimeout(1000);

  /* loop over events */
  while (1) {
    
    /* check shutdown condition */
    if (daqDA_checkShutdown()) break;
    
    /*
     * NOISE CHECK
     */

    /* check number of events and check noise */
    if (nEvents >= noiseCheckTrigger || totHits >= maxHits) {
      noiseHitThreshold = noiseThreshold * nEvents;
      printf("noise check triggered after %d events: threshold is %f hits\n", nEvents, noiseHitThreshold);
      /* loop over all channels */
      for (Int_t ich = 0; ich < nChannels; ich++) {
	/* check */
	if (nChHits[ich] < minNoiseHits || noiseFlag[ich] || nChHits[ich] < noiseHitThreshold) continue;
	printf("channel %d tagged as noisy (%d hits): disabled\n", ich, nChHits[ich]);
	noiseFlag[ich] = kTRUE;
	totHits -= nChHits[ich];
      } /* end of loop over all channels */
      /* set new noise check trigger value */
      noiseCheckTrigger *= 10;
    } /* end of noise check */    

    /* break loop when maximum number of hits exceeded */
    if (totHits >= maxHits) {
      printf("maximum number of hits exceeded (%d): stop hit collection\n", maxHits);
      break;
    }
    
    /*
     * GET EVENT
     */

    /* get next event (blocking call until timeout) */
    ret = monitorGetEventDynamic((void **)&event);
    if (ret == MON_ERR_EOF) {
      printf ("End of File detected\n");
      break; /* end of monitoring file has been reached */
    }
    if (ret != 0) {
      printf("monitorGetEventDynamic() failed : %s\n",monitorDecodeError(ret));
      break;
    }
    /* retry if got no event */
    if (event==NULL) continue;
    /* check event type */
    if (event->eventType != PHYSICS_EVENT) continue;
    /* increment number of events */
    nEvents++;
    
    /*
     * DECODE EVENT
     */

    /* create raw reader */
    AliRawReader *rawReader = new AliRawReaderDate((void *)event);
    /* setup raw stream */
    rawStream->SetRawReader(rawReader);
    /* reset buffers */
    rawStream->ResetBuffers();
    /* decode */
    rawStream->DecodeDDL(0, AliDAQ::NumberOfDdls("TOF") - 1, 0);

    /*
     * HIT MANIPULATION
     */

    /* loop over DDLs */
    for (Int_t iddl = 0; iddl < AliDAQ::NumberOfDdls("TOF"); iddl++) {
      /* get packed-data buffer */
      pdb = rawStream->GetPackedDataBuffer(iddl);
      nhits = pdb->GetEntries();
      /* loop over hits in buffer */
      for (Int_t ihit = 0; ihit < nhits; ihit++) {
	/* get hit */
	hit = pdb->GetHit(ihit);
	/* get channel info */
	ddl = iddl;
	slot = hit->GetSlotID();
	trm = slot - 3;
	chain = hit->GetChain();
	tdc = hit->GetTDC();
	channel = hit->GetChan();
	indexEO = calibHisto.GetIndexEO(ddl, trm, chain, tdc, channel);
	index = calibHisto.GetCalibMap(AliTOFcalibHisto::kIndex, indexEO);
	if (index < 0) continue;
	/* check noise flag */
	if (noiseFlag[index]) continue;
	/* increment number of channel hits and total hits */
	nChHits[index]++;
	totHits++;
	/* get signal info */
	timebin = hit->GetTimeBin();
	totbin = hit->GetTOTBin();
	deltaBC = hit->GetDeltaBunchID();
	l0l1latency = hit->GetL0L1Latency();
	/* set hit field data */
	hitField->SetIndex(index);
	hitField->SetTimeBin(timebin);
	hitField->SetTOTBin(totbin);
	hitField->SetDeltaBC(deltaBC);
	hitField->SetL0L1Latency(l0l1latency);
	/* fill temp tree */
	tempTree->Fill();
      } /* end of loop over hits in buffer */
    } /* end of loop over DDLs */
    
    /* delete raw reader */
    delete rawReader;
    
  } /* end of loop over events */

  /* final noise check */
  noiseHitThreshold = noiseThreshold * nEvents;
  printf("final noise check after %d events: threshold is %f hits\n", nEvents, noiseHitThreshold);
  /* loop over all channels */
  for (Int_t ich = 0; ich < nChannels; ich++) {
    /* check */
    if (nChHits[ich] < minNoiseHits || noiseFlag[ich] || nChHits[ich] < noiseHitThreshold) continue;
    printf("channel %d tagged as noisy (%d hits): disabled\n", ich, nChHits[ich]);
    noiseFlag[ich] = kTRUE;
    totHits -= nChHits[ich];
  } /* end of loop over all channels */
  
  /* copy hits into output tree from temp tree */
  printf("copy hits from temporary tree into output tree\n");
  printf("temporary tree contains %d hits\n", (Int_t)tempTree->GetEntries());
  for (Int_t ihit = 0; ihit < tempTree->GetEntries(); ihit++) {
    /* get entry */
    tempTree->GetEntry(ihit);
    /* check noise flag */
    if (noiseFlag[hitField->GetIndex()]) continue;
    /* fill output tree */
    outTree->Fill();
  } /* end of copy hits into output tree from temp tree */
  printf("output tree contains %d hits\n", (Int_t)outTree->GetEntries());
  
  /* write output tree on file */
  fileOut->cd();
  outTree->Write();
  fileOut->Close();

  /* export file to FXS */
  if (daqDA_FES_storeFile(FILE_HITS, "HITS"))
    return -2;

  return 0;
}
