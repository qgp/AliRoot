/**************************************************************************
 * Copyright(c) 1998-2000, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

/*
  Class for creating of the sumable digits and digits from MC data
  //
  The input :  ideal signals (Hits->Diffusion->Attachment -Ideal signal)
  The output:  "raw digits"

  Effect implemented:
  1. Pad by pad gain map
  2. Noise map  
  3. The dead channels identified  - zerro noise for corresponding pads
     In this case the outpu equal zerro
     
*/




#include <stdlib.h>
#include <TTree.h> 
#include <TObjArray.h>
#include <TFile.h>
#include <TDirectory.h>
#include <Riostream.h>
#include <TParameter.h>

#include "AliTPCDigitizer.h"

#include "AliTPC.h"
#include "AliTPCParam.h"
#include "AliTPCParamSR.h" 
#include "AliRun.h"
#include "AliLoader.h"
#include "AliPDG.h"
#include "AliDigitizationInput.h"
#include "AliSimDigits.h"
#include "AliLog.h"

#include "AliTPCcalibDB.h"
#include "AliTPCCalPad.h"
#include "AliTPCCalROC.h"
#include "TTreeStream.h"
#include "AliTPCReconstructor.h"
#include <TGraphErrors.h>

using std::cout;
using std::cerr;
using std::endl;
ClassImp(AliTPCDigitizer)

//___________________________________________
  AliTPCDigitizer::AliTPCDigitizer() :AliDigitizer(),fDebug(0), fDebugStreamer(0)
{
  //
// Default ctor - don't use it
//
  
}

//___________________________________________
AliTPCDigitizer::AliTPCDigitizer(AliDigitizationInput* digInput) 
  :AliDigitizer(digInput),fDebug(0), fDebugStreamer(0)
{
  //
// ctor which should be used
//  
  AliDebug(2,"(AliDigitizationInput* digInput) was processed");
  if (AliTPCReconstructor::StreamLevel()>0)  fDebugStreamer = new TTreeSRedirector("TPCDigitDebug.root");

}

//------------------------------------------------------------------------
AliTPCDigitizer::~AliTPCDigitizer()
{
// Destructor
  if (fDebugStreamer) delete fDebugStreamer;
}



//------------------------------------------------------------------------
Bool_t AliTPCDigitizer::Init()
{
// Initialization 
    
 return kTRUE;
}


//------------------------------------------------------------------------
void AliTPCDigitizer::Digitize(Option_t* option)
{
  DigitizeFast(option);  
  //DigitizeWithTailAndCrossTalk(option);
  
}
//------------------------------------------------------------------------
void AliTPCDigitizer::DigitizeFast(Option_t* option)
{
  
  // merge input tree's with summable digits
  //output stored in TreeTPCD
  char s[100]; 
  char ss[100];
  TString optionString = option;
  if (!strcmp(optionString.Data(),"deb")) {
    cout<<"AliTPCDigitizer:::DigitizeFast called with option deb "<<endl;
    fDebug = 3;
  }
  //get detector and geometry


  AliRunLoader *rl, *orl;
  AliLoader *gime, *ogime;
  
  if (gAlice == 0x0)
   {
     Warning("DigitizeFast","gAlice is NULL. Loading from input 0");
     rl = AliRunLoader::GetRunLoader(fDigInput->GetInputFolderName(0));
     if (rl == 0x0)
      {
        Error("DigitizeFast","Can not find Run Loader for input 0. Can not proceed.");
        return;
      }
     rl->LoadgAlice();
     rl->GetAliRun();
   }
  AliTPC *pTPC  = (AliTPC *) gAlice->GetModule("TPC");
  AliTPCParam * param = pTPC->GetParam();
  
  //sprintf(s,param->GetTitle());
  snprintf(s,100,"%s",param->GetTitle());
  //sprintf(ss,"75x40_100x60");
  snprintf(ss,100,"75x40_100x60");
  if(strcmp(s,ss)==0){
    printf("2 pad-length geom hits with 3 pad-lenght geom digits...\n");
    delete param;
    param=new AliTPCParamSR();
  }
  else{
    //sprintf(ss,"75x40_100x60_150x60");
    snprintf(ss,100,"75x40_100x60_150x60");
   if(strcmp(s,ss)!=0) {
     printf("No TPC parameters found...\n");
     exit(2); 
   }
  }
  
  pTPC->GenerNoise(500000); //create table with noise
  //
  Int_t nInputs = fDigInput->GetNinputs();
  Int_t * masks = new Int_t[nInputs];
  for (Int_t i=0; i<nInputs;i++)
    masks[i]= fDigInput->GetMask(i);
  Short_t **pdig= new Short_t*[nInputs];   //pointers to the expanded digits array
  Int_t **ptr=  new Int_t*[nInputs];       //pointers to the expanded tracks array
  Bool_t *active=  new Bool_t[nInputs];    //flag for active input segments
  Char_t phname[100];
  
  //create digits array for given sectors
  // make indexes
  //
  //create branch's in TPC treeD
  orl = AliRunLoader::GetRunLoader(fDigInput->GetOutputFolderName());
  ogime = orl->GetLoader("TPCLoader");
  TTree * tree  = ogime->TreeD();
  AliSimDigits * digrow = new AliSimDigits;  

  if (tree == 0x0)
   {
     ogime->MakeTree("D");
     tree  = ogime->TreeD();
   }
  tree->Branch("Segment","AliSimDigits",&digrow);
  //  
  AliSimDigits ** digarr = new AliSimDigits*[nInputs]; 
  for (Int_t i1=0;i1<nInputs; i1++)
    {
      digarr[i1]=0;
     //    intree[i1]
      rl = AliRunLoader::GetRunLoader(fDigInput->GetInputFolderName(i1));
      gime = rl->GetLoader("TPCLoader");
      gime->LoadSDigits("read");
      TTree * treear =  gime->TreeS();
     
      if (!treear) 
       {
        cerr<<"AliTPCDigitizer: Input tree with SDigits not found in"
            <<" input "<< i1<<endl;
        for (Int_t i2=0;i2<i1+1; i2++){ 
          
          if(digarr[i2])  delete digarr[i2];
	}
        delete [] digarr;
        delete [] active;
        delete []masks;
        delete []pdig;
        delete []ptr;
        return;
       }

      //sprintf(phname,"lhcphase%d",i1);
      snprintf(phname,100,"lhcphase%d",i1);
      TParameter<float> *ph = (TParameter<float>*)treear->GetUserInfo()
	                       ->FindObject("lhcphase0");
      if(!ph){
        cerr<<"AliTPCDigitizer: LHC phase  not found in"
            <<" input "<< i1<<endl;
        for (Int_t i2=0;i2<i1+1; i2++){ 
          if(digarr[i2])  delete digarr[i2];
	}
        delete [] digarr;
        delete [] active;
        delete []masks;
        delete []pdig;
        delete []ptr;
        return;
      }
      tree->GetUserInfo()->Add(new TParameter<float>(phname,ph->GetVal()));
	      //
      if (treear->GetIndex()==0)  
	treear->BuildIndex("fSegmentID","fSegmentID");
      treear->GetBranch("Segment")->SetAddress(&digarr[i1]);
    }




  //

  param->SetZeroSup(2);

  Int_t zerosup = param->GetZeroSup(); 
  AliTPCCalPad * gainTPC = AliTPCcalibDB::Instance()->GetDedxGainFactor(); 
  AliTPCCalPad * noiseTPC = AliTPCcalibDB::Instance()->GetPadNoise(); 
  //
  //Loop over segments of the TPC
    
  for (Int_t segmentID=0; segmentID<param->GetNRowsTotal(); segmentID++) 
   {
    Int_t sector, padRow;
    if (!param->AdjustSectorRow(segmentID,sector,padRow)) 
     {
      cerr<<"AliTPC warning: invalid segment ID ! "<<segmentID<<endl;
      continue;
     }
    AliTPCCalROC * gainROC = gainTPC->GetCalROC(sector);  // pad gains per given sector
    AliTPCCalROC * noiseROC = noiseTPC->GetCalROC(sector);  // noise per given sector
    digrow->SetID(segmentID);

    Int_t nTimeBins = 0;
    Int_t nPads = 0;

    Bool_t digitize = kFALSE;
    for (Int_t i=0;i<nInputs; i++) 
     { 

      rl = AliRunLoader::GetRunLoader(fDigInput->GetInputFolderName(i));
      gime = rl->GetLoader("TPCLoader");
      
      if (gime->TreeS()->GetEntryWithIndex(segmentID,segmentID) >= 0) {
	digarr[i]->ExpandBuffer();
	digarr[i]->ExpandTrackBuffer();
        nTimeBins = digarr[i]->GetNRows();
        nPads = digarr[i]->GetNCols();
	active[i] = kTRUE;
	if (!GetRegionOfInterest() || (i == 0)) digitize = kTRUE;
      } else {
	active[i] = kFALSE;
      }
      if (GetRegionOfInterest() && !digitize) break;
     }   
    if (!digitize) continue;

    digrow->Allocate(nTimeBins,nPads);
    digrow->AllocateTrack(3);

    Float_t q=0;
    Int_t label[1000]; //stack for 300 events 
    Int_t labptr = 0;

    Int_t nElems = nTimeBins*nPads;     
 
    for (Int_t i=0;i<nInputs; i++)
     if (active[i]) { 
       pdig[i] = digarr[i]->GetDigits();
       ptr[i]  = digarr[i]->GetTracks();
      }
     
    Short_t *pdig1= digrow->GetDigits();
    Int_t   *ptr1= digrow->GetTracks() ;

    

    for (Int_t elem=0;elem<nElems; elem++)
     {    

       q=0;
       labptr=0;
       // looop over digits 
        for (Int_t i=0;i<nInputs; i++) if (active[i]) 
         { 
          //          q  += digarr[i]->GetDigitFast(rows,col);
            q  += *(pdig[i]);
         
           for (Int_t tr=0;tr<3;tr++) 
            {
             //             Int_t lab = digarr[i]->GetTrackIDFast(rows,col,tr);
             Int_t lab = ptr[i][tr*nElems];
             if ( (lab > 1) && *(pdig[i])>zerosup) 
              {
                label[labptr]=lab+masks[i];
                labptr++;
              }          
            }
           pdig[i]++;
           ptr[i]++;
         }
        q/=16.;  //conversion factor
	Float_t gain = gainROC->GetValue(padRow,elem/nTimeBins);  // get gain for given - pad-row pad
	//if (gain<0.5){
	  //printf("problem\n");
	//}
	q*= gain;
	Float_t noisePad = noiseROC->GetValue(padRow,elem/nTimeBins);
        //       Float_t noise  = gRandom->Gaus(0,param->GetNoise()*param->GetNoiseNormFac());  
        Float_t noise  = pTPC->GetNoise();
        q+=noise*noisePad;
        q=TMath::Nint(q);
        if (q > zerosup)
         { 
          if(q >= param->GetADCSat()) q = (Short_t)(param->GetADCSat() - 1);
          //digrow->SetDigitFast((Short_t)q,rows,col);  
          *pdig1 =Short_t(q);
          for (Int_t tr=0;tr<3;tr++)
           {
            if (tr<labptr) 
             ptr1[tr*nElems] = label[tr];
           }
          }
        pdig1++;
        ptr1++;
     }
    //
    //  glitch filter
    //
    digrow->GlitchFilter();
    //
    digrow->CompresBuffer(1,zerosup);
    digrow->CompresTrackBuffer(1);
    tree->Fill();
    if (fDebug>0) cerr<<sector<<"\t"<<padRow<<"\n";  
   } //for (Int_t n=0; n<param->GetNRowsTotal(); n++) 
  

  orl = AliRunLoader::GetRunLoader(fDigInput->GetOutputFolderName());
  ogime = orl->GetLoader("TPCLoader");
  ogime->WriteDigits("OVERWRITE");
  
  //fDigInput->GetTreeDTPC()->Write(0,TObject::kOverwrite);
  
  delete digrow;     
  for (Int_t i1=0;i1<nInputs; i1++) delete digarr[i1];
  delete []masks;
  delete []pdig;
  delete []ptr;
  delete []active;
  delete []digarr;  
}



//------------------------------------------------------------------------
void AliTPCDigitizer::DigitizeSave(Option_t* option)
{
  //
  // Merge input tree's with summable digits
  // Output digits stored in TreeTPCD
  // 
  // Not active for long time.
  // Before adding modification (for ion tail calucation and for the crorsstalk) it should be 
  //  checked one by one with currenlty used AliTPCDigitizer::DigitizeFast
  //
  TString optionString = option;
  if (!strcmp(optionString.Data(),"deb")) {
    cout<<"AliTPCDigitizer::Digitize: called with option deb "<<endl;
    fDebug = 3;
  }
  //get detector and geometry 
  AliRunLoader *rl, *orl;
  AliLoader *gime, *ogime;

  
  orl = AliRunLoader::GetRunLoader(fDigInput->GetOutputFolderName());
  ogime = orl->GetLoader("TPCLoader");
  
  rl = AliRunLoader::GetRunLoader(fDigInput->GetInputFolderName(0));
  //gime = rl->GetLoader("TPCLoader");
  rl->GetLoader("TPCLoader");
  rl->LoadgAlice();
  AliRun* alirun = rl->GetAliRun();
  
  AliTPC *pTPC  = (AliTPC *) alirun->GetModule("TPC");
  AliTPCParam * param = pTPC->GetParam();
  pTPC->GenerNoise(500000); //create teble with noise
  printf("noise %f \n",  param->GetNoise()*param->GetNoiseNormFac());
  //
  Int_t nInputs = fDigInput->GetNinputs();
  // stupid protection...
  if (nInputs <= 0) return;
  //
  Int_t * masks = new Int_t[nInputs];
  for (Int_t i=0; i<nInputs;i++)
    masks[i]= fDigInput->GetMask(i);

  AliSimDigits ** digarr = new AliSimDigits*[nInputs]; 
  for(Int_t ii=0;ii<nInputs;ii++) digarr[ii]=0;

  for (Int_t i1=0;i1<nInputs; i1++)
   {
     //digarr[i1]=0;
    //    intree[i1]
    rl = AliRunLoader::GetRunLoader(fDigInput->GetInputFolderName(i1));
    gime = rl->GetLoader("TPCLoader");

    TTree * treear =  gime->TreeS();
    //
    if (!treear) {      
      cerr<<" TPC -  not existing input = \n"<<i1<<" "; 
      delete [] masks;  
      for(Int_t i=0; i<nInputs; i++) delete digarr[i];
      delete [] digarr;
      return;   
    } 
    //
    TBranch * br = treear->GetBranch("fSegmentID");
    if (br) br->GetFile()->cd();
    treear->GetBranch("Segment")->SetAddress(&digarr[i1]);
   }
  
  rl = AliRunLoader::GetRunLoader(fDigInput->GetInputFolderName(0));
  gime = rl->GetLoader("TPCLoader");
  Stat_t nentries = gime->TreeS()->GetEntries();
  

  //create branch's in TPC treeD
  AliSimDigits * digrow = new AliSimDigits;
  TTree * tree  = ogime->TreeD();

  tree->Branch("Segment","AliSimDigits",&digrow);
  param->SetZeroSup(2);

  Int_t zerosup = param->GetZeroSup();
  //Loop over segments of the TPC
    
  AliTPCCalPad * gainTPC = AliTPCcalibDB::Instance()->GetDedxGainFactor();
  AliTPCCalPad * noiseTPC = AliTPCcalibDB::Instance()->GetPadNoise();
  for (Int_t n=0; n<nentries; n++) {
    rl = AliRunLoader::GetRunLoader(fDigInput->GetInputFolderName(0));
    gime = rl->GetLoader("TPCLoader");
    gime->TreeS()->GetEvent(n);

    digarr[0]->ExpandBuffer();
    digarr[0]->ExpandTrackBuffer();


    for (Int_t i=1;i<nInputs; i++){ 
//      fDigInput->GetInputTreeTPCS(i)->GetEntryWithIndex(digarr[0]->GetID(),digarr[0]->GetID());      
      rl = AliRunLoader::GetRunLoader(fDigInput->GetInputFolderName(i));
      gime = rl->GetLoader("TPCLoader");
      gime->TreeS()->GetEntryWithIndex(digarr[0]->GetID(),digarr[0]->GetID());  
      digarr[i]->ExpandBuffer();
      digarr[i]->ExpandTrackBuffer();
      if ((digarr[0]->GetID()-digarr[i]->GetID())>0) 
       printf("problem\n");
    
    }   
    
    Int_t sector, padRow;
    if (!param->AdjustSectorRow(digarr[0]->GetID(),sector,padRow)) {
      cerr<<"AliTPC warning: invalid segment ID ! "<<digarr[0]->GetID()<<endl;
      continue;
    }

    AliTPCCalROC * gainROC = gainTPC->GetCalROC(sector);  // pad gains per given sector
    AliTPCCalROC * noiseROC = noiseTPC->GetCalROC(sector);  // noise per given sector
    digrow->SetID(digarr[0]->GetID());

    Int_t nTimeBins = digarr[0]->GetNRows();
    Int_t nPads = digarr[0]->GetNCols();
    digrow->Allocate(nTimeBins,nPads);
    digrow->AllocateTrack(3);

    Float_t q=0;
    Int_t label[1000]; //stack for 300 events 
    Int_t labptr = 0;

    

    for (Int_t iTimeBin=0;iTimeBin<nTimeBins; iTimeBin++){   // iTimeBin
      for (Int_t iPad=0;iPad<nPads; iPad++){    // pad
    
       q=0;
       labptr=0;
       // looop over digits 
        for (Int_t i=0;i<nInputs; i++){ 
         q  += digarr[i]->GetDigitFast(iTimeBin,iPad);
          //q  += *(pdig[i]);
         
          for (Int_t tr=0;tr<3;tr++) {
           Int_t lab = digarr[i]->GetTrackIDFast(iTimeBin,iPad,tr);
           //Int_t lab = ptr[i][tr*nElems];
            if ( (lab > 1) ) {
              label[labptr]=lab+masks[i];
              labptr++;
            }          
          }
         // pdig[i]++;
         //ptr[i]++;
         
        }
       q/=16.;  //conversion factor
       //       Float_t noise  = gRandom->Gaus(0,param->GetNoise()*param->GetNoiseNormFac());  
       Float_t gain = gainROC->GetValue(padRow,iPad);
       q*= gain;
       Float_t noisePad = noiseROC->GetValue(padRow, iPad);

       Float_t noise  = pTPC->GetNoise();
       q+=noise*noisePad;
       //
       // here we can get digits from past and add signal
       //
       //
       //for (Int_t jTimeBin=0; jTimeBin<iTimeBin; jTimeBin++)
       //  q+=ionTail
       //

        q=TMath::Nint(q);
        if (q > zerosup){ 
         
         if(q >= param->GetADCSat()) q = (Short_t)(param->GetADCSat() - 1);
         digrow->SetDigitFast((Short_t)q,iTimeBin,iPad);  
         // *pdig1 =Short_t(q);
         for (Int_t tr=0;tr<3;tr++){
           if (tr<labptr) 
             ((AliSimDigits*)digrow)->SetTrackIDFast(label[tr],iTimeBin,iPad,tr);
           //ptr1[tr*nElems] = label[tr];
           //else
             //           ((AliSimDigits*)digrow)->SetTrackIDFast(-1,iTimeBin,iPad,tr);          
           //  ptr1[tr*nElems] = 1;
         }
       }
       //pdig1++;
       //ptr1++;
    }
    }
    
    digrow->CompresBuffer(1,zerosup);
    digrow->CompresTrackBuffer(1);
    tree->Fill();
    if (fDebug>0) cerr<<sector<<"\t"<<padRow<<"\n";  
  } 
//  printf("end TPC merging - end -Tree %s\t%p\n",fDigInput->GetInputTreeH(0)->GetName(),fDigInput->GetInputTreeH(0)->GetListOfBranches()->At(3));
  //fDigInput->GetTreeDTPC()->Write(0,TObject::kOverwrite);
    ogime->WriteDigits("OVERWRITE");

    for (Int_t i=1;i<nInputs; i++) 
     { 
      rl = AliRunLoader::GetRunLoader(fDigInput->GetInputFolderName(i));
      gime = rl->GetLoader("TPCLoader");
      gime->UnloadSDigits();
     }
    ogime->UnloadDigits();
    
  delete digrow;     
  for (Int_t i1=0;i1<nInputs; i1++) delete digarr[i1];
  delete [] masks;
  delete [] digarr;  
}







//------------------------------------------------------------------------
void AliTPCDigitizer::DigitizeWithTailAndCrossTalk(Option_t* option) 
{
  // Modified version of the digitization function
  // Modification: adding the ion tail and crosstalk:
  //
  // pcstream used in order to visually inspect data
  //
  //
  // Crosstalk simulation:
  //   1.) Calculate per time bin mean charge (per pad)  within anode wire segment 
  //   2.) Subsract for the clusters at given time bin fraction of (mean charge) normalized by add hoc constant
  //       AliTPCRecoParam::GetCrosstalkCorrection() (0 if not crosstalk, 1 if ideal crosstalk)
  //       for simplicity we are assuming that wire segents are related to pad-rows
  //       Wire segmentationn is obtatined from the      
  //       AliTPCParam::GetWireSegment(Int_t sector, Int_t row); // to be implemented
  //       AliTPCParam::GetNPadsPerSegment(Int_t segmentID); // to be implemented
  //
  // Ion tail simulation:
  //    1.) Needs signal from pad+-1, taking signal from history
  // merge input tree's with summable digits
  // output stored in TreeTPCD
  //
  
  Int_t nROCs = 72;
  char s[100]; 
  char ss[100];
  TString optionString = option;
  if (!strcmp(optionString.Data(),"deb")) {
    cout<<"AliTPCDigitizer:::DigitizeFast called with option deb "<<endl;
    fDebug = 3;
  }

  // ======== get detector and geometry =======

  AliRunLoader *rl, *orl;
  AliLoader *gime, *ogime;

  if (gAlice == 0x0)
  {
    Warning("DigitizeFast","gAlice is NULL. Loading from input 0");
    rl = AliRunLoader::GetRunLoader(fDigInput->GetInputFolderName(0));
    if (rl == 0x0)
    {
      Error("DigitizeFast","Can not find Run Loader for input 0. Can not proceed.");
      return;
    }
    rl->LoadgAlice();
    rl->GetAliRun();
  }
  AliTPC *pTPC  = (AliTPC *) gAlice->GetModule("TPC");
  AliTPCParam * param = pTPC->GetParam();

  //sprintf(s,param->GetTitle());
  snprintf(s,100,"%s",param->GetTitle());
  //sprintf(ss,"75x40_100x60");
  snprintf(ss,100,"75x40_100x60");
  if(strcmp(s,ss)==0){
    printf("2 pad-length geom hits with 3 pad-lenght geom digits...\n");
    delete param;
    param=new AliTPCParamSR();
  } else {
    //sprintf(ss,"75x40_100x60_150x60");
    snprintf(ss,100,"75x40_100x60_150x60");
    if(strcmp(s,ss)!=0) {
      printf("No TPC parameters found...\n");
      exit(2); 
    }
  }

  pTPC->GenerNoise(500000); //create table with noise
  //
  Int_t nInputs = fDigInput->GetNinputs();
  Int_t * masks = new Int_t[nInputs];
  for (Int_t i=0; i<nInputs;i++)
    masks[i]= fDigInput->GetMask(i);
  Short_t **pdig= new Short_t*[nInputs];   //pointers to the expanded digits array
  Int_t **ptr=  new Int_t*[nInputs];       //pointers to the expanded tracks array
  Bool_t *active=  new Bool_t[nInputs];    //flag for active input segments
  Char_t phname[100];

  //create digits array for given sectors
  // make indexes
  //
  //create branch's in TPC treeD
  orl = AliRunLoader::GetRunLoader(fDigInput->GetOutputFolderName());
  ogime = orl->GetLoader("TPCLoader");
  TTree * tree  = ogime->TreeD();
  AliSimDigits * digrow = new AliSimDigits;  

  if (tree == 0x0)
  {
    ogime->MakeTree("D");
    tree  = ogime->TreeD();
  }
  tree->Branch("Segment","AliSimDigits",&digrow);
  //  
  AliSimDigits ** digarr = new AliSimDigits*[nInputs]; 
  for (Int_t i1=0;i1<nInputs; i1++)
  {
    digarr[i1]=0;
    //    intree[i1]
    rl = AliRunLoader::GetRunLoader(fDigInput->GetInputFolderName(i1));
    gime = rl->GetLoader("TPCLoader");
    gime->LoadSDigits("read");
    TTree * treear =  gime->TreeS();

    if (!treear) 
    {
      cerr<<"AliTPCDigitizer: Input tree with SDigits not found in"
        <<" input "<< i1<<endl;
      for (Int_t i2=0;i2<i1+1; i2++){ 

        if(digarr[i2])  delete digarr[i2];
      }
      delete [] digarr;
      delete [] active;
      delete []masks;
      delete []pdig;
      delete []ptr;
      return;
    }

    //sprintf(phname,"lhcphase%d",i1);
    snprintf(phname,100,"lhcphase%d",i1);
    TParameter<float> *ph = (TParameter<float>*)treear->GetUserInfo()
      ->FindObject("lhcphase0");
    if(!ph)
    {
      cerr<<"AliTPCDigitizer: LHC phase  not found in"
        <<" input "<< i1<<endl;
      for (Int_t i2=0;i2<i1+1; i2++){ 
        if(digarr[i2])  delete digarr[i2];
      }
      delete [] digarr;
      delete [] active;
      delete []masks;
      delete []pdig;
      delete []ptr;
      return;
    }
    tree->GetUserInfo()->Add(new TParameter<float>(phname,ph->GetVal()));
    //
    if (treear->GetIndex()==0)  
      treear->BuildIndex("fSegmentID","fSegmentID");
    treear->GetBranch("Segment")->SetAddress(&digarr[i1]);
  }




  //
  // zero supp, take gain and noise map of TPC from OCDB 
  param->SetZeroSup(2);
  Int_t zerosup = param->GetZeroSup(); 
  AliTPCCalPad * gainTPC = AliTPCcalibDB::Instance()->GetDedxGainFactor(); 
  AliTPCCalPad * noiseTPC = AliTPCcalibDB::Instance()->GetPadNoise(); 

 
  // 
  // Cache the ion tail objects form OCDB
  //      
 
  TObjArray *ionTailArr = (TObjArray*)AliTPCcalibDB::Instance()->GetIonTailArray();
  if (!ionTailArr) {AliFatal("TPC - Missing IonTail OCDB object");}
  TObject *rocFactorIROC  = ionTailArr->FindObject("factorIROC");
  TObject *rocFactorOROC  = ionTailArr->FindObject("factorOROC");
  Float_t factorIROC      = (atof(rocFactorIROC->GetTitle()));
  Float_t factorOROC      = (atof(rocFactorOROC->GetTitle()));
  TObjArray timeResFunc(nROCs); 
  for (Int_t isec = 0;isec<nROCs;isec++){        //loop overs sectors
    // Array of TGraphErrors for a given sector
    TGraphErrors ** graphRes   = new TGraphErrors *[20];
    Float_t * indexAmpGraphs   = new Float_t[20];
    for (Int_t icache=0; icache<20; icache++)
    {
      graphRes[icache]       = NULL;
      indexAmpGraphs[icache] = 0;
    }
    if (!AliTPCcalibDB::Instance()->GetTailcancelationGraphs(isec,graphRes,indexAmpGraphs)) continue;
    // fill all TGraphErrors of trfs (time response function) of a given sector to a TObjArray
    TObjArray *timeResArr = new TObjArray(20);  timeResArr -> SetOwner(kTRUE); 
    for (Int_t ires = 0;ires<20;ires++) timeResArr->AddAt(graphRes[ires],ires);
    timeResFunc.AddAt(timeResArr,isec); // Fill all trfs into a single TObjArray 
    delete timeResArr;
  }

  //
  // 1.) Make first loop to calculate mean amplitude per pad per segment for cross talk 
  //
  
  TObjArray   crossTalkSignalArray(nROCs);  // for 36 sectors 
  TVectorD  * qTotSectorOld  = new TVectorD(nROCs);
  TVectorD  * qTotSectorNew  = new TVectorD(nROCs);  
  Float_t qTotTPC = 0.;
  Int_t nTimeBinsAll = 1100;
  Int_t nWireSegments=11;
  // 1.a) crorstalk matrix initialization
  for (Int_t sector=0; sector<nROCs; sector++){
    TMatrixD *pcrossTalkSignal = new TMatrixD(nWireSegments,nTimeBinsAll);
    for (Int_t imatrix = 0; imatrix<11; imatrix++)
      for (Int_t jmatrix = 0; jmatrix<nTimeBinsAll; jmatrix++){
	(*pcrossTalkSignal)[imatrix][jmatrix]=0.;
      }
    crossTalkSignalArray.AddAt(pcrossTalkSignal,sector);
  }
  //  
  // main loop over rows of whole TPC
  for (Int_t globalRowID=0; globalRowID<param->GetNRowsTotal(); globalRowID++) {
    Int_t sector, padRow;
    if (!param->AdjustSectorRow(globalRowID,sector,padRow)) {
      cerr<<"AliTPC warning: invalid segment ID ! "<<globalRowID<<endl;
      continue;
    }
    // Calculate number of pads in a anode wire segment for normalization
    Int_t anodeSegmentID    = param->GetWireSegment(sector,padRow);
    Float_t nPadsPerSegment = (Float_t)(param->GetNPadsPerSegment(anodeSegmentID));
    // structure with mean signal per pad to be filled for each timebin in first loop (11 anodeWireSegment and 1100 timebin)
    TMatrixD &crossTalkSignal =  *((TMatrixD*)crossTalkSignalArray.At(sector));
    AliTPCCalROC * gainROC = gainTPC->GetCalROC(sector);  // pad gains per given sector
    digrow->SetID(globalRowID);
    Int_t nTimeBins = 0;
    Int_t nPads = 0;
    Bool_t digitize = kFALSE;
    for (Int_t i=0;i<nInputs; i++){   //here we can have more than one input  - merging of separate events, signal1+signal2+background 
      rl = AliRunLoader::GetRunLoader(fDigInput->GetInputFolderName(i));
      gime = rl->GetLoader("TPCLoader");
      if (gime->TreeS()->GetEntryWithIndex(globalRowID,globalRowID) >= 0) {
        digarr[i]->ExpandBuffer();
        digarr[i]->ExpandTrackBuffer();
        nTimeBins = digarr[i]->GetNRows();
        nPads = digarr[i]->GetNCols();
        active[i] = kTRUE;
        if (!GetRegionOfInterest() || (i == 0)) digitize = kTRUE;
      } else {
        active[i] = kFALSE;
      }
      if (GetRegionOfInterest() && !digitize) break;
    }   
    if (!digitize) continue;
    digrow->Allocate(nTimeBins,nPads);
    Float_t q    = 0;
    Int_t labptr = 0;
    Int_t nElems = nTimeBins*nPads; // element is a unit of a given row's pad-timebin space        
    for (Int_t i=0;i<nInputs; i++)
      if (active[i]) { 
        pdig[i] = digarr[i]->GetDigits();
      }
    //    
    // loop over elements i.e pad-timebin space of a row, "padNumber=elem/nTimeBins", "timeBin=elem%nTimeBins"
    // 
    for (Int_t elem=0;elem<nElems; elem++) {    
      q=0;
      labptr=0;
      // looop over digits 
      for (Int_t i=0;i<nInputs; i++) if (active[i])       { 
        q  += *(pdig[i]);
        pdig[i]++;
      }
      Int_t padNumber = elem/nTimeBins;
      Int_t timeBin   = elem%nTimeBins;      
      q/=16.;                                              //conversion factor
      Float_t gain = gainROC->GetValue(padRow,padNumber);  // get gain for given - pad-row pad
      q*= gain;
      
      crossTalkSignal[anodeSegmentID][timeBin]+= q/nPadsPerSegment;        // Qtot per segment for a given timebin
      qTotSectorOld -> GetMatrixArray()[sector] += q;                      // Qtot for each sector
      qTotTPC += q;                                                        // Qtot for whole TPC       
    } // end of q loop

    cout << " sector = " << sector << " row = " << padRow << " SegmentID = " << anodeSegmentID ;
    cout << " nPadsPerSegment = " <<  nPadsPerSegment << " QtotSector = " << qTotSectorOld -> GetMatrixArray()[sector] <<  endl;

  } // end of global row loop


  //
  // 2.) Loop over segments (padrows) of the TPC 
  //  
  for (Int_t globalRowID=0; globalRowID<param->GetNRowsTotal(); globalRowID++) {
    Int_t sector, padRow;
    if (!param->AdjustSectorRow(globalRowID,sector,padRow)) {
      cerr<<"AliTPC warning: invalid segment ID ! "<<globalRowID<<endl;
      continue;
    }
    Int_t anodeSegmentID    = param->GetWireSegment(sector,padRow);
    const Float_t ampfactor = (sector<36)?factorIROC:factorOROC;      // factor for the iontail which is ROC type dependent
    AliTPCCalROC * gainROC  = gainTPC->GetCalROC(sector);  // pad gains per given sector
    AliTPCCalROC * noiseROC = noiseTPC->GetCalROC(sector);  // noise per given sector
    digrow->SetID(globalRowID);
    Int_t nTimeBins = 0;
    Int_t nPads = 0;
    Bool_t digitize = kFALSE;
    for (Int_t i=0;i<nInputs; i++) {   //here we can have more than one input  - merging of separate events, signal1+signal2+background
      rl = AliRunLoader::GetRunLoader(fDigInput->GetInputFolderName(i));
      gime = rl->GetLoader("TPCLoader");
      if (gime->TreeS()->GetEntryWithIndex(globalRowID,globalRowID) >= 0) {
        digarr[i]->ExpandBuffer();
        digarr[i]->ExpandTrackBuffer();
        nTimeBins = digarr[i]->GetNRows();
        nPads = digarr[i]->GetNCols();
        active[i] = kTRUE;
        if (!GetRegionOfInterest() || (i == 0)) digitize = kTRUE;
      } else {
        active[i] = kFALSE;
      }
      if (GetRegionOfInterest() && !digitize) break;
    }   
    if (!digitize) continue;
    
    digrow->Allocate(nTimeBins,nPads);
    digrow->AllocateTrack(3);
    
    Float_t q    = 0.;
    Float_t qX   = 0.;
    Float_t qIon = 0.;
    Float_t qold = 0.;
    Int_t label[1000]; //stack for 300 events 
    Int_t labptr = 0;
    Int_t nElems = nTimeBins*nPads; // element is a unit of a given row's pad-timebin space    
    for (Int_t i=0;i<nInputs; i++)
      if (active[i]) { 
        pdig[i] = digarr[i]->GetDigits();
        ptr[i]  = digarr[i]->GetTracks();
      }
    Short_t *pdig1= digrow->GetDigits();
    Int_t   *ptr1= digrow->GetTracks() ;
    // loop over elements i.e pad-timebin space of a row
    for (Int_t elem=0;elem<nElems; elem++) 
    {     
      q=0; 
      labptr=0;
      // looop over digits 
      for (Int_t i=0;i<nInputs; i++) if (active[i]) 
      { 
        //          q  += digarr[i]->GetDigitFast(rows,col);
        q  += *(pdig[i]);

        for (Int_t tr=0;tr<3;tr++) 
        {
          //             Int_t lab = digarr[i]->GetTrackIDFast(rows,col,tr);
          Int_t lab = ptr[i][tr*nElems];
          if ( (lab > 1) && *(pdig[i])>zerosup) 
          {
            label[labptr]=lab+masks[i];
            labptr++;
          }          
        }
        pdig[i]++;
        ptr[i]++;
      }
      Int_t padNumber = elem/nTimeBins;
      Int_t timeBin   = elem%nTimeBins;

      q/=16.;                                              //conversion factor
      Float_t gain = gainROC->GetValue(padRow,padNumber);  // get gain for given - pad-row pad
      //if (gain<0.5){
      //printf("problem\n");
      //}
      q*= gain;
      qold = q;
      // Crosstalk correction 
      qX = (*(TMatrixD*)crossTalkSignalArray.At(sector))[anodeSegmentID][timeBin];
      
      // Ion tail correction:
      //   elem=padNumber*nTimeBins+timeBin;
      //    lowerElem=elem-nIonTailBins;    
      //    if (lowerElem<0) lowerElem=0;
      //    if (lowerElem in previospad) lowerElem = padNumber*nTimeBins;
      // 
      // for (Int_t celem=elem-1; celem>lowerElem; celem--){
      //  Int_t deltaT=elem-celem;
      //
      // }
      //
      Float_t noisePad = noiseROC->GetValue(padRow,padNumber);
      //       Float_t noise  = gRandom->Gaus(0,param->GetNoise()*param->GetNoiseNormFac());  
      Float_t noise  = pTPC->GetNoise();
      q+=noise*noisePad;	
      q=TMath::Nint(q);  // round to the nearest integer
      
      
      // fill info for degugging
      if (AliTPCReconstructor::StreamLevel()==1) {
        TTreeSRedirector &cstream = *fDebugStreamer;
        cstream <<"ionTailXtalk"<<
	  "sec="       << sector              <<   //
	  "row="       << globalRowID         <<
	  "pad="       << padNumber           <<
	  "tb="        << timeBin             <<	  
	  // "qsecOld.="  << qTotSectorOld       <<   // vector of total charge in sector => number total charge in given sector
	  //"qsecNew.="  << qTotSectorNew       <<    // 
	  "qtpc="      << qTotTPC             <<      // acumulated charge without crosstalk and ion tail in full TPC
	  "qold="      << qold                <<      // charge in given pad-row,pad,time-bin
	  "qX="        << qX                  <<      // crosstal contribtion at given position
	  "qIon="      << qIon                <<      // ion tail cotribution from past signal
	  "q="         << q                   <<      // q=qold-qX-qIon - to check sign of the effects
	  // "mult="      << sec                 <<
	  "\n";
      }// dump the results to the debug streamer if in debug mode
      
      if (q > zerosup){ 
        if(q >= param->GetADCSat()) q = (Short_t)(param->GetADCSat() - 1);
        //digrow->SetDigitFast((Short_t)q,rows,col);  
        *pdig1 =Short_t(q);
        for (Int_t tr=0;tr<3;tr++)
	  {
          if (tr<labptr) 
            ptr1[tr*nElems] = label[tr];
        }
      }
      pdig1++;
      ptr1++;
    }
    //
    //  glitch filter
    //
    digrow->GlitchFilter();
    //
    digrow->CompresBuffer(1,zerosup);
    digrow->CompresTrackBuffer(1);
    tree->Fill();
    if (fDebug>0) cerr<<sector<<"\t"<<padRow<<"\n";  
  } //for (Int_t n=0; n<param->GetNRowsTotal(); n++) 


  orl = AliRunLoader::GetRunLoader(fDigInput->GetOutputFolderName());
  ogime = orl->GetLoader("TPCLoader");
  ogime->WriteDigits("OVERWRITE");

  //fDigInput->GetTreeDTPC()->Write(0,TObject::kOverwrite);

  delete digrow;     
  for (Int_t i1=0;i1<nInputs; i1++) delete digarr[i1];
  delete []masks;
  delete []pdig;
  delete []ptr;
  delete []active;
  delete []digarr;  
}
