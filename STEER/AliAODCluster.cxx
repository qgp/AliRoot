/**************************************************************************
 * Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
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

//-------------------------------------------------------------------------
//     AOD cluster base class
//     Author: Markus Oldenburg, CERN
//-------------------------------------------------------------------------

#include "AliAODCluster.h"

ClassImp(AliAODCluster)

//______________________________________________________________________________
AliAODCluster::AliAODCluster() : 
  AliVirtualParticle(),
  fEnergy(0),
  fChi2(-999.),
  fID(-999),
  fLabel(-999),
  fCovMatrix(NULL),
  fProdVertex(0x0),
  fPrimTrack(NULL),
  fType(kUndef)
{
  // default constructor

  SetPosition((Float_t*)NULL);
  SetPID((Float_t*)NULL);
}

//______________________________________________________________________________
AliAODCluster::AliAODCluster(Int_t id,
			     Int_t label, 
			     Double_t energy,
			     Double_t x[3],
			     Double_t covMatrix[10],
			     Double_t pid[9],
			     AliAODVertex *prodVertex,
			     AliAODTrack *primTrack,
			     Char_t ttype) :
  AliVirtualParticle(),
  fEnergy(energy),
  fChi2(-999.),
  fID(id),
  fLabel(label),
  fCovMatrix(NULL),
  fProdVertex(prodVertex),
  fPrimTrack(primTrack),
  fType(ttype)
{
  // constructor
 
  SetPosition(x);
  if(covMatrix) SetCovMatrix(covMatrix);
  SetPID(pid);

}

//______________________________________________________________________________
AliAODCluster::AliAODCluster(Int_t id,
			     Int_t label, 
			     Float_t energy,
			     Float_t x[3],
			     Float_t covMatrix[10],
			     Float_t pid[9],
			     AliAODVertex *prodVertex,
			     AliAODTrack *primTrack,
			     Char_t ttype) :
  AliVirtualParticle(),
  fEnergy(energy),
  fChi2(-999.),
  fID(id),
  fLabel(label),
  fCovMatrix(NULL),
  fProdVertex(prodVertex),
  fPrimTrack(primTrack),
  fType(ttype)
{
  // constructor
 
  SetPosition(x);
  if(covMatrix) SetCovMatrix(covMatrix);
  SetPID(pid);

}


//______________________________________________________________________________
AliAODCluster::~AliAODCluster() 
{
  // destructor
  delete fCovMatrix;
}


//______________________________________________________________________________
AliAODCluster::AliAODCluster(const AliAODCluster& clus) :
  AliVirtualParticle(clus),
  fEnergy(clus.fEnergy),
  fChi2(clus.fChi2),
  fID(clus.fID),
  fLabel(clus.fLabel),
  fCovMatrix(NULL),
  fProdVertex(clus.fProdVertex),
  fPrimTrack(clus.fPrimTrack),
  fType(clus.fType)
{
  // Copy constructor

  clus.GetPosition(fPosition);
  if(clus.fCovMatrix) fCovMatrix=new AliAODRedCov<4>(*clus.fCovMatrix);
  SetPID(clus.fPID);

}

//______________________________________________________________________________
AliAODCluster& AliAODCluster::operator=(const AliAODCluster& clus)
{
  // Assignment operator
  if(this!=&clus) {

    AliVirtualParticle::operator=(clus);

    clus.GetPosition(fPosition);
    clus.GetPID(fPID);

    fEnergy = clus.fEnergy;
    fChi2 = clus.fChi2;

    fID = clus.fID;
    fLabel = clus.fLabel;    
    
    delete fCovMatrix;
    if(clus.fCovMatrix) fCovMatrix=new AliAODRedCov<4>(*clus.fCovMatrix);
    else fCovMatrix=NULL;
    fProdVertex = clus.fProdVertex;
    fPrimTrack = clus.fPrimTrack;

    fType = clus.fType;
  }

  return *this;
}

//______________________________________________________________________________
template <class T> void AliAODCluster::SetPosition(const T *x) 
{
  // set the position

  if (x) {
      fPosition[0] = x[0];
      fPosition[1] = x[1];
      fPosition[2] = x[2];
  } else {

    fPosition[0] = -999.;
    fPosition[1] = -999.;
    fPosition[2] = -999.;
  }
}

//______________________________________________________________________________
AliAODCluster::AODCluPID_t AliAODCluster::GetMostProbablePID() const 
{
  // Returns the most probable PID array element.
  
  Int_t nPID = 9;
  if (fPID) {
    AODCluPID_t loc = kUnknown;
    Double_t max = 0.;
    Bool_t allTheSame = kTRUE;
    
    for (Int_t iPID = 0; iPID < nPID; iPID++) {
      if (fPID[iPID] >= max) {
	if (fPID[iPID] > max) {
	  allTheSame = kFALSE;
	  max = fPID[iPID];
	  loc = (AODCluPID_t)iPID;
	} else {
	  allTheSame = kTRUE;
	}
      }
    }
    
    return allTheSame ? kUnknown : loc;
  } else {
    return kUnknown;
  }
}

//______________________________________________________________________________
void AliAODCluster::Print(Option_t* /* option */) const
{
  // prints information about AliAODCluster

  printf("Object name: %s   Cluster type: %s\n", GetName(), GetTitle()); 
  printf("    energy = %f\n", E());
  printf("      chi2 = %f\n", Chi2());
  printf(" PID object: %p\n", (void*)PID());
}

