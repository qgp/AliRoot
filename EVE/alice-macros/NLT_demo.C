// $Id$

void NLT_demo(Int_t type = Reve::NLTProjection::PT_RhoZ, Float_t distortion = 0.02)
{
  // simple example of non linear projection
  Reve::LoadMacro("alieve_init.C");
  Reve::LoadMacro("esd_tracks.C");
  Reve::LoadMacro("kine_tracks.C");
  Reve::LoadMacro("its_hits.C");
  Reve::LoadMacro("NLT_geo_demo.C");

  Reve::LoadMacro("region_marker.C");
  region_marker();

  alieve_init();

  Reve::NLTProjector* nlt = new  Reve::NLTProjector();
  nlt->SetProjection(type, distortion);
  gReve->AddGlobalRenderElement(nlt);

  // kine_tracks();
  make_gentle_geo(nlt);
  {
    // tacks
    Reve::TrackList* tl = kine_tracks(0.23, 0.24);
    nlt->RegisterTrackList(tl, kFALSE);

    // hits
    Reve::PointSet* ih = its_hits();
    nlt->ProjectPointSet(ih);
  }

  TGLViewer* v = (TGLViewer *)gReve->GetGLViewer();
  v->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  TGLCameraMarkupStyle* mup = v->GetCameraMarkup();
  if(mup) mup->SetShow(kFALSE);
  gReve->Redraw3D();
}
