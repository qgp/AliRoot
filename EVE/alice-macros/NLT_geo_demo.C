// $Id$

void NLT_geo_demo(Int_t type = Reve::NLTProjection::E_CFishEye, Float_t distortion = 0.02)
{  
   Reve::LoadMacro("region_marker.C");
   region_marker();
   
   Reve::NLTProjector* nlt = new Reve::NLTProjector();
   nlt->SetProjection(type, distortion);
   gReve->AddGlobalRenderElement(nlt);

   //  make_simple_geo(nlt);
   make_gentle_geo(nlt);
   
   TGLViewer* glv = dynamic_cast<TGLViewer*>(gReve->GetGLCanvas()->GetViewer3D());
   glv->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
}

/**************************************************************************/
void draw_node(const Text_t* path)
{ 
   // draws TGeoNode from given path, useful for debugging
   gGeoManager->cd(path);
   TGeoMatrix* mx = gGeoManager->GetCurrentMatrix();
   
   Reve::GeoTopNodeRnrEl* currn_re = new Reve::GeoTopNodeRnrEl
      (gGeoManager, gGeoManager->GetCurrentNode());
   currn_re->SetGlobalTrans(mx);
   gReve->AddGlobalRenderElement(currn_re);
}


/**************************************************************************/
void project_to_pointset(Reve::NLTProjector* nlt, const Text_t* path) 
{  
  // draws projected points from GeoNode, useful for debugging
  TBuffer3D* buff = make_buffer(path);
  if(buff)
  {
    PointSet* ps = new PointSet(buff->NbPnts());
    ps->SetMarkerColor(buff->fColor);
    for(Int_t i = 0; i<buff->NbPnts() ; i++)
      ps->SetPoint(i,  buff->fPnts[3*i], buff->fPnts[3*i+1], buff->fPnts[3*i+2]);
   
    PointSet* pps = nlt->ProjectPointSet(ps);
    gReve->AddGlobalRenderElement(pps, nlt);
  }
}
/**************************************************************************/
TBuffer3D* make_buffer(const Text_t* path)
{
  // creates TBuffer3D
  TBuffer3D* buff = 0;
  Bool_t valid = gGeoManager->cd(path);
  if(valid)  
  {
    TBuffer3D* buff = gGeoManager->GetCurrentVolume()->GetShape()->MakeBuffer3D();
    TGeoMatrix* mx = gGeoManager->GetCurrentMatrix();

    Int_t N = buff->NbPnts();   
    Double_t* pnts = buff->fPnts;   
    Double_t  master[4];
    for(Int_t i = 0; i<N; i++) 
    {
      mx->LocalToMaster(&pnts[3*i], master);
      pnts[3*i] =   master[0];
      pnts[3*i+1] = master[1];
      pnts[3*i+2] = master[2];
    }
    buff->fColor = gGeoManager->GetCurrentVolume()->GetLineColor();
  }
  return buff;
}

/**************************************************************************/
Reve::NLTPolygonSet* project_node(Reve::NLTProjector* nlt, const Text_t* path)
{
  // make a NLTPolygonSet from node loacted in given path
  Reve::NLTPolygonSet* ps = 0;
  TBuffer3D* buff = make_buffer(path); 
  if(buff) {
    ps = nlt->ProjectBuffer3D(buff);
    if(ps)
    {
      ps->SetName(Form("NLTPolygonSet %s",path));
      ps->SetFillColor(Color_t(buff->fColor));
      ps->SetLineColor((Color_t)TColor::GetColorBright(buff->fColor));
    }
  }
  return ps;
}

/**************************************************************************/
void project_nodes(Reve::NLTProjector* nlt, const Text_t* parent_path)
{
  // project child nodes from given path
  gGeoManager->cd(parent_path);
  TGeoNode* holder = gGeoManager->GetCurrentNode();
  holder->ls();
  TIter next_node(holder->GetNodes());
  TGeoNode* geon;

  Reve::RenderElementList* el = new Reve::RenderElementList(parent_path);
  gReve->AddGlobalRenderElement(el, nlt);
  while((geon = (TGeoNode*)next_node())) 
  {
    TGeoVolume* v = geon->GetVolume();
    if(v) {      
      TString path = Form("%s/%s",parent_path, geon->GetName());
      Reve::NLTPolygonSet* ps = project_node(nlt, path.Data());
      if(ps) {
	ps->SetName(geon->GetName());
	gReve->AddGlobalRenderElement(ps, el);
      }
    }
  }
}
/**************************************************************************/
void make_simple_geo(Reve::NLTProjector* nlt)
{
  // create a non-linear projection of simple_geo.root
  gGeoManager = gReve->GetGeometry("$REVESYS/alice-data/simple_geo.root");
  Reve::NLTPolygonSet* ps;

  project_nodes( nlt, "/ALIC_1/ITSV_holder_1/ITSV_1");
  ps = project_node( nlt, "/ALIC_1/TPC_holder_1/TPC_1/TDGN_1");
  if(ps) gReve->AddGlobalRenderElement(ps, nlt);
  ps = project_node(nlt, "/ALIC_1/TRD TOF_holder_1/B077_1");
  if(ps) gReve->AddGlobalRenderElement(ps, nlt);
  ps = project_node(nlt, "/ALIC_1/TRD TOF_holder_1/BRS4_1");
  if(ps) gReve->AddGlobalRenderElement(ps, nlt);
  ps = project_node(nlt, "/ALIC_1/TRD TOF_holder_1/BRS4_2");
  if(ps) gReve->AddGlobalRenderElement(ps, nlt);
  ps = project_node(nlt, "/ALIC_1/TRD TOF_holder_1/BFMO_1");
  if(ps) gReve->AddGlobalRenderElement(ps, nlt);
  ps = project_node(nlt, "/ALIC_1/TRD TOF_holder_1/BBMO_1");
  if( ps) gReve->AddGlobalRenderElement(ps, nlt);

  for(Int_t i = 1; i<6; i++) {
    ps = project_node(nlt, Form("/ALIC_1/PHOS_holder_1/PHOS_%d", i));
    ps->SetFillColor((Color_t)(kOrange-4));
    if(ps) gReve->AddGlobalRenderElement(ps, nlt);
  }
  project_nodes( nlt, "/ALIC_1/FMD_holder_1");  
  project_nodes( nlt, "/ALIC_1/RICH_holder_1");
}

/**************************************************************************/
void project_geo_shape(Reve::NLTProjector* nlt, Reve::GeoShapeRnrEl* pgsh, Reve::RenderElement* parent)
{
  // create a polygon set from GeoShapeRnrEl and repeat same calls on its children
  Reve::NLTPolygonSet* ps  = 0;
  if(pgsh->GetShape() &&  pgsh->GetRnrSelf())
  { 
    TBuffer3D* buff = pgsh->GetShape()->MakeBuffer3D();
    Reve::ZTrans& mx = pgsh->RefHMTrans();
    Int_t N = buff->NbPnts();
    Double_t* pnts = buff->fPnts;   
    for(Int_t k=0; k<N; k++) 
    {
      mx.MultiplyIP(&pnts[3*k]);
    }
    ps = nlt->ProjectBuffer3D(buff);
    ps->SetName(pgsh->GetName());
    ps->SetRnrSelf(pgsh->GetRnrSelf());
    ps->SetRnrChildren(pgsh->GetRnrChildren());
    gReve->AddGlobalRenderElement(ps, parent);
    //    ps->SetFillColor(pgsh->GetColor());
    ps->SetMainColor(pgsh->GetColor());
    ps->SetLineColor((Color_t)TColor::GetColorBright(pgsh->GetColor()));
    //    ps->SetMainTransparency(pgsh->GetMainTransparency());
    ps->SetMainTransparency(70);
  }

  Reve::RenderElement::List_i i = pgsh->BeginChildren();
  while (i != pgsh->EndChildren()) {
    Reve::GeoShapeRnrEl* gs = dynamic_cast<Reve::GeoShapeRnrEl*>(*i); 
    if(ps)
      project_geo_shape(nlt, gs, ps);
    else
      project_geo_shape(nlt, gs, parent);
    ++i;
  }
}

/**************************************************************************/
void make_gentle_geo(Reve::NLTProjector* nlt)
{ 
  // make non-linear projection of gentle_geo.root
  TFile f("$REVESYS/alice-data/gentle_geo.root");
  TGeoShapeExtract* gse = (TGeoShapeExtract*) f.Get("Gentle");
  Reve::GeoShapeRnrEl::ImportShapeExtract(gse, nlt);
  f.Close();

  Reve::RenderElement::List_i a = nlt->BeginChildren();
  Reve::GeoShapeRnrEl* top_node = dynamic_cast<Reve::GeoShapeRnrEl*>(*a);
  Reve::RenderElement::List_i i = top_node->BeginChildren();
  while (i != top_node->EndChildren()) 
  {
    Reve::GeoShapeRnrEl* gs = dynamic_cast<Reve::GeoShapeRnrEl*>(*i);
    if(gs)
    {
      if(strcmp(gs->GetName(),"TRD+TOF") == 0)
      {
	Reve::RenderElement::List_i a = gs->BeginChildren();
	(*a)->SetRnrSelf(kFALSE);
      }
      project_geo_shape(nlt, gs, nlt);
    }
    ++i;
  }
  // don't need original geomtery any more
  top_node->Destroy();
}
