#ifndef ALIITSURECOPARAM_H
#define ALIITSURECOPARAM_H
/* Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id: AliITSURecoParam.h 57215 2012-06-17 14:47:08Z masera $ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Class with ITS reconstruction parameters                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include "AliDetectorRecoParam.h"

class AliITSURecoParam : public AliDetectorRecoParam
{
 public: 
  AliITSURecoParam();
  AliITSURecoParam(Int_t nLr);
  virtual ~AliITSURecoParam();

  static AliITSURecoParam *GetLowFluxParam();// make reco parameters for low flux env.
  static AliITSURecoParam *GetHighFluxParam();// make reco parameters for high flux env. 
  static AliITSURecoParam *GetCosmicTestParam();// special setting for cosmic  

  Double_t    GetMaxDforV0dghtrForProlongation() const {return fMaxDforV0dghtrForProlongation;}
  Double_t    GetMaxDForProlongation()           const {return fMaxDForProlongation;}
  Double_t    GetMaxDZForProlongation()          const {return fMaxDZForProlongation;}
  Double_t    GetMinPtForProlongation()          const {return fMinPtForProlongation;}
  Double_t    GetTanLorentzAngle(Int_t lr)       const;
  Double_t    GetSigmaY2(Int_t lr)               const;
  Double_t    GetSigmaZ2(Int_t lr)               const;
  Double_t    GetMaxTr2ClChi2(Int_t lr)          const;
  //
  Double_t    GetNSigmaRoadY()                   const {return fNSigmaRoadY;}
  Double_t    GetNSigmaRoadZ()                   const {return fNSigmaRoadZ;}
  //
  Double_t    GetTPCITSWallRMin()                const {return fTPCITSWallRMin;}
  Double_t    GetTPCITSWallRMax()                const {return fTPCITSWallRMax;}
  Double_t    GetTPCITSWallZSpanH()              const {return fTPCITSWallZSpanH;}
  Double_t    GetTPCITSWallMaxStep()             const {return fTPCITSWallMaxStep;}
  //
  void        SetNLayers(Int_t n);
  void        SetTanLorentzAngle(Int_t lr, Double_t v);
  void        SetSigmaY2(Int_t lr, Double_t v);
  void        SetSigmaZ2(Int_t lr, Double_t v);
  void        SetMaxTr2ClChi2(Int_t lr, Double_t v);
  //
  void        GetMaxDforV0dghtrForProlongation(Double_t v)            {fMaxDforV0dghtrForProlongation = v;}
  void        SetMaxDForProlongation(Double_t v)                      {fMaxDForProlongation = v;}
  void        SetMaxDZForProlongation(Double_t v)                     {fMaxDZForProlongation = v;}
  void        SetMinPtForProlongation(Double_t v)                     {fMinPtForProlongation = v;}
  void        SetNSigmaRoadY(Double_t v)                              {fNSigmaRoadY=v;}
  void        SetNSigmaRoadZ(Double_t v)                              {fNSigmaRoadZ=v;}
  //
  void        SetTPCITSWallRMin(double v)                             {fTPCITSWallRMin = v;}
  void        SetTPCITSWallRMax(double v)                             {fTPCITSWallRMax = v;}
  void        SetTPCITSWallZSpanH(double v)                           {fTPCITSWallZSpanH = v;}
  void        SetTPCITSWallMaxStep(double v)                          {fTPCITSWallMaxStep = v;}
  //
  virtual void Print(Option_t *opt="")  const;
  //
 protected:
  Int_t          fNLayers;          // number of layers 
  //
  Double_t       fMaxDforV0dghtrForProlongation; // max. rphi imp. par. cut for V0 daughter
  Double_t       fMaxDForProlongation; // max. rphi imp. par. cut
  Double_t       fMaxDZForProlongation; // max. 3D imp. par. cut
  Double_t       fMinPtForProlongation; // min. pt cut
  Double_t       fNSigmaRoadY;          // N sigmas for road in Y
  Double_t       fNSigmaRoadZ;          // N sigmas for road in Z
  //
  Double_t       fTPCITSWallRMin;       // minR
  Double_t       fTPCITSWallRMax;       // maxR
  Double_t       fTPCITSWallZSpanH;     // half Z span
  Double_t       fTPCITSWallMaxStep;    // max tracking step
  //
  Double_t*      fTanLorentzAngle;  //[fNLayers] Optional Lorentz angle for each layer
  Double_t*      fSigmaY2;          //[fNLayers] addition to road width^2
  Double_t*      fSigmaZ2;          //[fNLayers] addition to road width^2
  Double_t*      fMaxTr2ClChi2;     //[fNLayers] max track-to-cluster chi2
  //


  static const Double_t fgkMaxDforV0dghtrForProlongation;      // default
  static const Double_t fgkMaxDForProlongation;                // default
  static const Double_t fgkMaxDZForProlongation;               // default
  static const Double_t fgkMinPtForProlongation;               // default
  static const Double_t fgkNSigmaRoadY;                        // default
  static const Double_t fgkNSigmaRoadZ;                        // default
  // for arrays
  static const Double_t fgkSigmaRoadY;                         // default
  static const Double_t fgkSigmaRoadZ;                         // default
  static const Double_t fgkMaxTr2ClChi2;                       // default
  static const Double_t fgkTanLorentzAngle;                    // default
  //
  // hardwired params for TPC-ITS border layer
  static const Double_t fgkTPCITSWallRMin;                     // minR
  static const Double_t fgkTPCITSWallRMax;                     // maxR
  static const Double_t fgkTPCITSWallZSpanH;                   // half Z span
  static const Double_t fgkTPCITSWallMaxStep;                  // max tracking step
  //
 private:
  AliITSURecoParam(const AliITSURecoParam & param);
  AliITSURecoParam & operator=(const AliITSURecoParam &param);

  ClassDef(AliITSURecoParam,1) // ITS reco parameters
};

//_____________________________________________________________________________
inline Double_t AliITSURecoParam::GetTanLorentzAngle(Int_t lr) const 
{
  // get tg of Lorentz Angle for the layer
  return (lr<fNLayers)&&fTanLorentzAngle ? fTanLorentzAngle[lr]:0;
}

//_____________________________________________________________________________
inline Double_t AliITSURecoParam::GetSigmaY2(Int_t lr) const 
{
  // get tg of Lorentz Angle for the layer
  return (lr<fNLayers)&&fSigmaY2 ? fSigmaY2[lr]:fgkSigmaRoadY*fgkSigmaRoadY; //0;
}

//_____________________________________________________________________________
inline Double_t AliITSURecoParam::GetSigmaZ2(Int_t lr) const 
{
  // get tg of Lorentz Angle for the layer
  return (lr<fNLayers)&&fSigmaZ2 ? fSigmaZ2[lr]:fgkSigmaRoadZ*fgkSigmaRoadZ;//0;
}

//_____________________________________________________________________________
inline Double_t AliITSURecoParam::GetMaxTr2ClChi2(Int_t lr) const
{
  // get tg of Lorentz Angle for the layer
  return (lr<fNLayers)&&fMaxTr2ClChi2 ? fMaxTr2ClChi2[lr]:fgkMaxTr2ClChi2; //0;
}

#endif


