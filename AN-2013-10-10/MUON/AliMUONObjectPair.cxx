/**************************************************************************
* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
*                                                                        *
* Author: The ALICE Off-line Project.                                    *
* Contributors are mentioned in the code where appropriate.              *
*                                                                        *
* Permission to use, copy, modify and distribute this software and its   *
* documentation strictly for non-commercial purposes is hereby granted   *
* without fee, provided that the above copyright notice appears in all   *
* copies and that both the copyright notice and this permission notice   *
* appear in the supporting documentation. The authors make no claims     *
* about the suitability of this software for any purpose. It is          *
* provided "as is" without express or implied warranty.                  *
**************************************************************************/

// $Id$

#include "AliMUONObjectPair.h"

#include "AliLog.h"
#include <Riostream.h>

//-----------------------------------------------------------------------------
/// \class AliMUONObjectPair
///
/// The equivalent of a std::pair<TObject*,TObject*> ;-)
///
/// What else can be said ? That if we'd been using STL, that class
/// would not be there, thus saving some octets ? No comment.
/// 
/// Well, in fact, there *is* a difference wrt to std::pair : here
/// we decide on the ownership of the first and/or second object...
///
/// \author Laurent Aphecetche
//-----------------------------------------------------------------------------

using std::cout;
using std::endl;
/// \cond CLASSIMP
ClassImp(AliMUONObjectPair)
/// \endcond

//_____________________________________________________________________________
AliMUONObjectPair::AliMUONObjectPair() 
: TObject(),
fFirst(0x0),
fSecond(0x0),
fIsOwnerOfFirst(kTRUE),
fIsOwnerOfSecond(kTRUE)
{
  /// ctor
  AliDebug(1,Form("this=%p",this));
}

//_____________________________________________________________________________
AliMUONObjectPair::AliMUONObjectPair(TObject* first, 
                  TObject* second,
                  Bool_t isOwnerOfFirst,
                  Bool_t isOwnerOfSecond)
: TObject(),
fFirst(first),
fSecond(second),
fIsOwnerOfFirst(isOwnerOfFirst),
fIsOwnerOfSecond(isOwnerOfSecond)
{
  /// ctor
  AliDebug(1,Form("this=%p first is %s second is %s",
                  this,
                  (first ? first->ClassName() : "0x0"),
                  (second ? second->ClassName() : "0x0")
                  ));

}

//_____________________________________________________________________________
AliMUONObjectPair::AliMUONObjectPair(const AliMUONObjectPair& other)
: TObject(other),
fFirst(0x0),
fSecond(0x0),
fIsOwnerOfFirst(kTRUE),
fIsOwnerOfSecond(kTRUE)
{
  /// copy ctor
  AliDebug(1,Form("this=%p copy ctor",this));
  other.Copy(*this);
}

//_____________________________________________________________________________
AliMUONObjectPair& 
AliMUONObjectPair::operator=(const AliMUONObjectPair& other)
{
  /// assignement operator
  if ( this != &other)
  {
    other.Copy(*this);
  }
  return *this;
}

//_____________________________________________________________________________
AliMUONObjectPair::~AliMUONObjectPair()
{
  /// dtor
  AliDebug(1,Form("this=%p",this));
  if ( fIsOwnerOfFirst ) delete fFirst;
  if ( fIsOwnerOfSecond ) delete fSecond;
}

//_____________________________________________________________________________
void
AliMUONObjectPair::Clear(Option_t*)
{
  /// Reset
  if ( fIsOwnerOfFirst ) delete fFirst;
  if ( fIsOwnerOfSecond ) delete fSecond;
  fFirst = 0x0;
  fSecond = 0x0;
}

//_____________________________________________________________________________
void
AliMUONObjectPair::Copy(TObject& other) const
{
  /// Copy this to other (used by copy ctor and operator=)
  
  TObject::Copy(other);
  AliMUONObjectPair& pair = (AliMUONObjectPair&)(other);
  pair.fIsOwnerOfFirst = fIsOwnerOfFirst;
  pair.fIsOwnerOfSecond = fIsOwnerOfSecond;
  if ( fIsOwnerOfFirst ) 
  {
    pair.fFirst = fFirst->Clone();
  }
  else
  {
    pair.fFirst = fFirst;
  }
  if ( fIsOwnerOfSecond )
  {
    pair.fSecond = fSecond->Clone();
  }
  else
  {
    pair.fSecond = fSecond;
  }
}

//_____________________________________________________________________________
void
AliMUONObjectPair::Print(Option_t* opt) const
{
  /// Printout
  
  cout << "First:";
  if ( First() ) 
  {
    First()->Print(opt);
  }
  else
  {
    cout << " NULL ";
  }
  cout << endl;
  cout << "Second:";
  if ( Second() ) 
  {
    Second()->Print(opt);
  }
  else
  {
    cout << " NULL ";
  }
  cout << endl;    
}
