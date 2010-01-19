#ifndef ALIDNDPTACCEPTANCECUTS_H
#define ALIDNDPTACCEPTANCECUTS_H

//------------------------------------------------------------------------------
// Class to keep selection cuts for MC tracks. 
// 
// Author: J.Otwinowski 03/11/2008 
//------------------------------------------------------------------------------

class TParticle;
class AliESDtrack;
class AliExternalTrackParam;

#include "AliAnalysisCuts.h"

class AlidNdPtAcceptanceCuts : public AliAnalysisCuts
{
public:
  AlidNdPtAcceptanceCuts(const Char_t* name ="AlidNdPtAcceptanceCuts", const Char_t *title ="");
  virtual ~AlidNdPtAcceptanceCuts(); 
 
  // setters 
  void SetEtaRange(const Float_t min=0., const Float_t max=1e99)  { fMinEta=min; fMaxEta=max; }
  void SetPhiRange(const Float_t min=0., const Float_t max=1e99)  { fMinPhi=min; fMaxPhi=max;}
  void SetPtRange(const Float_t min=0., const Float_t max=1e99)   { fMinPt=min;  fMaxPt=max;}

  void SetMaxDCAr(const Float_t max=1e99) { fMaxDCAr=max;}
  void SetMaxDCAz(const Float_t max=1e99) { fMaxDCAz=max;}

  // getters 
  Float_t GetMinEta() const {return fMinEta;}
  Float_t GetMaxEta() const {return fMaxEta;}
  Float_t GetMinPhi() const {return fMinPhi;}
  Float_t GetMaxPhi() const {return fMaxPhi;}
  Float_t GetMinPt() const {return fMinPt;}
  Float_t GetMaxPt() const {return fMaxPt;}

  Float_t GetMaxDCAr() const {return fMaxDCAr;}
  Float_t GetMaxDCAz() const {return fMaxDCAz;}

  // cuts init function
  void Init();

  // check MC tracks
  virtual Bool_t IsSelected(TObject *, TObject * /* event */ = 0) {return kTRUE;}
  //virtual Bool_t IsSelected(TList *) {return kTRUE;}

  //
  Bool_t AcceptTrack(AliESDtrack *track);
  Bool_t AcceptTrack(AliExternalTrackParam *track);
  Bool_t AcceptTrack(TParticle *particle);
  
  // Merge output objects (needed by PROOF) 
  virtual Long64_t Merge(TCollection* list);

private:
  Float_t fMinEta; // min pseudorapidity 
  Float_t fMaxEta; // max pseudorapidity
  Float_t fMinPhi; // min azimuthal angle (rad)
  Float_t fMaxPhi; // max azimuthal angle (rad)
  Float_t fMinPt;  // min pt
  Float_t fMaxPt;  // max pt

  // max DCAr and DCAz with respect
  // to nominal vertex position
  Float_t fMaxDCAr; // min DCAr
  Float_t fMaxDCAz; // max DCAz
 
  AlidNdPtAcceptanceCuts(const AlidNdPtAcceptanceCuts&); // not implemented
  AlidNdPtAcceptanceCuts& operator=(const AlidNdPtAcceptanceCuts&); // not implemented

  ClassDef(AlidNdPtAcceptanceCuts, 1)
};

#endif // 
