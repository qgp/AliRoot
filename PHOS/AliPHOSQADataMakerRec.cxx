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

/*
  Produces the data needed to calculate the quality assurance. 
  All data must be mergeable objects.
  Y. Schutz CERN July 2007
*/

// --- ROOT system ---
#include <TClonesArray.h>
#include <TFile.h> 
#include <TH1F.h> 
#include <TH1I.h> 
#include <TH2F.h> 

// --- Standard library ---

// --- AliRoot header files ---
#include "AliESDCaloCluster.h"
#include "AliESDEvent.h"
#include "AliLog.h"
#include "AliPHOSQADataMakerRec.h"
#include "AliQAChecker.h"
#include "AliPHOSCpvRecPoint.h" 
#include "AliPHOSEmcRecPoint.h" 
#include "AliPHOSRecParticle.h" 
#include "AliPHOSTrackSegment.h" 
#include "AliPHOSRawDecoder.h"
#include "AliPHOSRawDecoderv1.h"
#include "AliPHOSRawDecoderv2.h"
#include "AliPHOSReconstructor.h"
#include "AliPHOSRecoParam.h"

ClassImp(AliPHOSQADataMakerRec)
           
//____________________________________________________________________________ 
  AliPHOSQADataMakerRec::AliPHOSQADataMakerRec() : 
  AliQADataMakerRec(AliQA::GetDetName(AliQA::kPHOS), "PHOS Quality Assurance Data Maker")
{
  // ctor
}

//____________________________________________________________________________ 
AliPHOSQADataMakerRec::AliPHOSQADataMakerRec(const AliPHOSQADataMakerRec& qadm) :
  AliQADataMakerRec()
{
  //copy ctor 
  SetName((const char*)qadm.GetName()) ; 
  SetTitle((const char*)qadm.GetTitle()); 
}

//__________________________________________________________________
AliPHOSQADataMakerRec& AliPHOSQADataMakerRec::operator = (const AliPHOSQADataMakerRec& qadm )
{
  // Equal operator.
  this->~AliPHOSQADataMakerRec();
  new(this) AliPHOSQADataMakerRec(qadm);
  return *this;
}
 
//____________________________________________________________________________ 
void AliPHOSQADataMakerRec::EndOfDetectorCycle(AliQA::TASKINDEX task, TObjArray * list)
{
  //Detector specific actions at end of cycle
  // do the QA checking
  AliQAChecker::Instance()->Run(AliQA::kPHOS, task, list) ;  
}

//____________________________________________________________________________ 
void AliPHOSQADataMakerRec::InitESDs()
{
  //Create histograms to controll ESD
 
  TH1F * h1 = new TH1F("hESDPhosSpectrum",  "ESDs spectrum in PHOS",    200, 0., 20.) ; 
  h1->Sumw2() ;
  Add2ESDsList(h1, kESDSpec)  ;                                                                                                        
  TH1I * h2 = new TH1I("hESDPhosMul", "ESDs multiplicity distribution in PHOS", 100, 0,  100) ; 
  h2->Sumw2() ;
  Add2ESDsList(h2, kESDNtot) ;
 
  TH1I * h3 = new TH1I("hESDPhosEtot", "ESDs Etot", 100, 0,  1000.) ; 
  h3->Sumw2() ;
  Add2ESDsList(h3, kESDEtot) ;
 
  TH1F * h4 = new TH1F("hESDpid",    "ESDs PID distribution in PHOS",       100, 0., 1.) ;
  h4->Sumw2() ;
  Add2ESDsList(h4, kESDpid) ;
	
}

//____________________________________________________________________________ 
void AliPHOSQADataMakerRec::InitRecPoints()
{
  // create Reconstructed Points histograms in RecPoints subdir
  TH2I * h0 = new TH2I("hRpPHOSxyMod1","RecPoints Rows x Columns for PHOS module 1", 64, -72., 72., 56, -63., 63.) ;                             
  Add2RecPointsList(h0,kRPmod1) ;
  TH2I * h1 = new TH2I("hRpPHOSxyMod2","RecPoints Rows x Columns for PHOS module 2", 64, -72., 72., 56, -63., 63.) ;                             
  Add2RecPointsList(h1,kRPmod2) ;
  TH2I * h2 = new TH2I("hRpPHOSxyMod3","RecPoints Rows x Columns for PHOS module 3", 64, -72., 72., 56, -63., 63.) ;                             
  Add2RecPointsList(h2,kRPmod3) ;
  TH2I * h3 = new TH2I("hRpPHOSxyMod4","RecPoints Rows x Columns for PHOS module 4", 64, -72., 72., 56, -63., 63.) ;                             
  Add2RecPointsList(h3,kRPmod4) ;
  TH2I * h4 = new TH2I("hRpPHOSxyMod5","RecPoints Rows x Columns for PHOS module 5", 64, -72., 72., 56, -63., 63.) ;                             
  Add2RecPointsList(h4,kRPmod5) ;
 
  TH1F * h5 = new TH1F("hEmcPhosRecPointsSpectrum",  "EMC RecPoints spectrum in PHOS",   2000, 0., 20.) ; 
  h5->Sumw2() ;
  Add2RecPointsList(h5, kRPSpec)  ;

  TH1I * h6 = new TH1I("hEmcPhosRecPointsMul", "EMCA RecPoints multiplicity distribution in PHOS", 100, 0,  100) ; 
  h6->Sumw2() ;
  Add2RecPointsList(h6, kRPNtot) ;

  TH1I * h7 = new TH1I("hEmcPhosRecPointsEtot", "EMC RecPoints Etot", 200, 0,  200.) ; 
  h7->Sumw2() ;
  Add2RecPointsList(h7, kRPEtot) ;

  TH1I * h8 = new TH1I("hCpvPhosRecPointsMul", "CPV RecPoints multiplicity distribution in PHOS", 100, 0,  100) ; 
  h8->Sumw2() ;
  Add2RecPointsList(h8, kRPNcpv) ;
}

//____________________________________________________________________________ 
void AliPHOSQADataMakerRec::InitRaws()
{
  // create Raws histograms in Raws subdir
  TH2I * h0 = new TH2I("hHighPHOSxyMod1","High Gain Rows x Columns for PHOS module 1", 64, 0, 64, 56, 0, 56) ;
  Add2RawsList(h0,kHGmod1) ;
  TH2I * h1 = new TH2I("hHighPHOSxyMod2","High Gain Rows x Columns for PHOS module 2", 64, 0, 64, 56, 0, 56) ;
  Add2RawsList(h1,kHGmod2) ;
  TH2I * h2 = new TH2I("hHighPHOSxyMod3","High Gain Rows x Columns for PHOS module 3", 64, 0, 64, 56, 0, 56) ;
  Add2RawsList(h2,kHGmod3) ;
  TH2I * h3 = new TH2I("hHighPHOSxyMod4","High Gain Rows x Columns for PHOS module 4", 64, 0, 64, 56, 0, 56) ;
  Add2RawsList(h3,kHGmod4) ;
  TH2I * h4 = new TH2I("hHighPHOSxyMod5","High Gain Rows x Columns for PHOS module 5", 64, 0, 64, 56, 0, 56) ;
  Add2RawsList(h4,kHGmod5) ;
  TH2I * h5 = new TH2I("hLowPHOSxyMod1","Low Gain Rows x Columns for PHOS module 1", 64, 0, 64, 56, 0, 56) ;
  Add2RawsList(h5,kLGmod1) ;
  TH2I * h6 = new TH2I("hLowPHOSxyMod2","Low Gain Rows x Columns for PHOS module 2", 64, 0, 64, 56, 0, 56) ;
  Add2RawsList(h6,kLGmod2) ;
  TH2I * h7 = new TH2I("hLowPHOSxyMod3","Low Gain Rows x Columns for PHOS module 3", 64, 0, 64, 56, 0, 56) ;
  Add2RawsList(h7,kLGmod3) ;
  TH2I * h8 = new TH2I("hLowPHOSxyMod4","Low Gain Rows x Columns for PHOS module 4", 64, 0, 64, 56, 0, 56) ;
  Add2RawsList(h8,kLGmod4) ;                                                                                                               
  TH2I * h9 = new TH2I("hLowPHOSxyMod5","Low Gain Rows x Columns for PHOS module 5", 64, 0, 64, 56, 0, 56) ;                               
  Add2RawsList(h9,kLGmod5) ;                                                                                                               
                                                                                                                                           
                                                                                                                                           
  TH1I * h10 = new TH1I("hLowPhosModules",    "Low Gain Hits in EMCA PHOS modules",       6, 0, 6) ;                                       
  h10->Sumw2() ;                                                                                                                           
  Add2RawsList(h10, kNmodLG) ;                                                                                                             
  TH1I * h11 = new TH1I("hHighPhosModules",   "High Gain Hits in EMCA PHOS modules",       6, 0, 6) ;                                      
  h11->Sumw2() ;                                                                                                                           
  Add2RawsList(h11, kNmodHG) ;                                                                                                             
                                                                                                                                           
  TH1F * h12 = new TH1F("hLowPhosRawtime", "Low Gain Time of raw hits in PHOS", 500, -50., 200.) ;                                            
  h12->Sumw2() ;                                                                                                                           
  Add2RawsList(h12, kLGtime) ;                                                                                                             
  TH1F * h13 = new TH1F("hHighPhosRawtime", "High Gain Time of raw hits in PHOS", 500, -50., 200.) ;                                          
  h13->Sumw2() ;                                                                                                                           
  Add2RawsList(h13, kHGtime) ;                                                                                                             
                                                                                                                                           
  TH1F * h14 = new TH1F("hLowPhosRawEnergy", "Low Gain Energy of raw hits in PHOS", 500, 0., 1000.) ;                                      
  h14->Sumw2() ;                                                                                                                           
  Add2RawsList(h14, kSpecLG) ;                                                                                                             
  TH1F * h15 = new TH1F("hHighPhosRawEnergy", "High Gain Energy of raw hits in PHOS",500,0., 1000.) ;                                      
  h15->Sumw2() ;                                                                                                                           
  Add2RawsList(h15, kSpecHG) ;                                                                                                             
                                                                                                                                           
  TH1F * h16 = new TH1F("hLowNtot", "Low Gain Total Number of raw hits in PHOS", 500, 0., 5000.) ;                                         
  h16->Sumw2() ;                                                                                                                           
  Add2RawsList(h16, kNtotLG) ;                                                                                                             
  TH1F * h17 = new TH1F("hHighNtot", "High Gain Total Number of raw hits in PHOS",500,0., 5000.) ;                                         
  h17->Sumw2() ;                                                                                                                           
  Add2RawsList(h17, kNtotHG) ;                                                                                                             
                                                                                                                                           
  TH1F * h18 = new TH1F("hLowEtot", "Low Gain Total Energy of raw hits in PHOS", 500, 0., 5000.) ;                                       
  h18->Sumw2() ;                                                                                                                           
  Add2RawsList(h18, kEtotLG) ;                                                                                                             
  TH1F * h19 = new TH1F("hHighEtot", "High Gain Total Energy of raw hits in PHOS",500,0., 100000.) ;                                       
  h19->Sumw2() ;                                                                                                                           
  Add2RawsList(h19, kEtotHG) ;                                                                                                             
  
}

//____________________________________________________________________________
void AliPHOSQADataMakerRec::MakeESDs(AliESDEvent * esd)
{
  // make QA data from ESDs

  Int_t nTot = 0 ; 
  Double_t eTot = 0 ; 
  for ( Int_t index = 0; index < esd->GetNumberOfCaloClusters() ; index++ ) {
    AliESDCaloCluster * clu = esd->GetCaloCluster(index) ;
    if( clu->IsPHOS() ) {
      GetESDsData(kESDSpec)->Fill(clu->E()) ;
      Double_t *pid=clu->GetPid() ;
      GetESDsData(kESDpid)->Fill(pid[AliPID::kPhoton]) ;
      eTot+=clu->E() ;
      nTot++ ;
    } 
  }
  GetESDsData(kESDNtot)->Fill(nTot) ;
  GetESDsData(kESDEtot)->Fill(eTot) ;
}

//____________________________________________________________________________
void AliPHOSQADataMakerRec::MakeRaws(AliRawReader* rawReader)
{
  //Fill prepared histograms with Raw digit properties
  rawReader->Reset() ;
  AliPHOSRawDecoder * decoder ;
  if(strcmp(AliPHOSReconstructor::GetRecoParamEmc()->DecoderVersion(),"v1")==0)
    decoder=new AliPHOSRawDecoderv1(rawReader);
  else
    if(strcmp(AliPHOSReconstructor::GetRecoParamEmc()->DecoderVersion(),"v2")==0)
      decoder=new AliPHOSRawDecoderv2(rawReader);
    else
      decoder=new AliPHOSRawDecoder(rawReader);
  decoder->SetOldRCUFormat  (AliPHOSReconstructor::GetRecoParamEmc()->IsOldRCUFormat());
  decoder->SubtractPedestals(AliPHOSReconstructor::GetRecoParamEmc()->SubtractPedestals());
  Double_t lgEtot=0. ;
  Double_t hgEtot=0. ;
  Int_t lgNtot=0 ;
  Int_t hgNtot=0 ;

  while (decoder->NextDigit()) {
   Int_t module  = decoder->GetModule() ;
   Int_t row     = decoder->GetRow() ;
   Int_t col     = decoder->GetColumn() ;
   Double_t time = decoder->GetTime() ;
   Double_t energy  = decoder->GetEnergy() ;
   Bool_t lowGain = decoder->IsLowGain();
   if (lowGain) {
     if(energy<2.)
       continue ;
     switch(module){
        case 1: GetRawsData(kLGmod1)->Fill(row-0.5,col-0.5) ; break ;
        case 2: GetRawsData(kLGmod2)->Fill(row-0.5,col-0.5) ; break ;
        case 3: GetRawsData(kLGmod3)->Fill(row-0.5,col-0.5) ; break ;
        case 4: GetRawsData(kLGmod4)->Fill(row-0.5,col-0.5) ; break ;
        case 5: GetRawsData(kLGmod5)->Fill(row-0.5,col-0.5) ; break ;
     }                                  
     GetRawsData(kNmodLG)->Fill(module) ;
     GetRawsData(kLGtime)->Fill(time) ; 
     GetRawsData(kSpecLG)->Fill(energy) ;    
     lgEtot+=energy ;
     lgNtot++ ;   
   } else {        
     if(energy<8.)
       continue ;
     switch (module){
         case 1: GetRawsData(kHGmod1)->Fill(row-0.5,col-0.5) ; break ;
         case 2: GetRawsData(kHGmod2)->Fill(row-0.5,col-0.5) ; break ;
         case 3: GetRawsData(kHGmod3)->Fill(row-0.5,col-0.5) ; break ;
         case 4: GetRawsData(kHGmod4)->Fill(row-0.5,col-0.5) ; break ;
         case 5: GetRawsData(kHGmod5)->Fill(row-0.5,col-0.5) ; break ;
     }              
     GetRawsData(kNmodHG)->Fill(module) ; 
     GetRawsData(kHGtime)->Fill(time) ;  
     GetRawsData(kSpecHG)->Fill(energy) ;
     hgEtot+=energy ; 
     hgNtot++ ;  
   }                 
  }                    
  GetRawsData(kEtotLG)->Fill(lgEtot) ; 
  GetRawsData(kEtotHG)->Fill(hgEtot) ;  
  GetRawsData(kNtotLG)->Fill(lgNtot) ;
  GetRawsData(kNtotHG)->Fill(hgNtot) ;
}
//____________________________________________________________________________
void AliPHOSQADataMakerRec::MakeRecPoints(TTree * clustersTree)
{
  {
    // makes data from RecPoints
    TBranch *emcbranch = clustersTree->GetBranch("PHOSEmcRP");
    if (!emcbranch) { 
      AliError("can't get the branch with the PHOS EMC clusters !");
      return;
    }
    TObjArray * emcrecpoints = new TObjArray(100) ;
    emcbranch->SetAddress(&emcrecpoints);
    emcbranch->GetEntry(0);
    
    GetRecPointsData(kRPNtot)->Fill(emcrecpoints->GetEntriesFast()) ; 
    TIter next(emcrecpoints) ; 
    AliPHOSEmcRecPoint * rp ; 
    Double_t eTot = 0. ; 
    while ( (rp = dynamic_cast<AliPHOSEmcRecPoint *>(next())) ) {
      GetRecPointsData(kRPSpec)->Fill( rp->GetEnergy()) ;
      Int_t mod = rp->GetPHOSMod() ;
      TVector3 pos ;
      rp->GetLocalPosition(pos) ;
      switch(mod){
        case 1: GetRecPointsData(kRPmod1)->Fill(pos.X(),pos.Z()) ; break ;
        case 2: GetRecPointsData(kRPmod2)->Fill(pos.X(),pos.Z()) ; break ;
        case 3: GetRecPointsData(kRPmod3)->Fill(pos.X(),pos.Z()) ; break ;
        case 4: GetRecPointsData(kRPmod4)->Fill(pos.X(),pos.Z()) ; break ;
        case 5: GetRecPointsData(kRPmod5)->Fill(pos.X(),pos.Z()) ; break ;
      }
      eTot+= rp->GetEnergy() ;
    }
    GetRecPointsData(kRPEtot)->Fill(eTot) ;
    emcrecpoints->Delete();
    delete emcrecpoints;
  }
  {
    TBranch *cpvbranch = clustersTree->GetBranch("PHOSCpvRP");
    if (!cpvbranch) { 
      AliError("can't get the branch with the PHOS CPV clusters !");
      return;
    }
    TObjArray *cpvrecpoints = new TObjArray(100) ;
    cpvbranch->SetAddress(&cpvrecpoints);
    cpvbranch->GetEntry(0);
    
    GetRecPointsData(kRPNcpv)->Fill(cpvrecpoints->GetEntriesFast()) ; 
    cpvrecpoints->Delete();
    delete cpvrecpoints;
  }
}

//____________________________________________________________________________ 
void AliPHOSQADataMakerRec::StartOfDetectorCycle()
{
  //Detector specific actions at start of cycle
  
}
