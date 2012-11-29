#ifndef ALIITSURECODET
#define ALIITSURECODET

#include <TNamed.h>
#include <TObjArray.h>
#include "AliITSURecoLayer.h"
class AliITSUGeomTGeo;

///////////////////////////////////////////////////////////////////////
//                                                                   //
//  Class AliITSURecoDet                                             //
//  Interface between the framework and reconstruction for ITS       //
//                                                                   //
///////////////////////////////////////////////////////////////////////


class AliITSURecoDet : public TNamed
{
 public:
  //
  AliITSURecoDet(AliITSUGeomTGeo* geom, const char* name="");
  virtual ~AliITSURecoDet();
  //
  Double_t           GetRMin()                     const {return fRMin;}
  Double_t           GetRMax()                     const {return fRMax;}
  Int_t              GetNLayers()                  const {return fNLayers;}
  Int_t              GetNLayersActive()            const {return fNLayersActive;}
  Int_t              GetLrIDActive(Int_t lrActID)  const;
  AliITSURecoLayer*  GetLayer(Int_t i)             const;
  AliITSURecoLayer*  GetLayerActive(Int_t i)       const;
  AliITSUGeomTGeo*   GetGeom()                     const {return fGeom;}
  //
  void               SetRMin(Double_t r)                 {fRMin = r;}
  void               SetRMax(Double_t r)                 {fRMax = r;}
  //
  void               AddLayer(const AliITSURecoLayer* lr);
  //
  void               ProcessClusters(Int_t mode=0);
  //
  virtual void       Print(Option_t* option = "")  const;
  //
 protected:
  Bool_t             Build();
  void               IndexLayers();
  //
 protected:
  Int_t              fNLayers;        // total number of layers
  Int_t              fNLayersActive;  // N of active layers
  Double_t           fRMax;           // max  R
  Double_t           fRMin;           // min  R
  TObjArray          fLayers;         // layers
  TObjArray          fLayersActive;   // active layers
  AliITSUGeomTGeo*   fGeom;           // ITS geometry
  //
 protected:
  static const Char_t*     fgkBeamPipeVolName;    // name of the beam pipe volume

 private:
  AliITSURecoDet(const AliITSURecoDet &source); 
  AliITSURecoDet& operator=(const AliITSURecoDet &source); 
  //
  ClassDef(AliITSURecoDet,1); // helper for ITS data in reco
};



//_____________________________________________________________
inline Int_t AliITSURecoDet::GetLrIDActive(Int_t lrActID) const 
{
  // get global layer id from active id
  return (lrActID<fNLayersActive) ? ((AliITSURecoLayer*)fLayersActive.UncheckedAt(lrActID))->GetID() : fNLayers;
}

//_____________________________________________________________
inline AliITSURecoLayer* AliITSURecoDet::GetLayer(Int_t i) const 
{
  // get layer with global id=i
  return i<fNLayers ? (AliITSURecoLayer*)fLayers.UncheckedAt(i):0;
}

//_____________________________________________________________
inline AliITSURecoLayer* AliITSURecoDet::GetLayerActive(Int_t i) const
{
  // get layer with activeID=i
  return i<fNLayersActive ? (AliITSURecoLayer*)fLayersActive.UncheckedAt(i):0;
}

//______________________________________________________
inline void AliITSURecoDet::ProcessClusters(Int_t mode)
{
  // prepare clsuters for reconstrunction
  for (int ilr=fNLayersActive;ilr--;) GetLayerActive(ilr)->ProcessClusters(mode);
}

#endif
