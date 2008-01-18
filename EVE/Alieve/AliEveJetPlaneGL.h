// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

#ifndef ALIEVE_JetPlaneGL_H
#define ALIEVE_JetPlaneGL_H

#include <TGLObject.h>

class TGLViewer;
class TGLScene;


class AliEveJetPlane;

class AliEveJetPlaneGL : public TGLObject
{
private:
  AliEveJetPlaneGL(const AliEveJetPlaneGL&);            // Not implemented
  AliEveJetPlaneGL& operator=(const AliEveJetPlaneGL&); // Not implemented

protected:
  AliEveJetPlane* fM; // fModel dynamic-casted to AliEveJetPlaneGL

  virtual void DirectDraw(TGLRnrCtx & rnrCtx) const;

public:
  AliEveJetPlaneGL();
  virtual ~AliEveJetPlaneGL();

  virtual Bool_t SetModel(TObject* obj, const Option_t* opt=0);
  virtual void   SetBBox();

  // To support two-level selection
  virtual Bool_t SupportsSecondarySelect() const { return kTRUE; }
  virtual void ProcessSelection(TGLRnrCtx & rnrCtx, TGLSelectRecord & rec);

  ClassDef(AliEveJetPlaneGL, 0);
}; // endclass AliEveJetPlaneGL

#endif
