#ifndef ALIANAGAMMADIRECT_H
#define ALIANAGAMMADIRECT_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
/* $Id$ */

/* History of cvs commits:
 *
 * $Log$
 * Revision 1.4  2007/03/15 11:47:39  schutz
 * Methods added
 *
 * Revision 1.3  2007/03/08 10:24:32  schutz
 * Coding convention
 *
 * Revision 1.2  2007/02/09 18:40:40  schutz
 * New version from Gustavo
 *
 * Revision 1.1  2007/01/23 17:17:29  schutz
 * New Gamma package
 *
 *
 */

//_________________________________________________________________________

// Class for the analysis of prompt direct photons with the isolation cut analysis
//
//*-- Author: Gustavo Conesa (INFN-LNF)

// --- ROOT system ---
#include <TParticle.h> 
#include <TClonesArray.h> 
#include <TTree.h> 
#include "AliAnalysisTask.h" 
#include <TH2F.h>
 
class AliAnaGammaDirect : public AliAnalysisTask {

public: 

  AliAnaGammaDirect(const char *name) ; // default ctor
  AliAnaGammaDirect(const AliAnaGammaDirect & g) ; // cpy ctor
  AliAnaGammaDirect & operator = (const AliAnaGammaDirect & g) ;//cpy assignment
  virtual ~AliAnaGammaDirect() ; //virtual dtor

  virtual void Exec(Option_t * opt = "") ;
  virtual void ConnectInputData(Option_t *);
  virtual void CreateOutputObjects();
  virtual void Terminate(Option_t * opt = "");
  
  void InitParameters();

  Double_t  GetMinGammaPt()    const {return fMinGammaPt ; }
  Float_t     GetConeSize()          const {return fConeSize ; }
  Float_t     GetPtThreshold()      const {return fPtThreshold ; }
  Float_t     GetPtSumThres()     const {return fPtSumThreshold ; }
  Int_t        GetICMethod()          const {return fMakeICMethod ; }

  void Print(const Option_t * opt)const;

  void SetMinGammaPt(Double_t ptcut){fMinGammaPt =ptcut;}
  void SetConeSize(Float_t r)              {fConeSize = r ; }
  void SetPtThreshold(Float_t pt)        {fPtThreshold = pt; };
  void SetPtSumThreshold(Float_t pt) {fPtSumThreshold = pt; };
  void SetICMethod(Int_t i )          {fMakeICMethod = i ; }
  
  void GetPromptGamma(TClonesArray * plNe, TClonesArray * plCTS, TParticle * pGamma, Bool_t &Is)  const;
  
  void MakeIsolationCut(TClonesArray * plCTS, TClonesArray * plNe, 
			TParticle *pCandidate, Int_t index, 
			Bool_t &imcpt, Bool_t &icms, Float_t &ptsum) const ;  
  
 private:

  TClonesArray * fCaloList; //!input clusters
  TClonesArray * fCTSList; //!input tracks
  TList  *fOutputContainer ; //! output data container with histograms
  TParticle * fDirectGamma; //! output direct photon 
  
  Double_t    fMinGammaPt ;  // Min pt in Calorimeter
  Float_t      fConeSize ; //Size of the isolation cone 
  Float_t      fPtThreshold ; //Mimium pt of the particles in the cone to set isolation
  Float_t      fPtSumThreshold ; //Mimium pt sum of the particles in the cone to set isolation  
  Int_t        fMakeICMethod ; //Isolation cut method to be used
                                           // 0: No isolation
                                           // 1: Pt threshold method
                                           // 2: Cone pt sum method
  //Histograms  
  TH1F * fhNGamma    ; 
  TH2F * fhPhiGamma    ; 
  TH2F * fhEtaGamma    ; 

  ClassDef(AliAnaGammaDirect,0)
} ;
 

#endif //ALIANAGAMMADIRECT_H



