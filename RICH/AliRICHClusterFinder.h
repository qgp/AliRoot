#ifndef AliRICHClusterFinder_h
#define AliRICHClusterFinder_h

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include "TTask.h"
#include "TArrayD.h"
#include "TArrayI.h"

#include "AliRICH.h"
class AliHitMap;

class AliRICHClusterFinder : public TTask
{
public:    
           AliRICHClusterFinder(AliRICH *pRICH);
  virtual ~AliRICHClusterFinder()                                          {;}
  
  AliRICH *Rich() {return fRICH;}                                             //Pointer to RICH  
  void     Exec();                                                            //Loop on events and chambers  
  void     FindClusters(Int_t iChamber);                                      //Find all clusters for a given chamber
  void     FindClusterContribs(AliRICHcluster *pCluster);                     //Find CombiPid for the current cluster
  void     FormRawCluster(Int_t i, Int_t j);                                  //form a raw cluster
  void     FindLocalMaxima();                                                 //Find local maxima in a cluster
  void     ResolveCluster();                                                  //Try to resolve a cluster with maxima > 2
  void     FitCoG();                                                          //Evaluate the CoG as the best 
  void     WriteRawCluster();                                                 //write in the list of cluster  
  void     WriteResolvedCluster();                                            //write in the list of cluster  
  AliRICHcluster *GetRawCluster() {return &fRawCluster;}                      //Return pointer to the current raw cluster
  Bool_t   GetDebug()            const {return fRICH->GetDebug();}            //is debug printout needed?
protected:
  AliRICH                *fRICH;                         //Pointer to RICH
  AliHitMap              *fHitMap;                       //Hit Map with digit positions
  AliRICHcluster         fRawCluster;                    //Current raw cluster before deconvolution
  AliRICHcluster         fResolvedCluster;               //Current cluster after deconvolution
  Int_t                  fNlocals;                       // number of local maxima
  TArrayD                fLocalX,fLocalY;                // list of locals X,Y
  TArrayD                fLocalQ;                        // list of locals charge Q
  TArrayI                fLocalC;                        // list of locals CombiPid
  ClassDef(AliRICHClusterFinder,0) //Finds raw clusters, trasfers them to resolved clusters through declustering.
};
#endif
