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


#include "AliTPCclusterMI.h"

ClassImp(AliTPCclusterMI)
ClassImp(AliTPCclusterLMI)

Bool_t AliTPCclusterMI::IsSortable() const
{
  //
  //
  return kTRUE;

}

Int_t AliTPCclusterMI::Compare(const TObject* obj) const
{
  //
  // compare according y
  AliTPCclusterMI * o2 = (AliTPCclusterMI*)obj;
  return (o2->GetY()>fY)? -1:1; 
}
