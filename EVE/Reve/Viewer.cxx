// $Header$

#include "Viewer.h"
#include "Scene.h"
#include "SceneInfo.h"

#include <Reve/RGTopFrame.h>

#include <TGLSAViewer.h>
#include <TGLScenePad.h>

using namespace Reve;

//______________________________________________________________________
// Reve::Viewer
//

ClassImp(Viewer)

Viewer::Viewer(const Text_t* n, const Text_t* t) :
  RenderElementList(n, t),
  fGLViewer (0)
{
  SetChildClass(SceneInfo::Class());
}

Viewer::~Viewer()
{}

/**************************************************************************/

void Viewer::SetGLViewer(TGLViewer* s)
{
  delete fGLViewer;
  fGLViewer = s;

  fGLViewer->SetSmartRefresh(kTRUE);
  fGLViewer->SetResetCamerasOnUpdate(kFALSE);
  fGLViewer->SetResetCameraOnDoubleClick(kFALSE);
}

void Viewer::SpawnGLViewer(TGFrame* parent, TGedEditor* ged)
{
  TGLSAViewer* v = new TGLSAViewer(parent, 0, ged);
  SetGLViewer(v);
}

/**************************************************************************/

void Viewer::AddScene(Scene* scene)
{
  static const Exc_t eH("Viewer::AddScene ");

  TGLSceneInfo* glsi = fGLViewer->AddScene(scene->GetGLScene());
  if (glsi != 0) {
    SceneInfo* si = new SceneInfo(this, scene, glsi);
    gReve->AddRenderElement(si, this);
  } else {
    throw(eH + "scene already in the viewer.");
  }
}

void Viewer::RemoveElementLocal(RenderElement* el)
{
  fGLViewer->RemoveScene(((SceneInfo*)el)->GetGLScene());
  RenderElement::RemoveElementLocal(el);
}

void Viewer::RemoveElements()
{
  fGLViewer->RemoveAllScenes();
  RenderElement::RemoveElements();
}

void Viewer::HandleElementPaste(RenderElement* el)
{
  static const Exc_t eH("Viewer::HandleElementPaste ");

  Scene* scene = dynamic_cast<Scene*>(el);
  if (scene != 0) {
    AddScene(scene);
    gReve->Redraw3D();
  } else {
    throw(eH + "class Viewer only accepts Scene paste argument.");
  }
}

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

//______________________________________________________________________
// Reve::ViewerList
//

ClassImp(ViewerList)

ViewerList::ViewerList(const Text_t* n, const Text_t* t) :
  RenderElementList(n, t)
{
  SetChildClass(Viewer::Class());
}

ViewerList::~ViewerList()
{}

/**************************************************************************/

void ViewerList::RepaintChangedViewers(Bool_t resetCameras, Bool_t dropLogicals)
{
  for(List_i i=fChildren.begin(); i!=fChildren.end(); ++i)
  {
    TGLViewer* glv = ((Viewer*)*i)->GetGLViewer();
    if (glv->IsChanged())
    {
      // printf(" Viewer '%s' changed ... reqesting draw.\n", (*i)->GetObject()->GetName());

      if (resetCameras)	glv->ResetCamerasAfterNextUpdate();
      if (dropLogicals) glv->SetSmartRefresh(kFALSE);

      glv->RequestDraw();

      if (dropLogicals) glv->SetSmartRefresh(kTRUE);
    }
  }
}
