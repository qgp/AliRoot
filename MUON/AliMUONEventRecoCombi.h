#ifndef ALIMUONEVENTRECOCOMBI_H
#define ALIMUONEVENTRECOCOMBI_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

/// \ingroup rec
/// \class AliMUONEventRecoCombi
/// \brief Combined cluster / track finder in MUON arm of ALICE
 
#include <TObject.h>
#include <TArrayD.h>
class TClonesArray;
class AliMUONData;
class AliMUONDetElement;
class AliMUONTrackReconstructor;
class AliMUONClusterFinderAZ;
class AliMUONHitForRec;
class AliLoader;

class AliMUONEventRecoCombi : public TObject 
{
 public:
    virtual ~AliMUONEventRecoCombi();
    static AliMUONEventRecoCombi* Instance();
    void FillEvent(AliMUONData *dataCluster, AliMUONData *dataEvent, AliMUONClusterFinderAZ *recModel); // fill event info
    void FillRecP(AliMUONData *dataCluster, AliMUONTrackReconstructor *recoTrack); // fill used rec. points from det. elems

    Int_t Nz() const { return fNZ; } // number of DE different Z-positions
    Double_t Z(Int_t iz) const { return (*fZ)[iz]; } // Z of DE
    Int_t *DEatZ(Int_t iz) const { return fDEvsZ[iz]+1; } // list of DE's at Z
    AliMUONDetElement *DetElem(Int_t iPos) const { return (AliMUONDetElement*) fDetElems->UncheckedAt(iPos); }
    Int_t IZfromHit(AliMUONHitForRec *hit) const; // IZ from Hit

 protected:
    AliMUONEventRecoCombi();
    //AliMUONEventRecoCombi(const AliMUONEventRecoCombi& rhs);
    //AliMUONEventRecoCombi & operator = (const AliMUONEventRecoCombi& rhs);

 private:
    static AliMUONEventRecoCombi* fgRecoCombi; // ! singleton instance
    TClonesArray *fDetElems; // ! array of Det. Elem. objects
    TArrayD *fZ; // ! array of det. elem. Z-coordinates
    Int_t fNZ; // ! number of different Z's
    Int_t **fDEvsZ; // ! list of DE's vs Z-coordinates

    ClassDef(AliMUONEventRecoCombi, 0) // Combined cluster/track finder steering class
      };
#endif
