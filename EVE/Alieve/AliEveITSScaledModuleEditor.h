// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 * 
 **************************************************************************/

#ifndef ALIEVE_ITSScaledModuleEditor_H
#define ALIEVE_ITSScaledModuleEditor_H

#include <TGedFrame.h>
#include <TEveRGBAPaletteEditor.h>

class TGNumberEntry;
class TGColorSelect;
class TGComboBox;

class TEveGValuator;
class TEveGDoubleValuator;
class TEveRGBAPalette;


class AliEveDigitScaleInfo;
class AliEveITSScaledModule;
class AliITSsegmentation;

/**************************************************************************/

class AliEveITSScaledModuleEditor : public TGedFrame
{
private:
  AliEveITSScaledModuleEditor(const AliEveITSScaledModuleEditor&);            // Not implemented
  AliEveITSScaledModuleEditor& operator=(const AliEveITSScaledModuleEditor&); // Not implemented

  void CreateInfoFrame();

protected:
  TGVerticalFrame*  fInfoFrame;

  AliEveITSScaledModule*  fModule; // fModel dynamic-casted to AliEveITSScaledModuleEditor

  TGNumberEntry*    fScale;
  TGComboBox*       fStatistic;  

  TGLabel*          fInfoLabel0;
  TGLabel*          fInfoLabel1;

public:
  AliEveITSScaledModuleEditor(const TGWindow* p=0, Int_t width=170, Int_t height=30, UInt_t options = kChildFrame, Pixel_t back=GetDefaultFrameBackground());
  virtual ~AliEveITSScaledModuleEditor();

  virtual void   SetModel(TObject* obj);

  void DoScale();
  void DoStatType(Int_t t);

  ClassDef(AliEveITSScaledModuleEditor, 0); // Editor for AliEveITSScaledModule
}; // endclass AliEveITSScaledModuleEditor

#endif
