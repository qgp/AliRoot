// $Header$

#include "PointSet.h"

#include <Reve/RGTopFrame.h>
#include <Reve/NLTProjector.h>

#include <TTree.h>
#include <TTreePlayer.h>
#include <TF3.h>

#include <TColor.h>
#include <TCanvas.h>

using namespace Reve;

//______________________________________________________________________
// PointSet
//

ClassImp(PointSet)

PointSet::PointSet(Int_t n_points, TreeVarType_e tv_type) :
  RenderElement(fMarkerColor),
  TPointSet3D(n_points),
  TPointSelectorConsumer(tv_type),

  fTitle()
{
  fMarkerStyle = 20;
}

PointSet::PointSet(const Text_t* name, Int_t n_points, TreeVarType_e tv_type) :
  RenderElement(fMarkerColor),
  TPointSet3D(n_points),
  TPointSelectorConsumer(tv_type),

  fTitle()
{
  fMarkerStyle = 20;
  SetName(name);
}

PointSet::PointSet(const Text_t* name, TTree* tree, TreeVarType_e tv_type) :
  RenderElement(fMarkerColor),
  TPointSet3D(tree->GetSelectedRows()),
  TPointSelectorConsumer(tv_type),

  fTitle()
{
  static const Exc_t eH("PointSet::PointSet ");

  fMarkerStyle = 20;
  SetName(name);

  TTreePlayer* tp = dynamic_cast<TTreePlayer*>(tree->GetPlayer());
  TakeAction(dynamic_cast<TSelectorDraw*>(tp->GetSelector()));
}

/**************************************************************************/

void PointSet::ComputeBBox()
{
  TPointSet3D::ComputeBBox();
  AssertBBoxExtents(0.1);
}

void PointSet::Reset(Int_t n_points)
{
  delete [] fP; fP = 0;
  fN = n_points;
  if(fN) fP = new Float_t [3*fN];
  memset(fP, 0, 3*fN*sizeof(Float_t));
  fLastPoint = -1;
  ClearIds();
  ResetBBox();
}

Int_t PointSet::GrowFor(Int_t n_points)
{
  // Resizes internal array to allow additional n_points to be stored.
  // Returns the old size which is also the location where one can
  // start storing new data.
  // The caller is *obliged* to fill the new point slots.

  Int_t old_size = Size();
  Int_t new_size = old_size + n_points;
  SetPoint(new_size - 1, 0, 0, 0);
  return old_size;
}

/**************************************************************************/

void PointSet::SetRnrElNameTitle(const Text_t* name, const Text_t* title)
{
  SetName(name);
  SetTitle(title);
}

/**************************************************************************/

void PointSet::Paint(Option_t* option)
{
  if(fRnrSelf == kFALSE) return;

  TPointSet3D::Paint(option);
}

/**************************************************************************/

void PointSet::TakeAction(TSelectorDraw* sel)
{
  static const Exc_t eH("PointSet::TakeAction ");

  if(sel == 0)
    throw(eH + "selector is <null>.");

  Int_t    n = sel->GetNfill();
  Int_t  beg = GrowFor(n);
  Float_t *p = fP + 3*beg;

  // printf("PointSet::TakeAction beg=%d n=%d size=%d\n", beg, n, Size());

  Double_t *vx = sel->GetV1(), *vy = sel->GetV2(), *vz = sel->GetV3();

  switch(fSourceCS) {
  case TVT_XYZ:
    while(n-- > 0) {
      p[0] = *vx; p[1] = *vy; p[2] = *vz;
      p += 3;
      ++vx; ++vy; ++vz;
    }
    break;
  case TVT_RPhiZ:
    while(n-- > 0) {
      p[0] = *vx * TMath::Cos(*vy); p[1] = *vx * TMath::Sin(*vy); p[2] = *vz;
      p += 3;
      ++vx; ++vy; ++vz;
    }
    break;
  default:
    throw(eH + "unknown tree variable type.");
  }
}

/**************************************************************************/

TClass* PointSet::ProjectedClass() const
{
  return NLTPointSet::Class();
}


/**************************************************************************/
/**************************************************************************/

//______________________________________________________________________
// PointSetArray
//

ClassImp(PointSetArray)

PointSetArray::PointSetArray(const Text_t* name,
			     const Text_t* title) :
  RenderElement(fMarkerColor),
  TNamed(name, title),

  fBins(0), fDefPointSetCapacity(128), fNBins(0), fLastBin(-1),
  fMin(0), fCurMin(0), fMax(0), fCurMax(0),
  fBinWidth(0),
  fQuantName()
{}

PointSetArray::~PointSetArray()
{
  // Destructor: deletes the fBins array. Actual removal of
  // elements done by RenderElement.

  // printf("PointSetArray::~PointSetArray()\n");
  delete [] fBins; fBins = 0;
}

void PointSetArray::Paint(Option_t* option)
{
  if(fRnrSelf) {
    for(List_i i=fChildren.begin(); i!=fChildren.end(); ++i) {
      if((*i)->GetRnrSelf())
	(*i)->GetObject()->Paint(option);
    }
  }
}

void PointSetArray::RemoveElementLocal(RenderElement* el)
{
  for(Int_t i=0; i<fNBins; ++i) {
    if(fBins[i] == el) {
      fBins[i] = 0;
      break;
    }
  }
}

void PointSetArray::RemoveElementsLocal()
{
  delete [] fBins; fBins = 0; fLastBin = -1;
}

/**************************************************************************/

void PointSetArray::SetMarkerColor(Color_t tcolor)
{
  for(List_i i=fChildren.begin(); i!=fChildren.end(); ++i) {
    TAttMarker* m = dynamic_cast<TAttMarker*>((*i)->GetObject());
    if(m && m->GetMarkerColor() == fMarkerColor)
      m->SetMarkerColor(tcolor);
  }
  TAttMarker::SetMarkerColor(tcolor);
}

void PointSetArray::SetMarkerStyle(Style_t mstyle)
{
  for(List_i i=fChildren.begin(); i!=fChildren.end(); ++i) {
    TAttMarker* m = dynamic_cast<TAttMarker*>((*i)->GetObject());
    if(m && m->GetMarkerStyle() == fMarkerStyle)
      m->SetMarkerStyle(mstyle);
  }
  TAttMarker::SetMarkerStyle(mstyle);
}

void PointSetArray::SetMarkerSize(Size_t msize)
{
  for(List_i i=fChildren.begin(); i!=fChildren.end(); ++i) {
    TAttMarker* m = dynamic_cast<TAttMarker*>((*i)->GetObject());
    if(m && m->GetMarkerSize() == fMarkerSize)
      m->SetMarkerSize(msize);
  }
  TAttMarker::SetMarkerSize(msize);
}

/**************************************************************************/

void PointSetArray::TakeAction(TSelectorDraw* sel)
{
  static const Exc_t eH("PointSetArray::TakeAction ");

  if(sel == 0)
    throw(eH + "selector is <null>.");

  Int_t n = sel->GetNfill();

  // printf("PointSetArray::TakeAction n=%d\n", n);

  Double_t *vx = sel->GetV1(), *vy = sel->GetV2(), *vz = sel->GetV3();
  Double_t *qq = sel->GetV4();

  if(qq == 0)
    throw(eH + "requires 4-d varexp.");

  switch(fSourceCS) {
  case TVT_XYZ:
    while(n-- > 0) {
      Fill(*vx, *vy, *vz, *qq);
      ++vx; ++vy; ++vz; ++qq;
    }
    break;
  case TVT_RPhiZ:
    while(n-- > 0) {
      Fill(*vx * TMath::Cos(*vy), *vx * TMath::Sin(*vy), *vz, *qq);
      ++vx; ++vy; ++vz; ++qq;
    }
    break;
  default:
    throw(eH + "unknown tree variable type.");
  }
}

/**************************************************************************/

void PointSetArray::InitBins(const Text_t* quant_name,
			     Int_t nbins, Double_t min, Double_t max,
			     Bool_t addRe)
{
  static const Exc_t eH("PointSetArray::InitBins ");

  if(nbins < 1) throw(eH + "nbins < 1.");
  if(min > max) throw(eH + "min > max.");

  RemoveElements();

  fQuantName = quant_name;
  fNBins     = nbins;
  fLastBin   = -1;
  fMin = fCurMin = min;
  fMax = fCurMax = max;
  fBinWidth  = (fMax - fMin)/fNBins;

  fBins = new Reve::PointSet*[fNBins];
  for(Int_t i=0; i<fNBins; ++i) {
    fBins[i] = new Reve::PointSet
      (Form("Slice %d [%4.3lf, %4.3lf]", i, fMin + i*fBinWidth, fMin + (i+1)*fBinWidth),
       fDefPointSetCapacity);
    fBins[i]->SetMarkerColor(fMarkerColor);
    fBins[i]->SetMarkerStyle(fMarkerStyle);
    fBins[i]->SetMarkerSize(fMarkerSize);
    if(addRe)
      gReve->AddRenderElement(fBins[i], this);
    else
      AddElement(fBins[i]);
  }
}

void PointSetArray::Fill(Double_t x, Double_t y, Double_t z, Double_t quant)
{
  fLastBin = Int_t( (quant - fMin)/fBinWidth );
  if(fLastBin >= 0 && fLastBin < fNBins && fBins[fLastBin] != 0)
    fBins[fLastBin]->SetNextPoint(x, y, z);
  else
    fLastBin = -1;
}

void PointSetArray::SetPointId(TObject* id)
{
  if (fLastBin >= 0)
    fBins[fLastBin]->SetPointId(id);
}

void PointSetArray::CloseBins()
{
  for(Int_t i=0; i<fNBins; ++i) {
    if(fBins[i] != 0) {
      // HACK! PolyMarker3D does half-management of array size.
      // In fact, the error is mine, in pointset3d(gl) i use fN instead of Size().
      // Fixed in my root, but not elsewhere.
      fBins[i]->fN = fBins[i]->fLastPoint;

      fBins[i]->ComputeBBox();
    }
  }
  fLastBin = -1;
}

/**************************************************************************/

void PointSetArray::SetOwnIds(Bool_t o)
{
  for(Int_t i=0; i<fNBins; ++i)
  {
    if(fBins[i] != 0)
      fBins[i]->SetOwnIds(o);
  }
}

/**************************************************************************/

void PointSetArray::SetRange(Double_t min, Double_t max)
{
  using namespace TMath;

  fCurMin = min; fCurMax = max;
  Int_t  low_b = (Int_t) Max(Double_t(0),       Floor((min-fMin)/fBinWidth));
  Int_t high_b = (Int_t) Min(Double_t(fNBins-1), Ceil((max-fMin)/fBinWidth));
  for(Int_t i=0; i<fNBins; ++i) {
    if(fBins[i] != 0)
      fBins[i]->SetRnrSelf(i>=low_b && i<=high_b);
  }
}


/**************************************************************************/
/**************************************************************************/


//______________________________________________________________________
// NLTPointSet
//

ClassImp(NLTPointSet)

NLTPointSet::NLTPointSet() :
  PointSet     (),
  NLTProjected ()
{}

void NLTPointSet::SetProjection(NLTProjector* proj, NLTProjectable* model)
{
  NLTProjected::SetProjection(proj, model);

  * (TAttMarker*)this = * dynamic_cast<TAttMarker*>(fProjectable);
}

void NLTPointSet::UpdateProjection()
{
  NLTProjection& proj = * fProjector->GetProjection();
  PointSet     & ps   = * dynamic_cast<PointSet*>(fProjectable);

  Int_t n = ps.GetN();
  Reset(n);
  Float_t *o = ps.GetP(), *p = GetP();
  for(Int_t i = 0; i < n; ++i, o+=3, p+=3)
  {
    p[0] = o[0]; p[1] = o[1]; p[2] = o[2];
    proj.ProjectPoint(p[0], p[1], p[2]);
    p[2] = fDepth;
  }
  fLastPoint = n - 1;
}
