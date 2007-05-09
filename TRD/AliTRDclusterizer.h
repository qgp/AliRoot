#ifndef ALITRDCLUSTERIZER_H
#define ALITRDCLUSTERIZER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  TRD cluster finder base class                                         //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include <TNamed.h>

class TFile;
class TTree;
class TObjArray;

class AliRunLoader;

class AliTRD;
class AliTRDcluster;

class AliTRDclusterizer : public TNamed {

 public:

  AliTRDclusterizer();
  AliTRDclusterizer(const Text_t* name, const Text_t* title);
  AliTRDclusterizer(const AliTRDclusterizer &c);
  virtual         ~AliTRDclusterizer();
  AliTRDclusterizer &operator=(const AliTRDclusterizer &c);

  virtual void     Copy(TObject &c) const;
  virtual Bool_t   Open(const Char_t *name, Int_t nEvent = 0);
  virtual Bool_t   OpenInput(Int_t nEvent = 0);
  virtual Bool_t   OpenOutput();
  virtual Bool_t   OpenOutput(TTree *clusterTree);
  virtual Bool_t   MakeClusters() = 0;
  virtual Bool_t   WriteClusters(Int_t det);
          void     ResetRecPoints();

          TObjArray     *RecPoints();
  virtual AliTRDcluster *AddCluster(Double_t *pos, Int_t timebin, Int_t det
                                  , Double_t amp, Int_t *tracks
			          , Double_t *sig, Int_t iType, Float_t center = 0);

 protected:

          Double_t CalcXposFromTimebin(Float_t timebin, Int_t idet, Int_t col, Int_t row);
       
          AliRunLoader    *fRunLoader;     //! Run Loader
          TTree           *fClusterTree;   //! Tree with the cluster
          TObjArray       *fRecPoints;     //! Array of clusters

  ClassDef(AliTRDclusterizer,4)            //  TRD-Cluster manager base class

};

#endif
