/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $MpId: $ 

/// \ingroup mptrigger
/// \class AliMpTriggerCrate
/// \brief The class defines the properties of trigger crate
///
/// \author Ch. Finck, Subatech Nantes

#ifndef ALI_MP_TRIGGER_CRATE_H
#define ALI_MP_TRIGGER_CRATE_H

#include "AliMpArrayI.h"

#include <TNamed.h>
#include <TString.h>

class AliMpTriggerCrate : public  TNamed {

  public:
    AliMpTriggerCrate(const Char_t* name, Int_t ddlId);
    AliMpTriggerCrate(const Char_t* name, UShort_t Id, UShort_t mask, UShort_t mode, UShort_t coinc);
    AliMpTriggerCrate(TRootIOCtor* /*ioCtor*/);
    virtual ~AliMpTriggerCrate();
    
    static TString GenerateName(Int_t crateId, Int_t ddlId, Int_t nodDdls);
    
    static void SetPatchLocalBordId(Bool_t isPatchLocalBordId);
    static Bool_t IsPatchLocalBordId();

    // methods 
    Bool_t AddLocalBoard(Int_t localBoardId);

    /// get methods
    Int_t  GetDdlId() const;
    UShort_t GetId()  const;
    UShort_t GetMask() const;
    UShort_t GetMode() const;
    UShort_t GetCoinc() const;
    Int_t  GetNofLocalBoards() const;
    Int_t  GetLocalBoardId(Int_t index) const;
    Bool_t HasLocalBoard(Int_t localBoardId) const;
    
    /// set methods
    void SetDdlId(Int_t ddl) {fDdlId = ddl;}
    
  private:
    /// Not implemented
    AliMpTriggerCrate();
    /// Not implemented
    AliMpTriggerCrate(const AliMpTriggerCrate& rhs);
    /// Not implemented
    AliMpTriggerCrate& operator=(const AliMpTriggerCrate& rhs);

    // static data members
    static Bool_t fgkPatchLocalBordId;

    // data members
    UShort_t     fId;         ///< crate number
    Int_t        fDdlId;      ///< DDL to which this bus patch is connected
    AliMpArrayI  fLocalBoard; ///< local board connected to this crate
    UShort_t     fMask;       ///< regional mask
    UShort_t     fMode;       ///< mode operating for crate
    UShort_t     fCoinc;      ///< coincidence mode for crate

  ClassDef(AliMpTriggerCrate,2)  // The class collectiong electronics properties of DDL
};

// inline functions

/// Activate temporary fix for cosmic data
inline void AliMpTriggerCrate::SetPatchLocalBordId(Bool_t isPatchLocalBordId)
{ fgkPatchLocalBordId =  isPatchLocalBordId; }

/// Is activated temporary fix for cosmic data ?
inline Bool_t AliMpTriggerCrate::IsPatchLocalBordId()
{ return fgkPatchLocalBordId; }

/// Return the Ddl  Id
inline Int_t AliMpTriggerCrate::GetDdlId() const
{  return fDdlId; }

/// Return  Id
inline UShort_t AliMpTriggerCrate::GetId() const
{  return fId; }

/// Return mask
inline UShort_t AliMpTriggerCrate::GetMask() const
{  return fMask; }

/// Return Mode
inline UShort_t AliMpTriggerCrate::GetMode() const
{  return fMode; }

/// Return coinc
inline UShort_t AliMpTriggerCrate::GetCoinc() const
{  return fCoinc; }

#endif //ALI_MP_TRIGGER__CRATE_H














