// $Header$

#include "BoxSetGL.h"
#include <Reve/BoxSet.h>

#include <TGLIncludes.h>
#include <TGLRnrCtx.h>
#include <TGLScene.h>
#include <TGLSelectRecord.h>
#include <TGLContext.h>

using namespace Reve;

//______________________________________________________________________
// BoxSetGL
//

ClassImp(BoxSetGL)

BoxSetGL::BoxSetGL() : fM(0), fBoxDL(0)
{
  // fDLCache = false; // Disable display list.
}

BoxSetGL::~BoxSetGL()
{}

/**************************************************************************/
// Protected methods
/**************************************************************************/

Int_t BoxSetGL::PrimitiveType() const
{
  return (fM->fRenderMode != DigitSet::RM_Line) ? GL_QUADS : GL_LINE_LOOP;
}

inline Bool_t BoxSetGL::SetupColor(const DigitSet::DigitBase& q) const
{
  if (fM->fValueIsColor)
  {
    glColor4ubv((UChar_t*) & q.fValue);
    return kTRUE;
  }
  else
  {
    UChar_t c[4];
    Bool_t visible = fM->fPalette->ColorFromValue(q.fValue, fM->fDefaultValue, c);
    if (visible)
      glColor4ubv(c);
    return visible;
  }
}

void BoxSetGL::MakeOriginBox(Float_t* p, Float_t dx, Float_t dy, Float_t dz) const
{
  // bottom
  p[0] = 0;  p[1] = dy; p[2] = 0;  p += 3;
  p[0] = dx; p[1] = dy; p[2] = 0;  p += 3;
  p[0] = dx; p[1] = 0;  p[2] = 0;  p += 3;
  p[0] = 0;  p[1] = 0;  p[2] = 0;  p += 3;
  // top
  p[0] = 0;  p[1] = dy; p[2] = dz; p += 3;
  p[0] = dx; p[1] = dy; p[2] = dz; p += 3;
  p[0] = dx; p[1] = 0;  p[2] = dz; p += 3;
  p[0] = 0;  p[1] = 0;  p[2] = dz;
}

inline void BoxSetGL::RenderBox(const Float_t* p) const
{
  // bottom: 3210
  glNormal3f(0, 0, -1);
  glVertex3fv(p + 9);  glVertex3fv(p + 6);
  glVertex3fv(p + 3);  glVertex3fv(p);
  // top:   4567
  glNormal3f(0, 0, 1);
  glVertex3fv(p + 12); glVertex3fv(p + 15);
  glVertex3fv(p + 18); glVertex3fv(p + 21);
  // front: 0154
  glNormal3f(0, -1, 0);
  glVertex3fv(p);      glVertex3fv(p + 3);
  glVertex3fv(p + 15); glVertex3fv(p + 12);
  // back:  7623
  glNormal3f(0, 1, 0);
  glVertex3fv(p + 21); glVertex3fv(p + 18);
  glVertex3fv(p + 6);  glVertex3fv(p + 9);
  // left:  4730
  glNormal3f(-1, 0, 0);
  glVertex3fv(p + 12); glVertex3fv(p + 21);
  glVertex3fv(p + 9);  glVertex3fv(p);
  // right: 5126
  glNormal3f(1, 0, 0);
  glVertex3fv(p + 15); glVertex3fv(p + 3);
  glVertex3fv(p + 6);  glVertex3fv(p + 18);
}

void BoxSetGL::MakeDisplayList() const
{
  if (fM->fBoxType == BoxSet::BT_AABox ||
      fM->fBoxType == BoxSet::BT_AABoxFixedDim)
  {
    if (fBoxDL == 0)
      fBoxDL = glGenLists(1);

    Float_t p[24];
    if (fM->fBoxType == BoxSet::BT_AABox)
      MakeOriginBox(p, 1.0f, 1.0f, 1.0f);
    else
      MakeOriginBox(p, fM->fDefWidth, fM->fDefHeight, fM->fDefDepth);

    glNewList(fBoxDL, GL_COMPILE);
    glBegin(PrimitiveType());
    RenderBox(p);
    glEnd();
    glEndList();
  }
}

/**************************************************************************/
// Virtuals from base-classes
/**************************************************************************/

Bool_t BoxSetGL::ShouldDLCache(const TGLRnrCtx & rnrCtx) const
{
  MakeDisplayList();

  if (rnrCtx.DrawPass() == TGLRnrCtx::kPassOutlineLine)
    return kFALSE;
  return TGLObject::ShouldDLCache(rnrCtx);
}

void BoxSetGL::DLCacheDrop()
{
  fBoxDL = 0;
  TGLObject::DLCacheDrop();
}

void BoxSetGL::DLCachePurge()
{
  static const Exc_t eH("BoxSetGL::DLCachePurge ");

  if (fBoxDL == 0) return;
  if (fScene)
  {
    fScene->GetGLCtxIdentity()->RegisterDLNameRangeToWipe(fBoxDL, 1);
  }
  else
  {
    Warning(eH, "Scene unknown, attempting direct deletion.");
    glDeleteLists(fBoxDL, 1);
  }
  TGLObject::DLCachePurge();
}

/**************************************************************************/

Bool_t BoxSetGL::SetModel(TObject* obj, const Option_t* /*opt*/)
{
  Bool_t isok = SetModelCheckClass(obj, Reve::BoxSet::Class());
  fM = isok ? dynamic_cast<Reve::BoxSet*>(obj) : 0;
  return isok;
}

void BoxSetGL::SetBBox()
{
  SetAxisAlignedBBox(fM->AssertBBox());
}

void BoxSetGL::DirectDraw(TGLRnrCtx & rnrCtx) const
{
  static const Exc_t eH("BoxSetGL::DirectDraw ");

  if (rnrCtx.DrawPass() == TGLRnrCtx::kPassOutlineLine)
    return;

  BoxSet& mB = * fM;
  // printf("BoxSetGL::DirectDraw N boxes %d\n", mB.fPlex.Size());
  if(mB.fPlex.Size() == 0)
    return;

  glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_CULL_FACE);

  if (mB.fRenderMode == DigitSet::RM_Fill)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  else if (mB.fRenderMode == DigitSet::RM_Line)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  if (mB.fDisableLigting) glDisable(GL_LIGHTING);

  if (rnrCtx.SecSelection()) glPushName(0);

  VoidCPlex::iterator bi(mB.fPlex);

  switch (mB.fBoxType)
  {

    case BoxSet::BT_FreeBox:
    {
      GLenum primitiveType = PrimitiveType();
      while (bi.next())
      {
        BoxSet::BFreeBox& b = * (BoxSet::BFreeBox*) bi();
        if (SetupColor(b))
	{
          if (rnrCtx.SecSelection()) glLoadName(bi.index());
          glBegin(primitiveType);
          RenderBox(b.fVertices);
          glEnd();
        }
      }
      break;
    } // end case free-box

    case BoxSet::BT_AABox:
    {
      glEnable(GL_NORMALIZE);
      while (bi.next())
      {
        BoxSet::BAABox& b = * (BoxSet::BAABox*) bi();
        if (SetupColor(b))
	{
          if (rnrCtx.SecSelection()) glLoadName(bi.index());
          glPushMatrix();
          glTranslatef(b.fA, b.fB, b.fC);
          glScalef    (b.fW, b.fH, b.fD);
          glCallList(fBoxDL);
          glPopMatrix();
        }
      }
      break;
    }

    case BoxSet::BT_AABoxFixedDim:
    {
      while (bi.next())
      {
        BoxSet::BAABoxFixedDim& b = * (BoxSet::BAABoxFixedDim*) bi();
        if (SetupColor(b))
	{
          if (rnrCtx.SecSelection()) glLoadName(bi.index());
          glTranslatef(b.fA, b.fB, b.fC);
          glCallList(fBoxDL);
          glTranslatef(-b.fA, -b.fB, -b.fC);
        }
      }
      break;
    }

    default:
    {
      throw(eH + "unsupported box-type.");
    }

  } // end switch box-type

  if (rnrCtx.SecSelection()) glPopName();

  glPopAttrib();
}

/**************************************************************************/
/**************************************************************************/

//______________________________________________________________________________
void BoxSetGL::ProcessSelection(TGLRnrCtx & /*rnrCtx*/, TGLSelectRecord & rec)
{
  // Processes secondary selection from TGLViewer.
  // Calls TPointSet3D::PointSelected(Int_t) with index of selected
  // point as an argument.

  if (rec.GetN() < 2) return;
  fM->DigitSelected(rec.GetItem(1));
}

void BoxSetGL::Render(TGLRnrCtx & rnrCtx)
{
  // Interface for direct rendering from classes that include BoxSet as a member.

  MakeDisplayList();
  DirectDraw(rnrCtx);
  glDeleteLists(fBoxDL, 1);
  fBoxDL = 0;
}
