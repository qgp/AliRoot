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
Revision 1.5.4.1  2002/06/03 09:55:04  hristov
Merged with v3-08-02

Revision 1.7  2002/10/14 14:57:44  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.5.6.2  2002/07/24 10:09:31  alibrary
Updating VirtualMC

Revision 1.6  2002/03/28 14:59:07  cblume
Coding conventions

Revision 1.5  2001/11/06 17:19:41  cblume
Add detailed geometry and simple simulator

Revision 1.4  2001/10/21 18:30:02  hristov
Several pointers were set to zero in the default constructors to avoid memory management problems

Revision 1.3  2000/10/15 23:40:01  cblume
Remove AliTRDconst

Revision 1.2  2000/10/06 16:49:46  cblume
Made Getters const

Revision 1.1.2.2  2000/10/04 16:34:58  cblume
Replace include files by forward declarations

Revision 1.1.2.1  2000/09/22 14:47:52  cblume
Add the tracking code

*/                        
             
//////////////////////////////////////////////////////////////////////
//                                                                  //
//  Hit compression class                                           //
//  Adapted from AliTPCTimeBin by Marian                            //
//                                                                  //
//////////////////////////////////////////////////////////////////////
                   
#include "AliTRDcluster.h" 
#include "AliTRDtimeBin.h" 

ClassImp(AliTRDtimeBin)

//______________________________________________________

  AliTRDtimeBin::AliTRDtimeBin() {
  //default constructor
    fN=0;
    for (UInt_t i=0; i<kMaxClusterPerTimeBin; i++) 
      fClusters[i]=0;
  }
//______________________________________________________

void AliTRDtimeBin::InsertCluster(AliTRDcluster* c, UInt_t index) {

// Insert cluster in TimeBin cluster array.
// Clusters are sorted according to Y coordinate.  

  if (fN==kMaxClusterPerTimeBin) {
    printf("AliTRDtimeBin::InsertCluster(): Too many clusters !\n"); 
    return;
  }
  if (fN==0) {fIndex[0]=index; fClusters[fN++]=c; return;}
  Int_t i=Find(c->GetY());
  memmove(fClusters+i+1 ,fClusters+i,(fN-i)*sizeof(AliTRDcluster*));
  memmove(fIndex   +i+1 ,fIndex   +i,(fN-i)*sizeof(UInt_t)); 
  fIndex[i]=index; fClusters[i]=c; fN++;
}  

//______________________________________________________

Int_t AliTRDtimeBin::Find(Double_t y) const {

// Returns index of the cluster nearest in Y    

  if (y <= fClusters[0]->GetY()) return 0;
  if (y > fClusters[fN-1]->GetY()) return fN;
  Int_t b=0, e=fN-1, m=(b+e)/2;
  for (; b<e; m=(b+e)/2) {
    if (y > fClusters[m]->GetY()) b=m+1;
    else e=m;
  }
  return m;
}    

//______________________________________________________
AliTRDcluster *AliTRDtimeBin::operator[](Int_t i)
{
  //
  // Index operator
  //
 
  return fClusters[i];

}
