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

/*
$Log$
Revision 1.9  2002/06/07 06:13:07  kowal2
Corrected calculation of the cross talk in the outer sectors

Revision 1.8  2002/06/05 15:37:31  kowal2
Added cross-talk from the wires beyond the first and the last rows

Revision 1.7  2002/03/18 17:59:13  kowal2
Chnges in the pad geometry - 3 pad lengths introduced.

Revision 1.6  2002/02/25 11:02:56  kowal2
Changes towards speeding up the code. Thanks to Marian Ivanov.

Revision 1.5  2001/12/06 07:49:30  kowal2
corrected number of pads calculation

Revision 1.4  2000/11/02 07:33:15  kowal2
Improvements of the code.

Revision 1.3  2000/06/30 12:07:50  kowal2
Updated from the TPC-PreRelease branch

Revision 1.2.4.2  2000/06/14 16:48:24  kowal2
Parameter setting improved. Removed compiler warnings

Revision 1.2.4.1  2000/06/09 07:55:39  kowal2

Updated defaults

Revision 1.2  2000/04/17 09:37:33  kowal2
removed obsolete AliTPCDigitsDisplay.C

Revision 1.1.4.2  2000/04/10 11:36:13  kowal2

New Detector parameters handling class

*/

///////////////////////////////////////////////////////////////////////
//  Manager and of geomety  classes for set: TPC                     //
//                                                                   //
//  !sectors are numbered from  0                                     //
//  !pad rows are numbered from 0                                     //
//  
//  27.7.   - AliTPCPaaramSr object for TPC 
//            TPC with straight pad rows 
//  Origin:  Marian Ivanov, Uni. of Bratislava, ivanov@fmph.uniba.sk // 
//                                                                   //  
///////////////////////////////////////////////////////////////////////


#include <iostream.h>
#include <TMath.h>
#include <TObject.h>
#include <AliTPCParamSR.h>
#include "AliTPCPRF2D.h"
#include "AliTPCRF1D.h"
#include "TH1.h"


ClassImp(AliTPCParamSR)
const static  Int_t kMaxRows=600;
const static  Float_t  kEdgeSectorSpace = 2.5;
const static Float_t kFacSigmaPadRow=3.;
const static Float_t kFacSigmaPad=3.;
const static Float_t kFacSigmaTime=3.;


AliTPCParamSR::AliTPCParamSR()
{   
  //
  //constructor set the default parameters
  fInnerPRF=0;
  fOuter1PRF=0;
  fOuter2PRF=0;
  fTimeRF = 0;
  fFacSigmaPadRow = Float_t(kFacSigmaPadRow);
  fFacSigmaPad = Float_t(kFacSigmaPad);
  fFacSigmaTime = Float_t(kFacSigmaTime);
  SetDefault();
  Update();
}

AliTPCParamSR::~AliTPCParamSR()
{
  //
  //destructor destroy some dynmicaly alocated variables
  if (fInnerPRF != 0) delete fInnerPRF;
  if (fOuter1PRF != 0) delete fOuter1PRF;
  if (fOuter2PRF != 0) delete fOuter2PRF;
  if (fTimeRF != 0) delete fTimeRF;
}

void AliTPCParamSR::SetDefault()
{
  //set default TPC param   
  fbStatus = kFALSE;
  AliTPCParam::SetDefault();  
}  

Int_t  AliTPCParamSR::CalcResponse(Float_t* xyz, Int_t * index, Int_t row)
{
  //
  //calculate bin response as function of the input position -x 
  //return number of valid response bin
  //
  //we suppose that coordinate is expressed in float digits 
  // it's mean coordinate system 8
  //xyz[0] - float padrow xyz[1] is float pad  (center pad is number 0) and xyz[2] is float time bin
  if ( (fInnerPRF==0)||(fOuter1PRF==0)||(fOuter2PRF==0) ||(fTimeRF==0) ){ 
    Error("AliTPCParamSR", "response function was not adjusted");
    return -1;
  }
  
  Float_t sfpadrow;   // sigma of response function
  Float_t sfpad;      // sigma  of 
  Float_t sftime= fFacSigmaTime*fTimeRF->GetSigma()/fZWidth;     //3 sigma of time response
  if (index[1]<fNInnerSector){
    sfpadrow =fFacSigmaPadRow*fInnerPRF->GetSigmaY()/fInnerPadPitchLength;
    sfpad    =fFacSigmaPad*fInnerPRF->GetSigmaX()/fInnerPadPitchWidth;
  }
  else{
  if(row<fNRowUp1){
    sfpadrow =fFacSigmaPadRow*fOuter1PRF->GetSigmaY()/fOuter1PadPitchLength;
    sfpad    =fFacSigmaPad*fOuter1PRF->GetSigmaX()/fOuterPadPitchWidth;}
    else{
      sfpadrow =fFacSigmaPadRow*fOuter2PRF->GetSigmaY()/fOuter2PadPitchLength;
      sfpad    =fFacSigmaPad*fOuter2PRF->GetSigmaX()/fOuterPadPitchWidth;
    }   
  }

  Int_t fpadrow = TMath::Max(TMath::Nint(index[2]+xyz[0]-sfpadrow),0);  //"first" padrow
  Int_t fpad    = TMath::Nint(xyz[1]-sfpad);     //first pad
  Int_t ftime   = TMath::Max(TMath::Nint(xyz[2]+GetZOffset()/GetZWidth()-sftime),0);  // first time
  Int_t lpadrow = TMath::Min(TMath::Nint(index[2]+xyz[0]+sfpadrow),fpadrow+19);  //"last" padrow
  lpadrow       = TMath::Min(GetNRow(index[1])-1,lpadrow);
  Int_t lpad    = TMath::Min(TMath::Nint(xyz[1]+sfpad),fpad+19);     //last pad
  Int_t ltime   = TMath::Min(TMath::Nint(xyz[2]+GetZOffset()/GetZWidth()+sftime),ftime+19);    // last time
  ltime         = TMath::Min(ltime,GetMaxTBin()-1); 
  // 
  Int_t npads = GetNPads(index[1],row);
  if (fpad<-npads/2) 
    fpad = -npads/2;
  if (lpad>npads/2) 
    lpad= npads/2;
  if (ftime<0) ftime=0;
  // 
  if (row>=0) { //if we are interesting about given pad row
    if (fpadrow<=row) fpadrow =row;
    else 
      return 0;
    if (lpadrow>=row) lpadrow = row;
    else 
      return 0;
  }

 
  Float_t  padres[20][20];  //I don't expect bigger number of bins
  Float_t  timeres[20];     
  Int_t cindex3=0;
  Int_t cindex=0;
  Float_t cweight = 0;
  if (fpadrow>=0) {
  //calculate padresponse function    
  Int_t padrow, pad;
  for (padrow = fpadrow;padrow<=lpadrow;padrow++)
    for (pad = fpad;pad<=lpad;pad++){
      Float_t dy = (xyz[0]+Float_t(index[2]-padrow));
      Float_t dx = (xyz[1]+Float_t(pad));
      if (index[1]<fNInnerSector)
	padres[padrow-fpadrow][pad-fpad]=fInnerPRF->GetPRF(dx*fInnerPadPitchWidth,dy*fInnerPadPitchLength);
      else{
	if(row<fNRowUp1){
	padres[padrow-fpadrow][pad-fpad]=fOuter1PRF->GetPRF(dx*fOuterPadPitchWidth,dy*fOuter1PadPitchLength);}
	else{
	  padres[padrow-fpadrow][pad-fpad]=fOuter2PRF->GetPRF(dx*fOuterPadPitchWidth,dy*fOuter2PadPitchLength);}}}
  //calculate time response function
  Int_t time;
  for (time = ftime;time<=ltime;time++) 
    timeres[time-ftime]= fTimeRF->GetRF((-xyz[2]+Float_t(time))*fZWidth);     
  //write over threshold values to stack
  for (padrow = fpadrow;padrow<=lpadrow;padrow++)
    for (pad = fpad;pad<=lpad;pad++)
      for (time = ftime;time<=ltime;time++){
	cweight = timeres[time-ftime]*padres[padrow-fpadrow][pad-fpad];
	if (cweight>fResponseThreshold) {
	  fResponseBin[cindex3]=padrow;
	  fResponseBin[cindex3+1]=pad;
	  fResponseBin[cindex3+2]=time;
	  cindex3+=3;  
	  fResponseWeight[cindex]=cweight;
	  cindex++;
	}
      }
  }
  fCurrentMax=cindex;	
  return fCurrentMax;
}

void AliTPCParamSR::TransformTo8(Float_t *xyz, Int_t *index) const
{
  //
  // transformate point to digit coordinate
  //
  if (index[0]==0) Transform0to1(xyz,index);
  if (index[0]==1) Transform1to2(xyz,index);
  if (index[0]==2) Transform2to3(xyz,index);
  if (index[0]==3) Transform3to4(xyz,index);
  if (index[0]==4) Transform4to8(xyz,index);
}

void AliTPCParamSR::TransformTo2(Float_t *xyz, Int_t *index) const
{
  //
  //transformate point to rotated coordinate
  //
  //we suppose that   
  if (index[0]==0) Transform0to1(xyz,index);
  if (index[0]==1) Transform1to2(xyz,index);
  if (index[0]==4) Transform4to3(xyz,index);
  if (index[0]==8) {  //if we are in digit coordinate system transform to global
    Transform8to4(xyz,index);
    Transform4to3(xyz,index);  
  }
}

void AliTPCParamSR::CRXYZtoXYZ(Float_t *xyz,
	       const Int_t &sector, const Int_t & padrow, Int_t option) const  
{  
  //transform relative coordinates to absolute
  Bool_t rel = ( (option&2)!=0);
  Int_t index[2]={sector,padrow};
  if (rel==kTRUE)      Transform4to3(xyz,index);//if the position is relative to pad row  
  Transform2to1(xyz,index);
}

void AliTPCParamSR::XYZtoCRXYZ(Float_t *xyz,
			     Int_t &sector, Int_t & padrow, Int_t option) const
{
   //transform global position to the position relative to the sector padrow
  //if option=0  X calculate absolute            calculate sector
  //if option=1  X           absolute            use input sector
  //if option=2  X           relative to pad row calculate sector
  //if option=3  X           relative            use input sector
  //!!!!!!!!! WE start to calculate rows from row = 0
  Int_t index[2];
  Bool_t rel = ( (option&2)!=0);  

  //option 0 and 2  means that we don't have information about sector
  if ((option&1)==0)   Transform0to1(xyz,index);  //we calculate sector number 
  else
    index[0]=sector;
  Transform1to2(xyz,index);
  Transform2to3(xyz,index);
  //if we store relative position calculate position relative to pad row
  if (rel==kTRUE) Transform3to4(xyz,index);
  sector = index[0];
  padrow = index[1];
}

Float_t AliTPCParamSR::GetPrimaryLoss(Float_t *x, Int_t *index, Float_t *angle)
{
  //
  //
  Float_t padlength=GetPadPitchLength(index[1]);
  Float_t a1=TMath::Sin(angle[0]);
  a1*=a1;
  Float_t a2=TMath::Sin(angle[1]);
  a2*=a2;
  Float_t length =padlength*TMath::Sqrt(1+a1+a2);
  return length*fNPrimLoss;
}

Float_t AliTPCParamSR::GetTotalLoss(Float_t *x, Int_t *index, Float_t *angle)
{
  //
  //
  Float_t padlength=GetPadPitchLength(index[1]);
  Float_t a1=TMath::Sin(angle[0]);
  a1*=a1;
  Float_t a2=TMath::Sin(angle[1]);
  a2*=a2;
  Float_t length =padlength*TMath::Sqrt(1+a1+a2);
  return length*fNTotalLoss;
  
}


void AliTPCParamSR::GetClusterSize(Float_t *x, Int_t *index, Float_t *angle, Int_t mode, Float_t *sigma)
{
  //
  //return cluster sigma2 (x,y) for particle at position x
  // in this case x coordinata is in drift direction
  //and y in pad row direction
  //we suppose that input coordinate system is digit system
   
  Float_t  xx;
  Float_t lx[3] = {x[0],x[1],x[2]};
  Int_t   li[3] = {index[0],index[1],index[2]};
  TransformTo2(lx,li);
  //  Float_t  sigmadiff;
  sigma[0]=0;
  sigma[1]=0;
  
  xx = lx[2];  //calculate drift length in cm
  if (xx>0) {
    sigma[0]+= xx*GetDiffL()*GetDiffL();
    sigma[1]+= xx*GetDiffT()*GetDiffT(); 
  }


  //sigma[0]=sigma[1]=0;
  if (GetTimeRF()!=0) sigma[0]+=GetTimeRF()->GetSigma()*GetTimeRF()->GetSigma();
  if ( (index[1]<fNInnerSector) &&(GetInnerPRF()!=0))   
    sigma[1]+=GetInnerPRF()->GetSigmaX()*GetInnerPRF()->GetSigmaX();
  if ( (index[1]>=fNInnerSector) &&(index[2]<fNRowUp1) && (GetOuter1PRF()!=0))
    sigma[1]+=GetOuter1PRF()->GetSigmaX()*GetOuter1PRF()->GetSigmaX();
  if( (index[1]>=fNInnerSector) &&(index[2]>=fNRowUp1) && (GetOuter2PRF()!=0))
    sigma[1]+=GetOuter2PRF()->GetSigmaX()*GetOuter2PRF()->GetSigmaX();


  sigma[0]/= GetZWidth()*GetZWidth();
  sigma[1]/=GetPadPitchWidth(index[0])*GetPadPitchWidth(index[0]);
}




void AliTPCParamSR::GetSpaceResolution(Float_t *x, Int_t *index, Float_t *angle, 
				       Float_t amplitude, Int_t mode, Float_t *sigma)
{
  //
  //
  //
  
}
Float_t  AliTPCParamSR::GetAmp(Float_t *x, Int_t *index, Float_t *angle)
{
  //
  //
  //
  return 0;
}

Float_t * AliTPCParamSR::GetAnglesAccMomentum(Float_t *x, Int_t * index, Float_t* momentum, Float_t *angle)
{
  //
  //calculate angle of track to padrow at given position
  // for given magnetic field and momentum of the particle
  //

  TransformTo2(x,index);
  AliDetectorParam::GetAnglesAccMomentum(x,index,momentum,angle);    
  Float_t addangle = TMath::ASin(x[1]/GetPadRowRadii(index[1],index[2]));
  angle[1] +=addangle;
  return angle;				 
}

         
Bool_t AliTPCParamSR::Update()
{
  Int_t i;
  if (AliTPCParam::Update()==kFALSE) return kFALSE;
  fbStatus = kFALSE;

 Float_t firstrow = fInnerRadiusLow + 2.225 ;   
 for( i= 0;i<fNRowLow;i++)
   {
     Float_t x = firstrow + fInnerPadPitchLength*(Float_t)i;  
     fPadRowLow[i]=x;
     // number of pads per row
     Float_t y = (x-0.5*fInnerPadPitchLength)*tan(fInnerAngle/2.)-fInnerWireMount-
       fInnerPadPitchWidth/2.;
     // 0 and fNRowLow+1 reserved for cross talk rows
     fYInner[i+1]  = x*tan(fInnerAngle/2.)-fInnerWireMount;
     fNPadsLow[i] = 1+2*(Int_t)(y/fInnerPadPitchWidth) ;
   }
 // cross talk rows
 fYInner[0]=(fPadRowLow[0]-fInnerPadPitchLength)*tan(fInnerAngle/2.)-fInnerWireMount;
 fYInner[fNRowLow+1]=(fPadRowLow[fNRowLow-1]+fInnerPadPitchLength)*tan(fInnerAngle/2.)-fInnerWireMount; 
 firstrow = fOuterRadiusLow + 1.6;
 for(i=0;i<fNRowUp;i++)
   {
     if(i<fNRowUp1){
       Float_t x = firstrow + fOuter1PadPitchLength*(Float_t)i; 
       fPadRowUp[i]=x;
    Float_t y =(x-0.5*fOuter1PadPitchLength)*tan(fOuterAngle/2.)-fOuterWireMount-
	  fOuterPadPitchWidth/2.;
     fYOuter[i+1]= x*tan(fOuterAngle/2.)-fOuterWireMount;
     fNPadsUp[i] = 1+2*(Int_t)(y/fOuterPadPitchWidth) ;
     if(i==fNRowUp1-1) {
       fLastWireUp1=fPadRowUp[i] +0.375;
       firstrow = fPadRowUp[i] + 0.5*(fOuter1PadPitchLength+fOuter2PadPitchLength);
     }
     }
     else
       {
	 Float_t x = firstrow + fOuter2PadPitchLength*(Float_t)(i-64);
         fPadRowUp[i]=x;
Float_t y =(x-0.5*fOuter2PadPitchLength)*tan(fOuterAngle/2.)-fOuterWireMount-
	   fOuterPadPitchWidth/2.;
         fNPadsUp[i] = 1+2*(Int_t)(y/fOuterPadPitchWidth) ; 
       }
     fYOuter[i+1]  = fPadRowUp[i]*tan(fOuterAngle/2.)-fOuterWireMount;
   }
 // cross talk rows
 fYOuter[0]=(fPadRowUp[0]-fOuter1PadPitchLength)*tan(fOuterAngle/2.)-fOuterWireMount;
 fYOuter[fNRowUp+1]=(fPadRowUp[fNRowUp-1]+fOuter2PadPitchLength)*tan(fOuterAngle/2.)-fOuterWireMount;
 fNtRows = fNInnerSector*fNRowLow+fNOuterSector*fNRowUp;
 fbStatus = kTRUE;
 return kTRUE;
}
Float_t AliTPCParamSR::GetYInner(Int_t irow) const
{
  return fYInner[irow];
}
Float_t AliTPCParamSR::GetYOuter(Int_t irow) const
{
  return fYOuter[irow];
}

void AliTPCParamSR::Streamer(TBuffer &R__b)
{
   // Stream an object of class AliTPC.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      //      TObject::Streamer(R__b);
      AliTPCParam::Streamer(R__b);
      //      if (R__v < 2) return;
       Update();
   } else {
      R__b.WriteVersion(AliTPCParamSR::IsA());
      //TObject::Streamer(R__b);  
      AliTPCParam::Streamer(R__b);    
   }
}
Int_t  AliTPCParamSR::CalcResponseFast(Float_t* xyz, Int_t * index, Int_t row)
{
  //
  //calculate bin response as function of the input position -x 
  //return number of valid response bin
  //
  //we suppose that coordinate is expressed in float digits 
  // it's mean coordinate system 8
  //xyz[0] - electron position w.r.t. pad center, normalized to pad length,
  //xyz[1] is float pad  (center pad is number 0) and xyz[2] is float time bin
  if ( (fInnerPRF==0)||(fOuter1PRF==0)||(fOuter2PRF==0) ||(fTimeRF==0) ){ 
    Error("AliTPCParamSR", "response function was not adjusted");
    return -1;
  }
  
  const Int_t padn =  500;
  const Float_t fpadn =  500.;
  const Int_t timen = 500;
  const Float_t ftimen = 500.;
  const Int_t padrn = 500;
  const Float_t fpadrn = 500.;

 

  static Float_t prfinner[2*padrn][5*padn];  //pad divided by 50
  static Float_t prfouter1[2*padrn][5*padn];  //prfouter division
  static Float_t prfouter2[2*padrn][5*padn];

  static Float_t rftime[5*timen];         //time division
  static Int_t blabla=0;
  static Float_t zoffset=0;
  static Float_t zwidth=0;
  static Float_t zoffset2=0;
  static TH1F * hdiff=0;
  static TH1F * hdiff1=0;
  static TH1F * hdiff2=0;
  
  if (blabla==0) {  //calculate Response function - only at the begginning
    hdiff =new TH1F("prf_diff","prf_diff",10000,-1,1);
    hdiff1 =new TH1F("no_repsonse1","no_response1",10000,-1,1);
    hdiff2 =new TH1F("no_response2","no_response2",10000,-1,1);
    
    blabla=1;
    zoffset = GetZOffset();
    zwidth  = fZWidth;
    zoffset2 = zoffset/zwidth;
    for (Int_t i=0;i<5*timen;i++){
      rftime[i] = fTimeRF->GetRF(((i-2.5*ftimen)/ftimen)*zwidth+zoffset);
    }
    for (Int_t i=0;i<5*padn;i++){    
      for (Int_t j=0;j<2*padrn;j++){
	prfinner[j][i] =
	  fInnerPRF->GetPRF((i-2.5*fpadn)/fpadn
			    *fInnerPadPitchWidth,(j-fpadrn)/fpadrn*fInnerPadPitchLength);
	prfouter1[j][i] =
	  fOuter1PRF->GetPRF((i-2.5*fpadn)/fpadn
			    *fOuterPadPitchWidth,(j-fpadrn)/fpadrn*fOuter1PadPitchLength);

	//
	prfouter2[j][i] =
	  fOuter2PRF->GetPRF((i-2.5*fpadn)/fpadn
			    *fOuterPadPitchWidth,(j-fpadrn)/fpadrn*fOuter2PadPitchLength);
      }
    }      
  } // the above is calculated only once

  // calculate central padrow, pad, time
  Int_t npads = GetNPads(index[1],index[3]-1);
  Int_t cpadrow = index[2]; // electrons are here
  Int_t cpad    = TMath::Nint(xyz[1]);
  Int_t ctime   = TMath::Nint(xyz[2]+zoffset2);
  //calulate deviation
  Float_t dpadrow = xyz[0];
  Float_t dpad    = xyz[1]-cpad;
  Float_t dtime   = xyz[2]+zoffset2-ctime;
  Int_t cindex =0;
  Int_t cindex3 =0;
  Int_t maxt =GetMaxTBin();

  Int_t fpadrow;
  Int_t lpadrow;

  if (row>=0) { //if we are interesting about given pad row
    fpadrow = row-cpadrow;
    lpadrow = row-cpadrow;
  }else{
    fpadrow = (index[2]>1) ? -1 :0;
    lpadrow = (index[2]<GetNRow(index[1])-1) ? 1:0;
  }
  Int_t fpad =  (cpad > -npads/2+1) ? -2: -npads/2-cpad;
  Int_t lpad =  (cpad < npads/2-1)  ?  2: npads/2-cpad;
  Int_t ftime =  (ctime>1) ? -2: -ctime;
  Int_t ltime =  (ctime<maxt-2) ? 2: maxt-ctime-1;

  // cross talk from long pad to short one
  if(row==fNRowUp1 && fpadrow==-1) {
    dpadrow *= fOuter2PadPitchLength;
    dpadrow += fOuterWWPitch;
    dpadrow /= fOuter1PadPitchLength;
  }    
  // cross talk from short pad to long one
  if(row==fNRowUp1+1 && fpadrow==1){ 
    dpadrow *= fOuter1PadPitchLength;
    if(dpadrow < 0.) dpadrow = -1.; //protection against 3rd wire
    dpadrow += fOuterWWPitch;
    dpadrow /= fOuter2PadPitchLength;
    
  }
  // "normal"
  Int_t apadrow = TMath::Nint((dpadrow-fpadrow)*fpadrn+fpadrn);
  for (Int_t ipadrow = fpadrow; ipadrow<=lpadrow;ipadrow++){
    if ( (apadrow<0) || (apadrow>=2*padrn)) 
      continue;
    Int_t apad= TMath::Nint((dpad-fpad)*fpadn+2.5*fpadn);
    for (Int_t ipad = fpad; ipad<=lpad;ipad++){
	Float_t cweight;
	if (index[1]<fNInnerSector)
	  cweight=prfinner[apadrow][apad];
	else{
	  if(row < fNRowUp1+1)
	    cweight=prfouter1[apadrow][apad];
          else cweight=prfouter2[apadrow][apad];
	}

	//	if (cweight<fResponseThreshold) continue;
	Int_t atime = TMath::Nint((dtime-ftime)*ftimen+2.5*ftimen);
	for (Int_t itime = ftime;itime<=ltime;itime++){	
	  Float_t cweight2 = cweight*rftime[atime];
	  if (cweight2>fResponseThreshold) {
	    fResponseBin[cindex3++]=cpadrow+ipadrow;
	    fResponseBin[cindex3++]=cpad+ipad;
	    fResponseBin[cindex3++]=ctime+itime;
	    fResponseWeight[cindex++]=cweight2;
	    
	    if (cweight2>100) 
	      {
		printf("Pici pici %d %f %d\n",ipad,dpad,apad);
	      }
	    
	  }
	  atime-=timen;
	}
	apad-= padn;	
    }
    apadrow-=padrn;
  }
  fCurrentMax=cindex;	
  return fCurrentMax;    
  
}







