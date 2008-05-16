// @(#)root/eve:$Id$
// Author: Matevz Tadel 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

#ifndef AliEveEventManagerEditor_H
#define AliEveEventManagerEditor_H

#include "TGedFrame.h"

class TGCheckButton;
class TEveGValuator;
class TGTextView;

class AliEveEventManager;

//______________________________________________________________________________
// Short description of AliEveEventManagerEditor
//

class AliEveEventManagerEditor : public TGedFrame
{
public:
   AliEveEventManagerEditor(const TGWindow* p=0, Int_t width=170, Int_t height=30,
         UInt_t options=kChildFrame, Pixel_t back=GetDefaultFrameBackground());
   virtual ~AliEveEventManagerEditor() {}

   virtual void SetModel(TObject* obj);

   // Declare callback/slot methods
   void DoSetAutoLoad();
   void DoSetAutoLoadTime();
   void DoPrevEvent();
   void DoNextEvent();

protected:
   AliEveEventManager            *fM; // Model object.

   // Declare widgets
   TGCheckButton    *fAutoLoad;  // Check-box for automatic loading of events
   TEveGValuator    *fAutoLoadTime; // Time for automatic loading of events

   TGTextButton     *fNextEvent; // Load next event
   TGTextButton     *fPrevEvent; // Load previous event

   TGTextView       *fEventInfo; // Text box with event info

private:
   AliEveEventManagerEditor(const AliEveEventManagerEditor&);            // Not implemented
   AliEveEventManagerEditor& operator=(const AliEveEventManagerEditor&); // Not implemented

   ClassDef(AliEveEventManagerEditor, 0); // GUI editor for AliEveEventManager.
};

#endif
