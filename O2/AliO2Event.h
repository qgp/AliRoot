/// \file AliO2Event.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2Event class.
///
/// See implementation file for copyright details.

#ifndef ALIO2EVENT_H
#define ALIO2EVENT_H

// Include the base classes
#include "InterfaceTimestampped.h"
#include <AliVEvent.h>

#include "O2NotImplemented.h"

#include "AliO2Track.h"
#include "AliO2Vertex.h"
#include "O2Event.h"

#include <TBuffer.h>
#ifndef __CINT__
#include <cstdint>
#else
#include <stdint.h>
#endif

#include <vector>
/// Wrapper class which exposes a slice of a timeframe representing a single
/// collision. Pileups are currently not handled but can be added.
class AliO2Event : public AliVEvent, public InterfaceTimestampped {
public:
  /// Default constructor
  AliO2Event();
  // TODO: should make it so that this is hidden in a way (perhaps make it a
  // protected friend with timeframe).
  /// Explicit constructor
  AliO2Event(const O2Event &);
  /// Destructor
  ~AliO2Event();
  bool isPileup() const { return mVertices.size() > 1; }
  timestamp_t getTimestamp() const { return mVertices[0].getTimestamp(); }
  timestamp_t getTimestampResolution() const {
    return mVertices[0].getTimestampResolution();
  }
  Int_t GetNumberOfTracks() const {
    return GetNumberOfGlobalTracks() + GetNumberOfITSTracks();
  }
  Int_t GetNumberOfUnambigousTracks() const {
    return GetNumberOfUnambigousGlobalTracks() +
           GetNumberOfUnambigousITSTracks();
  }
  Int_t GetNumberOfAmbigousTracks() const {
    return GetNumberOfAmbigousGlobalTracks() + GetNumberOfAmbigousITSTracks();
  }
  Int_t GetNumberOfGlobalTracks() const { return mNumberOfGlobalTracks; }
  Int_t GetNumberOfUnambigousGlobalTracks() const {
    return mUnambigousGlobalTracksSize;
  }
  Int_t GetNumberOfAmbigousGlobalTracks() const {
    return GetNumberOfGlobalTracks() - GetNumberOfUnambigousGlobalTracks();
  }
  Int_t GetNumberOfITSTracks() const { return mNumberOfITSTracks; }
  Int_t GetNumberOfUnambigousITSTracks() const {
    return mUnambigousITSTracksSize;
  }
  Int_t GetNumberOfAmbigousITSTracks() const {
    return GetNumberOfITSTracks() - GetNumberOfUnambigousITSTracks();
  }
  AliO2Vertex *GetVertex() { return &mVertices[0]; }
  const AliO2Track *GetGlobalTrack(Int_t index) const {
    return (index < mNumberOfGlobalTracks ? mGlobalTracks.data() + index
                                          : NULL);
  }
  const AliO2Track *GetUnambigousGlobalTrack(Int_t index) const {
    return (index < GetNumberOfUnambigousGlobalTracks()
                ? mGlobalTracks.data() + index + mUnambigousGlobalTracksOffset
                : NULL);
  }
  const AliO2Track *GetITSTrack(Int_t index) const {
    return (index < mNumberOfITSTracks ? mITSTracks.data() + index : NULL);
  }
  const AliO2Track *GetUnambigousITSTrack(Int_t index) const {
    return (index < GetNumberOfUnambigousITSTracks()
                ? mITSTracks.data() + index + mUnambigousITSTracksOffset
                : NULL);
  }
  // This function prototype is constant, but it returns a mutable pointer.
  // We own the pointer in this case, so it is not const!
  AliO2Track *GetTrack(Int_t index) const {
    if (index >= GetNumberOfTracks()) {
      return NULL;
    }
    if (index < mNumberOfGlobalTracks) {
      return const_cast<AliO2GlobalTrack *>(mGlobalTracks.data() + index);
    } else {
      return const_cast<AliO2ITSTrack *>(mITSTracks.data() + index -
                                         mNumberOfGlobalTracks);
    }
  }
  // Services
  virtual void AddObject(TObject *obj) { EXIT_NOT_IMPLEMENTED(); }
  virtual TObject *FindListObject(const char *name) const {
    EXIT_NOT_IMPLEMENTED();
  }
  virtual TList *GetList() const { EXIT_NOT_IMPLEMENTED(); }

  virtual void CreateStdContent() { EXIT_NOT_IMPLEMENTED(); }
  virtual void GetStdContent() { EXIT_NOT_IMPLEMENTED(); }

  virtual void ReadFromTree(TTree *tree, Option_t *opt) {
    EXIT_NOT_IMPLEMENTED();
  }
  virtual void WriteToTree(TTree *tree) const { EXIT_NOT_IMPLEMENTED(); }

  virtual void Reset() { EXIT_NOT_IMPLEMENTED(); }
  // virtual void ResetStdContent() {EXIT_NOT_IMPLEMENTED();}
  virtual void SetStdNames() { EXIT_NOT_IMPLEMENTED(); }

  virtual void Print(Option_t *option = "") const { EXIT_NOT_IMPLEMENTED(); }

  // Header
  virtual AliVHeader *GetHeader() const { EXIT_NOT_IMPLEMENTED(); }
  //
  // field initialization
  virtual Bool_t InitMagneticField() const { return kFALSE; }

  // Delegated methods for fESDRun or AODHeader

  virtual void SetRunNumber(Int_t n) { EXIT_NOT_IMPLEMENTED(); }
  virtual void SetPeriodNumber(UInt_t n) { EXIT_NOT_IMPLEMENTED(); }
  virtual void SetMagneticField(Double_t mf) { EXIT_NOT_IMPLEMENTED(); }

  virtual Int_t GetRunNumber() const { -1; }
  virtual UInt_t GetPeriodNumber() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t GetMagneticField() const { EXIT_NOT_IMPLEMENTED(); }

  virtual Double_t GetDiamondX() const { return -999.; }
  virtual Double_t GetDiamondY() const { return -999.; }
  virtual void GetDiamondCovXY(Float_t cov[3]) const {
    cov[0] = -999.;
    return;
  }

  // Delegated methods for fHeader
  virtual void SetOrbitNumber(UInt_t n) { EXIT_NOT_IMPLEMENTED(); }
  virtual void SetBunchCrossNumber(UShort_t n) { EXIT_NOT_IMPLEMENTED(); }
  virtual void SetEventType(UInt_t eventType) { EXIT_NOT_IMPLEMENTED(); }
  virtual void SetTriggerMask(ULong64_t n) { EXIT_NOT_IMPLEMENTED(); }
  virtual void SetTriggerCluster(UChar_t n) { EXIT_NOT_IMPLEMENTED(); }

  virtual UInt_t GetOrbitNumber() const { EXIT_NOT_IMPLEMENTED(); }
  virtual UShort_t GetBunchCrossNumber() const { EXIT_NOT_IMPLEMENTED(); }
  virtual UInt_t GetEventType() const { EXIT_NOT_IMPLEMENTED(); }
  virtual ULong64_t GetTriggerMask() const { EXIT_NOT_IMPLEMENTED(); }
  virtual ULong64_t GetTriggerMaskNext50() const { return 0; }
  virtual UChar_t GetTriggerCluster() const { EXIT_NOT_IMPLEMENTED(); }
  virtual TString GetFiredTriggerClasses() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Bool_t IsTriggerClassFired(const char * /*name*/) const { return 0; }
  virtual Double_t GetZDCN1Energy() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t GetZDCP1Energy() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t GetZDCN2Energy() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t GetZDCP2Energy() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t GetZDCEMEnergy(Int_t i) const { EXIT_NOT_IMPLEMENTED(); }

  // Tracks
  virtual AliVTrack *GetVTrack(Int_t /*i*/) const { return NULL; }
  // virtual AliVTrack    *GetVTrack(Int_t /*i*/) {return NULL;}
  // virtual Int_t        AddTrack(const AliVParticle *t)
  // {EXIT_NOT_IMPLEMENTED();}
  virtual Int_t GetNumberOfV0s() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Int_t GetNumberOfCascades() const { EXIT_NOT_IMPLEMENTED(); }

  virtual AliCentrality *GetCentrality() { EXIT_NOT_IMPLEMENTED(); }
  virtual AliEventplane *GetEventplane() { EXIT_NOT_IMPLEMENTED(); }
  virtual Int_t EventIndex(Int_t itrack) const { EXIT_NOT_IMPLEMENTED(); }
  virtual Int_t EventIndexForCaloCluster(Int_t iclu) const {
    EXIT_NOT_IMPLEMENTED();
  }
  virtual Int_t EventIndexForPHOSCell(Int_t icell) const {
    EXIT_NOT_IMPLEMENTED();
  }
  virtual Int_t EventIndexForEMCALCell(Int_t icell) const {
    EXIT_NOT_IMPLEMENTED();
  }

  virtual AliVVZERO *GetVZEROData() const { EXIT_NOT_IMPLEMENTED(); }
  virtual AliVZDC *GetZDCData() const { EXIT_NOT_IMPLEMENTED(); }
  virtual EDataLayoutType GetDataLayoutType() const { EXIT_NOT_IMPLEMENTED(); }

protected:
  // protected stuff goes here

private:
  /// Pointer to the vertex in our timeframe
  std::vector<AliO2Vertex> mVertices;
  /// pointer to the first associated global track.  Can have ambigous tracks at
  /// the start and the end, but any unambigous tracks will be stored in the
  /// center. Sorted by time.
  std::vector<AliO2GlobalTrack> mGlobalTracks;
  /// Total amount of global tracks
  Int_t mNumberOfGlobalTracks;
  /// How far from the start of mGlobalTracks do unambigous  global tracks
  /// start.
  Int_t mUnambigousGlobalTracksOffset;
  /// How many unambigous global tracks do we have.
  Int_t mUnambigousGlobalTracksSize;

  /// pointer to the first associated ITS track.  Can have ambigous tracks at
  /// the start and the end, but any unambigous tracks will be stored in the
  /// center. Sorted by time.
  std::vector<AliO2ITSTrack> mITSTracks; //[mNumberOfITSTracks]
  /// Total amount of ITS tracks
  Int_t mNumberOfITSTracks;
  /// How far from the start of mITSTracks do unambigous  ITS tracks
  /// start.
  Int_t mUnambigousITSTracksOffset;
  /// How many unambigous ITS tracks do we have
  Int_t mUnambigousITSTracksSize;

  /// copy constructor prohibited
  // AliO2Event(const AliO2Event &);
  /// assignment operator prohibited
  // AliO2Event &operator=(const AliO2Event &);

  // root specific
  // void SerializeToBuffer(TBuffer &buffer);
  // void createFromBuffer(TBuffer &buffer);
  ClassDef(AliO2Event, 1);
};

#endif
