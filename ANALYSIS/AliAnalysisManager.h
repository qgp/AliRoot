#ifndef ALIANALYSISMANAGER_H
#define ALIANALYSISMANAGER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
// Author: Andrei Gheata, 31/05/2006

//==============================================================================
//   AliAnalysysManager - Manager analysis class. Allows creation of several
// analysis tasks and data containers storing their input/output. Allows 
// connecting/chaining tasks via shared data containers. Serializes the current
// event for all tasks depending only on initial input data.
//==============================================================================

#ifndef ROOT_TNamed
#include <TNamed.h>
#endif

class TClass;
class TTree;
class TFile;
class TStopwatch;
class AliAnalysisSelector;
class AliAnalysisDataContainer;
class AliAnalysisTask;
class AliVEventHandler;
class AliVEventPool;
class AliAnalysisGrid;


class AliAnalysisManager : public TNamed {

public:

enum EAliAnalysisContType {
   kExchangeContainer  = 0,
   kInputContainer   = 1,
   kOutputContainer  = 2
};   

enum EAliAnalysisExecMode {
   kLocalAnalysis    = 0,
   kProofAnalysis    = 1,
   kGridAnalysis     = 2,
   kMixingAnalysis   = 3
};

enum EAliAnalysisFlags {
   kEventLoop        = BIT(14),
   kDisableBranches  = BIT(15),
   kUseDataSet       = BIT(16),
   kSaveCanvases     = BIT(17),
   kExternalLoop     = BIT(18),
   kSkipTerminate    = BIT(19)
};   

   AliAnalysisManager(const char *name = "mgr", const char *title="");
   virtual            ~AliAnalysisManager();

   AliAnalysisManager(const AliAnalysisManager& other);
   AliAnalysisManager& operator=(const AliAnalysisManager& other);
   
   // Event loop control
   virtual Int_t       GetEntry(Long64_t entry, Int_t getall = 0);
   virtual Bool_t      Init(TTree *tree);   
   virtual Bool_t      Notify();
   virtual Bool_t      ProcessCut(Long64_t entry) {return Process(entry);}
   virtual Bool_t      Process(Long64_t entry);
   TFile              *OpenProofFile(AliAnalysisDataContainer *cont, const char *option);
   static TFile       *OpenFile(AliAnalysisDataContainer *cont, const char *option, Bool_t ignoreProof=kFALSE);
   void                PackOutput(TList *target);
   void                RegisterExtraFile(const char *fname);
   void                StartAnalysis(const char *type="local", TTree *tree=0, Long64_t nentries=1234567890, Long64_t firstentry=0);
   void                StartAnalysis(const char *type, const char *dataset, Long64_t nentries=1234567890, Long64_t firstentry=0);
   virtual void        SlaveBegin(TTree *tree);
   virtual void        Terminate();
   void                UnpackOutput(TList *source);

   // Getters/Setters
   static AliAnalysisManager *GetAnalysisManager() {return fgAnalysisManager;}
   EAliAnalysisExecMode 
                       GetAnalysisType() const    {return fMode;}
   void                GetAnalysisTypeString(TString &type) const;                    
   static const char  *GetCommonFileName()        {return fgCommonFileName.Data();}
   AliAnalysisDataContainer *
                       GetCommonInputContainer()  {return fCommonInput;}
   AliAnalysisDataContainer *
                       GetCommonOutputContainer() {return fCommonOutput;}
   TObjArray          *GetContainers() const      {return fContainers;}
   Long64_t            GetCurrentEntry() const    {return fCurrentEntry;}
   UInt_t              GetDebugLevel() const      {return fDebug;}
   TString             GetExtraFiles() const      {return fExtraFiles;}
   AliVEventPool*      GetEventPool()             {return fEventPool;}
   Bool_t              GetFileFromWrapper(const char *filename, TList *source);
   AliAnalysisGrid*    GetGridHandler()           {return fGridHandler;}
   TObjArray          *GetInputs() const          {return fInputs;}
   AliVEventHandler*   GetInputEventHandler()     {return fInputEventHandler;}
   AliVEventHandler*   GetMCtruthEventHandler()   {return fMCtruthEventHandler;}
   AliVEventHandler*   GetOutputEventHandler()    {return fOutputEventHandler;}
   TObjArray          *GetOutputs() const         {return fOutputs;}
   TObjArray          *GetTasks() const           {return fTasks;}
   TObjArray          *GetTopTasks() const        {return fTopTasks;}
   TTree              *GetTree() const            {return fTree;}
   TObjArray          *GetZombieTasks() const     {return fZombies;}
   Bool_t              IsUsingDataSet() const     {return TObject::TestBit(kUseDataSet);}
   void                SetAnalysisType(EAliAnalysisExecMode mode) {fMode = mode;}
   void                SetCurrentEntry(Long64_t entry)            {fCurrentEntry = entry;}
   void                SetCollectSysInfoEach(Int_t nevents=0)     {fNSysInfo = nevents;}
   static void         SetCommonFileName(const char *name)        {fgCommonFileName = name;}
   void                SetDebugLevel(UInt_t level)                {fDebug = level;}
   void                SetDisableBranches(Bool_t disable=kTRUE)   {TObject::SetBit(kDisableBranches,disable);}
   void                SetExternalLoop(Bool_t flag)               {TObject::SetBit(kExternalLoop,flag);}
   void                SetEventPool(AliVEventPool* epool)         {fEventPool = epool;}
   void                SetGridHandler(AliAnalysisGrid *handler)   {fGridHandler = handler;}
   void                SetInputEventHandler(AliVEventHandler*  handler);
   void                SetMCtruthEventHandler(AliVEventHandler* handler) {fMCtruthEventHandler = handler;}
   void                SetNSysInfo(Long64_t nevents)              {fNSysInfo = nevents;}
   void                SetOutputEventHandler(AliVEventHandler*  handler);
   void                SetSelector(AliAnalysisSelector *sel)      {fSelector = sel;}
   void                SetSaveCanvases(Bool_t flag=kTRUE)         {TObject::SetBit(kSaveCanvases,flag);}
   void                SetSkipTerminate(Bool_t flag)              {TObject::SetBit(kSkipTerminate,flag);}
   void                SetSpecialOutputLocation(const char *loc)  {fSpecialOutputLocation = loc;}

   // Container handling
   AliAnalysisDataContainer *CreateContainer(const char *name, TClass *datatype, 
                       EAliAnalysisContType type     = kExchangeContainer, 
                       const char          *filename = NULL);
   
   // Including tasks and getting them
   void                 AddTask(AliAnalysisTask *task);
   AliAnalysisTask     *GetTask(const char *name) const;
   
   // Connecting data containers to task inputs/outputs
   Bool_t               ConnectInput(AliAnalysisTask *task, Int_t islot,
                                     AliAnalysisDataContainer *cont);
   Bool_t               ConnectOutput(AliAnalysisTask *task, Int_t islot,
                                     AliAnalysisDataContainer *cont);
   // Garbage collection
   void                 CleanContainers();
   
   // Analysis initialization and execution, status
   Bool_t               InitAnalysis();
   Bool_t               IsInitialized() const {return fInitOK;}
   Bool_t               IsExternalLoop() const {return TObject::TestBit(kExternalLoop);}
   Bool_t               IsEventLoop() const {return TObject::TestBit(kEventLoop);}
   Bool_t               IsSkipTerminate() const {return TObject::TestBit(kSkipTerminate);}
   void                 ResetAnalysis();
   void                 ExecAnalysis(Option_t *option="");
   void                 FinishAnalysis();
   void                 PrintStatus(Option_t *option="all") const;
   static void          ProgressBar(const char *opname, Long64_t current, Long64_t size, TStopwatch *watch=0, Bool_t last=kFALSE, Bool_t refresh=kFALSE);
   Bool_t               ValidateOutputFiles() const;

protected:
   void                 ImportWrappers(TList *source);
   void                 SetEventLoop(Bool_t flag=kTRUE) {TObject::SetBit(kEventLoop,flag);}

private:
   TTree                  *fTree;                //! Input tree in case of TSelector model
   AliVEventHandler       *fInputEventHandler;   //  Optional common input  event handler
   AliVEventHandler       *fOutputEventHandler;  //  Optional common output event handler
   AliVEventHandler       *fMCtruthEventHandler; //  Optional common MC Truth event handler
   AliVEventPool          *fEventPool;           //  Event pool for mixing analysis
   Long64_t                fCurrentEntry;        //! Current processed entry in the tree
   Long64_t                fNSysInfo;            // Event frequency for collecting system information
   EAliAnalysisExecMode    fMode;                // Execution mode
   Bool_t                  fInitOK;              // Initialisation done
   UInt_t                  fDebug;               // Debug level
   TString                 fSpecialOutputLocation; // URL/path where the special outputs will be copied
   TObjArray              *fTasks;               // List of analysis tasks
   TObjArray              *fTopTasks;            // List of top tasks
   TObjArray              *fZombies;             // List of zombie tasks
   TObjArray              *fContainers;          // List of all containers
   TObjArray              *fInputs;              // List of containers with input data
   TObjArray              *fOutputs;             // List of containers with results
   AliAnalysisDataContainer *fCommonInput;       // Common input container
   AliAnalysisDataContainer *fCommonOutput;      // Common output container
   AliAnalysisSelector    *fSelector;            //! Current selector
   AliAnalysisGrid        *fGridHandler;         //! Grid handler plugin
   TString                 fExtraFiles;          // List of extra files to be merged

   static TString          fgCommonFileName;     //! Common output file name (not streamed)
   static AliAnalysisManager *fgAnalysisManager; //! static pointer to object instance
   ClassDef(AliAnalysisManager,5)  // Analysis manager class
};   
#endif
