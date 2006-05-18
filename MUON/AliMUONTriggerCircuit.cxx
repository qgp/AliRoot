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

#include <TMath.h>

#include "AliMUONTriggerCircuit.h"
#include "AliRun.h"
#include "AliMUON.h"
#include "AliMUONTriggerConstants.h"
#include "AliMUONGeometrySegmentation.h"
#include "AliMUONSegmentation.h"
#include "AliMUONConstants.h"
#include "AliLog.h"

ClassImp(AliMUONTriggerCircuit)

//----------------------------------------------------------------------
AliMUONTriggerCircuit::AliMUONTriggerCircuit()
  : TObject(),
    fIdCircuit(0),
    fX2m(0),
    fX2ud(0)
{
// Constructor

  fOrMud[0]=fOrMud[1]=0;
  Int_t i;  
  for (i=0; i<4; i++) {
    for (Int_t j=0; j<32; j++) {      
      fXcode[i][j]=0;
      fYcode[i][j]=0;
    }
  }
  for (i=0; i<16; i++) { fXpos11[i]=0.; }
  for (i=0; i<31; i++) { fYpos11[i]=0.; }
  for (i=0; i<63; i++) { fYpos21[i]=0.; }
}

//----------------------------------------------------------------------
AliMUONTriggerCircuit::AliMUONTriggerCircuit(const AliMUONTriggerCircuit& theMUONTriggerCircuit)
  : TObject(theMUONTriggerCircuit)
{
// Protected copy constructor

  AliFatal("Not implemented.");
}

//----------------------------------------------------------------------
AliMUONTriggerCircuit & 
AliMUONTriggerCircuit::operator=(const AliMUONTriggerCircuit& rhs)
{
// Protected assignement operator

  if (this == &rhs) return *this;

  AliFatal("Not implemented.");
    
  return *this;  
}

//----------------------------------------------------------------------
void AliMUONTriggerCircuit::Init(Int_t iCircuit) {
// initialize circuit characteristics
  fIdCircuit=AliMUONTriggerConstants::CircuitId(iCircuit);

  LoadX2();
  LoadXCode();
  LoadYCode();

  LoadXPos2();
  LoadYPos2();
  
}

//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::CircuitNumber(Int_t idCircuit) const {
// returns circuit number iCircuit (0-234) corresponding to circuit idCircuit
  Int_t iCircuit=0;
  for (Int_t i=0; i<234; i++) {
    if (AliMUONTriggerConstants::CircuitId(i)==idCircuit) {
      iCircuit=i;
      break;
    }
  }
  return iCircuit;
}
//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::ModuleNumber(Int_t idModule) const {
// returns module number imod (from 0 to 63) corresponding to module idmodule
  Int_t absidModule=TMath::Abs(idModule);
  Int_t iModule=0;
  for (Int_t i=0; i<63; i++) {
    if (AliMUONTriggerConstants::ModuleId(i)==absidModule) { 
      iModule=i;
      break;
    }
  }
  return iModule;
}

//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::Module(Int_t idCircuit) const {
// returns ModuleId where Circuit idCircuit is sitting
  return Int_t(idCircuit/10);
}
//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::Position(Int_t idCircuit) const {
// returns position of idCircuit in correcponding Module
  return TMath::Abs(idCircuit)-TMath::Abs(Module(idCircuit))*10;
}

//----------------------------------------------------------------------
void AliMUONTriggerCircuit::LoadX2() {
// initialize fX2m, fX2ud and fOrMud
  
  Int_t idModule=Module(fIdCircuit);        // corresponding module Id.
// and its number of X strips
  Int_t nStrX=AliMUONTriggerConstants::NstripX(ModuleNumber(idModule)); 
// and its number of Y strips
  Int_t nStrY=AliMUONTriggerConstants::NstripY(ModuleNumber(idModule)); 
  Int_t iPosCircuit=Position(fIdCircuit); // position of circuit in module
  
// first step : look at lower part 
  if (iPosCircuit==1) {               // need to scan lower module       
    if(idModule<91&&TMath::Abs(idModule)!=41&&idModule>-91) { 
      fOrMud[0]=1;
      Int_t idModuleD=(TMath::Abs(idModule)+10)*(TMath::Abs(idModule)/idModule); 
      Int_t nStrD=AliMUONTriggerConstants::NstripY(ModuleNumber(idModuleD));
      
      if (nStrY!=nStrD    
	  &&TMath::Abs(idModule)!=42&&TMath::Abs(idModule)!=52) {   
	if (nStrY==8) fX2m=1; 
	if (nStrD==8) fX2ud=1; 
      }      
    }      

  } else {                         // lower strips within same module       
    fOrMud[0]=0;
  }    
  
// second step : look at upper part
  if ((iPosCircuit==1&&nStrX==16)||(iPosCircuit==2&&nStrX==32)|| 
      (iPosCircuit==3&&nStrX==48)||(iPosCircuit==4&&nStrX==64)) {   
    if ((idModule>17||idModule<-17)&&TMath::Abs(idModule)!=61) {  
      fOrMud[1]=1;
      Int_t idModuleU=(TMath::Abs(idModule)-10)*(TMath::Abs(idModule)/idModule); 
      Int_t nStrU=AliMUONTriggerConstants::NstripY(ModuleNumber(idModuleU)); 

      if (nStrY!=nStrU    
	  &&TMath::Abs(idModule)!=62&&TMath::Abs(idModule)!=52) {   
	if (nStrY==8) fX2m=1; 
	if (nStrU==8) fX2ud=1;
      }      
    }     
    
  } else {                       // upper strips within same module       
    fOrMud[1]=0;
  }
}  

//----------------------------------------------------------------------
void AliMUONTriggerCircuit::LoadXCode(){
// assign a Id. number to each X strip of current circuit 
// Id.=(corresponding module Id.)*100+(Id. strip of module)

// first part : fill XMC11 XMC12 and strips 8 to 24 (middle) XMC21 XMC22
  Int_t iStripCircMT1=0, iStripCircMT2=8;
  Int_t idModule=Module(fIdCircuit);        // corresponding module Id.
// and its number of strips
  Int_t nStrX=AliMUONTriggerConstants::NstripX(ModuleNumber(idModule)); 
  Int_t iPosCircuit=Position(fIdCircuit);   // position of circuit in module  
  Int_t sign=TMath::Abs(idModule)/idModule; // left or right 
  Int_t istrip;

  for (istrip=(iPosCircuit-1)*16; 
       istrip<(iPosCircuit-1)*16+16; istrip++) {
        
    fXcode[0][iStripCircMT1]=sign*(TMath::Abs(idModule)*100+istrip); 
    fXcode[1][iStripCircMT1]=sign*(TMath::Abs(idModule)*100+istrip); 
    fXcode[2][iStripCircMT2]=sign*(TMath::Abs(idModule)*100+istrip); 
    fXcode[3][iStripCircMT2]=sign*(TMath::Abs(idModule)*100+istrip);     
    iStripCircMT1++;
    iStripCircMT2++;
  }

// second part 
// XMC21 XMC22 strips 0 to 7 and 24 to 31 
  Int_t idModuleD, idModuleU;
  Int_t nStrD, nStrU;

  idModule=Module(fIdCircuit); // corresponding module Id.
// number of X strips
  nStrX=AliMUONTriggerConstants::NstripX(ModuleNumber(idModule));  
  sign=TMath::Abs(idModule)/idModule;

// fill lower part (0 to 7)
  if (iPosCircuit==1) {                 // need to scan lower module 
    if(idModule<91&&TMath::Abs(idModule)!=41&&idModule>-91) { // non-existing
      idModuleD=sign*(TMath::Abs(idModule)+10);  // lower module Id
// and its number of strips
      nStrD=AliMUONTriggerConstants::NstripX(ModuleNumber(idModuleD)); 
      
      iStripCircMT2=0;
      for (istrip=nStrD-8; istrip<nStrD; istrip++) {  
	fXcode[2][iStripCircMT2]=sign*(TMath::Abs(idModuleD)*100+istrip); 
	fXcode[3][iStripCircMT2]=sign*(TMath::Abs(idModuleD)*100+istrip); 
	iStripCircMT2++;
      }
    }
     
  } else {                       // lower strips within same module 
    
    iStripCircMT2=0;
    for (istrip=(iPosCircuit-1)*16-8; 
	 istrip<(iPosCircuit-1)*16; istrip++) {  
      fXcode[2][iStripCircMT2]=sign*(TMath::Abs(idModule)*100+istrip); 
      fXcode[3][iStripCircMT2]=sign*(TMath::Abs(idModule)*100+istrip); 
      iStripCircMT2++;
    }
  }
  
// fill upper part (24 to 31)
  if ((iPosCircuit==1&&nStrX==16)||(iPosCircuit==2&&nStrX==32)|| 
      (iPosCircuit==3&&nStrX==48)||(iPosCircuit==4&&nStrX==64)) {   
    if ((idModule>17||idModule<-17)&&TMath::Abs(idModule)!=61) {  
      idModuleU=sign*(TMath::Abs(idModule)-10);  // upper module Id
// and its number of strips
      nStrU=AliMUONTriggerConstants::NstripX(ModuleNumber(idModuleU)); 
      
      iStripCircMT2=24;
      for (istrip=0; istrip<8; istrip++) {  	  
	fXcode[2][iStripCircMT2]=sign*(TMath::Abs(idModuleU)*100+istrip); 
	fXcode[3][iStripCircMT2]=sign*(TMath::Abs(idModuleU)*100+istrip); 
	iStripCircMT2++;
      }
    }
    
  } else if ((iPosCircuit==1&&nStrX>16)||(iPosCircuit==2&&nStrX>32)|| 
	     (iPosCircuit==3&&nStrX>48)) { // upper strips within same mod. 
    
    iStripCircMT2=24;
    for (istrip=(iPosCircuit-1)*16+16; 
	 istrip<(iPosCircuit-1)*16+24; istrip++) {  
      fXcode[2][iStripCircMT2]=sign*(TMath::Abs(idModule)*100+istrip); 
      fXcode[3][iStripCircMT2]=sign*(TMath::Abs(idModule)*100+istrip); 
      iStripCircMT2++;
    }	
  }
}

//----------------------------------------------------------------------
void AliMUONTriggerCircuit::LoadYCode(){
// assign a Id. number to each Y strip of current circuit 
// Id.=(corresponding module Id.)*100+(Id. strip of module)
// note : for Y plane fill only "central part" of circuit
// (upper and lower parts are filled in PreHandlingY of AliMUONTriggerDecision)
    
  Int_t idModule=Module(fIdCircuit);        // corresponding module Id.
// and its number of Y strips
  Int_t nStrY=AliMUONTriggerConstants::NstripY(ModuleNumber(idModule)); 
  Int_t sign=TMath::Abs(idModule)/idModule; // left or right 

  for (Int_t istrip=0; istrip<nStrY; istrip++) {
    fYcode[0][istrip]=sign*(TMath::Abs(idModule)*100+istrip); 
    fYcode[1][istrip]=sign*(TMath::Abs(idModule)*100+istrip); 
    fYcode[2][istrip]=sign*(TMath::Abs(idModule)*100+istrip); 
    fYcode[3][istrip]=sign*(TMath::Abs(idModule)*100+istrip); 
  }
}

//----------------------------------------------------------------------
Float_t AliMUONTriggerCircuit::PtCal(Int_t istripX, Int_t idev, Int_t istripY){
// returns calculated pt for circuit/istripX/idev/istripY according 
// to the formula of the TRD. Note : idev (input) is in [0+30]

  //  Int_t jdev = idev - 15;        // jdev in [-15+15]
  Int_t istripX2=istripX+idev+1; // find istripX2 using istripX and idev

  Float_t yPosX1=fYpos11[istripX];
  Float_t yPosX2=fYpos21[istripX2];
  Float_t xPosY1=fXpos11[istripY];
  
  Float_t zf=975.;
  Float_t z1=AliMUONConstants::DefaultChamberZ(10);
  Float_t z2=AliMUONConstants::DefaultChamberZ(12);
  Float_t thetaDev=(1./zf)*(yPosX1*z2-yPosX2*z1)/(z2-z1);
  Float_t xf=xPosY1*zf/z1; 
  Float_t yf=yPosX2-((yPosX2-yPosX1)*(z2-zf))/(z2-z1);
  return (3.*0.3/TMath::Abs(thetaDev)) * TMath::Sqrt(xf*xf+yf*yf)/zf;
}
//---------------------------------------------------------------------
//----------------------- New Segmentation ----------------------------
//---------------------------------------------------------------------

//---------------------------------------------------------------------
void AliMUONTriggerCircuit::LoadYPos2(){
// fill fYpos11 and fYpos21 -> y position of X declusterized strips

  Int_t chamber, cathode;
  Int_t code, idModule, idStrip, idSector;
  Float_t x, y, z, width;
  Int_t istrip, idDE;

  AliMUON *pMUON  = (AliMUON*)gAlice->GetModule("MUON");  
  AliMUONGeometrySegmentation* segmentation;    
  
// first plane (11)
  chamber=11;
  cathode=1;
  segmentation
    = pMUON->GetSegmentation()->GetModuleSegmentation(chamber-1, cathode-1);
  
  if (!segmentation) {
    AliWarning("Segmentation not defined.");
    return;
  }  

  for (istrip=0; istrip<16; istrip++) {
    code=fXcode[0][istrip];           // decode current strip
    idModule=Int_t(code/100);           // corresponding module Id.
    idDE = DetElemId(chamber, idModule);
    idStrip=TMath::Abs(code-idModule*100); // corresp. strip number in module
    idSector=segmentation->Sector(idDE, idModule, idStrip); // corresponding sector
    width=segmentation->Dpy(idDE, idSector);      // corresponding strip width
    segmentation->GetPadC(idDE, idModule,idStrip,x,y,z); // get strip real position
    
    fYpos11[2*istrip]=y;
    if (istrip!=15) fYpos11[2*istrip+1]=y+width/2.;
  }   
   
// second plane (21)
  chamber=13;
  cathode=1;
  segmentation
    = pMUON->GetSegmentation()->GetModuleSegmentation(chamber-1, cathode-1);
  
  for (istrip=0; istrip<32; istrip++) {
    code=fXcode[2][istrip];    // decode current strip
    idModule=Int_t(code/100);           // corresponding module Id.
    idDE = DetElemId(chamber, idModule);
    if (idModule == 0) continue;
    idStrip=TMath::Abs(code-idModule*100); // corresp. strip number in module
    idSector=segmentation->Sector(idDE, idModule, idStrip); // corresponding sector
    width=segmentation->Dpy(idDE, idSector);      // corresponding strip width
    segmentation->GetPadC(idDE, idModule,idStrip,x,y,z); // get strip real position
    
// using idModule!=0 prevents to fill garbage in case of circuits 
// in the first and last rows 
    if (idModule!=0) { 
      fYpos21[2*istrip]=y;
      if (istrip!=31) fYpos21[2*istrip+1]=y+width/2.;
    }
  }   
}

//----------------------------------------------------------------------
void AliMUONTriggerCircuit::LoadXPos2(){
// fill fXpos11 -> x position of Y strips for the first plane only
// fXpos11 contains the x position of Y strip for the current circuit
// taking into account whether or nor not part(s) of the circuit
// (middle, up or down) has(have) 16 strips
  
  Float_t x, y, z;
  Int_t istrip, idDE;  

  Int_t chamber=11;
  Int_t cathode=2;
  AliMUON *pMUON  = (AliMUON*)gAlice->GetModule("MUON");  
  AliMUONGeometrySegmentation*  segmentation; 
  segmentation
    = pMUON->GetSegmentation()->GetModuleSegmentation(chamber-1, cathode-1);

  if (!segmentation) {
    AliWarning("Segmentation not defined.");
    return;
  }  
  
  Int_t idModule=Module(fIdCircuit);        // corresponding module Id.  
// number of Y strips
  idDE = DetElemId(chamber, idModule);

  Int_t nStrY=AliMUONTriggerConstants::NstripY(ModuleNumber(idModule)); 
  Int_t idSector=segmentation->Sector(idDE, idModule,0); // corresp. sector
  Float_t width=segmentation->Dpx(idDE, idSector);      // corresponding strip width
  
// first case : up middle and down parts have all 8 or 16 strip 
  if ((nStrY==16)||(nStrY==8&&fX2m==0&&fX2ud==0)) { 
    for (istrip=0; istrip<nStrY; istrip++) {
      segmentation->GetPadC(idDE, idModule,istrip,x,y,z); 
      AliDebug(1,Form("idDE %d idModule %d istrip %d x,y,z=%e,%e,%e\n",
                      idDE,idModule,istrip,x,y,z));
      fXpos11[istrip]=x;
    }
// second case : mixing 8 and 16 strips within same circuit      
  } else {
    for (istrip=0; istrip<nStrY; istrip++) {
      if (nStrY!=8) { printf(" bug in LoadXpos \n");}
      segmentation->GetPadC(idDE, idModule, istrip, x, y, z); 
      fXpos11[2*istrip]=x-width/4.;
      fXpos11[2*istrip+1]=fXpos11[2*istrip]+width/2.;
    }
  }   
}

//----------------------------------------------------------------------
//--- methods which return member data related info
//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::GetIdCircuit() const { 
// returns circuit Id
  return fIdCircuit;
}
//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::GetIdModule() const { 
// returns module Id
  return Module(fIdCircuit);
}
//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::GetNstripX() const { 
// returns the number of X strips in the module where the circuit is sitting
  return AliMUONTriggerConstants::NstripX(ModuleNumber(Module(fIdCircuit)));
}
//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::GetNstripY() const { 
// returns the number of Y strips in the module where the circuit is sitting
  return AliMUONTriggerConstants::NstripY(ModuleNumber(Module(fIdCircuit)));
}
//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::GetPosCircuit() const { 
// returns the position of the circuit in its module
  return Position(fIdCircuit);
}
//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::GetIdCircuitD() const {
// returns the Id of the circuit down 
  Int_t idModule=Module(fIdCircuit);
  Int_t idModuleD=(TMath::Abs(idModule)+10)*(TMath::Abs(idModule)/idModule); 
  return (TMath::Abs(idModuleD)*10+1)*(TMath::Abs(idModule)/idModule);
}
//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::GetICircuitD() const {
// returns the number of the circuit down 
  Int_t idModule=Module(fIdCircuit);
  Int_t idModuleD=(TMath::Abs(idModule)+10)*(TMath::Abs(idModule)/idModule); 
  Int_t idCircuitD=
    (TMath::Abs(idModuleD)*10+1)*(TMath::Abs(idModule)/idModule);
  return CircuitNumber(idCircuitD);
}
//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::GetIdCircuitU() const {
// returns the Id of the circuit up 
  Int_t idModule=Module(fIdCircuit);
  Int_t idModuleU=(TMath::Abs(idModule)-10)*(TMath::Abs(idModule)/idModule); 
  return (TMath::Abs(idModuleU)*10+1)*(TMath::Abs(idModule)/idModule);
}
//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::GetICircuitU() const {
// returns the number of the circuit up 
  Int_t idModule=Module(fIdCircuit);
  Int_t idModuleU=(TMath::Abs(idModule)-10)*(TMath::Abs(idModule)/idModule); 
  Int_t idCircuitU=
    (TMath::Abs(idModuleU)*10+1)*(TMath::Abs(idModule)/idModule);
  return CircuitNumber(idCircuitU);
}
//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::GetX2m() const { 
// returns fX2m
  return fX2m;
}
//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::GetX2ud() const { 
// returns fX2ud
  return fX2ud;
}
//----------------------------------------------------------------------
void AliMUONTriggerCircuit::GetOrMud(Int_t orMud[2]) const {
// returns fOrMud 
  orMud[0]=fOrMud[0];
  orMud[1]=fOrMud[1];
}
//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::GetXcode(Int_t chamber, Int_t istrip) const {
// returns X code of circuit/chamber/istrip (warning : chamber in [0,3])
  return fXcode[chamber][istrip];
}
//----------------------------------------------------------------------
Int_t AliMUONTriggerCircuit::GetYcode(Int_t chamber, Int_t istrip) const {
// returns Y code of circuit/chamber/istrip (warning : chamber in [0,3])
  return fYcode[chamber][istrip];
}
//----------------------------------------------------------------------
Float_t AliMUONTriggerCircuit::GetY11Pos(Int_t istrip) const {
// returns Y position of X strip istrip in MC11
  return fYpos11[istrip];
}
//----------------------------------------------------------------------
Float_t AliMUONTriggerCircuit::GetY21Pos(Int_t istrip) const {
// returns Y position of X strip istrip in MC21
  return fYpos21[istrip];
}
//----------------------------------------------------------------------
Float_t AliMUONTriggerCircuit::GetX11Pos(Int_t istrip) const {
// returns X position of Y strip istrip in MC11
  return fXpos11[istrip];
}
//----------------------------------------------------------------------
//--- end of methods which return member data related info
//----------------------------------------------------------------------

void dump(const char* what, const Float_t* array, Int_t size)
{
  cout << what << " " << endl;
  for ( Int_t i = 0; i < size; ++i )
  {
    cout << array[i] << " , ";
  }
  cout << endl;
}

void dump(const char* what, const Int_t* array, Int_t size)
{
  cout << what << " " << endl;
  for ( Int_t i = 0; i < size; ++i )
  {
    cout << array[i] << " , ";
  }
  cout << endl;
}

//_____________________________________________________________________________
void
AliMUONTriggerCircuit::Print(Option_t* ) const
{
  cout << "IdCircuit " << fIdCircuit << " X2m,X2ud=" << fX2m << ","
  << fX2ud;
  for ( Int_t i = 0; i < 2; ++i )
  {
    cout << " OrMud[" << i << "]=" << fOrMud[i];
  }
  cout << endl;
  dump("Xpos11",fXpos11,16);
  dump("Ypos11",fYpos11,31);
  dump("Ypos21",fYpos21,63);
  for ( Int_t i = 0; i < 4; ++i )
  {
    char s[80];
    sprintf(s,"Xcode[%d]",i);
    dump(s,fXcode[i],32);
    sprintf(s,"Ycode[%d]",i);
    dump(s,fYcode[i],32);
  }
 // Int_t fIdCircuit;            // circuit Id number
//  Int_t fX2m;                  // internal info needed by TriggerDecision
//  Int_t fX2ud;                 // internal info needed by TriggerDecision
//  Int_t fOrMud[2];             // internal info needed by TriggerDecision
//  Int_t fXcode[4][32];         // code of X strips
//  Int_t fYcode[4][32];         // code of Y strips 
//  Float_t fXpos11[16];         // X position of Y strips in MC11
//  Float_t fYpos11[31];         // Y position of X strips in MC11
//  Float_t fYpos21[63];         // Y position of X strips in MC21
  
}

Int_t AliMUONTriggerCircuit::DetElemId(Int_t ichamber, Int_t idModule)
{
// adpated to official numbering (09/20/05)
// returns the detection element Id for given chamber and module
// ichamber (from 11 to 14), idModule (from -97 to 97)
//    
    Int_t itmp=0;    
    Int_t linenumber=Int_t(idModule/10);    
    switch (linenumber) // (from 1 to 9, from top to bottom)
    {
    case 1:
	itmp = 4;
	break;	
    case 2:
	itmp = 3;
	break;		
    case 3:
	itmp = 2;
	break;
    case 4:
	itmp = 1;
	break;	    
    case 5:
	itmp = 0;
	break;		
    case 6:
	itmp = 17;
	break;		
    case 7:
	itmp = 16;
	break;		
    case 8:
	itmp = 15;
	break;		
    case 9:
	itmp = 14;
	break;		
// left
    case -1:
	itmp = 5;
	break;		
    case -2:
	itmp = 6;
	break;		
    case -3:
	itmp = 7;
	break;		
    case -4:
	itmp = 8;
	break;		
    case -5:
	itmp = 9;
	break;		
    case -6:
	itmp = 10;
	break;		
    case -7:
	itmp = 11;
	break;		
    case -8:
	itmp = 12;
	break;
    case -9:
	itmp = 13;
	break;		
    }
    return (ichamber*100)+itmp;    
}




