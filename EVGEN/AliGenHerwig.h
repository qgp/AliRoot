#ifndef ALIGENHERWIG_H
#define ALIGENHERWIG_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

// Generator using HERWIG as an external generator
// The main HERWIG options are accessable for the user through this interface.


#include "AliGenMC.h"
#include <TString.h>
#include <TArrayI.h>
#include <AliRndm.h>
#include <AliStructFuncType.h>

class THerwig6;
class TArrayI;
class TParticle;
class TClonesArray;


class AliGenHerwig : public AliGenMC

{
    enum {kNoTrigger, kHardProcesses, kDirectPhotons};

 public:
    AliGenHerwig();
    AliGenHerwig(Int_t npart);
    AliGenHerwig(const AliGenHerwig &Herwig);
    virtual ~AliGenHerwig();
    virtual void    Generate();
    virtual void    Init();
    // set centre of mass energy
    virtual void    SetBeamMomenta(Float_t p1=7000., Float_t p2=7000.)
	{fMomentum1 = p1; fMomentum2 = p2;}
    virtual void    SetProjectile(TString proj="P")  {fProjectile = proj;}    
    virtual void    SetTarget(TString tar="P")       {fTarget = tar;}
    virtual void    SetProcess(Int_t proc)       {fProcess = proc;}    
    virtual void    KeepFullEvent();
    virtual void    SetDecaysOff(Int_t flag=1)        {fDecaysOff = flag;}
    virtual void    SetTrigger(Int_t flag=kNoTrigger) {fTrigger   = flag;}
    virtual void    SetFlavor(Int_t flag=0)           {fFlavor    = flag;}    
    virtual void    SetSelectAll(Int_t flag=0)        {fSelectAll = flag;}    
    AliGenHerwig &  operator=(const AliGenHerwig & rhs);
    virtual void    SetStrucFunc(StrucFunc_t func = kGRVHO) 
      {fStrucFunc = func;}
    virtual void    SetPtHardMin(Double_t pt) {fPtHardMin=pt;}
    virtual void    SetPtRMS(Double_t pt) {fPtRMS=pt;}
    virtual void    SetMaxPr(Int_t i) {fMaxPr=i;}
    virtual void    SetMaxErrors(Int_t i) {fMaxErrors=i;}
    virtual void    FinishRun();
    virtual void    SetEnSoft(Double_t e) {fEnSoft=e;}
 protected:
    Bool_t SelectFlavor(Int_t pid);

 protected:
    TString     fProjectile;     // Projectile
    TString     fTarget;         // Target
    TString     fAutPDF;         // PDF group
    Int_t       fModPDF;         // PDF set
    StrucFunc_t fStrucFunc;      //Structure Function
    Int_t       fKeep;           // Flag to keep full event information
    Int_t       fDecaysOff;      // Flag to turn off decays of pi0, K_s, D, Lambda, sigma
    Int_t       fTrigger;        // Trigger type
    Int_t       fSelectAll;      // Flag to write the full event
    Int_t       fFlavor;         // Selected particle flavor 4: charm+beauty 5: beauty
    Float_t     fEnergyCMS;      // Centre of mass energy
    Float_t     fMomentum1;      // Momentum of projectile
    Float_t     fMomentum2;      // Momentum of target
    Float_t     fKineBias;       // Bias from kinematic selection
    Int_t       fTrials;         // Number of trials
    TArrayI     fParentSelect;   // Parent particles to be selected 
    TArrayI     fChildSelect;    // Decay products to be selected
    Float_t     fXsection;       // Cross-section
    THerwig6    *fHerwig;        // Herwig
    Int_t       fProcess;        // Process number
    Double_t    fPtHardMin;      // lower pT-hard cut 
    Double_t    fPtRMS;          // intrinsic pt of incoming hadrons
    Int_t       fMaxPr;          // maximum number of events to print out
    Int_t       fMaxErrors;      // maximum number of errors allowed
    Double_t    fEnSoft;          // change on soft energy distribution
      
 private:
    // check if particle is selected as parent particle
    Bool_t ParentSelected(Int_t ip);
    // check if particle is selected as child particle
    Bool_t ChildSelected(Int_t ip);
    // adjust the weight from kinematic cuts
    void   AdjustWeights();
    // check seleted daughters
    Bool_t DaughtersSelection(TParticle* iparticle, TClonesArray* particles);
    // check if stable
    Bool_t Stable(TParticle*  particle);
    
    void InitPDF();

    ClassDef(AliGenHerwig,1) // AliGenerator interface to Herwig
};
#endif





