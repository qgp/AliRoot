//
// Class AliRsnMother
//
// Implementation of a pair of tracks, for several purposes
// - computing the total 4-momentum & inv. mass for output histos filling
// - evaluating cut checks on the pair of particles
// - evaluating any kind of kinematic value over their sum
//
// authors: Martin Vala (martin.vala@cern.ch)
//          Alberto Pulvirenti (alberto.pulvirenti@ct.infn.it)
//

#ifndef ALIRSNMOTHER_H
#define ALIRSNMOTHER_H

#include <TLorentzVector.h>

#include "AliRsnEvent.h"
#include "AliRsnDaughter.h"

class AliRsnPairDef;

class AliRsnMother : public TObject
{
  public:

    AliRsnMother();
    AliRsnMother(const AliRsnMother &obj);
    AliRsnMother& operator=(const AliRsnMother &obj);
    virtual ~AliRsnMother();
    
    void              SetDefaultMass(Double_t mass) {fDefaultMass = mass; fRef.SetXYZM(fSum.X(),fSum.Y(),fSum.Z(),mass); fRefMC.SetXYZM(fSumMC.X(),fSumMC.Y(),fSumMC.Z(),mass);}
    TLorentzVector&   Sum()   {return fSum;}
    TLorentzVector&   Ref()   {return fRef;}
    TLorentzVector&   SumMC() {return fSumMC;}
    TLorentzVector&   RefMC() {return fRefMC;}
    Double_t          OpeningAngle(Bool_t mc = kFALSE) const {if (fDaughter[0] && fDaughter[1]) return fDaughter[0]->P(mc).Angle(fDaughter[1]->P(mc).Vect()); return 1E6;}
    Double_t          AngleTo(AliRsnDaughter track, Bool_t mc = kFALSE) const {return fSum.Angle(track.P(mc).Vect());}
    Double_t          CosThetaStar(Bool_t first = kTRUE, Bool_t useMC = kFALSE);

    AliRsnDaughter*   GetDaughter   (const Int_t &index) const {if (index==0||index==1) return fDaughter[index]; return 0x0;}
    AliRsnDaughter&   GetDaughterRef(const Int_t &index) const {if (index==1) return (*fDaughter[1]); return (*fDaughter[0]);}

    Bool_t            IsLabelEqual() const {return abs(fDaughter[0]->GetLabel()) == abs(fDaughter[1]->GetLabel());}
    Bool_t            IsIndexEqual() const {return (fDaughter[0]->GetID() == fDaughter[1]->GetID());}
    Int_t             CommonMother(Int_t &m0, Int_t &m1) const;
    Int_t             CommonMother() const {Int_t d0, d1; return CommonMother(d0,d1);}

    void              SetDaughters(AliRsnDaughter * const daughter1, Double_t mass1, AliRsnDaughter * const daughter2, Double_t mass2);
    void              ResetPair();
    void              PrintInfo(const Option_t *option = "ALL") const;
    Bool_t            CheckPair() const;
    Bool_t            MatchesDef(AliRsnPairDef *pairDef);

  private:

    Bool_t           fUseMC;            // choose if momenta are taken from ESD/AOD or MC
    Double_t         fDefaultMass;      // nominal resonance mass
    AliRsnDaughter  *fDaughter[2];      // elements of the pair
    TLorentzVector   fSum;              // sum computed from the two daughters
    TLorentzVector   fSumMC;            // sum computed from the two daughters
    TLorentzVector   fRef;              // a 4-vector with same momentum as the sum but the nominal mass of mother
    TLorentzVector   fRefMC;            // a 4-vector with same momentum as the sum but the nominal mass of mother

    ClassDef(AliRsnMother,1)
};

#endif
