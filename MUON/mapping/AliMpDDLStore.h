/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// $MpId: AliMpDDLStore.h,v 1.6 2006/05/24 13:58:16 ivana Exp $ 

/// \ingroup management
/// \class AliMpDDLStore
/// \brief The top container class for DDLs, det elements and bus patched
///
/// It provides acces to DDL, det element and bus patches objects
/// via various characteristics.
///
/// \author Ivana Hrivnacova, IPN Orsay;
///         Christian Finck, SUBATECH Nantes

#ifndef ALI_MP_DDL_STORE_H
#define ALI_MP_DDL_STORE_H

#include "AliMpExMap.h"
#include "AliMpIntPair.h"
#include "AliMpGlobalCrate.h"
#include <TObject.h>
#include <TObjArray.h>
#include <TArrayI.h>


class AliMpDDL;
class AliMpDetElement;
class AliMpBusPatch;
class AliMpLocalBoard;
class AliMpTriggerCrate;

class TArrayI;

class AliMpDDLStore : public  TObject {

  public:
    AliMpDDLStore(TRootIOCtor* /*ioCtor*/);
    virtual ~AliMpDDLStore();
    
    // static access method
    static AliMpDDLStore* Instance(Bool_t warn = true); 
    static AliMpDDLStore* ReadData(Bool_t warn = true);
    
    // methods
    AliMpDDL*          GetDDL(Int_t ddlId, Bool_t warn = true) const;
    AliMpDetElement*   GetDetElement(Int_t detElemId, Bool_t warn = true) const;
    AliMpBusPatch*     GetBusPatch(Int_t busPatchId, Bool_t warn = true) const;
    AliMpLocalBoard*   GetLocalBoard(Int_t localBoardId, Bool_t warn = true) const;
    AliMpTriggerCrate* GetTriggerCrate(TString crateName, Bool_t warn = true) const;
    AliMpTriggerCrate* GetTriggerCrate(Int_t ddlId, Int_t index, Bool_t warn = true) const;
    /// Get Global Crate object
    AliMpGlobalCrate*  GetGlobalCrate() const {return fGlobalCrate;}
    
    Int_t  GetDEfromBus(Int_t busPatchId) const;
    Int_t  GetDEfromLocalBoard(Int_t localBoardId, Int_t chamberId) const;
    Int_t  GetNextDEfromLocalBoard(Int_t localBoardId, Int_t chamberId) const;
    Int_t  GetPreviousDEfromLocalBoard(Int_t localBoardId, Int_t chamberId) const;

    Int_t  GetDDLfromBus(Int_t busPatchId) const;
    Int_t  GetBusPatchId(Int_t detElemId, Int_t manuId) const;
    
    /// Return trigger crates iterator
    TExMapIter GetTriggerCrateItr() const {return fTriggerCrates.GetIterator();}

    /// Return trigger local board iterator
    TExMapIter GetLocalBoardItr() const {return fLocalBoards.GetIterator();}

    /// Get an iterator to loop over bus patches
    TExMapIter GetBusPatchesIterator() const { return fBusPatches.GetIterator(); }
    
    /// read Global trigger crate file
    static Bool_t ReadGlobalTrigger(AliMpGlobalCrate& crate, const Char_t* globalName = 0);
       
    /// Get detection elt and Manu number from serial number
    AliMpIntPair  GetDetElemIdManu(Int_t manuSerial) const;

    /// print info of all manus
    void PrintAllManu() const;

    
  private:
    AliMpDDLStore();
    /// Not implemented
    AliMpDDLStore(const AliMpDDLStore& rhs);
    /// Not implemented
    AliMpDDLStore& operator=(const AliMpDDLStore& rhs);

    // methods
    Int_t  GetManuListIndex(Int_t detElemId) const;
    Int_t  GetBusPatchIndex(Int_t detElemId, Int_t manuId) const;
    Bool_t ReadDDLs();
    Bool_t ReadTriggerDDLs();
    Bool_t ReadLocalTrigger();
    Bool_t SetManus();
    Bool_t SetPatchModules();
    Bool_t SetBusPatchLength();
    Int_t  GetLocalBoardId(TString name) const;

    // static data members	
    static AliMpDDLStore* fgInstance; ///< Singleton instance
    static const Int_t    fgkNofDDLs; ///< Total number of DDLs
    static const Int_t    fgkNofTriggerDDLs; ///< Total number of trigger DDLs

    // data members	
    TObjArray     fDDLs;           ///< Array of DDL objects
    AliMpExMap    fBusPatches;     ///< The map of bus patches per their IDs
    AliMpExMap    fTriggerCrates;  ///< The map of trigger crate per their ID
    AliMpExMap    fLocalBoards;    ///< The map of local board per their ID
    TArrayI       fManuList12[16]; ///< Arrays of 1st manu in bus
    TArrayI       fManuBridge2[16]; ///< Arrays of manu number before the bridge in buspatch
    AliMpGlobalCrate* fGlobalCrate;  ///< Global Crate Object 
        
  ClassDef(AliMpDDLStore,3)  // The manager class for definition of detection element types
};

#endif //ALI_MP_DDL_STORE_H















