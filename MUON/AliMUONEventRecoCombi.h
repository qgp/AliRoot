#ifndef ALIMUONEVENTRECOCOMBI_H
#define ALIMUONEVENTRECOCOMBI_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

/// \ingroup rec
/// \class AliMUONEventRecoCombi
/// \brief Combined cluster / track finder in the MUON arm of ALICE
/// \author Alexander Zinchenko, JINR Dubna
 
#include <TObject.h>
#include <TArrayD.h>
#include <TClonesArray.h>

class AliMUONData;
class AliMUONDetElement;
class AliMUONTrackReconstructorK;
class AliMUONClusterFinderAZ;
class AliMUONHitForRec;
class AliMUONSegmentation;
class AliLoader;

class AliMUONEventRecoCombi : public TObject 
{
 public:
    virtual ~AliMUONEventRecoCombi();
    static AliMUONEventRecoCombi* Instance(AliMUONSegmentation* segmentation = 0);
    void FillEvent(AliMUONData *data, AliMUONClusterFinderAZ *recModel); // fill event info
    void FillRecP(AliMUONData *dataCluster, AliMUONTrackReconstructorK *recoTrack) const; // fill used rec. points from det. elems

    Int_t Nz() const { return fNZ; } ///< number of DE different Z-positions
    Double_t Z(Int_t iz) const { return (*fZ)[iz]; } ///< Z of DE
    Int_t *DEatZ(Int_t iz) const { return fDEvsZ[iz]+1; } ///< list of DE's at Z
    AliMUONDetElement *DetElem(Int_t iPos) const { return (AliMUONDetElement*) fDetElems->UncheckedAt(iPos); } ///< Det element
    Int_t IZfromHit(AliMUONHitForRec *hit) const; // IZ from Hit

 protected:
    AliMUONEventRecoCombi(AliMUONSegmentation* segmentation = 0);

 private:
    /// Not implemented
    AliMUONEventRecoCombi(const AliMUONEventRecoCombi& rhs);
    /// Not implemented
    AliMUONEventRecoCombi & operator = (const AliMUONEventRecoCombi& rhs);

    static AliMUONEventRecoCombi* fgRecoCombi; //!< singleton instance
    AliMUONSegmentation*  fSegmentation;  //!< Segmentation
    TClonesArray *fDetElems; //!< array of Det. Elem. objects
    TArrayD *fZ; //!< array of det. elem. Z-coordinates
    Int_t fNZ; //!< number of different Z's
    Int_t **fDEvsZ; //!< list of DE's vs Z-coordinates

    ClassDef(AliMUONEventRecoCombi, 0) // Combined cluster/track finder steering class
      };
#endif
