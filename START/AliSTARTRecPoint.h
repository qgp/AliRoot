#ifndef ALISTARTRECPOINT_H
#define ALISTARTRECPOINT_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
#include <TObject.h>
class TArrayI;

//___________________________________________
class AliSTARTRecPoint: public TObject  {
////////////////////////////////////////////////////////////////////////
 public:
    AliSTARTRecPoint();
    virtual ~AliSTARTRecPoint();
    void SetMeanTime(Int_t time) {fTimeAverage=time;}
    Int_t  GetMeanTime() {return fTimeAverage;}
    Int_t  GetBestTimeRight() {return fTimeBestRight ;}
    Int_t  GetBestTimeLeft() {return fTimeBestLeft ;}
    void SetTimeBestRight( Int_t time) {fTimeBestRight = time;}
    void SetTimeBestLeft( Int_t time) {fTimeBestLeft = time;}
    void SetTimeDifference( Int_t time) {fTimeDifference= time;}
   Int_t  GetTimeDifference() {return fTimeDifference;}
    //    void SetProcessKoef( Float_t pp) {fProcessKoef = pp;}
    virtual void SetTime (TArrayI &o);
    virtual void GetTime (TArrayI &o);
    virtual void SetADC (TArrayI &o);
    virtual void GetADC (TArrayI &o);
    virtual const char* GetName() const {return "START_V";}
  private: 
    //    Float_t fProcessKoef;  // for pp fProcessKoef=1 ; for Pb-Pb - 0.001
    Int_t fTimeAverage;     // Average time
    Int_t fTimeDifference;     // Diffrence time between left and right
    Int_t fTimeBestRight;   //TOF first particle on the right
    Int_t fTimeBestLeft;    //TOF first particle on the left
    TArrayI *fTime;    // array's TDC in ns
    TArrayI *fADC;    // array's ADC in number of photo electrons

    ClassDef(AliSTARTRecPoint,1)  //Digit (Header) object for set:START
};


#endif



