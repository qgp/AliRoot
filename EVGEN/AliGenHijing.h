#ifndef ALIGENHIJING_H
#define ALIGENHIJING_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

// Generator using HIJING as an external generator
// The main HIJING options are accessable for the user through this interface.
// andreas.morsch@cern.ch

#include "AliGenMC.h"
#include <TString.h>
#include <TArrayI.h>

class THijing;
class TArrayI;
class TParticle;
class TClonesArray;
class TGraph;

class AliGenHijing : public AliGenMC
{
    enum {kNoTrigger, kHardProcesses, kDirectPhotons};

 public:
    AliGenHijing();
    AliGenHijing(Int_t npart);
    AliGenHijing(const AliGenHijing &Hijing);
    virtual ~AliGenHijing();
    virtual void    Generate();
    virtual void    Init();
    // set centre of mass energy
    virtual void    SetEnergyCMS(Float_t energy=5500) {fEnergyCMS=energy;}
    virtual void    SetReferenceFrame(TString frame="CMS")
	{fFrame=frame;}
    virtual void    SetProjectile(TString proj="A", Int_t a=208, Int_t z=82)
	{fProjectile = proj; fAProjectile = a; fZProjectile = z;}    
    virtual void    SetTarget(TString tar="A", Int_t a=208, Int_t z=82)
	{fTarget = tar; fATarget = a; fZTarget = z;}    
    virtual void    SetImpactParameterRange(Float_t bmin = 0, Float_t bmax = 15.)
	{fMinImpactParam=bmin; fMaxImpactParam=bmax;}
    virtual void    KeepFullEvent();
    virtual void    SetJetQuenching(Int_t flag=1)     {fQuench     = flag;}
    virtual void    SetShadowing(Int_t flag=1)        {fShadowing  = flag;}
    virtual void    SetDecaysOff(Int_t flag=1)        {fDecaysOff  = flag;}
    virtual void    SetTrigger(Int_t flag=kNoTrigger) {fTrigger    = flag;}
    virtual void    SetFlavor(Int_t flag=0)           {fFlavor     = flag;}
    virtual void    SetEvaluate(Int_t flag=0)         {fEvaluate   = flag;}
    virtual void    SetSelectAll(Int_t flag=0)        {fSelectAll  = flag;}
    virtual void    SetRadiation(Int_t flag=3)        {fRadiation  = flag;}    
    virtual void    SetSpectators(Int_t spects=1)     {fSpectators = spects;}
    virtual void    SetPtJet(Float_t ptmin)           {fPtMinJet   = ptmin;}
    virtual void    SetSimpleJets(Int_t flag=0)       {fSimpleJet  = flag;}
    virtual void    SetNoGammas(Int_t flag=0)         {fNoGammas   = flag;}
	    
    virtual void    SetJetEtaRange(Float_t etamin = -20., Float_t etamax = 20.)
	{fEtaMinJet = etamin; fEtaMaxJet = etamax;}
    virtual void    SetJetPhiRange(Float_t phimin = -180., Float_t phimax = 180.)
	{fPhiMinJet = TMath::Pi()*phimin/180.; fPhiMaxJet = TMath::Pi()*phimax/180.;}
    virtual void    SetBoostLHC(Int_t flag = 0)         {fLHC        = flag;}
// Getters
    virtual Float_t GetEnergyCMS() {return fEnergyCMS;}
    virtual TString GetReferenceFrame() {return fFrame;}
    virtual void    GetProjectile(TString& tar, Int_t& a, Int_t& z)
	{tar = fProjectile; a = fAProjectile; z = fZProjectile;}    
    virtual void    GetTarget(TString& tar, Int_t& a, Int_t& z)
	{tar = fTarget; a = fATarget; z = fZTarget;}    
    virtual void    GetImpactParameterRange(Float_t& bmin, Float_t& bmax)
	{bmin = fMinImpactParam; bmax = fMaxImpactParam;}
    virtual Int_t   GetJetQuenching()                    {return fQuench;}
    virtual Int_t   GetShadowing()                       {return fShadowing;}
    virtual Int_t   GetTrigger(Int_t flag=kNoTrigger)    {return fTrigger;}
    virtual Int_t   GetFlavor(Int_t flag=0)              {return fFlavor;}
    virtual Int_t   GetRadiation(Int_t flag=3)           {return fRadiation;}    
    virtual Int_t   GetSpectators(Int_t spects=1)        {return fSpectators;}
    virtual Float_t GetPtJet(Float_t ptmin)              {return fPtMinJet;}
    virtual void    GetJetEtaRange(Float_t& etamin, Float_t& etamax)
	{etamin = fEtaMinJet; etamax = fEtaMaxJet;}
    virtual void    GetJetPhiRange(Float_t& phimin, Float_t& phimax)
	{phimin = fPhiMinJet*180./TMath::Pi(); phimax = fPhiMaxJet*180./TMath::Pi();}

// Physics Routines	    
    virtual void EvaluateCrossSections();
    virtual void Boost(TClonesArray* particles);
    virtual TGraph* CrossSection()     {return fDsigmaDb;}
    virtual TGraph* BinaryCollisions() {return fDnDb;}
    virtual Bool_t  CheckTrigger();
//
    AliGenHijing &  operator=(const AliGenHijing & rhs);
 protected:
    Bool_t SelectFlavor(Int_t pid);
    void   MakeHeader();

 protected:
    TString     fFrame;         // Reference frame 
    TString     fProjectile;    // Projectile
    TString     fTarget;        // Target
    Int_t       fAProjectile;    // Projectile A
    Int_t       fZProjectile;    // Projectile Z
    Int_t       fATarget;        // Target A
    Int_t       fZTarget;        // Target Z
    Float_t     fMinImpactParam; // minimum impact parameter
    Float_t     fMaxImpactParam; // maximum impact parameter	
    Int_t       fKeep;           // Flag to keep full event information
    Int_t       fQuench;         // Flag to switch on jet quenching
    Int_t       fShadowing;      // Flag to switch on voclear effects on parton distribution function
    Int_t       fDecaysOff;      // Flag to turn off decays of pi0, K_s, D, Lambda, sigma
    Int_t       fTrigger;        // Trigger type
    Int_t       fEvaluate;       // Evaluate total and partial cross-sections
    Int_t       fSelectAll;      // Flag to write the full event
    Int_t       fFlavor;         // Selected particle flavor 4: charm+beauty 5: beauty
    Float_t     fEnergyCMS;      // Centre of mass energy
    Float_t     fKineBias;       // Bias from kinematic selection
    Int_t       fTrials;         // Number of trials
    TArrayI     fParentSelect;   // Parent particles to be selected 
    TArrayI     fChildSelect;    // Decay products to be selected
    Float_t     fXsection;       // Cross-section
    THijing    *fHijing;         // Hijing
    Float_t     fPtHardMin;      // lower pT-hard cut 
    Float_t     fPtHardMax;      // higher pT-hard cut
    Int_t       fSpectators;     // put spectators on stack
    TGraph*     fDsigmaDb;       // dSigma/db for the system
    TGraph*     fDnDb;           // dNBinaryCollisions/db
    Float_t     fPtMinJet;       // Minimum Pt of triggered Jet
    Float_t     fEtaMinJet;      // Minimum eta of triggered Jet
    Float_t     fEtaMaxJet;      // Maximum eta of triggered Jet
    Float_t     fPhiMinJet;      // At least one of triggered Jets must be in this
    Float_t     fPhiMaxJet;      // phi range
    Int_t       fRadiation;      // Flag to switch on/off initial and final state radiation
    Int_t       fSimpleJet;      // Flag to produce simple tiggered jet topology
    Int_t       fNoGammas;       // Don't write gammas if flag "on"
    
// ZDC proposal (by Chiara) to store num. of SPECTATORS protons and neutrons
    Int_t 	fSpecn;		 // Num. of spectator neutrons
    Int_t 	fSpecp;		 // Num. of spectator protons
    Int_t       fLHC;            // Assume LHC as lab frame
    
 private:
    // adjust the weight from kinematic cuts
    void   AdjustWeights();
    // check seleted daughters
    Bool_t DaughtersSelection(TParticle* iparticle, TClonesArray* particles);
    // check if stable
    Bool_t Stable(TParticle*  particle);
    
    ClassDef(AliGenHijing,3) // AliGenerator interface to Hijing
};
#endif





