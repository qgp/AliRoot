// $Id$
// Category: basic
//
// Class AliMpPadPair
// ------------------
// Wrap up for std::pair<AliMpPad, AliMpPad>
// to avoid problems with CINT.
//
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_PAD_PAIR_H
#define ALI_MP_PAD_PAIR_H

#include "AliMpBasicTypes.h"
#include "AliMpPad.h"

class AliMpPadPair : public TObject
{
  public:
    AliMpPadPair(const AliMpPad& pad1, const AliMpPad& pad2);      
    AliMpPadPair(const AliMpPadPair& pair);      
    AliMpPadPair();
    virtual ~AliMpPadPair();

    // operators    
    Bool_t operator == (const AliMpPadPair& right) const;
    Bool_t operator != (const AliMpPadPair& right) const;
    AliMpPadPair& operator = (const AliMpPadPair& right);

    // methods
    AliMpPad GetFirst() const;  
    AliMpPad GetSecond() const;  

  private:
    // data members
    PadPair  fPair;
    
  ClassDef(AliMpPadPair,1) //utility class for the motif type
};

// inline functions

inline AliMpPad AliMpPadPair::GetFirst() const  { return fPair.first; } 
inline AliMpPad AliMpPadPair::GetSecond() const { return fPair.second; } 


#endif //ALI_MP_PAD_PAIR_H
