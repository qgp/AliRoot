#ifndef ALIESDTRACK_H
#define ALIESDTRACK_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//-------------------------------------------------------------------------
//                          Class AliESDtrack
//   This is the class to deal with during the physical analysis of data
//      
//         Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//-------------------------------------------------------------------------
#include "TObject.h"

class AliKalmanTrack;

class AliESDtrack : public TObject {
public:
  AliESDtrack();
  virtual ~AliESDtrack() {}
  void SetStatus(ULong_t flags) {fFlags|=flags;}
  void ResetStatus(ULong_t flags) {fFlags&=~flags;}
  Bool_t UpdateTrackParams(const AliKalmanTrack *t, ULong_t flags);
  void SetIntegratedLength(Double_t l) {fTrackLength=l;}
  void SetIntegratedTimes(const Double_t *times);
  void SetESDpid(const Double_t *p);
  void GetESDpid(Double_t *p) const;
  
  ULong_t GetStatus() const {return fFlags;}
  Int_t GetLabel() const {return fLabel;}
  Double_t GetAlpha() const {return fRalpha;}
  void GetExternalParameters(Double_t &x, Double_t p[5]) const;
  void GetExternalCovariance(Double_t cov[15]) const;
  Double_t GetIntegratedLength() const {return fTrackLength;}
  void GetIntegratedTimes(Double_t *times) const;
  Float_t GetMass() const;
  Double_t GetP() const;
  void GetPxPyPz(Double_t *p) const;
  void GetXYZ(Double_t *r) const;

  void SetTPCpid(const Double_t *p);
  void GetTPCpid(Double_t *p) const;
  Float_t GetTPCsignal() const {return fTPCsignal;}
  Int_t GetTPCclusters(Int_t *idx) const;

  void SetITSpid(const Double_t *p);
  void GetITSpid(Double_t *p) const;
  Float_t GetITSsignal() const {return fITSsignal;}
  Int_t GetITSclusters(UInt_t *idx) const;

  void SetTRDpid(const Double_t *p);
  void GetTRDpid(Double_t *p) const;
  Float_t GetTRDsignal() const {return fTRDsignal;}
  void    SetTRDpid(Int_t iSpecies, Float_t p);
  Float_t GetTRDpid(Int_t iSpecies) const;

  void SetTOFsignal(Double_t tof) {fTOFsignal=tof;}
  Float_t GetTOFsignal() const {return fTOFsignal;}
  void    SetTOFpid(const Double_t *p);
  void    GetTOFpid(Double_t *p) const;
  UInt_t  GetTOFcluster() const {return fTOFindex;}
  void  SetTOFcluster(UInt_t index) {fTOFindex=index;}

  enum {
    kITSin=0x0001,kITSout=0x0002,kITSrefit=0x0004,kITSpid=0x0008,
    kTPCin=0x0010,kTPCout=0x0020,kTPCrefit=0x0040,kTPCpid=0x0080,
    kTRDin=0x0100,kTRDout=0x0200,kTRDrefit=0x0400,kTRDpid=0x0800,
    kTOFin=0x1000,kTOFout=0x2000,kTOFrefit=0x4000,kTOFpid=0x8000,
    kESDpid=0x40000000,
    kTIME=0x80000000
  }; 
  enum {kSPECIES=5}; // Number of particle species recognized by the PID

protected:
  ULong_t   fFlags;        // Reconstruction status flags 
  Int_t     fLabel;        // Track label

  Float_t   fTrackLength;         // Track length
  Float_t   fTrackTime[kSPECIES]; // TOFs estimated by the tracking
  Float_t   fR[kSPECIES];         // combined "detector response probability"

  Int_t     fStopVertex;          // Index of stop vertex

//Running track parameters
  Double_t fRalpha;  // track rotation angle
  Double_t fRx;      // X-coordinate of the track reference plane 
  Double_t fRp[5];   // external track parameters  
  Double_t fRc[15];  // external cov. matrix of the track parameters

//Track parameters at the innermost measured point
  //Double_t fIalpha,fIx,fIp[5],fIc[15];

//Track parameters at the outermost measured point
  //Double_t fOalpha,fOx,fOp[5],fOc[15];

  // ITS related track information
  Float_t fITSchi2;        // chi2 in the ITS
  Int_t   fITSncls;        // number of clusters assigned in the ITS
  UInt_t  fITSindex[6];    //! indices of the assigned ITS clusters
  Float_t fITSsignal;      // detector's PID signal
  Float_t fITSr[kSPECIES]; //! "detector response probabilities" (for the PID)

  // TPC related track information
  Float_t fTPCchi2;        // chi2 in the TPC
  Int_t   fTPCncls;        // number of clusters assigned in the TPC
  Int_t  fTPCindex[180];  //! indices of the assigned TPC clusters
  Float_t fTPCsignal;      // detector's PID signal
  Float_t fTPCr[kSPECIES]; // "detector response probabilities" (for the PID)

  // TRD related track information
  Float_t fTRDchi2;        // chi2 in the TRD
  Int_t   fTRDncls;        // number of clusters assigned in the TRD
  Float_t fTRDsignal;      // detector's PID signal
  Float_t fTRDr[kSPECIES]; //! "detector response probabilities" (for the PID)

  // TOF related track information
  Float_t fTOFchi2;        // chi2 in the TOF
  UInt_t  fTOFindex;       //! index of the assigned TOF cluster
  Float_t fTOFsignal;      // detector's PID signal
  Float_t fTOFr[kSPECIES]; // "detector response probabilities" (for the PID)

  // HMPID related track information

  ClassDef(AliESDtrack,1)  //ESDtrack 
};

#endif 

