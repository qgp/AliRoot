#include "RGTopFrame.h"

#include <Reve/Viewer.h>
#include <Reve/Scene.h>
#include <Reve/Pad.h>
#include <Reve/EventBase.h>

#include <Reve/RGBrowser.h>
#include <Reve/RGEditor.h>

#include <TGMenu.h>
#include <TGTab.h>
#include <TGToolBar.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TGSplitter.h>
#include <TRootEmbeddedCanvas.h>
#include <TGMimeTypes.h>

#include <TGLSAViewer.h>

#include <TROOT.h>
#include <TFile.h>
#include <TMacro.h>
#include <TFolder.h>
#include <TStyle.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TRint.h>
#include <TVirtualX.h>
#include <TEnv.h>
#include <TStyle.h>
#include <TColor.h>
#include <TGeoShape.h>
#include <KeySymbols.h>
#include "TVirtualGL.h"
#include "TPluginManager.h"

#include <iostream>

using namespace Reve;
using namespace Reve;

Reve::RGTopFrame* gReve = 0;

/**************************************************************************/

RGTopFrame::RGTopFrame(const TGWindow *p, UInt_t w, UInt_t h, LookType_e look) :
  TGMainFrame(p, w, h),

  fMasterFrame (0),
  fMasterTab   (0),
  fBrowser     (0),
  fStatusBar   (0),

  fMacroFolder (0),
  fEditor      (0),

  fViewers        (0),
  fScenes         (0),
  fViewer         (0),
  fGlobalScene    (0),
  fEventScene     (0),
  fCurrentEvent   (0),

  fRedrawDisabled (0),
  fResetCameras   (kFALSE),
  fDropLogicals   (kFALSE),
  fTimerActive    (kFALSE),
  fRedrawTimer    (),

  fLook           (LT_Editor),
  fGeometries     ()
{
  gReve = this;
  fRedrawTimer.Connect("Timeout()", "Reve::RGTopFrame", this, "DoRedraw3D()");
  fMacroFolder = new TFolder("EVE", "Visualization macros");
  gROOT->GetListOfBrowsables()->Add(fMacroFolder);

  fClient->GetMimeTypeList()->AddType("root/tmacro", "Reve::RMacro",
                                      "tmacro_s.xpm", "tmacro_t.xpm", "");

  // Build GUI

  TGLayoutHints *lay0 = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY | kLHintsExpandY | kLHintsExpandX);
  TGLayoutHints *lay1 = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY | kLHintsExpandY | kLHintsExpandX, 2, 0, 2, 2);
  TGLayoutHints *lay2 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2);

  fMasterFrame = new TGCompositeFrame(this, w, h, kHorizontalFrame | kRaisedFrame);
  TGVerticalFrame* fv2 = new TGVerticalFrame(fMasterFrame, GetWidth()-40, GetHeight()-40, kSunkenFrame);

  fMasterFrame->AddFrame(fv2, lay1);

  fMasterTab = new TGTab(fv2, GetWidth(), GetHeight());  

  // browser tab
  TGCompositeFrame* tframe1 = fMasterTab->AddTab("Object Browser");
  fBrowser = new RGBrowser(tframe1, w, h);
  tframe1->AddFrame(fBrowser, lay2);

  fv2->AddFrame(fMasterTab, lay0);
  AddFrame(fMasterFrame, lay0);
   
  // Create status bar
  Int_t parts[] = {45, 45, 10};
  fStatusBar = new TGStatusBar(this, 50, 10, kHorizontalFrame);
  fStatusBar->SetParts(parts, 3);
  TGLayoutHints* lay6 = new TGLayoutHints(kLHintsBottom| kLHintsExpandX, 0, 0, 0, 0);
  AddFrame(fStatusBar, lay6);
  fStatusBar->SetText("GUI created", 0);

  MapSubwindows();

  /**************************************************************************/
  /**************************************************************************/
  TGLViewer* glv = 0;
  switch(look) {

  case LT_Editor: {
    fBrowser->SetupEditorLook(fEditor);
    glv = new TGLSAViewer(0, 0, fEditor);
    break;
  }

  case LT_GLViewer: {
    glv = fBrowser->SetupGLViewerLook(fEditor);
    break;
  }

  default: {
    printf("RGTopFrame unknown look-type, ignoring.\n");
    break;
  }
  }

  fViewers = new ViewerList("Viewers");
  fViewers->SetDenyDestroy(kTRUE);
  AddToListTree(fViewers, kTRUE);

  fViewer  = new Viewer("GL-One");
  fViewer->SetGLViewer(glv);
  fViewer->SetDenyDestroy(kTRUE);
  AddRenderElement(fViewer, fViewers);

  fScenes  = new SceneList ("Scenes");
  fScenes->SetDenyDestroy(kTRUE);
  AddToListTree(fScenes, kTRUE);

  fGlobalScene = new Scene("Geometry scene");
  fGlobalScene->SetDenyDestroy(kTRUE);
  AddRenderElement(fGlobalScene, fScenes);

  fEventScene = new Scene("Event scene");
  fEventScene->SetDenyDestroy(kTRUE);
  AddRenderElement(fEventScene, fScenes);

  fViewer->AddScene(fGlobalScene);
  fViewer->AddScene(fEventScene);

  /**************************************************************************/
  /**************************************************************************/

  Resize(GetDefaultSize()); // this is used here to init layout algorithm
  SetWindowName("Reve");
  MapWindow();

  fEditor->DisplayObject(0);

  gSystem->ProcessEvents();

  printf("initiating first redraw ...\n");
  Redraw3D(kTRUE);
}

/**************************************************************************/

TCanvas* RGTopFrame::AddCanvasTab(const char* name)
{
  TGCompositeFrame    *f  = fMasterTab->AddTab(name);
  TRootEmbeddedCanvas *ec = new TRootEmbeddedCanvas(name, f, 580, 360);
  f->AddFrame(ec, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY,
				    2, 2, 2, 2));

  f->MapSubwindows();
  f->MapWindow();
  fMasterTab->GetTabTab(name)->MapWindow();
  fMasterTab->Layout();
  
  return ec->GetCanvas();
}

TGLViewer* RGTopFrame::GetGLViewer() const
{
  return fViewer->GetGLViewer();
}

Viewer* RGTopFrame::SpawnNewViewer(const Text_t* name, const Text_t* title)
{
  Viewer* v = new Viewer(name, title);
  v->SpawnGLViewer(0, 0); // Could pass fEditor.
  v->SetDenyDestroy(kTRUE);
  AddRenderElement(v, fViewers);
  return v;
}
Scene* RGTopFrame::SpawnNewScene(const Text_t* name, const Text_t* title)
{
  Scene* s = new Scene(name, title);
  s->SetDenyDestroy(kTRUE);
  AddRenderElement(s, fScenes);
  return s;
}

/**************************************************************************/
// Macro management
/**************************************************************************/

TMacro* RGTopFrame::GetMacro(const Text_t* name) const
{
  return dynamic_cast<TMacro*>(fMacroFolder->FindObject(name));
}

/**************************************************************************/
// Editor
/**************************************************************************/

void RGTopFrame::EditRenderElement(RenderElement* rnr_element)
{
  static const Exc_t eH("RGTopFrame::EditRenderElement ");

  fEditor->DisplayRenderElement(rnr_element);
}

/**************************************************************************/
// 3D Pad management
/**************************************************************************/

void RGTopFrame::RegisterRedraw3D()
{
  fRedrawTimer.Start(0, kTRUE);
  fTimerActive = true;
}

void RGTopFrame::DoRedraw3D()
{
  // printf("RGTopFrame::DoRedraw3D redraw triggered\n");

  fScenes ->RepaintChangedScenes();
  fViewers->RepaintChangedViewers(fResetCameras, fDropLogicals);

  fResetCameras = kFALSE;
  fDropLogicals = kFALSE;

  fTimerActive = kFALSE;
}

void RGTopFrame::ElementChanged(RenderElement* rnr_element)
{
  RenderElement::List_t scenes;
  rnr_element->CollectSceneParents(scenes);
  for (RenderElement::List_i s=scenes.begin(); s!=scenes.end(); ++s)
    ((Scene*)*s)->Changed();
}

/**************************************************************************/

int RGTopFrame::SpawnGuiAndRun(int argc, char **argv)
{
  LookType_e revemode = LT_GLViewer;
  Int_t w = 540;
  Int_t h = 500;
  if(argc >= 3 && (strcmp(argv[1], "-revemode")==0 || strcmp(argv[1], "-mode")==0)) {
    LookType_e m = LookType_e(atoi(argv[2]));
    if(m > LT_Unknown && m <= LT_GLViewer)
      revemode = m;
    printf("revemode = %d\n", revemode);
    if(revemode == LT_GLViewer) {
      w = 1024; h = 768;
    }
  }

  TRint theApp("App", &argc, argv);

  Reve::SetupGUI();
  /* gReve = */ new RGTopFrame(gClient ? gClient->GetRoot() : 0, w, h, revemode);

 run_loop:
  try {
    theApp.Run();
  }
  catch(Exc_t& exc) {
    gReve->GetStatusBar()->SetText(exc.Data());
    fprintf(stderr, "Exception: %s\n", exc.Data());
    goto run_loop;
  }
  return 0;
}

void RGTopFrame::SpawnGui(LookType_e revemode)
{
  Int_t w = 540;
  Int_t h = 500;
  if(revemode == LT_GLViewer) {
    w = 1024; h = 768;
  }

  Reve::SetupGUI();
  /* gReve = */ new RGTopFrame(gClient->GetRoot(), w, h, revemode);
}

/**************************************************************************/
/**************************************************************************/

TGListTree* RGTopFrame::GetListTree() const
{
  return fBrowser->GetListTree();
}

TGListTreeItem*
RGTopFrame::AddToListTree(RenderElement* re, Bool_t open, TGListTree* lt)
{
  if (lt == 0) lt = GetListTree();
  TGListTreeItem* lti = re->AddIntoListTree(lt, (TGListTreeItem*)0);
  if (open) lt->OpenItem(lti);
  return lti;
}

TGListTreeItem* RGTopFrame::AddEvent(EventBase* event)
{
  fCurrentEvent = event;
  fCurrentEvent->SetDenyDestroy(kTRUE);
  AddRenderElement(fCurrentEvent, fEventScene);
  return AddToListTree(event, kTRUE);
}

TGListTreeItem* RGTopFrame::AddRenderElement(RenderElement* rnr_element,
					     RenderElement* parent)
{
  static const Exc_t eH("RGTopFrame::AddRenderElement ");

  if (parent && ! parent->AcceptRenderElement(rnr_element)) {
    throw(eH + "parent '%s' rejects '%s'.",
	  parent->GetObject()->GetName(), rnr_element->GetObject()->GetName());
  }

  if (parent == 0) {
    if (fCurrentEvent == 0)
      AddEvent(new EventBase("Event", "Auto-created event directory"));
    parent = fCurrentEvent;
  }

  parent->AddElement(rnr_element);

  TGListTreeItem* newitem =
    rnr_element->AddIntoListTrees(parent);

  return newitem;
}

TGListTreeItem* RGTopFrame::AddGlobalRenderElement(RenderElement* rnr_element,
						   RenderElement* parent)
{
  static const Exc_t eH("RGTopFrame::AddGlobalRenderElement ");

  if (parent && ! parent->AcceptRenderElement(rnr_element)) {
    throw(eH + "parent '%s' rejects '%s'.",
	  parent->GetObject()->GetName(), rnr_element->GetObject()->GetName());
  }

  if (parent == 0)
    parent = fGlobalScene;

  parent->AddElement(rnr_element);

  TGListTreeItem* newitem =
    rnr_element->AddIntoListTrees(parent);

  return newitem;
}

/**************************************************************************/

void RGTopFrame::RemoveRenderElement(RenderElement* rnr_element,
				     RenderElement* parent)
{
  rnr_element->RemoveFromListTree(GetListTree());

  parent->RemoveElement(rnr_element);
}

void RGTopFrame::PreDeleteRenderElement(RenderElement* rnr_element)
{
  if (fEditor->GetRnrElement() == rnr_element)
    fEditor->DisplayObject(0);
}

/**************************************************************************/

void RGTopFrame::RenderElementSelect(RenderElement* rnr_element)
{
  EditRenderElement(rnr_element);
}

void RGTopFrame::RenderElementPaste(RenderElement* rnr_element)
{
  RenderElement* src = fEditor->GetRnrElement();
  if (src)
    rnr_element->HandleElementPaste(src);
}

void RGTopFrame::RenderElementChecked(TObject* obj, Bool_t state)
{
  // Item's user-data is blindly casted into TObject.
  // We recast it blindly back into the render element.
  // State is ignored as it is kept consistent with internal fRnrSelf and
  // fRnrChildren.

  RenderElement* rnrEl = (RenderElement*) obj;
  rnrEl->SetRnrState(state);

  if(fEditor->GetModel() == rnrEl->GetObject()) {
    fEditor->DisplayObject(rnrEl->GetObject());
  }

  ElementChanged(rnrEl);
  Redraw3D();
}

/**************************************************************************/

void RGTopFrame::NotifyBrowser(TGListTreeItem* parent_lti)
{
  TGListTree* l_tree = GetListTree();
  if(parent_lti)
    l_tree->OpenItem(parent_lti);
  gClient->NeedRedraw(l_tree);
}

void RGTopFrame::NotifyBrowser(RenderElement* parent)
{
  TGListTreeItem* parent_lti = parent ? parent->FindListTreeItem(GetListTree()) : 0;
  NotifyBrowser(parent_lti);
}

/**************************************************************************/
// GeoManager registration
/**************************************************************************/

TGeoManager* RGTopFrame::GetGeometry(const TString& filename)
{
  static const Exc_t eH("RGTopFrame::GetGeometry ");

  TString exp_filename = filename;
  gSystem->ExpandPathName(exp_filename);
  printf("%s loading: '%s' -> '%s'.\n", eH.Data(),
	 filename.Data(), exp_filename.Data());

  std::map<TString, TGeoManager*>::iterator g = fGeometries.find(filename);
  if(g != fGeometries.end()) {
    return g->second;
  } else {
    if(gSystem->AccessPathName(exp_filename, kReadPermission))
      throw(eH + "file '" + exp_filename + "' not readable.");
    gGeoManager = 0;
    TGeoManager::Import(filename); 
    if(gGeoManager == 0)
      throw(eH + "GeoManager import failed.");
    gGeoManager->GetTopVolume()->VisibleDaughters(1);

    // Import colors exported by Gled, if they exist.
    {
      TFile f(exp_filename, "READ");
      TObjArray* collist = (TObjArray*) f.Get("ColorList");
      f.Close();
      if(collist != 0) {
	TIter next(gGeoManager->GetListOfVolumes());
	TGeoVolume* vol;
	while ((vol = (TGeoVolume*) next()) != 0)
	{
          Int_t oldID = vol->GetLineColor();
          TColor* col = (TColor*)collist->At(oldID);
          Float_t r, g, b;
	  col->GetRGB(r, g, b);
          Int_t  newID = TColor::GetColor(r,g,b);
          vol->SetLineColor(newID);
	} 
      }
    }

    fGeometries[filename] = gGeoManager;
    return gGeoManager;
  }
}

/**************************************************************************/
// Testing exceptions
/**************************************************************************/

void RGTopFrame::ThrowException(const char* text)
{
  static const Exc_t eH("RGTopFrame::ThrowException ");

  throw(eH + text);
}
