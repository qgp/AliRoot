/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

#include <Riostream.h>

#include <TMath.h>

#include "MUONChamberGL.h"

#include <Alieve/MUONChamber.h>
#include <Alieve/MUONChamberData.h>

#include <TEveQuadSetGL.h>
#include <TGLRnrCtx.h>
#include <TGLIncludes.h>
using namespace Alieve;

//______________________________________________________________________
// MUONChamberGL
//

ClassImp(MUONChamberGL)

//______________________________________________________________________
MUONChamberGL::MUONChamberGL() :
  TGLObject(),
  fChamber(0),
  fRTS(0)
{
  //
  // constructor
  //

}

//______________________________________________________________________
MUONChamberGL::~MUONChamberGL()
{
  //
  // destructor
  //

}

//______________________________________________________________________
Bool_t MUONChamberGL::SetModel(TObject* obj, const Option_t* /*opt*/)
{
  //
  // ...
  //

  if(SetModelCheckClass(obj, Alieve::MUONChamber::Class())) {
    
    fChamber = (MUONChamber*) fExternalObj;
    fQS1.SetModel(&fChamber->fQuadSet1);
    fQS2.SetModel(&fChamber->fQuadSet2);
    return kTRUE;

  }

  return kFALSE;

}

//______________________________________________________________________
void MUONChamberGL::SetBBox()
{
  //
  // ...
  //

  SetAxisAlignedBBox(((MUONChamber*)fExternalObj)->AssertBBox());

}

//______________________________________________________________________
void MUONChamberGL::DirectDraw(TGLRnrCtx& rnrCtx) const
{
  //
  // Actual GL drawing.
  //

  glDisable(GL_LIGHTING);

  //Double_t width = 10;
  //glOrtho(-width,+width,-width,+width,-width,+width);

  if(fRTS < fChamber->fRTS) {
    fChamber->UpdateQuads();
    fRTS = fChamber->fRTS;
  }
  
  Bool_t hasData = (fChamber->GetChamberData() != 0);
  
  if(hasData) {

    DrawQuads(rnrCtx);
    DrawPoints();
  
  }

  DrawChamberFrame();

}

//______________________________________________________________________
void MUONChamberGL::DrawQuads(TGLRnrCtx& rnrCtx) const
{
  //
  // draw the digits as GL_QUADS
  //

  glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);

  glDisable(GL_LIGHTING);
  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_CULL_FACE);

  //Float_t c[4]; glGetFloatv(GL_CURRENT_COLOR, c);

  glPolygonMode(GL_FRONT, GL_FILL);
  glPolygonMode(GL_BACK,  GL_LINE);

  fQS1.DirectDraw(rnrCtx);

  glPolygonMode(GL_FRONT, GL_LINE);
  glPolygonMode(GL_BACK,  GL_FILL);

  fQS2.DirectDraw(rnrCtx);

  glPopAttrib();

}

//______________________________________________________________________
void MUONChamberGL::DrawPoints() const
{
  //
  // draw the clusters as GL_QUADS
  //

  Float_t x, y, z;

  glDisable(GL_LIGHTING);
  glLineWidth(1.0);

  glColor3f(1.0,1.0,1.0);
    
  glBegin(GL_LINES);
    
  // clusters

  Int_t clsSize = fChamber->fClusterSize;

  if (clsSize > 1) {

    for (Int_t i = 0; i < fChamber->fPointSet1.GetN(); i++) {
      
      fChamber->fPointSet1.GetPoint(i,x,y,z);

      glVertex3f(x-clsSize,y+clsSize,z);
      glVertex3f(x+clsSize,y-clsSize,z);
      
      glVertex3f(x-clsSize,y-clsSize,z);
      glVertex3f(x+clsSize,y+clsSize,z);
      
    }
    
  }

  // hits

  Int_t hitSize = fChamber->fHitSize;

  if (hitSize > 1) {

    for (Int_t i = 0; i < fChamber->fPointSet2.GetN(); i++) {
      
      fChamber->fPointSet2.GetPoint(i,x,y,z);
      
      glVertex3f(x-hitSize,y,z);
      glVertex3f(x+hitSize,y,z);
      
      glVertex3f(x,y-hitSize,z);
      glVertex3f(x,y+hitSize,z);
      
    }
    
  }

  glEnd();
    
}

//______________________________________________________________________
void MUONChamberGL::DrawChamberFrame() const
{
  //
  // draw the chamber frame as GL_LINE_LOOP
  // 

  MUONChamberData* chamberData = fChamber->GetChamberData();
  Int_t nDetElem = chamberData->GetNDetElem();
  Float_t *frameCoord;
  Float_t xOrig, yOrig, xRad, yRad, x, y, z;

  UChar_t pix[4];
  pix[0] = 255;
  pix[1] =   0;
  pix[2] =   0;
  pix[3] = 255;

  glColor4ubv(pix);
  
  for (Int_t id = 0; id < nDetElem; id++) {

    frameCoord = chamberData->GetFrameCoord(id);

    if (fChamber->GetID() < 4) {

      xOrig = frameCoord[0];
      yOrig = frameCoord[1];
      xRad  = frameCoord[2];
      yRad  = frameCoord[3];
      z     = frameCoord[4];
      
      xRad += 0.0;
      yRad += 0.0;

      glBegin(GL_LINE_LOOP);

      glVertex3f(xOrig,yOrig,z);
      
      Int_t nstep = 100;
      Float_t dstep = TMath::Pi()/2.0 / (Float_t)nstep;
      Float_t d;
      for (Int_t istep = 0; istep < nstep; istep++) {

	d = istep * dstep;
	x = xOrig + xRad * TMath::Cos(d);
	y = yOrig + yRad * TMath::Sin(d);

	glVertex3f(x,y,z);

      }
      
      glVertex3f(xOrig,yOrig,z);

      glEnd();

    } else {

      glBegin(GL_LINE_LOOP);
      glVertex3f(frameCoord[0],frameCoord[1],frameCoord[4]);
      glVertex3f(frameCoord[0],frameCoord[3],frameCoord[4]);
      glVertex3f(frameCoord[2],frameCoord[3],frameCoord[4]);
      glVertex3f(frameCoord[2],frameCoord[1],frameCoord[4]);
      glVertex3f(frameCoord[0],frameCoord[1],frameCoord[4]);
      glEnd();

    }

  }

}
