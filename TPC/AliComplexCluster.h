#ifndef ALICOMPLEXCLUSTER_H
#define ALICOMPLEXCLUSTER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include "TObject.h"
#include "TMath.h"

class AliComplexCluster : public TObject {
public:
  Int_t     fTracks[3];//labels of overlapped tracks
  Float_t   fX ;       //Y of cluster
  Float_t   fY ;       //Z of cluster
  Float_t   fQ ;       //Q of cluster (in ADC counts)
  Float_t   fSigmaX2;  //Sigma Y square of cluster
  Float_t   fSigmaY2;  //Sigma Z square of cluster
  Float_t   fSigmaXY;  //      XY moment 
  Float_t   fArea;     //area of cluster
  Float_t   fMax;     //amplitude at maximum 
public:
  AliComplexCluster() {
    fTracks[0]=fTracks[1]=fTracks[2]=0; 
    fX=fY=fQ=fSigmaX2=fSigmaY2=fSigmaXY=fArea=fMax=0.;
  }
  virtual ~AliComplexCluster() {;}
  Bool_t    IsSortable() const;
  Int_t Compare(const TObject *o) const;
  ClassDef(AliComplexCluster,1)  // Cluster manager
};

class AliDigitCluster : public AliComplexCluster {
public:
  Int_t fNx; //number of accepted x bins
  Int_t fNy; //number of accepted y bins
  Float_t fMaxX; //maximum x bin
  Float_t fMaxY; //maximum y bin
public:  
  ClassDef(AliDigitCluster,1)  // Tclusters
};



class AliTPCTrackerPoint  {  
 private:
  Short_t   fTX;        // x position of the cluster  in cm - 10 mum prec
  Short_t   fTZ;        // current prolongation in Z  in cm - 10 mum prec.
  Short_t   fTY;        // current prolongation in Y  in cm - 10 mum prec.
  Char_t   fTAngleZ;   // angle 
  Char_t   fTAngleY;   // angle 
  UChar_t  fSigmaZ;   // shape  Z - normalised shape - normaliziation 1 - precision 2 percent
  UChar_t  fSigmaY;   // shape  Y - normalised shape - normaliziation 1 - precision 2 percent
 public:
  AliTPCTrackerPoint(){fTX=0; fTY=0; fTZ=0; fTAngleZ=0; fTAngleY=0;}
  inline Float_t  GetX() {return (fTX*0.01);}
  inline Float_t  GetZ() {return (fTZ*0.01);}
  inline Float_t  GetY() {return (fTY*0.01);}
  inline Float_t  GetAngleZ() {return (Float_t(fTAngleZ)*0.02);}
  inline Float_t  GetAngleY() {return (Float_t(fTAngleY)*0.02);}
  //
  void     SetX(Float_t x){ fTX = Short_t(TMath::Nint(x*100.));} 
  void     SetY(Float_t y){ fTY = Short_t(TMath::Nint(y*100.));} 
  void     SetZ(Float_t z){ fTZ = Short_t(TMath::Nint(z*100.));} 
  void     SetAngleZ(Float_t anglez) {fTAngleZ = Char_t(TMath::Nint(anglez*50.));}
  void     SetAngleY(Float_t angley) {fTAngleY = Char_t(TMath::Nint(angley*50.));}
  inline Float_t  GetSigmaZ() {return (fSigmaZ*0.02);}
  inline Float_t  GetSigmaY() {return (fSigmaY*0.02);}  
  void     SetSigmaZ(Float_t sigmaz) {fSigmaZ = UChar_t(TMath::Nint(sigmaz*50.));}
  void     SetSigmaY(Float_t sigmay) {fSigmaY = UChar_t(TMath::Nint(sigmay*50.));}
  //
 public:
  ClassDef(AliTPCTrackerPoint,1)  
};

class AliTPCClusterPoint  {
 private:
  Short_t  fCZ;       // current cluster position Z in cm - 100 mum precision
  Short_t  fCY;       // current cluster position Y in cm - 100 mum precision
  UChar_t  fErrZ;     // z error estimate - in  mm - 50 mum precision 
  UChar_t  fErrY;     // y error estimate - in  mm - 50 mum precision 
  UChar_t  fSigmaZ;   // shape  Z - normalised shape - normaliziation 1 - precision 2 percent
  UChar_t  fSigmaY;   // shape  Y - normalised shape - normaliziation 1 - precision 2 percent
  UShort_t fQ;        // total charge in cluster 
  UShort_t fMax;      // charge at maximum  
  Char_t   fCType;    // type of the cluster
 public:
  AliTPCClusterPoint(){fCZ=fCY=fSigmaZ=fSigmaY=fErrZ=fErrY=fQ=fMax=fCType=0;}
  inline Float_t  GetZ()    {return (fCZ*0.01);}
  inline Float_t  GetY()    {return (fCY*0.01);}
  inline Float_t  GetErrZ()   {return (fErrZ*0.005);}
  inline Float_t  GetErrY()   {return (fErrY*0.005);}
  inline Float_t  GetSigmaZ() {return (fSigmaZ*0.02);}
  inline Float_t  GetSigmaY() {return (fSigmaY*0.02);}  
  inline Int_t  GetType()   {return fCType;}
  inline Int_t  GetMax()   {return fMax;}
  inline Float_t  GetQ()   {return fQ;}

  //
  void     SetY(Float_t y){ fCY = Short_t(TMath::Nint(y*100.));} 
  void     SetZ(Float_t z){ fCZ = Short_t(TMath::Nint(z*100.));} 
  void     SetErrZ(Float_t errz) {fErrZ = UChar_t(TMath::Nint(errz*200.));}
  void     SetErrY(Float_t erry) {fErrY = UChar_t(TMath::Nint(erry*200.));}
  void     SetSigmaZ(Float_t sigmaz) {fSigmaZ = UChar_t(TMath::Nint(sigmaz*50.));}
  void     SetSigmaY(Float_t sigmay) {fSigmaY = UChar_t(TMath::Nint(sigmay*50.));}
  void     SetQ(Float_t q) {fQ = UShort_t(q);}
  void     SetMax(Float_t max) {fMax = UShort_t(max);}
  void     SetType(Char_t type) {fCType = type;}

  //
 public:
  ClassDef(AliTPCClusterPoint,1)  
};


class AliTPCExactPoint : public TObject{
 public:
  AliTPCExactPoint(){fEZ=fEY=fEAngleZ=fEAngleY=fEAmp=fEPrim=fTrackID=0;}
  Float_t fEZ;       // current "exact" position according simulation
  Float_t fEY;       // current "exact" position according simulation
  Float_t fEAngleZ;  // angle Z
  Float_t fEAngleY;  // angle Y
  Float_t fEAmp;     // total charge deposited in row
  Float_t fEPrim;    // primary charge deposited in row
  Int_t   fTrackID;  // id of the track
  ClassDef(AliTPCExactPoint,1)  
};


class AliTPCTrackPoint: public TObject{
 public:
  AliTPCTrackPoint(){ fIsShared = kFALSE;}
  // AliTPCClusterPoint & GetCPoint(){return fCPoint;}
  AliTPCTrackerPoint & GetTPoint(){return fTPoint;}
 public:
  //  AliTPCClusterPoint fCPoint;
  AliTPCTrackerPoint fTPoint;
  Bool_t fIsShared;  // indicate sharing of the point between several tracks
  ClassDef(AliTPCTrackPoint,1)  
};


class AliTPCTrackPointRef: public AliTPCTrackPoint{
 public:
  AliTPCExactPoint & GetExactPoint(){return fEPoint;}
  AliTPCExactPoint & GetNearestPoint(){return fNPoint;}  
 public:
  AliTPCExactPoint fEPoint; //exact point belonging to track
  AliTPCExactPoint fNPoint; //nearest point  
  ClassDef(AliTPCTrackPointRef,1)  
};


#endif //ALICOMPLEXCLUSTER_H
