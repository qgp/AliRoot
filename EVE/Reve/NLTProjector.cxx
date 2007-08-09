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

namespace {
  struct Seg {
    Int_t v1;
    Int_t v2;

    Seg(Int_t i1=-1, Int_t i2=-1):v1(i1), v2(i2){};
  };
   
  typedef std::list<Seg>::iterator It_t;    
}

//______________________________________________________________________________
Vector*  NLTProjection::Project(Vector* origPnts, Int_t Npnts, Bool_t copy)
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
void  RhoZ::ProjectPoint(Float_t& x, Float_t& y, Float_t& z)
{
    Float_t R = TMath::Sqrt(x*x+y*y);
    Float_t NR =R/(1+R*fDistortion);
    y = ( y > 0) ? NR : -NR;
    x = z/(1+TMath::Abs(z)*fDistortion);
    z = 0;
}

//______________________________________________________________________________
Bool_t RhoZ::AcceptSegment(Vector& v1, Vector& v2, Float_t tolerance) 
{
  //  return (v1.y*v2.y < 0) ? kFALSE : kTRUE;
  if(v1.y*v2.y < 0){
    if(TMath::Abs(v1.y - v2.y) > tolerance)
    {
      return kFALSE;
    }
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
  Float_t NR = R/(1+R*fDistortion);
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
  fProjection(0),
  fEps(0.05),fIdxMap(0),  
  fNPnts(0), fPnts(0),
  fNRPnts(0), fRPnts(0)
{
}

//______________________________________________________________________________
NLTProjector::~NLTProjector()
{
  CleanUp();

  TQObject::Disconnect("Reve::Track", "ProjectTrack(Bool_t)");
  if(fProjection) delete fProjection;
}

//______________________________________________________________________________
void NLTProjector::CleanUp()
{
  if(fIdxMap)
  {
    delete [] fIdxMap; 
    fIdxMap = 0;
  }
  if(fRPnts)
  {
    delete [] fRPnts; 
    fNRPnts  = 0;
    fRPnts = 0;
  }
}

//______________________________________________________________________________
void NLTProjector::SetProjection(NLTProjection::Type_e type, Float_t distort)
{
  static const Exc_t eH("NLTProjector::SetProjection ");
  TString name;
  switch (type)
  {
    case NLTProjection::E_RhoZ:
    {
      fProjection  = new RhoZ();
      name = Form("RhoZ %3f", distort);
      break;
    }
    case NLTProjection::E_CFishEye:
    {
      fProjection  = new CircularFishEye();
      name = Form("CircularFishEye %3f", distort);
      break;
    }
    default:
      throw(eH + "projection type not valid.");
      break;
  }
  SetName(name.Data());
  fProjection->fDistortion = distort;
}

//______________________________________________________________________________
void NLTProjector::DumpBuffer(TBuffer3D* buff)
{
  Int_t* bpols = buff->fPols;
  
  for(UInt_t pi = 0; pi< buff->NbPols(); pi++) 
  {
    UInt_t Nseg = bpols[1]; 
    printf("%d polygon of %d has %d segments \n", pi,buff->NbPols(),Nseg);
    
    Int_t* seg =  &bpols[2];
    for(UInt_t a=0; a<Nseg; a++)
    {
      Int_t a1 = buff->fSegs[3*seg[a]+ 1];
      Int_t a2 = buff->fSegs[3*seg[a]+ 2];
      printf("(%d, %d) \n", a1, a2);
      printf("ORIG points :(%f, %f, %f)  (%f, %f, %f)\n", 
      	     buff->fPnts[3*a1],buff->fPnts[3*a1+1], buff->fPnts[3*a1+2],
      	     buff->fPnts[3*a2],buff->fPnts[3*a2+1], buff->fPnts[3*a2+2]);
    }
    printf("\n");
    bpols += (Nseg+2);
  }
}

//______________________________________________________________________________
Bool_t NLTProjector::IsFirstIdxHead(Int_t s0, Int_t s1, TBuffer3D* buff)
{
  Int_t v0 = buff->fSegs[3*s0 + 1];
  Int_t v2 = buff->fSegs[3*s1 + 1];
  Int_t v3 = buff->fSegs[3*s1 + 2];
  if(v0 != v2 && v0 != v3 )
    return kTRUE;
  else 
    return kFALSE;
}
//______________________________________________________________________________
void NLTProjector::AddPolygon(std::list<Int_t>& pp, std::list<NLTPolygon>& pols)
{
  if(pp.size() <= 2) return;

  // dimension of bbox
  Float_t bbox[] = {0., 0., 0., 0., 0., 0.};
  for (std::list<Int_t>::iterator u = pp.begin(); u!= pp.end(); u++) 
  {
    Int_t idx = *u; 
    if(fRPnts[idx].x < bbox[0]) bbox[0] = fRPnts[idx].x;   
    if(fRPnts[idx].x > bbox[1]) bbox[1] = fRPnts[idx].x;

    if(fRPnts[idx].y < bbox[2]) bbox[2] = fRPnts[idx].y;   
    if(fRPnts[idx].y > bbox[3]) bbox[3] = fRPnts[idx].y;

    if(fRPnts[idx].z < bbox[4]) bbox[4] = fRPnts[idx].z;   
    if(fRPnts[idx].z > bbox[5]) bbox[5] = fRPnts[idx].z;
  }
  if((bbox[1]-bbox[0])<fEps || (bbox[3]-bbox[2])<fEps) return;
  // printf("bbox (%f, %f) (%f, %f)\n", bbox[1], bbox[0], bbox[3], bbox[2]);

  // duplication
  for (std::list<NLTPolygon>::iterator poi = pols.begin(); poi!= pols.end(); poi++)
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

  // printf("add %d NLTPolygon points %d \n", pols.size(), pp.size());
  Int_t* pv = new Int_t[pp.size()];
  Int_t count=0;
  for( std::list<Int_t>::iterator u = pp.begin(); u!= pp.end(); u++){
    pv[count] = *u;
    count++;
  }
  pols.push_back(NLTPolygon(pp.size(), pv));
}

//______________________________________________________________________________
void  NLTProjector::ReducePoints(Vector* pnts, Int_t N)
{
  fIdxMap   = new Int_t[N];  
  Int_t* ra = new Int_t[N];  // list of reduced vertices
  fNRPnts = 0;
  
  for(UInt_t v = 0; v < (UInt_t)N; v++)
  {
    fIdxMap[v] = -1;
    for(Int_t k = 0; k<fNRPnts; k++) 
    {
      if(pnts[v].SquareDistance(pnts[ra[k]]) < fEps*fEps)
      {
	fIdxMap[v] = k; 
	break;
      }
    } 
    // have not find a point inside epsilon, add new point in scaled array
    if(fIdxMap[v] == -1)
    {
      fIdxMap[v] = fNRPnts;
      ra[fNRPnts] = v;
      fNRPnts++;
    }
    // printf("(%f, %f) vertex map %d -> %d \n", pnts[v*2], pnts[v*2 + 1], v, fIdxMap[v]);
  }
  
  // create an array of scaled points
  fRPnts = new Vector[fNRPnts];
  for(Int_t i = 0; i<fNRPnts; i++)
    fRPnts[i].Set(pnts[ra[i]].x,  pnts[ra[i]].y,  pnts[ra[i]].z);
  
  delete [] ra;  
  // printf("reduced %d points of %d\n", fNRPnts, N);
}

//______________________________________________________________________________
void  NLTProjector::MakePolygonsFromBP(TBuffer3D* buff, std::list<NLTPolygon>& pols)
{
  // build polygons from sorted list of segments : buff->fPols

  //  printf("START NLTProjector::MakePolygonsFromBP\n");
  Int_t* bpols = buff->fPols;
  for(UInt_t pi = 0; pi< buff->NbPols(); pi++) 
  {
    std::list<Int_t>  pp; // points in current polygon 
    UInt_t Nseg = bpols[1]; 
    Int_t* seg =  &bpols[2];
    // start idx in the fist segment depends of second segment 
    Int_t  tail, head;
    Bool_t h = IsFirstIdxHead(seg[0], seg[1], buff);
    if(h) {
      head = fIdxMap[buff->fSegs[3*seg[0] + 1]];
      tail = fIdxMap[buff->fSegs[3*seg[0] + 2]];
    }
    else {
      head = fIdxMap[buff->fSegs[3*seg[0] + 2]];
      tail = fIdxMap[buff->fSegs[3*seg[0] + 1]];
    }
    pp.push_back(head);
    // printf("start idx head %d, tail %d\n", head, tail);
    std::list<Seg> segs;  
    for(UInt_t s=1; s< Nseg; s++)
      segs.push_back(Seg(buff->fSegs[3*seg[s] + 1],buff->fSegs[3*seg[s] + 2]));
    Bool_t accepted = kFALSE; 
    for(std::list<Seg>::iterator it = segs.begin(); it != segs.end(); it++ )
    { 
      Int_t mv1 = fIdxMap[(*it).v1];
      Int_t mv2 = fIdxMap[(*it).v2];         
      accepted = fProjection->AcceptSegment(fRPnts[mv1], fRPnts[mv2], fEps);
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
      AddPolygon(pp, pols);
    }
    bpols += (Nseg+2);
  }
}// MakePolygonsFromBP

//______________________________________________________________________________
void  NLTProjector::MakePolygonsFromBS(TBuffer3D* buff, std::list<NLTPolygon>& pols)
{
  // builds polygons from the set of buffer segments

  // create your own list of segments according to reduced and projected points
  std::list<Seg> segs;  
  std::list<Seg>::iterator it;
  for(UInt_t s=0; s< buff->NbSegs(); s++)
  {
    Bool_t duplicate = kFALSE;
    Int_t vo1, vo2;   // idx from buff segment
    Int_t vor1, vor2; // mapped idx 
    vo1 =  buff->fSegs[3*s + 1];
    vo2 =  buff->fSegs[3*s + 2]; //... skip color info
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
    if(duplicate == kFALSE && fProjection->AcceptSegment(fRPnts[vor1], fRPnts[vor2], fEps))
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
    AddPolygon(pp, pols);
  }
}//MakePolygonsFromBS

 //______________________________________________________________________________
NLTPolygonSet*  NLTProjector::ProjectBuffer3D(TBuffer3D* buff, Int_t useBuffPols)
{ 
  // rewrite from Double_t to Vector array
  // DumpBuffer(buff);  
  Vector*  pnts  = new Vector[buff->NbPnts()];
  for(Int_t i = 0; i<(Int_t)buff->NbPnts(); i++) 
    pnts[i].Set(buff->fPnts[3*i],buff->fPnts[3*i+1], buff->fPnts[3*i+2]);

  fProjection->Project(pnts, buff->NbPnts(), kFALSE);
  ReducePoints(pnts, buff->NbPnts());

  // build polygons
  std::list<NLTPolygon>   pols;
  if(useBuffPols == -1) 
  { 
    MakePolygonsFromBP(buff, pols); 
    if(pols.empty())
    {
      // printf("useBuffPols == -1 call MakePolygonsFromBS \n");
      MakePolygonsFromBS(buff, pols);
    }
  }
  if(useBuffPols == 1)
  {
    MakePolygonsFromBP(buff, pols); 
  }
  if(useBuffPols == 0) 
  {
    MakePolygonsFromBS(buff, pols); 
  }
 
  NLTPolygonSet* ps = new NLTPolygonSet();
  if(pols.empty() == kFALSE) 
  {
    ps->SetPoints(fRPnts,fNRPnts);
    NLTPolygon* nltp = new NLTPolygon[pols.size()];
    Int_t pc = 0;
    for( std::list<NLTPolygon>::iterator pi = pols.begin(); pi!= pols.end(); pi++)
    {
      nltp[pc].fNPnts = (*pi).fNPnts;
      nltp[pc].fPnts = (*pi).fPnts;
      pc++;
    }
    ps->SetPolygons(nltp, pols.size());
    ResetBBox();
    fRPnts = 0; // points are owned by NLTPolygonSet
  }
  CleanUp();
  return ps;
}

//______________________________________________________________________________
void NLTProjector::ProjectPointSet(PointSet* orig)
{
  // keep original rewrite points into Vector array
  Float_t* op = orig->GetP();
  Vector* vp = new Vector[orig->GetN()];
  for( Int_t k=0; k<orig->GetN(); k++)
    vp[k].Set(op[3*k], op[3*k+1], op[3*k+2]);
  Vector* pnts = fProjection->Project(vp, orig->GetN(), kFALSE);
  ReducePoints(pnts, orig->GetN());
  orig->Reset(fNRPnts);
  for(Int_t i = 0; i< fNRPnts; i++){
    orig->SetPoint(i, fRPnts[i].x, fRPnts[i].y, fRPnts[3].z);
  }
  CleanUp();
}

//______________________________________________________________________________
void NLTProjector::RegisterTrack(Track* track, Bool_t recurse)
{
  track->Connect("MakeTrack(Bool_t)", "Reve::NLTProjector", this, "ProjectTrack(Bool_t)");

  if(recurse)
  {
    for(List_i i=track->BeginChildren(); i!=track->EndChildren(); ++i) 
    {
      Track* dt = dynamic_cast<Track*>(*i);
      RegisterTrack(dt, recurse);
    }
  }
}

//______________________________________________________________________________
void NLTProjector::RegisterTrackList(TrackList* tl, Bool_t recurse)
{
  for(List_i i=tl->BeginChildren(); i!=tl->EndChildren(); ++i) 
  {
    Track* track = dynamic_cast<Track*>(*i);
    RegisterTrack(track, recurse);
  }
  tl->MakeTracks(kFALSE);

  // set projected first vertices in track list 
  Float_t x, y, z;
  tl->Reset(tl->GetNChildren());
  for(List_i j=tl->BeginChildren(); j!=tl->EndChildren(); ++j) 
  {
    Track* track = dynamic_cast<Track*>(*j);
    if(track->GetLastPoint() >= 0)
    {
      track->GetPoint(0, x, y, z);
      tl->SetNextPoint(x, y, z);
    }
  }
}

//______________________________________________________________________________
void  NLTProjector::GetBreakPoint(Int_t idx, Bool_t back,  Float_t& x, Float_t& y, Float_t& z)
{
  Vector vL = fPnts[idx];
  Vector vR = fPnts[idx+1];
  Bool_t debug = kTRUE;
  // printf("out of tolerance:%d (%f, %f, %f)(%f, %f, %f) \n",
  //	 N, vL.x, vL.y, vL.z, vR.x, vR.y, vR.z );

  while((vL-vR).Mag() > 0.1)
  { 
    Vector vM = (vL+vR)*0.5;
    Vector vLP  = vL; fProjection->ProjectPoint(vLP.x, vLP.y, vLP.z);
    Vector vMP  = vM; fProjection->ProjectPoint(vMP.x, vMP.y, vMP.z);
    if(fProjection->AcceptSegment(vL, vM, 0.))
    {
      vL = vM;
    }
    else 
    {
      vR = vM;
    }
    // printf("new interval Mag %f (%f, %f, %f)(%f, %f, %f) \n",(vL-vR).Mag(), vL.x, vL.y, vL.z, vR.x, vR.y, vR.z);
  }

  if(back)
    x = vL.x; y = vL.y; z = vL.z;
  else
    x = vR.x; y = vR.y; z = vR.z;
  fProjection->ProjectPoint(x, y, z);
}

//______________________________________________________________________________
Int_t  NLTProjector::GetBreakPointIdx(Int_t start)
{
  Int_t val = fNPnts-1;
  if( fNPnts > 1 )
  {
    Bool_t broken = kFALSE;
    Int_t i = start;
    while(i<fNPnts-1)
    {
      if(fProjection->AcceptSegment(fRPnts[fIdxMap[i]], fRPnts[fIdxMap[i+1]], 0.) == kFALSE)
      {
	broken = kTRUE;
        break;
      }
      i++;
    }
    if(broken) val = i;
  }
  // printf("GetBreakPoint start:%d, break %d,  total:%d \n", start, val, fNPnts);
  return val;
}

//______________________________________________________________________________
Track*  NLTProjector::MakeTrack(Track* tb, Bool_t create, Int_t first, Int_t last)
{
  Track* track;
  if(create) 
  {
    track = new Track(*tb);
    track->SetName(Form("NLT%d_%s", first, tb->GetName()));
    gReve->AddRenderElement(tb, track);
  }
  else
  { 
    track = tb;
  }
  
  // set points
  Int_t i0 = fIdxMap[first];
  Int_t i1 = fIdxMap[last];
  Int_t N =  i1 - i0 +1;

  if(i0 != 0) N++; // add in front if cut
  if(i1 != fNRPnts-1) N++; // add in back if cut 
  track->Reset(N);
  Float_t x, y, z;
  if(i0 != 0)
  {
    GetBreakPoint(first-1, kFALSE,  x, y, z);
    track->SetNextPoint(x, y, z); 
    //printf("ADD POINT to start (%f, %f, %f) \n", x, y, z);  
  }
  for(Int_t i=i0; i<=i1; i++)
  {
    track->SetNextPoint(fRPnts[i].x, fRPnts[i].y, fRPnts[i].z); 
    //printf("set point %d (%f, %f, %f) \n", i, fRPnts[i].x, fRPnts[i].y, fRPnts[i].z);
  }
  if(i1 != fNRPnts-1 )
  {
    GetBreakPoint(last, kTRUE,  x, y, z);
    track->SetNextPoint(x, y, z);  
    // printf("ADD POINT to finish (%f, %f, %f) \n", x, y, z); 
  }
  
  return track;
}

//______________________________________________________________________________
void  NLTProjector::ProjectTrack(Bool_t /*recurse*/)
{
  TQObject* qobj = (TQObject*)gTQSender;
  Track* track = dynamic_cast<Track*>(qobj);
  
  // destroy track fragments created with projection
  List_i bi;
  for(List_i i=track->BeginChildren(); i!=track->EndChildren(); ++i) 
  {
    Track* child = dynamic_cast<Track*>(*i);
    if(child)
    {
      TString name = child->GetName();
      if(name.BeginsWith("NLT"));
      {
        bi = i;
        bi--;
	child->Destroy();
        i = bi;
      }
    }
  }
  
  // track might not have any points
  if(track->GetLastPoint() == -1)
    return;
  
  // project track points store original points
  fNPnts = track->GetLastPoint()+1;
  fPnts = new Vector[fNPnts];
  Float_t* op = track->GetP();
  for( Int_t k=0; k<fNPnts; k++)
    fPnts[k].Set(op[3*k], op[3*k+1], op[3*k+2]);
  Vector* pp = fProjection->Project(fPnts, fNPnts);
  ReducePoints(pp, fNPnts);

  Int_t L, R;// indices of fPnts
  L = R = 0;
  R = GetBreakPointIdx(L);
  MakeTrack(track, kFALSE, L, R);
  L = R + 1;
  // create NLT child tracks from remaining part  
  if(R < fNRPnts-1)
  {
    while(L <fNRPnts)
    {
      R = GetBreakPointIdx(L);
      MakeTrack(track, kTRUE, L, R);
      L = R+1;
    } 
  }
  delete [] fPnts;
  fPnts = 0; fNPnts = 0;
  CleanUp();

  // compute range of path marks
  Track::viPathMark_t& vpm = track->GetVisPathMarksRef();
  for (std::vector<Reve::Vector>::iterator i=vpm.begin(); i!=vpm.end(); ++i)
  {
    fProjection->ProjectPoint((*i).x, (*i).y, (*i).z);
  }
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
  for(List_i i=BeginChildren(); i!=EndChildren(); ++i) {
    NLTPolygonSet* poly = dynamic_cast<NLTPolygonSet*>(*i);
    if(poly)
    {
      for(Int_t k = 0; k<poly->GetNPnts(); k++) 
	BBoxCheckPoint(poly->GetPnt(k));
    }
  }  
}
