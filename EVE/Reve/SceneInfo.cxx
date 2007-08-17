// $Header$

#include "SceneInfo.h"
#include "Scene.h"

#include <TGLSceneInfo.h>

using namespace Reve;

//______________________________________________________________________
// SceneInfo
//

ClassImp(SceneInfo)

SceneInfo::SceneInfo(Viewer* viewer, Scene* scene, TGLSceneInfo* sinfo) :
  RenderElement (),
  TNamed        (Form("SI - %s", scene->GetName()),
		 Form("SceneInfo of scene '%s'", scene->GetName())),
  fViewer       (viewer),
  fScene        (scene),
  fGLSceneInfo  (sinfo)
{}

SceneInfo::~SceneInfo()
{}

/**************************************************************************/

TGLSceneBase* SceneInfo::GetGLScene() const
{
  return fGLSceneInfo->GetScene();
}

/**************************************************************************/

void SceneInfo::SetRnrSelf(Bool_t rnr)
{
  RenderElement::SetRnrSelf(rnr);
  fGLSceneInfo->SetActive(fRnrSelf);
}

void SceneInfo::SetRnrState(Bool_t rnr)
{
  RenderElement::SetRnrState(rnr);
  fGLSceneInfo->SetActive(fRnrSelf);
}
