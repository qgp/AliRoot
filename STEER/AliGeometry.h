#ifndef ALIGEOMETRY_H
#define ALIGEOMETRY_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

////////////////////////////////////////////////
//  AliGeometry Base Class pABC               //
//                                            //
//  Author Yves Schutz     SUBATECH           //
//                                            //  
//                                            //
////////////////////////////////////////////////

// --- ROOT system ---
#include "TNamed.h"

class AliRecPoint;
class TMatrix;
class TParticle;
class TVector3;

class AliGeometry : public TNamed {

public:

  AliGeometry() ;          // ctor            
  virtual ~AliGeometry() ; // dtor
 
  virtual void GetGlobal(const AliRecPoint * p, TVector3 & pos, TMatrix & mat) const = 0; 
  virtual void GetGlobal(const AliRecPoint * p, TVector3 & pos) const = 0; 
  virtual Bool_t Impact(const TParticle * particle) const             = 0;

protected:

  AliGeometry(const Text_t* name, const Text_t* title) : TNamed (name,title) {}                                   

  ClassDef(AliGeometry,1)  // Base class for detector geometry

};

#endif // ALIGEOMETRY_H



