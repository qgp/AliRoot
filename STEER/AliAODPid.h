#ifndef AliAODPid_H
#define AliAODPid_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//-------------------------------------------------------------------------
//     AOD Pid object for additional pid information
//     Author: Annalisa Mastroserio, CERN
//-------------------------------------------------------------------------

#include <TObject.h>

class AliAODPid : public TObject {

 public:
  AliAODPid();
  virtual ~AliAODPid();
  AliAODPid(const AliAODPid& pid); 
  AliAODPid& operator=(const AliAODPid& pid);
  
  enum{kSPECIES=5, kTRDnPlanes=6};

 //setters
  void      SetITSsignal(Double_t its)                         {fITSsignal=its;}
  void      SetTPCsignal(Double_t tpc)                         {fTPCsignal=tpc;}
  void      SetTPCmomentum(Double_t tpcMom)                    {fTPCmomentum=tpcMom;}
  void      SetTRDsignal(Int_t nslices, Double_t* trdslices)   {fTRDnSlices = nslices; fTRDslices=trdslices;}
  void      SetTRDmomentum(Int_t nplane, Float_t trdMom)       {fTRDmomentum[nplane]=trdMom;}
  void      SetTOFsignal(Double_t tof)                         {fTOFesdsignal=tof;}
  void      SetIntegratedTimes(Double_t timeint[5]);
  void      SetHMPIDsignal(Double_t hmpid)                     {fHMPIDsignal=hmpid;}
  void      SetEMCALPosition(Double_t emcalpos[3]);
  void      SetEMCALMomentum(Double_t emcalmom[3]);

  Double_t  GetITSsignal()       const {return  fITSsignal;}
  Double_t  GetTPCsignal()       const {return  fTPCsignal;}
  Double_t  GetTPCmomentum()     const {return  fTPCmomentum;}
  Int_t     GetTRDnSlices()      const {return  fTRDnSlices;}
  Double_t* GetTRDsignal()       const {return  fTRDslices;}
  Float_t*  GetTRDmomentum()           {return  fTRDmomentum;}
  Double_t  GetTOFsignal()       const {return  fTOFesdsignal;}
  Double_t  GetHMPIDsignal()     const {return  fHMPIDsignal;}

  void      GetIntegratedTimes(Double_t timeint[5])  const; 
  void      GetEMCALPosition  (Double_t emcalpos[3]) const;
  void      GetEMCALMomentum  (Double_t emcalmom[3]) const;

 private :
  Double32_t  fITSsignal;        //[0.,0.,10] detector raw signal
  Double32_t  fTPCsignal;        //[0.,0.,10] detector raw signal
  Double_t    fTPCmomentum;      // momentum at the inner wall of TPC;
  Int_t       fTRDnSlices;       // N slices used for PID in the TRD
  Double32_t* fTRDslices;        //[fTRDnSlices]
  Float_t     fTRDmomentum[6];   // momentum at the TRD layers
  Double32_t  fTOFesdsignal;     // TOF signal - t0 (T0 interaction time)
  Double32_t  fIntTime[5];       // track time hypothesis
  Double32_t  fHMPIDsignal;      // detector raw signal
  Double32_t  fEMCALPosition[3]; // global position of track
				 // extrapolated to EMCAL surface
  Double32_t  fEMCALMomentum[3]; // momentum of track
				 // extrapolated to EMCAL surface

  ClassDef(AliAODPid, 3);
};

#endif
