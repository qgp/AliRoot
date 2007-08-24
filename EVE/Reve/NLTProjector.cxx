#include "NLTProjector.h"
#include "RGTopFrame.h"
#include "NLTPolygonSet.h"
#include "PODs.h"
#include "PointSet.h"
#include "Track.h"

#include "TQObject.h"
#include "TBuffer3D.h"
#include "TBuffer3DTypes.h"
#include "TColor.h"
#include "TPointSet3D.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include <TVirtualPad.h>
#include <TVirtualViewer3D.h>

#include <list>

using namespace Reve;
//______________________________________________________________________________
Vector* NLTProjection::Project(Vector* origPnts, Int_t Npnts, Bool_t copy)
{
  Vector* pnts = 0; 
  if(copy) 
  {
    pnts = (Vector* )malloc(Npnts*sizeof(Vector));
    memcpy(pnts, origPnts, Npnts*sizeof(Vector));
  }
  else
  { 
    pnts =  origPnts;
  }

  for(Int_t i = 0; i<Npnts; i++)
  {
    ProjectPoint(pnts[i].x, pnts[i].y, pnts[i].z);
  }
  return pnts;
}

//______________________________________________________________________________
void RhoZ::ProjectPoint(Float_t& x, Float_t& y, Float_t& z)
{
    Float_t R = TMath::Sqrt(x*x+y*y);
    Float_t NR = R / (1.0f + R*fDistortion);
    y = (y > 0) ? NR : -NR;
    y *= fScale;
    x = (z*fScale)/ (1.0f + TMath::Abs(z)*fDistortion);
    z = 0;
}

//______________________________________________________________________________
Bool_t RhoZ::AcceptSegment(Vector& v1, Vector& v2, Float_t tolerance) 
{
  if((v1.y < 0 && v2.y > 0) || (v1.y > 0 && v2.y < 0))
  {
    if (tolerance > 0)
    {
      Float_t a1 = TMath::Abs(v1.y), a2 = TMath::Abs(v2.y);
      if (a1 < a2)
      {
	if (a1 < tolerance) { v1.y = 0; return kTRUE; }
      }
      else
      {
	if (a2 < tolerance) { v2.y = 0; return kTRUE; }
      }
    }
    return kFALSE;
  }
  return kTRUE;
}

/**************************************************************************/
/**************************************************************************/
//______________________________________________________________________________
void  CircularFishEye::ProjectPoint(Float_t& x, Float_t& y, Float_t& z) 
{
  Float_t R = TMath::Sqrt(x*x+y*y);
  Float_t phi = x == 0.0 && y == 0.0 ? 0.0 : TMath::ATan2(y,x);
  Float_t NR = (R*fScale) / (1.0f + R*fDistortion);
  x = NR*TMath::Cos(phi);
  y = NR*TMath::Sin(phi);
  z = 0;
}

/**************************************************************************/
/**************************************************************************/
//______________________________________________________________________
// NLTProjector
//

ClassImp(NLTProjector)

NLTProjector::NLTProjector():
  RenderElementList("NLTProjector",""),
  fProjection (0)
{
  SetProjection(NLTProjection::PT_CFishEye, 0);
}

//______________________________________________________________________________
NLTProjector::~NLTProjector()
{
  TQObject::Disconnect("Reve::Track", "ProjectTrack(Bool_t)");
  if(fProjection) delete fProjection;
}

//______________________________________________________________________________
void NLTProjector::SetProjection(NLTProjection::PType_e type, Float_t distort)
{
  static const Exc_t eH("NLTProjector::SetProjection ");

  delete fProjection;
  fProjection = 0;

  TString name;
  switch (type)
  {
    case NLTProjection::PT_CFishEye:
    {
      fProjection  = new CircularFishEye();
      name = Form("CircularFishEye %3f", distort);
      break;
    }
    case NLTProjection::PT_RhoZ:
    {
      fProjection  = new RhoZ;
      name = Form("RhoZ %3f", distort);
      break;
    }
    default:
      throw(eH + "projection type not valid.");
      break;
  }
  SetName(name.Data());
  fProjection->SetDistortion(distort);
}

//______________________________________________________________________________
void NLTProjector::SetProjection(NLTProjection* p)
{
  delete fProjection;
  fProjection = p;
}

//______________________________________________________________________________
Bool_t NLTProjector::HandleElementPaste(RenderElement* el)
{
  size_t n_children  = fChildren.size();
  ImportElements(el);
  return n_children != fChildren.size();
}

//______________________________________________________________________________
Bool_t NLTProjector::ShouldImport(RenderElement* rnr_el)
{
  if (rnr_el->IsA()->InheritsFrom(NLTProjectable::Class()))
    return kTRUE;
  for (List_i i=rnr_el->BeginChildren(); i!=rnr_el->EndChildren(); ++i)
    if (ShouldImport(*i))
      return kTRUE;
  return kFALSE;
}

//______________________________________________________________________________
void NLTProjector::ImportElementsRecurse(RenderElement* rnr_el, RenderElement* parent)
{
  if (ShouldImport(rnr_el))
  {
    RenderElement  *new_re = 0;
    NLTProjected   *new_pr = 0;
    NLTProjectable *pble   = dynamic_cast<NLTProjectable*>(rnr_el);
    if (pble)
    {
      new_re = (RenderElement*) pble->ProjectedClass()->New();
      new_pr = dynamic_cast<NLTProjected*>(new_re);
      new_pr->SetProjection(this, pble);
    }
    else
    {
      new_re = new RenderElementList;      
    }
    TObject *tobj   = rnr_el->GetObject();
    new_re->SetRnrElNameTitle(Form("NLT %s", tobj->GetName()),
			      tobj->GetTitle());
    gReve->AddRenderElement(new_re, parent);

    if (new_pr)
    {
      new_pr->UpdateProjection();
    }

    for (List_i i=rnr_el->BeginChildren(); i!=rnr_el->EndChildren(); ++i)
      ImportElementsRecurse(*i, new_re);
  }
}

//______________________________________________________________________________
void NLTProjector::ImportElements(RenderElement* rnr_el)
{
  ImportElementsRecurse(rnr_el, this);
}

//______________________________________________________________________________
void NLTProjector::ProjectChildrenRecurse(RenderElement* rnr_el)
{
  NLTProjected* pted = dynamic_cast<NLTProjected*>(rnr_el);
  if (pted)
  {
    pted->UpdateProjection();
    rnr_el->ElementChanged(kFALSE);
  }

  for (List_i i=rnr_el->BeginChildren(); i!=rnr_el->EndChildren(); ++i)
    ProjectChildrenRecurse(*i);
}

//______________________________________________________________________________
void NLTProjector::ProjectChildren()
{
  ProjectChildrenRecurse(this);

  List_t scenes;
  CollectSceneParentsFromChildren(scenes, 0);
  gReve->ScenesChanged(scenes);
}

//______________________________________________________________________________
void NLTProjector::Paint(Option_t* /*option*/)
{
  static const Exc_t eH("NLTProjector::Paint ");
  TBuffer3D buff(TBuffer3DTypes::kGeneric);

  // Section kCore
  buff.fID           = this;
  buff.fColor        = GetMainColor();
  buff.fTransparency = 0;
  buff.SetSectionsValid(TBuffer3D::kCore);

  Int_t reqSections = gPad->GetViewer3D()->AddObject(buff);
  if (reqSections != TBuffer3D::kNone)
    Error(eH, "only direct GL rendering supported.");
}

//______________________________________________________________________________
void NLTProjector::ComputeBBox()
{
  // Fill bounding-box information in base-class TAttBBox (virtual method).

  // TODO:: be reseted and checked points, tracks  ...
  static const Exc_t eH("NLTProjector::ComputeBBox ");

  if(GetNChildren() == 0) {
    BBoxZero();
    return;
  }

  BBoxInit();
}
