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

#include <Riostream.h> 
#include <TMath.h>
#include <TRandom.h>

#include "AliRICHParam.h"

ClassImp(AliRICHParam)

// RICH main parameters manipulator
//__________________________________________________________________________________________________
AliRICHParam::AliRICHParam():
fCurrentPadX(0),fCurrentPadY(0),fCurrentWire(0),
fSizeZ(0),
fAngleRot(0),fAngleYZ(0),fAngleXY(0),
fOffset(0),
fProximityGapThickness(0),
fQuartzLength(0),
fQuartzWidth(0),
fOuterFreonLength(0),
fOuterFreonWidth(0),
fInnerFreonLength(0),
fInnerFreonWidth(0),
fChargeSlope(0),
fChargeSpreadX(0),
fChargeSpreadY(0),
fSigmaIntegration(0),
fAlphaFeedback(0),
fEIonisation(0),
fMaxAdc(0),
fWireSag(0),
fVoltage(0)
{//defines the default parameters
  Size                 (132.6*kcm,26*kcm,136.7*kcm);  //full length, not GEANT half notation
  AngleRot             (-60);                         //rotation of the whole RICH around Z, deg
  Angles               (20,19.5);                     //XY angle, YZ angle  deg  
  Offset               (490*kcm+1.267*kcm);           //1.267???????cm distance from IP to the center of module 
  ProximityGapThickness(0.4*kcm);            
  QuartzLength         (133*kcm);            
  QuartzWidth          (127.9*kcm);          
  OuterFreonLength     (133*kcm);            
  OuterFreonWidth      (41.3*kcm);           
  InnerFreonLength     (133*kcm);            
  InnerFreonWidth      (41.3*kcm);           
  
  ChargeSlope(27.);
  ChargeSpreadX(0.18);ChargeSpreadY(0.18);
  SigmaIntegration(5.);
  MaxAdc(4096);
  AlphaFeedback(0.036);
  EIonisation(26.e-9);
  WireSag(1);		      // 1->On, 0->Off
  Voltage(2150);	      // Should only be 2000, 2050, 2100 or 2150  
}//AliRICHParam::named ctor 
//__________________________________________________________________________________________________
Int_t AliRICHParam::Local2Sector(Float_t &x, Float_t &y)
{//Determines sector for a given hit (x,y) and trasform this point to the local system of that sector.
  Int_t sector=kBad;  
  Float_t x1=-0.5*PcSizeX();      Float_t x2=-0.5*SectorSizeX()-DeadZone();  Float_t x3=-0.5*SectorSizeX();
  Float_t x4= 0.5*SectorSizeX();  Float_t x5= 0.5*SectorSizeX()+DeadZone();  Float_t x6= 0.5*PcSizeX();

  if     (x>=x1&&x<=x2)    {sector=1;x+=0.5*PcSizeX();}
  else if(x>=x3&&x<=x4)    {sector=2;x+=0.5*SectorSizeX();}
  else if(x>=x5&&x<=x6)    {sector=3;x-=0.5*SectorSizeX()+DeadZone();}
  else if(x< x1||x> x6)    {return kBad;}
  else                                                        {return kBad;} //in dead zone

  if     (y>=-0.5*PcSizeY()   &&y<=-0.5*DeadZone())  {y+=0.5*PcSizeY();  return -sector;}
  else if(y> -0.5*DeadZone()  &&y<  0.5*DeadZone())  {return kBad;} //in dead zone
  else if(y>= 0.5*DeadZone()  &&y<= 0.5*PcSizeY())   {y-=0.5*DeadZone(); return  sector;}
  else                                               {return kBad;}
}//Int_t AliRICHParam::Local2Sector(Float_t x, Float_t y)
//__________________________________________________________________________________________________
Int_t AliRICHParam::Pad2Sector(Int_t &padx, Int_t &pady)
{//Determines sector for a given pad (padx,pady) and trasform this point to the local system of that sector.
  Int_t sector=kBad;      
  if     (padx>=1            &&padx<=NpadsXsec())      {sector=1;}
  else if(padx> NpadsXsec()  &&padx<=NpadsXsec()*2)    {sector=2;padx-=NpadsXsec();}
  else if(padx> NpadsXsec()*2&&padx<=NpadsX())         {sector=3;padx-=NpadsXsec()*2;}
  else                                                 {return kBad;}

  if     (pady>=1         &&pady<= NpadsYsec())     {return -sector;}
  else if(pady>NpadsYsec()&&pady<= NpadsY())        {pady-=NpadsYsec();return sector;} 
  else                                              {return kBad;}
}//Local2Sector()
//__________________________________________________________________________________________________
Int_t AliRICHParam::Local2Pad(Float_t x, Float_t y, Int_t &padx, Int_t &pady)
{//returns pad numbers (iPadX,iPadY) for given point in local coordinates (x,y) 
 //count starts in lower left corner from 1,1 to 144,180
  
  padx=pady=kBad;
  Int_t sector=Local2Sector(x,y);
  if(sector==kBad) return sector;
  
  padx=Int_t(x/PadSizeX())+1; 
  if(padx>NpadsXsec())            padx= NpadsXsec();
  if(sector==2||sector==-2)       padx+=NpadsXsec();
  else if(sector==3||sector==-3)  padx+=NpadsXsec()*2;
  
  pady=Int_t(y/PadSizeY())+1;
  if(pady>NpadsYsec())            padx= NpadsYsec();
  if(sector>0)                    pady+=NpadsYsec();    

  return sector;
}//Local2Pad()
//__________________________________________________________________________________________________
void AliRICHParam::Pad2Local(Int_t padx,Int_t pady,Float_t &x,Float_t &y)
{
  Int_t sector=Pad2Sector(padx,pady);  
  if(sector>0)
    y=0.5*DeadZone()+pady*PadSizeY()-0.5*PadSizeY();
  else{
    sector=-sector;
    y=-0.5*PcSizeY()+pady*PadSizeY()-0.5*PadSizeY();
  }
  if(sector==1)
    x=-0.5*PcSizeX()+padx*PadSizeX()-0.5*PadSizeX();
  else if(sector==2)
    x=-0.5*SectorSizeX()+padx*PadSizeX()-0.5*PadSizeX();
  else
    x= 0.5*SectorSizeX()+DeadZone()+padx*PadSizeX()-0.5*PadSizeX();
  return;
}//Pad2Local()
//__________________________________________________________________________________________________
Float_t AliRICHParam::Gain(Float_t y)
{//Calculates the gain
  if(fWireSag){
    Float_t gainK=9e-6*TMath::Power(y,4)+2e-7*TMath::Power(y,3)-0.0316*TMath::Power(y,2)-3e-4*y+25.367;
    Float_t gain = (ChargeSlope()+ChargeSlope()*gainK/100)*0.9;
    return -gain*TMath::Log(gRandom->Rndm());
  }else     
    return -ChargeSlope()*TMath::Log(gRandom->Rndm());
}//Float_t AliRICHParam::IntPH(Float_t yhit)
//__________________________________________________________________________________________________
Float_t AliRICHParam::TotalCharge(Int_t iPID,Float_t eloss,Float_t y)
{//Get number of electrons and return charge
    
  if(iPID>50000)//it's photon no more then 1 electron after photoelectron conversion
    return Gain(y);
  else{  
    Int_t iNelectrons=Int_t(eloss/fEIonisation);if(iNelectrons==0) iNelectrons=1;
    Float_t charge=0;
    for(Int_t i=1;i<=iNelectrons;i++)
      charge+=Gain(y);
    return charge;
  }
}//Float_t AliRICHParam::TotalCharge(Int_t iPID,Float_t eloss, Float_t y)
//__________________________________________________________________________________________________
void AliRICHParam::FirstPad(Float_t x,Float_t y)
{
  Int_t padx,pady;
  Local2Pad(x,y,padx,pady);
}//void AliRICHParam::FirstPad(Float_t x,Float_t y)
//__________________________________________________________________________________________________
Float_t AliRICHParam::AssignChargeToPad(Float_t hitx,Float_t hity,Int_t padx,Int_t pady)
{//
  Float_t padXcenter=0,padYcenter=0;
  Pad2Local(padx,pady,padXcenter,padYcenter);
  
  Float_t xi1=hitx-padXcenter-PadSizeX()/2;
  Float_t xi2=hitx-padXcenter+PadSizeX()/2; 
  Float_t yi1=hity-padYcenter-PadSizeY()/2;
  Float_t yi2=hity-padYcenter+PadSizeY()/2;
  xi1/=AnodeCathodeGap();
  xi2/=AnodeCathodeGap();
  yi1/=AnodeCathodeGap();
  yi2/=AnodeCathodeGap();
// The Mathieson function 
  Double_t ux1=SqrtKx3()*TMath::TanH(Kx2()*xi1);
  Double_t ux2=SqrtKx3()*TMath::TanH(Kx2()*xi2);    
  Double_t uy1=SqrtKy3()*TMath::TanH(Ky2()*yi1);
  Double_t uy2=SqrtKy3()*TMath::TanH(Ky2()*yi2);
  return 4.*Kx4()*(TMath::ATan(ux2)-TMath::ATan(ux1))*Ky4()*(TMath::ATan(uy2)-TMath::ATan(uy1));
}//AssignChargeToPad()
//__________________________________________________________________________________________________
