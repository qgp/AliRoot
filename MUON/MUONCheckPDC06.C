/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
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

// Macro for checking MUON PDC06 output and associated files contents
// Frederic Yermia, INFN Torino Sept 2006

// ROOT includes
#include <fstream>
#include "Riostream.h"
#include "TBranch.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TParticle.h"
#include "TTree.h"
#include "TMath.h"
#include "TNtuple.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include "TStyle.h"
// STEER includes
#include "AliRun.h"
#include "AliRunLoader.h"
#include "AliHeader.h"
#include "AliLoader.h"
#include "AliStack.h"
#include "AliTrackReference.h"
// MUON includes
#include "AliMUON.h"
#include "AliMUONData.h"
#include "AliMUONHit.h"
#include "AliMUONConstants.h"
#include "AliMUONDigit.h"
#include "AliMUONRawCluster.h"
#include "AliMUONGlobalTrigger.h"

#include "AliMUONTrack.h"
#include "AliMagF.h"
#include "AliESD.h"
#include "AliESDMuonTrack.h"
// EVGEN includes
#include "AliGenMUONCocktail.h"



void TestTrigger(Int_t, char *, char *);
Bool_t ESD(Int_t, char*, char* , char*);
void Hits(Int_t, char *, Int_t, char *);
void TRef(Int_t,char *,Int_t,char *);

void Checking( Int_t event2Check=0)
{ 
    
    Int_t Ntot=100; // Number of events by galice.root
    char *filename="/dalice01/bastid/PDC06/local-prod/v4-04-Rev-06/trg2mu/run1-100/galice.root";
    char *esdFileName ="/dalice01/bastid/PDC06/local-prod/v4-04-Rev-06/trg2mu/run1-100/AliESDs.root";
    char *outDir="DataQualityControl";
   
     char command[60];

     sprintf(command,"rm -rf %s", outDir);
     gSystem->Exec(command);
     
    
    gSystem->mkdir(outDir);
    //ROOT options    
    gROOT->Reset();
    gStyle->SetFrameFillColor(10);
    gStyle->SetCanvasColor(10);
    gStyle->SetPalette(1,0);
    gStyle->SetTitleSize(0.07);  
    gStyle->SetTitleColor(10);
    gStyle->SetTitleTextColor(1);
    gStyle->SetTitleFillColor(10);
    gStyle->SetTitleBorderSize(1);
    
   
 
    
      TestTrigger(event2Check,filename,outDir);
      ESD(event2Check,filename,esdFileName,outDir);
      Hits(event2Check,filename,Ntot,outDir);
      TRef(event2Check,filename,Ntot,outDir);
      
 printf("                                          \n");
 printf("***       ************************     ***\n");
 printf("***        ***Check is Finished***     ***\n");
 printf("***       ************************     ***\n");


}

 
//************Hits density  and theta on spectro chambers 1,10,11,14   
void Hits(Int_t event2Check,char *filename,Int_t Ntot,char *outDir){

Int_t nbevt=0;

 TH1F  *HitDensity[4];
        HitDensity[0] =  new TH1F("dhits01","",30,0,300);
	HitDensity[0]->SetFillColor(4);
	HitDensity[0]->SetXTitle("R (cm)");
	HitDensity[1] =  new TH1F("dhits10","",30,0,300);
	HitDensity[1]->SetFillColor(4);
	HitDensity[1]->SetXTitle("R (cm)");
	HitDensity[2] =  new TH1F("dhits11","",30,0,300);
	HitDensity[2]->SetFillColor(2);
	HitDensity[2]->SetXTitle("R (cm)");
	HitDensity[3] =  new TH1F("dhits14","",30,0,300);
	HitDensity[3]->SetFillColor(2);
	HitDensity[3]->SetXTitle("R (cm)");  

 TH1F *Theta[4];
  Theta[0]= new TH1F("Theta01","Theta01",180,0.,180);
  Theta[1]= new TH1F("Theta10","Theta10",180,0.,180);
  Theta[2]= new TH1F("Theta11","Theta11",180,0.,180);
  Theta[3]= new TH1F("Theta14","Theta14",180,0.,180);
   

 



 



  // Creating Run Loader and openning file containing Hits
 AliRunLoader * RunLoader = AliRunLoader::Open(filename,"MUONFolder","READ");
printf(">>>>  in Hits():  \n");
printf("Opening %s file \n",filename);
 

 RunLoader->LoadgAlice();
 RunLoader->LoadHeader();

  gAlice = RunLoader->GetAliRun();




  AliLoader * MUONLoader = RunLoader->GetLoader("MUONLoader");

  MUONLoader->LoadHits("READ");  // Loading Tree of hits for MUON
  // Creating MUON data container
  AliMUONData muondata(MUONLoader,"MUON","MUON");

  
  Int_t ievent, nevents;
  nevents = RunLoader->GetNumberOfEvents();
  
 
  
  for (ievent=0; ievent<nevents; ievent++) {
    if (event2Check!=0) ievent=event2Check;
 nbevt++;
// cout << "ievent" << ievent << "\n";
    RunLoader->GetEvent(ievent);
    muondata.SetTreeAddress("H"); 
 
	   



      
  
//*************************ALIMUONHIT************************************
 Int_t itrack, ntracks;
    ntracks = (Int_t) muondata.GetNtracks();
    for (itrack=0; itrack<ntracks; itrack++) { // Track loop


      //Getting List of Hits of Track itrack
      muondata.GetTrack(itrack); 

      Int_t ihit, nhits;
      nhits = (Int_t) muondata.Hits()->GetEntriesFast();
 
     
      
      AliMUONHit* mHit;
      for(ihit=0; ihit<nhits; ihit++) {
	mHit = static_cast<AliMUONHit*>(muondata.Hits()->At(ihit));
  	Int_t nch      = mHit->Chamber();  // chamber number

	Float_t x      = mHit->X();
  	Float_t y      = mHit->Y();
	Float_t theta  = mHit->Theta();
	Float_t Age    = mHit->Age();
	Float_t r=TMath::Sqrt(x*x+y*y);
	
	Float_t wgt=1/(2*10*TMath::Pi()*r)/(Ntot);
 
 if(nch>=1){
    if(nch==14)
{ 
 if(Age<=75e-9){
HitDensity[3]->Fill(r,wgt);
 Theta[3]->Fill(180*theta/(TMath::Pi()),1); 
 }
 
}

 if(nch==11)
{ if(Age<=75e-9){
 HitDensity[2]->Fill(r,wgt);
 Theta[2]->Fill(180*theta/(TMath::Pi()),1); 
}
 
}  


  
   if(nch==10)
{
 HitDensity[1]->Fill(r,wgt);
 Theta[1]->Fill(180*theta/(TMath::Pi()),1); 
 
}  
 if(nch==1)
{
 HitDensity[0]->Fill(r,wgt);
 Theta[0]->Fill(180*theta/(TMath::Pi()),1); 

}

}

}//hits loop 
      muondata.ResetHits();
		      } // end track loop
 
 
if (event2Check!=0) ievent=nevents;
 
} // end loop on event  

MUONLoader->UnloadHits();


delete gAlice->GetRunLoader();

delete gAlice;

gSystem->cd(outDir);


TCanvas *c1 = new TCanvas("c1","Hits Density",400,10,600,700);
c1->Divide(2,2);
c1->cd(1);
HitDensity[0]->Draw();
c1->cd(2);
HitDensity[1]->Draw();
c1->cd(3);
HitDensity[2]->Draw();
c1->cd(4);
HitDensity[3]->Draw();
c1->Print("Hit_Densities.ps");


TCanvas *c2 = new TCanvas("c2","Theta",400,10,600,700);
c2->Divide(2,2);
c2->cd(1);
Theta[0]->Draw();
c2->cd(2);
Theta[1]->Draw();
c2->cd(3);
Theta[2]->Draw();
c2->cd(4);
Theta[3]->Draw();
c2->Print("Hit_theta.ps");
 
 
  
   printf("***                Leaving Hits()               *** \n");
   printf("*************************************************** \n");
  
   




}
            


  




//**************TRIGGER RESPONSE***********************************************
    void TestTrigger (Int_t event2Check,char *filename,char *outDir)
{
    // TDirectory *curDir=gDirectory;
    

  // reads and dumps trigger objects from MUON.RecPoints.root
TClonesArray * globalTrigger;

  // Book a ntuple for more detailled studies
  TNtuple *TgtupleGlo = new TNtuple("TgtupleGlo","Global Trigger Ntuple","ev:global:spapt:smapt:undefapt:uplpt:uphpt:upapt:lpapt");
 // counters
  Int_t	fSingleUndefLpt=0,fSingleUndefHpt=0,fSinglePlusLpt=0,fSinglePlusHpt=0,fSingleMinusLpt=0,fSingleMinusHpt=0,fPairUnlikeLpt=0,fPairUnlikeHpt=0,fPairUnlikeApt=0, fSinglePlusApt=0, fSingleMinusApt=0,fSingleUndefApt=0,nbevt=0;

 Int_t	 fPairlikeLpt=0,fPairlikeHpt=0,fPairlikeApt=0;
 Float_t  CountTrigPairlikeHpt=0,CountTrigPairlikeLpt=0,CountTrigPairlikeApt=0;
 Float_t CountTrigLpt=0,CountTrigHpt=0,CountTrigSpLpt=0,CountTrigSmLpt=0,CountTrigUnLpt=0,CountTrigSpHpt=0,CountTrigSmHpt=0,CountTrigUnHpt=0,CountTrigPairUnlikeHpt=0,CountTrigPairUnlikeLpt=0,CountTrigPairUnlikeApt=0, CountTrigApt=0;
 Float_t CountTrigSpApt=0,CountTrigSmApt=0,CountTrigUnApt=0;

 
  Int_t SPLowpt=0,SPHighpt=0,SPAllpt=0;
  Int_t SMLowpt=0,SMHighpt=0,SMAllpt=0;
  Int_t SULowpt=0,SUHighpt=0,SUAllpt=0;
  Int_t USLowpt=0,USHighpt=0,USAllpt=0;
  Int_t LSLowpt=0,LSHighpt=0,LSAllpt=0;

 
  
  
 
  
// Creating Run Loader and openning file containing Hits
  AliRunLoader * RunLoader = AliRunLoader::Open(filename,"MUONFolder","READ");
  if (RunLoader ==0x0) {
    printf(">>> Error : Error Opening %s file \n",filename);
    return;
  }
  printf(">>>>  in TestTrigger():  \n");
printf("Opening %s file \n",filename);

//delete gAlice;
 
 RunLoader->LoadgAlice();
RunLoader->LoadHeader(); 
 gAlice = RunLoader->GetAliRun();
 AliLoader * MUONLoader = RunLoader->GetLoader("MUONLoader");
  MUONLoader->LoadDigits("READ");
  // Creating MUON data container
  AliMUONData muondata(MUONLoader,"MUON","MUON");


 
  
Int_t ievent, nevents;
  nevents = RunLoader->GetNumberOfEvents();


  AliGenMUONCocktail *cockhead = (AliGenMUONCocktail*)gAlice->Generator();

  Int_t nprocess_succeded=cockhead->GetNSucceded();
  //cout<<"succeded process id = "<<nprocess_succeded<<endl;
 
  AliMUONGlobalTrigger *gloTrg(0x0);

  
  for (ievent=0; ievent<nevents; ievent++) {
 nbevt++;


 if (event2Check!=0) ievent=event2Check;

 RunLoader->GetEvent(ievent);

  
   
    muondata.SetTreeAddress("D,GLT");
    muondata.GetTriggerD();
    globalTrigger = muondata.GlobalTrigger();
   
     
    Int_t nglobals = (Int_t) globalTrigger->GetEntriesFast(); // should be 1
   
    
    for (Int_t iglobal=0; iglobal<nglobals; iglobal++) { // Global Trigger
      gloTrg = static_cast<AliMUONGlobalTrigger*>(globalTrigger->At(iglobal)); 


            fSingleUndefLpt =gloTrg->SingleUndefLpt();
            fSinglePlusLpt =gloTrg->SinglePlusLpt();
	    fSingleMinusLpt =gloTrg->SingleMinusLpt();
	   
	    fSingleUndefHpt =gloTrg->SingleUndefHpt();
	    fSinglePlusHpt =gloTrg->SinglePlusHpt();
	    fSingleMinusHpt =gloTrg->SingleMinusHpt();
	   
	    fPairUnlikeHpt =gloTrg->PairUnlikeHpt();
	    fPairUnlikeLpt =gloTrg->PairUnlikeLpt();
	    fPairUnlikeApt =gloTrg->PairUnlikeApt();
	    fPairlikeLpt=gloTrg->PairLikeLpt(); 
	    fPairlikeHpt =gloTrg->PairLikeHpt();
	    fPairlikeApt=gloTrg->PairLikeApt();

	    
	    fSinglePlusApt =gloTrg->SinglePlusApt();
	    fSingleMinusApt =gloTrg->SingleMinusApt();
	    fSingleUndefApt =gloTrg->SingleUndefApt();

	SPLowpt+=gloTrg->SinglePlusLpt() ;
	SPHighpt+=gloTrg->SinglePlusHpt() ;
	SPAllpt+=gloTrg->SinglePlusApt() ;
	SMLowpt+=gloTrg->SingleMinusLpt();
	SMHighpt+=gloTrg->SingleMinusHpt();
	SMAllpt+=gloTrg->SingleMinusApt();
	SULowpt+=gloTrg->SingleUndefLpt();
	SUHighpt+=gloTrg->SingleUndefHpt();
	SUAllpt+=gloTrg->SingleUndefApt();
	USLowpt+=gloTrg->PairUnlikeLpt(); 
	USHighpt+=gloTrg->PairUnlikeHpt();
	USAllpt+=gloTrg->PairUnlikeApt();
	LSLowpt+=gloTrg->PairLikeLpt(); 
	LSHighpt+=gloTrg->PairLikeHpt();
	LSAllpt+=gloTrg->PairLikeApt();
	

      
    } // end of loop on Global Trigger

 if(fPairUnlikeLpt!=0){
     CountTrigPairUnlikeLpt++;
   
 
 }
 if(fPairUnlikeHpt!=0){
    CountTrigPairUnlikeHpt++;
 
}
 if(fPairUnlikeApt!=0){
    CountTrigPairUnlikeApt++;
 
}
 if(fPairlikeLpt!=0){
     CountTrigPairlikeLpt++;
 }

 if(fPairlikeHpt!=0){
    CountTrigPairlikeHpt++;
 
}
 if(fPairlikeApt!=0){
    CountTrigPairlikeApt++;
 
}

 if(fSinglePlusLpt!=0){
     CountTrigSpLpt++;
	}
 if(fSingleUndefLpt!=0){
     CountTrigUnLpt++;
	}
 if(fSingleMinusLpt!=0){
     CountTrigSmLpt++;
 }
 if(fSinglePlusHpt!=0){
     CountTrigSpHpt++;
     
 }
 if(fSingleUndefHpt!=0){
     CountTrigUnHpt++;
	}
 if(fSingleMinusHpt!=0){
     CountTrigSmHpt++;
 }
 if(fSinglePlusApt!=0){
	  CountTrigSpApt++;
	  
 }
 if(fSingleUndefApt!=0){
     CountTrigUnApt++;
 }
 if(fSingleMinusApt!=0){
     CountTrigSmApt++;
 }
 
 
 
 if(fSinglePlusLpt!=0||fSingleMinusLpt!=0||fSingleUndefLpt!=0){
     CountTrigLpt++;
 }
 if(fSinglePlusHpt!=0||fSingleMinusHpt!=0||fSingleUndefHpt!=0){
     CountTrigHpt++;
	}
 if(fSinglePlusApt!=0||fSingleMinusApt!=0||fSingleUndefApt!=0){
     CountTrigApt++;
	}
 
 


 TgtupleGlo->Fill(ievent,nglobals,gloTrg->SinglePlusApt(),gloTrg->SingleMinusApt(),gloTrg->SingleUndefApt(),gloTrg->PairUnlikeLpt(),gloTrg->PairUnlikeHpt(),gloTrg->PairUnlikeApt(),gloTrg->PairLikeApt());

    muondata.ResetTrigger();

  
    if (event2Check!=0) ievent=nevents;
  } // end loop on event  
MUONLoader->UnloadDigits();
delete gAlice->GetRunLoader();
delete gAlice;

 
gSystem->cd(outDir);
TFile *myFile = new TFile("TriggerCheck.root", "RECREATE");  
    TgtupleGlo->Write();
   
    myFile->Close();

 FILE *outtxt=fopen("output.txt","w");
 fprintf(outtxt,"             MUON TRIGGER RESPONSE         \n");
 fprintf(outtxt,"                                                      \n");
    fprintf(outtxt,"                                                      \n");



 fprintf(outtxt,"\n");
    fprintf(outtxt,"===================================================\n");
    fprintf(outtxt,"============== SUMMARY (raw response) =============\n");
    fprintf(outtxt,"\n");
    fprintf(outtxt,"         Total number of processed events  %d      \n", (event2Check==0) ? nevents : 1);
    fprintf(outtxt,"\n");
    fprintf(outtxt,"Table 1:                                          \n");
    fprintf(outtxt," Global Trigger output       Low pt  High pt   All\n");
    fprintf(outtxt," number of Single Plus      :\t");
    fprintf(outtxt,"%i\t%i\t%i\t",SPLowpt,SPHighpt,SPAllpt);
    fprintf(outtxt,"\n");
    fprintf(outtxt," number of Single Minus     :\t");
    fprintf(outtxt,"%i\t%i\t%i\t",SMLowpt,SMHighpt,SMAllpt);
    fprintf(outtxt,"\n");
    fprintf(outtxt," number of Single Undefined :\t"); 
    fprintf(outtxt,"%i\t%i\t%i\t",SULowpt,SUHighpt,SUAllpt);
    fprintf(outtxt,"\n");
    fprintf(outtxt," number of UnlikeSign pair  :\t"); 
    fprintf(outtxt,"%i\t%i\t%i\t",USLowpt,USHighpt,USAllpt);
    fprintf(outtxt,"\n");
    fprintf(outtxt," number of LikeSign pair    :\t");  
    fprintf(outtxt,"%i\t%i\t%i\t",LSLowpt,LSHighpt, LSAllpt);
    fprintf(outtxt,"\n");
    fprintf(outtxt,"===================================================\n");

    fprintf(outtxt,"\n");
    fprintf(outtxt,"===================================================\n");
    fprintf(outtxt,"============== SUMMARY (trigger events) ===========\n");
    fprintf(outtxt,"\n");
    fprintf(outtxt,"\n");
    fprintf(outtxt,"Table 2:                                          \n");
    fprintf(outtxt," Global Trigger output       Low pt  High pt   All\n");
    fprintf(outtxt," number of Single Plus      :\t");
    fprintf(outtxt,"%.0f\t%.0f\t%.0f\t",CountTrigSpLpt,CountTrigSpHpt,CountTrigSpApt);
    fprintf(outtxt,"\n");
    fprintf(outtxt," number of Single Minus     :\t");
    fprintf(outtxt,"%.0f\t%.0f\t%.0f\t",CountTrigSmLpt,CountTrigSmHpt,CountTrigSmApt);
    fprintf(outtxt,"\n");
    fprintf(outtxt," number of Single Undefined :\t"); 
    fprintf(outtxt,"%.0f\t%.0f\t%.0f\t",CountTrigUnLpt,CountTrigUnHpt,CountTrigUnApt);
    fprintf(outtxt,"\n");
    fprintf(outtxt," number of UnlikeSign pair  :\t"); 
    fprintf(outtxt,"%.0f\t%.0f\t%.0f\t",CountTrigPairUnlikeLpt,CountTrigPairUnlikeHpt,CountTrigPairUnlikeApt);
    fprintf(outtxt,"\n");
    fprintf(outtxt," number of LikeSign pair    :\t");  
    fprintf(outtxt,"%.0f\t%.0f\t%.0f\t",CountTrigPairlikeLpt,CountTrigPairlikeHpt,CountTrigPairlikeApt);
    fprintf(outtxt,"\n");
    fprintf(outtxt,"===================================================\n");
  

   


 CountTrigPairUnlikeLpt=CountTrigPairUnlikeLpt*200000/nprocess_succeded;
 CountTrigPairUnlikeHpt=CountTrigPairUnlikeLpt*200000/nprocess_succeded;
 CountTrigPairUnlikeApt=CountTrigPairUnlikeApt*200000/nprocess_succeded;
 CountTrigPairlikeLpt=CountTrigPairlikeLpt*200000/nprocess_succeded;
 CountTrigPairlikeHpt=CountTrigPairlikeHpt*200000/nprocess_succeded;
 CountTrigPairlikeApt=CountTrigPairlikeApt*200000/nprocess_succeded;
 CountTrigLpt=CountTrigLpt*200000/nprocess_succeded;
 CountTrigHpt=CountTrigHpt*200000/nprocess_succeded;
 CountTrigApt=CountTrigApt*200000/nprocess_succeded;
 


    fprintf(outtxt,"                                                  \n");
    fprintf(outtxt,"                                                  \n");
    fprintf(outtxt,"Table 3:                                          \n");
    fprintf(outtxt,"==================================================\n");
    fprintf(outtxt,"   Trigger rates, normalised for 200000 coll.s-1: \n"); 
    fprintf(outtxt,"    (taking into account the number of events     \n");
    fprintf(outtxt,"   with at less one response for each trigger cut)\n");
    fprintf(outtxt,"==================================================\n");
    fprintf(outtxt,"         Unlike Pair Lpt (Hz):     %.1f  \n",CountTrigPairUnlikeLpt);
    fprintf(outtxt,"         Unlike Pair Hpt (Hz):     %.1f  \n",CountTrigPairUnlikeHpt);
    fprintf(outtxt,"         Unlike Pair Apt (Hz):     %.1f  \n",CountTrigPairUnlikeApt);
    fprintf(outtxt,"         Like Pair Lpt (Hz):       %.1f  \n",CountTrigPairlikeLpt);
    fprintf(outtxt,"         Like Pair Hpt (Hz):       %.1f  \n",CountTrigPairlikeHpt);
    fprintf(outtxt,"         Like Pair Apt (Hz):       %.1f  \n",CountTrigPairlikeApt);
    fprintf(outtxt,"         Single Lpt (Hz):          %.1f  \n",CountTrigLpt);
    fprintf(outtxt,"         Single Hpt (Hz):          %.1f  \n",CountTrigHpt);
    fprintf(outtxt,"         Single Apt (Hz):          %.1f  \n",CountTrigApt);
 
    fprintf(outtxt,"===================================================\n");
    fprintf(outtxt," \n");
    fprintf(outtxt," \n");
    fclose(outtxt);
    
  printf("***                Leaving TestTrigger()         *** \n");
   printf("*************************************************** \n");
  


 
  
   
}


//***************************ESD check*******************************
Bool_t ESD(Int_t event2Check,char *filename,char *esdFileName,char *outDir)
{  
  
 TH1F *hVertex = new TH1F("hVertex","ITS Vertex",100,-25.,25.);
  TH1F *hMult = new TH1F("hMult","Multiplicity of ESD tracks",10,-0.5,9.5);
 
  Double_t fZVertex = 0;

  ULong64_t Trigword;
  Int_t SPLowpt=0,SPHighpt=0,SPAllpt=0;
  Int_t SMLowpt=0,SMHighpt=0,SMAllpt=0;
  Int_t SULowpt=0,SUHighpt=0,SUAllpt=0;
  Int_t USLowpt=0,USHighpt=0,USAllpt=0;
  Int_t LSLowpt=0,LSHighpt=0,LSAllpt=0;
  
 
  Int_t tracktot=0;
  Int_t  nTrackTrig=0;
  Int_t eff_match=0;
  

  
  
  // ------------>set off mag field 
  // AliMagF::SetReadField(kFALSE);


 // ------------>open run loader and load gAlice, kinematics and header
  AliRunLoader* RunLoader = AliRunLoader::Open(filename);
  if (!RunLoader) {
    Error("CheckESD", "getting run loader from file %s failed", 
	    filename);
    return kFALSE;
  }

  printf(">>>>  in ESD():  \n");
printf("Opening %s file \n",filename);


  
 RunLoader->LoadgAlice();
 
RunLoader->LoadHeader(); 

 gAlice = RunLoader->GetAliRun();
 

  if (!gAlice) {
    Error("CheckESD", "no galice object found");
    return kFALSE;
  }
  // ------------->open the ESD file
  TFile* esdFile = TFile::Open(esdFileName);
  if (!esdFile || !esdFile->IsOpen()) {
    Error("CheckdESD", "opening ESD file %s failed", esdFileName);
    return kFALSE;
  }
 //--------> tree & branch
  AliESD* esd = new AliESD();
  TTree* tree = (TTree*) esdFile->Get("esdTree");
  if (!tree) {
    Error("CheckESD", "no ESD tree found");
    return kFALSE;
  }
  tree->SetBranchAddress("ESD", &esd);

 RunLoader->LoadHeader();
 Int_t ievent, nevents;

 
  //----------> get number of events in  file
  nevents =RunLoader->GetNumberOfEvents();
 // &&&&&&&&&&&&&&&&&&&&Loop over events&&&&&&&&&&&&&&&&&&&&&&&
  for (Int_t iEvent = 0; iEvent<nevents; iEvent++) {
      

      
       
      if (event2Check!=0) iEvent=event2Check; 
    // get current event
   RunLoader->GetEvent(iEvent);
   
    // get the event summary data
    tree->GetEvent(iEvent);
    if (!esd) {
      Error("CheckESD", "no ESD object found for event %d", iEvent);
      return kFALSE;
    }

AliESDVertex* Vertex = (AliESDVertex*) esd->AliESD::GetVertex();
    if (Vertex) fZVertex = Vertex->GetZv();

    
    Int_t nTracks = (Int_t)esd->GetNumberOfMuonTracks() ;
   
 Trigword=esd->GetTriggerMask();

  if (Trigword & 0x01) {
  SPLowpt++;
  }
  
             if (Trigword & 0x02){
  SPHighpt++;
  }
             if (Trigword & 0x04){
  SPAllpt++;
  } 
             if (Trigword & 0x08){
  SMLowpt++;
  }  
             if (Trigword & 0x010){
  SMHighpt++;
  }
             if (Trigword & 0x020){
  SMAllpt++;
  } 
             if (Trigword & 0x040){
  SULowpt++;
  }  
             if (Trigword & 0x080){
  SUHighpt++;
  }   
             if (Trigword & 0x100){
  SUAllpt++;
  }  
	     if (Trigword & 0x200){
  USLowpt++;
  }
	     
	     if (Trigword & 0x400){
  USHighpt++;
  }
	     if (Trigword & 0x800){
  USAllpt++;
  }
	     if (Trigword & 0x1000){
  LSLowpt++;
  }

	     if (Trigword & 0x2000){
  LSHighpt++;
  }

	     if (Trigword & 0x4000){
  LSAllpt++;
  }


Int_t tracktrig=0;
   
   
       
    for (Int_t iTrack1 = 0; iTrack1<nTracks; iTrack1++) { //1st loop
	AliESDMuonTrack* muonTrack = esd->GetMuonTrack(iTrack1);
	tracktot++;
	
	if(muonTrack->GetMatchTrigger()) {
	    nTrackTrig++;
	    tracktrig++;
	}
    }
    
   hVertex->Fill(fZVertex);
    hMult->Fill(Float_t(nTracks));



   if (event2Check!=0) ievent=nevents;
  } // loop over events




 gSystem->cd(outDir);

  TCanvas *c3 = new TCanvas("c3","ESD",400,10,600,700);
  c3->Divide(1,2);
  c3->cd(1);
  hVertex->Draw();
  c3->cd(2);
  hMult->Draw();

  FILE *outtxt=fopen("output.txt","a");
  freopen("output.txt","a",outtxt);

 
     eff_match=100*nTrackTrig/tracktot;



  

    fprintf(outtxt,"                                                   \n");
    fprintf(outtxt,"===================================================\n");
    fprintf(outtxt,"================      ESD SUMMARY    ==============\n");
    fprintf(outtxt,"                                                   \n");
    fprintf(outtxt,"         Total number of processed events  %d      \n", (event2Check==0) ? nevents : 1);
    fprintf(outtxt,"\n");
    
    fprintf(outtxt,"\n");
     fprintf(outtxt,"Table 4:                                         \n");
    fprintf(outtxt," Global Trigger output       Low pt  High pt   All\n");
    fprintf(outtxt," number of Single Plus      :\t");
    fprintf(outtxt,"%i\t%i\t%i\t",SPLowpt,SPHighpt,SPAllpt);
    fprintf(outtxt,"\n");
    fprintf(outtxt," number of Single Minus     :\t");
    fprintf(outtxt,"%i\t%i\t%i\t",SMLowpt,SMHighpt,SMAllpt);
    fprintf(outtxt,"\n");
    fprintf(outtxt," number of Single Undefined :\t"); 
    fprintf(outtxt,"%i\t%i\t%i\t",SULowpt,SUHighpt,SUAllpt);
    fprintf(outtxt,"\n");
    fprintf(outtxt," number of UnlikeSign pair  :\t"); 
    fprintf(outtxt,"%i\t%i\t%i\t",USLowpt,USHighpt,USAllpt);
    fprintf(outtxt,"\n");
    fprintf(outtxt," number of LikeSign pair    :\t");  
    fprintf(outtxt,"%i\t%i\t%i\t",LSLowpt,LSHighpt, LSAllpt);
    fprintf(outtxt,"\n");
    fprintf(outtxt,"===================================================\n");
   fprintf(outtxt,"\n");
 fprintf(outtxt,"matching efficiency with the trigger for single tracks = %2d %% \n", eff_match);
 

    fclose(outtxt);
    c3->Print("ESD.ps");

   
    return kTRUE;
    printf("***                Leaving ESD()               *** \n");
    printf("*************************************************** \n");


    
}


void TRef(Int_t event2Check,char *filename,Int_t Ntot, char *outDir){

    Int_t flag11=0,flag12=0,flag13=0,flag14=0;


  
    TH1F *Tof01= new TH1F("Tof01","TOF for first tracking plane",100,0.,100);
    Tof01->SetXTitle("tof (ns)");
    TH1F *Tof14= new TH1F("Tof14","TOF for MT22",100,0.,100);
    Tof14->SetXTitle("tof (ns)");
    
    
    TH1F   *HitDensity[4];
    HitDensity [0] =  new TH1F("TR_dhits01","",30,0,300);
    HitDensity[0]->SetFillColor(3);
    HitDensity[0]->SetXTitle("R (cm)");
    HitDensity[1] =  new TH1F("TR_dhits10","",30,0,300);
    HitDensity[1]->SetFillColor(3);
    HitDensity[1]->SetXTitle("R (cm)");
    HitDensity[2] =  new TH1F("TR_dhits11","",30,0,300);
    HitDensity[2]->SetFillColor(3);
    HitDensity[2]->SetXTitle("R (cm)");
    HitDensity[3] =  new TH1F("TR_dhits14","",30,0,300);
    HitDensity[3]->SetFillColor(3);
    HitDensity[3]->SetXTitle("R (cm)");

   // Creating Run Loader and openning file containing Track reference
    AliRunLoader * RunLoader = AliRunLoader::Open(filename);
    printf(">>>>  in TRef():  \n");
    printf("Opening %s file \n",filename);
    

    RunLoader->LoadgAlice();
    RunLoader->LoadHeader();
    gAlice = RunLoader->GetAliRun();
    
    RunLoader->LoadKinematics(); 
    RunLoader->LoadTrackRefs();
    
    AliLoader *MUONLoader = RunLoader->GetLoader("MUONLoader");
    MUONLoader->LoadHits();  
    
    Int_t nevents;   
nevents = RunLoader->GetNumberOfEvents();

 for (Int_t nev=0; nev<nevents ; nev++) 
   {
       if (event2Check!=0) nev=event2Check;
      
     Int_t  save=-99;
       
       
      RunLoader->GetEvent(nev);

    
      if (nev < 0) continue;


      TTree *TR = RunLoader->TreeTR();
      Int_t nentries = (Int_t)TR->GetEntries();
      TClonesArray *fRefArray = new TClonesArray("AliTrackReference");
      TBranch *branch = TR->GetBranch("MUON");
      branch->SetAddress(&fRefArray);

      for(Int_t l=0; l<nentries; l++)
      {
         if(!branch->GetEvent(l)) continue;
         Int_t nnn = fRefArray->GetEntriesFast();

	

         for(Int_t k=0; k<nnn; k++) 
         {
            AliTrackReference *tref = (AliTrackReference*)fRefArray->UncheckedAt(k);
            Int_t label = tref->GetTrack();
	    Float_t x     =    tref->X();        // x-pos of hit
	    Float_t y     =    tref->Y();        // y-pos
	    Float_t z     = tref->Z();

	    Float_t r=TMath::Sqrt(x*x+y*y);
	    Float_t time =    tref->GetTime();  
	    
	    Float_t wgt=1/(2*10*TMath::Pi()*r)/(Ntot);

	   if (save!=label){
		    save=label;
		    flag11=0;
		    flag12=0;
		    flag13=0;
		    flag14=0;
		}
 

		if (save==label){

//Ch 1, z=-526.16
 if (z<=-521&& z>=-531&&flag11==0){
	 flag11=1;
	 
	 HitDensity[0]->Fill(r,wgt);
	 Tof01->Fill(1000000000*time,1);
     };
 
//Ch 10, z=-1437.6
     if (z<=-1432&&z>=-1442&&flag12==0){
	 flag12=1;
	 
	 HitDensity[1]->Fill(r,wgt);
	 }


//Ch 11, z=-1603.5
 if (z<=-1598&& z>=-1608&&flag13==0){
	 flag13=1;
	 
	 HitDensity[2]->Fill(r,wgt);
     };
  
//ch 14 z=-1720.5    
     if(z<=-1715&&z>=-1725&&flag14==0){
	 flag14=1;
 
	 HitDensity[3]->Fill(r,wgt);
	 Tof14->Fill(1000000000*time,1);
	 
     }; 
	 
		}//if save==label
	 
 
         }//hits de TR
	 
	
      }//entree de TR 
      
      if (event2Check!=0) nev=nevents; 
      fRefArray->Delete();
      delete  fRefArray; 
   }//evt loop
 RunLoader->UnloadKinematics();
 RunLoader->UnloadTrackRefs();
 MUONLoader->UnloadHits(); 
 delete gAlice->GetRunLoader();
 
 delete gAlice;
  

 gSystem->cd(outDir);
 TCanvas *c4 = new TCanvas("c4","TOF",400,10,600,700);
 c4->Divide(1,2);
 c4->cd(1);
 
 Tof01->Draw();
 c4->cd(2);
 Tof14->Draw();
 c4->Print("Tof_on_trigger.ps");
 
 
 TCanvas *c5 = new TCanvas("c5","TRef:Hits Density",400,10,600,700);
 c5->Divide(2,2);
 c5->cd(1);
 HitDensity[0]->Draw();
 c5->cd(2);
 HitDensity[1]->Draw();
 c5->cd(3);
 HitDensity[2]->Draw();
 c5->cd(4);
 HitDensity[3]->Draw();
 c5->Print("TR_Hit_densities.ps");
 
 printf("***                Leaving TRef()               *** \n");
 printf("*************************************************** \n");

}





 


