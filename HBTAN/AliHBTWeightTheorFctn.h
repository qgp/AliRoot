#ifndef ALIHBTWeightTHEORFCTN_H
#define ALIHBTWeightTHEORFCTN_H
/* $Id$ */
//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
//
// class AliHBTWeightTheorQInvFctn
//
// This function allows to obtain Q_inv correlation function with weights
// calculated by Lednicky's alghorithm.
// Numerator is filled with weighted events. Weights are attributed to simulated particles.
// Weights are calculated with corresponding simulated particles momenta.
// Denominator is filled with mixing unweighted simulated particles.
// One needs only simulated pairs, so 
// this function is of class AliHBTOnePairFctn1D.
// Author Ludmila Malinina JINR (malinina@sunhe.jinr.ru)
//
///////////////////////////////////////////////////////////////////////////////

#include "AliHBTFunction.h"

class AliHBTWeights;

class AliHBTWeightTheorQInvFctn: public AliHBTOnePairFctn1D
{

  public:
  AliHBTWeightTheorQInvFctn(Int_t nbins = 100, Double_t maxXval = 0.15, Double_t minXval = 0.0);
  virtual  ~AliHBTWeightTheorQInvFctn(){}

  TH1* GetResult(); 
  void   ProcessSameEventParticles(AliHBTPair* partpair);
      
  Double_t GetValue(AliHBTPair* partpair)
    { return partpair->GetQInv();} 

  ClassDef(AliHBTWeightTheorQInvFctn,1)
};

class AliHBTWeightTheorQOutFctn: public AliHBTOnePairFctn1D
{

  public:
  AliHBTWeightTheorQOutFctn(Int_t nbins = 100, Double_t maxXval = 0.15, Double_t minXval = 0.0);
  virtual  ~AliHBTWeightTheorQOutFctn(){}

  TH1* GetResult(); 
  void   ProcessSameEventParticles(AliHBTPair* partpair);
      
  Double_t GetValue(AliHBTPair* partpair)
    { return partpair->GetQOutCMSLC();}

  ClassDef(AliHBTWeightTheorQOutFctn,1)
};

class AliHBTWeightTheorQSideFctn: public AliHBTOnePairFctn1D
{

  public:
  AliHBTWeightTheorQSideFctn(Int_t nbins = 100, Double_t maxXval = 0.15, Double_t minXval = 0.0);
  virtual  ~AliHBTWeightTheorQSideFctn(){}

  TH1* GetResult(); 
  void   ProcessSameEventParticles(AliHBTPair* partpair);
      
  Double_t GetValue(AliHBTPair* partpair)
    { return partpair->GetQSideCMSLC();} 

  ClassDef(AliHBTWeightTheorQSideFctn,1)
};

class AliHBTWeightTheorQLongFctn: public AliHBTOnePairFctn1D
{

  public:
  AliHBTWeightTheorQLongFctn(Int_t nbins = 100, Double_t maxXval = 0.15, Double_t minXval = 0.0);
  virtual  ~AliHBTWeightTheorQLongFctn(){}

  TH1* GetResult(); 
  void   ProcessSameEventParticles(AliHBTPair* partpair);
      
  Double_t GetValue(AliHBTPair* partpair)
    { return partpair->GetQLongCMSLC();} 

  ClassDef(AliHBTWeightTheorQLongFctn,1)
};

class AliHBTWeightTheorOSLFctn: public AliHBTOnePairFctn3D
{

  public:
    AliHBTWeightTheorOSLFctn(Int_t nXbins = 100, Double_t maxXval = 0.15, Double_t minXval = 0.0,
                             Int_t nYbins = 100, Double_t maxYval = 0.15, Double_t minYval = 0.0,
	         Int_t nZbins = 100, Double_t maxZval = 0.15, Double_t minZval = 0.0);
    virtual  ~AliHBTWeightTheorOSLFctn(){}

  TH1* GetResult();
  void   ProcessSameEventParticles(AliHBTPair* partpair);
  
  void GetValues(AliHBTPair* pair, Double_t& x, Double_t& y, Double_t& z)
    { x=TMath::Abs(pair->GetQOutCMSLC()); y=TMath::Abs(pair->GetQSideCMSLC()); z=TMath::Abs(pair->GetQLongCMSLC());} 

  ClassDef(AliHBTWeightTheorOSLFctn,1)
};
    
#endif
