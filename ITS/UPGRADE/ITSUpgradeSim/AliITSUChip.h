#ifndef ALIITSUCHIP_H
#define ALIITSUCHIP_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id: AliITSUChip.h 53509 2011-12-10 18:55:52Z masera $ */
///////////////////////////////////////////////////////////////////////
//                                                                   //
//  Class AliITSUChip                                                //
//  The main function of chips is to simulate DIGITS from            //
//  GEANT HITS and produce POINTS from DIGITS                        //
//  It also make fast simulation without use of DIGITS               //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TObjArray.h>
#include "AliITSUHit.h"
class AliITSUGeomTGeo;

class AliITSUChip: public TObject {

 public:
  AliITSUChip();             // default constructor
  AliITSUChip(Int_t index, AliITSUGeomTGeo* tg);
  virtual ~AliITSUChip();
  UInt_t     GetIndex()                                 const {return GetUniqueID();}
  void       SetIndex(UInt_t ind)                             {return SetUniqueID(ind);}
  Int_t      GetNHits()                                 const {return fHitsM->GetEntriesFast();}
  TObjArray *GetHits()                                  const {return fHitsM;}
  AliITSUHit *GetHit(Int_t i)                           const {return (AliITSUHit*)fHitsM->UncheckedAt(i);}
  void       AddHit(AliITSUHit *hit)                          {fHitsM->AddLast(hit);}
  void       Clear(Option_t* opt=0);
  //
  Bool_t   MedianHitG(AliITSUHit *h1,AliITSUHit *h2,Float_t &x,Float_t &y,Float_t &z);
  void     MedianHitG(Int_t index, Float_t hitx1,Float_t hity1,Float_t hitz1,Float_t hitx2,Float_t hity2,Float_t hitz2, Float_t &xMg,Float_t &yMg, Float_t &zMg);
  Bool_t   MedianHitL(AliITSUHit *h1,AliITSUHit *h2,Float_t &x,Float_t &y,Float_t &z) const;
  void     MedianHitL(Int_t,AliITSUHit *,AliITSUHit *,Float_t &,Float_t &, Float_t &){};
  Double_t PathLength(const AliITSUHit *itsHit1,const AliITSUHit *itsHit2);
  void     MedianHit(Int_t index, Float_t xg,Float_t yg,Float_t zg,Int_t status,Float_t &xMg, Float_t &yMg, Float_t &zMg,Int_t &flag);
  void     PathLength(Float_t x,Float_t y,Float_t z,Int_t status,Int_t &nseg,Float_t &x1,Float_t &y1,Float_t &z1,Float_t &dx1,Float_t &dy1, Float_t &dz1,Int_t &flag) const;
  Bool_t   LineSegmentL(Int_t hindex,Double_t &a,Double_t &b,Double_t &c,Double_t &d,Double_t &e,Double_t &f,Double_t &de);
  Bool_t   LineSegmentL(Int_t hindex,Double_t &a,Double_t &b,Double_t &c,Double_t &d,Double_t &e,Double_t &f,Double_t &de, Double_t &tof, Int_t &track);
  //
  Bool_t   LineSegmentG(Int_t hindex,Double_t &a,Double_t &b,Double_t &c,Double_t &d,Double_t &e,Double_t &f,Double_t &de);
  Bool_t   LineSegmentG(Int_t hindex,Double_t &a,Double_t &b,Double_t &c,Double_t &d,Double_t &e,Double_t &f,Double_t &de, Double_t &tof, Int_t &track);
  //
 protected:
    AliITSUChip(const AliITSUChip &source); 
    AliITSUChip& operator=(const AliITSUChip &source); 
    TObjArray           *fHitsM;     // Pointer to list of hits on this chip
    AliITSUGeomTGeo    *fGeomTG;    // pointed to geometry
    //
    ClassDef(AliITSUChip,1) // Copy the hits into a more useful order
};

inline void AliITSUChip::Clear(Option_t *) {fHitsM->Clear();}

#endif



