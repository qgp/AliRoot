#ifndef ALIANALYSISDATACONTAINER_H
#define ALIANALYSISDATACONTAINER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
// Author: Andrei Gheata, 31/05/2006

//==============================================================================
//   AliAnalysysDataContainer - Container of data of arbitrary type deriving
//      from TObject used for analysis. A container must be connected to the 
//      output data slot of a single analysis task (producer) , but also as 
//      input slot for possibly several other tasks (consumers). The connected 
//      slots must enforce the same data type as the container (or a derived type).
//      A container becomes the owner of the contained data once this was produced.
//==============================================================================

#ifndef ROOT_TNamed
#include "TNamed.h"
#endif

#ifndef ROOT_TString
#include "TString.h"
#endif

class TClass;
class TObjArray;
class TCollection;
class AliAnalysisTask;
class AliAnalysisDataWrapper;
class AliESD;

class AliAnalysisDataContainer : public TNamed {

public:
enum ENotifyMessage {
   kDeleteData,
   kSaveData,
   kFileChange
};   
enum EAnalysisContainerFlags {
   kContEvtByEvt
};     
   AliAnalysisDataContainer();
   AliAnalysisDataContainer(const AliAnalysisDataContainer &cont);
   AliAnalysisDataContainer(const char *name, TClass *type);
   virtual ~AliAnalysisDataContainer();

   // Assignment
   AliAnalysisDataContainer &operator=(const AliAnalysisDataContainer &cont);
   // Getters
   TObject                  *GetData() const      {return fData;}
   const char               *GetFileName() const  {return fFileName.Data();}
   TClass                   *GetType() const;
   AliAnalysisTask          *GetProducer() const  {return fProducer;}
   TObjArray                *GetConsumers() const {return fConsumers;}
   virtual void              GetEntry(Long64_t ientry);
   // Setters
   void                      ResetDataReady()     {fDataReady = kFALSE;}
   virtual Bool_t            SetData(TObject *data, Option_t *option="");
   void                      SetDataOwned(Bool_t flag) {fOwnedData = flag;}
   void                      SetEventByEvent(Bool_t flag=kTRUE) {TObject::SetBit(kContEvtByEvt,flag);}
   void                      SetFileName(const char *filename) {fFileName = filename;}
   void                      SetProducer(AliAnalysisTask *prod, Int_t islot);
   void                      AddConsumer(AliAnalysisTask *cons, Int_t islot);
   void                      DeleteData();
   // Wrapping
   AliAnalysisDataWrapper   *ExportData() const;
   void                      ImportData(AliAnalysisDataWrapper *pack);
   // Container status checking
   Bool_t                    IsDataReady() const  {return fDataReady;}
   Bool_t                    IsEventByEvent() const {return TObject::TestBit(kContEvtByEvt);}
   Bool_t                    IsOwnedData() const  {return fOwnedData;}
   Bool_t                    ClientsExecuted() const;
   Bool_t                    HasConsumers() const {return (fConsumers != 0);}
   Bool_t                    HasProducer() const  {return (fProducer != 0);}
   // Container merging
   virtual Long64_t          Merge(TCollection *list);
   // Send a notify signal to the container
   virtual void              NotifyChange(ENotifyMessage /*type*/) {;}
   // Print connected tasks/status
   void                      PrintContainer(Option_t *option="all", Int_t indent=0) const;

private:
   void                      SetType(TClass *type) {fType = type;}   

protected:
   Bool_t                    fDataReady;  // Flag that data is ready
   Bool_t                    fOwnedData;  // Flag data ownership
   TString                   fFileName;   // File storing the data
   TObject                  *fData;       // Contained data
   TClass                   *fType;       //! Type of contained data
   AliAnalysisTask          *fProducer;   // Analysis task to which the slot belongs
   TObjArray                *fConsumers;  // List of consumers of the data
   
   ClassDef(AliAnalysisDataContainer,1)  // Class describing a data container for analysis
};

//==============================================================================
//   AliAnalysysDataWrapper - A basic wrapper for exchanging via the network
// the data held by AliAnalysisDataContainer between the master and the client
// in PROOF case. 
//==============================================================================

class AliAnalysisDataWrapper : public TNamed {

public:
   AliAnalysisDataWrapper() : TNamed(), fData(NULL) {}
   AliAnalysisDataWrapper(TObject *data) : TNamed(), fData(data) {}
   AliAnalysisDataWrapper(const AliAnalysisDataWrapper &other) 
                        : TNamed(other), fData(other.fData) {}
   virtual ~AliAnalysisDataWrapper() {}
   
   // Assignment
   AliAnalysisDataWrapper &operator=(const AliAnalysisDataWrapper &other);

   TObject                  *Data() const {return fData;}
   // Merging
   virtual Long64_t          Merge(TCollection *list);

protected:
   TObject                  *fData;       // Wrapped data

   ClassDef(AliAnalysisDataWrapper, 1) // Data wrapper class for exchange via the net
};

#endif
