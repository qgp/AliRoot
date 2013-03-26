#ifndef ALIITSUSEGMENTATIONPIX_H
#define ALIITSUSEGMENTATIONPIX_H

#include "AliITSsegmentation.h"



// segmentation and response for pixels in ITS upgrade 

class AliITSUSegmentationPix :
public AliITSsegmentation {
 public:
  AliITSUSegmentationPix(UInt_t id=0, int nchips=0,int ncol=0,int nrow=0,
			   double pitchX=0,double pitchZ=0,
			   double thickness=0,
			   double pitchLftC=-1,double pitchRgtC=-1,
			   double edgL=0,double edgR=0,double edgT=0,double edgB=0);
  
  //  AliITSUSegmentationPix(Option_t *opt="" );
  AliITSUSegmentationPix(const AliITSUSegmentationPix &source);
  virtual ~AliITSUSegmentationPix() {}
  AliITSUSegmentationPix& operator=(const AliITSUSegmentationPix &source);
  //
  virtual void    Init();
  //  
  virtual void    SetNPads(Int_t, Int_t) {MayNotUse("SetPadSize");}
  virtual Int_t   GetNPads() const {return fNCol*fNRow;}
  //
  virtual void    GetPadIxz(Float_t x,Float_t z,Int_t &ix,Int_t &iz) const;
  virtual void    GetPadCxz(Int_t ix,Int_t iz,Float_t &x,Float_t &z) const;
  virtual void    GetPadTxz(Float_t &x ,Float_t &z) const;
  virtual Bool_t  LocalToDet(Float_t x,Float_t z,Int_t &ix,Int_t &iz) const;
  virtual void    DetToLocal(Int_t ix,Int_t iz,Float_t &x,Float_t &z) const;
  virtual void    CellBoundries(Int_t ix,Int_t iz,Double_t &xl,Double_t &xu,
				Double_t &zl,Double_t &zu) const;
  //
  virtual Int_t    GetNumberOfChips() const {return fNChips;}
  virtual Int_t    GetMaximumChipIndex() const {return fNChips-1;}
  //
  virtual Int_t    GetChipFromLocal(Float_t, Float_t zloc) const;
  virtual Int_t    GetChipsInLocalWindow(Int_t* array, Float_t zmin, Float_t zmax, Float_t, Float_t) const;
  //
  virtual Int_t    GetChipFromChannel(Int_t, Int_t iz) const;
  //
  virtual Float_t Dpx(Int_t ix=0) const;
  virtual Float_t Dpz(Int_t iz)   const;
  //
  Int_t   GetNRow()               const {return fNRow;}
  Int_t   GetNCol()               const {return fNCol;}
  //
  virtual Int_t                   Npx() const {return GetNRow();}
  virtual Int_t                   Npz() const {return GetNCol();}
  //
  virtual void Neighbours(Int_t iX,Int_t iZ,Int_t* Nlist,Int_t Xlist[10],Int_t Zlist[10]) const;
  //
  virtual void PrintDefaultParameters() const {AliWarning("No def. parameters defined as const static data members");}
  //
  virtual Int_t                    GetDetTypeID()              const {return GetUniqueID();}
  //
  Bool_t                           Store(const char* outf);
  static AliITSUSegmentationPix*   LoadWithID(UInt_t id, const char* inpf);
  static void                      LoadSegmentations(TObjArray* dest, const char* inpf);
  //
 protected:
  Float_t Z2Col(Float_t z) const;
  Float_t Col2Z(Int_t col) const;
  //
 protected:
    Double_t fGuardLft;        // left guard edge
    Double_t fGuardRgt;        // right guard edge
    Double_t fGuardTop;        // upper guard edge
    Double_t fGuardBot;        // bottom guard edge
    Double_t fPitchX;          // default pitch in X
    Double_t fPitchZ;          // default pitch in Z
    Double_t fPitchZLftCol;    // Z pitch of left column of each chip
    Double_t fPitchZRgtCol;    // Z pitch of right column of each chip
    Double_t fChipDZ;          // aux: chip size along Z
    Int_t    fNChips;          // number of chips per module
    Int_t    fNColPerChip;     // number of columns per chip
    Int_t    fNRow;            // number of rows
    Int_t    fNCol;            // number of columns (total)
    //
    static const char* fgkSegmListName; // pattern for segmentations list name
    //
  ClassDef(AliITSUSegmentationPix,1) //Segmentation class upgrade pixels 

};

#endif
