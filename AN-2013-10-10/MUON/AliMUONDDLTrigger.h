#ifndef ALIMUONDDLTRIGGER_H
#define ALIMUONDDLTRIGGER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*$Id$*/

/// \ingroup raw
/// \class AliMUONDDLTrigger
/// \brief MUON DDL Trigger
///
//  Author Christian Finck

#include <TObject.h>

class AliMUONLocalStruct;
class AliMUONRegHeader;
class AliMUONDarcHeader;

class AliMUONDDLTrigger : public TObject {
 
public:
   AliMUONDDLTrigger();
   AliMUONDDLTrigger(TRootIOCtor* dummy);
   virtual ~AliMUONDDLTrigger();


   void    AddLocStruct(const AliMUONLocalStruct& loc, Int_t iReg);
   void    AddRegHeader(const AliMUONRegHeader& regHeader);

   /// get AliMUONDarcHeader
   AliMUONDarcHeader*  GetDarcHeader() const {return fDarcHeader;}

 private:
   /// Not implemented
   AliMUONDDLTrigger(const AliMUONDDLTrigger& event);
   /// Not implemented
   AliMUONDDLTrigger& operator=(const AliMUONDDLTrigger& event);

   AliMUONDarcHeader* fDarcHeader;  ///< pointer of darc header

   ClassDef(AliMUONDDLTrigger,1)  // MUON DDL Trigger
};
#endif
