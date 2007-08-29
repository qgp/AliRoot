// $Id$

Reve::GeoShapeRnrEl* geom_gentle()
{
  TFile f("$REVESYS/alice-data/gentle_geo.root");
  TGeoShapeExtract* gse = (TGeoShapeExtract*) f.Get("Gentle");
  GeoShapeRnrEl* gsre = Reve::GeoShapeRnrEl::ImportShapeExtract(gse, 0);
  f.Close();

  return gsre;
}
