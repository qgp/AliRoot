// $Id$
// Category: motif
//
// Class AliMpConnection
// ---------------------
// Class that defines a connexion properties.
//
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_CONNECTION_H
#define ALI_MP_CONNECTION_H

#include <TObject.h>

#include "AliMpMotifType.h"
#include "AliMpIntPair.h"

class AliMpConnection : public TObject
{
  public:
    AliMpConnection();
    AliMpConnection(Int_t padNum,Int_t bergNum,Int_t kaptonNum,Int_t gassiNum);
    virtual ~AliMpConnection();

    // methods

    // accessors
    Int_t GetBergNum()   const {return fBergNum;}
    Int_t GetKaptonNum() const {return fKaptonNum;}
    Int_t GetGassiNum()  const {return fGassiNum;}
    Int_t GetPadNum()  const {return fPadNum;}
    AliMpMotifType *GetOwner() const {return fOwner;}
    
    AliMpIntPair LocalIndices() const;
    TString  PadName() const;
    // modifiers
    void SetOwner(AliMpMotifType *owner) {fOwner=owner;}

  private:
    // data members
    Int_t fPadNum;    // Pad number
    Int_t fBergNum;   // Berg connector number
    Int_t fKaptonNum; // Kapton connector number
    Int_t fGassiNum;  // Gassiplex channel number
    AliMpMotifType *fOwner; //The motif type which contains this connection

  ClassDef(AliMpConnection,1)  // Connection description
};

// inline functions

inline TString AliMpConnection::PadName() const 
{ return fOwner->PadName(fPadNum); }

inline AliMpIntPair AliMpConnection::LocalIndices() const
{ return fOwner->FindLocalIndicesByConnection(this);}

#endif //ALI_MP_CONNECTION_H
