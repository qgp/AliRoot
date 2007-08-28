// $Header$

#include "FrameBoxGL.h"
#include <Reve/FrameBox.h>

#include <TGLIncludes.h>

using namespace Reve;

//______________________________________________________________________
// FrameBoxGL
//
// A class encapsulating GL rendering of Reve::FrameBox via a static
// meber function.

ClassImp(FrameBoxGL)

void FrameBoxGL::Render(const FrameBox* box)
{
  GLboolean lightp, cullp;
  glGetBooleanv(GL_LIGHTING, &lightp);
  if (lightp) glDisable(GL_LIGHTING);
  glGetBooleanv(GL_CULL_FACE, &cullp);
  if (cullp)  glDisable(GL_CULL_FACE);

  const FrameBox& b = *box;
  const Float_t*  p =  b.fFramePoints;

  if (b.fFrameFill)
  {
    glPushAttrib(GL_POLYGON_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  glColor4ubv(b.fFrameRGBA);
  if (b.fFrameType == FrameBox::FT_Quad)
  {
    glBegin(b.fFrameFill ? GL_POLYGON : GL_LINE_LOOP);
    glVertex3fv(p);       glVertex3fv(p + 3);
    glVertex3fv(p + 6);   glVertex3fv(p + 9);
    glEnd();
  }
  else if (b.fFrameType == FrameBox::FT_Box)
  {
    // !!! frame-fill not implemented.
    glBegin(GL_LINE_STRIP);
    glVertex3fv(p);       glVertex3fv(p + 3);
    glVertex3fv(p + 6);   glVertex3fv(p + 9);
    glVertex3fv(p);
    glVertex3fv(p + 12);  glVertex3fv(p + 15);
    glVertex3fv(p + 18);  glVertex3fv(p + 21);
    glVertex3fv(p + 12);
    glEnd();
    glBegin(GL_LINES);
    glVertex3fv(p + 3);   glVertex3fv(p + 15);
    glVertex3fv(p + 6);   glVertex3fv(p + 18);
    glVertex3fv(p + 9);   glVertex3fv(p + 21);
    glEnd();
  }

  if (b.fFrameFill) glPopAttrib();
  if (lightp)       glEnable(GL_LIGHTING);
  if (cullp)        glEnable(GL_CULL_FACE);
}
