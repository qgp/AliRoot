// $Header$

#ifndef REVE_QuadSetEditor_H
#define REVE_QuadSetEditor_H

#include <TGedFrame.h>

class TGCheckButton;
class TGNumberEntry;
class TGColorSelect;

namespace Reve {

class QuadSet;

class RGValuator;
class RGDoubleValuator;
class ZTransSubEditor;

// It would be better to have button to change model to the palette
// object itself.
class RGBAPaletteSubEditor;

class QuadSetEditor : public TGedFrame
{
private:
  QuadSetEditor(const QuadSetEditor&);            // Not implemented
  QuadSetEditor& operator=(const QuadSetEditor&); // Not implemented

  void CreateInfoTab();
protected:
  QuadSet*              fM; // fModel dynamic-casted to QuadSetEditor

  ZTransSubEditor*      fHMTrans;
  RGBAPaletteSubEditor* fPalette;

  TGHorizontalFrame*    fHistoButtFrame;
  TGVerticalFrame*      fInfoFrame;

public:
  QuadSetEditor(const TGWindow* p=0, Int_t width=170, Int_t height=30, UInt_t options = kChildFrame, Pixel_t back=GetDefaultFrameBackground());
  virtual ~QuadSetEditor();

  virtual void SetModel(TObject* obj);

  // Declare callback/slot methods
  void DoHisto();
  void DoRangeHisto();
  void PlotHisto(Int_t min, Int_t max);

  ClassDef(QuadSetEditor, 1); // Editor for QuadSet
}; // endclass QuadSetEditor

}

#endif
