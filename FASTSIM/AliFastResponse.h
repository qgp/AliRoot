#ifndef ALIFASTRESPONSE_H
#define ALIFASTRESPONSE_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include <TNamed.h>
class AliFastParticle;

class AliFastResponse : public TNamed {
 public:
    AliFastResponse(){;}
    AliFastResponse(char* Name, char* Title) : TNamed(Name, Title) {}
    virtual ~AliFastResponse(){}
    virtual void    Init()                                                  = 0;
    virtual Float_t Evaluate(Float_t  pt,  Float_t  theta , Float_t   phi)
	{return -1.;}
    virtual void    Evaluate(Float_t   p,  Float_t  theta , Float_t   phi,
			     Float_t& pS,  Float_t& thetaS, Float_t&  phiS);
    virtual Float_t Evaluate(AliFastParticle* part);
 protected:
    ClassDef(AliFastResponse,1)    // Base class for fast response
};

#endif
