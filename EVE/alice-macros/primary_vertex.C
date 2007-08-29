// $Header$

TPolyMarker3D* make_vertex_marker(AliESDVertex* v, const Text_t* name)
{
  Double_t x[3], e[3];
  v->GetXYZ(x);
  v->GetSigmaXYZ(e);

  printf("%16s: %f %f %f   -   %f %f %f\n", name,
	 x[0], x[1], x[2], e[0], e[1], e[2]);

  TPolyMarker3D* m = new TPolyMarker3D(1);
  m->SetName(name);
  m->SetPoint(0, x[0], x[1], x[2]);

  return m;
}

Reve::StraightLineSet* make_vertex_lines(AliESDVertex* v, const Text_t* name)
{ 
  Double_t x[3], e[3];
  v->GetXYZ(x);
  v->GetSigmaXYZ(e);
  printf("%16s: %f %f %f   -   %f %f %f\n", name,
	 x[0], x[1], x[2], e[0], e[1], e[2]);

  Reve::StraightLineSet* ls = new Reve::StraightLineSet();
  ls->AddLine(x[0] + e[0], x[1], x[2], 
	      x[0] - e[0], x[1], x[2]);

  ls->AddLine(x[0], x[1] + e[1], x[2], 
	      x[0], x[1] - e[1], x[2]);

  ls->AddLine(x[0], x[1], x[2] + e[2], 
	      x[0], x[1], x[2] - e[2]);
  // corners of the three lines 
  for(Int_t i =0; i < 3; i++)
  {
    ls->AddMarker(i, 0);
    ls->AddMarker(i, 1);
  }

  // centre 
  ls->AddMarker(0, 0.5);

  e[0] *= 30;
  e[1] *= 30;
  e[2] *= 10;

  Int_t N = 32;
  Float_t S = 2*TMath::Pi()/N;
  Float_t b, a, phi;
  // elipse XY
  a = e[0];
  b = e[1];
  for(Int_t i = 0; i<N; i++)
    ls->AddLine(x[0] + a*TMath::Cos(i*S),   x[1] + b*TMath::Sin(i*S),   x[2], 
		x[0] + a*TMath::Cos(i*S+S), x[1] + b*TMath::Sin(i*S+S), x[2]);
  // elipse YZ
  a = e[1];
  b = e[2];
  for(Int_t i = 0; i<N; i++)
    ls->AddLine(x[0], x[1] + a*TMath::Cos(i*S)  , x[2] + b*TMath::Sin(i*S), 
		x[0], x[1] + a*TMath::Cos(i*S+S), x[2] + b*TMath::Sin(i*S+S));
  //elipse YZ
  a = e[0];
  b = e[2];
  for(Int_t i = 0; i<N; i++)
    ls->AddLine(x[0] + a*TMath::Cos(i*S),   x[1], x[2] + b*TMath::Sin(i*S), 
		x[0] + a*TMath::Cos(i*S+S), x[1], x[2] + b*TMath::Sin(i*S+S));
  
  ls->SetRnrMarkers(kFALSE);
  ls->SetName("Primary Vertex");
  return ls;  
}

Reve::BoxSet* make_vertex_boxes(AliESDVertex* v)
{
  Double_t x[3], e[3];
  v->GetXYZ(x);
  v->GetSigmaXYZ(e);

  Reve::BoxSet* bs;

  bs = new BoxSet("+- 10 x 10 x 20mm");
  bs->SetRenderMode(Reve::BoxSet::RM_Line);
  bs->AddBox(Reve::Box(-1, x[0], x[1], x[2], 1, 1, 2));
  bs->SetMainColor((Color_t) 2);
  gReve->AddRenderElement(bs);

  bs = new BoxSet("+- 30 sigma_r x 10 sigma_z");
  bs->SetRenderMode(Reve::BoxSet::RM_Line);
  bs->AddBox(Reve::Box(-1, x[0], x[1], x[2], 30*e[0], 30*e[1], 10*e[2]));
  bs->SetMainColor((Color_t) 3);
  gReve->AddRenderElement(bs);

  gReve->Redraw3D();
}

void register_vertex_marker(TPolyMarker3D* m)
{
  using namespace Reve;
  Color_t* colp = FindColorVar(m, "fMarkerColor");
  RenderElementObjPtr* rnrEl = new RenderElementObjPtr(m, *colp);
  gReve->AddRenderElement(rnrEl);
  gReve->Redraw3D();
}

void primary_vertex_primitive(Bool_t showSPD=kTRUE, Bool_t showBoxes=kFALSE)
{
  AliESDEvent* esd = Alieve::Event::AssertESD();

  AliESDVertex*  pv  = esd->GetPrimaryVertex();
  TPolyMarker3D* pvm = make_vertex_marker(pv, "Primary Vertex");
  pvm->SetMarkerStyle(5);
  pvm->SetMarkerColor(5);
  pvm->SetMarkerSize(1.4);
  register_vertex_marker(pvm);

  if(showSPD) {
    AliESDVertex*  spdv  = esd->GetVertex();
    TPolyMarker3D* spdvm = make_vertex_marker(spdv, "SPD Vertex");
    spdvm->SetMarkerStyle(2);
    spdvm->SetMarkerColor(6);
    register_vertex_marker(spdvm);
  }

  if(showBoxes)
    make_vertex_boxes(pv);
}


void primary_vertex(Bool_t showSPD=kTRUE, Bool_t showElispse=kFALSE)
{
  using namespace Reve;
  AliESDEvent* esd = Alieve::Event::AssertESD();

  AliESDVertex*  pv  =  esd->GetPrimaryVertex();
  StraightLineSet* ls = make_vertex_lines(pv, "Primary Vertex");
  ls->SetMarkerStyle(5);
  ls->SetMarkerColor(5);
  ls->SetMarkerSize(1.4);
  ls->SetLineColor(7);
  gReve->AddRenderElement(ls);

}
