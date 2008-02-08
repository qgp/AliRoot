#ifndef ALIMUONTRACKERDATA_H
#define ALIMUONTRACKERDATA_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
* See cxx source for full Copyright notice                               */

// $Id$

/// \ingroup graphics
/// \class AliMUONTrackerData
/// \brief Implementation of AliMUONVTrackerData
/// 
// Author Laurent Aphecetche, Subatech

#ifndef ALIMUONVTRACKERDATA_H
#  include "AliMUONVTrackerData.h"
#endif

class AliMUONSparseHisto;
class AliMUONVCalibParam;
class AliMUONVStore;
class AliMpDetElement;
class TH1;

class AliMUONTrackerData : public AliMUONVTrackerData
{
public:
  AliMUONTrackerData(const char* name="", const char* title="", 
                     Int_t dimension=0,
                     Bool_t runnable=kTRUE);
  virtual ~AliMUONTrackerData();

  virtual Bool_t Add(const AliMUONVStore& channelValues);
  
  virtual Double_t BusPatch(Int_t busPatchId, Int_t dim=0) const;

  virtual Double_t Chamber(Int_t chamberId, Int_t dim=0) const;

  virtual Double_t Channel(Int_t detElemId, Int_t manuId, Int_t manuChannel,
                           Int_t dim=0) const;
  
  virtual void Clear(Option_t* opt="");
  
  virtual Double_t Count(Int_t detElemId, Int_t manuId, Int_t manuChannel) const;
  
  virtual Double_t DetectionElement(Int_t detElemId, Int_t dim=0) const;

  virtual TString DimensionName(Int_t dim) const;
  
  virtual TString ExternalDimensionName(Int_t dim) const;

  virtual Bool_t HasChamber(Int_t chamberId) const;
  
  virtual Bool_t HasBusPatch(Int_t busPatchId) const;

  virtual Bool_t HasDetectionElement(Int_t detElemId) const;

  virtual Bool_t HasManu(Int_t detElemId, Int_t manuId) const;

  virtual Bool_t HasPCB(Int_t detElemId, Int_t pcbIndex) const;
  
  /// Whether we can be run
  virtual Bool_t IsRunnable() const { return fIsRunnable; }
  
  virtual Double_t Manu(Int_t detElemId, Int_t manuId, Int_t dim=0) const;
      
  /// Returns the number of dimensions (i.e. the number of values) each element has
  virtual Int_t NumberOfDimensions() const;
  
  /// Returns the number of events we have seen so far
  virtual Int_t NumberOfEvents() const { return fNevents; }
  
  virtual Double_t PCB(Int_t detElemId, Int_t pcbIndex, Int_t dim=0) const;

  using TObject::Print;
  
  /// Print, with option, all objects whose name matches wildcard
  virtual void Print(Option_t* wildcard, Option_t* opt) const;
  
  virtual void SetDimensionName(Int_t index, const char* value);  

  Bool_t CanHistogram() const { return kTRUE; }
  
  void SetHistogramDimension(Int_t index, Bool_t value);
  
  TH1* CreateChannelHisto(Int_t detElemId, Int_t manuId, 
                          Int_t manuChannel, Int_t dim=0);

  TH1* CreateBusPatchHisto(Int_t busPatchId, Int_t dim=0);
  
  TH1* CreateDEHisto(Int_t detElemId, Int_t dim=0);

  TH1* CreateManuHisto(Int_t detElemId, Int_t manuId, Int_t dim=0);

  TH1* CreatePCBHisto(Int_t detElemId, Int_t pcbIndex, Int_t dim=0);
  
  TH1* CreateChamberHisto(Int_t chamberId, Int_t dim=0);
  
private:
    
  void FillChannel(Int_t detElemId, Int_t manuId, Int_t manuChannel,
                   Int_t dim, Double_t value);

  AliMUONSparseHisto* GetChannelHisto(Int_t detElemId, Int_t manuId, 
                                      Int_t manuChannel, Int_t dim=0);
  
  AliMUONVCalibParam* BusPatchParam(Int_t busPatch, Bool_t create=kFALSE) const;

  AliMUONVCalibParam* CreateBusPatchParam(Int_t busPatch) const;
  
  AliMUONVCalibParam* ChamberParam(Int_t chamberId, Bool_t create=kFALSE) const;

  AliMUONVCalibParam* CreateChamberParam(Int_t chamberId) const;
  
  AliMUONVCalibParam* ChannelParam(Int_t detElemId, Int_t manuId,
                                   AliMUONVCalibParam* external=0x0) const;

  AliMUONVCalibParam* DetectionElementParam(Int_t detElemId, Bool_t create=kFALSE) const;

  AliMUONVCalibParam* CreateDetectionElementParam(Int_t detElemId) const;
  
  AliMUONVCalibParam* ManuParam(Int_t detElemId, Int_t manuId, Bool_t create=kFALSE) const;

  AliMUONVCalibParam* CreateManuParam(Int_t detElemInd, Int_t manuId) const;
  
  AliMUONVCalibParam* PCBParam(Int_t detElemId, Int_t pcbIndex, Bool_t create=kFALSE) const;

  AliMUONVCalibParam* CreatePCBParam(Int_t detElemId, Int_t pcbIndex) const;
  
  /// Index of the dimension containing the number of time an item was hit
  virtual Int_t IndexOfNumberDimension() const { return fDimension - 1; }

  /// Index of the dimension containing the occupancy number
  virtual Int_t IndexOfOccupancyDimension() const { return fDimension - 2; }

private:
  /// Not implemented
  AliMUONTrackerData(const AliMUONTrackerData& rhs);
  /// Not implemented
  AliMUONTrackerData& operator=(const AliMUONTrackerData& rhs);
  
  void Add(TH1& h, const AliMUONSparseHisto& sh);
  
  void AddManuHisto(TH1& h, Int_t detElemId, Int_t manuId, Int_t dim);

  void AddBusPatchHisto(TH1& h, Int_t busPatchId, Int_t dim);

  void AddDEHisto(TH1& h, Int_t detElemId, Int_t dim);
  
  TH1* CreateHisto(const char* name, Int_t dim) const;

  AliMUONVCalibParam* CreateDouble(const AliMUONVCalibParam& param) const;

  Int_t GetParts(AliMUONVCalibParam* external,
                 AliMUONVCalibParam*& chamber,
                 AliMUONVCalibParam*& de,
                 AliMUONVCalibParam*& busPatch,
                 AliMUONVCalibParam*& pcb,
                 AliMUONVCalibParam*& manu,
                 AliMUONVCalibParam*& channel,
                 AliMpDetElement*& mpde);

  /// Convert from external to internal index
  Int_t External2Internal(Int_t index) const { return index*2; }

  void SetInternalDimensionName(Int_t index, const char* value);  

  void SetExternalDimensionName(Int_t index, const char* value);  

  Double_t Value(const AliMUONVCalibParam& param, Int_t i, Int_t dim) const;
  
  /// The number of values we actually *store* for each item
  Int_t Dimension() const { return fDimension; }

  /// The number of values we are inputting
  Int_t ExternalDimension() const { return fExternalDimension; }
  
  /// Whether we have histograms for a given dimension, or not
  Bool_t IsHistogrammed(Int_t dim) const { return ( fHistogramming[dim] > 0 ); }
  
private:
    
  AliMUONVStore* fChannelValues; ///< the channel store
  AliMUONVStore* fManuValues; ///< the manu store
  AliMUONVStore* fBusPatchValues; ///< the bus patch store
  AliMUONVStore* fDEValues; ///< the detection element store
  AliMUONVStore* fChamberValues; ///< the chamber store
  AliMUONVStore* fPCBValues; ///< the pcb store
  Int_t fDimension; ///< the dimension of the data
  Int_t fNevents; ///< the number of events treated
  TObjArray* fDimensionNames; ///< the names of the (internal) dimensions
  TObjArray* fExternalDimensionNames; ///< the names of the external (i.e. original) dimensions
  Int_t fExternalDimension; ///< number of interface values per item 
  Bool_t fIsRunnable; ///< whether we can deal with more than one event
  /// whether we should histogram the dimension(s)
  Int_t* fHistogramming; //[fExternalDimension] whether we should histogram the dimension(s)
  AliMUONVStore* fChannelHistos; ///< the channel histograms
  
  static const Int_t fgkExtraDimension; ///< to hold extra information
  static const Int_t fgkVirtualExtraDimension; ///< to give access to information not stored, but computed on the fly
  
  ClassDef(AliMUONTrackerData,2) // Implementation of AliMUONVTrackerData
};

#endif
