#ifndef ALIFASTMUONTRACKINGRES
#define ALIFASTMUONTRACKINGRES
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include "AliFastResponse.h"
class AliMUONFastTracking;

class AliFastMuonTrackingRes :  public AliFastResponse {
 public:
    AliFastMuonTrackingRes();
    ~AliFastMuonTrackingRes(){;}
    void SetBackground(Float_t bg = 1.) {fBackground = bg;}
    void SetCharge(Float_t charge = 1.) {fCharge     = charge;}
    virtual void Init();
    virtual void Evaluate(Float_t   p,  Float_t  theta , Float_t   phi,
			  Float_t& pS,  Float_t& thetaS, Float_t&  phiS);
 protected:
    Float_t              fBackground;   // Background level
    Float_t              fCharge;       // Current charge
    
    AliMUONFastTracking* fFastTracking; //!Pointer to Fast Tracking Data Handler
    ClassDef(AliFastMuonTrackingRes,1)  // Fast MUON Tracking 
};

#endif


