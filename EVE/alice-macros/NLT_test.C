
Reve::NLTProjector* NLT_test(RenderElement* top=0)
{
  Reve::NLTProjector* p = new Reve::NLTProjector;

  gReve->AddToListTree(p, kTRUE);
  // gReve->AddRenderElement(p, gReve->GetCurrentEvent());

  if (top == 0)
    top = gReve->GetCurrentEvent();

  p->ImportElements(top);

  return p;
}
