#ifndef AliRICHHelix_h
#define AliRICHHelix_h

#include <TObject.h>
#include <TVector3.h>
#include "AliRICHParam.h"
#include "AliRICHChamber.h"

class AliRICHHelix: public TObject
{
public:
           AliRICHHelix()                                              {;}
           AliRICHHelix(TVector3 x0,TVector3 p0,Int_t q,Double_t b=0.4)  {fX0=x0;fP0=p0;fQ=q;fBz=b;} //takes position and momentum at parametrised point
  virtual ~AliRICHHelix()                                              {;}        
  inline void   Propagate(Double_t lenght);
         Bool_t Intersection(TVector3 plane)          {return Intersection(plane,plane.Unit());} // special plane given by point only
  inline Bool_t Intersection(TVector3 planePoint,TVector3 planeNorm); //intersection with plane given by point and normal vector
  inline Int_t  RichIntersect(AliRICHParam *pParam);
  TVector3 X()const {return fX;}
  TVector3 P()const {return fP;}
  TVector3 X0()const {return fX0;}
  TVector3 P0()const {return fP0;}
  TVector2 PosPc() const {return fPosPc;}           //returns position of intersection with PC
  TVector2 PosRad()const {return fPosRad;}          //returns position of intersection with radiator 
  TVector3 Ploc()const   {return fPloc;}            //returns momentum at the position of intersection with radiator 
  
         void   Print(Option_t *sOption)const; //virtual interface from TObject
protected:
  TVector3 fX0;            //helix position in parametrised point, cm    in MRS
  TVector3 fP0;            //helix momentum in parametrised point, GeV/c in MRS
  TVector3 fX;             //helix position in point of interest,  cm    in MRS
  TVector3 fP;             //helix momentum in point of interest,  GeV/c in MRS
  Double_t fLength;        //helix length in point of interest    
  Int_t    fQ;             //sign of track charge (value not provided by current ESD)  
  Double_t fBz;            //magnetic field along z value in Tesla under assumption of uniformity 
  TVector2 fPosPc;         //position on PC in local system
  TVector2 fPosRad;        //position on radiator in local system 
  TVector3 fPloc;          //momentum in local system
  ClassDef(AliRICHHelix,0) //General helix
};//class AliRICHHelix
//__________________________________________________________________________________________________
void AliRICHHelix::Propagate(Double_t length)
{
// Propogates the helix to the position of interest defined by helix length s  
// Assumes uniform magnetic field along z direction.  
  const Double_t c = 0.00299792458;//this value provides that coordinates are in cm momentum in GeV/c
  Double_t a = -c*fBz*fQ;
 
  Double_t rho = a/fP0.Mag();
  fX.SetX( fX0.X()+fP0.X()*TMath::Sin(rho*length)/a-fP0.Y()*(1-TMath::Cos(rho*length))/a  );
  fX.SetY( fX0.Y()+fP0.Y()*TMath::Sin(rho*length)/a+fP0.X()*(1-TMath::Cos(rho*length))/a  ); 
  fX.SetZ( fX0.Z()+fP0.Z()*length/fP0.Mag()                                               );
  fP.SetX( fP0.X()*TMath::Cos(rho*length)-fP0.Y()*TMath::Sin(rho*length)                  );
  fP.SetY( fP0.Y()*TMath::Cos(rho*length)+fP0.X()*TMath::Sin(rho*length)                  );
  fP.SetZ( fP0.Z()                                                                        );
  fLength=length;
}
//__________________________________________________________________________________________________
Bool_t AliRICHHelix::Intersection(TVector3 planePoint,TVector3 planeNorm)
{
// Finds point of intersection (if exists) of the helix to the plane given by point and normal vector.
// Returns kTrue if helix intersects the plane, kFALSE otherwise.
// Stores result in current helix fields fX and fP.   
  
  Double_t s=(planePoint-fX0)*planeNorm,dist=99999,distPrev=dist;

  while(TMath::Abs(dist)>0.0001){
    Propagate(s);                        //calculates helix at the distance s from x0 ALONG the helix
    dist=(fX-planePoint)*planeNorm;      //distance between current helix position and plane
    if(TMath::Abs(dist) >= TMath::Abs(distPrev)) { return kFALSE;}
    distPrev=dist;
    s-=dist;
  }
  return kTRUE;
}
//__________________________________________________________________________________________________
Int_t AliRICHHelix::RichIntersect(AliRICHParam *pParam)
{
// Searchs for intersection of this helix with all RICH chambers, returns chamber number or 0 if no intersection
// On exit fPosRad contain position of intersection in Local System with radiator     
//         fPosPc  contains the same for photocathode  
  for(Int_t iChamberN=1;iChamberN<=kNchambers;iChamberN++){//chamber loop
    if(Intersection(pParam->C(iChamberN)->Rad())){//there is intersection with radiator plane
      fPosRad=pParam->C(iChamberN)->Mrs2Rad(fX);//position on radiator plane
      if(pParam->IsAccepted(fPosRad)){//intersection within radiator (even if in dead zone)
        if(Intersection(pParam->C(iChamberN)->Pc())){//there is intersection with photocathode
          fPosPc=pParam->C(iChamberN)->Mrs2Pc(fX);//position on photcathode plane
          if(pParam->IsAccepted(fPosPc)){//intersection within pc (even if in dead zone)
            fPloc=pParam->C(iChamberN)->PMrs2Loc(fP);
            return iChamberN;
          }//if inside PC
        }//if for PC
      }//if inside radiator
    }//if for radiator       
  }//chamber loop
  return 0;
}
//__________________________________________________________________________________________________    
#endif
