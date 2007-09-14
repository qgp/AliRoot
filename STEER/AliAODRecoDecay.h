#ifndef ALIAODRECODECAY_H
#define ALIAODRECODECAY_H
/* Copyright(c) 1998-2006, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//***********************************************************
// Class AliAODRecoDecay
// base class for AOD reconstructed decays
// Author: A.Dainese, andrea.dainese@lnl.infn.it
//***********************************************************

#include <TMath.h>
#include "AliAODVertex.h"
#include "AliVParticle.h"

class AliAODRecoDecay : public AliVParticle {

 public:

  AliAODRecoDecay();
  AliAODRecoDecay(AliAODVertex *vtx2,Int_t nprongs,Short_t charge,
		  Double_t *px,Double_t *py,Double_t *pz,
		  Double_t *d0);
  AliAODRecoDecay(AliAODVertex *vtx2,Int_t nprongs,Short_t charge,
		  Double_t *d0);
  virtual ~AliAODRecoDecay();

  AliAODRecoDecay(const AliAODRecoDecay& source);
  AliAODRecoDecay& operator=(const AliAODRecoDecay& source); 
   

  // decay vertex
  Double_t GetSecVtxX() const {return fSecondaryVtx->GetX();}
  Double_t GetSecVtxY() const {return fSecondaryVtx->GetY();}
  Double_t GetSecVtxZ() const {return fSecondaryVtx->GetZ();}
  Double_t RadiusSecVtx() const;
  void     SetSecondaryVtx(AliAODVertex *vtx2) {fSecondaryVtx=vtx2;}
  AliAODVertex* GetSecondaryVtx() const {return fSecondaryVtx;}
  void     GetSecondaryVtx(Double_t vtx[3]) const;
  Double_t GetReducedChi2() const {return fSecondaryVtx->GetChi2perNDF();}
  Short_t  Charge() const {return fCharge;}
  Short_t  GetCharge() const {return fCharge;}
  void     SetCharge(Short_t charge=0) {fCharge=charge;}

  // PID
  void      SetPID(Int_t nprongs,Double_t *pid);
  Double_t *GetPID() const { return fPID; }
  void      GetPIDProng(Int_t ip,Double_t *pid) const;
  virtual const Double_t *PID() const { return fPID; }

  // prong-to-prong DCAs
  void    SetDCAs(Int_t nDCA,Float_t *dca);
  void    SetDCA(Float_t dca); // 2 prong
  Float_t GetDCA(Int_t i=0) const {return fDCA[i];}

  //event and run number
  void SetEventRunNumbers(Int_t en,Int_t rn) 
    { fEventNumber=en; fRunNumber=rn; return; }
  Int_t GetEventNumber() const { return fEventNumber; }
  Int_t GetRunNumber() const { return fRunNumber; }

  // kinematics & topology
  Double_t Px() const; 
  Double_t Py() const;
  Double_t Pz() const;
  Double_t P() const {return TMath::Sqrt(Px()*Px()+Py()*Py()+Pz()*Pz());}
  Double_t Pt() const {return TMath::Sqrt(Px()*Px()+Py()*Py());}
  Double_t OneOverPt() const {return (Pt() ? 1./Pt() : 0.);}
  Double_t Phi() const {return TMath::Pi()+TMath::ATan2(-Py(),-Px());}
  Double_t Theta() const {return 0.5*TMath::Pi()-TMath::ATan(Pz()/(Pt()+1.e-13));}
  Double_t Eta() const {return 0.5*TMath::Log((P()+Pz())/(P()-Pz()+1.e-13));}
  Double_t E(UInt_t pdg) const;
  Double_t Y(UInt_t pdg) const {return 0.5*TMath::Log((E(pdg)+Pz())/(E(pdg)-Pz()+1.e-13));}
  Double_t DecayLength(Double_t point[3]) const;
  Double_t DecayLength(AliAODVertex *vtx1) const
    {return fSecondaryVtx->DistanceToVertex(vtx1);}
  Double_t DecayLengthError(AliAODVertex *vtx1) const
    {return fSecondaryVtx->ErrorDistanceToVertex(vtx1);}
  Double_t NormalizedDecayLength(AliAODVertex *vtx1) const 
    {return DecayLength(vtx1)/DecayLengthError(vtx1);}
  Double_t DecayLengthXY(Double_t point[3]) const;
  Double_t DecayLengthXY(AliAODVertex *vtx1) const
    {return fSecondaryVtx->DistanceXYToVertex(vtx1);}
  Double_t DecayLengthXYError(AliAODVertex *vtx1) const
    {return fSecondaryVtx->ErrorDistanceXYToVertex(vtx1);}
  Double_t NormalizedDecayLengthXY(AliAODVertex *vtx1) const 
    {return DecayLengthXY(vtx1)/DecayLengthXYError(vtx1);}
  Double_t Ct(UInt_t pdg,Double_t point[3]) const;
  Double_t Ct(UInt_t pdg,AliAODVertex *vtx1) const;
  Double_t CosPointingAngle(Double_t point[3]) const;
  Double_t CosPointingAngle(AliAODVertex *vtx1) const;
  Double_t CosPointingAngleXY(Double_t point[3]) const;
  Double_t CosPointingAngleXY(AliAODVertex *vtx1) const;
  Double_t CosThetaStar(Int_t ip,UInt_t pdgvtx,UInt_t pdgprong0,UInt_t pdgprong1) const;
  Double_t InvMass(Int_t npdg,UInt_t *pdg) const;
  Double_t ImpParXY(Double_t point[3]) const;
  Double_t ImpParXY(AliAODVertex *vtx1) const;

  // prongs
  //Int_t   GetNProngs() const {return fSecondaryVtx->GetNDaughters();}
  Int_t   GetNProngs() const {return fNProngs;}

  Short_t ChargeProng(Int_t ip) const;
  Double_t Getd0Prong(Int_t ip) const {return fd0[ip];}
  Double_t Prodd0d0(Int_t ip1=0,Int_t ip2=0) const {return fd0[ip1]*fd0[ip2];} 
  Double_t PxProng(Int_t ip) const {return fPx[ip];}
  Double_t PyProng(Int_t ip) const {return fPy[ip];}
  Double_t PzProng(Int_t ip) const {return fPz[ip];}
  Double_t PtProng(Int_t ip) const; 
  Double_t PProng(Int_t ip) const;
  Double_t PhiProng(Int_t ip) const 
    {return TMath::ATan2(PyProng(ip),PxProng(ip));}
    Double_t ThetaProng(Int_t ip) const 
      {return 0.5*TMath::Pi()-TMath::ATan(PzProng(ip)/(PtProng(ip)+1.e-13));}
  Double_t EtaProng(Int_t ip) const 
    {return -TMath::Log(TMath::Tan(0.5*ThetaProng(ip)));}
  Double_t EProng(Int_t ip,UInt_t pdg) const;
  Double_t YProng(Int_t ip,UInt_t pdg) const 
    {return 0.5*TMath::Log((EProng(ip,pdg)+PzProng(ip))/(EProng(ip,pdg)-PzProng(ip)+1.e-13));}
  Double_t Alpha() const;             // for Armenteros-Podolanski plot (V0's)
  Double_t QlProng(Int_t ip) const;
  Double_t QtProng(Int_t ip=0) const; // for Armenteros-Podolanski plot (V0's)
  Double_t QlProngFlightLine(Int_t ip,Double_t point[3]) const;
  Double_t QlProngFlightLine(Int_t ip,AliAODVertex *vtx1) const;
  Double_t QtProngFlightLine(Int_t ip,Double_t point[3]) const;
  Double_t QtProngFlightLine(Int_t ip,AliAODVertex *vtx1) const;
  Double_t InvMass2Prongs(Int_t ip1,Int_t ip2,UInt_t pdg1,UInt_t pdg2) const;
  Double_t ProngsRelAngle(Int_t ip1=0,Int_t ip2=1) const;

  // relate to other objects
  //Double_t DistanceToVertex(AliAODVertex *vtx) // distance to a AliAODVertex
  //Double_t DistanceToTrack(AliAODTrack *trk)   // distance to a AliAODTrack


  // print
  void    Print(Option_t* option = "") const;
  //void    PrintIndices() const {fSecondaryVtx->PrintIndices();}

  // dummy functions for inheritance from AliVParticle
  Double_t E() const 
    {printf("Dummy function; use AliAODRecoDecay::E(UInt_t pdg) instead"); return (Double_t)-999.;}
  Double_t Y() const 
    {printf("Dummy function; use AliAODRecoDecay::Y(UInt_t pdg) instead"); return (Double_t)-999.;}
  Double_t M() const 
    {printf("Dummy function"); return (Double_t)-999.;}

 protected:

  AliAODVertex *fSecondaryVtx;  // decay vertex
  Short_t  fCharge; // charge, use this convention for prongs charges:
                    // if(charge== 0) even-index prongs are +
                    //                odd-index prongs are -
                    // if(charge==+1) even-index prongs are +
                    //                odd-index prongs are -
                    // if(charge==-1) even-index prongs are -
                    //                odd-index prongs are +

  // TEMPORARY, to be removed when we do analysis on AliAODEvent
  Int_t fNProngs;  // number of prongs
  Int_t fNDCA;     // number of dca's
  Int_t fNPID;     // number of PID probabilities
  Double_t *fPx;   //[fNProngs] px of tracks at the vertex [GeV/c]
  Double_t *fPy;   //[fNProngs] py of tracks at the vertex [GeV/c]
  Double_t *fPz;   //[fNProngs] pz of tracks at the vertex [GeV/c]
  Double_t *fd0;   //[fNProngs] rphi impact params w.r.t. Primary Vtx [cm]
  Float_t *fDCA;   //[fNDCA] prong-to-prong DCA [cm]
                   // convention:fDCA[0]=p0p1,fDCA[1]=p0p2,fDCA[2]=p1p2,...
  Double_t *fPID;  //[fNPID] combined pid
                   //  (combined detector response probabilities)
                            
  // TEMPORARY, to be removed when we do analysis on AliAODEvent
  Int_t fEventNumber;
  Int_t fRunNumber;
  // TO BE PUT IN SPECIAL MC CLASS
  //Bool_t   fSignal; // TRUE if signal, FALSE if background (for simulation)
  //Int_t    fEvent;  // number of the event this candidate comes from
  //Int_t  fTrkNum[2]; // numbers of the two decay tracks  
  //Int_t fPdg[2];  // PDG codes of the two tracks (for sim.)
  //Int_t fMum[2];  // PDG codes of the mothers    (for sim.)

  //

  ClassDef(AliAODRecoDecay,1)  // base class for AOD reconstructed decays
};


inline Short_t AliAODRecoDecay::ChargeProng(Int_t ip) const
{
  if(fCharge==0 || fCharge==+1) {
    if(ip%2==0) {
      return (Short_t)1;
    } else {
      return (Short_t)-1;
    }
  } else { // fCharge==-1
    if(ip%2==0) {
      return (Short_t)-1;
    } else {
      return (Short_t)1;
    }
  }
}

inline Double_t AliAODRecoDecay::RadiusSecVtx() const 
{ 
  return TMath::Sqrt(GetSecVtxX()*GetSecVtxX()+GetSecVtxY()*GetSecVtxY());
}

inline void AliAODRecoDecay::GetSecondaryVtx(Double_t vtx[3]) const 
{
  fSecondaryVtx->GetPosition(vtx);
  return;
}

inline Double_t AliAODRecoDecay::Px() const 
{
  Double_t px=0.; 
  for(Int_t i=0;i<GetNProngs();i++) px+=PxProng(i); 
  return px;
}

inline Double_t AliAODRecoDecay::Py() const 
{
  Double_t py=0.; 
  for(Int_t i=0;i<GetNProngs();i++) py+=PyProng(i); 
  return py;
}

inline Double_t AliAODRecoDecay::Pz() const 
{
  Double_t pz=0.; 
  for(Int_t i=0;i<GetNProngs();i++) pz+=PzProng(i); 
  return pz;
}

inline Double_t AliAODRecoDecay::Ct(UInt_t pdg,AliAODVertex *vtx1) const
{
  Double_t v[3];
  vtx1->GetPosition(v);
  return Ct(pdg,v);
}

inline Double_t AliAODRecoDecay::CosPointingAngle(AliAODVertex *vtx1) const
{
  Double_t v[3];
  vtx1->GetPosition(v);
  return CosPointingAngle(v);
}

inline Double_t AliAODRecoDecay::CosPointingAngleXY(AliAODVertex *vtx1) const
{
  Double_t v[3];
  vtx1->GetPosition(v);
  return CosPointingAngleXY(v);
}

inline Double_t AliAODRecoDecay::ImpParXY(AliAODVertex *vtx1) const
{
  Double_t v[3];
  vtx1->GetPosition(v);
  return ImpParXY(v);
}

inline Double_t AliAODRecoDecay::PtProng(Int_t ip) const 
{
  return TMath::Sqrt(PxProng(ip)*PxProng(ip)+PyProng(ip)*PyProng(ip));
}

inline Double_t AliAODRecoDecay::PProng(Int_t ip) const 
{
  return TMath::Sqrt(PtProng(ip)*PtProng(ip)+PzProng(ip)*PzProng(ip));
}

inline Double_t AliAODRecoDecay::QlProngFlightLine(Int_t ip,AliAODVertex *vtx1) const
{
  Double_t v[3];
  vtx1->GetPosition(v);
  return QlProngFlightLine(ip,v);
}

inline Double_t AliAODRecoDecay::QtProngFlightLine(Int_t ip,AliAODVertex *vtx1) const
{
  Double_t v[3];
  vtx1->GetPosition(v);
  return QtProngFlightLine(ip,v);
}

inline void AliAODRecoDecay::SetDCAs(Int_t nDCA,Float_t *dca) 
{
  if(nDCA!=(GetNProngs()*(GetNProngs()-1)/2)) { 
    printf("Wrong number of DCAs, must be nProngs*(nProngs-1)/2");
    return;
  }
  if(fDCA) delete [] fDCA;
  fDCA = new Float_t[nDCA];
  for(Int_t i=0;i<nDCA;i++) 
    fDCA[i] = dca[i]; 
  return;
}

inline void AliAODRecoDecay::SetDCA(Float_t dca) 
{
  Float_t ddca[1]; ddca[0]=dca;
  SetDCAs(1,ddca);
  return;
}

inline void AliAODRecoDecay::SetPID(Int_t nprongs,Double_t *pid) 
{
  if(nprongs!=GetNProngs()) {
    printf("Wrong number of prongs");
    return;
  }
  if(fPID) delete [] fPID;
  fPID = new Double_t[nprongs*5];
  for(Int_t i=0;i<nprongs;i++) 
    for(Int_t j=0;j<5;j++)
      fPID[i*5+j] = pid[i*5+j]; 
  return;
}

inline void AliAODRecoDecay::GetPIDProng(Int_t ip,Double_t *pid) const
{ 
  for(Int_t j=0;j<5;j++)
    pid[j] = fPID[ip*5+j];
  return;
}



#endif
