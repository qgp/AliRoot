//*****************************************************************************
//      Analysis of matching of ESD primary tracks with TOF digits.
//      This macro should be run after simulation and reconstruction runs.
//      It uses the files: AliESDs.root and TOF.Digits.root
//      Matching efficiency and contamination are defined by:
//      efficiency = (true matched tracks) / (tracks having a TOF digit)
//      contamination = (wrong matched tracks) / (wrong matched tracks + true matched tracks) 
//      Origin: Sergey Kiselev, ITEP, Moscow Sergey.Kiselev@cern.ch, TOF group
//*****************************************************************************

#if !defined( __CINT__) || defined(__MAKECINT__)
  #include <Riostream.h>
  #include "TFile.h"
  #include "TH1F.h"
  #include "TCanvas.h"
  #include "TStopwatch.h"
  #include "TParticle.h"
  #include "TROOT.h"

  #include "AliRun.h"
  #include "AliStack.h"
  #include "AliRunLoader.h"
  #include "AliLoader.h"

  #include "AliESD.h"
  #include "AliESDtrack.h"
  #include "AliTOFdigit.h"
#endif

extern AliRun *gAlice;
extern TROOT *gROOT;

Int_t TestTOF(const Char_t *dir=".") { 

   TStopwatch timer;
  

   AliTOFGeometry *pippo = new AliTOFGeometryV5();
   static const Double_t kMasses[]={
     0.000511, 0.105658, 0.139570, 0.493677, 0.938272, 1.875613
   };

   //a-priori particle concentrations

   Double_t c[5]={0.01, 0.01, 0.85, 0.10, 0.05};

   //Hits/Reco

   TH1F *hSector= new TH1F("hSector"," TOF TrackRefs, sector # ",18,0.,18.);
   TH1F *hSectorM= new TH1F("hSectorM"," TOF Matched, sector # ",18,0.,18.);
   TH1F *hSectorMF= new TH1F("hSectorMF"," TOF Matched G, sector # ",18,0.,18.);
   TH1F *hSectorMG= new TH1F("hSectorMG"," TOF Matched F , sector # ",18,0.,18.);
   TH1F *hprimP= new TH1F("hprimP"," TPC mom  tracks",20,0.,4.);
   TH1F *hprimPpi= new TH1F("hprimPpi"," TPC mom  tracks",20,0.,4.);
   TH1F *hprimPka= new TH1F("hprimPka"," TPC mom  tracks",20,0.,4.);
   TH1F *hprimPpr= new TH1F("hprimPpr"," TPC mom  tracks",20,0.,4.);


   // Reaching TOF,prim
   TH1F *hprimPTOF= new TH1F("hprimPTOF"," TPC mom  tracks",20,0.,4.);
   TH1F *hprimPTOFpi= new TH1F("hprimPTOFpi"," TPC mom  tracks",20,0.,4.);
   TH1F *hprimPTOFka= new TH1F("hprimPTOFka"," TPC mom  tracks",20,0.,4.);
   TH1F *hprimPTOFpr= new TH1F("hprimPTOFpr"," TPC mom  tracks",20,0.,4.);


   // Well matched,prim
   TH1F *hprimPTOF3= new TH1F("hprimPTOF3"," TPC mom  tracks",20,0.,4.);
   TH1F *hprimPTOFpi3= new TH1F("hprimPTOFpi3"," TPC mom  tracks",20,0.,4.);
   TH1F *hprimPTOFka3= new TH1F("hprimPTOFka3"," TPC mom  tracks",20,0.,4.);
   TH1F *hprimPTOFpr3= new TH1F("hprimPTOFpr3"," TPC mom  tracks",20,0.,4.);

   // bad matched,prim
   TH1F *hprimPTOF4= new TH1F("hprimPTOF4"," TPC mom  tracks",20,0.,4.);
   TH1F *hprimPTOFpi4= new TH1F("hprimPTOFpi4"," TPC mom  tracks",20,0.,4.);
   TH1F *hprimPTOFka4= new TH1F("hprimPTOFka4"," TPC mom  tracks",20,0.,4.);
   TH1F *hprimPTOFpr4= new TH1F("hprimPTOFpr4"," TPC mom  tracks",20,0.,4.);

   // matched,prim
   TH1F *hprimPTOF34= new TH1F("hprimPTOF34"," TPC mom  tracks",20,0.,4.);
   TH1F *hprimPTOFpi34= new TH1F("hprimPTOFpi34"," TPC mom  tracks",20,0.,4.);
   TH1F *hprimPTOFka34= new TH1F("hprimPTOFka34"," TPC mom  tracks",20,0.,4.);
   TH1F *hprimPTOFpr34= new TH1F("hprimPTOFpr34"," TPC mom  tracks",20,0.,4.);


   //PID

   Double_t pl=0.,pu=4.;
   Int_t nbins = 20;

   const Char_t *hname[]={
    "piP","piG","piR","piF","piGood","piFake",
    "kaP","kaG","kaR","kaF","kaGood","kaFake",
    "prP","prG","prR","prF","prGood","prFake"
   };  
   Int_t nh=sizeof(hname)/sizeof(const Char_t *);
   TH1F **hprt=new TH1F*[nh]; 

   for (Int_t i=0; i<nh; i++) {
     hprt[i]=(TH1F*)gROOT->FindObject(hname[i]);
     if (hprt[i]==0) {hprt[i]=new TH1F(hname[i],"",nbins,pl,pu);hprt[i]->Sumw2();}
   }
   TH1F *piP=hprt[0];
   TH1F *piG=hprt[1];
   TH1F *piR=hprt[2];
   TH1F *piF=hprt[3];
   //Kaons
   TH1F *kaP=hprt[6];
   TH1F *kaG=hprt[7];
   TH1F *kaR=hprt[8];
   TH1F *kaF=hprt[9];
   // Protons
   TH1F *prP=hprt[12];
   TH1F *prG=hprt[13];
   TH1F *prR=hprt[14];
   TH1F *prF=hprt[15];


   delete[] hprt;

   Char_t fname[100];

   if (gAlice) {
      delete gAlice->GetRunLoader();
      delete gAlice;
      gAlice=0;
   }

   sprintf(fname,"%s/galice.root",dir);
   AliRunLoader *rl = AliRunLoader::Open(fname);
   if (rl == 0x0) {
      cerr<<"Can not open session"<<endl;
      return 1;
   }
      
   sprintf(fname,"%s/AliESDs.root",dir);
   TFile *ef=TFile::Open(fname);
   if (!ef || !ef->IsOpen()) {
     cerr<<"Can't AliESDs.root !\n";
     delete rl;
     return 1;
   }


//----------------------------------------

//----------------------------------------

//------------------------------------------------------

   AliESD* event = new AliESD;
   TTree* tree = (TTree*) ef->Get("esdTree");
   if (!tree) {
     cerr<<"no ESD tree found !\n";
     delete rl;
     return 1;
   }

   tree->SetBranchAddress("ESD", &event);

   Int_t nAReachTOF=0;
   Int_t nAGMatchTOF=0;
   Int_t nABMatchTOF=0;
   Int_t nprim = 0; 
   Int_t nprimpi = 0; 
   Int_t nprimka = 0; 
   Int_t nprimpr = 0; 
   const Int_t labelMax=2000000;
//******* The loop over events --------------------------------------------------

   Int_t * iTrackRefV = new Int_t[labelMax]; //TOF digit number for track with a given label
   Int_t * iTrackRefVS = new Int_t[labelMax]; //TOF digit number for track with a given label
   Int_t e=0;

   while (tree->GetEvent(e)) {

     cout<<endl<<"********* Processing event number: "<<e<<"*******\n";
     rl->GetEvent(e);
     rl->LoadKinematics();
     rl->LoadTrackRefs();
     rl->LoadHeader();     
     
     e++;
     AliStack *stack = rl->Stack();
     
     TArrayF vertex(3);
     rl->GetHeader()->GenEventHeader()->PrimaryVertex(vertex);
     
     
     TTree *trTree= rl->TreeTR();
     Int_t nEntriesTR = trTree->GetEntries(); 

     TClonesArray * TOFtrackrefs = new TClonesArray("AliTrackReference",1000);
     TOFtrackrefs = trTree->GetBranch("TOF")->SetAddress(&TOFtrackrefs); 
     
     Int_t ntrk=event->GetNumberOfTracks();
     cerr<<"Number of ESD tracks : "<<ntrk<<endl; 

     
     for (Int_t i=0; i<labelMax; i++) {
       iTrackRefV[i]=-1;
       iTrackRefVS[i]=-1;
     }

     for (Int_t iPrimPart = 0; iPrimPart<nEntriesTR; iPrimPart++) {
       trTree->GetEntry(iPrimPart);
       
       Int_t nTOFtrackrefs=TOFtrackrefs->GetEntriesFast();
       // Loop over TOF references
       for (Int_t iTrackRef = 0; iTrackRef < TOFtrackrefs->GetEntriesFast(); iTrackRef++) {
	 AliTrackReference *trackRef = (AliTrackReference*)TOFtrackrefs->At(iTrackRef);            
         iTrackRefV[trackRef->Label()]=iTrackRef;         
         Float_t pos[3];
         pos[0]=trackRef->X();
         pos[1]=trackRef->Y();
         pos[2]=trackRef->Z();
         iTrackRefVS[trackRef->Label()]=pippo->GetSector(pos);         

       }
     }

     Int_t npart = stack->GetNprimary();
     
     for (Int_t part=0; part<npart; part++) {
       TParticle *MPart = stack->Particle(part);
       TVector3 dVertex(MPart->Vx() - vertex[0], 
			MPart->Vy() - vertex[1],
			MPart->Vz() - vertex[2]);
       if (dVertex.Mag() > 0.0001) continue;
       Int_t mpart  = MPart->GetPdgCode();
       Float_t P = MPart->P();
       Float_t theta = MPart->Theta();
       Float_t phi = MPart->Phi();
       Int_t icod = TMath::Abs(mpart);
       if(theta<45./180.*TMath::Pi() || theta>135./180.*TMath::Pi())continue;
       if(icod != 211 && icod != 321 && icod != 2212 && icod != 11)continue;
       hprimP->Fill(P); 
       theta=theta*180./TMath::Pi();
       phi=phi*180./TMath::Pi();
       switch(icod){
       case 211: // Geant and Hit generation Step
	 hprimPpi->Fill(P); 
	 piP->Fill(P); 
	 break;
       case 321: // Geant and Hit generation Step
	 hprimPka->Fill(P); 
	 kaP->Fill(P); 
	 break;
       case 2212: // Geant and Hit generation Step
	 hprimPpr->Fill(P); 
	 prP->Fill(P); 
	 break;
       }
     } // primary loop
     
//******* The loop over tracks --------------------------------------------------


     Int_t nReachTOF=0;
     Int_t nGMatchTOF=0;
     Int_t nBMatchTOF=0;
     Int_t nCalinSec=8736;
     while (ntrk--) {
       AliESDtrack *t=event->GetTrack(ntrk);
       if ((t->GetStatus()&AliESDtrack::kTIME)==0)continue;
       Int_t label=TMath::Abs(t->GetLabel());
       Double_t p=t->GetP(); 
       UInt_t AssignedTOFcluster=t->GetTOFcluster();//index of the assigned TOF cluster, >0 ?
       Int_t detid=t->GetTOFCalChannel();//index of the assigned TOF cluster, >0 ?
       
       Int_t sector = detid/nCalinSec;

       TParticle *mypart=stack->Particle(label);
       Int_t code=mypart->GetPdgCode();
       TVector3 dVertex(mypart->Vx() - vertex[0], 
			mypart->Vy() - vertex[1],
			mypart->Vz() - vertex[2]);
       Float_t P = mypart->P(); 
       Float_t eta = mypart->Eta();
       Float_t theta = mypart->Theta();
       Float_t phi = mypart->Phi();
       Int_t icod = TMath::Abs(code);
       // Primary?
       if( dVertex.Mag() > 0.0001)continue;
       if(!(theta>=45./180.*TMath::Pi() && theta<=135./180.*TMath::Pi()))continue;
       if(icod != 211 && icod != 321 && icod != 2212 && icod != 11)continue;
       // Primary ESD tracks
       
       if(iTrackRefV[label]>-1) {
         nReachTOF++;
	 hSector->Fill(iTrackRefVS[label]); 
	 hprimPTOF->Fill(P); 
	 // Primary ESD tracks reaching the tof
	 switch(icod){
	 case 211: // Geant and Hit generation Step
	   hprimPTOFpi->Fill(P); 
	   break;
	 case 321: // Geant and Hit generation Step
	   hprimPTOFka->Fill(P); 
	   break;
	 case 2212: // Geant and Hit generation Step
	   hprimPTOFpr->Fill(P); 
	   break;
	 }
       }
       if(AssignedTOFcluster){ //matched

         hSectorM->Fill(sector);
	 hprimPTOF34->Fill(P);

	 //switch 
	 switch(icod){
	 case 211: // Geant and Hit generation Step
	   hprimPTOFpi34->Fill(P); 
	   break;
	 case 321: // Geant and Hit generation Step
	   hprimPTOFka34->Fill(P); 
	   break;
	 case 2212: // Geant and Hit generation Step
	   hprimPTOFpr34->Fill(P); 
	   break;
	 }//end switch


 	 if(iTrackRefV[label]>-1) { // matched good
	   
	   nGMatchTOF++;

	   hSectorMG->Fill(sector);
	   hprimPTOF3->Fill(P);

	   switch(icod){
	   case 211: // Geant and Hit generation Step
	     hprimPTOFpi3->Fill(P); 
	     break;
	   case 321: // Geant and Hit generation Step
	     hprimPTOFka3->Fill(P); 
	     break;
	   case 2212: // Geant and Hit generation Step
	     hprimPTOFpr3->Fill(P); 
	     break;
	   }

	 } else { //matched wrong
	 
	   nBMatchTOF++;
	   hSectorMF->Fill(sector);
	   hprimPTOF4->Fill(P);

	   switch(icod){
	   case 211: // Geant and Hit generation Step
	     hprimPTOFpi4->Fill(P); 
	     break;
	   case 321: // Geant and Hit generation Step
	     hprimPTOFka4->Fill(P); 
	     break;
	   case 2212: // Geant and Hit generation Step
	     hprimPTOFpr4->Fill(P); 
	     break;
	   }
	 } 	 
	 // Now Test PID

	 UInt_t status=AliESDtrack::kESDpid;
	 status|=AliESDtrack::kTOFpid; 
	 if ((t->GetStatus()&status) == status) {
	   if ((t->GetStatus()&AliESDtrack::kTIME)==0) continue;	   
           Double_t r[10]; t->GetTOFpid(r);
           Double_t rcc=0.;
           Int_t i;
           for (i=0; i<AliPID::kSPECIES; i++) rcc+=(c[i]*r[i]);
           if (rcc==0.) continue;
	   //Here we apply Bayes' formula
           Double_t w[10];
           for (i=0; i<AliPID::kSPECIES; i++) w[i]=c[i]*r[i]/rcc;
	   //// PROTONS/////
           if (TMath::Abs(code)==2212) prR->Fill(P);
	   if (w[4]>w[3] && w[4]>w[2] && w[4]>w[1] && w[4]>w[0]) {//proton
	     prG->Fill(P);
	     //ID as proton, but fake//
	     if (TMath::Abs(code)!=2212)prF->Fill(P);
	   }
	   ////KAONS////
           if (TMath::Abs(code)==321) kaR->Fill(P);
           if (w[3]>w[4] && w[3]>w[2] && w[3]>w[1] && w[3]>w[0]) {//kaon
	     kaG->Fill(P);
	     //ID as kaon, but fake//
	     if (TMath::Abs(code)!=321)kaF->Fill(P);
	   }
	   ////PIONS////
	   if (TMath::Abs(code)==211)piR->Fill(P);
	   if (w[2]>w[3] && w[2]>w[4] && w[2]>w[1] && w[2]>w[0]) {//pion
	     piG->Fill(P);
	     if (TMath::Abs(code)!=211)piF->Fill(P);
	   }
	 } 
       } // end if on AssignedTOFCluster             
     }// end loop on tracks

     nAReachTOF=nReachTOF+nAReachTOF;
     nAGMatchTOF=nGMatchTOF+nAGMatchTOF;
     nABMatchTOF=nBMatchTOF+nABMatchTOF;

   } //end of while (tree->GetEvent(e)) {

   delete iTrackRefVS;
   delete iTrackRefV;

   cout << " Tracks reaching TOF event = " <<  nAReachTOF << endl; 
   cout << " Tracks Good Match TOF event = " << nAGMatchTOF << endl; 
   cout << " Tracks Bad Match TOF event = "  << nABMatchTOF << endl; 
   
   timer.Stop(); timer.Print();
   
   TFile *file = new TFile("TestTOF.root","recreate");
   hprimP->Write();

   hSector->Write();
   hSectorM->Write();
   hSectorMG->Write();
   hSectorMF->Write();
   hprimPpi->Write();
   hprimPka->Write();
   hprimPpr->Write();
   hprimPTOF->Write();
   hprimPTOFpi->Write();
   hprimPTOFka->Write();
   hprimPTOFpr->Write();
   hprimPTOF3->Write();
   hprimPTOFpi3->Write();
   hprimPTOFka3->Write();
   hprimPTOFpr3->Write();
   hprimPTOF4->Write();
   hprimPTOFpi4->Write();
   hprimPTOFka4->Write();
   hprimPTOFpr4->Write();
   hprimPTOF34->Write();
   hprimPTOFpi34->Write();
   hprimPTOFka34->Write();
   hprimPTOFpr34->Write();
   piP->Write();
   piR->Write();
   piG->Write();
   piF->Write();
   kaP->Write();
   kaR->Write();
   kaG->Write();
   kaF->Write();
   prP->Write();
   prR->Write();
   prG->Write();
   prF->Write();




   file->Close();
//------------------------------------------
   rl->UnloadHeader();
   rl->UnloadgAlice();
   if (gAlice) {
      delete gAlice->GetRunLoader();
      delete gAlice;
      gAlice = 0x0;
   }
   delete rl;
   ef->Close();
   return 0;
}


