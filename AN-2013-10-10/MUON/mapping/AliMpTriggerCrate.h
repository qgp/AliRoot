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
    AliMpTriggerCrate(const Char_t* name, UShort_t Id);
    AliMpTriggerCrate(TRootIOCtor* /*ioCtor*/);
    virtual ~AliMpTriggerCrate();
    
    static TString GenerateName(Int_t crateId, Int_t ddlId, Int_t nodDdls);

    // methods 
    Bool_t AddLocalBoard(Int_t localBoardId);

    /// get methods
    Int_t  GetDdlId() const;
    UShort_t GetId()  const;
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

    // data members
    UShort_t     fId;         ///< crate number
    Int_t        fDdlId;      ///< DDL to which this bus patch is connected
    AliMpArrayI  fLocalBoard; ///< local board connected to this crate

  ClassDef(AliMpTriggerCrate,3)  // The class collectiong electronics properties of DDL
};

// inline functions


/// Return the Ddl  Id
inline Int_t AliMpTriggerCrate::GetDdlId() const
{  return fDdlId; }

/// Return  Id
inline UShort_t AliMpTriggerCrate::GetId() const
{  return fId; }


#endif //ALI_MP_TRIGGER__CRATE_H














