#ifndef ALITRDCALPID_H
#define ALITRDCALPID_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Authors:                                                                  //
//                                                                           //
//  Alex Bercuci <A.Bercuci@gsi.de>                                          //
//  Alex Wilk <wilka@uni-muenster.de>                                        //
//  Prashant Shukla <shukla@pi0.physi.uni-heidelberg.de>                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <TNamed.h>

#include "AliPID.h"

class AliTRDCalPID : public TNamed
{
public:
  enum {
    kNMom      = 11,
    kNSlicesLQ = 2,
    kNSlicesNN = 8
  };

  AliTRDCalPID();
  AliTRDCalPID(const Text_t *name, const Text_t *title);
  virtual         ~AliTRDCalPID();

  virtual Bool_t   LoadReferences(Char_t *refFile) = 0;
  static  Double_t GetMomentum(Int_t ip) { 
    return (ip<0 || ip>=kNMom) ? -1.0 : fgTrackMomentum[ip]; }
  static  Double_t GetMomentumBinning(Int_t ip) { 
    return (ip<0 || ip>=kNMom+1) ? -1.0 : fgTrackMomentumBinning[ip]; }
  virtual TObject *GetModel(Int_t ip, Int_t iType, Int_t iPlane) const = 0;
  //virtual static Int_t GetModelID(Int_t mom, Int_t spec, Int_t plane) = 0;
  virtual Double_t GetProbability(Int_t spec, Float_t mom
                                , const Float_t * const dedx
                                , Float_t length, Int_t plane) const = 0;
  static  Color_t  GetPartColor(Int_t i)                    { return fgPartColor[i]; }
  static  Int_t    GetPartIndex(Int_t pdg);
  static  const Char_t  *GetPartName(Int_t i)               { return fPartName[i];   }
  static  const Char_t  *GetPartSymb(Int_t i)               { return fPartSymb[i];   }

  void     SetPartName(Int_t i, const Char_t *name) { fPartName[i] = name;   }
  void     SetPartSymb(Int_t i, const Char_t *symb) { fPartSymb[i] = symb;   }

protected:
  virtual void     Init() = 0;

  static const Char_t   *fPartName[AliPID::kSPECIES];     //! Names of particle species
  static const Char_t   *fPartSymb[AliPID::kSPECIES];     //! Symbols of particle species
  static       Color_t   fgPartColor[AliPID::kSPECIES];   //! Colors of particle species
  static       Float_t   fgTrackMomentum[kNMom];          //  Track momenta for which response functions are available
  static       Float_t   fgTrackMomentumBinning[kNMom+1]; //  Defines the start and the endpoints of the momentum bins
  TObjArray             *fModel;                          //  Model for probability estimate

 private:

  AliTRDCalPID(const AliTRDCalPID& pd);
  AliTRDCalPID    &operator=(const AliTRDCalPID &c);

  ClassDef(AliTRDCalPID, 4)                               //  Base class for TRD PID methods

};
#endif
