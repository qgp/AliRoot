#ifndef ALISIMULATION_H
#define ALISIMULATION_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//
// class for running generation, simulation and digitization
// Hits, sdigits and digits are created for all detectors by typing:
//   AliSimulation sim;
//   sim.Run();
//

#include <TNamed.h>
#include <TString.h>
#include <TObjArray.h>
#include "AliQA.h"

class AliCDBId;
class AliCDBParam;
class AliRunLoader;
class AliQADataMakerSteer ; 

class AliSimulation: public TNamed {
public:
  AliSimulation(const char* configFileName = "Config.C",
		const char* name = "AliSimulation", 
		const char* title = "generation, simulation and digitization");
  AliSimulation(const AliSimulation& sim);
  AliSimulation& operator = (const AliSimulation& sim);
  virtual ~AliSimulation();

  static AliSimulation *GetInstance() {return fgInstance;}

  void           SetNumberOfEvents(Int_t nEvents);
  void           SetConfigFile(const char* fileName);
  void           SetGAliceFile(const char* fileName);
  void           SetEventsPerFile(const char* detector, const char* type, 
				  Int_t nEvents);

  void           SetRunGeneration(Bool_t run) {fRunGeneration = run;};
  void           SetRunSimulation(Bool_t run) {fRunSimulation = run;};
  void           SetLoadAlignFromCDB(Bool_t load)  {fLoadAlignFromCDB = load;};
  void           SetLoadAlignData(const char* detectors) 
                   {fLoadAlObjsListOfDets = detectors;};
  void           SetMakeSDigits(const char* detectors) 
                   {fMakeSDigits = detectors;};
  void           MergeWith(const char* fileName, Int_t nSignalPerBkgrd = 0);
  void           EmbedInto(const char* fileName, Int_t nSignalPerBkgrd = 0);
  void           SetUseBkgrdVertex(Bool_t useBkgrdVertex)
                   {fUseBkgrdVertex = useBkgrdVertex;};
  void           SetRegionOfInterest(Bool_t flag) {fRegionOfInterest = flag;};
  void           SetMakeDigits(const char* detectors)
                   {fMakeDigits = detectors;};
  void           SetMakeTrigger(const char* descriptors)
                   {fMakeTrigger = descriptors;};
  void           SetMakeDigitsFromHits(const char* detectors)
                   {fMakeDigitsFromHits = detectors;};
  void           SetWriteRawData(const char* detectors, 
				 const char* fileName = NULL,
				 Bool_t deleteIntermediateFiles = kFALSE)
                   {fWriteRawData = detectors; fRawDataFileName = fileName;
		   fDeleteIntermediateFiles = deleteIntermediateFiles;};
  void           SetAlignObjArray(TObjArray *array)
                   {fAlignObjArray = array;
		   fLoadAlignFromCDB = kFALSE;}

  Bool_t         MisalignGeometry(AliRunLoader *runLoader = NULL);

  void           SetRunNumber(Int_t run);
  void           SetSeed(Int_t seed);
    
  void 		 ProcessEnvironmentVars();
		   
  // CDB storage activation
  void SetDefaultStorage(const char* uri);
  void SetSpecificStorage(const char* calibType, const char* uri);

  virtual Bool_t Run(Int_t nEvents = 0);

  virtual Bool_t RunSimulation(Int_t nEvents = 0);
  virtual Bool_t RunSDigitization(const char* detectors = "ALL");
  virtual Bool_t RunTrigger(const char* descriptors ="" );
  virtual Bool_t WriteTriggerRawData();
  virtual Bool_t RunDigitization(const char* detectors = "ALL",
				 const char* excludeDetectors = "");
  virtual Bool_t RunHitsDigitization(const char* detectors = "ALL");
  virtual Bool_t WriteRawData(const char* detectors = "ALL",
			      const char* fileName = NULL,
			      Bool_t deleteIntermediateFiles = kFALSE);
  virtual Bool_t WriteRawFiles(const char* detectors = "ALL");
  virtual Bool_t ConvertRawFilesToDate(const char* dateFileName = "raw.date");
  virtual Bool_t ConvertDateToRoot(const char* dateFileName = "raw.date",
				   const char* rootFileName = "raw.root");
  virtual Bool_t ConvertRaw2SDigits(const char* rawDirectory, const char* esdFile = "");

  // HLT
  void SetRunHLT(const char* options) {fRunHLT=options;}
  virtual Bool_t RunHLT();

  //Quality Assurance
  Int_t       GetDetIndex(const char * detector);
  const Int_t GetQACycles(const char * detector) { return fQACycles[GetDetIndex(detector)] ; }
  void        SetQACycles(const char * detector, const Int_t cycles) { fQACycles[GetDetIndex(detector)] = cycles ; }
  Bool_t      RunQA() ;
  void        SetQA(const Bool_t val) { fRunQA = val ; } 
  
private:
  void 		 InitCDB();
  void 		 InitRunNumber();
  void 		 SetCDBLock();
  Bool_t         SetRunNumberFromData();
  AliRunLoader*  LoadRun(const char* mode = "UPDATE") const;
  Int_t          GetNSignalPerBkgrd(Int_t nEvents = 0) const;
  Bool_t         IsSelected(TString detName, TString& detectors) const;

  static AliSimulation *fgInstance;    // Static pointer to object

  Bool_t         fRunGeneration;      // generate prim. particles or not
  Bool_t         fRunSimulation;      // simulate detectors (hits) or not
  Bool_t         fLoadAlignFromCDB;   // Load alignment data from CDB and apply it to geometry or not
  TString        fLoadAlObjsListOfDets;   // Load alignment data from CDB for these detectors
  TString        fMakeSDigits;        // create sdigits for these detectors
  TString        fMakeDigits;         // create digits for these detectors
  TString        fMakeTrigger;        // run trigger for these descriptors
  TString        fMakeDigitsFromHits; // create digits from hits for these detectors
  TString        fWriteRawData;       // write raw data for these detectors
  TString        fRawDataFileName;    // file name for the raw data file
  Bool_t         fDeleteIntermediateFiles; // delete intermediate raw data files
  Bool_t         fStopOnError;        // stop or continue on errors

  Int_t          fNEvents;            // number of events
  TString        fConfigFileName;     // name of the config file
  TString        fGAliceFileName;     // name of the galice file
  TObjArray      fEventsPerFile;      // number of events per file for given detectors and data types

  TObjArray*     fBkgrdFileNames;     // names of background files for merging
  TObjArray* 	 fAlignObjArray;      // array with the alignment objects to be applied to the geometry
  Bool_t         fUseBkgrdVertex;     // use vertex from background in case of merging
  Bool_t         fRegionOfInterest;   // digitization in region of interest

  TString 	 fCDBUri;	      //! Uri of the default CDB storage
  TObjArray      fSpecCDBUri;         //! Array with detector specific CDB storages
  Int_t 	 fRun; 		      //! Run number, will be passed to CDB and gAlice!!
  Int_t 	 fSeed;               //! Seed for random number generator 
  Bool_t 	 fInitCDBCalled;      //! flag to check if CDB storages are already initialized
  Bool_t 	 fInitRunNumberCalled;  //! flag to check if run number is already initialized
  Bool_t 	 fSetRunNumberFromDataCalled;  //! flag to check if run number is already loaded from run loader
  
  Bool_t         fEmbeddingFlag;      // Flag for embedding
  
  //QA stuff
  static const Int_t   fgkNDetectors = 15 ;             // number of detectors
  static const char *  fgkDetectorName[fgkNDetectors] ; // names of detectors
  Int_t                fQACycles[fgkNDetectors] ;       // cycle length (# events) over which QA data are accumulated
  Bool_t              fRunQA ;                         // Runs the QA at the end of simulation

  //HLT
  TString        fRunHLT;             // HLT options, HLT is disabled if empty, default='default'

  ClassDef(AliSimulation, 7)  // class for running generation, simulation and digitization
};

#endif
