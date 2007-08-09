// $Header$

#include "NLTProjectorGL.h"
#include <Reve/NLTProjector.h>

#include <TGLRnrCtx.h>
#include <TGLIncludes.h>
#include <TGLText.h>

#include <list>

using namespace Reve;

//______________________________________________________________________
// NLTProjectorGL
//

ClassImp(NLTProjectorGL)

NLTProjectorGL::NLTProjectorGL() : TGLObject(), 
                                   fSplitLevel(1),
				   fSplitByVal(kTRUE),
				   fM(0), fText(0)
{
  fDLCache = kFALSE; // Disable display list.
  fText = new TGLText();
  fText->SetGLTextFont(40);
  fText->SetTextColor(0);
}

NLTProjectorGL::~NLTProjectorGL()
{}

/**************************************************************************/

Bool_t NLTProjectorGL::SetModel(TObject* obj, const Option_t* /*opt*/)
{
  if(SetModelCheckClass(obj, NLTProjector::Class())) {
    fM = dynamic_cast<NLTProjector*>(obj);
    return kTRUE;
  }
  return kFALSE;
}

void NLTProjectorGL::SetBBox()
{
  // !! This ok if master sub-classed from TAttBBox
  SetAxisAlignedBBox(((NLTProjector*)fExternalObj)->AssertBBox());
}

/**************************************************************************/
void NLTProjectorGL::SplitInterval(Int_t ax) const
{
  
  if(fSplitByVal)
  {
    SplitIntervalByVal(fVals.front(), fVals.back(), ax, 0);
  }
  else
    SplitIntervalByPos(fPos.front(), fPos.back(), ax, 0);
}
/**************************************************************************/
void NLTProjectorGL::SplitIntervalByPos(Float_t minp, Float_t maxp, Int_t ax, Int_t level) const
{
  Float_t p = (minp+maxp)*0.5;
  Float_t v = fM->GetProjection()->PositionToValue(p, ax);
  fPos.push_back(p);
  fVals.push_back(v);
  // printf("level %d position %f value %f\n", level, p,v);
  level++;
  if(level<fSplitLevel)
  {
    SplitIntervalByPos(minp, p , ax, level);
    SplitIntervalByPos(p, maxp, ax, level);
  }
}

/**************************************************************************/
void NLTProjectorGL::SplitIntervalByVal(Float_t minv, Float_t maxv, Int_t ax, Int_t level) const
{
  Float_t v = (minv+maxv)*0.5;
  Float_t p = fM->GetProjection()->ValueToPosition(v, ax);

  fPos.push_back(p);
  fVals.push_back(v);
  // printf("level %d position %f value %f MINMAX val(%f, %f)\n", level, p,v, minv, maxv);
  level++;
  if(level<fSplitLevel)
  {
    SplitIntervalByVal(minv, v , ax, level);
    SplitIntervalByVal(v, maxv, ax, level);
  }
}

/**************************************************************************/
void NLTProjectorGL::DirectDraw(TGLRnrCtx & /*rnrCtx*/) const
{
  //printf("NLTProjectorGL::DirectDraw Style %d, LOD %d\n", rnrCtx.ViewerStyle(), rnrCtx.ViewerLOD());
  Float_t* bbox = fM->GetBBox();
  Float_t scale=0.8;
  Float_t labelsSize      = 0.018*scale;
  Float_t tickMarksLength = 0.02*scale;
  Float_t labelsOffset    = 0.035*scale;

  // text length variables from bbox
  Float_t llx, lly, llz, urx, ury, urz;
  const char* txt;
  Float_t axl; // axis length
  Float_t tms; // tick mark size
  Float_t loff; // label offset

  GLboolean lightp;
  glGetBooleanv(GL_LIGHTING, &lightp);
  if (lightp) glDisable(GL_LIGHTING);
  // horizontal axis
  {
    glPushMatrix();
    glTranslatef(0, bbox[2], 0);

    axl = bbox[1] -bbox[0];
    tms = axl*tickMarksLength;
    loff = axl*labelsOffset;
    fText->SetTextSize(labelsSize*axl);

    // body
    glBegin(GL_LINES);
    glVertex3f(bbox[0], 0., 0.);
    glVertex3f(bbox[1], 0,  0.);
    glEnd();

    // left
    {
      glPushMatrix();
      fPos.clear(); 
      fVals.clear();
      fPos.push_back(bbox[0]); 
      fPos.push_back(0);
      fVals.push_back(fM->GetProjection()->PositionToValue(bbox[0], 0)); 
      fVals.push_back(fM->GetProjection()->PositionToValue(0, 0));
      // printf("left horizontal :: pos %f, val %f\n", bbox[0], fM->GetProjection()->PositionToValue(bbox[0],0));
      SplitInterval(0);
      {      
	// tickmarks
	glBegin(GL_LINES);
	for( std::list<Float_t>::iterator pi = fPos.begin(); pi!= fPos.end(); pi++)
	{
	  glVertex3f(*pi, 0, 0.);
	  glVertex3f(*pi, -tms, 0.);
	}
	glEnd();
      }
      // labels
      glRotatef(-90, 1, 0, 0);
      glTranslatef(0, 0, -tms -loff);
      std::list<Float_t>::iterator vi = fVals.begin();
      for( std::list<Float_t>::iterator pi = fPos.begin(); pi!= fPos.end(); pi++)
      {
	txt = Form("%4.0f", *vi );
	fText->BBox(txt, llx, lly, llz, urx, ury, urz);
	fText->PaintGLText(*pi -(urx-llx)*fText->GetTextSize()*0.5, 0, 0, txt);
	vi++;
      }
      glPopMatrix();
    }
    // right
    {
      glPushMatrix();
      fPos.clear(); 
      fVals.clear();
      fPos.push_back(0); 
      fPos.push_back(bbox[1]);
      fVals.push_back(fM->GetProjection()->PositionToValue(0, 0)); 
      fVals.push_back(fM->GetProjection()->PositionToValue(bbox[1], 0));
      SplitInterval(0);
      fVals.pop_front();
      fPos.pop_front();
      {      
	// tickmarks
	glBegin(GL_LINES);
	for( std::list<Float_t>::iterator pi = fPos.begin(); pi!= fPos.end(); pi++)
	{
	  glVertex3f(*pi, 0, 0.);
	  glVertex3f(*pi, -tms, 0.);
	}
	glEnd();
      }
      // labels
      glRotatef(-90, 1, 0, 0);
      glTranslatef(0, 0, -tms -loff);
      std::list<Float_t>::iterator vi = fVals.begin();
      for( std::list<Float_t>::iterator pi = fPos.begin(); pi!= fPos.end(); pi++)
      {
	txt = Form("%4.0f", *vi );
	fText->BBox(txt, llx, lly, llz, urx, ury, urz);
	fText->PaintGLText(*pi -(urx-llx)*fText->GetTextSize()*0.5, 0, 0, txt);
	vi++;
      }
      glPopMatrix();
    }

    glPopMatrix();
  }

  // vertical axis
  {
    glPushMatrix();
    glTranslatef(bbox[0], 0, 0);
    glRotatef(90, 0, 0, 1);

    axl = bbox[3] -bbox[2];
    tms = axl*tickMarksLength;
    loff = axl*labelsOffset;
    fText->SetTextSize(labelsSize*axl);
    // body
    glBegin(GL_LINES);  
    glVertex3f(bbox[2],0., 0.);
    glVertex3f( bbox[3], 0, 0.);
    glEnd();

    // bottom
    if (kTRUE) {
      glPushMatrix();
      fPos.clear(); 
      fVals.clear();
      fPos.push_back(bbox[2]); 
      fPos.push_back(0);
      fVals.push_back(fM->GetProjection()->PositionToValue(bbox[2], 1)); 
      fVals.push_back(fM->GetProjection()->PositionToValue(0, 1));
      SplitInterval(1);
      {
	glBegin(GL_LINES);  
	for( std::list<Float_t>::iterator pi = fPos.begin(); pi!= fPos.end(); pi++)
	{
	  glVertex3f(*pi, 0, 0.);
	  glVertex3f(*pi, tms, 0.);
	}
	glEnd();
      }
      // labels
      glRotatef(-90, 1, 0, 0);
      glTranslatef(0, 0, loff);
      std::list<Float_t>::iterator vi = fVals.begin();
      for( std::list<Float_t>::iterator pi = fPos.begin(); pi!= fPos.end(); pi++)
      {
	glPushMatrix();
	txt= Form("%4.0f", *vi );
	fText->BBox(txt, llx, lly, llz, urx, ury, urz);
	glTranslatef(*pi,  -(ury-lly)*fText->GetTextSize()*0.5, 0);
	glRotatef(90, 0, 1, 0);
	fText->PaintGLText(-(urx-llx)*fText->GetTextSize()*0.5 -tms -loff, 0, 0, txt);
	vi++;
	glPopMatrix();
      }
      glPopMatrix();
    }
    // top
    {
      glPushMatrix();
      fPos.clear(); 
      fVals.clear();
      fPos.push_back(0); 
      fPos.push_back(bbox[3]);
      fVals.push_back(fM->GetProjection()->PositionToValue(0, 1)); 
      fVals.push_back(fM->GetProjection()->PositionToValue(bbox[3], 1));
      SplitInterval( 1);
      fPos.pop_front();
      fVals.pop_front();
      {
	glBegin(GL_LINES);  
	for( std::list<Float_t>::iterator pi = fPos.begin(); pi!= fPos.end(); pi++)
	{
	  glVertex3f(*pi, 0, 0.);
	  glVertex3f(*pi, tms, 0.);
	}
	glEnd();
      }
      // labels
      glRotatef(-90, 1, 0, 0);
      glTranslatef(0, 0, loff);
      std::list<Float_t>::iterator vi = fVals.begin();
      for( std::list<Float_t>::iterator pi = fPos.begin(); pi!= fPos.end(); pi++)
      {
	glPushMatrix();
	txt= Form("%4.0f", *vi );
	fText->BBox(txt, llx, lly, llz, urx, ury, urz);
	glTranslatef(*pi,  -(ury-lly)*fText->GetTextSize()*0.5, 0);
	glRotatef(90, 0, 1, 0);
	fText->PaintGLText(-(urx-llx)*fText->GetTextSize()*0.5 -tms -loff, 0, 0, txt);
	vi++;
	glPopMatrix();
      }
      glPopMatrix();
    }
    glPopMatrix();
  }
  if (lightp) glEnable(GL_LIGHTING);
}
