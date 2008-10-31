#ifndef ALITRDTRACKINFO_H
#define ALITRDTRACKINFO_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id: AliTRDtrackInfo.h 27496 2008-07-22 08:35:45Z cblume $ */

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  Reconstruction QA                                                     //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#ifndef Root_TObject
#include "TObject.h"
#endif

#ifndef ALIPID_H
#include "AliPID.h"
#endif

class AliTRDseedV1;
class AliTRDtrackV1;
class AliTrackReference;
class AliExternalTrackParam;
class AliTRDtrackInfo : public TObject{
public:
  enum{
    kNTrackRefs = 12
  };
  class AliESDinfo{
  friend class AliTRDtrackInfo;
  public:
    AliESDinfo();
    AliESDinfo(const AliESDinfo&);
    virtual ~AliESDinfo();
    AliESDinfo& operator=(const AliESDinfo&);

    Int_t       GetId() const {return fId;}
    ULong_t     GetStatus() const {return fStatus;}
    Int_t       GetKinkIndex() const {return fKinkIndex;}
    UShort_t    GetTPCncls() const {return fTPCncls;}
    UChar_t     GetPidQuality() const {return fTRDpidQuality;}
    Int_t       GetNSlices() const {return fTRDnSlices;}
    Double32_t* GetSliceIter() const {return fTRDslices;}
    Double32_t* GetResponseIter() {return &fTRDr[0];}
  protected:
    Int_t       fId;            // ESD track id
    ULong_t     fStatus;        // ESD track status
    Int_t       fKinkIndex;     // ESD kink index
    UShort_t    fTPCncls;       // Number of Clusters inside TPC
    Double32_t  fTRDr[AliPID::kSPECIES];  
    UChar_t     fTRDpidQuality; // TRD PID quality
    Int_t       fTRDnSlices;    // number of slices used for PID
    Double32_t *fTRDslices;     //[fTRDnSlices] 

    ClassDef(AliESDinfo, 1)     // ESD info related to TRD
  };

  class AliMCinfo{
  friend class AliTRDtrackInfo;
  public:
    //typedef AliTrackReference (const* constAliTrackReference);
    AliMCinfo();
    AliMCinfo(const AliMCinfo&);
    virtual ~AliMCinfo();
    AliMCinfo& operator=(const AliMCinfo&);
    Int_t   GetLabel() const {return fLabel;}
    Int_t   GetNTrackRefs() const {return fNTrackRefs;}
    Int_t   GetPDG() const {return fPDG;}
    Bool_t  GetDirections(Float_t x0, Float_t &y0, Float_t &z0, Float_t &dydx, Float_t &dzdx) const;
    AliTrackReference const* GetTrackRef(Int_t ref=0) const {return fTrackRefs[ref];}

  protected:
    Int_t   fLabel;             // MC label  
    Int_t   fPDG;               // particle code
    Int_t   fNTrackRefs;    	  // number of track refs
    AliTrackReference  *fTrackRefs[kNTrackRefs];	// track refs array
    ClassDef(AliMCinfo, 1)      // MC info related to TRD
  };

  AliTRDtrackInfo();
  AliTRDtrackInfo(const AliTRDtrackInfo &);
  ~AliTRDtrackInfo();
  
//  void               Clear(const Option_t *){}
  void               Delete(const Option_t *);
  
  AliTRDtrackInfo&   operator=(const AliTRDtrackInfo &);
  
  void               AddTrackRef(const AliTrackReference *trackRef);
  
  Int_t              GetTrackId() { return fESD.fId;}
  const AliESDinfo*  GetESDinfo() const { return &fESD; }
  const AliMCinfo*   GetMCinfo() const { return fMC; }
  Int_t              GetNumberOfClusters() const;
  Int_t              GetNumberOfClustersRefit() const {return fNClusters;}
  Int_t              GetNTracklets() const;
  Int_t              GetNTrackRefs() const {return fMC ? fMC->fNTrackRefs:0;} 
  Int_t              GetLabel() const { return fMC ? fMC->fLabel:0; }
  Int_t              GetKinkIndex() const { return fESD.fKinkIndex;}
  UShort_t           GetTPCncls() const { return fESD.fTPCncls;}
  Int_t              GetPDG() const { return fMC ? fMC->fPDG : 0; }
  ULong_t            GetStatus() const {return fESD.fStatus;}
  AliTRDtrackV1*     GetTrack() const { return fTRDtrack; }
  AliTrackReference* GetTrackRef(Int_t entry) const;
  AliExternalTrackParam* GetOuterParam() const {return fOP;}

  Bool_t             IsCurved() const {return TestBit(kCurv);}
  Bool_t             IsPrimary() const {return TestBit(kPrim);}
	Bool_t             HasESDtrack() const{return ((fTRDtrack != 0x0) ||(fOP != 0));}
	Bool_t             HasMCinfo() const { return (Bool_t)fMC; }

  void               SetCurved(Bool_t curv = kTRUE) {SetBit(kCurv, curv);}
  void               SetLabel(Int_t lab) { SetMC(); fMC->fLabel = lab; }
  void               SetNumberOfClustersRefit(Int_t n) {fNClusters = n;}
  inline void        SetMC();
  void               SetPDG(Int_t pdg) { SetMC(); fMC->fPDG = pdg; }
  void               SetPrimary(Bool_t prim = kTRUE) {SetBit(kPrim, prim);}
  void               SetOuterParam(const AliExternalTrackParam *op);
  void               SetStatus(ULong_t stat) {fESD.fStatus = stat;}
  void               SetKinkIndex(Int_t kinkIndex) {fESD.fKinkIndex = kinkIndex;}
  void               SetTPCncls(UShort_t TPCncls) {fESD.fTPCncls = TPCncls;}
  void               SetTrackId(Int_t id) {fESD.fId = id;}
  void               SetTRDtrack(const AliTRDtrackV1 *track);
  void               SetPidQuality(UChar_t q) { fESD.fTRDpidQuality = q;}
  void               SetSlices(Int_t n, Double32_t*);
  inline void        SetResponse(Double32_t *);
  
private:
    enum{
      kCurv = 14,
      kPrim = 15
  };
  // this 2 data members have to go to ESD header.
  Int_t              fNClusters;     	// Numer of clusters from refit
  AliTRDtrackV1      *fTRDtrack; 	    // tracklets data array
  AliExternalTrackParam *fOP;       	// outer param if no tracklets
  AliMCinfo          *fMC;            // MC extract for TRD
  AliESDinfo         fESD;            // ESD extract for TRD
 
  ClassDef(AliTRDtrackInfo, 2)        // TRD track info
};


//________________________________________________________
inline void AliTRDtrackInfo::SetMC()
{
  if(!fMC) fMC = new AliMCinfo();
}

//________________________________________________________
inline void AliTRDtrackInfo::SetResponse(Double32_t *r)
{
  memcpy(fESD.fTRDr, r, AliPID::kSPECIES*sizeof(Double32_t));
}

#endif
