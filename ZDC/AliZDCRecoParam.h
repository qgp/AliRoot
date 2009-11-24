#ifndef ALIZDCRECOPARAM_H
#define ALIZDCRECOPARAM_H
/* Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//////////////////////////////////////////////////////////
//                                                      //
//   Class with ZDC reconstruction parameters           //
//   Origin: Chiara.Oppedisano@to.infn.it               //
//                                                      //
//////////////////////////////////////////////////////////

#include <TH1D.h>
#include <TF1.h>
#include "AliDetectorRecoParam.h"

//class TF1;

class AliZDCRecoParam : public AliDetectorRecoParam {
 public:
  AliZDCRecoParam();
  virtual ~AliZDCRecoParam();

  virtual TH1D*   GethNpartDist() const = 0;	
  virtual TH1D*   GethbDist()     const = 0;	
  virtual Float_t GetClkCenter()  const = 0;
      
  virtual void PrintParameters() const {;} 
  
 protected:
  
  AliZDCRecoParam(const AliZDCRecoParam&);
  AliZDCRecoParam& operator =(const AliZDCRecoParam&);
   
 ClassDef(AliZDCRecoParam, 2)

};

#endif
