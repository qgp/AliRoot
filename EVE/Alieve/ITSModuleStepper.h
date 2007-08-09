// $Header$

#ifndef ALIEVE_ITSModuleStepper_H
#define ALIEVE_ITSModuleStepper_H

#include <TNamed.h>
#include <TGLOverlay.h>

#include <Reve/RenderElement.h>
#include <Reve/GridStepper.h>

#include <vector>

class TGLText;
class TGLAxis;

namespace Alieve {

class ITSDigitsInfo;
class DigitScaleInfo;

class ITSModuleStepper : public Reve::RenderElement,
                         public TGLOverlayElement,
                         public TNamed
{
  friend class ITSModuleStepperGL;

public:
  typedef std::vector<UInt_t>           vpInt_t;
  typedef std::vector<UInt_t>::iterator vpInt_i;

private:
  vpInt_t                 fIDs;
  UInt_t                  fPosition;  // position of top corner ITS module in vector fIDs

  ITSModuleStepper(const ITSModuleStepper&);            // Not implemented
  ITSModuleStepper& operator=(const ITSModuleStepper&); // Not implemented

protected:
  ITSDigitsInfo*          fDigitsInfo;
  DigitScaleInfo*         fScaleInfo;
  Int_t                   fSubDet;

  Reve::GridStepper*      fStepper;
  TGLAxis*                fAxis;
  TGLText*                fText;
  Float_t                 fTextSize;
  Float_t                 fPagerGap;
  Bool_t                  fRnrFrame;

  // module configuration
  Float_t                 fExpandCell;
  Color_t                 fModuleFrameCol;

  // palette configuratiom
  Float_t                 fPaletteOffset;
  Float_t                 fPaletteLength;  

  // symbol configuration 
  Int_t                   fWActive; 
  Float_t                 fWWidth;
  Float_t                 fWHeight;
  Float_t                 fWOff; ///offset relative to widget size
  Color_t                 fWCol;  
  Int_t                   fWActiveCol;
  Color_t                 fFontCol;

  // wrappers
  void   RenderFrame(Float_t dx, Float_t dy, Int_t id);
  void   RenderSymbol(Float_t dx, Float_t dy, Int_t id);
  void   RenderString(TString tex ,Int_t id = -1);
  void   RenderPalette(Float_t dx, Float_t x, Float_t y);
  void   RenderMenu();
  void   RenderCellIDs();
  Float_t TextLength(const char* txt);

  // module ID navigation
  Int_t  Nxy(){ return fStepper->Nx*fStepper->Ny; }
  void   AddToList( Int_t modID ){ fIDs.push_back(modID);}
  void   ResetList(){ fIDs.clear();}
  void   SetFirst(Int_t first);
  void   Apply();

public:
  ITSModuleStepper(ITSDigitsInfo* di);
  virtual ~ITSModuleStepper();

  // external functions
  void     DisplayDet(Int_t det, Int_t layer = -1);
  void     DisplayTheta(Float_t min, Float_t max);

  // overlay functions
  virtual  Bool_t MouseEnter(TGLOvlSelectRecord& selRec);
  virtual  Bool_t Handle(TGLRnrCtx& rnrCtx, TGLOvlSelectRecord& selRec,
                        Event_t* event);
  virtual void   MouseLeave();
  virtual void   Render(TGLRnrCtx& rnrCtx);

  // stepper
  void     ConfigStepper(Int_t nx, Int_t ny);
  Int_t    GetCurrentPage();
  Int_t    GetPages();
  void     Start();
  void     Next();
  void     Previous();
  void     End();

  // RenderElement
  virtual  Bool_t CanEditMainColor() { return kTRUE; }

  // getters/setters
  Color_t  GetWColor(){ return fWCol; };
  void     SetWColor(Color_t c){ fWCol = c; }
  TGLText* GetFont(){ return fText; }
  void     SetGLText(TGLText* t) {fText = t;}

  ClassDef(ITSModuleStepper, 0);
};

} // Alieve namespace

#endif
