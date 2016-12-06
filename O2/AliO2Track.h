/// \file AliO2Track.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2Track class.
///
/// See implementation file for copyright details.

#ifndef ALIO2TRACK_H
#define ALIO2TRACK_H
#include "O2Track.h"
#include <AliVTrack.h>

#include "O2NotImplemented.h"

class AliExternalTrackParam;
class AliO2Vertex;
/// New Track class, still highly unspecified. Basis for all track classes.
class AliO2Track : public AliVTrack, public O2Track {
public:
  AliO2Track() {}
  AliO2Track(const O2Track &);
  /// Destructor
  ~AliO2Track();

  // kinematics
  virtual Double_t Px() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t Py() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t Pz() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t Pt() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t P() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Bool_t PxPyPz(Double_t p[3]) const { EXIT_NOT_IMPLEMENTED(); }

  virtual Double_t Xv() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t Yv() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t Zv() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Bool_t XvYvZv(Double_t x[3]) const { EXIT_NOT_IMPLEMENTED(); }

  virtual Double_t OneOverPt() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t Phi() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t Theta() const { EXIT_NOT_IMPLEMENTED(); }

  virtual Double_t E() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t M() const { EXIT_NOT_IMPLEMENTED(); }

  virtual Double_t Eta() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t Y() const { EXIT_NOT_IMPLEMENTED(); }

  virtual Short_t Charge() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Int_t GetLabel() const { EXIT_NOT_IMPLEMENTED(); }
  // PID
  virtual Int_t PdgCode() const { EXIT_NOT_IMPLEMENTED(); }
  virtual const Double_t *PID() const {
    EXIT_NOT_IMPLEMENTED();
  } // return PID object (to be defined, still)

  virtual Int_t GetID() const { EXIT_NOT_IMPLEMENTED(); }
  virtual UChar_t GetITSClusterMap() const { EXIT_NOT_IMPLEMENTED(); }
  virtual ULong_t GetStatus() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Bool_t GetXYZ(Double_t *p) const { EXIT_NOT_IMPLEMENTED(); }
  virtual Bool_t GetCovarianceXYZPxPyPz(Double_t cv[21]) const {
    EXIT_NOT_IMPLEMENTED();
  }
  virtual Bool_t PropagateToDCA(const AliVVertex *vtx, Double_t b,
                                Double_t maxd, Double_t dz[2],
                                Double_t covar[3]) {
    EXIT_NOT_IMPLEMENTED();
  }

  /** Compare this class with an other instance of this class
   *  used in a TCollection::Sort()/TClonesArray::Sort()
   *  @param   obj  ptr to other instance
   *  @return  Returns 0 when equal, 1 when this is smaller
   *  and -1 when bigger -- sorts descending
   */
  Int_t Compare(const TObject *obj) const { EXIT_NOT_IMPLEMENTED(); };

  /** Defines this class as being sortable in a TCollection
   *  @return     always kTRUE;
   */
  Bool_t IsSortable() const { return kTRUE; }
  virtual Bool_t IsPrimary() const { EXIT_NOT_IMPLEMENTED(); }
  virtual void SetFlag(UInt_t) { EXIT_NOT_IMPLEMENTED(); }
  virtual UInt_t GetFlag() const { EXIT_NOT_IMPLEMENTED(); }

  // coordinate system conversions
  Bool_t Local2GlobalMomentum(Double_t p[3], Double_t alpha) const {
    EXIT_NOT_IMPLEMENTED();
  };
  Bool_t Local2GlobalPosition(Double_t r[3], Double_t alpha) const {
    EXIT_NOT_IMPLEMENTED();
  };
  Bool_t Global2LocalMomentum(Double_t p[3], Short_t charge,
                              Double_t &alpha) const {
    EXIT_NOT_IMPLEMENTED();
  };
  Bool_t Global2LocalPosition(Double_t r[3], Double_t alpha) const {
    EXIT_NOT_IMPLEMENTED();
  };

  // Navigation
  virtual Int_t GetMother() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Int_t GetFirstDaughter() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Int_t GetLastDaughter() const { EXIT_NOT_IMPLEMENTED(); }
  virtual void SetGeneratorIndex(Short_t) { EXIT_NOT_IMPLEMENTED(); }
  virtual Short_t GetGeneratorIndex() const { EXIT_NOT_IMPLEMENTED(); }

protected:
  // protected stuff goes here
private:
  /// copy constructor prohibited
  // AliO2Track(const AliO2Track &);
  /// assignment operator prohibited

  ClassDef(AliO2Track, 1);
};
/// Global tracks
class AliO2GlobalTrack : public AliO2Track {
  // c++11, inherit constructors
  using AliO2Track::AliO2Track;

public:
  timestamp_t getTimestampResolution() const {
    return 600;
  } // 100ns guassian, 6 sigma.
  ClassDef(AliO2GlobalTrack, 1);
};
/// ITS tracks
class AliO2ITSTrack : public AliO2Track {
  // c++11,  inherit constructors
  using AliO2Track::AliO2Track;

public:
  timestamp_t getTimestampResolution() const {
    return 2500; // boxed per 5000ns, so everything within this box can be
    // associated.
  }
  ClassDef(AliO2ITSTrack, 1);
};

#endif
