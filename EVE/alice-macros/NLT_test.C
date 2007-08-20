
Reve::NLTProjector* NLT_test(RenderElement* top=0)
{
  using namespace Reve;

  Scene* s = gReve->SpawnNewScene("Projected Event");
  gReve->GetDefViewer()->AddScene(s);

  NLTProjector* p = new NLTProjector;

  gReve->AddToListTree(p, kTRUE);
  gReve->AddRenderElement(p, s);

  if (top == 0)
    top = gReve->GetCurrentEvent();

  p->ImportElements(top);

  return p;
}
