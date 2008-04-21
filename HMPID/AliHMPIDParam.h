#ifndef AliHMPIDParam_h
#define AliHMPIDParam_h
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include <TMath.h>
#include <TNamed.h>        //base class
#include <TGeoManager.h>   //Instance()
#include <TGeoMatrix.h>   //Instance()
#include <TVector3.h>      //Lors2Mars() Mars2Lors()
 
// Class providing all the needed parametrised information
// to construct the geometry, to define segmentation and to provide response model
// In future will also provide all the staff needed for alignment and calibration

class AliHMPIDParam :public TNamed  
{
public:
//ctor&dtor    
  virtual        ~AliHMPIDParam() {if (fgInstance){for(Int_t i=0;i<7;i++){delete fM[i];fM[i] = 0x0;};fgInstance=0;}}
  
  void     Print(Option_t *opt="") const;                                         //print current parametrization
         
  static inline AliHMPIDParam* Instance();                                //pointer to AliHMPIDParam singleton
  static inline AliHMPIDParam* InstanceNoGeo();                           //pointer to AliHMPIDParam singleton without geometry.root for MOOD, displays, ...
//geo info
  enum EChamberData{kMinCh=0,kMaxCh=6,kMinPc=0,kMaxPc=5};      //Segmenation
  enum EPadxData{kPadPcX=80,kMinPx=0,kMaxPx=79,kMaxPcx=159};   //Segmentation structure along x
  enum EPadyData{kPadPcY=48,kMinPy=0,kMaxPy=47,kMaxPcy=143};   //Segmentation structure along y 

  static Float_t SizePadX    (                               )     {return fgCellX;                                  }  //pad size x, [cm]  
  static Float_t SizePadY    (                               )     {return fgCellY;                                  }  //pad size y, [cm]  

  static Float_t SizePcX    (                                )     {return fgPcX;                                    }  // PC size x
  static Float_t SizePcY    (                                )     {return fgPcY;                                    }  // PC size y
  static Float_t MaxPcX      (Int_t iPc                      )     {return fgkMaxPcX[iPc];                           }  // PC limits
  static Float_t MaxPcY      (Int_t iPc                      )     {return fgkMaxPcY[iPc];                           }  // PC limits
  static Float_t MinPcX      (Int_t iPc                      )     {return fgkMinPcX[iPc];                           }  // PC limits
  static Float_t MinPcY      (Int_t iPc                      )     {return fgkMinPcY[iPc];                           }  // PC limits
  static Int_t   Nsig        (                               )     {return fgSigmas;                                 }  //Getter n. sigmas for noise
  static Float_t SizeAllX    (                               )     {return fgAllX;                                   }  //all PCs size x, [cm]        
  static Float_t SizeAllY    (                               )     {return fgAllY;                                   }  //all PCs size y, [cm]    

  static Float_t LorsX       (Int_t pc,Int_t padx             )     {return (padx    +0.5)*SizePadX()+fgkMinPcX[pc]; }  //center of the pad x, [cm]

  static Float_t LorsY       (Int_t pc,Int_t pady            )     {return (pady    +0.5)*SizePadY()+fgkMinPcY[pc];  }  //center of the pad y, [cm]

  inline static void   Lors2Pad(Float_t x,Float_t y,Int_t &pc,Int_t &px,Int_t &py);                                     //(x,y)->(pc,px,py) 

  static Int_t   Abs         (Int_t ch,Int_t pc,Int_t x,Int_t y)   {return ch*100000000+pc*1000000+x*1000+y;         }  //(ch,pc,padx,pady)-> abs pad
  static Int_t   DDL2C       (Int_t ddl                      )     {return ddl/2;                                    }  //ddl -> chamber
  static Int_t   A2C         (Int_t pad                      )     {return pad/100000000;                            }  //abs pad -> chamber
  static Int_t   A2P         (Int_t pad                      )     {return pad%100000000/1000000;                    }  //abs pad -> pc 
  static Int_t   A2X         (Int_t pad                      )     {return pad%1000000/1000;                         }  //abs pad -> pad X 
  static Int_t   A2Y         (Int_t pad                      )     {return pad%1000;                                 }  //abs pad -> pad Y 

  static Bool_t  IsOverTh    (Float_t q                      )     {return q >= fgSigmas;                            }  //is digit over threshold?
  
  Double_t GetRefIdx         (                               )const{return fRadNmean;                                }  //refractive index of freon
  Bool_t  GetInstType        (                               )const{return fgInstanceType;                            }  //return if the instance is from geom or ideal                        
  
  inline static Bool_t IsInDead(Float_t x,Float_t y        );                                                           //is the point in dead area?
  inline static Int_t  InHVSector(           Float_t y     );                                                           //find HV sector
  static Bool_t  IsInside    (Float_t x,Float_t y,Float_t d=0)     {return  x>-d&&y>-d&&x<fgkMaxPcX[kMaxPc]+d&&y<fgkMaxPcY[kMaxPc]+d; } //is point inside chamber boundaries?

            Double_t   MeanIdxRad              ()const {return 1.29204;}   //<--TEMPORAR--> to be removed in future. Mean ref index C6F14
            Double_t   MeanIdxWin              ()const {return 1.57819;}   //<--TEMPORAR--> to be removed in future. Mean ref index quartz
            Float_t    DistCut                 ()const {return 1.0;}       //<--TEMPORAR--> to be removed in future. Cut for MIP-TRACK residual 
            Float_t    QCut                    ()const {return 100;}       //<--TEMPORAR--> to be removed in future. Separation PHOTON-MIP charge 
            Float_t    MultCut                 ()const {return 200;}       //<--TEMPORAR--> to be removed in future. Multiplicity cut to activate WEIGHT procedure 

            Double_t   RadThick                ()const {return 1.5;}       //<--TEMPORAR--> to be removed in future. Radiator thickness
            Double_t   WinThick                ()const {return 0.5;}       //<--TEMPORAR--> to be removed in future. Window thickness
            Double_t   GapThick                ()const {return 8.0;}       //<--TEMPORAR--> to be removed in future. Proximity gap thickness
            Double_t   WinIdx                  ()const {return 1.5787;}    //<--TEMPORAR--> to be removed in future. Mean refractive index of WIN material (SiO2) 
            Double_t   GapIdx                  ()const {return 1.0005;}    //<--TEMPORAR--> to be removed in future. Mean refractive index of GAP material (CH4)

  static        Int_t      Stack(Int_t evt=-1,Int_t tid=-1);              //Print stack info for event and tid
  static        Int_t      StackCount(Int_t pid,Int_t evt);               //Counts stack particles of given sort in given event  
  static        void       IdealPosition(Int_t iCh,TGeoHMatrix *m);       //ideal position of given chamber 
  //trasformation methodes
  void     Lors2Mars   (Int_t c,Float_t x,Float_t y,Double_t *m,Int_t pl=kPc)const{Double_t z=0; switch(pl){case kPc:z=8.0;break; case kAnod:z=7.806;break; case kRad:z=-1.25; break;}   Double_t l[3]={x-fX,y-fY,z};  fM[c]->LocalToMaster(l,m); }    
  TVector3 Lors2Mars   (Int_t c,Float_t x,Float_t y,            Int_t pl=kPc)const{Double_t m[3];Lors2Mars(c,x,y,m,pl); return TVector3(m);    }//MRS->LRS  
  void     Mars2Lors   (Int_t c,Double_t *m,Float_t &x ,Float_t &y          )const{Double_t l[3];fM[c]->MasterToLocal(m,l);x=l[0]+fX;y=l[1]+fY;}//MRS->LRS
  void     Mars2LorsVec(Int_t c,Double_t *m,Float_t &th,Float_t &ph         )const{Double_t l[3]; fM[c]->MasterToLocalVect(m,l); 
                                                                                   Float_t pt=TMath::Sqrt(l[0]*l[0]+l[1]*l[1]); 
                                                                                           th=TMath::ATan(pt/l[2]); 
                                                                                           ph=TMath::ATan2(l[1],l[0]);}    
  TVector3 Norm        (Int_t c                                             )const{Double_t n[3]; Norm(c,n); return TVector3(n);               }//norm 
  void     Norm        (Int_t c,Double_t *n                                 )const{Double_t l[3]={0,0,1};fM[c]->LocalToMasterVect(l,n);        }//norm
  void     Point       (Int_t c,Double_t *p,Int_t plane                     )const{Lors2Mars(c,0,0,p,plane);}      //point of given chamber plane

  void     SetRefIdx      (Double_t refRadIdx                                  ) {fRadNmean = refRadIdx;}             //set refractive index of freon
  void     SetSigmas      (Int_t sigmas                                        ) {fgSigmas = sigmas;}                 //set sigma cut    
  void     SetInstanceType(Bool_t inst                                         ) {fgInstanceType = inst;}             //kTRUE if from geomatry kFALSE if from ideal geometry
  //For PID
  Double_t SigLoc      (Double_t trkTheta,Double_t trkPhi,Double_t ckovTh,Double_t ckovPh,Double_t beta);//error due to cathode segmetation
  Double_t SigGeom     (Double_t trkTheta,Double_t trkPhi,Double_t ckovTh,Double_t ckovPh,Double_t beta);//error due to unknown photon origin
  Double_t SigCrom     (Double_t trkTheta,Double_t trkPhi,Double_t ckovTh,Double_t ckovPh,Double_t beta);//error due to unknonw photon energy
  Double_t Sigma2      (Double_t trkTheta,Double_t trkPhi,Double_t ckovTh,Double_t ckovPh              );//photon candidate sigma^2

  //Mathieson Getters
  
  static Double_t PitchAnodeCathode()  {return fgkD;}
  static Double_t SqrtK3x() {return fgkSqrtK3x;}
  static Double_t K2x    () {return fgkK2x;}
  static Double_t K1x    () {return fgkK1x;}
  static Double_t K4x    () {return fgkK4x;}
  static Double_t SqrtK3y() {return fgkSqrtK3y;}
  static Double_t K2y    () {return fgkK2y;}
  static Double_t K1y    () {return fgkK1y;}
  static Double_t K4y    () {return fgkK4y;}
  //
  enum EPlaneId {kPc,kRad,kAnod};            //3 planes in chamber 
  enum ETrackingFlags {kMipDistCut=-9,kMipQdcCut=-5,kNoPhotAccept=-11};     //flags for Reconstruction

protected:
  static /*const*/ Float_t fgkMinPcX[6];                                                           //limits PC
  static /*const*/ Float_t fgkMinPcY[6];                                                           //limits PC
  static /*const*/ Float_t fgkMaxPcX[6];                                                           //limits PC
  static /*const*/ Float_t fgkMaxPcY[6]; 
  
// Mathieson constants
// For HMPID --> x direction means parallel      to the wires: K3 = 0.66  (NIM A270 (1988) 602-603) fig.1  
// For HMPID --> y direction means perpendicular to the wires: K3 = 0.90  (NIM A270 (1988) 602-603) fig.2  
//

  static const Double_t fgkD;  // ANODE-CATHODE distance 0.445/2
  
  static const Double_t fgkSqrtK3x,fgkK2x,fgkK1x,fgkK4x;
  static const Double_t fgkSqrtK3y,fgkK2y,fgkK1y,fgkK4y;
//
    
  static Int_t    fgSigmas;                                                                        //sigma Cut
  static Bool_t   fgInstanceType;                                                                  //kTRUE if from geomatry kFALSE if from ideal geometry

  static Float_t fgCellX, fgCellY, fgPcX, fgPcY, fgAllX, fgAllY;                                   //definition of HMPID geometric parameters 
         AliHMPIDParam(Bool_t noGeo);             //default ctor is protected to enforce it to be singleton

  static AliHMPIDParam *fgInstance;   //static pointer  to instance of AliHMPIDParam singleton

  TGeoHMatrix *fM[7];                 //pointers to matrices defining HMPID chambers rotations-translations
  Float_t fX;                         //x shift of LORS with respect to rotated MARS 
  Float_t fY;                         //y shift of LORS with respect to rotated MARS   
  Double_t fRadNmean;                 //C6F14 mean index as a running parameter
  
private:
  AliHMPIDParam(const AliHMPIDParam& r);              //dummy copy constructor
  AliHMPIDParam &operator=(const AliHMPIDParam& r);   //dummy assignment operator
      
  ClassDef(AliHMPIDParam,0)           //HMPID main parameters class
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AliHMPIDParam* AliHMPIDParam::Instance()
{
// Return pointer to the AliHMPIDParam singleton. 
// Arguments: none
//   Returns: pointer to the instance of AliHMPIDParam or 0 if no geometry       
  if(!fgInstance) new AliHMPIDParam(kFALSE);                                //default setting for reconstruction, if no geometry.root -> AliFatal
  return fgInstance;  
}//Instance()    
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AliHMPIDParam* AliHMPIDParam::InstanceNoGeo()
{
// Return pointer to the AliHMPIDParam singleton without the geometry.root. 
// Arguments: none
//   Returns: pointer to the instance of AliHMPIDParam or 0 if no geometry       
  if(!fgInstance) new AliHMPIDParam(kTRUE);                               //to avoid AliFatal, for MOOD and displays, use ideal geometry parameters
  return fgInstance;  
}//Instance()    
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Bool_t AliHMPIDParam::IsInDead(Float_t x,Float_t y)
{
// Check is the current point is outside of sensitive area or in dead zones
// Arguments: x,y -position
//   Returns: 1 if not in sensitive zone           
  for(Int_t iPc=0;iPc<6;iPc++)
    if(x>=fgkMinPcX[iPc] && x<=fgkMaxPcX[iPc] && y>=fgkMinPcY[iPc] && y<=fgkMaxPcY [iPc]) return kFALSE; //in current pc
  
  return kTRUE;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDParam::Lors2Pad(Float_t x,Float_t y,Int_t &pc,Int_t &px,Int_t &py)
{
// Check the pad of given position
// Arguments: x,y- position [cm] in LORS; pc,px,py- pad where to store the result
//   Returns: none
  pc=px=py=-1;
  if     (x>fgkMinPcX[0] && x<fgkMaxPcX[0]) {pc=0; px=Int_t( x               / SizePadX());}//PC 0 or 2 or 4
  else if(x>fgkMinPcX[1] && x<fgkMaxPcX[1]) {pc=1; px=Int_t((x-fgkMinPcX[1]) / SizePadX());}//PC 1 or 3 or 5
  else return;
  if     (y>fgkMinPcY[0] && y<fgkMaxPcY[0]) {      py=Int_t( y               / SizePadY());}//PC 0 or 1
  else if(y>fgkMinPcY[2] && y<fgkMaxPcY[2]) {pc+=2;py=Int_t((y-fgkMinPcY[2]) / SizePadY());}//PC 2 or 3
  else if(y>fgkMinPcY[4] && y<fgkMaxPcY[4]) {pc+=4;py=Int_t((y-fgkMinPcY[4]) / SizePadY());}//PC 4 or 5
  else return;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Int_t AliHMPIDParam::InHVSector(Float_t y)
{
//Calculate the HV sector corresponding to the cluster position
//Arguments: y
//Returns the HV sector in the single module
 
   Int_t hvsec = -1;
   Int_t pc,px,py;
   Lors2Pad(1.,y,pc,px,py);
   if(py==-1) return hvsec;
   
   hvsec = (py+(pc/2)*(kMaxPy+1))/((kMaxPy+1)/2);
   
   return hvsec;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#endif
