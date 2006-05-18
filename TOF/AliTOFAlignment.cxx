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
***************************************************************************/

/*
$Log$
Revision 1.5  2006/04/16 22:29:05  hristov
Coding conventions (Annalisa)

Revision 1.4  2006/04/05 08:35:38  hristov
Coding conventions (S.Arcelli, C.Zampolli)

Revision 1.3  2006/03/31 13:49:07  arcelli
Removing some junk printout

Revision 1.2  2006/03/31 11:26:30  arcelli
 changing CDB Ids according to standard convention

Revision 1.1  2006/03/28 14:54:48  arcelli
class for TOF alignment

author: Silvia Arcelli, arcelli@bo.infn.it
*/  

/////////////////////////////////////////////////////////
//                                                     //
//            Class for alignment procedure            //
//                                                     //
//                                                     //
//                                                     //
/////////////////////////////////////////////////////////

#include <Rtypes.h>

#include "TRandom.h"
#include "TString.h"

#include "AliLog.h"
#include "AliAlignObj.h"
#include "AliAlignObjAngles.h"
#include "AliCDBManager.h"
#include "AliCDBMetaData.h"
#include "AliCDBId.h"
#include "AliCDBEntry.h"
#include "AliTOFAlignment.h"

ClassImp(AliTOFAlignment)

//_____________________________________________________________________________
AliTOFAlignment::AliTOFAlignment():TTask("AliTOFAlignment","") { 
  //AliTOFalignment main Ctor

  fNTOFAlignObj=0;
  fTOFAlignObjArray=0x0;
}
//_____________________________________________________________________________
AliTOFAlignment::AliTOFAlignment(const AliTOFAlignment &t):TTask("AliTOFAlignment",""){ 
  //AliTOFAlignment copy Ctor

  fNTOFAlignObj=t.fNTOFAlignObj;
  fTOFAlignObjArray=t.fTOFAlignObjArray;

}

//_____________________________________________________________________________
AliTOFAlignment& AliTOFAlignment::operator=(const AliTOFAlignment &t){ 
  //AliTOFAlignment assignment operator

  this->fNTOFAlignObj=t.fNTOFAlignObj;
  this->fTOFAlignObjArray=t.fTOFAlignObjArray;
  return *this;

}

//_____________________________________________________________________________
void AliTOFAlignment::Smear( Float_t *tr, Float_t *rot)
{
  //Introduce Random Offset/Tilts
  fTOFAlignObjArray = new TObjArray(kMaxAlignObj);
  Float_t dx, dy, dz;  // shifts
  Float_t dpsi, dtheta, dphi; // angular displacements
  TRandom *rnd   = new TRandom(1567);

  TString path;
  const char *sSM71="/ALIC_1/B077_1/B071_"; //1-13
  const char *sm71="/BTO1_1";
  const char *sSM74="/ALIC_1/B077_1/B074_"; //1-2
  const char *sm74="/BTO2_1";
  const char *sSM75="/ALIC_1/B077_1/B075_"; //1-3
  const char *sm75="/BTO3_1";


  Int_t nSM71 = 13, nSM74=2, nSM75=3;
  AliAlignObj::ELayerID iLayer = AliAlignObj::kInvalidLayer;
  UShort_t iIndex=0; //dummy volume index
  //  AliAlignObj::ELayerID iLayer = AliAlignObj::kTOF;
  //  Int_t iIndex=1; //dummy volume index
  UShort_t dvoluid = AliAlignObj::LayerToVolUID(iLayer,iIndex); //dummy volume identity 
  Int_t i;
  for (i = 1; i<=nSM71 ; i++) {

    dx = (rnd->Gaus(0.,1.))*tr[0]/nSM71;
    dy = (rnd->Gaus(0.,1.))*tr[1]/nSM71;
    dz = (rnd->Gaus(0.,1.))*tr[2]/nSM71;
    dpsi   = rot[0]/nSM71;
    dtheta = rot[1]/nSM71;
    dphi   = rot[2]/nSM71;
    
    path = sSM71;
    path += i;
    path += sm71;
    AliAlignObjAngles *o =new AliAlignObjAngles(path, dvoluid, dx, dy, dz, dpsi, dtheta, dphi);
    fTOFAlignObjArray->Add(o);
  }

  for (i = 1; i<=nSM74 ; i++) {

    dx = (rnd->Gaus(0.,1.))*tr[0]/nSM74;
    dy = (rnd->Gaus(0.,1.))*tr[1]/nSM74;
    dz = (rnd->Gaus(0.,1.))*tr[2]/nSM74;
    dpsi   = rot[0]/nSM74;
    dtheta = rot[1]/nSM74;
    dphi   = rot[2]/nSM74;
    
    path = sSM74;
    path += i;
    path += sm74;
    AliAlignObjAngles *o =new AliAlignObjAngles(path, dvoluid, dx, dy, dz, dpsi, dtheta, dphi);
    fTOFAlignObjArray->Add(o);
  }

  for (i = 1; i<=nSM75; i++) {

    dx = (rnd->Gaus(0.,1.))*tr[0]/nSM75;
    dy = (rnd->Gaus(0.,1.))*tr[1]/nSM75;
    dz = (rnd->Gaus(0.,1.))*tr[2]/nSM75;
    dpsi   = rot[0]/nSM75;
    dtheta = rot[1]/nSM75;
    dphi   = rot[2]/nSM75;
    
    path = sSM75;
    path += i;
    path += sm75;
    AliAlignObjAngles *o =new AliAlignObjAngles(path, dvoluid, dx, dy, dz, dpsi, dtheta, dphi);
    fTOFAlignObjArray->Add(o);
  }
  fNTOFAlignObj=fTOFAlignObjArray->GetEntries();
  AliInfo(Form("Number of Alignable Volumes: %d",fNTOFAlignObj));
  delete rnd;
}

//_____________________________________________________________________________
void AliTOFAlignment::Align( Float_t *tr, Float_t *rot)
{
  //Introduce Offset/Tilts

  fTOFAlignObjArray = new TObjArray(kMaxAlignObj);
  Float_t dx, dy, dz;  // shifts
  Float_t dpsi, dtheta, dphi; // angular displacements
  TString path;
  const char *sSM71="/ALIC_1/B077_1/B071_"; //1-13
  const char *sm71="/BTO1_1";
  const char *sSM74="/ALIC_1/B077_1/B074_"; //1-2
  const char *sm74="/BTO2_1";
  const char *sSM75="/ALIC_1/B077_1/B075_"; //1-3
  const char *sm75="/BTO3_1";


  Int_t nSM71 = 13, nSM74=2, nSM75=3;
  AliAlignObj::ELayerID iLayer = AliAlignObj::kInvalidLayer;
  UShort_t iIndex=0; //dummy volume index
  //  AliAlignObj::ELayerID iLayer = AliAlignObj::kTOF;
  //  Int_t iIndex=1; //dummy volume index
  UShort_t dvoluid = AliAlignObj::LayerToVolUID(iLayer,iIndex); //dummy volume identity 
  Int_t i;
  for (i = 1; i<=nSM71 ; i++) {

    dx = tr[0]/nSM71;
    dy = tr[1]/nSM71;
    dz = tr[2]/nSM71;
    dpsi   = rot[0]/nSM71;
    dtheta = rot[1]/nSM71;
    dphi   = rot[2]/nSM71;
    
    path = sSM71;
    path += i;
    path += sm71;
    AliAlignObjAngles *o =new AliAlignObjAngles(path, dvoluid, dx, dy, dz, dpsi, dtheta, dphi);
    fTOFAlignObjArray->Add(o);
  }

  for (i = 1; i<=nSM74 ; i++) {

    dx = tr[0]/nSM74;
    dy = tr[1]/nSM74;
    dz = tr[2]/nSM74;
    dpsi   = rot[0]/nSM74;
    dtheta = rot[1]/nSM74;
    dphi   = rot[2]/nSM74;
    
    path = sSM74;
    path += i;
    path += sm74;
    AliAlignObjAngles *o =new AliAlignObjAngles(path, dvoluid, dx, dy, dz, dpsi, dtheta, dphi);
    fTOFAlignObjArray->Add(o);
  }

  for (i = 1; i<=nSM75; i++) {

    dx = tr[0]/nSM75;
    dy = tr[1]/nSM75;
    dz = tr[2]/nSM75;
    dpsi   = rot[0]/nSM75;
    dtheta = rot[1]/nSM75;
    dphi   = rot[2]/nSM75;
    
    path = sSM75;
    path += i;
    path += sm75;
    AliAlignObjAngles *o =new AliAlignObjAngles(path, dvoluid, dx, dy, dz, dpsi, dtheta, dphi);
    fTOFAlignObjArray->Add(o);
  }
  fNTOFAlignObj=fTOFAlignObjArray->GetEntries();
  AliInfo(Form("Number of Alignable Volumes: %d",fNTOFAlignObj));
}
//_____________________________________________________________________________
void AliTOFAlignment::WriteParOnCDB(Char_t *sel, Int_t minrun, Int_t maxrun)
{
  //Write Align Par on CDB
  AliCDBManager *man = AliCDBManager::Instance();
  if(!man->IsDefaultStorageSet())man->SetDefaultStorage("local://$ALICE_ROOT");
  Char_t *sel1 = "AlignPar" ;
  Char_t  out[100];
  sprintf(out,"%s/%s",sel,sel1); 
  AliCDBId idTOFAlign(out,minrun,maxrun);
  AliCDBMetaData *mdTOFAlign = new AliCDBMetaData();
  mdTOFAlign->SetResponsible("TOF");
  AliInfo(Form("Number of Alignable Volumes: %d",fNTOFAlignObj));
  man->Put(fTOFAlignObjArray,idTOFAlign,mdTOFAlign);
}
//_____________________________________________________________________________
void AliTOFAlignment::ReadParFromCDB(Char_t *sel, Int_t nrun)
{
  //Read Align Par from CDB
  AliCDBManager *man = AliCDBManager::Instance();
  if(!man->IsDefaultStorageSet())man->SetDefaultStorage("local://$ALICE_ROOT");
  Char_t *sel1 = "AlignPar" ;
  Char_t  out[100];
  sprintf(out,"%s/%s",sel,sel1); 
  AliCDBEntry *entry = man->Get(out,nrun);
  fTOFAlignObjArray=(TObjArray*)entry->GetObject();
  fNTOFAlignObj=fTOFAlignObjArray->GetEntries();
  AliInfo(Form("Number of Alignable Volumes from CDB: %d",fNTOFAlignObj));

}
//_____________________________________________________________________________
void AliTOFAlignment::WriteSimParOnCDB(Char_t *sel, Int_t minrun, Int_t maxrun)
{
  //Write Sim Align Par on CDB
  AliCDBManager *man = AliCDBManager::Instance();
  if(!man->IsDefaultStorageSet())man->SetDefaultStorage("local://$ALICE_ROOT");
  Char_t *sel1 = "AlignSimPar" ;
  Char_t  out[100];
  sprintf(out,"%s/%s",sel,sel1); 
  AliCDBId idTOFAlign(out,minrun,maxrun);
  AliCDBMetaData *mdTOFAlign = new AliCDBMetaData();
  mdTOFAlign->SetResponsible("TOF");
  AliInfo(Form("Number of Alignable Volumes: %d",fNTOFAlignObj));
  man->Put(fTOFAlignObjArray,idTOFAlign,mdTOFAlign);
}
//_____________________________________________________________________________
void AliTOFAlignment::ReadSimParFromCDB(Char_t *sel, Int_t nrun){
  //Read Sim Align Par from CDB
  AliCDBManager *man = AliCDBManager::Instance();
  if(!man->IsDefaultStorageSet())man->SetDefaultStorage("local://$ALICE_ROOT");
  Char_t *sel1 = "AlignSimPar" ;
  Char_t  out[100];
  sprintf(out,"%s/%s",sel,sel1); 
  AliCDBEntry *entry = man->Get(out,nrun);
  fTOFAlignObjArray=(TObjArray*)entry->GetObject();
  fNTOFAlignObj=fTOFAlignObjArray->GetEntries();
  AliInfo(Form("Number of Alignable Volumes from CDB: %d",fNTOFAlignObj));

}
//_____________________________________________________________________________
void AliTOFAlignment::WriteOnCDBforDC()
{
  //Write Align Par on CDB for DC06
  AliCDBManager *man = AliCDBManager::Instance();
  if(!man->IsDefaultStorageSet())man->SetDefaultStorage("local://$ALICE_ROOT");
  AliCDBId idTOFAlign("TOF/Align/Data",0,0);
  AliCDBMetaData *mdTOFAlign = new AliCDBMetaData();
  mdTOFAlign->SetComment("Alignment objects for ideal geometry, i.e. applying them to TGeo has to leave geometry unchanged");
  mdTOFAlign->SetResponsible("TOF");
  AliInfo(Form("Number of Alignable Volumes: %d",fNTOFAlignObj));
  man->Put(fTOFAlignObjArray,idTOFAlign,mdTOFAlign);
}
//_____________________________________________________________________________
void AliTOFAlignment::ReadFromCDBforDC()
{
  //Read Sim Align Par from CDB for DC06
  AliCDBManager *man = AliCDBManager::Instance();
  if(!man->IsDefaultStorageSet())man->SetDefaultStorage("local://$ALICE_ROOT");
  AliCDBEntry *entry = man->Get("TOF/Align/Data",0);
  fTOFAlignObjArray=(TObjArray*)entry->GetObject();
  fNTOFAlignObj=fTOFAlignObjArray->GetEntries();
  AliInfo(Form("Number of Alignable Volumes from CDB: %d",fNTOFAlignObj));

}
