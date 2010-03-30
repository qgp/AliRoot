/**************************************************************************
 * Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
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

//  *************************************************************
//  Checks the quality assurance 
//  by comparing with reference data
//  contained in a DB
//  -------------------------------------------------------------
//  W. Ferrarese + P. Cerello Feb 2008
//  M.Siciliano Aug 2008 QA RecPoints 
//  INFN Torino

// --- ROOT system ---

#include <TProfile2D.h>
#include <TH2D.h>
#include <TH1F.h>
#include <TBranch.h>
#include <TTree.h>
#include <TGaxis.h>
#include <TMath.h>
#include <TF1.h>
#include <TDirectory.h>
#include <TSystem.h>
// --- Standard library ---

// --- AliRoot header files ---
#include "AliITSQASDDDataMakerRec.h"
#include "AliLog.h"
#include "AliQAv1.h"
#include "AliQAChecker.h"
#include "AliRawReader.h"
#include "AliITSRawStream.h"
#include "AliITSRawStreamSDD.h"
#include "AliITSRawStreamSDDCompressed.h"
#include "AliITSDetTypeRec.h"
#include "AliITSdigit.h"
#include "AliITSRecPoint.h"
#include "AliITSRecPointContainer.h"
#include "AliITSgeomTGeo.h"
#include "AliCDBManager.h"
#include "AliCDBStorage.h"
#include "AliCDBEntry.h"
#include "Riostream.h"
#include "AliITSdigitSDD.h"
#include "AliITS.h"
#include "AliRunLoader.h"
#include "AliITSLoader.h"
#include "AliITSDetTypeRec.h"



ClassImp(AliITSQASDDDataMakerRec)

//____________________________________________________________________________ 
AliITSQASDDDataMakerRec::AliITSQASDDDataMakerRec(AliITSQADataMakerRec *aliITSQADataMakerRec, Bool_t kMode, Short_t ldc) :
TObject(),
fAliITSQADataMakerRec(aliITSQADataMakerRec),
fkOnline(kMode),
fLDC(ldc),
fSDDhRawsTask(0),
fSDDhDigitsTask(0),
fSDDhRecPointsTask(0),
fGenRawsOffset(0),
fGenDigitsOffset(0),
fGenRecPointsOffset(0),
fTimeBinSize(1),
fDDLModuleMap(0)
{
  //ctor used to discriminate OnLine-Offline analysis
  if(fLDC < 0 || fLDC > 6) {
	AliError("Error: LDC number out of range; return\n");
  }
	fGenRawsOffset = new Int_t[AliRecoParam::kNSpecies];
	fGenRecPointsOffset = new Int_t[AliRecoParam::kNSpecies];
	fGenDigitsOffset = new Int_t[AliRecoParam::kNSpecies];
	for(Int_t i=0; i<AliRecoParam::kNSpecies; i++) {
		fGenRawsOffset[i] = 0;
		fGenRecPointsOffset[i] = 0;
		fGenDigitsOffset[i]=0;
	}
}

//____________________________________________________________________________ 
AliITSQASDDDataMakerRec::AliITSQASDDDataMakerRec(const AliITSQASDDDataMakerRec& qadm) :
TObject(),
fAliITSQADataMakerRec(qadm.fAliITSQADataMakerRec),
fkOnline(qadm.fkOnline),
fLDC(qadm.fLDC),
fSDDhRawsTask(qadm.fSDDhRawsTask),
fSDDhDigitsTask(qadm.fSDDhDigitsTask),
fSDDhRecPointsTask(qadm.fSDDhRecPointsTask),
fGenRawsOffset(qadm.fGenRawsOffset),
fGenDigitsOffset(qadm.fGenDigitsOffset),
fGenRecPointsOffset(qadm.fGenRecPointsOffset),
fTimeBinSize(1),
fDDLModuleMap(0)
{
  //copy ctor 
  fAliITSQADataMakerRec->SetName((const char*)qadm.fAliITSQADataMakerRec->GetName()) ; 
  fAliITSQADataMakerRec->SetTitle((const char*)qadm.fAliITSQADataMakerRec->GetTitle());
  fDDLModuleMap=NULL;
}

//____________________________________________________________________________ 
AliITSQASDDDataMakerRec::~AliITSQASDDDataMakerRec(){
  // destructor
  //if(fDDLModuleMap) delete fDDLModuleMap;
}
//__________________________________________________________________
AliITSQASDDDataMakerRec& AliITSQASDDDataMakerRec::operator = (const AliITSQASDDDataMakerRec& qac )
{
  // Equal operator.
  this->~AliITSQASDDDataMakerRec();
  new(this) AliITSQASDDDataMakerRec(qac);
  return *this;
}

//____________________________________________________________________________ 
void AliITSQASDDDataMakerRec::StartOfDetectorCycle()
{
  //Detector specific actions at start of cycle
  AliDebug(AliQAv1::GetQADebugLevel(),"AliITSQADM::Start of SDD Cycle\n");
}

//____________________________________________________________________________ 
void AliITSQASDDDataMakerRec::EndOfDetectorCycle(AliQAv1::TASKINDEX_t /*task*/, TObjArray* /*list*/)
{
	AliDebug(AliQAv1::GetQADebugLevel(),"AliITSDM instantiates checker with Run(AliQAv1::kITS, task, list)\n"); 
}

//____________________________________________________________________________ 
Int_t AliITSQASDDDataMakerRec::InitRaws()
{ 
  // Initialization for RAW data - SDD -
  const Bool_t expert   = kTRUE ; 
  const Bool_t saveCorr = kTRUE ; 
  const Bool_t image    = kTRUE ; 



  Int_t rv = 0 ; 
  /*
  AliCDBEntry *ddlMapSDD = AliCDBManager::Instance()->Get("ITS/Calib/DDLMapSDD");
  Bool_t cacheStatus = AliCDBManager::Instance()->GetCacheFlag();
  if(!ddlMapSDD)
    {
      AliError("Calibration object retrieval failed! SDD will not be processed");
      fDDLModuleMap = NULL;
      return rv;
    }
  fDDLModuleMap = (AliITSDDLModuleMapSDD*)ddlMapSDD->GetObject();
  if(!cacheStatus)ddlMapSDD->SetObject(NULL);
  ddlMapSDD->SetOwner(kTRUE);
  if(!cacheStatus)
    {
      delete ddlMapSDD;
    }
  */
  Int_t lay, lad, det;
  Int_t indexlast = 0;
  Int_t index1 = 0;

  if(fkOnline) 
    {
      AliInfo("Book Online Histograms for SDD\n");
    }
  else 
    {
      AliInfo("Book Offline Histograms for SDD\n ");
    }
  TH1D *h0 = new TH1D("SDDModPattern","HW Modules pattern",fgknSDDmodules,239.5,499.5); //0
  h0->GetXaxis()->SetTitle("Module Number");
  h0->GetYaxis()->SetTitle("Counts");
  rv = fAliITSQADataMakerRec->Add2RawsList(h0,0+fGenRawsOffset[fAliITSQADataMakerRec->GetEventSpecie()], expert, !image, !saveCorr);
  fSDDhRawsTask++;
  
  //zPhi distribution using ladder and modules numbers
  TH2D *hphil3 = new TH2D("SDDphizL3","SDD #varphiz Layer3 ",12,0.5,6.5,14,0.5,14.5);
  hphil3->GetXaxis()->SetTitle("z[Module Number L3 ]");
  hphil3->GetYaxis()->SetTitle("#varphi[ Ladder Number L3]");
  rv = fAliITSQADataMakerRec->Add2RawsList(hphil3,1+fGenRawsOffset[fAliITSQADataMakerRec->GetEventSpecie()], !expert, image, saveCorr); 
  fSDDhRawsTask++;
  
  TH2D *hphil4 = new TH2D("SDDphizL4","SDD #varphiz Layer4 ",16,0.5,8.5,22,0.5,22.5); 
  hphil4->GetXaxis()->SetTitle("z[Module Number L4]");
  hphil4->GetYaxis()->SetTitle("#varphi[Ladder Number L4]");
   rv = fAliITSQADataMakerRec->Add2RawsList(hphil4,2+fGenRawsOffset[fAliITSQADataMakerRec->GetEventSpecie()], !expert, image, saveCorr); 
  fSDDhRawsTask++;
  

  if(fkOnline) 
    {

      //DDL Pattern 
      TH2D *hddl = new TH2D("SDDDDLPattern","SDD DDL Pattern ",24,-0.5,11.5,24,-0.5,23.5); 
      hddl->GetXaxis()->SetTitle("Channel");
      hddl->GetYaxis()->SetTitle("DDL Number");
      rv = fAliITSQADataMakerRec->Add2RawsList(hddl,3+fGenRawsOffset[fAliITSQADataMakerRec->GetEventSpecie()], expert, !image, !saveCorr);
      fSDDhRawsTask++;
      Int_t indexlast1 = 0;
  
      fTimeBinSize = 4;
      indexlast = 0;
      index1 = 0;
      indexlast1 = fSDDhRawsTask;
      char *hname[3];
      for(Int_t i=0; i<3; i++) hname[i]= new char[50];
      for(Int_t moduleSDD =0; moduleSDD<fgknSDDmodules; moduleSDD++){
	for(Int_t iside=0;iside<fgknSide;iside++){
	  AliITSgeomTGeo::GetModuleId(moduleSDD+fgkmodoffset, lay, lad, det);
	  sprintf(hname[0],"SDDchargeMapFSE_L%d_%d_%d_%d",lay,lad,det,iside);
	  sprintf(hname[1],"SDDChargeMapForSingleEvent_L%d_%d_%d_%d",lay,lad,det,iside);
	  //	  sprintf(hname[2],"SDDhmonoDMap_L%d_%d_%d_%d",lay,lad,det,iside);
	  TProfile2D *fModuleChargeMapFSE = new TProfile2D(hname[0],hname[1],256/fTimeBinSize,-0.5,255.5,256,-0.5,255.5);
	  fModuleChargeMapFSE->GetXaxis()->SetTitle("Time Bin");
	  fModuleChargeMapFSE->GetYaxis()->SetTitle("Anode");
	   rv = fAliITSQADataMakerRec->Add2RawsList(fModuleChargeMapFSE,indexlast1 + index1 + fGenRawsOffset[fAliITSQADataMakerRec->GetEventSpecie()], expert, !image, !saveCorr);	  
	  fSDDhRawsTask++;
	  index1++;	 
	}
      }
      
      for(Int_t moduleSDD =0; moduleSDD<fgknSDDmodules; moduleSDD++){
	for(Int_t iside=0;iside<fgknSide;iside++){
	  AliITSgeomTGeo::GetModuleId(moduleSDD+fgkmodoffset, lay, lad, det);
	  sprintf(hname[0],"SDDchargeMap_L%d_%d_%d_%d",lay,lad,det,iside);
	  sprintf(hname[1],"SDDChargeMap_L%d_%d_%d_%d",lay,lad,det,iside);
	  TProfile2D *fModuleChargeMap = new TProfile2D(hname[0],hname[1],256/fTimeBinSize,-0.5,255.5,256,-0.5,255.5);
	  fModuleChargeMap->GetXaxis()->SetTitle("Time Bin");
	  fModuleChargeMap->GetYaxis()->SetTitle("Anode Number");
	   rv = fAliITSQADataMakerRec->Add2RawsList(fModuleChargeMap,indexlast1 + index1 + fGenRawsOffset[fAliITSQADataMakerRec->GetEventSpecie()], expert, !image, !saveCorr);
  
	  fSDDhRawsTask++;
	  index1++;	 
	}
      }
      
      //Event Size 
      TH1F *hsize = new TH1F("SDDEventSize","SDD Event Size ",1000,-0.5,999.5); 
      hsize->GetXaxis()->SetTitle("Event Size [kB]");
      hsize->GetYaxis()->SetTitle("Entries");
      rv = fAliITSQADataMakerRec->Add2RawsList(hsize,indexlast1 + index1 + fGenRawsOffset[fAliITSQADataMakerRec->GetEventSpecie()], expert, !image, !saveCorr);
      fSDDhRawsTask++;
	  
    }  // kONLINE
  
  AliDebug(AliQAv1::GetQADebugLevel(),Form("%d SDD Raws histograms booked\n",fSDDhRawsTask));
  return rv ; 
}


//____________________________________________________________________________
Int_t AliITSQASDDDataMakerRec::MakeRaws(AliRawReader* rawReader)
{ 
  // Fill QA for RAW - SDD -
	Int_t rv = 0;
  // Check id histograms already created for this Event Specie

  if(!fDDLModuleMap){
    CreateTheMap();
    //AliError("SDD DDL module map not available - skipping SDD QA");
    //return rv;
  
  }
  if(rawReader->GetType() != 7) return rv;  // skips non physical triggers
  AliDebug(AliQAv1::GetQADebugLevel(),"entering MakeRaws\n");                 
  rawReader->Reset();       
  rawReader->Select("ITSSDD");
  AliITSRawStream *stream=AliITSRawStreamSDD::CreateRawStreamSDD(rawReader);
   stream->SetDDLModuleMap(fDDLModuleMap);
  
  Int_t lay, lad, det; 
  
  Int_t index=0;
  if(fkOnline) {
    for(Int_t moduleSDD =0; moduleSDD<fgknSDDmodules; moduleSDD++){
      for(Int_t iside=0;iside<fgknSide;iside++) {
		if(fSDDhRawsTask > 4 + index) fAliITSQADataMakerRec->GetRawsData(4 + index +fGenRawsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Reset();   
	// 4  because the 2D histos for single events start after the fourth position
	index++;
      }
    }
  }
  
  Int_t cnt = 0;
  Int_t ildcID = -1;
  Int_t iddl = -1;
  Int_t isddmod = -1;
  Int_t coord1, coord2, signal, moduleSDD, activeModule, index1; 
  //if(fkOnline)
  //{
      Int_t prevDDLID = -1;
      UInt_t size = 0;
      int totalddl=static_cast<int>(fDDLModuleMap->GetNDDLs());
      Bool_t *ddldata=new Bool_t[totalddl];
      for(Int_t jddl=0;jddl<totalddl;jddl++){ddldata[jddl]=kFALSE;}
      //}
  while(stream->Next()) {
    ildcID = rawReader->GetLDCId();
    iddl = rawReader->GetDDLID();// - fgkDDLIDshift;


    isddmod = fDDLModuleMap->GetModuleNumber(iddl,stream->GetCarlosId());
    if(isddmod==-1){
      AliDebug(AliQAv1::GetQADebugLevel(),Form("Found module with iddl: %d, stream->GetCarlosId: %d \n",iddl,stream->GetCarlosId()));
      continue;
    }
    if(stream->IsCompletedModule()) {
      AliDebug(AliQAv1::GetQADebugLevel(),Form("IsCompletedModule == KTRUE\n"));
      continue;
    } 
    if(stream->IsCompletedDDL()) {

      if(fkOnline){
	if ((rawReader->GetDDLID() != prevDDLID)&&(ddldata[iddl])==kFALSE){
	  size += rawReader->GetDataSize();//in bytes
	  prevDDLID = rawReader->GetDDLID();
	  ddldata[iddl]=kTRUE;
	}
      }
      AliDebug(AliQAv1::GetQADebugLevel(),Form("IsCompletedDDL == KTRUE\n"));
      continue;
    } 
    
    coord1 = stream->GetCoord1();
    coord2 = stream->GetCoord2();
    signal = stream->GetSignal();
    
    moduleSDD = isddmod - fgkmodoffset;
    
    if(isddmod <fgkmodoffset|| isddmod>fgknSDDmodules+fgkmodoffset-1) {
      AliDebug(AliQAv1::GetQADebugLevel(),Form( "Module SDD = %d, resetting it to 1 \n",isddmod));
      isddmod = 1;
    }
    
    AliITSgeomTGeo::GetModuleId(isddmod, lay, lad, det);
    Short_t iside = stream->GetChannel();

    //printf(" \n%i %i %i %i \n ",lay, lad, det,iside );
    fAliITSQADataMakerRec->GetRawsData( 0 + fGenRawsOffset[fAliITSQADataMakerRec->GetEventSpecie()] )->Fill(isddmod);   
    if(lay==3)    fAliITSQADataMakerRec->GetRawsData(1+fGenRawsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(det+0.5*iside-0.5,lad); 
    if(lay==4) { 
      fAliITSQADataMakerRec->GetRawsData(2+fGenRawsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(det+0.5*iside-0.5,lad);}  
 
    if(fkOnline) {
      fAliITSQADataMakerRec->GetRawsData(3+fGenRawsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill((stream->GetCarlosId())+0.5*iside -0.5,iddl);

      activeModule = moduleSDD;
      index1 = activeModule * 2 + iside;
      
      if(index1<0){
        AliDebug(AliQAv1::GetQADebugLevel(),Form("Wrong index number %d - patched to 0\n",index1));
	index1 = 0;
      }      

      if(fSDDhRawsTask > 4 + index1) {                                  
        ((TProfile2D *)(fAliITSQADataMakerRec->GetRawsData(4 + index1 +fGenRawsOffset[fAliITSQADataMakerRec->GetEventSpecie()])))->Fill(coord2, coord1, signal);     
        ((TProfile2D *)(fAliITSQADataMakerRec->GetRawsData(4 + index1 + 260*2 +fGenRawsOffset[fAliITSQADataMakerRec->GetEventSpecie()])))->Fill(coord2, coord1, signal); 
      }

    }//online
    cnt++;
    if(!(cnt%10000)) AliDebug(AliQAv1::GetQADebugLevel(),Form(" %d raw digits read",cnt));
  }//end next()
  if(fkOnline)
    {
      ((TH1F*)(fAliITSQADataMakerRec->GetRawsData(4 + 260*4 +fGenRawsOffset[fAliITSQADataMakerRec->GetEventSpecie()])))->Fill(size/1024.);//KB
    }
  AliDebug(AliQAv1::GetQADebugLevel(),Form("Event completed, %d raw digits read",cnt)); 
  delete stream;
  stream = NULL; 

//	if(fkOnline) {
//		AnalyseBNG(); // Analyse Baseline, Noise, Gain
//		AnalyseINJ(); // Analyse Injectors
//	}

  delete []ddldata;
  ddldata=NULL;
  return rv ; 
}

//____________________________________________________________________________ 
Int_t AliITSQASDDDataMakerRec::InitDigits()
{ 


  // Initialization for DIGIT data - SDD -  
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ;
  Int_t rv = 0 ; 
//  fGenDigitsOffset = (fAliITSQADataMakerRec->fDigitsQAList[AliRecoParam::kDefault])->GetEntries();
  //fSDDhTask must be incremented by one unit every time a histogram is ADDED to the QA List
  TH1F* h0=new TH1F("SDD DIGITS Module Pattern","SDD DIGITS Module Pattern",260,239.5,499.5);       //hmod
  h0->GetXaxis()->SetTitle("SDD Module Number");
  h0->GetYaxis()->SetTitle("# DIGITS");
  rv = fAliITSQADataMakerRec->Add2DigitsList(h0,fGenDigitsOffset[fAliITSQADataMakerRec->GetEventSpecie()], !expert, image);
  fSDDhDigitsTask ++;
  // printf("Add %s \t the task offset is %i\n",fAliITSQADataMakerRec->GetDigitsData(fGenDigitsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->GetName() , fSDDhDigitsTask );
  TH1F* h1=new TH1F("SDD Anode Distribution","DIGITS Anode Distribution",512,-0.5,511.5);      //hanocc
  h1->GetXaxis()->SetTitle("Anode Number");
  h1->GetYaxis()->SetTitle("# DIGITS");
  rv = fAliITSQADataMakerRec->Add2DigitsList(h1,1+fGenDigitsOffset[fAliITSQADataMakerRec->GetEventSpecie()], !expert, image);
  fSDDhDigitsTask ++;
  //printf("Add %s \t the task offset is %i\n",fAliITSQADataMakerRec->GetDigitsData(1+fGenDigitsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->GetName() , fSDDhDigitsTask );
  TH1F* h2=new TH1F("SDD Tbin Distribution","DIGITS Tbin Distribution",256,-0.5,255.5);      //htbocc
  h2->GetXaxis()->SetTitle("Tbin Number");
  h2->GetYaxis()->SetTitle("# DIGITS");
  rv = fAliITSQADataMakerRec->Add2DigitsList(h2,2+fGenDigitsOffset[fAliITSQADataMakerRec->GetEventSpecie()], !expert, image);
  fSDDhDigitsTask ++;
  //printf("Add %s \t the task offset is %i\n",fAliITSQADataMakerRec->GetDigitsData(2+fGenDigitsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->GetName() , fSDDhDigitsTask );
  TH1F* h3=new TH1F("SDD ADC Counts Distribution","DIGITS ADC Counts Distribution",200,0.,1024.);          //hsig
  h3->GetXaxis()->SetTitle("ADC Value");
  h3->GetYaxis()->SetTitle("# DIGITS");
  rv = fAliITSQADataMakerRec->Add2DigitsList(h3,3+fGenDigitsOffset[fAliITSQADataMakerRec->GetEventSpecie()], !expert, image);
  fSDDhDigitsTask ++;
  //printf("Add %s \t the task offset is %i\n",fAliITSQADataMakerRec->GetDigitsData(3+fGenDigitsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->GetName() , fSDDhDigitsTask );
  AliDebug(AliQAv1::GetQADebugLevel(),Form("%d SDD Digits histograms booked\n",fSDDhDigitsTask));
  return rv ; 
}

//____________________________________________________________________________
Int_t AliITSQASDDDataMakerRec::MakeDigits(TTree * digits)
{ 

  // Fill QA for DIGIT - SDD -
  //AliITS *fITS  = (AliITS*)gAlice->GetModule("ITS");
  //fITS->SetTreeAddress();
  //TClonesArray *iITSdigits  = fITS->DigitsAddress(1);


  Int_t rv = 0 ; 

  TBranch *branchD = digits->GetBranch("ITSDigitsSDD");

  if (!branchD) {
    AliError("can't get the branch with the ITS SDD digits !");
    return rv ;
  }
  // Check id histograms already created for this Event Specie
//  if ( ! fAliITSQADataMakerRec->GetDigitsData(fGenDigitsOffset) )
//    rv = InitDigits() ;
  
  static TClonesArray statDigits("AliITSdigitSDD");
  TClonesArray *iITSdigits = &statDigits;
  branchD->SetAddress(&iITSdigits);

  for(Int_t i=0; i<260; i++){
    Int_t nmod=i+240;
    digits->GetEvent(nmod);
    Int_t ndigits = iITSdigits->GetEntries();
    fAliITSQADataMakerRec->GetDigitsData(fGenDigitsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(nmod,ndigits);

    for (Int_t idig=0; idig<ndigits; idig++) {
      AliITSdigit *dig=(AliITSdigit*)iITSdigits->UncheckedAt(idig);
      Int_t iz=dig->GetCoord1();  // cell number z
      Int_t ix=dig->GetCoord2();  // cell number x
      Int_t sig=dig->GetSignal();
      fAliITSQADataMakerRec->GetDigitsData(1+fGenDigitsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(iz);
      fAliITSQADataMakerRec->GetDigitsData(2+fGenDigitsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(ix);
      fAliITSQADataMakerRec->GetDigitsData(3+fGenDigitsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(sig);
    }
  }
  return rv ; 
}

//____________________________________________________________________________ 
Int_t AliITSQASDDDataMakerRec::InitRecPoints()
{

	//AliInfo("Initialize SDD recpoints histos\n");
  // Initialization for RECPOINTS - SDD -
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 
  Int_t rv = 0 ; 
//  fGenRecPointsOffset = (fAliITSQADataMakerRec->fRecPointsQAList[AliRecoParam::kDefault])->GetEntries();

  Int_t  nOnline=1;
  Int_t  nOnline2=1;
  Int_t  nOnline3=1; 
  Int_t  nOnline4=1;
  Int_t nOnline5=1;
  if(fkOnline)
    {
      nOnline=4;
      nOnline2=28;
      nOnline3=64;
      nOnline4=14;
    }

  //AliInfo(Form("fAliITSQADataMakerRec->GetEventSpecie() %d\n",fAliITSQADataMakerRec->GetEventSpecie()));
  //AliInfo(Form("fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()] %d\n",fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()]));
  TH1F *h0 = new TH1F("SDDLay3TotCh","Layer 3 total charge",1000/nOnline,-0.5, 499.5); //position number 0
  h0->GetXaxis()->SetTitle("ADC value");
  h0->GetYaxis()->SetTitle("Entries");
  rv = fAliITSQADataMakerRec->Add2RecPointsList(h0, 0 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], !expert, image);//NON expert image
  fSDDhRecPointsTask++;
 
  TH1F *h1 = new TH1F("SDDLay4TotCh","Layer 4 total charge",1000/nOnline,-0.5, 499.5);//position number 1
  h1->GetXaxis()->SetTitle("ADC value");
  h1->GetYaxis()->SetTitle("Entries");
  rv = fAliITSQADataMakerRec->Add2RecPointsList(h1, 1 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], !expert, image);//NON expert image
  fSDDhRecPointsTask++;

  char hisnam[50];
  TH2F *h2 = new TH2F("SDDGlobalCoordDistribYX","YX Global Coord Distrib",5600/nOnline2,-28,28,5600/nOnline2,-28,28);//position number 2
  h2->GetYaxis()->SetTitle("Y[cm]");
  h2->GetXaxis()->SetTitle("X[cm]");
  rv = fAliITSQADataMakerRec->Add2RecPointsList(h2,2+fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], !expert, image);// NON expert image
  fSDDhRecPointsTask++;

  TH2F *h3 = new TH2F("SDDGlobalCoordDistribRZ","RZ Global Coord Distrib",6400/nOnline3,-32,32,1400/nOnline4,12,26);//position number 3
  h3->GetYaxis()->SetTitle("R[cm]");
  h3->GetXaxis()->SetTitle("Z[cm]");
  rv = fAliITSQADataMakerRec->Add2RecPointsList(h3,3+fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], !expert, image);// NON expert image
  fSDDhRecPointsTask++;
  
  TH2F *h4 = new TH2F("SDDGlobalCoordDistribL3PHIZ","#varphi Z Global Coord Distrib L3",6400/nOnline3,-32,32,360/nOnline,-TMath::Pi(),TMath::Pi());//position number 4
  h4->GetYaxis()->SetTitle("#phi[rad]");
  h4->GetXaxis()->SetTitle("Z[cm]");
  rv = fAliITSQADataMakerRec->Add2RecPointsList(h4,4+fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], !expert, image);//NON expert image
  fSDDhRecPointsTask++;

  TH2F *h5 = new TH2F("SDDGlobalCoordDistribL4PHIZ","#varphi Z Global Coord Distrib L4",6400/nOnline3,-32,32,360/nOnline,-TMath::Pi(),TMath::Pi());//position number 5
  h5->GetYaxis()->SetTitle("#phi[rad]");
  h5->GetXaxis()->SetTitle("Z[cm]");
  rv = fAliITSQADataMakerRec->Add2RecPointsList(h5,5+fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], !expert, image);//NON expert image
  fSDDhRecPointsTask++;
  
  TH1F *h6 = new TH1F("SDDModPatternRP","Modules pattern RP",fgknSDDmodules,239.5,499.5); //position number 6
  h6->GetXaxis()->SetTitle("Module number"); //spd offset = 240
  h6->GetYaxis()->SetTitle("Entries");
  rv = fAliITSQADataMakerRec->Add2RecPointsList(h6,6 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], expert, !image);// expert NO image
  fSDDhRecPointsTask++;


  TH2F *h7 = new TH2F("SDDModPatternL3RP","Modules pattern L3 RP",6,0.5,6.5,14,0.5,14.5);  //position number 7
  h7->GetXaxis()->SetTitle("z[#Module L3 ]");
  h7->GetYaxis()->SetTitle("#varphi[#Ladder L3]");
  rv = fAliITSQADataMakerRec->Add2RecPointsList(h7,7 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], !expert, image);// NO expert image
  fSDDhRecPointsTask++;

  TH2F *h8 = new TH2F("SDDModPatternL4RP","Modules pattern L4 RP",8,0.5,8.5,22,0.5,22.5); //position number 8
  h8->GetXaxis()->SetTitle("[#Module L3 ]");
  h8->GetYaxis()->SetTitle("#varphi[#Ladder L4]");
  rv = fAliITSQADataMakerRec->Add2RecPointsList(h8,8 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], !expert, image);//  NO expert image
  fSDDhRecPointsTask++;

//   TH1F *h7 = new TH1F("SDDLadPatternL3RP","Ladder pattern L3 RP",14,0.5,14.5);  //position number 7
//   h7->GetXaxis()->SetTitle("Ladder #, Layer 3");
//   h7->GetYaxis()->SetTitle("Entries");
//   rv = fAliITSQADataMakerRec->Add2RecPointsList(h7,7 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], expert, !image);// expert NO image
//   fSDDhRecPointsTask++;

//   TH1F *h8 = new TH1F("SDDLadPatternL4RP","Ladder pattern L4 RP",22,0.5,22.5); //position number 8
//   h8->GetXaxis()->SetTitle("Ladder #, Layer 4");
//   h8->GetYaxis()->SetTitle("Entries");
//   rv = fAliITSQADataMakerRec->Add2RecPointsList(h8,8 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], expert, !image);//  expert  NO image
//   fSDDhRecPointsTask++;

  TH2F *h9 = new TH2F("SDDLocalCoordDistrib","Local Coord Distrib",1000/nOnline,-4,4,1000/nOnline,-4,4);//position number 9
  h9->GetXaxis()->SetTitle("X local coord, drift, cm");
  h9->GetYaxis()->SetTitle("Z local coord, anode, cm");
  rv = fAliITSQADataMakerRec->Add2RecPointsList(h9,9 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], expert, !image);//  expert  NO image
  fSDDhRecPointsTask++;
  
  //AliInfo("Create SDD recpoints histos\n");
  
  TH1F *h10 = new TH1F("SDDrdistrib_Layer3" ,"SDD r distribution Layer3" ,100,14.,18.);//position number 10 (L3)
  h10->GetXaxis()->SetTitle("r[cm]");
  h10->GetXaxis()->CenterTitle();
  h10->GetYaxis()->SetTitle("Entries");
  rv = fAliITSQADataMakerRec->Add2RecPointsList(h10,10 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], expert, !image);// expert NO image
  fSDDhRecPointsTask++;
  
  TH1F *h11 = new TH1F("SDDrdistrib_Layer4" ,"SDD r distribution Layer4" ,100,22.,26.);// and position number 11 (L4)
  h11->GetXaxis()->SetTitle("r[cm]");
  h11->GetXaxis()->CenterTitle();
  h11->GetYaxis()->SetTitle("Entries");
  rv = fAliITSQADataMakerRec->Add2RecPointsList(h11,11 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], expert, !image);// expert NO image
  fSDDhRecPointsTask++;
  
  for(Int_t iLay=0; iLay<=1; iLay++){
    sprintf(hisnam,"SDDphidistrib_Layer%d",iLay+3);
    TH1F *h12 = new TH1F(hisnam,hisnam,180,-TMath::Pi(),TMath::Pi());//position number 12 (L3) and position number 13 (L4)
    h12->GetXaxis()->SetTitle("#varphi[rad]");
    h12->GetXaxis()->CenterTitle();
    h12->GetYaxis()->SetTitle("Entries");
    rv = fAliITSQADataMakerRec->Add2RecPointsList(h12,iLay+12+fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], expert, !image);// expert NO image
    fSDDhRecPointsTask++;
  }
  
  for(Int_t iLay=0; iLay<=1; iLay++){
    sprintf(hisnam,"SDDdrifttime_Layer%d",iLay+3);
    TH1F *h14 = new TH1F(hisnam,hisnam,200/nOnline5,-0.5,9999.5);//position number 14 (L3) and position number 15 (L4)
    h14->GetXaxis()->SetTitle("drift time[#mus]");
    h14->GetXaxis()->CenterTitle();
    h14->GetYaxis()->SetTitle("Entries");
    rv = fAliITSQADataMakerRec->Add2RecPointsList(h14,iLay+14+fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], !expert, image);// NON expert  image
    fSDDhRecPointsTask++;
  }
  
  if(fkOnline)
    {
      TH2F *h16 = new TH2F("SDDGlobalCoordDistribYXFSE","YX Global Coord Distrib FSE",5600/nOnline2,-28,28,5600/nOnline2,-28,28);//position number 16
      h16->GetYaxis()->SetTitle("Y[cm]");
      h16->GetXaxis()->SetTitle("X[cm]");
      rv = fAliITSQADataMakerRec->Add2RecPointsList(h16,16+fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], expert, !image);// expert NO image
      fSDDhRecPointsTask++;
      
      TH2F *h17 = new TH2F("SDDGlobalCoordDistribRZFSE","RZ Global Coord Distrib FSE",Int_t(6400/nOnline3),-32,32,1400/nOnline4,12,26);//position number 17
      h17->GetYaxis()->SetTitle("R[cm]");
      h17->GetXaxis()->SetTitle("Z[cm]");
      rv = fAliITSQADataMakerRec->Add2RecPointsList(h17,17+fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()], expert, !image);// expert NO image
      fSDDhRecPointsTask++;
      
    }//online
  
  AliDebug(AliQAv1::GetQADebugLevel(),Form("%d SDD Recs histograms booked\n",fSDDhRecPointsTask));
  
  return rv ; 
}

//____________________________________________________________________________ 
Int_t AliITSQASDDDataMakerRec::MakeRecPoints(TTree * clustersTree)
{
 // Fill QA for RecPoints - SDD -
  Int_t rv = 0 ; 

  Int_t lay, lad, det; 
  //AliInfo("get the branch with the ITS clusters !\n");
  AliITSRecPointContainer* rpcont=AliITSRecPointContainer::Instance();
  TClonesArray *recpoints=NULL; 
  if(fkOnline){
    recpoints = rpcont->FetchClusters(0,clustersTree,fAliITSQADataMakerRec->GetEventNumber());
  }
  else{
    recpoints = rpcont->FetchClusters(0,clustersTree); 
  }
  if(!rpcont->GetStatusOK() || !rpcont->IsSDDActive()){
    AliError("can't get SDD clusters !");
    return rv;
  }

  Int_t npoints = 0;      
  Float_t cluglo[3]={0.,0.,0.}; 
  if(fkOnline)
    {
      for(Int_t i=16;i<18;i++)
	{
	  fAliITSQADataMakerRec->GetRecPointsData(i+fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Reset();
	}
    }
  Int_t firMod=AliITSgeomTGeo::GetModuleIndex(3,1,1);
  Int_t lasMod=AliITSgeomTGeo::GetModuleIndex(5,1,1);
  for(Int_t module=firMod; module<lasMod;module++){
    //AliInfo(Form("Module %d\n",module));
    recpoints = rpcont->UncheckedGetClusters(module);
    npoints += recpoints->GetEntries();
    //AliInfo(Form("modnumb %d, npoints %d, total points %d\n",module, recpoints->GetEntries(),npoints));
    //AliITSgeomTGeo::GetModuleId(module, lay, lad, det);
    //AliInfo(Form("modnumb %d, lay %d, lad %d, det %d \n",module, lay, lad, det));
    
    //AliInfo(Form("modnumb %d, entries %d\n",module, recpoints->GetEntries()));
    for(Int_t j=0;j<recpoints->GetEntries();j++){
      //AliInfo(Form("modnumb %d, entry %d \n",module, j));
      AliITSRecPoint *recp = (AliITSRecPoint*)recpoints->At(j); 
      Int_t index = recp->GetDetectorIndex();
      lay=recp->GetLayer();
      Int_t modnumb=index+AliITSgeomTGeo::GetModuleIndex(lay+1,1,1);
      //printf("modnumb  %i\n",modnumb);  
      AliITSgeomTGeo::GetModuleId(modnumb, lay, lad, det);  
      fAliITSQADataMakerRec->GetRecPointsData(6 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(modnumb);//modpatternrp
      recp->GetGlobalXYZ(cluglo);
      Float_t rad=TMath::Sqrt(cluglo[0]*cluglo[0]+cluglo[1]*cluglo[1]); 
      Float_t phi=TMath::ATan2(cluglo[1],cluglo[0]);
      Float_t drifttime=recp->GetDriftTime();
      fAliITSQADataMakerRec->GetRecPointsData(9 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(recp->GetDetLocalX(),recp->GetDetLocalZ());//local distribution
      if(lay==3||lay==4)fAliITSQADataMakerRec->GetRecPointsData(2 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(cluglo[0],cluglo[1]);//global distribution YX
      fAliITSQADataMakerRec->GetRecPointsData(3 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(cluglo[2],rad);//global distribution rz
      if(fkOnline) {
	fAliITSQADataMakerRec->GetRecPointsData(16 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(cluglo[0],cluglo[1]);//global distribution YX FSE
	fAliITSQADataMakerRec->GetRecPointsData(17 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(cluglo[2],rad);//global distribution rz FSE
      }
      if(recp->GetLayer() == 2) {
	fAliITSQADataMakerRec->GetRecPointsData(0  +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(recp->GetQ()) ;//total charge of layer 3
	//fAliITSQADataMakerRec->GetRecPointsData(7  +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(lad);//lad pattern layer 3
	fAliITSQADataMakerRec->GetRecPointsData(7  +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(det,lad);//mod pattern layer 3
	fAliITSQADataMakerRec->GetRecPointsData(10 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(rad);//r distribution layer 3
	fAliITSQADataMakerRec->GetRecPointsData(12 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(phi);// phi distribution layer 3
	fAliITSQADataMakerRec->GetRecPointsData(4  +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(cluglo[2],phi);// zphi distribution layer
 	fAliITSQADataMakerRec->GetRecPointsData(14  +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(drifttime);// time distribution layer 3
      } else if(recp->GetLayer() == 3) {
	fAliITSQADataMakerRec->GetRecPointsData(1  +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(recp->GetQ()) ;//total charge layer 4
	//fAliITSQADataMakerRec->GetRecPointsData(8  +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(lad);//ladpatternlayer4
	fAliITSQADataMakerRec->GetRecPointsData(8  +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(det,lad);//mod pattern layer 4
	fAliITSQADataMakerRec->GetRecPointsData(11 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(rad);//r distribution
	fAliITSQADataMakerRec->GetRecPointsData(13 +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(phi);//phi distribution
	fAliITSQADataMakerRec->GetRecPointsData(5  +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(cluglo[2],phi);// zphi distribution layer 4
	fAliITSQADataMakerRec->GetRecPointsData(15  +fGenRecPointsOffset[fAliITSQADataMakerRec->GetEventSpecie()])->Fill(drifttime);// time distribution layer 4
      }
    }
  }
  return rv ; 
}

//_______________________________________________________________

Int_t AliITSQASDDDataMakerRec::GetOffset(AliQAv1::TASKINDEX_t task, Int_t specie)
{
  Int_t offset=0;
  if( task == AliQAv1::kRAWS )
    {
      offset=fGenRawsOffset[specie];  
    }
  else if(task == AliQAv1::kDIGITSR )
    {
      offset=fGenDigitsOffset[specie];
    }
  else if( task == AliQAv1::kRECPOINTS )
    {
      offset=fGenRecPointsOffset[specie];   
    }
  return offset;
}

//_______________________________________________________________

void AliITSQASDDDataMakerRec::SetOffset(AliQAv1::TASKINDEX_t task, Int_t offset, Int_t specie) {
  // Returns offset number according to the specified task
  if( task == AliQAv1::kRAWS ) {
    fGenRawsOffset[specie]=offset;
  }
  else if( task == AliQAv1::kDIGITSR ) {
    fGenDigitsOffset[specie]=offset;
  }
  else if( task == AliQAv1::kRECPOINTS ) {
    fGenRecPointsOffset[specie]=offset;
  }
}

//_______________________________________________________________

Int_t AliITSQASDDDataMakerRec::GetTaskHisto(AliQAv1::TASKINDEX_t task)
{

  Int_t histotot=0;

  if( task == AliQAv1::kRAWS )
    {
      histotot=fSDDhRawsTask ;  
    }
  else if(task == AliQAv1::kDIGITSR)
    {
      histotot=fSDDhDigitsTask;
    }
  else if( task == AliQAv1::kRECPOINTS )
    {
      histotot=fSDDhRecPointsTask;   
    }
  else {
    AliInfo("No task has been selected. TaskHisto set to zero.\n");
  }
  return histotot;
}


//_______________________________________________________________


void AliITSQASDDDataMakerRec::CreateTheMap()
{

  AliCDBEntry *ddlMapSDD = AliCDBManager::Instance()->Get("ITS/Calib/DDLMapSDD");
  Bool_t cacheStatus = AliCDBManager::Instance()->GetCacheFlag();
  if(!ddlMapSDD)
    {
      AliError("Calibration object retrieval failed! SDD will not be processed");
      fDDLModuleMap = NULL;
      //return rv;
    }
  fDDLModuleMap = (AliITSDDLModuleMapSDD*)ddlMapSDD->GetObject();
  if(!cacheStatus)ddlMapSDD->SetObject(NULL);
  ddlMapSDD->SetOwner(kTRUE);
  if(!cacheStatus)
    {
      delete ddlMapSDD;
    }
  AliInfo("DDL Map Created\n ");
}


//____________________________________________________________________

void AliITSQASDDDataMakerRec::ResetDetector(AliQAv1::TASKINDEX_t task)
{
  
  AliInfo(Form("Reset detector in SDD called for task index %i", task));
  if(task== AliQAv1::kRAWS ){

  fDDLModuleMap=NULL;
  }
  
}
