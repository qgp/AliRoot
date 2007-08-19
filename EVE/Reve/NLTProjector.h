#ifndef REVE_NLTProjector
#define REVE_NLTProjector

#include <TAtt3D.h>
#include <TAttBBox.h>

#include <Reve/RenderElement.h>

namespace std {
template<typename _Tp> class allocator;
template<typename _Tp, typename _Alloc > class list;
}

class TGeoManager;
class TGeoMatrix;
class TBuffer3D;

namespace Reve {
class Vector;
class PointSet;
class NLTPolygonSet;
class NLTPolygon;
class Track;
class TrackList;
/**************************************************************************/
//  NLTProjections
/**************************************************************************/

class NLTProjection
{
public: 
  enum PType_e { PT_Unknown, PT_CFishEye, PT_RhoZ }; // , PT_RhoPhi}; 

  PType_e             fType;
  Float_t             fDistortion; // sensible values from 0 to 0.01
 
  virtual   Bool_t    AcceptSegment(Vector&, Vector&, Float_t /*tolerance*/) {return kTRUE;}
  virtual   Vector*   Project(Vector* pnts, Int_t npnts, Bool_t create_new = kTRUE);
  virtual   void      ProjectPoint(Float_t&, Float_t&, Float_t&){};

  // required to draw scale on the axis
  virtual   Float_t   PositionToValue(Float_t /*pos*/, Int_t /*axis*/) = 0;
  virtual   Float_t   ValueToPosition(Float_t /*pos*/, Int_t /*axis*/) = 0;

  NLTProjection() : fType(PT_Unknown), fDistortion(0) {}
  virtual ~NLTProjection() {}

  ClassDef(NLTProjection, 0);
};

class RhoZ: public NLTProjection
{
public:
  RhoZ() : NLTProjection() { fType = PT_RhoZ; }
  virtual ~RhoZ() {}

  virtual   Bool_t    AcceptSegment(Vector& v1, Vector& v2, Float_t tolerance); 
  virtual   void      ProjectPoint(Float_t& x, Float_t& y, Float_t& z);

  // required to draw scale on the axis
  virtual   Float_t   PositionToValue(Float_t a, Int_t /*axis*/){ return a/(1-TMath::Abs(a)*fDistortion); }
  virtual   Float_t   ValueToPosition(Float_t a, Int_t /*axis*/){ return a/(1+TMath::Abs(a)*fDistortion); }

  ClassDef(RhoZ, 0);
};

class CircularFishEye : public NLTProjection
{
public:
  CircularFishEye():NLTProjection() { fType = PT_CFishEye; }
  virtual ~CircularFishEye() {}

  virtual   void      ProjectPoint(Float_t& x, Float_t& y, Float_t& z); 

  // required to draw scale on the axis
  virtual   Float_t   PositionToValue(Float_t a, Int_t /*axis*/){ return a/(1-TMath::Abs(a)*fDistortion); }
  virtual   Float_t   ValueToPosition(Float_t a, Int_t /*axis*/){ return a/(1+TMath::Abs(a)*fDistortion); }

  ClassDef(CircularFishEye, 0);
};

/**************************************************************************/
//  NLTProjector
/**************************************************************************/
class NLTProjector : public RenderElementList,
		     public TAttBBox,
                     public TAtt3D
{ 
  NLTProjector(const NLTProjector&);            // Not implemented
  NLTProjector& operator=(const NLTProjector&); // Not implemented

public:

protected:
  Int_t  GetBreakPointIdx(Int_t start);
  void   GetBreakPoint(Int_t N, Bool_t back, Float_t& x, Float_t& y, Float_t& z);
  Bool_t IsFirstIdxHead(Int_t s0, Int_t s1, TBuffer3D* buff);
  void   AddPolygon( std::list<Int_t, std::allocator<Int_t> >& pp, std::list<NLTPolygon, std::allocator<NLTPolygon> >& pols);

  Track* MakeTrack(Track* track, Bool_t create, Int_t start_idx, Int_t end_idx);

private:
  NLTProjection*  fProjection;

  Float_t         fEps;    // distance accounted in reducing the ponts

  // temporary variables cashed 
  Int_t*          fIdxMap; // map from original to projected and reduced point needed oly for geometry
  Int_t           fNPnts; // number of reduced and projected points
  Vector*         fPnts;  // reduced and projected points
  Int_t           fNRPnts; // number of reduced and projected points
  Vector*         fRPnts;  // reduced and projected points

  void            ReducePoints(Vector* p, Int_t N);
  void            MakePolygonsFromBP(TBuffer3D* buff, std::list<NLTPolygon, std::allocator<NLTPolygon> >& pols);
  void            MakePolygonsFromBS(TBuffer3D* buff, std::list<NLTPolygon, std::allocator<NLTPolygon> >& pols);
  void            CleanUp();

public:
  NLTProjector();
  virtual ~NLTProjector();

  NLTPolygonSet*  ProjectBuffer3D(TBuffer3D* buff, Int_t useBuffPols=-1);
  void            ProjectPointSet(PointSet* ps);
  void            ProjectTrack(Bool_t recurse);
  void            RegisterTrack(Track* track, Bool_t recurse=kTRUE);
  void            RegisterTrackList(TrackList* tl, Bool_t recurse=kTRUE);
  
  void            SetProjection(NLTProjection::PType_e type, Float_t distort=0);
  void            SetProjection(NLTProjection* p);
  NLTProjection*  GetProjection() { return fProjection; }

  virtual Bool_t  ShouldImport(RenderElement* rnr_el);
  virtual void    ImportElementsRecurse(RenderElement* rnr_el, RenderElement* parent);
  virtual void    ImportElements(RenderElement* rnr_el);

  virtual void    ProjectChildrenRecurse(RenderElement* rnr_el);
  virtual void    ProjectChildren();

  void            DumpBuffer(TBuffer3D* b);

  virtual Bool_t  CanEditMainColor() { return kTRUE; }

  virtual void    ComputeBBox();
  virtual void    Paint(Option_t* option = "");

  ClassDef(NLTProjector, 0); //GUI for editing TGLViewer attributes
};

}
#endif
