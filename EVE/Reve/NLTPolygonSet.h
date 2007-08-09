#ifndef REVE_NLTPolygonSet_H
#define REVE_NLTPolygonSet_H

#include <Reve/RenderElement.h>

#include "TNamed.h"
#include "TAtt3D.h"
#include "TAttBBox.h"
#include "TColor.h"
#include "PODs.h"

namespace Reve {

class Vector;

class NLTPolygon
{
public:
  Int_t     fNPnts;
  Int_t*    fPnts; 

  NLTPolygon() : fNPnts(0), fPnts(0) {}
  NLTPolygon(Int_t n, Int_t* p) : fNPnts(n), fPnts(p) {}
  NLTPolygon(const NLTPolygon& x) : fNPnts(x.fNPnts), fPnts(x.fPnts) {}
  virtual ~NLTPolygon() {}

  NLTPolygon& operator=(const NLTPolygon& x)
  { fNPnts = x.fNPnts; fPnts = x.fPnts; return *this; }

  ClassDef(NLTPolygon, 0)
};


class NLTPolygonSet :  public Reve::RenderElementList,
		       public TAtt3D, 
                       public TAttBBox
{
  friend class NLTPolygonSetGL;
  friend class NLTPolygonSetEditor;

  NLTPolygonSet(const NLTPolygonSet&);            // Not implemented
  NLTPolygonSet& operator=(const NLTPolygonSet&); // Not implemented

protected:
  Int_t        fNPnts;
  Vector*      fPnts;

  Int_t        fNPols; // number of polygons
  NLTPolygon*  fPols;  // vector of polygon structs

  Color_t      fFillColor;  
  Color_t      fLineColor;
  Float_t      fLineWidth;

  UChar_t      fTransparency;

  Float_t      fZDepth;
public:
  NLTPolygonSet(const Text_t* n="NLTPolygonSet", const Text_t* t="");
  virtual ~NLTPolygonSet();

  virtual void ComputeBBox();
  virtual void Paint(Option_t* option = "");

  void SetPoints(Vector* p, Int_t n) {fPnts = p; fNPnts = n;}
  void SetPolygons(NLTPolygon* p, Int_t n){fPols=p; fNPols=n;}

  virtual Bool_t CanEditMainColor()        { return kTRUE; }

  // virtual Color_t GetFillColor() const { return fFillColor; }
  virtual Color_t GetLineColor() const { return fLineColor; }
  
  virtual Bool_t  CanEditMainTransparency()      { return kTRUE; }
  virtual UChar_t GetMainTransparency() const    { return fTransparency; }
  virtual void    SetMainTransparency(UChar_t t) { fTransparency = t; }  

  Int_t GetSize(){return fNPols;}
  virtual void Dump() const;

  virtual void SetFillColor(Pixel_t pixel) { fFillColor = Color_t(TColor::GetColor(pixel));}
  virtual void SetLineColor(Pixel_t pixel) { fLineColor = Color_t(TColor::GetColor(pixel));}

  virtual void SetFillColor(Color_t c) { fFillColor = c; }
  virtual void SetLineColor(Color_t c) { fLineColor = c; }

  virtual void SetZDepth(Float_t z){fZDepth = z;}
  virtual void SetLineWidth(Double_t lw){fLineWidth = lw;}

  Int_t        GetNPnts(){return fNPnts;}
  Float_t*     GetPnt(Int_t i) {return fPnts[i].c_vec();}

  ClassDef(NLTPolygonSet,0) 
    }; // endclass NLTPolygonSet
} // namespace Reve

#endif
