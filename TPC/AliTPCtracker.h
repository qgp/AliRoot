#ifndef ALITPCTRACKER_H
#define ALITPCTRACKER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//-------------------------------------------------------
//                       TPC tracker
//
//   Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//-------------------------------------------------------
#include "AliTracker.h"
#include "AliConfig.h"
#include "AliTPCtrack.h"
#include "AliTPCClustersArray.h"

#include "AliTPCreco.h"

class TFile;
class AliTPCParam;
class AliRunLoader;
class AliTPCcluster;

class AliTPCtracker : public AliTracker {
public:
   AliTPCtracker():AliTracker(),fkNIS(0),fkNOS(0) {
      fInnerSec=fOuterSec=0; fSeeds=0; 
   }
   AliTPCtracker(const AliTPCParam *par, Int_t eventn=0, 
                 const char* evfoldname  = AliConfig::fgkDefaultEventFolderName);
  ~AliTPCtracker();

   Int_t ReadSeeds(const TFile *in);

   void LoadInnerSectors();
   void UnloadInnerSectors();
   void LoadOuterSectors();
   void UnloadOuterSectors();

   AliCluster *GetCluster(Int_t index) const;
   Int_t Clusters2Tracks();
   Int_t PropagateBack();

   virtual void  CookLabel(AliKalmanTrack *t,Float_t wrong) const; 
   Int_t LoadClusters(){return 0;}
   void UnloadClusters(){}

public:
//**************** Internal tracker class ********************** 
   class AliTPCRow {
   public:
     AliTPCRow() {fN=0;}
     void InsertCluster(const AliTPCcluster *c, UInt_t index);
     operator int() const {return fN;}
     const AliTPCcluster* operator[](Int_t i) const {return fClusters[i];}
     UInt_t GetIndex(Int_t i) const {return fIndex[i];}
     Int_t Find(Double_t y) const; 
     void SetX(Double_t x) {fX=x;}
     Double_t GetX() const {return fX;}

   private:
     Int_t fN;                                          //number of clusters 
     const AliTPCcluster *fClusters[kMaxClusterPerRow]; //pointers to clusters
     UInt_t fIndex[kMaxClusterPerRow];                  //indeces of clusters
     Double_t fX;                                 //X-coordinate of this row

   private:
     AliTPCRow(const AliTPCRow& r);            //dummy copy constructor
     AliTPCRow &operator=(const AliTPCRow& r); //dummy assignment operator
   };

//**************** Internal tracker class ********************** 
   class AliTPCSector {
   public:
     AliTPCSector() { fN=0; fRow = 0;}
    ~AliTPCSector() { delete[] fRow; }
     AliTPCRow& operator[](Int_t i) const { return *(fRow+i); }
     Int_t GetNRows() const { return fN; }
     void Setup(const AliTPCParam *par, Int_t flag);
     Double_t GetX(Int_t l) const {return fRow[l].GetX();}
     Double_t GetMaxY(Int_t l) const {
         return GetX(l)*TMath::Tan(0.5*GetAlpha());
     } 
     Double_t GetAlpha() const {return fAlpha;}
     Double_t GetAlphaShift() const {return fAlphaShift;}
     Int_t GetRowNumber(Double_t x) const {
        //return pad row number for this x
       Double_t r;
       if (fN < 64){
	 r=fRow[fN-1].GetX();
	 if (x > r) return fN;
	 r=fRow[0].GetX();
	 if (x < r) return -1;
	 return Int_t((x-r)/f1PadPitchLength + 0.5);}
       else{	
	   r=fRow[fN-1].GetX();
	   if (x > r) return fN;
	   r=fRow[0].GetX();
	   if (x < r) return -1;
	  Double_t r1=fRow[64].GetX();
	  if(x<r1){	  
	    return Int_t((x-r)/f1PadPitchLength + 0.5);}
	  else{
	    return (Int_t((x-r1)/f2PadPitchLength + 0.5)+64);} 
       }
     }
     Double_t GetPadPitchWidth()  const {return fPadPitchWidth;}

   private:
     Int_t fN;                        //number of pad rows 
     AliTPCRow *fRow;                    //array of pad rows
     Double_t fAlpha;                    //opening angle
     Double_t fAlphaShift;               //shift angle;
     Double_t fPadPitchWidth;            //pad pitch width
     Double_t f1PadPitchLength;           //pad pitch length
     Double_t f2PadPitchLength;            //pad pitch length 
   private:
     AliTPCSector(const AliTPCSector &s);           //dummy copy contructor
     AliTPCSector& operator=(const AliTPCSector &s);//dummy assignment operator
   };

//**************** Internal tracker class ********************** 
   class AliTPCseed : public AliTPCtrack {
   public:
     AliTPCseed():AliTPCtrack(){}
     AliTPCseed(const AliTPCtrack &t):AliTPCtrack(t){}
     AliTPCseed(const AliKalmanTrack &t, Double_t a):AliTPCtrack(t,a){}
     AliTPCseed(UInt_t index, const Double_t xx[5], 
                const Double_t cc[15], Double_t xr, Double_t alpha): 
                AliTPCtrack(index, xx, cc, xr, alpha) {}
     void SetSampledEdx(Float_t q, Int_t i) {
        Double_t s=GetSnp(), t=GetTgl();
        q *= TMath::Sqrt((1-s*s)/(1+t*t));
        fdEdxSample[i]=q;
     }
     void CookdEdx(Double_t low=0.05, Double_t up=0.70);

   private:
     Float_t fdEdxSample[200]; //array of dE/dx samples 
   };

private:
   void MakeSeeds(Int_t i1, Int_t i2);
   Int_t FollowProlongation(AliTPCseed& t, Int_t rf=0);
   Int_t FollowBackProlongation(AliTPCseed &s, const AliTPCtrack &t);

   AliTPCtracker(const AliTPCtracker& r);           //dummy copy constructor
   AliTPCtracker &operator=(const AliTPCtracker& r);//dummy assignment operator

   const Int_t fkNIS;        //number of inner sectors
   AliTPCSector *fInnerSec;  //array of inner sectors;
   const Int_t fkNOS;        //number of outer sectors
   AliTPCSector *fOuterSec;  //array of outer sectors;

   Int_t fN;               //number of loaded sectors
   AliTPCSector *fSectors; //pointer to loaded sectors;

   Int_t fEventN;                      //event number
   AliTPCClustersArray fClustersArray; //array of TPC clusters
   TObjArray *fSeeds;                  //array of track seeds
   TString fEvFolderName;//! name of data folder
};

#endif


