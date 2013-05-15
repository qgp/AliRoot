/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// $MpId: AliMpDEManager.h,v 1.6 2006/05/24 13:58:16 ivana Exp $ 

/// \ingroup management
/// \class AliMpDEManager
/// \brief The manager class for definition of detection element types
///
/// The detection element types are defined via unique names
/// in denames.dat file for each station in the mapping data.
/// Detection element name is composed of DETypeName and planeTypeName.
/// DETypeName is only one per station in case of station1 and 2 quadrants, 
/// there are more DETypes in case of slat and trigger stations. 
///
/// \author Ivana Hrivnacova, IPN Orsay;
///         Laurent Aphecetche, SUBATECH Nantes

#ifndef ALI_MP_DE_MANAGER_H
#define ALI_MP_DE_MANAGER_H

#include <TObject.h>
#include <TArrayI.h>

#include "AliMpExMap.h"
#include "AliMpPlaneType.h"
#include "AliMpCathodType.h"
#include "AliMpStationType.h"
#include "AliMpStation12Type.h"
#include "AliMpDEIterator.h"
#include "AliMpEncodePair.h"

class AliMpDetElement;
class TString;

class AliMpDEManager : public  TObject {

  public:
    virtual ~AliMpDEManager();
    
    // methods
    static Bool_t IsValidDetElemId(Int_t detElemId, Bool_t warn = false);
    static Bool_t IsValidChamberId(Int_t chamberId, Bool_t warn = false);
    static Bool_t IsValidGeomModuleId(Int_t moduleId, Bool_t warn = false);

    static Int_t   GetChamberId(Int_t detElemId, Bool_t warn = true);    
    static Int_t   GetGeomModuleId(Int_t detElemId, Bool_t warn = true);    
    static AliMp::PlaneType     GetPlaneType(Int_t detElemId, AliMp::CathodType cath);
    static AliMp::CathodType    GetCathod(Int_t detElemId, AliMp::PlaneType planeType);
    static AliMp::StationType   GetStationType(Int_t detElemId);
    static AliMq::Station12Type GetStation12Type(Int_t detElemId);

    static AliMpDetElement* GetDetElement(Int_t detElemId, Bool_t warn = true);
    static AliMpDetElement* GetDetElement(const TString& deName, Bool_t warn = true);

    static Int_t     GetNofDEInChamber(Int_t chamberId, Bool_t warn = true);
    static MpPair_t  GetDetElemIdRange(Int_t chamberId);

  private:
    /// Not implemented
    AliMpDEManager();
    /// Not implemented
    AliMpDEManager(const AliMpDEManager& rhs);
    /// Not implemented
    AliMpDEManager& operator=(const AliMpDEManager& rhs);

    // static methods
    static  TArrayI&  GetNofDEPerChamber();
      
    // static data members	
    static const Int_t  fgkCoefficient; ///< Coefficient used in DE Id <-> station

  ClassDef(AliMpDEManager,0)  // The manager class for definition of detection element types
};

#endif //ALI_MP_MANAGER_H















