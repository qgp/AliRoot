// $Header$

#include "TPCSector3DGL.h"
#include <Alieve/TPCSector3D.h>

#include <Reve/BoxSetGL.h>

#include <TGLRnrCtx.h>
#include <TGLIncludes.h>

using namespace Reve;
using namespace Alieve;

//______________________________________________________________________
// TPCSector3DGL
//

ClassImp(TPCSector3DGL)

TPCSector3DGL::TPCSector3DGL() :
  TGLObject(),
  fSector(0), fBoxRnr(0),
  fRTS(0)
{
  // fDLCache = false; // Disable display list.
}

TPCSector3DGL::~TPCSector3DGL()
{
  delete fBoxRnr;
}

/**************************************************************************/

Bool_t TPCSector3DGL::SetModel(TObject* obj, const Option_t* /*opt*/)
{
  if(SetModelCheckClass(obj, Alieve::TPCSector3D::Class())) {
    fSector = (TPCSector3D*) fExternalObj;
    if(fBoxRnr == 0) {
      fBoxRnr = new BoxSetGL;
      fBoxRnr->SetModel(&fSector->fBoxSet);
    }
    return kTRUE;
  }
  return kFALSE;
}

void TPCSector3DGL::SetBBox()
{
  SetAxisAlignedBBox(((TPCSector3D*)fExternalObj)->AssertBBox());
}

/**************************************************************************/

void TPCSector3DGL::DirectDraw(TGLRnrCtx & rnrCtx) const
{
  // printf("TPCSector3DGL::DirectDraw Style %d, LOD %d\n", rnrCtx.Style(), rnrCtx.LOD());

  if(fRTS < fSector->fRTS) {
    fSector->UpdateBoxes();
    fRTS = fSector->fRTS;
  }  

  Bool_t hasData = (fSector->GetSectorData() != 0);

  if(hasData)
    fBoxRnr->Render(rnrCtx);

  glPushAttrib(GL_CURRENT_BIT | GL_POINT_BIT | GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);
  UChar_t col[4];

  if(hasData && fSector->fPointSetOn) {
    glEnable(GL_BLEND);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(fSector->fPointSize);

    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
    glEnableClientState(GL_VERTEX_ARRAY);

    const Reve::PointSetArray& psa = fSector->fPointSetArray;
    for(Int_t b=0; b<psa.GetNBins(); ++b) {
      Reve::PointSet* ps = psa.GetBin(b);
      if(ps->Size() > 0) {
	ColorFromIdx(ps->GetMarkerColor(), col);
	glColor4ubv(col);

	glVertexPointer(3, GL_FLOAT, 0, ps->GetP());
	glDrawArrays(GL_POINTS, 0, ps->Size());
      }
    }

    glPopClientAttrib();
  }

  if(fSector->fRnrFrame) {
    ColorFromIdx(fSector->fFrameColor, col);
    glColor4ubv(col);

    if(fSector->fRnrInn)
      DrawSegmentFrame(TPCSectorData::GetInnSeg(),  0, 2);
    if(fSector->fRnrOut1)
      DrawSegmentFrame(TPCSectorData::GetOut1Seg(), 2, 1);
    if(fSector->fRnrOut2)
      DrawSegmentFrame(TPCSectorData::GetOut2Seg(), 2, 2);
  }

  glPopAttrib();
}

void TPCSector3DGL::DrawSegmentFrame(const TPCSectorData::SegmentInfo& s,
                                     Int_t botExtraPads, Int_t topExtraPads) const
{
  Float_t xl, xh, yl, yh, zl, zh;
  xl = 0.5*s.GetPadWidth()*(TPCSectorData::GetNPadsInRow(s.GetFirstRow()) + botExtraPads);
  xh = 0.5*s.GetPadWidth()*(TPCSectorData::GetNPadsInRow(s.GetLastRow())  + topExtraPads);
  yl = s.GetRLow();
  yh = yl + s.GetNRows()*s.GetPadHeight();
  zl = 0;
  zh = TPCSectorData::GetZLength();

  glBegin(GL_LINE_LOOP);
  glVertex3f( xl, yl, zl);  glVertex3f( xh, yh, zl);
  glVertex3f(-xh, yh, zl);  glVertex3f(-xl, yl, zl);
  glEnd();
  glBegin(GL_LINE_LOOP);
  glVertex3f( xl, yl, zh);  glVertex3f( xh, yh, zh);
  glVertex3f(-xh, yh, zh);  glVertex3f(-xl, yl, zh);
  glEnd();
  glBegin(GL_LINES);
  glVertex3f( xl, yl, zl);  glVertex3f( xl, yl, zh);
  glVertex3f( xh, yh, zl);  glVertex3f( xh, yh, zh);
  glVertex3f(-xh, yh, zl);  glVertex3f(-xh, yh, zh);
  glVertex3f(-xl, yl, zl);  glVertex3f(-xl, yl, zh);
  glEnd();
}
