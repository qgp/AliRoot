/***************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Conributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/*
 $Id$
*/

////////////////////////////////////////////////////////////////////////
// This class defines the "Standard" reconstruction for the ITS       // 
// detector.                                                          //
//                                                                    //
////////////////////////////////////////////////////////////////////////
#include "TObjArray.h"
#include "TTree.h"

#include "AliCDBManager.h"
#include "AliCDBStorage.h"
#include "AliCDBEntry.h"
#include "AliITSClusterFinder.h"
#include "AliITSClusterFinderV2.h"
#include "AliITSClusterFinderV2SPD.h"
#include "AliITSClusterFinderV2SDD.h"
#include "AliITSClusterFinderV2SSD.h"
#include "AliITSClusterFinderSPD.h"
#include "AliITSClusterFinderSDD.h"
#include "AliITSClusterFinderSSD.h"
#include "AliITSDetTypeRec.h"
#include "AliITSgeom.h"
#include "AliITSRawCluster.h"
#include "AliITSRawClusterSPD.h"
#include "AliITSRawClusterSDD.h"
#include "AliITSRawClusterSSD.h"
#include "AliITSRecPoint.h"
#include "AliITSReconstructor.h"
#include "AliITSRecoParam.h"
#include "AliITSCalibrationSDD.h"
#include "AliITSMapSDD.h"
#include "AliITSDriftSpeedArraySDD.h"
#include "AliITSDriftSpeedSDD.h"
#include "AliITSCalibrationSSD.h"
#include "AliITSNoiseSSDv2.h"
#include "AliITSGainSSDv2.h"
#include "AliITSBadChannelsSSDv2.h"
#include "AliITSNoiseSSD.h"
#include "AliITSGainSSD.h"
#include "AliITSBadChannelsSSD.h"
#include "AliITSsegmentationSPD.h"
#include "AliITSsegmentationSDD.h"
#include "AliITSsegmentationSSD.h"
#include "AliLog.h"


const Int_t AliITSDetTypeRec::fgkNdettypes = 3;
const Int_t AliITSDetTypeRec::fgkDefaultNModulesSPD =  240;
const Int_t AliITSDetTypeRec::fgkDefaultNModulesSDD =  260;
const Int_t AliITSDetTypeRec::fgkDefaultNModulesSSD = 1698;

ClassImp(AliITSDetTypeRec)

//________________________________________________________________
AliITSDetTypeRec::AliITSDetTypeRec(): TObject(),
fNMod(0),
fITSgeom(0),
fReconstruction(0),
fSegmentation(0),
fCalibration(0),
fSSDCalibration(0),
fSPDDead(0),
fPreProcess(0),
fPostProcess(0),
fDigits(0),
fDDLMapSDD(0),
fRespSDD(0),
fNdtype(0),
fCtype(0),
fNctype(0),
fRecPoints(0),
fNRecPoints(0),
fSelectedVertexer(),
fFirstcall(kTRUE){
    // Standard Constructor
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //   

  fReconstruction = new TObjArray(fgkNdettypes);
  fDigits = new TObjArray(fgkNdettypes);
  for(Int_t i=0; i<3; i++){
    fClusterClassName[i]=0;
    fDigClassName[i]=0;
    fRecPointClassName[i]=0;
  }
  fSSDCalibration=new AliITSCalibrationSSD();
  fNdtype = new Int_t[fgkNdettypes];
  fCtype = new TObjArray(fgkNdettypes);
  fNctype = new Int_t[fgkNdettypes];
  fNMod = new Int_t [fgkNdettypes];
  fNMod[0] = fgkDefaultNModulesSPD;
  fNMod[1] = fgkDefaultNModulesSDD;
  fNMod[2] = fgkDefaultNModulesSSD;
  fRecPoints = new TClonesArray("AliITSRecPoint",3000);
  fNRecPoints = 0;
  
  for(Int_t i=0;i<fgkNdettypes;i++){
    fNdtype[i]=0;
    fNctype[i]=0;
  }
  
  SelectVertexer(" "); 
}

//______________________________________________________________________
AliITSDetTypeRec::AliITSDetTypeRec(const AliITSDetTypeRec & rec):TObject(rec),
fNMod(rec.fNMod),
fITSgeom(rec.fITSgeom),
fReconstruction(rec.fReconstruction),
fSegmentation(rec.fSegmentation),
fCalibration(rec.fCalibration),
fSSDCalibration(rec.fSSDCalibration),
fSPDDead(rec.fSPDDead),
fPreProcess(rec.fPreProcess),
fPostProcess(rec.fPostProcess),
fDigits(rec.fDigits),
fDDLMapSDD(rec.fDDLMapSDD),
fRespSDD(rec.fRespSDD),
fNdtype(rec.fNdtype),
fCtype(rec.fCtype),
fNctype(rec.fNctype),
fRecPoints(rec.fRecPoints),
fNRecPoints(rec.fNRecPoints),
fSelectedVertexer(rec.fSelectedVertexer),
fFirstcall(rec.fFirstcall)
{

  // Copy constructor. 

}
//______________________________________________________________________
AliITSDetTypeRec& AliITSDetTypeRec::operator=(const AliITSDetTypeRec& source){
    // Assignment operator. 
    this->~AliITSDetTypeRec();
    new(this) AliITSDetTypeRec(source);
    return *this;

}

//_____________________________________________________________________
AliITSDetTypeRec::~AliITSDetTypeRec(){
  //Destructor

  if(fReconstruction){
    fReconstruction->Delete();
    delete fReconstruction;
    fReconstruction = 0;
  }
  if(fSegmentation){
    fSegmentation->Delete();
    delete fSegmentation;
    fSegmentation = 0;
  }
  if(fCalibration){
    if(!(AliCDBManager::Instance()->GetCacheFlag())) {
      AliITSCalibration * calSPD = (AliITSCalibration*)fCalibration->At(GetITSgeom()->GetStartSPD()); 
      if (calSPD) {
	AliITSresponse* rspd = calSPD->GetResponse();    
	if(rspd) delete rspd;
      }

      AliITSCalibration * calSSD = (AliITSCalibration*)fCalibration->At(GetITSgeom()->GetStartSSD());
      if (calSSD) {
	AliITSresponse* rssd = calSSD->GetResponse();
	if(rssd) delete rssd;
      }
      fCalibration->Delete();
      delete fCalibration;
      fCalibration = 0;
      if(fRespSDD) delete fRespSDD;
      if(fDDLMapSDD) delete fDDLMapSDD;
   }
  }
  if(fSSDCalibration) delete fSSDCalibration;
   if(fSPDDead){
    if(!(AliCDBManager::Instance()->GetCacheFlag())) {
      fSPDDead->Delete();
      delete fSPDDead;
      fSPDDead = 0;
    }
  }  
  if(fPreProcess) delete fPreProcess;
  if(fPostProcess) delete fPostProcess;
  if(fDigits){
    fDigits->Delete();
    delete fDigits;
    fDigits=0;
  }
  if(fRecPoints){
    fRecPoints->Delete();
    delete fRecPoints;
    fRecPoints=0;
  }
  if(fCtype) {
    fCtype->Delete();
    delete fCtype;
    fCtype = 0;
  }
  delete [] fNctype;
  delete [] fNdtype;
  delete [] fNMod;
  
  if (fITSgeom) delete fITSgeom;
 
}

//___________________________________________________________________
void AliITSDetTypeRec::SetReconstructionModel(Int_t dettype,AliITSClusterFinder *clf){

  //Set reconstruction model for detector type

  if(fReconstruction==0) fReconstruction = new TObjArray(fgkNdettypes);
  if(fReconstruction->At(dettype)!=0) delete fReconstruction->At(dettype);
  fReconstruction->AddAt(clf,dettype);
}
//______________________________________________________________________
AliITSClusterFinder* AliITSDetTypeRec::GetReconstructionModel(Int_t dettype){

  //Get reconstruction model for detector type
  if(fReconstruction==0)  {
    Warning("GetReconstructionModel","fReconstruction is 0!");
    return 0;     
  }
  return (AliITSClusterFinder*)fReconstruction->At(dettype);
}

//______________________________________________________________________
void AliITSDetTypeRec::SetSegmentationModel(Int_t dettype,AliITSsegmentation *seg){
   
  //Set segmentation model for detector type
  
  if(fSegmentation==0) fSegmentation = new TObjArray(fgkNdettypes);
  if(fSegmentation->At(dettype)!=0) delete fSegmentation->At(dettype);
  fSegmentation->AddAt(seg,dettype);

}
//______________________________________________________________________
AliITSsegmentation* AliITSDetTypeRec::GetSegmentationModel(Int_t dettype){

  //Get segmentation model for detector type
   
   if(fSegmentation==0) {
     Warning("GetSegmentationModel","fSegmentation is 0!");
     return 0; 
   } 
   return (AliITSsegmentation*)fSegmentation->At(dettype);

}
//_______________________________________________________________________
void AliITSDetTypeRec::SetCalibrationModel(Int_t iMod, AliITSCalibration *cal){

  //Set calibration (response) for the module iMod of type dettype
  if (fCalibration==0) {
    fCalibration = new TObjArray(GetITSgeom()->GetIndexMax());
    fCalibration->SetOwner(kTRUE);
    fCalibration->Clear();
  }

  if (fCalibration->At(iMod) != 0)
    delete (AliITSCalibration*) fCalibration->At(iMod);
  fCalibration->AddAt(cal,iMod);

}
//_______________________________________________________________________
void AliITSDetTypeRec::SetSPDDeadModel(Int_t iMod, AliITSCalibration *cal){

  //Set dead pixel info for the SPD module iMod
  if (fSPDDead==0) {
    fSPDDead = new TObjArray(fgkDefaultNModulesSPD);
    fSPDDead->SetOwner(kTRUE);
    fSPDDead->Clear();
  }

  if (fSPDDead->At(iMod) != 0)
    delete (AliITSCalibration*) fSPDDead->At(iMod);
  fSPDDead->AddAt(cal,iMod);
}
//_______________________________________________________________________
AliITSCalibration* AliITSDetTypeRec::GetCalibrationModel(Int_t iMod){
  
  //Get calibration model for module type
  
  if(fCalibration==0) {
    Warning("GetalibrationModel","fCalibration is 0!");
    return 0; 
  }  

  if(iMod<fgkDefaultNModulesSPD+fgkDefaultNModulesSDD){
    return (AliITSCalibration*)fCalibration->At(iMod);
  }else{
    Int_t i=iMod-(fgkDefaultNModulesSPD+fgkDefaultNModulesSDD);
    fSSDCalibration->SetModule(i);
    return (AliITSCalibration*)fSSDCalibration;
  }

}
//_______________________________________________________________________
AliITSCalibration* AliITSDetTypeRec::GetSPDDeadModel(Int_t iMod){
  
  //Get SPD dead for module iMod
  
  if(fSPDDead==0) {
    AliWarning("fSPDDead is 0!");
    return 0; 
  }  

  return (AliITSCalibration*)fSPDDead->At(iMod);
}

//______________________________________________________________________
void AliITSDetTypeRec::SetTreeAddressD(TTree *treeD){
    // Set branch address for the tree of digits.

    const char *det[4] = {"SPD","SDD","SSD","ITS"};
    TBranch *branch;
    Char_t* digclass;
    Int_t i;
    char branchname[30];

    if(!treeD) return;
    if (fDigits == 0x0) fDigits = new TObjArray(fgkNdettypes);
    for (i=0; i<fgkNdettypes; i++) {
        digclass = GetDigitClassName(i);
	if(!(fDigits->At(i))) {
            fDigits->AddAt(new TClonesArray(digclass,1000),i);
        }else{
            ResetDigits(i);
        } 
        if (fgkNdettypes==3) sprintf(branchname,"%sDigits%s",det[3],det[i]);
        else  sprintf(branchname,"%sDigits%d",det[3],i+1);
        if (fDigits) {
            branch = treeD->GetBranch(branchname);
            if (branch) branch->SetAddress(&((*fDigits)[i]));
        } 
    } 
}

//_______________________________________________________________________
TBranch* AliITSDetTypeRec::MakeBranchInTree(TTree *tree, const char* name, 
                                       const char *classname, 
                                       void* address,Int_t size, 
                                       Int_t splitlevel)
{ 
//
// Makes branch in given tree and diverts them to a separate file
// 
//
//
    
  if (tree == 0x0) {
    Error("MakeBranchInTree","Making Branch %s Tree is NULL",name);
    return 0x0;
  }
  TBranch *branch = tree->GetBranch(name);
  if (branch) {  
    return branch;
  }
  if (classname){
    branch = tree->Branch(name,classname,address,size,splitlevel);
  }
  else {
    branch = tree->Bronch(name, "TClonesArray", address, size, splitlevel);
  }
  
  return branch;
}

//____________________________________________________________________
void AliITSDetTypeRec::SetDefaults(){
  
  //Set defaults for segmentation and response

  if(!GetITSgeom()){
    Warning("SetDefaults","null pointer to AliITSgeomGeom !");
    return;
  }

  AliITSsegmentation* seg;
  if(!GetCalibration()) {AliFatal("Exit");exit(0);}  

  for(Int_t dettype=0;dettype<fgkNdettypes;dettype++){
    if(dettype==0){
      seg = new AliITSsegmentationSPD();
      SetSegmentationModel(dettype,seg);
      SetDigitClassName(dettype,"AliITSdigitSPD");
      SetClusterClassName(dettype,"AliITSRawClusterSPD");

    }
    if(dettype==1){
      seg = new AliITSsegmentationSDD();
      SetSegmentationModel(dettype,seg);
      SetDigitClassName(dettype,"AliITSdigitSDD");
      SetClusterClassName(dettype,"AliITSRawClusterSDD");
    }
    if(dettype==2){
      AliITSsegmentationSSD* seg2 = new AliITSsegmentationSSD();
      SetSegmentationModel(dettype,seg2);
      SetDigitClassName(dettype,"AliITSdigitSSD");
      SetClusterClassName(dettype,"AliITSRawClusterSSD");
    }
  }
  
}
//______________________________________________________________________
Bool_t AliITSDetTypeRec::GetCalibration() {
  // Get Default calibration if a storage is not defined.

  if(!fFirstcall){
    AliITSCalibration* cal = GetCalibrationModel(0);
    if(cal)return kTRUE;
  }
  else {
    fFirstcall = kFALSE;
  }

  //  SetRunNumber((Int_t)AliCDBManager::Instance()->GetRun());
  //  Int_t run=GetRunNumber();

  Bool_t cacheStatus = AliCDBManager::Instance()->GetCacheFlag();
  if (fCalibration==0) {
    fCalibration = new TObjArray(GetITSgeom()->GetIndexMax());
    fCalibration->SetOwner(!cacheStatus);
    fCalibration->Clear();
  }

  // dead pixel are not used for local reconstruction
  AliCDBEntry *entrySPD = AliCDBManager::Instance()->Get("ITS/Calib/SPDNoisy");
  AliCDBEntry *deadSPD = AliCDBManager::Instance()->Get("ITS/Calib/SPDDead");
  AliCDBEntry *entrySDD = AliCDBManager::Instance()->Get("ITS/Calib/CalibSDD");
 
 //  AliCDBEntry *entrySSD = AliCDBManager::Instance()->Get("ITS/Calib/CalibSSD");
  AliCDBEntry *entryNoiseSSD = AliCDBManager::Instance()->Get("ITS/Calib/NoiseSSD");
  AliCDBEntry *entryGainSSD = AliCDBManager::Instance()->Get("ITS/Calib/GainSSD");
  AliCDBEntry *entryBadChannelsSSD = AliCDBManager::Instance()->Get("ITS/Calib/BadChannelsSSD");
  AliCDBEntry *entry2SPD = AliCDBManager::Instance()->Get("ITS/Calib/RespSPD");
  AliCDBEntry *entry2SDD = AliCDBManager::Instance()->Get("ITS/Calib/RespSDD");
  AliCDBEntry *entry2SSD = AliCDBManager::Instance()->Get("ITS/Calib/RespSSD");
  AliCDBEntry *drSpSDD = AliCDBManager::Instance()->Get("ITS/Calib/DriftSpeedSDD");
  AliCDBEntry *ddlMapSDD = AliCDBManager::Instance()->Get("ITS/Calib/DDLMapSDD");
  AliCDBEntry *mapASDD = AliCDBManager::Instance()->Get("ITS/Calib/MapsAnodeSDD");
  AliCDBEntry *mapTSDD = AliCDBManager::Instance()->Get("ITS/Calib/MapsTimeSDD");

  if(!entrySPD || !deadSPD || !entrySDD || !entryNoiseSSD || !entryGainSSD || 
     !entryBadChannelsSSD || 
     !entry2SPD || !entry2SDD || !entry2SSD || !drSpSDD || !ddlMapSDD || !mapASDD || !mapTSDD ){
    AliFatal("Calibration object retrieval failed! ");
    return kFALSE;
  }  	

  TObjArray *calSPD = (TObjArray *)entrySPD->GetObject();
  if(!cacheStatus)entrySPD->SetObject(NULL);
  entrySPD->SetOwner(kTRUE);
 
  TObjArray *caldeadSPD = (TObjArray *)deadSPD->GetObject();
  if(!cacheStatus)deadSPD->SetObject(NULL);
  deadSPD->SetOwner(kTRUE);

  AliITSresponseSPD *pSPD = (AliITSresponseSPD*)entry2SPD->GetObject();
  if(!cacheStatus)entry2SPD->SetObject(NULL);
  entry2SPD->SetOwner(kTRUE);
    
  TObjArray *calSDD = (TObjArray *)entrySDD->GetObject();
  if(!cacheStatus)entrySDD->SetObject(NULL);
  entrySDD->SetOwner(kTRUE);
 
  AliITSresponseSDD *pSDD = (AliITSresponseSDD*)entry2SDD->GetObject();
  if(!cacheStatus)entry2SDD->SetObject(NULL);
  entry2SDD->SetOwner(kTRUE);

  TObjArray *drSp = (TObjArray *)drSpSDD->GetObject();
  if(!cacheStatus)drSpSDD->SetObject(NULL);
  drSpSDD->SetOwner(kTRUE);

  AliITSDDLModuleMapSDD *ddlsdd=(AliITSDDLModuleMapSDD*)ddlMapSDD->GetObject();
  if(!cacheStatus)ddlMapSDD->SetObject(NULL);
  ddlMapSDD->SetOwner(kTRUE);

  TObjArray *mapAn = (TObjArray *)mapASDD->GetObject();
  if(!cacheStatus)mapASDD->SetObject(NULL);
  mapASDD->SetOwner(kTRUE);

  TObjArray *mapT = (TObjArray *)mapTSDD->GetObject();
  if(!cacheStatus)mapTSDD->SetObject(NULL);
  mapTSDD->SetOwner(kTRUE);

  TObject *emptyssd = 0; TString ssdobjectname = 0;
  AliITSNoiseSSDv2 *noiseSSD = new AliITSNoiseSSDv2();  
  emptyssd = (TObject *)entryNoiseSSD->GetObject();
  ssdobjectname = emptyssd->GetName();
  if(ssdobjectname=="TObjArray") {
    TObjArray *noiseSSDOld = (TObjArray *)entryNoiseSSD->GetObject();
    ReadOldSSDNoise(noiseSSDOld, noiseSSD);
  }
  else if(ssdobjectname=="AliITSNoiseSSDv2")
    noiseSSD = (AliITSNoiseSSDv2 *)entryNoiseSSD->GetObject();
  if(!cacheStatus)entryNoiseSSD->SetObject(NULL);
  entryNoiseSSD->SetOwner(kTRUE);

  AliITSGainSSDv2 *gainSSD = new AliITSGainSSDv2();
  emptyssd = (TObject *)entryGainSSD->GetObject();
  ssdobjectname = emptyssd->GetName();
  if(ssdobjectname=="Gain") {
    TObjArray *gainSSDOld = (TObjArray *)entryGainSSD->GetObject();
    ReadOldSSDGain(gainSSDOld, gainSSD);
  }
  else if(ssdobjectname=="AliITSGainSSDv2")
    gainSSD = (AliITSGainSSDv2 *)entryGainSSD->GetObject();
  if(!cacheStatus)entryGainSSD->SetObject(NULL);
  entryGainSSD->SetOwner(kTRUE);

  AliITSBadChannelsSSDv2 *badChannelsSSD = new AliITSBadChannelsSSDv2();
  emptyssd = (TObject *)entryBadChannelsSSD->GetObject();
  ssdobjectname = emptyssd->GetName();
  if(ssdobjectname=="TObjArray") {
    TObjArray *badChannelsSSDOld = (TObjArray *)entryBadChannelsSSD->GetObject();
    ReadOldSSDBadChannels(badChannelsSSDOld, badChannelsSSD);
  }
  else if(ssdobjectname=="AliITSBadChannelsSSDv2")
    badChannelsSSD = (AliITSBadChannelsSSDv2*)entryBadChannelsSSD->GetObject();
  if(!cacheStatus)entryBadChannelsSSD->SetObject(NULL);
  entryBadChannelsSSD->SetOwner(kTRUE);

  AliITSresponseSSD *pSSD = (AliITSresponseSSD*)entry2SSD->GetObject();
  if(!cacheStatus)entry2SSD->SetObject(NULL);
  entry2SSD->SetOwner(kTRUE);

  // DB entries are deleted. In this way metadeta objects are deleted as well
  if(!cacheStatus){
    delete entrySPD;
    delete deadSPD;
    delete entrySDD;
    delete entryNoiseSSD;
    delete entryGainSSD;
    delete entryBadChannelsSSD;
    delete entry2SPD;
    delete entry2SDD;
    delete entry2SSD;
    delete mapASDD;
    delete mapTSDD;
    delete drSpSDD;
    delete ddlMapSDD;
  }

  if ((!pSPD)||(!pSDD)||(!pSSD) || (!calSPD) || (!caldeadSPD) ||(!calSDD) || (!drSp) || (!ddlsdd)
      || (!mapAn) || (!mapT) || (!noiseSSD)|| (!gainSSD)|| (!badChannelsSSD)) {
    AliWarning("Can not get calibration from calibration database !");
    return kFALSE;
  }

  fNMod[0] = calSPD->GetEntries();
  fNMod[1] = calSDD->GetEntries();
  //fNMod[2] = noiseSSD->GetEntries();
  AliInfo(Form("%i SPD, %i SDD and %i SSD in calibration database",
	       fNMod[0], fNMod[1], fNMod[2]));
  AliITSCalibration* cal;
  for (Int_t i=0; i<fNMod[0]; i++) {
    cal = (AliITSCalibration*) calSPD->At(i);
    cal->SetResponse((AliITSresponse*)pSPD);
    SetCalibrationModel(i, cal);
    cal = (AliITSCalibration*) caldeadSPD->At(i);
    SetSPDDeadModel(i, cal);
  }

  fDDLMapSDD=ddlsdd;
  fRespSDD=pSDD;
  for(Int_t iddl=0; iddl<AliITSDDLModuleMapSDD::GetNDDLs(); iddl++){
    for(Int_t icar=0; icar<AliITSDDLModuleMapSDD::GetNModPerDDL();icar++){
      Int_t iMod=fDDLMapSDD->GetModuleNumber(iddl,icar);
      if(iMod==-1) continue;
      Int_t i=iMod - fgkDefaultNModulesSPD;
      cal = (AliITSCalibration*) calSDD->At(i);
      Int_t i0=2*i;
      Int_t i1=1+2*i;
      AliITSDriftSpeedArraySDD* arr0 = (AliITSDriftSpeedArraySDD*) drSp->At(i0);
      AliITSMapSDD* ma0 = (AliITSMapSDD*)mapAn->At(i0);
      AliITSMapSDD* mt0 = (AliITSMapSDD*)mapT->At(i0);
      AliITSDriftSpeedArraySDD* arr1 = (AliITSDriftSpeedArraySDD*) drSp->At(i1);
      AliITSMapSDD* ma1 = (AliITSMapSDD*)mapAn->At(i1);
      AliITSMapSDD* mt1 = (AliITSMapSDD*)mapT->At(i1);
      cal->SetDriftSpeed(0,arr0);
      cal->SetDriftSpeed(1,arr1);
      cal->SetMapA(0,ma0);
      cal->SetMapA(1,ma1);
      cal->SetMapT(0,mt0);
      cal->SetMapT(1,mt1);
      SetCalibrationModel(iMod, cal);
    }
  }

  fSSDCalibration->SetResponse((AliITSresponse*)pSSD);
  fSSDCalibration->SetNoise(noiseSSD);
  fSSDCalibration->SetGain(gainSSD);
  fSSDCalibration->SetBadChannels(badChannelsSSD);
  //fSSDCalibration->FillBadChipMap();


  /*
  for (Int_t i=0; i<fNMod[2]; i++) {

    AliITSCalibrationSSD *calibSSD = new AliITSCalibrationSSD();
    calibSSD->SetResponse((AliITSresponse*)pSSD);
    
    AliITSNoiseSSD *noise = (AliITSNoiseSSD*) (noiseSSD->At(i));
    calibSSD->SetNoise(noise);
    AliITSPedestalSSD *pedestal = (AliITSPedestalSSD*) (pedestalSSD->At(i));
    calibSSD->SetPedestal(pedestal);
    AliITSGainSSD *gain = (AliITSGainSSD*) (gainSSD->At(i));
    calibSSD->SetGain(gain);
    AliITSBadChannelsSSD *bad = (AliITSBadChannelsSSD*) (badchannelsSSD->At(i));
    calibSSD->SetBadChannels(bad);
    calibSSD->FillBadChipMap();

    Int_t iMod = i + fgkDefaultNModulesSPD + fgkDefaultNModulesSDD;
    SetCalibrationModel(iMod, calibSSD);
 }
  */

  return kTRUE;
}


//________________________________________________________________
void AliITSDetTypeRec::SetDefaultClusterFinders(){
  
  //set defaults for standard cluster finder

  if(!GetITSgeom()){
    Warning("SetDefaults","null pointer to AliITSgeom!");
    return;
  }

  AliITSClusterFinder *clf; 

  for(Int_t dettype=0;dettype<fgkNdettypes;dettype++){
    //SPD
    if(dettype==0){
      if(!GetReconstructionModel(dettype)){
	TClonesArray *dig0 = DigitsAddress(0);
	TClonesArray *rec0 = ClustersAddress(0);
	clf = new AliITSClusterFinderSPD(this,dig0,rec0);
	SetReconstructionModel(dettype,clf);

      }
    }
   
    //SDD
    if(dettype==1){
      if(!GetReconstructionModel(dettype)){
	TClonesArray *dig1 = DigitsAddress(1);
	TClonesArray *rec1 = ClustersAddress(1);
	clf = new AliITSClusterFinderSDD(this,dig1,rec1);
	SetReconstructionModel(dettype,clf);
      }

    }
    //SSD
    if(dettype==2){
      if(!GetReconstructionModel(dettype)){
	TClonesArray* dig2 = DigitsAddress(2);
	clf = new AliITSClusterFinderSSD(this,dig2);
	SetReconstructionModel(dettype,clf);
      }
    }

 }
 
  
}

//________________________________________________________________
void AliITSDetTypeRec::SetDefaultClusterFindersV2(Bool_t rawdata){

  //Set defaults for cluster finder V2

  if(!GetITSgeom()){
    Warning("SetDefaults","Null pointer to AliITSgeom !");
    return;
  }

  AliITSClusterFinder *clf; 

  for(Int_t dettype=0;dettype<fgkNdettypes;dettype++){
    //SPD
    if(dettype==0){
      if(!GetReconstructionModel(dettype)){
	clf = new AliITSClusterFinderV2SPD(this);
	clf->InitGeometry();
	if(!rawdata) clf->SetDigits(DigitsAddress(0));
	SetReconstructionModel(dettype,clf);

      }
    }
    //SDD
    if(dettype==1){
      if(!GetReconstructionModel(dettype)){
	clf = new AliITSClusterFinderV2SDD(this);
	clf->InitGeometry();
	if(!rawdata) clf->SetDigits(DigitsAddress(1));
	SetReconstructionModel(dettype,clf);
      }

    }

    //SSD
    if(dettype==2){
      if(!GetReconstructionModel(dettype)){
	clf = new AliITSClusterFinderV2SSD(this);
	clf->InitGeometry();
	if(!rawdata) clf->SetDigits(DigitsAddress(2));
	SetReconstructionModel(dettype,clf);
      }
    }

 }
   
}
//______________________________________________________________________
void AliITSDetTypeRec::MakeBranch(TTree* tree, Option_t* option){

  //Creates branches for clusters and recpoints
  Bool_t cR = (strstr(option,"R")!=0);
  Bool_t cRF = (strstr(option,"RF")!=0);
  
  if(cRF)cR = kFALSE;

  if(cR) MakeBranchR(tree);
  if(cRF) MakeBranchRF(tree);

}

//___________________________________________________________________
void AliITSDetTypeRec::AddCluster(Int_t id, AliITSRawCluster *c){

  // Adds a raw cluster to the list
  TClonesArray &lc = *((TClonesArray*)fCtype->At(id));  
  switch(id){
  case 0:
    new(lc[fNctype[id]++]) AliITSRawClusterSPD(*((AliITSRawClusterSPD*)c));
    break;
  case 1:
    new(lc[fNctype[id]++]) AliITSRawClusterSDD(*((AliITSRawClusterSDD*)c));
    break;
  case 2:
    new(lc[fNctype[id]++]) AliITSRawClusterSSD(*((AliITSRawClusterSSD*)c));
    break;
  } 
}
//___________________________________________________________________
void AliITSDetTypeRec::ResetDigits(){
  // Reset number of digits and the digits array for the ITS detector.
  
  if(!fDigits) return;
  for(Int_t i=0;i<fgkNdettypes;i++){
    ResetDigits(i);
  }
}
//___________________________________________________________________
void AliITSDetTypeRec::ResetDigits(Int_t branch){
  // Reset number of digits and the digits array for this branch.
  
  if(fDigits->At(branch)) ((TClonesArray*)fDigits->At(branch))->Clear();
  if(fNdtype) fNdtype[branch]=0;

}

//__________________________________________________________________
void AliITSDetTypeRec::ResetClusters(){

  //Resets number of clusters and the cluster array 
  for(Int_t i=0;i<fgkNdettypes;i++){
    ResetClusters(i);
  }
}

//__________________________________________________________________
void AliITSDetTypeRec::ResetClusters(Int_t i){

  //Resets number of clusters and the cluster array for this branch

  if (fCtype->At(i))    ((TClonesArray*)fCtype->At(i))->Clear();
  if (fNctype)  fNctype[i]=0;
}
//__________________________________________________________________
void AliITSDetTypeRec::MakeBranchR(TTree *treeR, Option_t *opt){

  //Creates tree branches for recpoints
  // Inputs:
  //      cont char *file  File name where RecPoints branch is to be written
  //                       to. If blank it write the SDigits to the same
  //                       file in which the Hits were found.

  Int_t buffsz = 4000;
  char branchname[30];

  // only one branch for rec points for all detector types
  Bool_t oFast= (strstr(opt,"Fast")!=0);
  
  Char_t detname[10] = "ITS";
 
  
  if(oFast){
    sprintf(branchname,"%sRecPointsF",detname);
  } else {
    sprintf(branchname,"%sRecPoints",detname);
  }
  
  if(!fRecPoints)fRecPoints = new TClonesArray("AliITSRecPoint",1000);
  if (treeR)
    MakeBranchInTree(treeR,branchname,0,&fRecPoints,buffsz,99);
}
//______________________________________________________________________
void AliITSDetTypeRec::SetTreeAddressR(TTree *treeR){
    // Set branch address for the Reconstructed points Trees.
    // Inputs:
    //      TTree *treeR   Tree containing the RecPoints.
    // Outputs:
    //      none.
    // Return:

    char branchname[30];
    Char_t namedet[10]="ITS";

    if(!treeR) return;
    if(fRecPoints==0x0) fRecPoints = new TClonesArray("AliITSRecPoint",1000);
    TBranch *branch;
    sprintf(branchname,"%sRecPoints",namedet);
    branch = treeR->GetBranch(branchname);
    if (branch) {
      branch->SetAddress(&fRecPoints);
    }else {
      sprintf(branchname,"%sRecPointsF",namedet);
      branch = treeR->GetBranch(branchname);
      if (branch) {
	branch->SetAddress(&fRecPoints);
      }

    }
}
//____________________________________________________________________
void AliITSDetTypeRec::AddRecPoint(const AliITSRecPoint &r){
    // Add a reconstructed space point to the list
    // Inputs:
    //      const AliITSRecPoint &r RecPoint class to be added to the tree
    //                              of reconstructed points TreeR.
    // Outputs:
    //      none.
    // Return:
    //      none.

    TClonesArray &lrecp = *fRecPoints;
    new(lrecp[fNRecPoints++]) AliITSRecPoint(r);
}

//______________________________________________________________________
void AliITSDetTypeRec::DigitsToRecPoints(TTree *treeD,TTree *treeR,Int_t lastentry,Option_t *opt, Bool_t v2){
  // cluster finding and reconstruction of space points
  // the condition below will disappear when the geom class will be
  // initialized for all versions - for the moment it is only for v5 !
  // 7 is the SDD beam test version
  // Inputs:
  //      TTree *treeD     Digits tree
  //      TTree *treeR     Clusters tree
  //      Int_t lastentry  Offset for module when not all of the modules
  //                       are processed.
  //      Option_t *opt    String indicating which ITS sub-detectors should
  //                       be processed. If ="All" then all of the ITS
  //                       sub detectors are processed.

  const char *all = strstr(opt,"All");
  const char *det[3] = {strstr(opt,"SPD"),strstr(opt,"SDD"),
                        strstr(opt,"SSD")};
  if(!v2) {
    SetDefaultClusterFinders();
    AliInfo("Original cluster finder has been selected\n");
  }
  else   { 
    SetDefaultClusterFindersV2();
    AliInfo("V2 cluster finder has been selected \n");
  }

  AliITSClusterFinder *rec     = 0;
  Int_t id,module,first=0;
  for(module=0;module<GetITSgeom()->GetIndexMax();module++){
      id       = GetITSgeom()->GetModuleType(module);
      if (!all && !det[id]) continue;
      if(det[id]) first = GetITSgeom()->GetStartDet(id);
      rec = (AliITSClusterFinder*)GetReconstructionModel(id);
      TClonesArray *itsDigits  = DigitsAddress(id);
      if (!rec)
          AliFatal("The reconstruction class was not instanciated!");
      ResetDigits();  // MvL: Not sure we neeed this when rereading anyways
      if (all) {
          treeD->GetEvent(lastentry+module);
      }else {
          treeD->GetEvent(lastentry+(module-first));
      }
      Int_t ndigits = itsDigits->GetEntriesFast();
      if(ndigits>0){
	rec->SetDetTypeRec(this);
	rec->SetDigits(DigitsAddress(id));
	//	rec->SetClusters(ClustersAddress(id));
	rec->FindRawClusters(module);
      } // end if
      treeR->Fill();
      ResetRecPoints();
      ResetClusters();
  } 
}
//______________________________________________________________________
void AliITSDetTypeRec::DigitsToRecPoints(AliRawReader* rawReader,TTree *treeR,Option_t *opt){
  // cluster finding and reconstruction of space points
  // the condition below will disappear when the geom class will be
  // initialized for all versions - for the moment it is only for v5 !
  // 7 is the SDD beam test version
  // Inputs:
  //      AliRawReader *rawReader  Pointer to the raw-data reader
  //      TTree *treeR             Clusters tree
  // Outputs:
  //      none.
  // Return:
  //      none.
  const char *all = strstr(opt,"All");
  const char *det[3] = {strstr(opt,"SPD"),strstr(opt,"SDD"),
                        strstr(opt,"SSD")};
  AliITSClusterFinderV2 *rec     = 0;
  Int_t id=0;

  TClonesArray *array=new TClonesArray("AliITSRecPoint",1000);
  TBranch *branch = treeR->Branch("ITSRecPoints",&array);
  delete array;
 
  TClonesArray** clusters = new TClonesArray*[GetITSgeom()->GetIndexMax()]; 
  for (Int_t iModule = 0; iModule < GetITSgeom()->GetIndexMax(); iModule++) {
    clusters[iModule] = NULL;
  }
  for(id=0;id<3;id++){
    if (!all && !det[id]) continue;
    rec = (AliITSClusterFinderV2*)GetReconstructionModel(id);
    if (!rec)
      AliFatal("The reconstruction class was not instanciated");
    rec->SetDetTypeRec(this);
    rec->RawdataToClusters(rawReader,clusters);    
  } 
  Int_t nClusters =0;
  TClonesArray *emptyArray=new TClonesArray("AliITSRecPoint");
  for(Int_t iModule=0;iModule<GetITSgeom()->GetIndexMax();iModule++){
    id = GetITSgeom()->GetModuleType(iModule);
    if (!all && !det[id]) continue;
    array = clusters[iModule];
    if(!array){
      AliDebug(1,Form("data for module %d missing!",iModule));
      array = emptyArray;
    }
    branch->SetAddress(&array);
    treeR->Fill();
    nClusters+=array->GetEntriesFast();

    if (array != emptyArray) {
      array->Delete();
      delete array;
    }
  }
  delete emptyArray;

  delete[] clusters;
  Info("DigitsToRecPoints", "total number of found recpoints in ITS: %d\n", 
       nClusters);
  
}

//______________________________________________________________________
void AliITSDetTypeRec::ReadOldSSDNoise(TObjArray *array, 
				       AliITSNoiseSSDv2 *noiseSSD) {
  //Reads the old SSD calibration object and converts it to the new format
  const Int_t fgkSSDSTRIPSPERMODULE = 1536;
  const Int_t fgkSSDPSIDESTRIPSPERMODULE = 768;

  Int_t fNMod = array->GetEntries();
  cout<<"Converting old calibration object for noise..."<<endl;

  //NOISE
  Double_t noise = 0.0;
  for (Int_t iModule = 0; iModule < fNMod; iModule++) {
    AliITSNoiseSSD *noiseModule = (AliITSNoiseSSD*) (array->At(iModule));
    for(Int_t iStrip = 0; iStrip < fgkSSDSTRIPSPERMODULE; iStrip++) {
      noise = (iStrip < fgkSSDPSIDESTRIPSPERMODULE) ? noiseModule->GetNoiseP(iStrip) : noiseModule->GetNoiseN(1535 - iStrip);
      if(iStrip < fgkSSDPSIDESTRIPSPERMODULE)
	noiseSSD->AddNoiseP(iModule,iStrip,noise);
      if(iStrip >= fgkSSDPSIDESTRIPSPERMODULE)
	noiseSSD->AddNoiseN(iModule,1535 - iStrip,noise);
    }//loop over strips
  }//loop over modules      
}

//______________________________________________________________________
void AliITSDetTypeRec::ReadOldSSDBadChannels(TObjArray *array, 
					     AliITSBadChannelsSSDv2 *badChannelsSSD) {
  //Reads the old SSD calibration object and converts it to the new format
  Int_t fNMod = array->GetEntries();
  cout<<"Converting old calibration object for bad channels..."<<endl;
  for (Int_t iModule = 0; iModule < fNMod; iModule++) {
    //for (Int_t iModule = 0; iModule < 1; iModule++) {
    AliITSBadChannelsSSD *bad = (AliITSBadChannelsSSD*) (array->At(iModule));
    TArrayI arrayPSide = bad->GetBadPChannelsList();
    for(Int_t iPCounter = 0; iPCounter < arrayPSide.GetSize(); iPCounter++) 
      badChannelsSSD->AddBadChannelP(iModule,
				     iPCounter,
				     (Char_t)arrayPSide.At(iPCounter));
        
    TArrayI arrayNSide = bad->GetBadNChannelsList();
    for(Int_t iNCounter = 0; iNCounter < arrayNSide.GetSize(); iNCounter++) 
      badChannelsSSD->AddBadChannelN(iModule,
				     iNCounter,
				     (Char_t)arrayNSide.At(iNCounter));
    
  }//loop over modules      
}

//______________________________________________________________________
void AliITSDetTypeRec::ReadOldSSDGain(TObjArray *array, 
				      AliITSGainSSDv2 *gainSSD) {
  //Reads the old SSD calibration object and converts it to the new format

  Int_t fNMod = array->GetEntries();
  cout<<"Converting old calibration object for gain..."<<endl;

  //GAIN
  for (Int_t iModule = 0; iModule < fNMod; iModule++) {
    AliITSGainSSD *gainModule = (AliITSGainSSD*) (array->At(iModule));
    TArrayF arrayPSide = gainModule->GetGainP();
    for(Int_t iPCounter = 0; iPCounter < arrayPSide.GetSize(); iPCounter++)
      gainSSD->AddGainP(iModule,
			iPCounter,
			arrayPSide.At(iPCounter));
    TArrayF arrayNSide = gainModule->GetGainN();
    for(Int_t iNCounter = 0; iNCounter < arrayNSide.GetSize(); iNCounter++)
      gainSSD->AddGainN(iModule,
			iNCounter,
			arrayNSide.At(iNCounter));
  }//loop over modules 
}


