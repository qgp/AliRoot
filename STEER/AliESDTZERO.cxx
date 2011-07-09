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

//-------------------------------------------------------------------------
//                        Implemenation Class AliESDTZERO
//   This is a class that summarizes the TZERO data for the ESD   
//   Origin: Christian Klein-Boesing, CERN, Christian.Klein-Boesing@cern.ch 
//-------------------------------------------------------------------------



#include "AliESDTZERO.h"
#include "AliLog.h"


ClassImp(AliESDTZERO)

//______________________________________________________________________________
AliESDTZERO::AliESDTZERO() :
  TObject(),
  fT0clock(0),
  fT0zVertex(0),
  fT0timeStart(0),   
  fT0trig(0),
  fPileup(kFALSE),
  fSattelite(kFALSE),
  fMultC(0),
  fMultA(0)

{
  for(int i = 0;i<24;i++) {
    fT0time[i] = fT0amplitude[i] = 0;
    for(Int_t iHit=0; iHit<5; iHit++) {
      fTimeFull[i][iHit] = 0;   
      fOrA[iHit] = 0; 
      fOrC[iHit] = 0;  
      fTVDC[iHit] = 0; 
    }
  }
  for(int i = 0;i<3;i++) fT0TOF[i]=0;
}
//______________________________________________________________________________
AliESDTZERO::AliESDTZERO(const AliESDTZERO &tzero ) :
  TObject(tzero),
  fT0clock(tzero.fT0clock),  
  fT0zVertex(tzero.fT0zVertex),
  fT0timeStart(tzero.fT0timeStart),
  fT0trig(tzero.fT0trig),
  fPileup(tzero.fPileup),
  fSattelite(tzero.fSattelite),
  fMultC(tzero.fMultC),
  fMultA(tzero.fMultA)
{
  // copy constuctor
  for(int i = 0;i<3;i++) fT0TOF[i] = tzero.fT0TOF[i];

  for(int i = 0;i<24;i++){
    fT0time[i] = tzero.fT0time[i]; 
    fT0amplitude[i] = tzero.fT0amplitude[i];
    for(Int_t iHit=0; iHit<5; iHit++) {
      fTimeFull[i][iHit] = tzero.fTimeFull[i][iHit];   
      fOrA[iHit] = tzero.fOrA[iHit]; 
      fOrC[iHit] = tzero.fOrC[iHit];  
      fTVDC[iHit] = tzero.fTVDC[iHit]; 
    }
  }
}
//______________________________________________________________________________
AliESDTZERO& AliESDTZERO::operator=(const AliESDTZERO& tzero){
  // assigmnent operator
  if(this!=&tzero) {
    TObject::operator=(tzero);
    fT0clock = tzero.fT0clock;
    fT0zVertex = tzero.fT0zVertex;
    fT0timeStart = tzero.fT0timeStart;
    fPileup = tzero.fPileup;
    fSattelite = tzero.fSattelite;
    fMultC = tzero.fMultC;
    fMultA = tzero.fMultA;
    fT0trig = tzero.fT0trig;
    for(int i = 0;i<3;i++) fT0TOF[i] = tzero.fT0TOF[i];
    for(int i = 0;i<24;i++){
      fT0time[i] = tzero.fT0time[i]; 
      fT0amplitude[i] = tzero.fT0amplitude[i];
      for(Int_t iHit=0; iHit<5; iHit++) {
	fTimeFull[i][iHit] = tzero.fTimeFull[i][iHit];   
	fOrA[iHit] = tzero.fOrA[iHit]; 
	fOrC[iHit] = tzero.fOrC[iHit];  
	fTVDC[iHit] = tzero.fTVDC[iHit]; 
      }
   }
  } 
  return *this;
}
//______________________________________________________________________________
void AliESDTZERO::Copy(TObject &obj) const {
  
  // this overwrites the virtual TOBject::Copy()
  // to allow run time copying without casting
  // in AliESDEvent

  if(this==&obj)return;
  AliESDTZERO *robj = dynamic_cast<AliESDTZERO*>(&obj);
  if(!robj)return; // not an AliESDTZERO
  *robj = *this;

}


//______________________________________________________________________________
void AliESDTZERO::Reset()
{
  // reset contents
  fT0clock=0;
  fT0zVertex = 0;  
  fT0timeStart = 0;
  for(int i = 0;i<24;i++) {
    fT0time[i] = fT0amplitude[i] =  0;
    for(Int_t iHit=0; iHit<5; iHit++)  fTimeFull[i][iHit] = -10000;
  }
  for(Int_t iHit=0; iHit<5; iHit++) fOrA[iHit] = fOrC[iHit] = fTVDC[iHit] = -10000; 

  for(int i = 0;i<3;i++) fT0TOF[i] = 0;
}

//______________________________________________________________________________
void AliESDTZERO::Print(const Option_t *) const
{
  // does noting fornow
  AliInfo(Form(" Vertex %f (T0A+T0C)/2 %f #channels T0signal %f ns OrA %f ns OrC %f \n",fT0zVertex,  fT0timeStart, fT0TOF[0],fT0TOF[1],fT0TOF[2]));

  Bool_t tr[5];
  for (Int_t i=0; i<5; i++) tr[i] = fT0trig & (1<<i);
  AliInfo(Form("T0 triggers %d %d %d %d %d",tr[0],tr[1],tr[2],tr[3],tr[4])); 

}
