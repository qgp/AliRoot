#ifndef ALIANAGAMMAPARTON_H
#define ALIANAGAMMAPARTON_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
/* $Id$ */

/* History of cvs commits:
 *
 * $Log$
 *
 */

//_________________________________________________________________________
// Class that contains the algorithm for the analysis of gamma-parton correlation 
//-- Author: Gustavo Conesa (INFN-LNF)

#include "AliAnaGammaCorrelation.h"
     
class AliAnaGammaParton : public AliAnaGammaCorrelation {
       
  public: 
       
       AliAnaGammaParton() ; // default ctor
       AliAnaGammaParton(const AliAnaGammaParton & g) ; // cpy ctor
       AliAnaGammaParton & operator = (const AliAnaGammaParton & g) ;//cpy assignment
       virtual ~AliAnaGammaParton() ; //virtual dtor
              
       TList * GetCreateOutputObjects();
       
       void InitParameters();
       
       void Print(const Option_t * opt) const;
       
       void MakeGammaCorrelation( TParticle * pGamma, TClonesArray *pl, TClonesArray *)  ;
       
  private:
       
       TH2F * fhDeltaEtaParton;
       TH2F * fhDeltaPhiParton;
       TH2F * fhDeltaPtParton;
       TH2F * fhPtRatParton;
       
       ClassDef(AliAnaGammaParton,0)
 } ;


#endif //ALIANAGAMMAPARTON_H



