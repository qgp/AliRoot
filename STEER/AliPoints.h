#ifndef ALIPOINTS_H
#define ALIPOINTS_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include "TPolyMarker3D.h"
class AliDetector;
class TParticle;

class AliPoints : public TPolyMarker3D {
public:
  AliPoints();
  AliPoints(const AliPoints& pts);
  AliPoints(Int_t nhits);
  virtual ~AliPoints();
  virtual Int_t         DistancetoPrimitive(Int_t px, Int_t py);
  virtual void          ExecuteEvent(Int_t event, Int_t px, Int_t py);
  AliDetector          *GetDetector() const {return fDetector;}
  Int_t                 GetIndex() const {return fIndex;}
  TParticle            *GetParticle() const;
  virtual const Text_t *GetName() const;
  virtual void          InspectParticle(); // *MENU*
  virtual void          DumpParticle(); // *MENU*
  virtual Text_t       *GetObjectInfo(Int_t px, Int_t py) const;
  AliPoints &           operator=(const AliPoints &pts)
    {pts.Copy(*this); return (*this);}
  virtual void          Propagate(); // *MENU*
  virtual void          SetDetector(AliDetector *det) {fDetector = det;}
  virtual void          SetParticle(Int_t index) {fIndex = index;}
  
protected:
  void Copy(AliPoints &pts) const;

  AliDetector     *fDetector;    //Pointer to AliDetector object
  Int_t            fIndex;       //Particle number in AliRun::fParticles

  ClassDef(AliPoints,1) //Class to draw detector hits (is PolyMarker3D)
};
#endif
