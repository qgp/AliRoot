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

//-------------------------------------------------------------------------
//               Implementation of the AliESDfriend class
//  This class contains some additional to the ESD information like
//  the clusters associated to tracks.
//      Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch
//-------------------------------------------------------------------------

#include "AliESDfriend.h"
#include "AliESDVZEROfriend.h"
#include "AliESDTZEROfriend.h"
#include "AliESDADfriend.h"

ClassImp(AliESDfriend)

AliESDfriend::AliESDfriend(): TObject(), fTracks("AliESDfriendTrack",1),
  fESDVZEROfriend(NULL),
  fESDTZEROfriend(NULL),
  fESDADfriend(NULL),
  fNclustersTPC(),
  fNclustersTPCused()
{
 //
 // Default constructor
 //
  for (Int_t i=0;i<72;i++)
  {
    fNclustersTPC[i]=0;
    fNclustersTPCused[i]=0;
  }
}

AliESDfriend::AliESDfriend(const AliESDfriend &f) :
  TObject(f),
  fTracks(f.fTracks),
  fESDVZEROfriend(f.fESDVZEROfriend ? new AliESDVZEROfriend(*f.fESDVZEROfriend) : NULL),
  fESDTZEROfriend(f.fESDTZEROfriend ? new AliESDTZEROfriend(*f.fESDTZEROfriend) : NULL),
  fESDADfriend(f.fESDADfriend ? new AliESDADfriend(*f.fESDADfriend) : NULL),
  fNclustersTPC(),
  fNclustersTPCused()
{
 //
 // Copy constructor
 //
 memcpy(fNclustersTPC,f.fNclustersTPC,sizeof(fNclustersTPC));
 memcpy(fNclustersTPCused,f.fNclustersTPCused,sizeof(fNclustersTPCused));

}

AliESDfriend& AliESDfriend::operator=(const AliESDfriend& esd)
{
    
    // Assignment operator
    if(&esd == this) return *this;
    TObject::operator=(esd);
    fTracks = esd.fTracks;

    delete fESDVZEROfriend;
    fESDVZEROfriend = new AliESDVZEROfriend(*esd.fESDVZEROfriend);

    delete fESDTZEROfriend;
    fESDTZEROfriend = new AliESDTZEROfriend(*esd.fESDTZEROfriend);
    
    delete fESDADfriend;
    fESDADfriend = new AliESDADfriend(*esd.fESDADfriend);
 
    memcpy(fNclustersTPC,esd.fNclustersTPC,sizeof(fNclustersTPC));
    memcpy(fNclustersTPCused,esd.fNclustersTPCused,sizeof(fNclustersTPCused));
 
 
    return *this;
}



AliESDfriend::~AliESDfriend() {
  //
  // Destructor
  //
  fTracks.Delete();
  delete fESDVZEROfriend;
  delete fESDTZEROfriend;
  delete fESDADfriend;
}


void AliESDfriend::Reset()
{
  //
  // Reset friend information
  //
  fTracks.Delete();
  for (Int_t i=0;i<72;i++)
  {
    fNclustersTPC[i]=0;
    fNclustersTPCused[i]=0;
  }
  delete fESDVZEROfriend; fESDVZEROfriend=0;
  delete fESDTZEROfriend; fESDTZEROfriend=0;
  delete fESDADfriend; fESDADfriend=0;
}  


void AliESDfriend::SetVZEROfriend(AliESDVZEROfriend * obj)
{
  //
  // Set the VZERO friend data object
  // (complete raw data)
  if (!fESDVZEROfriend) fESDVZEROfriend = new AliESDVZEROfriend();
  if (obj) *fESDVZEROfriend = *obj;
}
void AliESDfriend::SetTZEROfriend(AliESDTZEROfriend * obj)
{
  //
  // Set the TZERO friend data object
  // (complete raw data)
  if (!fESDTZEROfriend) fESDTZEROfriend = new AliESDTZEROfriend();
  if (obj) *fESDTZEROfriend = *obj;
}
void AliESDfriend::SetADfriend(AliESDADfriend * obj)
{
  //
  // Set the AD friend data object
  // (complete raw data)
  if (!fESDADfriend) fESDADfriend = new AliESDADfriend();
  if (obj) *fESDADfriend = *obj;
}
