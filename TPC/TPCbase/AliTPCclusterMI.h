#ifndef ALITPCCLUSTERMI_H
#define ALITPCCLUSTERMI_H

/// \class AliTPCclusterMI
/// \brief TPC Cluster Class
///
/// Parallel tracking
///
/// \author Marian Ivanov

/* $Id$ */


#include "AliCluster.h"
#include "TMath.h"
//#include "AliTPCclusterInfo.h"
#include <AliTrackPointArray.h>

//_____________________________________________________________________________
class AliTPCclusterMI : public AliCluster {
  enum Status{ kDisabled = 0x7F};
public:
  AliTPCclusterMI();
  AliTPCclusterMI(const AliTPCclusterMI & cluster);
  AliTPCclusterMI &operator = (const AliTPCclusterMI & cluster); //assignment operator
  AliTPCclusterMI(Int_t *lab, Float_t *hit);
  virtual ~AliTPCclusterMI();
  virtual void	Clear(const Option_t*) { };//delete fInfo; fInfo=0;}
  virtual Bool_t IsSortable() const;
  virtual Int_t Compare(const TObject* obj) const;
  inline  void Use(Int_t inc=10);
  inline  void Disable(){fUsed=kDisabled;}
  inline  Bool_t IsDisabled() const {return (fUsed==kDisabled);}

  virtual Int_t GetDetector() const {return fDetector;}
  virtual Int_t GetRow() const {return fRow;}
  virtual void SetDetector(Int_t detector);
  virtual void SetRow(Int_t row){fRow = (UChar_t)(row%256);}
  virtual void SetTimeBin(Float_t timeBin){ fTimeBin= timeBin;}
  virtual void SetPad(Float_t pad){ fPad = pad;}
  //
  void SetQ(Float_t q) {fQ=(UShort_t)q;}
  void SetType(Char_t type) {fType=type;}
  void SetMax(UShort_t max) {fMax=max;}
  Int_t IsUsed(Int_t th=10) const {return (fUsed>=th) ? 1 : 0;}
  Float_t GetQ() const {return TMath::Abs(fQ);}
  Float_t GetMax() const {return fMax;}
  Char_t  GetType()const {return fType;}
  Float_t GetTimeBin() const { return fTimeBin;}
  Float_t GetPad() const { return fPad;}
  //  AliTPCclusterInfo * GetInfo() const { return fInfo;}
  //  void SetInfo(AliTPCclusterInfo * info);
  //
  AliTPCclusterMI*  MakeCluster(AliTrackPoint* point);
  AliTrackPoint*    MakePoint();
  static void     SetGlobalTrackPoint(const AliCluster &cl, AliTrackPoint &point);

private:
  //  AliTPCclusterInfo * fInfo;  ///< pointer to the cluster debug info
  Float_t   fTimeBin;  ///< time bin coordinate
  Float_t   fPad;  ///< pad coordinate
  Short_t   fQ ;       ///< Q of cluster (in ADC counts)
  Short_t   fMax;      ///< maximal amplitude in cluster
  Char_t    fType;     ///< type of the cluster 0 means golden
  Char_t    fUsed;     ///< counter of usage
  UChar_t   fDetector; ///< detector  number
  UChar_t   fRow;      ///< row number number
  /// \cond CLASSIMP
  ClassDef(AliTPCclusterMI,6)  // Time Projection Chamber clusters
  /// \endcond
};

void AliTPCclusterMI::Use(Int_t inc)
{
  if (inc>0)  fUsed+=inc;
  else
    fUsed=0;
}



#endif


