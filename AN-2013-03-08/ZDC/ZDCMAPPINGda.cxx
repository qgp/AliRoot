/*

This program reads the DAQ data files passed as argument using the monitoring library.

The program reports about its processing progress.

Messages on stdout are exported to DAQ log system.

DA to write mapping for ADC modules and VME scaler

Contact: Chiara.Oppedisano@to.infn.it
Link: 
Run Type: PHYSICS
DA Type: MON
Number of events needed: at least 50% 
Input Files:  none
Output Files: ZDCChMapping.dat
Trigger Types Used: different trigger types are used

*/

#define MAPDATA_FILE  "ZDCChMapping.dat"
#define TDCDATA_FILE  "ZDCTDCCalib.dat"
#define TDCHISTO_FILE "ZDCTDCHisto.root"

#include <stdio.h>
#include <stdlib.h>
#include <Riostream.h>
#include <signal.h>

// DATE
#include <event.h>
#include <monitor.h>
#include <daqDA.h>

//ROOT
#include <TROOT.h>
#include <TPluginManager.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile.h>
#include <TF1.h>
#include <TFile.h>
#include <TFitter.h>
#include "TMinuitMinimizer.h"

//AliRoot
#include <AliRawReaderDate.h>
#include <AliRawEventHeaderBase.h>
#include <AliZDCRawStream.h>

int main(int argc, char **argv) {

  // needed for streamer application
  gROOT->GetPluginManager()->AddHandler("TVirtualStreamerInfo",
                                        "*",
                                        "TStreamerInfo",
                                        "RIO",
                                        "TStreamerInfo()"); 
  // needed for Minuit plugin
  gROOT->GetPluginManager()->AddHandler("ROOT::Math::Minimizer",
                                        "Minuit",
                                        "TMinuitMinimizer",
                                        "Minuit",
                                        "TMinuitMinimizer(const char*)");

  //TMinuitMinimizer m; 
  TVirtualFitter::SetDefaultFitter("Minuit");

  char *monitor_table[] = { "ALL", "no", "PHY", "yes", "SOD", "all", NULL };
  int err = monitorDeclareTable(monitor_table);
  if(err){
    printf("monitorDeclareTable() failed: %s\n", monitorDecodeError(err));
    return -1;
  } 
  
  int status=0, nphys=0;
  int const kNModules = 9;
  int const kNChannels = 24;
  int const kNScChannels = 32;  
  int const kZDCTDCGeo = 4;
  
  int itdc=0, iprevtdc=-1, ihittdc=0;
  float tdcData[6], tdcL0=-999.;	
  for(int ij=0; ij<6; ij++) tdcData[ij]=-999.;
  
  Int_t iMod=-1;
  Int_t modGeo[kNModules], modType[kNModules],modNCh[kNModules];
  for(Int_t kl=0; kl<kNModules; kl++){
     modGeo[kl]=modType[kl]=modNCh[kl]=0;
  }
  
  Int_t ich=0;
  Int_t adcMod[2*kNChannels], adcCh[2*kNChannels], sigCode[2*kNChannels];
  Int_t det[2*kNChannels], sec[2*kNChannels];
  for(Int_t y=0; y<2*kNChannels; y++){
    adcMod[y]=adcCh[y]=sigCode[y]=det[y]=sec[y]=0;
  }
  
  Int_t iScCh=0;
  Int_t scMod[kNScChannels], scCh[kNScChannels], scSigCode[kNScChannels];
  Int_t scDet[kNScChannels], scSec[kNScChannels];
  for(Int_t y=0; y<kNScChannels; y++){
    scMod[y]=scCh[y]=scSigCode[y]=scDet[y]=scSec[y]=0;
  }
      
  Int_t itdcCh=0;
  Int_t tdcMod[kNScChannels], tdcCh[kNScChannels], tdcSigCode[kNScChannels];
  Int_t tdcDet[kNScChannels], tdcSec[kNScChannels];
  for(Int_t y=0; y<kNScChannels; y++){
    tdcMod[y]=tdcCh[y]=tdcSigCode[y]=tdcDet[y]=tdcSec[y]=-1;
  }
 
  TH1F * hTDC[6]={0x0,0x0,0x0,0x0,0x0,0x0};
  for(int it=0; it<6; it++){
    if(it==0) hTDC[it] = new TH1F("TDCZEM1","TDC ZEM1",200, -200., 200.);
    else if(it==1) hTDC[it] = new TH1F("TDCZEM2","TDC ZEM2",200, -200., 200.);
    else if(it==2) hTDC[it] = new TH1F("TDCZNC", "TDC ZNC", 200, -200., 200.);
    else if(it==3) hTDC[it] = new TH1F("TDCZPC", "TDC ZPC", 200, -200., 200.);
    else if(it==4) hTDC[it] = new TH1F("TDCZNA", "TDC ZNA", 200, -200., 200.);
    else if(it==5) hTDC[it] = new TH1F("TDCZPA", "TDC ZPA", 200, -200., 200.);
  }
  
  /* log start of process */
  printf("\n ZDC MAPPING program started\n");  
  signal(SIGSEGV, SIG_DFL);

  /* check that we got some arguments = list of files */
  if(argc<2) {
    printf("Wrong number of arguments\n");
    return -1;
  }
  
  FILE *mapFile4Shuttle;

  /* read the data files */
  for(int n=1;n<argc;n++){
   
    status=monitorSetDataSource( argv[n] );
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
    monitorSetNowait();
    monitorSetNoWaitNetworkTimeout(1000);


    int iev = 0;
    //Bool_t sodRead = kFALSE;
    //while(!sodRead){
    
    /* loop on events (infinite) */
    for(;;) {
      
      if(nphys > 50000) break;
      
      struct eventHeaderStruct *event;
      eventTypeType eventT;
 
      /* check shutdown condition */
      if (daqDA_checkShutdown()) {break;}

      /* get next event */
      status=monitorGetEventDynamic((void **)&event);
      if(status==MON_ERR_EOF){
        printf ("End of File detected\n");
        break; /* end of monitoring file has been reached */
      }
      if(status!=0) {
        printf("monitorGetEventDynamic() failed : %s\n",monitorDecodeError(status));
        return -1;
      }

      /* retry if got no event */
      if(event==NULL) continue;
      
      // Initalize raw-data reading and decoding
      AliRawReader *reader = new AliRawReaderDate((void*)event);
      reader->Reset();
      reader->Select("ZDC");
      // --- Reading event header
      //UInt_t evtype = reader->GetType();
      //printf("\t ZDCMAPPINGda -> run # %d\n",reader->GetRunNumber());
      //
      AliZDCRawStream *rawStreamZDC = new AliZDCRawStream(reader);
        
	
      /* use event - here, just write event id to result file */
      eventT=event->eventType;
      
      if(eventT==START_OF_DATA){
	  	
	rawStreamZDC->SetSODReading(kTRUE);
	
	// --------------------------------------------------------
	// --- Writing ascii data file for the Shuttle preprocessor
        mapFile4Shuttle = fopen(MAPDATA_FILE,"w");
	//if(rawStreamZDC->Next()){
	  while((rawStreamZDC->Next())){
            if(rawStreamZDC->IsHeaderMapping()){ // mapping header
	       iMod++;
	       modGeo[iMod]  = rawStreamZDC->GetADCModule();
	       modType[iMod] = rawStreamZDC->GetModType();
	       modNCh[iMod]  = rawStreamZDC->GetADCNChannels();
	    }
            if(rawStreamZDC->IsChMapping()){ 
	      if(modType[iMod]==1){ // ADC mapping ----------------------
	        adcMod[ich]  = rawStreamZDC->GetADCModFromMap(ich);
	        adcCh[ich]   = rawStreamZDC->GetADCChFromMap(ich);
	        sigCode[ich] = rawStreamZDC->GetADCSignFromMap(ich);
	        det[ich]     = rawStreamZDC->GetDetectorFromMap(ich);
	        sec[ich]     = rawStreamZDC->GetTowerFromMap(ich);
	        ich++;
	      }
	      else if(modType[iMod]==2){ // VME scaler mapping -------------
	        scMod[iScCh]     = rawStreamZDC->GetScalerModFromMap(iScCh);
	        scCh[iScCh]      = rawStreamZDC->GetScalerChFromMap(iScCh);
	        scSigCode[iScCh] = rawStreamZDC->GetScalerSignFromMap(iScCh);
	        scDet[iScCh]     = rawStreamZDC->GetScDetectorFromMap(iScCh);
	        scSec[iScCh]     = rawStreamZDC->GetScTowerFromMap(iScCh);
	        iScCh++;
	      }
	      else if(modType[iMod]==6 && modGeo[iMod]==4){ // ZDC TDC mapping --------------------
	        tdcMod[itdcCh]     = rawStreamZDC->GetTDCModFromMap(itdcCh);
	        tdcCh[itdcCh]      = rawStreamZDC->GetTDCChFromMap(itdcCh);
	        tdcSigCode[itdcCh] = rawStreamZDC->GetTDCSignFromMap(itdcCh);
	        itdcCh++;
	      }
	    }
 	  }
	  // Writing data on output FXS file
	  for(Int_t is=0; is<2*kNChannels; is++){
	     fprintf(mapFile4Shuttle,"\t%d\t%d\t%d\t%d\t%d\t%d\n",
	       is,adcMod[is],adcCh[is],sigCode[is],det[is],sec[is]);
    	     //printf("  Mapping DA -> %d ADC: mod %d ch %d, code %d det %d, sec %d\n",
	       //is,adcMod[is],adcCh[is],sigCode[is],det[is],sec[is]);
	  }
	  for(Int_t is=0; is<kNScChannels; is++){
	     fprintf(mapFile4Shuttle,"\t%d\t%d\t%d\t%d\t%d\t%d\n",
	       is,scMod[is],scCh[is],scSigCode[is],scDet[is],scSec[is]);
 	     //if(scMod[is]!=-1) printf("  Mapping DA -> %d Scaler: mod %d ch %d, code %d det %d, sec %d\n",
	       //is,scMod[is],scCh[is],scSigCode[is],scDet[is],scSec[is]);
	  }
	  for(Int_t is=0; is<kNScChannels; is++){
	     fprintf(mapFile4Shuttle,"\t%d\t%d\t%d\t%d\n",
	       is,tdcMod[is],tdcCh[is],tdcSigCode[is]);
 	     //if(tdcMod[is]!=-1) printf("  Mapping DA -> %d TDC: mod %d ch %d, code %d\n",
	       //is,tdcMod[is],tdcCh[is],tdcSigCode[is]);
	  }
	  for(Int_t is=0; is<kNModules; is++){
	     fprintf(mapFile4Shuttle,"\t%d\t%d\t%d\n",
	     modGeo[is],modType[is],modNCh[is]);
	     //printf("  Mapping DA -> Module mapping: geo %d type %d #ch %d\n",
	       //modGeo[is],modType[is],modNCh[is]);
	  }
	  
	//} //if (rawstream)
        fclose(mapFile4Shuttle);
      }// SOD event
      else if(eventT==PHYSICS_EVENT){ 

        for(int ij=0; ij<6; ij++) tdcData[ij]=-999.;
	rawStreamZDC->SetSODReading(kTRUE);

  	// ----- Setting ch. mapping -----
	for(int jk=0; jk<2*kNChannels; jk++){
	  rawStreamZDC->SetMapADCMod(jk, adcMod[jk]);
	  rawStreamZDC->SetMapADCCh(jk, adcCh[jk]);
	  rawStreamZDC->SetMapADCSig(jk, sigCode[jk]);
	  rawStreamZDC->SetMapDet(jk, det[jk]);
	  rawStreamZDC->SetMapTow(jk, sec[jk]);
	}
	
  	while(rawStreamZDC->Next()){
         if(rawStreamZDC->GetADCModule()!=kZDCTDCGeo) continue; //skipping ADCs, scalers and trigger cards

          if(rawStreamZDC->GetADCModule()==kZDCTDCGeo && rawStreamZDC->IsZDCTDCDatum()==kTRUE){
             //
	     itdc = rawStreamZDC->GetChannel(); 
             if(itdc==iprevtdc) ihittdc++;
             else ihittdc=0;
             iprevtdc=itdc;
	     //
	     if(((itdc>=8 && itdc<=13) || itdc==15) && ihittdc==0){
               if(itdc!=15){
	         tdcData[itdc-8] = 0.025*rawStreamZDC->GetZDCTDCDatum();
	         //
	         //printf("   ev.%d **** TDC%d %1.0f ns  \n",nphys,itdc, tdcData[itdc-8]);
	       }
	       //
	       else if(itdc==15){
	          tdcL0 = 0.025*rawStreamZDC->GetZDCTDCDatum();
	          //
		  //printf("   ev.%d ++++ TDCL0 %1.0f ns  \n",nphys,tdcL0);
		  //
		  for(int ic=0; ic<6; ic++){
		    if(tdcData[ic]!=-999. && tdcL0!=-999.){
		      hTDC[ic]->Fill(tdcData[ic]-tdcL0);
		      //printf(" ev.%d -> Filling histo%d: %f ns\n",nphys,ic, tdcData[ic]-tdcL0);
		    }
		  }
               }
	     }
	  }
        }
	
 	nphys++;
      
        delete rawStreamZDC;
        rawStreamZDC = 0x0;	
	delete reader;

      }//(if PHYSICS_EVENT) 
      else if(eventT==END_OF_RUN){
        printf("End Of Run detected\n");
        break;
      }
      
      iev++; 

      /* free resources */
      free(event);
 
    } // event loop    
      
  }
  printf(" \n # of physics events analyzed = %d\n\n", nphys);

  /* Analysis of the histograms */
  //
  FILE *fileShuttle;
  fileShuttle = fopen(TDCDATA_FILE,"w");
  //
  Float_t xUp=0., xLow=0., deltaX=0;
  Int_t binMax=0, nBinsx=0;
  Float_t mean[6]={0.,0.,0.,0.,0.,0.}, sigma[6]={0.,0.,0.,0.,0.,0.};
  TF1 *fitfun[6]={0x0,0x0,0x0,0x0,0x0,0x0};
  for(int k=0; k<6; k++){
    if(hTDC[k]->GetEntries()!=0){
       binMax = hTDC[k]->GetMaximumBin();
       if(binMax<=1){
         printf("\n WARNING! Something wrong with det %d histo \n\n", k);
         continue;
       }
       // 
       xUp = (hTDC[k]->GetXaxis())->GetXmax();
       xLow = (hTDC[k]->GetXaxis())->GetXmin();
       deltaX = xUp-xLow;
       nBinsx = (hTDC[k]->GetXaxis())->GetNbins();
       //printf(" xMax = %f\n", xLow+binMax*deltaX/nBinsx);
       hTDC[k]->Fit("gaus","Q","",xLow+binMax*deltaX/nBinsx*0.75,xLow+binMax*deltaX/nBinsx*1.25);
       fitfun[k] = hTDC[k]->GetFunction("gaus");
       mean[k] = (Float_t) (fitfun[k]->GetParameter(1));
       sigma[k] = (Float_t) (fitfun[k]->GetParameter(2));
       //printf("\t Mean value from fit = %1.2f ns\n", mean[k]);
       //
       fprintf(fileShuttle,"\t%f\t%f\n",mean[k], sigma[k]);
     }
     else printf("  WARNING! TDC histo %d has no entries and can't be processed!\n",k);
  }
  //						       
  fclose(fileShuttle);
  
  TFile *histofile = new TFile(TDCHISTO_FILE,"RECREATE");
  histofile->cd();
  for(int k=0; k<6; k++) hTDC[k]->Write();						       
  histofile->Close();
  //
  for(Int_t j=0; j<6; j++) delete hTDC[j];
  
  /* store the result files on FES */
  // [1] File with mapping
  status = daqDA_FES_storeFile(MAPDATA_FILE, "MAPPING");
  if(status){
    printf("Failed to export file : %d\n",status);
    return -1;
  }
  // [2] File with TDC data
  status = daqDA_FES_storeFile(TDCDATA_FILE, "TDCDATA");
  if(status){
    printf("Failed to export TDC data file to DAQ FES\n");
    return -1;
  }
  // [3] File with TDC histos
  status = daqDA_FES_storeFile(TDCHISTO_FILE, "TDCHISTOS");
  if(status){
    printf("Failed to export TDC histos file to DAQ FES\n");
    return -1;
  }

  return status;
}
