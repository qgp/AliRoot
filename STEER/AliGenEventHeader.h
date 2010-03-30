#ifndef ALIGENEVENTHEADER_H
#define ALIGENEVENTHEADER_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//---------------------------------------------------------------------
// Event header base class for generator. 
// Stores generated event information
// Author: andreas.morsch@cern.ch
//---------------------------------------------------------------------

#include <TNamed.h>
#include <TArrayF.h>

class AliGenEventHeader : public TNamed
{
 public:

  AliGenEventHeader(const char* name);
  AliGenEventHeader();
  virtual ~AliGenEventHeader() {}
  // Getters
  virtual Int_t           NProduced()  const       {return fNProduced;}
  virtual void            PrimaryVertex(TArrayF &o) const;
  virtual Float_t         InteractionTime() const  {return fInteractionTime;}
  // Setters
  virtual void   SetNProduced(Int_t nprod)         {fNProduced=nprod;}
  virtual void   SetPrimaryVertex(const TArrayF &o);
  virtual void   SetInteractionTime(Float_t t)     {fInteractionTime = t;}
        
	  
protected:
  Int_t     fNProduced;                 // Number stable or undecayed particles
  TArrayF   fVertex;                    // Primary Vertex Position
  Float_t   fInteractionTime;           // Time of the interaction
  
  ClassDef(AliGenEventHeader, 3)        // Event header for primary event
};

#endif
