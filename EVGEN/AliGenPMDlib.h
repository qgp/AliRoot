#ifndef ALIGENPMDLIB_H
#define ALIGENPMDLIB_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//
// Library of generators for PMD
// providing y and pt parametrisation
// for generated tracks
//

class TRandom;

#include "AliGenLib.h"

class AliGenPMDlib :public AliGenLib
{
 public:
     enum constants{kPion, kEta};
// Neutral pions
    static Double_t PtPi0(Double_t *px, Double_t *dummy);
    static Double_t PtScal(Double_t pt, Int_t np);
    static Double_t YPi0( Double_t *py, Double_t *dummy);
    static Int_t    IpPi0(TRandom *ran);
// Etas
    static Double_t PtEta(Double_t *px, Double_t *dummy);
    static Double_t YEta( Double_t *py, Double_t *dummy);
    static Int_t    IpEta(TRandom *ran);
//
    GenFunc   GetPt(Int_t param, const char* tname=0) const;
    GenFunc   GetY (Int_t param, const char* tname=0) const;
    GenFuncIp GetIp(Int_t param, const char* tname=0) const;    
    ClassDef(AliGenPMDlib,0) // Library providing y and pT parameterisations
};
#endif







