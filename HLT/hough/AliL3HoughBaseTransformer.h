// @(#) $Id$

#ifndef ALIL3HOUGHBASETRANSFORMER_H
#define ALIL3HOUGHBASETRANSFORMER_H

#include "AliL3RootTypes.h"

#ifdef do_mc
const UInt_t MaxTrack=120;
struct AliL3TrackIndex {
  Int_t fLabel[MaxTrack];//MC label
  UChar_t fNHits[MaxTrack];//Number of different mc labels
  UChar_t fCurrentRow[MaxTrack];//Index of the current row while filling Hough space
};
typedef struct AliL3TrackIndex AliL3TrackIndex;
#endif

class AliL3DigitRowData;
class AliL3Histogram;

#include "../RAW/AliTPCRawStream.h"

class AliL3HoughBaseTransformer {

 public:

  AliL3HoughBaseTransformer(); 
  AliL3HoughBaseTransformer(Int_t slice,Int_t patch,Int_t netasegments,Float_t zvertex=0.0);
  virtual ~AliL3HoughBaseTransformer();
  
  void SetInputData(UInt_t /*ndigits*/,AliL3DigitRowData *ptr) {fDigitRowData = ptr;}
  
  //this is for adaptave histograms
  virtual void CreateHistograms(Float_t /*ptmin*/,Float_t /*ptmax*/,Float_t /*pres*/,Int_t /*nybin*/,Float_t /*psi*/)
    {STDCERR<<"Adaptive histograms are not supported  for this Transformer!"<<STDENDL;}

  virtual void CreateHistograms(Int_t nxbin,Float_t ptmin,Int_t nybin,Float_t phimin,Float_t phimax) = 0;
  virtual void CreateHistograms(Int_t nxbin,Float_t xmin,Float_t xmax,Int_t nybin,Float_t ymin,Float_t ymax) = 0;

  virtual void Reset() = 0;
  virtual void TransformCircle()
    {STDCERR<<"TransformCircle is not defined for this transformer!"<<STDENDL;}
  virtual void TransformCircle(Int_t */*row_range*/,Int_t /*every*/)
    {STDCERR<<"TransformCircle is not defined for this transformer!"<<STDENDL;}
  virtual void TransformCircleC(Int_t */*row_range*/,Int_t /*every*/)
    {STDCERR<<"TransformCircleC is not defined for this transformer!"<<STDENDL;}
  virtual void TransformLine(Int_t */*rowrange*/=0,Float_t */*phirange*/=0)
    {STDCERR<<"TransformLine is not defined for this Transformer!"<<STDENDL;}
  virtual void TransformLineC(Int_t */*rowrange*/,Float_t */*phirange*/)
      {STDCERR<<"TransformLineC is not defined for this Transformer!"<<STDENDL;}

  //Getters
  Int_t GetSlice() const {return fSlice;}
  Int_t GetPatch() const {return fPatch;}
  Int_t GetLastPatch() const {return fLastPatch;}
  AliL3HoughBaseTransformer *GetLastTransfromer() const {return fLastTransformer;}
  Int_t GetNEtaSegments() const {return fNEtaSegments;}
  Int_t GetLowerThreshold() const {return fLowerThreshold;}
  Int_t GetUpperThreshold() const {return fUpperThreshold;}
  Double_t GetEtaMin() const {return fEtaMin;}
  Double_t GetEtaMax() const {return fEtaMax;}
  Float_t GetZVertex() const {return fZVertex;}

  AliL3DigitRowData *GetDataPointer() {return fDigitRowData;}
 
  virtual Int_t GetEtaIndex(Double_t eta) const = 0;
  virtual void GetEtaIndexes(Double_t /*eta*/,Int_t */*indexes*/) const
    {STDCERR<<"GetEtaIndexes not implemented for this Transformer class"<<STDENDL;}
  virtual AliL3Histogram *GetHistogram(Int_t etaindex) = 0;
  virtual Double_t GetEta(Int_t etaindex,Int_t slice) const = 0;

  virtual Int_t GetTrackID(Int_t /*etaindex*/,Double_t /*kappa*/,Double_t /*psi*/) const {
    STDCERR<<"GetTrackID not implemented for this Transformer class"<<STDENDL; 
    return -1;
  }
  
  //setters
  virtual void Init(Int_t slice=0,Int_t patch=0,Int_t netasegments=100,Int_t nsegs=-1);
  void SetLowerThreshold(Int_t i) {fLowerThreshold = i;}
  void SetUpperThreshold(Int_t i) {fUpperThreshold = i;}
  void SetLastPatch(Int_t i) {fLastPatch = i;}
  void SetLastTransformer(AliL3HoughBaseTransformer *transformer) {fLastTransformer = transformer;}

  virtual void SetTPCRawStream(AliTPCRawStream */*rawstream*/){};

  virtual void Print(){};

 protected:

  AliL3HoughBaseTransformer *fLastTransformer;//Pointer to the previous hough transformer
  
 private:

  Int_t fSlice;//Index of the current slice being processed
  Int_t fPatch;//Index of the current patch being processed
  Int_t fLastPatch;//Index of the last processed patch
  Int_t fNEtaSegments;//Number of eta slices
  Double_t fEtaMin;//Minimum allowed eta
  Double_t fEtaMax;//Maximum allowed eta
  Int_t fLowerThreshold;//Lower threshold for digits amplitude 
  Int_t fUpperThreshold;//Upper threshold for digits amplitude
  
  AliL3DigitRowData *fDigitRowData; //!

  Float_t fZVertex;//Z position of the primary vertex

  ClassDef(AliL3HoughBaseTransformer,1) //Hough transformation base class

};


#endif

