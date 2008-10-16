#ifndef ALITRDGTUPARAM_H
#define ALITRDGTUPARAM_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id: AliTRDgtuParam.h 27496 2008-07-22 08:35:45Z cblume $ */

// --------------------------------------------------------
// 
// Singleton class to hold the parameters steering the GTU 
// tracking
//
// --------------------------------------------------------

#include "TObject.h"
#include "TVectorD.h"

class AliTRDgeometry;

class AliTRDgtuParam : public TObject {
 public:
  virtual ~AliTRDgtuParam();

  static AliTRDgtuParam *Instance(); // Singleton
  static void Terminate(); 

  static inline Int_t GetNLinks() { return fgkNLinks; }
  static inline Int_t GetNLayers() { return fgkNLinks/2; }
  static inline Int_t GetNZChannels() { return fgkNZChannels; }
  static inline Int_t GetNRefLayers() { return fgkNRefLayers; }

  static inline Float_t GetChamberThickness() { return 3.0; }

  // ----- Bin widths (granularity) -----
  static inline Float_t GetBinWidthY() { return fgkBinWidthY; }
  static inline Float_t GetBinWidthdY() { return fgkBinWidthdY; }

  // ----- Bit Widths (used for internal representation) -----
  static inline Int_t GetBitWidthY() { return fgkBitWidthY; }
  static inline Int_t GetBitWidthdY() { return fgkBitWidthdY; }
  static inline Int_t GetBitWidthYProj() { return fgkBitWidthYProj; }
  static inline Int_t GetBitExcessY() { return fgkBitExcessY; }
  static inline Int_t GetBitExcessAlpha() { return fgkBitExcessAlpha; }
  static inline Int_t GetBitExcessYProj() { return fgkBitExcessYProj; }

  AliTRDgeometry* GetGeo() const { return fGeo; }
  Float_t GetVertexSize() const { return fVertexSize; }
  Int_t GetCiAlpha(Int_t layer) const;
  Int_t GetCiYProj(Int_t layer) const;
  Int_t GetYt(Int_t stack, Int_t layer, Int_t zrow) const;
  Int_t GetDeltaY() const { return fgkDeltaY; }
  Int_t GetDeltaAlpha() const { return fgkDeltaAlpha; }
  Int_t GetZSubchannel(Int_t stack, Int_t layer, Int_t zchannel, Int_t zpos) const;
  Int_t GetRefLayer(Int_t refLayerIdx) const;
//  Bool_t GetFitParams(TVectorD &rhs, Int_t k); // const
  Bool_t GetIntersectionPoints(Int_t k, Float_t &x1, Float_t &x2); // const
  Float_t GetRadius(Int_t a, Float_t b, Float_t x1, Float_t x2); // const

  Bool_t IsInZChannel(Int_t stack, Int_t layer, Int_t zchannel, Int_t zpos) const;

  void SetVertexSize(Float_t vertexsize) { fVertexSize = vertexsize; }

  // z-channel map
  Int_t zChannelGen(); // could have different modes (for beam-beam, cosmics, ...)
  Bool_t DisplayZChannelMap(Int_t zchannel = -1, Int_t subch = 0) const;

  // variables for pt-reconstruction (not used at the moment)
  Bool_t GenerateRecoCoefficients(Int_t trackletMask);
  Float_t GetAki(Int_t k, Int_t i);
  Float_t GetBki(Int_t k, Int_t i);
  Float_t GetCki(Int_t k, Int_t i);
//  Float_t GetD(Int_t k) const;

 protected:
  static const Int_t fgkNZChannels = 3; // No. of z-channels
  static const Int_t fgkNLinks = 12;	// No. of links
  static const Int_t fgkFixLayer = 2;	// which layer is fixed for the generation of the z-channel map
  static const Int_t fgkDeltaY = 39;	// accepted deviation in y_proj, default: 9
  static const Int_t fgkDeltaAlpha = 31; // accepted deviation in alpha, default: 11
  static const Int_t fgkNRefLayers = 3;	 // no. of reference layers

  static const Float_t fgkBinWidthY;
  static const Float_t fgkBinWidthdY;

  static const Int_t fgkBitWidthY;
  static const Int_t fgkBitWidthdY;
  static const Int_t fgkBitWidthYProj;
  static const Int_t fgkBitExcessY;
  static const Int_t fgkBitExcessAlpha;
  static const Int_t fgkBitExcessYProj;
 
  Float_t fVertexSize;		// assumed vertex size (z-dir.) for the z-channel map

  Int_t fZChannelMap[5][16][6][16];		  // must be changed
  Int_t fZSubChannel[5][fgkNZChannels][6][16];    // must be changed

  Int_t fCurrTrackletMask;
  Float_t fAki[6];
  Float_t fBki[6];
  Float_t fCki[6];

  Int_t *fRefLayers;		//[fgkNRefLayers] reference layers for track finding

  AliTRDgeometry *fGeo;		//! pointer to the TRD geometry

  static AliTRDgtuParam *fgInstance; // instance pointer

 private:
  AliTRDgtuParam();			     // instance only via Instance()
  AliTRDgtuParam(const AliTRDgtuParam &rhs); // not implemented
  AliTRDgtuParam& operator=(const AliTRDgtuParam &rhs); // not implemented

  ClassDef(AliTRDgtuParam, 1);
};

#endif
