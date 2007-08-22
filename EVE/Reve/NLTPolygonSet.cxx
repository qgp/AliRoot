#include "NLTPolygonSet.h"
#include "Reve/PODs.h"
#include "Reve/GeoNode.h"
#include "Reve/NLTProjector.h"

#include <TBuffer3D.h>
#include <TBuffer3DTypes.h>
#include <TVirtualPad.h>
#include <TVirtualViewer3D.h>

#include <list>

using namespace Reve;

namespace {
  struct Seg {
    Int_t v1;
    Int_t v2;

    Seg(Int_t i1=-1, Int_t i2=-1):v1(i1), v2(i2){};
  };
  typedef std::list<Seg>::iterator It_t;    
}


ClassImp(NLTPolygonSet)

/**************************************************************************/
NLTPolygonSet::NLTPolygonSet(const Text_t* n, const Text_t* t) :
  RenderElementList(n, t),

  fBuff(0),
  fIdxMap(0),

  fEps(0.05),
  fNPnts(0),
  fPnts(0),

  fFillColor(5),
  fLineColor(3),
  fLineWidth(1),
  fTransparency (0),
  fZDepth(0)
{
  SetMainColorPtr(&fFillColor);
}

/**************************************************************************/

NLTPolygonSet::~NLTPolygonSet()
{
  ClearPolygonSet();
}

void NLTPolygonSet::ClearPolygonSet()
{
  // delete polygon vertex indices
  Int_t* p;
  for (vpPolygon_i i = fPols.begin(); i!= fPols.end(); i++)
  {
    p =  (*i).fPnts; delete [] p;
  }
  fPols.clear();
  
  // delete reduced points  
  delete [] fPnts; fNPnts = 0;
}

/**************************************************************************/
void NLTPolygonSet::SetProjection(NLTProjector* proj, NLTProjectable* model)
{
  NLTProjected::SetProjection(proj, model);
  GeoShapeRnrEl* gsre = dynamic_cast<GeoShapeRnrEl*>(model);

  fBuff = gsre->MakeBuffer3D();
  if(fBuff)
  {
    SetMainColor(gsre->GetColor());
    SetLineColor((Color_t)TColor::GetColorBright(gsre->GetColor()));
    SetMainTransparency(gsre->GetMainTransparency());

    SetRnrSelf(gsre->GetRnrSelf());
    SetRnrChildren(gsre->GetRnrChildren());

    ProjectBuffer3D();
  }
}

/**************************************************************************/
void NLTPolygonSet::UpdateProjection()
{
  if(fBuff == 0) return;

  // drop polygons, and projected/reduced points
  ClearPolygonSet();
  ProjectBuffer3D();
}

//______________________________________________________________________________
Bool_t NLTPolygonSet::IsFirstIdxHead(Int_t s0, Int_t s1)
{
  Int_t v0 = fBuff->fSegs[3*s0 + 1];
  Int_t v2 = fBuff->fSegs[3*s1 + 1];
  Int_t v3 = fBuff->fSegs[3*s1 + 2];
  if(v0 != v2 && v0 != v3 )
    return kTRUE;
  else 
    return kFALSE;
}

//______________________________________________________________________________
void NLTPolygonSet::ProjectAndReducePoints()
{
  NLTProjection* projection = fProjector->GetProjection();
  
  Int_t N = fBuff->NbPnts();
  Vector*  pnts  = new Vector[N];
  for(Int_t i = 0; i<N; i++) 
    pnts[i].Set(fBuff->fPnts[3*i],fBuff->fPnts[3*i+1], fBuff->fPnts[3*i+2]);
  projection->Project(pnts, N, kFALSE);

  fIdxMap   = new Int_t[N];  
  Int_t* ra = new Int_t[N];  // list of reduced vertices

  for(UInt_t v = 0; v < (UInt_t)N; v++)
  {
    fIdxMap[v] = -1;
    for(Int_t k = 0; k<fNPnts; k++) 
    {
      if(pnts[v].SquareDistance(pnts[ra[k]]) < fEps*fEps)
      {
	fIdxMap[v] = k; 
	break;
      }
    } 
    // have not found a point inside epsilon, add new point in scaled array
    if(fIdxMap[v] == -1)
    {
      fIdxMap[v] = fNPnts;
      ra[fNPnts] = v;
      fNPnts++;
    }
    // printf("(%f, %f) vertex map %d -> %d \n", pnts[v*2], pnts[v*2 + 1], v, fIdxMap[v]);
  }
  
  // create an array of scaled points
  fPnts = new Vector[fNPnts];
  for(Int_t i = 0; i<fNPnts; i++)
    fPnts[i].Set(pnts[ra[i]].x,  pnts[ra[i]].y,  pnts[ra[i]].z);
  
  delete [] ra;  
  // printf("reduced %d points of %d\n", fNPnts, N);
}

//______________________________________________________________________________
void NLTPolygonSet::AddPolygon(std::list<Int_t>& pp)
{
  if(pp.size() <= 2) return;

  // dimension of bbox
  Float_t bbox[] = {0., 0., 0., 0., 0., 0.};
  for (std::list<Int_t>::iterator u = pp.begin(); u!= pp.end(); u++) 
  {
    Int_t idx = *u; 
    if(fPnts[idx].x < bbox[0]) bbox[0] = fPnts[idx].x;   
    if(fPnts[idx].x > bbox[1]) bbox[1] = fPnts[idx].x;

    if(fPnts[idx].y < bbox[2]) bbox[2] = fPnts[idx].y;   
    if(fPnts[idx].y > bbox[3]) bbox[3] = fPnts[idx].y;

    if(fPnts[idx].z < bbox[4]) bbox[4] = fPnts[idx].z;   
    if(fPnts[idx].z > bbox[5]) bbox[5] = fPnts[idx].z;
    // printf("bbox (%f, %f) (%f, %f)\n", bbox[1], bbox[0], bbox[3], bbox[2]);
  }
  if((bbox[1]-bbox[0])<fEps || (bbox[3]-bbox[2])<fEps) return;

  // duplication
  for (std::list<NLTPolygon>::iterator poi = fPols.begin(); poi!= fPols.end(); poi++)
  {
    NLTPolygon P = *poi;
    if(pp.size() != (UInt_t)P.fNPnts) 
      continue;      
    std::list<Int_t>::iterator u;
    for (u = pp.begin(); u!= pp.end(); u++) 
    {
      if ((*u) != P.fPnts[*u]) break;
    }
    if (u == pp.end()) return;
  }

  // printf("add %d NLTPolygon points %d \n", fPols.size(), pp.size());
  Int_t* pv = new Int_t[pp.size()];
  Int_t count=0;
  for( std::list<Int_t>::iterator u = pp.begin(); u!= pp.end(); u++){
    pv[count] = *u;
    count++;
  }
  fPols.push_back(NLTPolygon(pp.size(), pv));
} // AddPolygon

//______________________________________________________________________________
void NLTPolygonSet::MakePolygonsFromBP()
{
  // build polygons from sorted list of segments : buff->fPols

  //  printf("START NLTPolygonSet::MakePolygonsFromBP\n");
  NLTProjection* projection = fProjector->GetProjection();
  Int_t* bpols = fBuff->fPols;
  for(UInt_t pi = 0; pi< fBuff->NbPols(); pi++) 
  {
    std::list<Int_t>  pp; // points in current polygon 
    UInt_t Nseg = bpols[1]; 
    Int_t* seg =  &bpols[2];
    // start idx in the fist segment depends of second segment 
    Int_t  tail, head;
    Bool_t h = IsFirstIdxHead(seg[0], seg[1]);
    if(h) {
      head = fIdxMap[fBuff->fSegs[3*seg[0] + 1]];
      tail = fIdxMap[fBuff->fSegs[3*seg[0] + 2]];
    }
    else {
      head = fIdxMap[fBuff->fSegs[3*seg[0] + 2]];
      tail = fIdxMap[fBuff->fSegs[3*seg[0] + 1]];
    }
    pp.push_back(head);
    // printf("start idx head %d, tail %d\n", head, tail);
    std::list<Seg> segs;  
    for(UInt_t s=1; s< Nseg; s++)
      segs.push_back(Seg(fBuff->fSegs[3*seg[s] + 1],fBuff->fSegs[3*seg[s] + 2]));
    Bool_t accepted = kFALSE; 
    for(std::list<Seg>::iterator it = segs.begin(); it != segs.end(); it++ )
    { 
      Int_t mv1 = fIdxMap[(*it).v1];
      Int_t mv2 = fIdxMap[(*it).v2];         
      accepted = projection->AcceptSegment(fPnts[mv1], fPnts[mv2], fEps);
      if(accepted == kFALSE)
      {
	pp.clear();
	break;
      }	  
      if(tail != pp.back()) pp.push_back(tail);
      tail = (mv1 == tail) ? mv2 :mv1;
    }
    // DirectDraw implementation: last and first vertices should not be equal
    if(pp.empty() == kFALSE) 
    {
      if(pp.front() == pp.back()) pp.pop_front();
      AddPolygon(pp);
    }
    bpols += (Nseg+2);
  }
}// MakePolygonsFromBP

//______________________________________________________________________________
void NLTPolygonSet::MakePolygonsFromBS()
{
  // builds polygons from the set of buffer segments

  // create your own list of segments according to reduced and projected points
  std::list<Seg> segs;  
  std::list<Seg>::iterator it;
  NLTProjection* projection = fProjector->GetProjection();
  for(UInt_t s=0; s< fBuff->NbSegs(); s++)
  {
    Bool_t duplicate = kFALSE;
    Int_t vo1, vo2;   // idx from fBuff segment
    Int_t vor1, vor2; // mapped idx 
    vo1 =  fBuff->fSegs[3*s + 1];
    vo2 =  fBuff->fSegs[3*s + 2]; //... skip color info
    vor1 = fIdxMap[vo1];
    vor2 = fIdxMap[vo2];
    if(vor1 == vor2) continue;
    // check duplicate
    for(it = segs.begin(); it != segs.end(); it++ ){
      Int_t vv1 = (*it).v1;
      Int_t vv2 = (*it).v2;
      if((vv1 == vor1 && vv2 == vor2 )||(vv1 == vor2 && vv2 == vor1 )){
	duplicate = kTRUE;
	continue;
      }
    }
    if(duplicate == kFALSE && projection->AcceptSegment(fPnts[vor1], fPnts[vor2], fEps))
    {
      segs.push_back(Seg(vor1, vor2));
    }
  }

  // build polygons from segment pool
  while(segs.empty() == kFALSE)
  {
    // printf("Start building polygon %d from %d segments in POOL \n", pols.size(), segs.size());
    std::list<Int_t> pp; // points in current polygon
    pp.push_back(segs.front().v1);
    Int_t tail = segs.front().v2;
    segs.pop_front();
    Bool_t match = kTRUE;
    while(match && segs.empty() == kFALSE)
    {
      // printf("second loop search tail %d \n",tail);
      for(It_t k=segs.begin(); k!=segs.end(); ++k){
	Int_t cv1 = (*k).v1;
	Int_t cv2 = (*k).v2;
	if( cv1 == tail || cv2 == tail){
	  // printf("found point %d  in %d,%d  \n", tail, cv1, cv2);
	  pp.push_back(tail);
          tail = (cv1 == tail)? cv2:cv1;
	  It_t to_erase = k--;
	  segs.erase(to_erase);
	  match = kTRUE;
	  break;
	}
	else 
	{
	  match = kFALSE;
	}
      } // end for loop in the segment pool
      if(tail == pp.front())
	break;
    };
    AddPolygon(pp);
  }
}//MakePolygonsFromBS


/**************************************************************************/
void  NLTPolygonSet::ProjectBuffer3D()
{
  //DumpBuffer3D();
  ProjectAndReducePoints();

  MakePolygonsFromBP(); 
  if(fPols.empty())
    MakePolygonsFromBS();
  
  delete []  fIdxMap;

  ResetBBox();
}

/**************************************************************************/
void NLTPolygonSet::ComputeBBox()
{
  // if(fPols.size() == 0) {
  //  BBoxZero();
  // }
  
  BBoxInit();
  for(Int_t pi = 0; pi<fNPnts; pi++) 
    BBoxCheckPoint(fPnts[pi].x, fPnts[pi].y, fPnts[pi].z );
  AssertBBoxExtents(0.1);
}

/**************************************************************************/
void NLTPolygonSet::Paint(Option_t* )
{
  if(fNPnts == 0) return;
  TBuffer3D buffer(TBuffer3DTypes::kGeneric);

  // Section kCore
  buffer.fID           = this;
  buffer.fColor        = GetMainColor();
  buffer.fTransparency = fTransparency;
  buffer.fLocalFrame   = false; 

  buffer.SetSectionsValid(TBuffer3D::kCore);
   
  // We fill kCore on first pass and try with viewer
  Int_t reqSections = gPad->GetViewer3D()->AddObject(buffer);
  if (reqSections == TBuffer3D::kNone) {
    return;
  }
}

/**************************************************************************/
void NLTPolygonSet::DumpPolys() const
{
  printf("NLTPolygonSet %d polygons\n", fPols.size());
  for (vpPolygon_ci i = fPols.begin(); i!= fPols.end(); i++)
  {
    Int_t N =  (*i).fNPnts;
    printf("polygon %d points :\n", N);    
    for(Int_t vi = 0; vi<N; vi++) {
      Int_t pi = (*i).fPnts[vi];
      printf("(%f, %f, %f)", fPnts[pi].x, fPnts[pi].y, fPnts[pi].z);
    }
    printf("\n");
  }
}

//______________________________________________________________________________
void NLTPolygonSet::DumpBuffer3D()
{
  Int_t* bpols = fBuff->fPols;
  
  for(UInt_t pi = 0; pi< fBuff->NbPols(); pi++) 
  {
    UInt_t Nseg = bpols[1]; 
    printf("%d polygon of %d has %d segments \n", pi,fBuff->NbPols(),Nseg);
    
    Int_t* seg =  &bpols[2];
    for(UInt_t a=0; a<Nseg; a++)
    {
      Int_t a1 = fBuff->fSegs[3*seg[a]+ 1];
      Int_t a2 = fBuff->fSegs[3*seg[a]+ 2];
      printf("(%d, %d) \n", a1, a2);
      printf("ORIG points :(%f, %f, %f)  (%f, %f, %f)\n", 
      	     fBuff->fPnts[3*a1],fBuff->fPnts[3*a1+1], fBuff->fPnts[3*a1+2],
      	     fBuff->fPnts[3*a2],fBuff->fPnts[3*a2+1], fBuff->fPnts[3*a2+2]);
    }
    printf("\n");
    bpols += (Nseg+2);
  }
}
