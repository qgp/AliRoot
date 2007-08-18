// $Header$

#ifndef REVE_RGTopFrame_H
#define REVE_RGTopFrame_H

#include <TClass.h>
#include <TGFrame.h>
#include <TGStatusBar.h>
#include <TGeoManager.h>
#include <TROOT.h>
#include <TTimer.h>
#include <TVirtualPad.h>

#include <map>

class TMacro;
class TFolder;
class TCanvas;

class TGTab;
class TGStatusBar;
class TGListTree;
class TGListTreeItem;

class TGLViewer;

namespace Reve {

class RGBrowser;
class RGEditor;

class RenderElement;
class PadPrimitive;

class Viewer; class ViewerList;
class Scene;  class SceneList;

class EventBase;


class RGTopFrame : public TGMainFrame
{
  RGTopFrame(const RGTopFrame&);            // Not implemented
  RGTopFrame& operator=(const RGTopFrame&); // Not implemented

public:
  enum LookType_e { LT_Classic, LT_Editor, LT_GLViewer };

  class RedrawDisabler
  {
  private:
    RedrawDisabler(const RedrawDisabler&);            // Not implemented
    RedrawDisabler& operator=(const RedrawDisabler&); // Not implemented

    RGTopFrame* fFrame;
  public:
    RedrawDisabler(RGTopFrame* f) : fFrame(f)
    { if (fFrame) fFrame->DisableRedraw(); }
    ~RedrawDisabler()
    { if (fFrame) fFrame->EnableRedraw(); }
  };

private:
  TGCompositeFrame    *fMasterFrame;
  TGTab               *fMasterTab;

  RGBrowser           *fBrowser;
  TGStatusBar         *fStatusBar;

  TFolder             *fMacroFolder;

  RGEditor            *fEditor;

  ViewerList          *fViewers;
  SceneList           *fScenes;

  Viewer              *fViewer;   // First / default gl-viewer.
  Scene               *fGlobalScene;
  Scene               *fEventScene;
  EventBase           *fCurrentEvent;

  Int_t                fRedrawDisabled;
  Bool_t               fFullRedraw;
  Bool_t               fResetCameras;
  Bool_t               fDropLogicals;
  Bool_t               fTimerActive;
  TTimer               fRedrawTimer;

protected:
  LookType_e           fLook;

  std::map<TString, TGeoManager*> fGeometries;

public:
  RGTopFrame(const TGWindow *p, UInt_t w, UInt_t h, LookType_e look=LT_Classic);

  TGCompositeFrame* GetMasterFrame() { return fMasterFrame; }
  TGTab*            GetMasterTab()   { return fMasterTab; }

  RGBrowser*   GetBrowser()    { return fBrowser; }
  TGStatusBar* GetStatusBar()  { return fStatusBar; }

  SceneList*   GetScenes()   const { return fScenes; }
  ViewerList*  GetViewers()  const { return fViewers; }

  Viewer*      GetDefViewer()    const { return fViewer; }
  Scene*       GetGlobalScene()  const { return fGlobalScene; }
  Scene*       GetEventScene()   const { return fEventScene; }
  EventBase*   GetCurrentEvent() const { return fCurrentEvent; }

  TCanvas*     AddCanvasTab(const char* name);
  TGLViewer*   GetGLViewer() const;
  Viewer*      SpawnNewViewer(const Text_t* name, const Text_t* title="");
  Scene*       SpawnNewScene(const Text_t* name, const Text_t* title="");

  TFolder*  GetMacroFolder() const { return fMacroFolder; }
  TMacro*   GetMacro(const Text_t* name) const;

  RGEditor* GetEditor() const { return fEditor; }
  void EditRenderElement(RenderElement* rnr_element);

  void DisableRedraw() { ++fRedrawDisabled; }
  void EnableRedraw()  { --fRedrawDisabled; if(fRedrawDisabled <= 0) Redraw3D(); }

  void Redraw3D(Bool_t resetCameras=kFALSE, Bool_t dropLogicals=kFALSE)
  {
    if(fRedrawDisabled <= 0 && !fTimerActive) RegisterRedraw3D();
    if(resetCameras) fResetCameras = kTRUE;
    if(dropLogicals) fDropLogicals = kTRUE;
  }
  void RegisterRedraw3D();
  void DoRedraw3D();

  void ElementChanged(RenderElement* rnr_element);

  static int  SpawnGuiAndRun(int argc, char **argv);
  static void SpawnGui(LookType_e revemode = LT_Editor);

  // These are more like ReveManager stuff.
  TGListTree*     GetListTree() const;
  TGListTreeItem* AddToListTree(RenderElement* re, Bool_t open, TGListTree* lt=0);
  TGListTreeItem* AddEvent(EventBase* event);
  TGListTreeItem* AddRenderElement(RenderElement* rnr_element,
				   RenderElement* parent=0);
  TGListTreeItem* AddGlobalRenderElement(RenderElement* rnr_element,
					 RenderElement* parent=0);

  void RemoveRenderElement(RenderElement* rnr_element, RenderElement* parent);
  void PreDeleteRenderElement(RenderElement* rnr_element);

  void RenderElementSelect(RenderElement* rnr_element);
  void RenderElementPaste(RenderElement* rnr_element);
  void RenderElementChecked(TObject* obj, Bool_t state);

  void NotifyBrowser(TGListTreeItem* parent_lti=0);
  void NotifyBrowser(RenderElement* parent);

  // Hmmph ... geometry management?
  TGeoManager* GetGeometry(const TString& filename);

  void ThrowException(const char* text="foo");

  ClassDef(RGTopFrame, 0);
};

} // namespace Reve

extern Reve::RGTopFrame* gReve;

#endif
