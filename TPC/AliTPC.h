#ifndef ALITPC_H
#define ALITPC_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

////////////////////////////////////////////////
//  Manager class for TPC                     //
////////////////////////////////////////////////
#include "AliDetector.h"
#include "AliHit.h" 
#include "AliDigit.h" 
#include "TLorentzVector.h" 

#include "AliTPCLoader.h"


class TMatrix;
class AliTPCFastMatrix;  //MI change
class AliTPCFastVector;  //MI change

class TTree;

class TFile;
class AliTPCParam;
class AliTPCDigitsArray;
class AliTPCClustersArray;
class AliTPCTrackHitsV2; // M.I.
class AliTPCTrackHits; // M.I.  -MI4 old hits - to be removed later


class AliTPC : public AliDetector {
protected:
  Int_t          fDefaults;
  Int_t          fSens;             // ISENS
  Int_t          fSecAL;            // Upper sector selector
  Int_t          fSecAU;            // Lower sector selector
  Int_t          fSecLows[6];       // List of lower sectors selected
  Int_t          fSecUps[12];       // List of upper sectors selected
  Int_t          fNsectors;         // Number of sectors in TPC
  //MI changes
  AliTPCDigitsArray * fDigitsArray;              //!detector digit object  
  AliTPCClustersArray * fClustersArray; //!detector cluster object
  AliTPCParam *fTPCParam;           // pointer to TPC parameters 
  AliTPCTrackHitsV2 *fTrackHits;      //!hits for given track M.I.
  AliTPCTrackHits *fTrackHitsOld;      //!hits for given track M.I. MIold -

  Int_t  fHitType; // if fNewHit = 1 old data structure if 2 new hits  if 4  old MI stucture
  //  3 both types 
  Int_t fDigitsSwitch; // digits type, 0->normal, 1->summable

  //MK changes

  Float_t        fSide;  // selects left(-1), right(+1), or both(0) sides of the TPC
  Int_t          fNoComp; // number of a drift gas components
  Int_t          fMixtComp[3]; // drift gas components
  Float_t        fMixtProp[3]; // mixture proportions

public:
  AliTPC();
  AliTPC(const char *name, const char *title);
  
  virtual AliLoader* MakeLoader(const char* topfoldername);
  
  virtual      ~AliTPC();
  virtual void  AddHit(Int_t a1, Int_t *a2, Float_t *a3);
  Int_t         DistancetoPrimitive(Int_t px, Int_t py);
  virtual void  BuildGeometry();
  virtual void  CreateGeometry() {}
  virtual void  CreateMaterials();
  virtual void  Hits2Clusters(TFile *of, Int_t eventn=0);
  virtual void  Hits2ExactClustersSector(Int_t isec); // MI change calculate "exact" cluster position
  
  virtual void  SDigits2Digits(){;} //MI change -cycling to production
  virtual void  SDigits2Digits2(Int_t eventnumber=0);

  virtual void  Hits2SDigits(); // MI - cycling around
  virtual void  Hits2SDigits2(Int_t eventnumber=0);

  virtual void  Digits2Reco(Int_t firstevent=0,Int_t lastevent=0);
  virtual void  Hits2Digits(Int_t eventnumber=0);   //MI change
  virtual void  Hits2DigitsSector(Int_t isec);  //MI change
  virtual void  Init();
  virtual Int_t IsVersion() const =0;
  virtual void  Digits2Clusters(Int_t eventnumber=0);
  virtual void  Clusters2Tracks();

  Int_t         GetNsectors()       {return fNsectors;}
  virtual void  MakeBranch(Option_t *opt=" ");
  virtual void  ResetDigits();
  virtual void  SetSecAL(Int_t sec);
  virtual void  SetSecAU(Int_t sec);
  virtual void  SetSecLows(Int_t s1,Int_t s2,Int_t s3,Int_t s4,Int_t s5, Int_t s6);
  virtual void  SetSecUps (Int_t s1,Int_t s2,Int_t s3,Int_t s4,Int_t s5, Int_t s6,
	  Int_t s7,Int_t s8,Int_t s9,Int_t s10, Int_t s11, Int_t s12);
  virtual void  SetSens(Int_t sens);


  virtual void  SetSide(Float_t side);
  virtual void  SetGasMixt(Int_t nc,Int_t c1,Int_t c2,Int_t c3,Float_t p1,
                           Float_t p2,Float_t p3); 

  virtual void  StepManager()=0;
  virtual void  DrawDetector() {}
  AliTPCDigitsArray*  GetDigitsArray() {return fDigitsArray;} //MI change
  AliTPCClustersArray* GetClustersArray(){return fClustersArray;} //MI change
  AliTPCParam *GetParam(){return fTPCParam;} // M.K, M.I changes
  void SetParam(AliTPCParam *param){fTPCParam=param;} // M.K, M.I changes
  void SetDigitsArray(AliTPCDigitsArray* param) {fDigitsArray=param;}  //MI change
  void SetClustersArray(AliTPCClustersArray *clusters) {fClustersArray = clusters;} //MI change

// additional function neccesary for the new hits 
   virtual void MakeBranch2(Option_t *opt=" ", const char *file=0);  //
   virtual void SetTreeAddress();
   virtual void SetTreeAddress2();
   virtual void AddHit2(Int_t a1,  Int_t *a2, Float_t *a3);  //
   virtual void ResetHits();
   virtual void ResetHits2();     
   virtual AliHit* FirstHit(Int_t track);
   virtual AliHit* NextHit();
   virtual AliHit* FirstHit2(Int_t track);
   virtual AliHit* NextHit2();
   virtual void LoadPoints(Int_t dummy);
   virtual void LoadPoints2(Int_t dummy);
   virtual void LoadPoints3(Int_t dumy);
   virtual void FinishPrimary();
   virtual void RemapTrackHitIDs(Int_t *map);
   virtual void FindTrackHitsIntersection(TClonesArray * arr);
   //fill clones array with intersection of current point with the
   //middle of the row
   void SetHitType(Int_t type){fHitType =type;} //set type of hit container
   void SetDigitsSwitch(Int_t sw){fDigitsSwitch = sw;}
   void SetDefSwitch(Int_t def){fDefaults = def;}
   Float_t GetNoise();  //get Current noise  
   void    GenerNoise(Int_t tablasize);  // make noise table
   Bool_t  IsSectorActive(Int_t sec);    // check if the sector is active
   void    SetActiveSectors(Int_t * sectors, Int_t n);  //set active sectors
   Int_t GetHitType(){return fHitType;}
   void    SetActiveSectors(Int_t flag=0); //loop over al hits and set active only hitted sectors

// static functions
   static AliTPCParam* LoadTPCParam(TFile *file);  
   
 private:
  //
   Bool_t  TrackInVolume(Int_t id,Int_t track);  //return true if current track is in volume
  void SetDefaults();
  void DigitizeRow(Int_t irow,Int_t isec,TObjArray **rowTriplet);
  Float_t GetSignal(TObjArray *p1, Int_t ntr, AliTPCFastMatrix *m1, 
                   AliTPCFastMatrix *m2,Int_t *IndexRange);
  void GetList (Float_t label,Int_t np,AliTPCFastMatrix *m,Int_t *IndexRange,
                Float_t **pList);
  void MakeSector(Int_t isec,Int_t nrows,TTree *TH,Stat_t ntracks,TObjArray **row);
  void TransportElectron(Float_t *xyz, Int_t *index);
  Int_t fCurrentIndex[4];// index[0] indicates coordinate system, 
                         // index[1] sector number, 
                         // index[2] pad row number  
                         // index[3] pad row number for which signal is calculated
  Int_t      fNoiseDepth;  //!noise table
  Float_t *  fNoiseTable;  //![fNoiseDepth] table with noise
  Int_t      fCurrentNoise; //!index of the noise in  the noise table 
  Bool_t*    fActiveSectors; //!bool indicating which sectors are active

  ClassDef(AliTPC,9)  // Time Projection Chamber class
};


//_____________________________________________________________________________

class AliTPCdigit : public AliDigit {
public:
   Int_t     fSector;     //array of volumes
   Int_t     fPadRow;     //Row number
   Int_t     fPad ;       //Pad number
   Int_t     fTime;       //Time bucket
   Int_t     fSignal;     //Signal amplitude
 
public:
   AliTPCdigit() {}
   AliTPCdigit(Int_t *tracks, Int_t *digits);
   virtual ~AliTPCdigit() {}
 
   ClassDef(AliTPCdigit,1)  // Time Projection Chamber digits
};
 
 
//_____________________________________________________________________________
 
class AliTPChit : public AliHit {
public:
   Int_t     fSector;     //sector number
   Int_t     fPadRow;     //Pad Row number
   Float_t   fQ ;         //charge
 
public:
   AliTPChit() {}
   AliTPChit(Int_t shunt, Int_t track, Int_t *vol, Float_t *hits);
   virtual ~AliTPChit() {}
   void SetX(Float_t x){fX = x;}
   void SetY(Float_t y){fY = y;}
   void SetZ(Float_t z){fZ = z;}
 
   ClassDef(AliTPChit,1)  // Time Projection Chamber hits
};

#endif









