#ifndef ALIHBTPAIRCUT_H
#define ALIHBTPAIRCUT_H

/* $Id$ */

//Piotr Skowronski@cern.ch
//Class implements cut on the pair of particles
//
//more info: http://alisoft.cern.ch/people/skowron/analyzer/index.html
 
#include "AliHBTPair.h"

class AliHBTParticleCut;
class AliHbtBasePairCut;

enum AliHBTPairCutProperty
{
  kHbtPairCutPropQInv, //Q invariant
  kHbtPairCutPropKt,
  kHbtPairCutPropKStar,
  kHbtPairCutPropQSideCMSLC,
  kHbtPairCutPropQOutCMSLC,
  kHbtPairCutPropQLongCMSLC,
  kHbtPairCutPropNone
};

class AliHBTPairCut: public TNamed
{
 public:
  AliHBTPairCut();
  AliHBTPairCut(const AliHBTPairCut& in);
  AliHBTPairCut& operator = (const AliHBTPairCut& in);
  
  virtual ~AliHBTPairCut();
  virtual Bool_t Pass(AliHBTPair* pair) const;
  virtual Bool_t PassPairProp(AliHBTPair* pair) const;
     
  virtual Bool_t IsEmpty() const {return kFALSE;}
  void SetFirstPartCut(AliHBTParticleCut* cut);  //sets the cut on the first particle
  void SetSecondPartCut(AliHBTParticleCut* cut); //sets the cut on the second particle
  
  void SetPartCut(AliHBTParticleCut* cut);//sets the the same cut on both particles
  
  void AddBasePairCut(AliHbtBasePairCut* cut);
  
  void SetQInvRange(Double_t min, Double_t max);
  void SetKtRange(Double_t min, Double_t max);
  void SetKStarRange(Double_t min, Double_t max);
  void SetQOutCMSLRange(Double_t min, Double_t max);
  void SetQSideCMSLRange(Double_t min, Double_t max);
  void SetQLongCMSLRange(Double_t min, Double_t max);
  
  AliHBTParticleCut* GetFirstPartCut() const {return fFirstPartCut;}
  AliHBTParticleCut* GetSecondPartCut() const {return fSecondPartCut;}
  
 protected:
  AliHBTParticleCut*      fFirstPartCut;//cut on first particle in pair
  AliHBTParticleCut*      fSecondPartCut;//cut on second particle in pair
  
  AliHbtBasePairCut** fCuts; //! array of poiters to base cuts
  Int_t fNCuts;//Number of cuts in fCuts array
  
  
  AliHbtBasePairCut* FindCut(AliHBTPairCutProperty cut);
 private:
  static const Int_t fgkMaxCuts; // Max number of cuts
  ClassDef(AliHBTPairCut,2)
};
/******************************************************************/
/******************************************************************/
/******************************************************************/

class AliHBTEmptyPairCut:  public AliHBTPairCut
{
  //Empty - it passes possitively all particles - it means returns always False
  //Class describing cut on pairs of particles
 public:
  AliHBTEmptyPairCut(){};
  AliHBTEmptyPairCut(const AliHBTEmptyPairCut& in){}; 
  virtual ~AliHBTEmptyPairCut(){};
  
  Bool_t Pass(AliHBTPair*) const {return kFALSE;} //accpept everything
  Bool_t IsEmpty() const {return kTRUE;}
  
  ClassDef(AliHBTEmptyPairCut,1)
};



/******************************************************************/
/******************************************************************/
/******************************************************************/

class AliHbtBasePairCut: public TObject
{
  //This class defines the range of some property - pure virtual
  //Property is coded by AliHBTCutTypes type
   
 public:
     
  AliHbtBasePairCut(Double_t min = 0.0, Double_t max = 0.0, AliHBTPairCutProperty prop= kHbtPairCutPropNone):
    fMin(min),fMax(max),fProperty(prop){}
  
  virtual   ~AliHbtBasePairCut(){}
     
  Bool_t    Pass(AliHBTPair* pair) const;
  
  void      SetRange(Double_t min, Double_t max){fMin = min; fMax = max;}
  
  void      SetMinimum(Double_t min){fMin = min;}
  void      SetMaximum(Double_t max){fMax = max;}
  
  Double_t  GetMinimum() const {return fMin;}
  Double_t  GetMaximum() const {return fMax;}
  
  AliHBTPairCutProperty GetProperty() const {return fProperty;}
  
 protected:
  virtual Double_t  GetValue(AliHBTPair* pair) const = 0;
  
  Double_t fMin; // Lower boundary of the range
  Double_t fMax; // Upper boundary of the range
  
  AliHBTPairCutProperty fProperty; // The property itself
  
  ClassDef(AliHbtBasePairCut,1)
 
 };

inline Bool_t AliHbtBasePairCut::Pass(AliHBTPair* pair) const
{
  Double_t value = GetValue(pair);
  if ( (value > fMin) && (value <fMax ) ) return kFALSE; //accepted
  else return kTRUE; //rejected
}
/******************************************************************/
/******************************************************************/
/******************************************************************/
class AliHBTQInvCut: public AliHbtBasePairCut
{
 public:
  AliHBTQInvCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBasePairCut(min,max,kHbtPairCutPropQInv){}
  virtual ~AliHBTQInvCut(){}
 protected:
  virtual Double_t  GetValue(AliHBTPair* pair) const {return pair->GetQInv();}
  
  ClassDef(AliHBTQInvCut,1)
 };


class AliHBTKtCut: public AliHbtBasePairCut {
 public:
  AliHBTKtCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBasePairCut(min,max,kHbtPairCutPropKt){}
  virtual ~AliHBTKtCut(){}
 protected:
  virtual Double_t  GetValue(AliHBTPair* pair) const {return pair->GetKt();}

  ClassDef(AliHBTKtCut,1)
 };

class AliHBTKStarCut: public AliHbtBasePairCut
{
 public:
  AliHBTKStarCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBasePairCut(min,max,kHbtPairCutPropKStar){}
  virtual ~AliHBTKStarCut(){}
 protected:
  virtual Double_t  GetValue(AliHBTPair* pair) const {return pair->GetKStar();}

  ClassDef(AliHBTKStarCut,1)
};

class AliHBTQSideCMSLCCut: public AliHbtBasePairCut
{
 public:
  AliHBTQSideCMSLCCut(Double_t min = 0.0, Double_t max = 0.0):
    AliHbtBasePairCut(min,max,kHbtPairCutPropQSideCMSLC){}
  virtual ~AliHBTQSideCMSLCCut(){}
 protected:
  virtual Double_t  GetValue(AliHBTPair* pair) const 
    {return pair->GetQSideCMSLC();}

  ClassDef(AliHBTQSideCMSLCCut,1)
};


class AliHBTQOutCMSLCCut: public AliHbtBasePairCut
{
 public:
  AliHBTQOutCMSLCCut(Double_t min = 0.0, Double_t max = 0.0):
    AliHbtBasePairCut(min,max,kHbtPairCutPropQOutCMSLC){}
  virtual ~AliHBTQOutCMSLCCut(){}
 protected:
  virtual Double_t  GetValue(AliHBTPair* pair) const 
    {return pair->GetQOutCMSLC();}
  
  ClassDef(AliHBTQOutCMSLCCut,1)
};

class AliHBTQLongCMSLCCut: public AliHbtBasePairCut
{
 public:
  AliHBTQLongCMSLCCut(Double_t min = 0.0, Double_t max = 0.0):
    AliHbtBasePairCut(min,max,kHbtPairCutPropQLongCMSLC){}
  virtual ~AliHBTQLongCMSLCCut(){}
 protected:
  virtual Double_t  GetValue(AliHBTPair* pair) const 
    {return pair->GetQLongCMSLC();}

  ClassDef(AliHBTQLongCMSLCCut,1)
};

#endif
