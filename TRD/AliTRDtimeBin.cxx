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

/* $Id$ */
             
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
