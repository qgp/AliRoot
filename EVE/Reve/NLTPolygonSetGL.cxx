#include "NLTPolygonSetGL.h"
#include "NLTPolygonSet.h"
#include "PODs.h"

#include <TGLRnrCtx.h>
#include <TGLIncludes.h>


using namespace Reve;

/**************************************************************************/

NLTPolygonSetGL::NLTPolygonSetGL() : TGLObject()
{
  // fDLCache = false; // Disable DL.
}

NLTPolygonSetGL::~NLTPolygonSetGL()
{}

/**************************************************************************/
Bool_t NLTPolygonSetGL::SetModel(TObject* obj, const Option_t* /*opt*/)
{
  return SetModelCheckClass(obj, NLTPolygonSet::Class());
}

/**************************************************************************/

void NLTPolygonSetGL::SetBBox()
{
  SetAxisAlignedBBox(((NLTPolygonSet*)fExternalObj)->AssertBBox());
}

/**************************************************************************/
static GLUtriangulatorObj *GetTesselator()
{
   static struct Init {
      Init()
      {
#if defined(R__WIN32)
         typedef void (CALLBACK *tessfuncptr_t)();
#elif defined(R__AIXGCC)
         typedef void (*tessfuncptr_t)(...);
#else
         typedef void (*tessfuncptr_t)();
#endif
         fTess = gluNewTess();

         if (!fTess) {
            Error("GetTesselator::Init", "could not create tesselation object");
         } else {
            gluTessCallback(fTess, (GLenum)GLU_BEGIN, (tessfuncptr_t)glBegin);
            gluTessCallback(fTess, (GLenum)GLU_END, (tessfuncptr_t)glEnd);
            gluTessCallback(fTess, (GLenum)GLU_VERTEX, (tessfuncptr_t)glVertex3fv);
         }
      }
      ~Init()
      {
         if(fTess)
            gluDeleteTess(fTess);
      }
      GLUtriangulatorObj *fTess;
   }singleton;

   return singleton.fTess;
}

/**************************************************************************/
void NLTPolygonSetGL::DirectDraw(TGLRnrCtx & /*rnrCtx*/) const
{
  //  printf("NLTPolygonSetGL::DirectDraw %s \n",fExternalObj->GetName() );
  NLTPolygonSet& PS = * (NLTPolygonSet*) fExternalObj;
  if(PS.fPols.size() == 0) return;

  glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_POLYGON_BIT);

  glDisable(GL_LIGHTING);
  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_CULL_FACE);

  // polygons
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.,1.);
  GLUtriangulatorObj *tessObj = GetTesselator();

  Vector* pnts = PS.fPnts;
  for (NLTPolygonSet::vpPolygon_ci i = PS.fPols.begin(); i!= PS.fPols.end(); i++)
  {
    Int_t vi; //current vertex index of curent polygon
    Int_t N = (*i).fNPnts; // number of points in current polygon
    if(N < 4) 
    {
      glBegin(GL_POLYGON);
      for(Int_t k=0; k<N; k++)
      {
	vi = (*i).fPnts[k]; 
	glVertex3fv(pnts[vi].c_vec());
      }
      glEnd();
    }
    else {
      gluBeginPolygon(tessObj);
      gluNextContour(tessObj, (GLenum)GLU_UNKNOWN);
      glNormal3f(0., 0., 1.);
      Double_t coords[3];
      coords[2] = 0.;
      for (Int_t k = 0; k<N; k++)
      {
	vi = (*i).fPnts[k];
	coords[0] = pnts[vi].x;
	coords[1] = pnts[vi].y;
	gluTessVertex(tessObj, coords, pnts[vi].c_vec());
      }
      gluEndPolygon(tessObj);
    }
  }
  glDisable(GL_POLYGON_OFFSET_FILL);

  // outline 
  UChar_t lcol[4];
  ColorFromIdx(PS.fLineColor, lcol);
  glColor4ubv(lcol);
  glEnable(GL_LINE_SMOOTH);

  glLineWidth(PS.fLineWidth);
  Int_t vi;
  for (NLTPolygonSet::vpPolygon_ci i = PS.fPols.begin(); i!= PS.fPols.end(); i++)
  {
    glBegin(GL_LINE_LOOP); 
    for(Int_t k=0; k<(*i).fNPnts; k++)
    {
      vi = (*i).fPnts[k];
      glVertex3fv(PS.fPnts[vi].c_vec());
    }
    glEnd();
  }

  glPopAttrib();
}

