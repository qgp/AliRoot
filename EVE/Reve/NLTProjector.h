#ifndef REVE_NLTProjector
#define REVE_NLTProjector

#include <TAtt3D.h>
#include <TAttBBox.h>

#include <Reve/RenderElement.h>

namespace Reve {

class Vector;

/**************************************************************************/
//  NLTProjections
/**************************************************************************/

class NLTProjection
{
public:
  enum PType_e { PT_Unknown, PT_CFishEye, PT_RhoZ }; // , PT_RhoPhi}; 

protected:
  PType_e             fType;
  Float_t             fDistortion; // sensible values from 0 to 0.01
  Float_t             fScale;

public: 
  virtual   Bool_t    AcceptSegment(Vector&, Vector&, Float_t /*tolerance*/) {return kTRUE;}
  virtual   Vector*   Project(Vector* pnts, Int_t npnts, Bool_t create_new = kTRUE);
  virtual   void      ProjectPoint(Float_t&, Float_t&, Float_t&){};

  // required to draw scale on the axis
  virtual   Float_t   PositionToValue(Float_t /*pos*/, Int_t /*axis*/) = 0;
  virtual   Float_t   ValueToPosition(Float_t /*pos*/, Int_t /*axis*/) = 0;

  void      SetDistortion(Float_t d){fDistortion=d; fScale = 1+300*fDistortion;}
  Float_t   GetDistortion(){return fDistortion;}

  void      SetType(PType_e t){fType = t;}
  PType_e   GetType(){return fType;}

  NLTProjection() : fType(PT_Unknown), fDistortion(0), fScale(1.0f) {}
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
private:
  NLTProjector(const NLTProjector&);            // Not implemented
  NLTProjector& operator=(const NLTProjector&); // Not implemented
  
  NLTProjection*  fProjection;

public:
  NLTProjector();
  virtual ~NLTProjector();

  void            SetProjection(NLTProjection::PType_e type, Float_t distort=0);
  void            SetProjection(NLTProjection* p);
  NLTProjection*  GetProjection() { return fProjection; }

  virtual Bool_t  HandleElementPaste(RenderElement* el);

  virtual Bool_t  ShouldImport(RenderElement* rnr_el);
  virtual void    ImportElementsRecurse(RenderElement* rnr_el, RenderElement* parent);
  virtual void    ImportElements(RenderElement* rnr_el);

  virtual void    ProjectChildrenRecurse(RenderElement* rnr_el);
  virtual void    ProjectChildren();

  virtual Bool_t  CanEditMainColor() { return kTRUE; }

  virtual void    ComputeBBox();
  virtual void    Paint(Option_t* option = "");

  ClassDef(NLTProjector, 0); //GUI for editing TGLViewer attributes
};

}
#endif
