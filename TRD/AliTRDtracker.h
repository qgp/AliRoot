#ifndef ALITRDTRACKER_H
#define ALITRDTRACKER_H   

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */ 

#include "AliTracker.h" 
#include "TObjArray.h" 

class TFile;
class TParticle;
class TParticlePDG;

class AliTRDgeometry;
class AliTRDparameter;
class AliTRDtrack;
class AliTRDcluster;
class AliTRDmcTrack;

const unsigned kMAX_LAYERS_PER_SECTOR = 1000;  
const unsigned kMAX_TIME_BIN_INDEX = 216;  // (30 drift + 6 ampl) * 6 planes  
const unsigned kMAX_CLUSTER_PER_TIME_BIN = 7000; 
const unsigned kZONES = 5; 
const Int_t kTRACKING_SECTORS = 18; 

class AliTRDtracker : public AliTracker { 

 public:

  AliTRDtracker():AliTracker() {} 
  AliTRDtracker(const TFile *in);
  ~AliTRDtracker(); 

  Int_t         Clusters2Tracks(const TFile *in, TFile *out);
  Int_t         PropagateBack(const TFile *in, TFile *out);
  Int_t         LoadClusters() {LoadEvent(); return 0;};
  void          UnloadClusters() {UnloadEvent();};
  AliCluster   *GetCluster(Int_t index) const { return (AliCluster*) fClusters->UncheckedAt(index); };
  virtual void  CookLabel(AliKalmanTrack *t,Float_t wrong) const;
  virtual void  UseClusters(const AliKalmanTrack *t, Int_t from=0) const;  
  
  void          SetAddTRDseeds() { fAddTRDseeds = kTRUE; }
  void          SetNoTilt() { fNoTilt = kTRUE; }

  Double_t      GetTiltFactor(const AliTRDcluster* c);

  void          ReadClusters(TObjArray *array, const Char_t *filename); 
  Int_t         CookSectorIndex(Int_t gs) { return kTRACKING_SECTORS - 1 - gs; }


  Float_t  GetSeedGap()       const {return fSeedGap;}   
  Int_t    GetMaxGap()        const {return fMaxGap;}   
  Int_t    GetTimeBinsPerPlane()   const {return fTimeBinsPerPlane;}   
  Float_t  GetSeedStep()      const {return fSeedStep;}
  Float_t  GetSeedDepth()     const {return fSeedDepth;}
  Float_t  GetSkipDepth()     const {return fSkipDepth;}
  Double_t GetMaxChi2()       const {return fMaxChi2;}
  Float_t  GetMaxSeedC()      const {return fMaxSeedC;}
  Float_t  GetMaxSeedTan()    const {return fMaxSeedTan;}
  Double_t GetSeedErrorSY()   const {return fSeedErrorSY;}
  Double_t GetSeedErrorSY3()  const {return fSeedErrorSY3;}
  Double_t GetSeedErrorSZ()   const {return fSeedErrorSZ;}
  Float_t  GetLabelFraction() const {return fLabelFraction;}
  Float_t  GetWideRoad()      const {return fWideRoad;}

  Float_t  GetMinClustersInTrack() const {return fMinClustersInTrack;}
  Float_t  GetMinClustersInSeed()  const {return fMinClustersInSeed;} 
  Float_t  GetMaxSeedDeltaZ()      const {return fMaxSeedDeltaZ;}
  Float_t  GetMaxSeedVertexZ()     const {return fMaxSeedVertexZ;}

  // x <-> timebin conversions useful in analysis macros
  Double_t GetX(Int_t sec, Int_t plane, Int_t local_tb) const;
  Double_t GetX(Int_t sec, Int_t pl) const { 
    return fTrSec[sec]->GetLayer(pl)->GetX(); }
  Int_t GetGlobalTimeBin(Int_t sec, Int_t plane, Int_t local_tb) const {
    return fTrSec[sec]->CookTimeBinIndex(plane,local_tb); }
  Double_t GetLayerNumber(Int_t sec, Double_t x) const {
    return fTrSec[sec]->GetLayerNumber(x); }

 public:
   class AliTRDpropagationLayer {
   // *****************  internal class *******************
   public: 
     AliTRDpropagationLayer(Double_t x, Double_t dx, Double_t rho, 
                            Double_t x0, Int_t tb_index); 

     ~AliTRDpropagationLayer() { 
       if(fTimeBinIndex >= 0) { delete[] fClusters; delete[] fIndex; }
     }
     void InsertCluster(AliTRDcluster*, UInt_t);
     operator       Int_t() const {return fN;}
     AliTRDcluster* operator[](Int_t i) {return fClusters[i];}
     UInt_t         GetIndex(Int_t i) const {return fIndex[i];} 
     Double_t       GetX() const { return fX; }
     Double_t       GetdX() const { return fdX; }
     Double_t       GetRho() const { return fRho; }
     Double_t       GetX0() const { return fX0; }
     Int_t          GetTimeBinIndex() const { return fTimeBinIndex; }     
     void           GetPropagationParameters(Double_t y, Double_t z,
                                Double_t &dx, Double_t &rho, Double_t &x0, 
                                Bool_t &lookForCluster) const;
     Int_t          Find(Double_t y) const; 
     void           SetZmax(Int_t cham, Double_t center, Double_t w)
                      { fZc[cham] = center;  fZmax[cham] = w; }
     void           SetYmax(Double_t w) { fYmax = w; }
     Double_t       GetYmax() const { return fYmax; }
     Double_t       GetZmax(Int_t c) const { return fZmax[c]; }
     Double_t       GetZc(Int_t c) const { return fZc[c]; }
     
     void           SetHole(Double_t Zmax, Double_t Ymax,
                            Double_t rho = 1.29e-3, Double_t x0 = 36.66,
                            Double_t Yc = 0, Double_t Zc = 0);
                            
     void    Clear() {for(Int_t i=0; i<fN; i++) fClusters[i] = NULL; fN = 0;}
                   
   private:     

     Int_t         fN;
     AliTRDcluster **fClusters; // array of pointers to clusters
     UInt_t        *fIndex;     // array of cluster indexes
     Double_t       fX;         // x coordinate of the middle plane
     Double_t       fdX;        // radial thickness of the time bin
     Double_t       fRho;       // default density of the material 
     Double_t       fX0;        // default radiation length 
     Int_t          fTimeBinIndex;  // plane * F(local_tb)  
     Double_t       fZc[kZONES];  // Z position of the center for 5 active areas
     Double_t       fZmax[kZONES]; // half of active area length in Z
     Double_t       fYmax;        // half of active area length in Y

     Bool_t         fHole;        // kTRUE if there is a hole in the layer
     Double_t       fHoleZc;      // Z of the center of the hole 
     Double_t       fHoleZmax;    // half of the hole length in Z
     Double_t       fHoleYc;      // Y of the center of the hole 
     Double_t       fHoleYmax;    // half of the hole length in Y 
     Double_t       fHoleRho;     // density of the gas in the hole 
     Double_t       fHoleX0;      // radiation length of the gas in the hole 
   };

   class AliTRDtrackingSector {
   public:
     AliTRDtrackingSector(AliTRDgeometry* geo, Int_t gs, AliTRDparameter* par);
     ~AliTRDtrackingSector() { for(Int_t i=0; i<fN; i++) delete fLayers[i]; }
     Int_t    GetNumberOfLayers() const { return fN; }
     Int_t    GetNumberOfTimeBins() const;
     Double_t GetX(Int_t pl) const { return fLayers[pl]->GetX(); }
     void     MapTimeBinLayers();
     Int_t    GetLayerNumber(Double_t x) const;
     Int_t    GetInnerTimeBin() const;
     Int_t    GetOuterTimeBin() const;
     Int_t    GetLayerNumber(Int_t tb) const {return fTimeBinIndex[tb];}
     Float_t  GetTzeroShift() const { return fTzeroShift; }   
     Int_t    Find(Double_t x) const; 
     void     InsertLayer(AliTRDpropagationLayer* pl);
     //     AliTRDpropagationLayer* operator[](Int_t i) { return fLayers[i]; }
     AliTRDpropagationLayer* GetLayer(Int_t i) { return fLayers[i]; }
     Int_t    CookTimeBinIndex(Int_t plane, Int_t local_tb) const;     

   private:
     Int_t                     fN;      // total number of layers
     AliTRDgeometry            *fGeom;     
     AliTRDparameter           *fPar;     
     AliTRDpropagationLayer    *fLayers[kMAX_LAYERS_PER_SECTOR];     
     Int_t                     fTimeBinIndex[kMAX_TIME_BIN_INDEX];     
     Float_t                   fTzeroShift;   // T0 shift in cm
     Int_t                     fGeomSector;   // sector # in AliTRDgeometry
   };

 private:

  void          LoadEvent();
  void          UnloadEvent();

  virtual void  MakeSeeds(Int_t inner, Int_t outer, Int_t turn);

  Int_t         FollowProlongation(AliTRDtrack& t, Int_t rf);
  Int_t         FollowBackProlongation(AliTRDtrack& t);

  Int_t         PropagateToTPC(AliTRDtrack& t);
  Int_t         PropagateToOuterPlane(AliTRDtrack& t, Double_t x);

  Int_t         WriteTracks(); 
  void          ReadClusters(TObjArray *array, const TFile *in = 0);

  void          SetSY2corr(Float_t w)    {fSY2corr = w;}
  void          SetSZ2corr(Float_t w)    {fSZ2corr = w;}
  Double_t      ExpectedSigmaY2(Double_t r, Double_t tgl, Double_t pt);
  Double_t      ExpectedSigmaZ2(Double_t r, Double_t tgl);


 protected:

  AliTRDgeometry     *fGeom;            // Pointer to TRD geometry
  AliTRDparameter    *fPar;     

  AliTRDtrackingSector *fTrSec[kTRACKING_SECTORS];       
                                       // array of tracking sectors;    
  
  Int_t            fNclusters;        // Number of clusters in TRD 
  TObjArray        *fClusters;        // List of clusters for all sectors

  Int_t            fNseeds;           // Number of track seeds  
  TObjArray        *fSeeds;           // List of track seeds
   
  Int_t            fNtracks;          // Number of reconstructed tracks 
  TObjArray        *fTracks;          // List of reconstructed tracks   

  Float_t          fSY2corr;          // Correction coefficient for
                                      // cluster SigmaY2 

  Float_t          fSZ2corr;          // Correction coefficient for
                                      // cluster SigmaZ2 

  static const Float_t  fSeedGap;     // Distance between inner and outer
                                      // time bin in seeding 
                                      // (fraction of all time bins) 
  
  static const Float_t  fSeedStep;    // Step in iterations
  static const Float_t  fSeedDepth;   // Fraction of TRD allocated for seeding
  static const Float_t  fSkipDepth;   // Fraction of TRD which can be skipped
                                      // in track prolongation             
  Int_t       fTimeBinsPerPlane;      // number of sensitive timebins per plane
  Int_t       fMaxGap;                // max gap (in time bins) in the track  
                                      // in track prolongation             

  static const Double_t fMaxChi2;     // max increment in track chi2 
        
  static const Float_t  fMinClustersInTrack; // min number of clusters in track
                                             // out of total timebins

  static const Float_t  fMinFractionOfFoundClusters; // min found clusters 
                                                     // out of expected  

  static const Float_t  fMinClustersInSeed;  // min fraction of clusters in seed
  static const Float_t  fMaxSeedDeltaZ;   // max dZ in MakeSeeds
  static const Float_t  fMaxSeedDeltaZ12; // max abs(z1-z2) in MakeSeeds
  static const Float_t  fMaxSeedC;       // max initial curvature in MakeSeeds
  static const Float_t  fMaxSeedTan;     // max initial Tangens(lambda) in MakeSeeds
  static const Float_t  fMaxSeedVertexZ; // max vertex Z in MakeSeeds
  static const Double_t fSeedErrorSY;    // sy parameter in MakeSeeds
  static const Double_t fSeedErrorSY3;   // sy3 parameter in MakeSeeds
  static const Double_t fSeedErrorSZ;    // sz parameter in MakeSeeds
  static const Float_t  fLabelFraction;  // min fraction of same label
  static const Float_t  fWideRoad;       // max road width in FindProlongation

  Bool_t                fVocal;   
  Bool_t                fAddTRDseeds;

  Bool_t                fNoTilt;
 
  ClassDef(AliTRDtracker,1)           // manager base class  

};

#endif 
