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

// --- ROOT system ---
#include <TClonesArray.h>
#include <TFile.h> 
#include <TH1F.h> 
#include <TH2F.h>
#include <TProfile.h>
#include <Riostream.h>
// --- Standard library ---

// --- AliRoot header files ---
#include "AliESDCaloCluster.h"
#include "AliESDEvent.h"
#include "AliQAChecker.h"
#include "AliLog.h"
#include "AliHMPIDDigit.h"
#include "AliHMPIDHit.h"
#include "AliHMPIDDigit.h"
#include "AliHMPIDCluster.h"
#include "AliHMPIDQADataMakerRec.h"
#include "AliHMPIDQAChecker.h"
#include "AliHMPIDParam.h"
#include "AliHMPIDRawStream.h"
#include "AliLog.h"

//.
// HMPID AliHMPIDQADataMakerRec base class
// for QA of reconstruction
// here also errors are calculated
//.

ClassImp(AliHMPIDQADataMakerRec)
           
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  AliHMPIDQADataMakerRec::AliHMPIDQADataMakerRec() : 
  AliQADataMakerRec(AliQAv1::GetDetName(AliQAv1::kHMPID), "HMPID Quality Assurance Data Maker"),fEvtRaw(0), fChannel(0)
{
  // ctor
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AliHMPIDQADataMakerRec::AliHMPIDQADataMakerRec(const AliHMPIDQADataMakerRec& qadm) :
  AliQADataMakerRec(),fEvtRaw(qadm.fEvtRaw), fChannel(qadm.fChannel)
{
  //copy ctor 
  SetName((const char*)qadm.GetName()) ; 
  SetTitle((const char*)qadm.GetTitle()); 
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AliHMPIDQADataMakerRec& AliHMPIDQADataMakerRec::operator = (const AliHMPIDQADataMakerRec& qadm )
{
  // Equal operator.
  this->~AliHMPIDQADataMakerRec();
  new(this) AliHMPIDQADataMakerRec(qadm);
  return *this;
}
 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDQADataMakerRec::InitDigits()
{
  // create Digits histograms in Digits subdir
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 
  
  TH1F *hDigChEvt = new TH1F("hDigChEvt","Chamber occupancy per event;Occupancy [%];Counts",AliHMPIDParam::kMaxCh+1,AliHMPIDParam::kMinCh,AliHMPIDParam::kMaxCh+1);
  TH1F *hDigPcEvt = new TH1F("hDigPcEvt","PC occupancy",156,-1,77);
  TH2F *hDigMap[7];
  TH1F *hDigQ[42];
  for(Int_t iCh =0; iCh < 7; iCh++){
    hDigMap[iCh] = new TH2F(Form("MapCh%i",iCh),Form("Digit Map in Chamber %i",iCh),159,0,159,143,0,143);
    for(Int_t iPc =0; iPc < 6; iPc++ ){
      hDigQ[iCh*6+iPc] = new TH1F(Form("QCh%iPc%i        ",iCh,iPc),Form("Charge of digits (ADC) in Chamber %i and PC %i;Charge [ADC counts];Counts",iCh,iPc),4100,0,4100);
    }
  }
  
  Add2DigitsList(hDigChEvt,0, !expert, image);
  Add2DigitsList(hDigPcEvt,1,expert, !image);
  for(Int_t iMap=0; iMap < 7; iMap++) Add2DigitsList(hDigMap[iMap],2+iMap,expert, !image);
  for(Int_t iH =0; iH < 42 ; iH++) Add2DigitsList(hDigQ[iH]    ,9+iH,expert,!image);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDQADataMakerRec::InitRecPoints()
{
  // create cluster histograms in RecPoint subdir
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 
 
  TProfile *hCluMult          = new TProfile("CluMult"   ,"Cluster multiplicity per chamber;Chamber Id;# of clusters"    , 16, -1 , 7  , 0, 500);
  Add2RecPointsList(hCluMult    , 0,expert, !image);
  
  TH2F *hCluFlg          = new TH2F("CluFlg"      ,"Cluster flag;??;??"                              ,  56  ,-1.5, 12.5, 70, -0.5, 6.5);
  Add2RecPointsList(hCluFlg    , 1,expert, !image);
  
  TH1F *hCluSizeMip[7], *hCluSizePho[7];
  
  TH1F *hCluQSect[42], *hCluQSectZoom[42];
  
  for(Int_t iCh =0; iCh <7; iCh++){
    hCluSizeMip[iCh] = new TH1F(Form("CluSizeMipCh%i",iCh),Form("Cluster size  MIP  (cluster Q > 100 ADC) in Chamber %i;Size [MIP];Counts",iCh),  50  , 0  , 50  );
    Add2RecPointsList(hCluSizeMip[iCh], iCh+2,expert,!image);

    hCluSizePho[iCh]  = new TH1F(Form("CluSizePho%i",iCh ),Form("Cluster size  Phots(cluster Q < 100 ADC) in Chamber %i;Size [MIP];Counts",iCh),  50  , 0  , 50  );
    Add2RecPointsList(hCluSizePho[iCh], iCh+7+2,expert,!image);
    
    for(Int_t iSect =0; iSect < 6; iSect++){
      hCluQSectZoom[iCh*6+iSect] = new TH1F(Form("QClusCh%iSect%iZoom",iCh,iSect) ,Form("Zoom on Cluster charge (ADC) in Chamber %i and sector %i;Charge [ADC counts];Counts",iCh,iSect),100,0,100);
      Add2RecPointsList(hCluQSectZoom[iCh*6+iSect],2+14+iCh*6+iSect,expert,!image);
      
      hCluQSect[iCh*6+iSect] = new TH1F(Form("QClusCh%iSect%i",iCh,iSect) ,Form("Cluster charge (ADC) in Chamber %i and sector %i;Charge [ADC counts];Counts",iCh,iSect),250,0,5000);
      Add2RecPointsList(hCluQSect[iCh*6+iSect],2+14+42+iCh*6+iSect, !expert, image);
    }  
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDQADataMakerRec::InitRaws()
{
//
// Booking QA histo for Raw data
//
// All histograms implemented in InitRaws are used in AMORE. Any change here should be propagated to the amoreHMP-QA as well!!! (clm)
//  
  
  const Bool_t expert   = kTRUE ; 
  const Bool_t saveCorr = kTRUE ; 
  const Bool_t image    = kTRUE ; 
  
  const Int_t kNerr = (Int_t)AliHMPIDRawStream::kSumErr+1;
  TH1F *hSumErr[14];
  TH2F *hDilo[14];
  TH2I *hPadMap[42]; //AMORE monitoring
  TH1I *hPadQ[42]; //AMORE monitoring
  for(Int_t iddl =0; iddl<AliHMPIDRawStream::kNDDL; iddl++) {
    
    hSumErr[iddl] = new TH1F(Form("hSumErrDDL%i",iddl), Form("Error summary for DDL %i;??;??",iddl), 2*kNerr,0,2*kNerr);
    for(Int_t ilabel=0; ilabel< kNerr; ilabel++) {
      hSumErr[iddl]->GetXaxis()->CenterLabels(kTRUE);
      hSumErr[iddl]->GetXaxis()->SetBinLabel((2*ilabel+1),Form("%i  %s",ilabel+1,AliHMPIDRawStream::GetErrName(ilabel)));
    }
    
    Add2RawsList(hSumErr[iddl],iddl,expert,!image, !saveCorr);
    
    hDilo[iddl] = new TH2F(Form("hDiloDDL%i",iddl),Form("Dilogic response at DDL;Row # ;Dilogic #",iddl),24,1,25,10,1,11);
    Add2RawsList(hDilo[iddl],14+iddl,expert,!image, !saveCorr);
  }//DDL loop
  for(Int_t iCh = AliHMPIDParam::kMinCh; iCh <=AliHMPIDParam::kMaxCh ;iCh++) {
    for(Int_t iPc = AliHMPIDParam::kMinPc; iPc <= AliHMPIDParam::kMaxPc ;iPc++) {
      hPadMap[iPc+6*iCh] = new TH2I(Form("hPadMap_Ch_%i_Pc%i",iCh,iPc),Form("Pad Map of Ch: %i Pc: %i;Pad X;Pad Y;",iCh,iPc),80,0,80,48,0,48);
      Add2RawsList(hPadMap[iPc+6*iCh],28+iPc+6*iCh,expert,!image, !saveCorr); 
      hPadQ[iPc+6*iCh]   = new TH1I(Form("hPadQ_Ch_%i_Pc%i",iCh,iPc),Form("Pad Charge of Ch: %i Pc: %i;Pad Q;Entries;",iCh,iPc),4100,0,4100);
      Add2RawsList(hPadQ[iPc+6*iCh],70+iPc+6*iCh,expert,!image, !saveCorr); 
    }//PC loop
  }//Ch loop  
  
  TH2I *hGeneralErrorSummary = new TH2I("GeneralErrorSummary"," DDL index vs Error type plot", 2*kNerr, 0, 2*kNerr, 2*AliHMPIDRawStream::kNDDL,0,2*AliHMPIDRawStream::kNDDL);
  for(Int_t igenlabel =0 ; igenlabel< kNerr; igenlabel++) hGeneralErrorSummary->GetXaxis()->SetBinLabel((2*igenlabel+1),Form("%i  %s",igenlabel+1,AliHMPIDRawStream::GetErrName(igenlabel)));
  Add2RawsList(hGeneralErrorSummary,14+14+42+42, !expert, image, !saveCorr);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDQADataMakerRec::InitESDs()
{
  //
  //Booking ESDs histograms
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 

  TH2F*  hCkovP  = new TH2F("CkovP" , "#theta_{c}, [rad];P, [GeV]"   , 150,      0,  7  ,100, 0, 1)   ;
  TH2F*  hSigP   = new TH2F("SigP"  ,"#sigma_{#theta_c} [mrad];[GeV]", 150,      0,  7  ,100, 0, 1)   ;
  TH2F*  hDifXY  = new TH2F("DifXY" ,"diff"                          , 200,    -10, 10  ,200,-10,10)  ;
  TH2F*  hMvsP = new TH2F("MvsP","Reconstructed Mass vs P",60,0,6,1000,0,1) ;
  TH1F*  hPid[5];
  hPid[0] = new TH1F("PidE" ,"electron response"              , 101, -0.005,1.005)             ;
  hPid[1] = new TH1F("PidMu","#mu response"                   , 101, -0.005,1.005)             ;
  hPid[2] = new TH1F("PidPi","#pi response"                   , 101, -0.005,1.005)             ;
  hPid[3] = new TH1F("PidK" ,"K response"                     , 101, -0.005,1.005)             ;
  hPid[4] = new TH1F("PidP" ,"p response"                     , 101, -0.005,1.005)             ;
  
  Add2ESDsList(hCkovP,0, !expert, image);
  Add2ESDsList(hSigP ,1, expert, !image);
  Add2ESDsList(hDifXY,2, !expert, image);
  Add2ESDsList(hMvsP,3, expert, !image);
  for(Int_t i=0; i< 5; i++) Add2ESDsList(hPid[i],i+4, expert, !image);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDQADataMakerRec::MakeRaws(AliRawReader *rawReader)
{
//
// Filling Raws QA histos
//
	  rawReader->Reset() ; 
		AliHMPIDRawStream stream(rawReader);

    fEvtRaw++;

    while(stream.Next())
     {
       UInt_t ddl=stream.GetDDLNumber(); //returns 0,1,2 ... 13
       if(ddl > 13) continue;
       for(Int_t iErr =1; iErr<(Int_t)AliHMPIDRawStream::kSumErr; iErr++){
         Int_t numOfErr = stream.GetErrors(ddl,iErr);
         GetRawsData(ddl)->Fill(iErr,numOfErr);
         //Printf("err type %i   ddl number %i  Num of errors %i",iErr,ddl,numOfErr);
        ((TH2I*)GetRawsData(14+14+42+42))->Fill(iErr,ddl,iErr); //
       }
       UInt_t word; Int_t Nddl, r, d, a;//pc,pcX,pcY;      
       for(Int_t iPad=0;iPad<stream.GetNPads();iPad++) {
       AliHMPIDDigit dig(stream.GetPadArray()[iPad],stream.GetChargeArray()[iPad]);dig.Raw(word,Nddl,r,d,a);
       GetRawsData(ddl+14)->Fill(r,d);
       GetRawsData(28+stream.Pc(Nddl,r,d,a)+6*AliHMPIDParam::DDL2C(ddl))->Fill(stream.PadPcX(Nddl,r,d,a),stream.PadPcY(Nddl,r,d,a));
       GetRawsData(70+stream.Pc(Nddl,r,d,a)+6*AliHMPIDParam::DDL2C(ddl))->Fill(stream.GetChargeArray()[iPad]);
       }      
     }

    //   stream.Delete();
   
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDQADataMakerRec::MakeDigits()
{
  //
  //filling QA histos for Digits
  //

  Int_t i = fChannel ; 
  GetDigitsData(0)->Fill(i,fDigitsArray->GetEntriesFast()/(48.*80.*6.));
  TIter next(fDigitsArray); 
  AliHMPIDDigit * digit; 
  while ( (digit = dynamic_cast<AliHMPIDDigit *>(next())) ) {
    GetDigitsData(1)->Fill(10.*i+digit->Pc(),1./(48.*80.));
    GetDigitsData(2+i)->Fill(digit->PadChX(),digit->PadChY());
    GetDigitsData(9+i*6+digit->Pc())->Fill(digit->Q());
  }  
}  
  
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDQADataMakerRec::MakeDigits(TTree * data)
{
  //
  //Opening the Digit Tree
  //

  if(fDigitsArray) 
    fDigitsArray->Clear() ; 
  else
    fDigitsArray=new TClonesArray("AliHMPIDDigit");
  
  for(Int_t iCh=AliHMPIDParam::kMinCh;iCh<=AliHMPIDParam::kMaxCh;iCh++){
    fChannel = iCh ; 
    fDigitsArray->Clear() ; 
    data->SetBranchAddress(Form("HMPID%i",iCh),&fDigitsArray);
    data->GetEntry(0);
    MakeDigits();
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDQADataMakerRec::MakeRecPoints(TTree * clustersTree)
{
  //
  //filling QA histos for clusters
  //
  AliHMPIDParam *pPar =AliHMPIDParam::Instance();
 
  if (fRecPointsArray) 
    fRecPointsArray->Clear() ; 
  else 
    fRecPointsArray = new TClonesArray("AliHMPIDCluster");
  
  for(Int_t iCh=AliHMPIDParam::kMinCh;iCh<=AliHMPIDParam::kMaxCh;iCh++){
    TBranch *branch = clustersTree->GetBranch(Form("HMPID%d",iCh));
    branch->SetAddress(&fRecPointsArray);
    branch->GetEntry(0);
    GetRecPointsData(0)->Fill(iCh,fRecPointsArray->GetEntries());
    TIter next(fRecPointsArray);
    AliHMPIDCluster *clu;
    while ( (clu = dynamic_cast<AliHMPIDCluster *>(next())) ) {
      GetRecPointsData(1)->Fill(clu->Status(),iCh);
      Int_t sect =  pPar->InHVSector(clu->Y());
      if(clu->Q()>100) GetRecPointsData(2+iCh)->Fill(clu->Size());
      else {
        GetRecPointsData(2+7+iCh)->Fill(clu->Size());
        GetRecPointsData(2+14+iCh*6+sect)->Fill(clu->Q());
      }    
      GetRecPointsData(2+14+42+iCh*6+sect)->Fill(clu->Q());
    }
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDQADataMakerRec::MakeESDs(AliESDEvent * esd)
{
  //
  //fills QA histos for ESD
  //
 
  for(Int_t iTrk = 0 ; iTrk < esd->GetNumberOfTracks() ; iTrk++){
    AliESDtrack *pTrk = esd->GetTrack(iTrk) ;
    GetESDsData(0)->Fill(pTrk->GetP(),pTrk->GetHMPIDsignal());
    GetESDsData(1)->Fill( pTrk->GetP(),TMath::Sqrt(pTrk->GetHMPIDchi2()));
    Float_t xm,ym; Int_t q,np;  
    pTrk->GetHMPIDmip(xm,ym,q,np);                       //mip info
    Float_t xRad,yRad,th,ph;        
    pTrk->GetHMPIDtrk(xRad,yRad,th,ph);              //track info at the middle of the radiator
    Float_t xPc = xRad+9.25*TMath::Tan(th)*TMath::Cos(ph); // temporar: linear extrapol (B=0!)
    Float_t yPc = yRad+9.25*TMath::Tan(th)*TMath::Sin(ph); // temporar:          "
    GetESDsData(2)->Fill(xm-xPc,ym-yPc); //track info
    if(pTrk->GetHMPIDsignal()>0) {
     Double_t a = 1.292*1.292*TMath::Cos(pTrk->GetHMPIDsignal())*TMath::Cos(pTrk->GetHMPIDsignal())-1.;
     if(a > 0) {
    Double_t mass = pTrk->P()*TMath::Sqrt(1.292*1.292*TMath::Cos(pTrk->GetHMPIDsignal())*TMath::Cos(pTrk->GetHMPIDsignal())-1.);
    GetESDsData(3)->Fill( pTrk->GetP(),mass);
     }
    }
   Double_t pid[5] ;      pTrk->GetHMPIDpid(pid) ;
    for(Int_t i = 0 ; i < 5 ; i++) GetESDsData(4+i)->Fill(pid[i]) ;
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDQADataMakerRec::StartOfDetectorCycle()
{
  //Detector specific actions at start of cycle
  
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void AliHMPIDQADataMakerRec::EndOfDetectorCycle(AliQAv1::TASKINDEX_t task, TObjArray **histos)
{
  //Detector specific actions at end of cycle
  // do the QA checking
  
  if(task==AliQAv1::kRAWS) {
    for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) {
      if (! IsValidEventSpecie(specie, histos) )
        continue ;
      for(Int_t iddl=0;iddl<14;iddl++) {
        TH1F *h = (TH1F*)histos[specie]->At(14+iddl); //ddl histos scaled by the number of events 
        h->Scale(1./(Float_t)fEvtRaw);
      }
    }
  }
   AliQAChecker::Instance()->Run(AliQAv1::kHMPID, task, histos);
}

