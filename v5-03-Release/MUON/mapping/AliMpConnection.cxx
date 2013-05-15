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
// $MpId: AliMpConnection.cxx,v 1.7 2006/05/24 13:58:41 ivana Exp $
// Category: motif

//-----------------------------------------------------------------------------
// Class AliMpConnection
// ----------------
// Class that defines a connexion properties.
// Included in AliRoot: 2003/05/02
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay
//-----------------------------------------------------------------------------

#include "AliMpConnection.h"
#include "AliMpEncodePair.h"

#include "AliLog.h"

/// \cond CLASSIMP
ClassImp(AliMpConnection)
/// \endcond

//_____________________________________________________________________________
AliMpConnection::AliMpConnection(Int_t padNum, 
                                 Int_t bergNum,
                                 Int_t kaptonNum,
		                 Int_t gassiNum, 
                                 MpPair_t localIndices) 
  : TObject(),
    fBergNum(bergNum),
    fKaptonNum(kaptonNum),
    fGassiNum(gassiNum),
    fLocalIndices(localIndices),
    fOwner(0)
{
/// Standard constructor

  AliDebug(1,Form("this=%p padNum=%d bergNum=%d kaptonNum=%d gassiNum=%d",
                   this,padNum,bergNum,kaptonNum,gassiNum));
  SetUniqueID(padNum);
}

//_____________________________________________________________________________
AliMpConnection::AliMpConnection(TRootIOCtor* /*ioCtor*/) 
  : TObject(),
    fBergNum(),
    fKaptonNum(),
    fGassiNum(),
    fLocalIndices(),
    fOwner()
{
/// Root IO constructor

  AliDebug(1,Form("this=%p",this));
}
/*
//_____________________________________________________________________________
AliMpConnection::AliMpConnection() 
  : TObject(),
    fBergNum(-1),
    fKaptonNum(-1),
    fGassiNum(-1),
    fLocalIndices(-1)
    fOwner(0)
{
/// Default constructor

  AliDebug(1,Form("this=%p",this));
}
*/
//_____________________________________________________________________________
AliMpConnection::~AliMpConnection() 
{
/// Destructor  

  AliDebug(1,Form("this=%p", this));
}

//_____________________________________________________________________________
Int_t  AliMpConnection::GetLocalIx() const
{
/// Return local ix

  return AliMp::PairFirst(fLocalIndices);
}  

Int_t  AliMpConnection::GetLocalIy() const
{
/// Return local iy

  return AliMp::PairSecond(fLocalIndices);
}  

