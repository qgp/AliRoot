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

/* 
$Log$
Revision 1.6  2002/10/22 14:26:28  alibrary
Introducing Riostream.h

Revision 1.5  2002/10/14 14:57:42  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.3.6.1  2002/07/24 11:07:40  alibrary
Updating VirtualMC

Revision 1.4  2002/07/24 09:38:28  vicinanz
Fixed (ininfluential) bug on TestHit method

Revision 1.3  2001/12/19 09:33:32  hristov
Index corrected

Revision 1.2  2001/11/22 11:30:30  hristov
Correct log field

Revision 1.1  2001/11/22 11:22:51  hristov
Updated version of TOF digitization, N^2 problem solved (J.Chudoba)

*/

////////////////////////////////////////////////////////////////////////
//
// AliTOFHitMap class
//
// hitmap enables fast check if the pad was already hit
// The index of a AliTOFSDigit is saved in the each hitmap "cell"
// (there is an offset +1, because the index can be zero and 
// zero means empty cell. 
// In TOF, number of strips varies according plate type, the highest
// number is in plate C. For all plates is used this number, so
// the size of the hitmap is a little bit greater than necessary, but
// it simplifies the access algorithm. 
// 
//
// Author: Jiri Chudoba (CERN), based on AliMUONHitMap
//
////////////////////////////////////////////////////////////////////////

#include <Riostream.h>
#include <TMath.h>

#include "AliTOFHitMap.h"
#include "AliTOFSDigit.h"
#include "AliTOFConstants.h"


#include <TClonesArray.h>

ClassImp(AliTOFHitMap)

AliTOFHitMap::AliTOFHitMap()
{
//
// Default ctor
//
  fHitMap = 0;
  fSDigits = 0;
}

////////////////////////////////////////////////////////////////////////
AliTOFHitMap::AliTOFHitMap(TClonesArray *dig)
{
//
// ctor
//

// of course, these constants must not be hardwired
// change later

  fNSector = AliTOFConstants::fgkNSectors;
  fNplate = AliTOFConstants::fgkNPlates;
  fNstrip = AliTOFConstants::fgkNStripC;
  fNpx  = AliTOFConstants::fgkNpadX;
  fNpy  = AliTOFConstants::fgkNpadZ;
  fMaxIndex=fNSector*fNplate*fNstrip*fNpx*fNpy;
  fHitMap = new Int_t[fMaxIndex];
  fSDigits =  dig;
  Clear();
}

////////////////////////////////////////////////////////////////////////
AliTOFHitMap::AliTOFHitMap(const AliTOFHitMap & hitMap)
{
//
// Dummy copy constructor
//
    ;
}

 
////////////////////////////////////////////////////////////////////////
AliTOFHitMap::~AliTOFHitMap()
{
//
// Destructor
//
    if (fHitMap) delete[] fHitMap;
}

////////////////////////////////////////////////////////////////////////
void AliTOFHitMap::Clear(const char *)
{
//
// Clear hitmap
//
    memset(fHitMap,0,sizeof(int)*fMaxIndex);
}

////////////////////////////////////////////////////////////////////////
Int_t AliTOFHitMap::CheckedIndex(Int_t *vol) const
{
//
// Return checked indices for vol
//
  Int_t index=
    (vol[0]-1)*fNplate*fNstrip*fNpx*fNpy+             // sector
    (vol[1]-1)*fNstrip*fNpx*fNpy+                     // plate
    (vol[2]-1)*fNpx*fNpy+                             // strip
    (vol[3]-1)*fNpy+                                  // padx
    (vol[4]-1);                                        // pady (=padz)

    if (index >= fMaxIndex) {
      Error("AliTOFHitMap","CheckedIndex - input outside bounds");
	return -1;
    } else {
	return index;
    }
}

////////////////////////////////////////////////////////////////////////
void  AliTOFHitMap::SetHit(Int_t *vol, Int_t idigit)
{
//
// Assign digit to pad vol
//

// 0 means empty pad, we need to shift indeces by 1
    fHitMap[CheckedIndex(vol)]=idigit+1;
}

////////////////////////////////////////////////////////////////////////
void  AliTOFHitMap::SetHit(Int_t *vol)
{
//
// Assign last digit to pad vol 
//

// 0 means empty pad, we need to shift indeces by 1
    fHitMap[CheckedIndex(vol)]=fSDigits->GetLast()+1;
}

////////////////////////////////////////////////////////////////////////
Int_t AliTOFHitMap::GetHitIndex(Int_t *vol) const
{
//
// Get contents of pad vol
//

// 0 means empty pad, we need to shift indeces by 1
    return fHitMap[CheckedIndex(vol)]-1;
}

////////////////////////////////////////////////////////////////////////
TObject* AliTOFHitMap::GetHit(Int_t *vol) const
{
//
// Get pointer to object at vol
// return 0 if vol out of bounds
    Int_t index=GetHitIndex(vol);
    return (index <0) ? 0 : fSDigits->UncheckedAt(index);
}

////////////////////////////////////////////////////////////////////////
FlagType AliTOFHitMap::TestHit(Int_t *vol) const
{
//
// Check if hit cell is empty, used or unused
//
    Int_t inf=fHitMap[CheckedIndex(vol)];
    if (inf > 0) {
	return kUsed;
    } else if (inf == 0) {
	return kEmpty;
    } else {
	return kUnused;
    }
}

////////////////////////////////////////////////////////////////////////
AliTOFHitMap & AliTOFHitMap::operator = (const AliTOFHitMap & rhs) 
{
// Dummy assignment operator
    return *this;
}





