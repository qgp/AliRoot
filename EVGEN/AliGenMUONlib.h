#ifndef ALIGENMUONLIB_H
#define ALIGENMUONLIB_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

// Library class for particle pt and y distributions used for 
// muon spectrometer simulations.
// To be used with AliGenParam.
//
// andreas.morsch@cern.ch

#include "AliGenLib.h"

class AliGenMUONlib :
  public AliGenLib
{
 public:
  enum constants{kPhi, kOmega, kEta, kJpsi, kJpsiFamily, kPsiP, kJpsiFromB, kUpsilon, kUpsilonFamily,
		   kUpsilonP, kUpsilonPP, kCharm, kBeauty, kPion, kKaon, kChic, kChic0, kChic1, kChic2 }; 
    
    GenFunc   GetPt(Int_t param, const char* tname=0) const;
    GenFunc   GetY (Int_t param, const char* tname=0) const;
    GenFuncIp GetIp(Int_t param, const char* tname=0) const;
 private:
    
// pions
    static Double_t PtPion(const Double_t *px, const Double_t *dummy);
    static Double_t PtScal(Double_t pt, Int_t np);
    static Double_t YPion( const Double_t *py, const Double_t *dummy);
    static Int_t    IpPion(TRandom *ran);
// kaons
    static Double_t PtKaon(const Double_t *px, const Double_t *dummy);
    static Double_t YKaon( const Double_t *py, const Double_t *dummy);
    static Int_t    IpKaon(TRandom *ran);
// Phi
    static Double_t PtPhi( const Double_t *px, const Double_t *dummy);
    static Double_t YPhi( const  Double_t *px, const Double_t *dummy);
    static Int_t    IpPhi(TRandom *ran);
// Omega
    static Double_t PtOmega( const Double_t *px, const Double_t *dummy);
    static Double_t YOmega( const Double_t *px, const Double_t *dummy);
    static Int_t    IpOmega(TRandom *ran);
// Eta
    static Double_t PtEta( const Double_t *px, const Double_t *dummy);
    static Double_t YEta( const Double_t *px, const Double_t *dummy);
    static Int_t    IpEta(TRandom *ran);
// J/Psi     
    static Double_t PtJpsi( const Double_t *px, const Double_t *dummy);
    static Double_t PtJpsiCDFscaled( const Double_t *px, const Double_t *dummy);
    static Double_t PtJpsiCDFscaledPP( const Double_t *px, const Double_t *dummy);
    static Double_t PtJpsiCDFscaledPP10( const Double_t *px, const Double_t *dummy);
    static Double_t PtJpsiCDFscaledold( const Double_t *px, const Double_t *dummy);
    static Double_t PtJpsiCDFscaledPPold( const Double_t *px, const Double_t *dummy);
    static Double_t YJpsi(const Double_t *py, const Double_t *dummy);
    static Double_t PtJpsiPbPb( const Double_t *px, const Double_t *dummy);
    static Double_t PtJpsiBPbPb( const Double_t *px, const Double_t *dummy);
    static Double_t YJpsiPbPb(const Double_t *py, const Double_t *dummy);
    static Double_t YJpsiCDFscaled(const Double_t *py, const Double_t *dummy);
    static Double_t YJpsiCDFscaledPP( const Double_t *px, const Double_t *dummy);
    static Double_t YJpsiCDFscaledPP10( const Double_t *px, const Double_t *dummy);
    static Double_t YJpsiCDFscaledold(const Double_t *py, const Double_t *dummy);
    static Double_t YJpsiCDFscaledPPold( const Double_t *px, const Double_t *dummy);
    static Double_t PtJpsiPP( const Double_t *px, const Double_t *dummy);
    static Double_t YJpsiPP(const Double_t *py, const Double_t *dummy);
    static Double_t YJpsiBPbPb(const Double_t *py, const Double_t *dummy);
    static Int_t    IpJpsi(TRandom *ran);
    static Int_t    IpJpsiFamily(TRandom *ran);
    static Int_t    IpPsiP(TRandom *ran);
    static Double_t PtJpsiFlat( const Double_t *px, const Double_t *dummy );
    static Double_t YJpsiFlat(const Double_t *py, const Double_t *dummy);

// Upsilon    
    static Double_t PtUpsilon( const Double_t *px, const Double_t *dummy );
    static Double_t PtUpsilonCDFscaled( const Double_t *px, const Double_t *dummy );
    static Double_t PtUpsilonCDFscaledPP( const Double_t *px, const Double_t *dummy );
    static Double_t YUpsilon(const Double_t *py, const Double_t *dummy);
    static Double_t YUpsilonCDFscaled(const Double_t *py, const Double_t *dummy);
    static Double_t YUpsilonCDFscaledPP( const Double_t *px, const Double_t *dummy );
    static Double_t PtUpsilonPbPb( const Double_t *px, const Double_t *dummy );
    static Double_t YUpsilonPbPb(const Double_t *py, const Double_t *dummy);
    static Double_t PtUpsilonPP( const Double_t *px, const Double_t *dummy );
    static Double_t YUpsilonPP(const Double_t *py, const Double_t *dummy);
    static Int_t    IpUpsilon(TRandom *ran);
    static Int_t    IpUpsilonFamily(TRandom *ran);
    static Int_t    IpUpsilonP(TRandom *ran);
    static Int_t    IpUpsilonPP(TRandom *ran);
    static Double_t PtUpsilonFlat( const Double_t *px, const Double_t *dummy );
    static Double_t YUpsilonFlat(const Double_t *py, const Double_t *dummy);
//
// Charm    
    static Double_t PtCharm( const Double_t *px, const Double_t *dummy );
    static Double_t PtCharmCentral( const Double_t *px, const Double_t *dummy );
    static Double_t PtCharmF0M0S0PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtCharmF1M0S0PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtCharmF2M0S0PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtCharmF0M1S0PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtCharmF0M2S0PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtCharmF0M0S1PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtCharmF0M0S2PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtCharmF0M0S3PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtCharmF0M0S4PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtCharmF0M0S5PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtCharmF0M0S6PP( const Double_t *px, const Double_t *dummy );
    static Double_t YCharm(const Double_t *py, const Double_t *dummy);
    static Double_t YCharmF0M0S0PP(const Double_t *py, const Double_t *dummy);
    static Double_t YCharmF1M0S0PP(const Double_t *py, const Double_t *dummy);
    static Double_t YCharmF2M0S0PP(const Double_t *py, const Double_t *dummy);
    static Double_t YCharmF0M1S0PP(const Double_t *py, const Double_t *dummy);
    static Double_t YCharmF0M2S0PP(const Double_t *py, const Double_t *dummy);
    static Double_t YCharmF0M0S1PP(const Double_t *py, const Double_t *dummy);
    static Double_t YCharmF0M0S2PP(const Double_t *py, const Double_t *dummy);
    static Double_t YCharmF0M0S3PP(const Double_t *py, const Double_t *dummy);
    static Double_t YCharmF0M0S4PP(const Double_t *py, const Double_t *dummy);
    static Double_t YCharmF0M0S5PP(const Double_t *py, const Double_t *dummy);
    static Double_t YCharmF0M0S6PP(const Double_t *py, const Double_t *dummy);
    static Int_t    IpCharm(TRandom *ran);
//
// Beauty
    static Double_t PtBeauty( const Double_t *px, const Double_t *dummy );
    static Double_t PtBeautyF0M0S0PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtBeautyF1M0S0PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtBeautyF2M0S0PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtBeautyF0M1S0PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtBeautyF0M2S0PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtBeautyF0M0S1PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtBeautyF0M0S2PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtBeautyF0M0S3PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtBeautyF0M0S4PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtBeautyF0M0S5PP( const Double_t *px, const Double_t *dummy );
    static Double_t PtBeautyF0M0S6PP( const Double_t *px, const Double_t *dummy );
    static Double_t YBeauty(const Double_t *py, const Double_t *dummy);
    static Double_t YBeautyF0M0S0PP(const Double_t *py, const Double_t *dummy);
    static Double_t YBeautyF1M0S0PP(const Double_t *py, const Double_t *dummy);
    static Double_t YBeautyF2M0S0PP(const Double_t *py, const Double_t *dummy);
    static Double_t YBeautyF0M1S0PP(const Double_t *py, const Double_t *dummy);
    static Double_t YBeautyF0M2S0PP(const Double_t *py, const Double_t *dummy);
    static Double_t YBeautyF0M0S1PP(const Double_t *py, const Double_t *dummy);
    static Double_t YBeautyF0M0S2PP(const Double_t *py, const Double_t *dummy);
    static Double_t YBeautyF0M0S3PP(const Double_t *py, const Double_t *dummy);
    static Double_t YBeautyF0M0S4PP(const Double_t *py, const Double_t *dummy);
    static Double_t YBeautyF0M0S5PP(const Double_t *py, const Double_t *dummy);
    static Double_t YBeautyF0M0S6PP(const Double_t *py, const Double_t *dummy);
    static Double_t PtBeautyCentral( const Double_t *px, const Double_t *dummy );
    static Int_t    IpBeauty(TRandom *ran);
//

   // Chi 1c 2c
   static Double_t PtChic0( const Double_t *px, const Double_t *dummy);
   static Double_t YChic0(const Double_t *py, const Double_t *dummy);
   static Int_t    IpChic0(TRandom *ran);

   static Double_t PtChic1( const Double_t *px, const Double_t *dummy);
   static Double_t YChic1(const Double_t *py, const Double_t *dummy);
   static Int_t    IpChic1(TRandom *ran);

   static Double_t PtChic2( const Double_t *px, const Double_t *dummy);
   static Double_t YChic2(const Double_t *py, const Double_t *dummy);
   static Int_t    IpChic2(TRandom *ran);

   static Double_t PtChic( const Double_t *px, const Double_t *dummy);
   static Double_t YChic(const Double_t *py, const Double_t *dummy);
   static Int_t    IpChic(TRandom *ran);

//

    
    static Float_t Interpolate(Float_t x, Float_t* y, Float_t x0, 
			Float_t dx,
			Int_t n, Int_t no);
    
    ClassDef(AliGenMUONlib,0) // Library providing y and pT parameterisations
};
#endif







