#ifndef ALICONFIG_H
#define ALICONFIG_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include <TNamed.h>
class TDatabasePDG;
class TFolder;
class TString;
class TVirtualMC;

class AliConfig;
class AliDetector;
class AliGenerator;
class AliModule;
class AliTasks;

class AliConfig : public TNamed {
  
public:
  
  AliConfig();
  
  virtual ~ AliConfig (); 

  void       Add(TDatabasePDG *pdg);
  void       Add(char *list);
  
  void       Add(AliGenerator *generator,const char* eventfolder = fgkDefaultEventFolderName);
  void       Add (TVirtualMC *mc,const char* eventfolder = fgkDefaultEventFolderName);
  void       Add (AliModule *module,const char* eventfolder = fgkDefaultEventFolderName);
  void       Add (AliDetector *detector,const char* eventfolder = fgkDefaultEventFolderName);

  Int_t      AddDetector(const char* evntfoldername,const char *name, const char* title);
  Int_t      AddDetector(TFolder* evntfolder,const char *name, const char* title);
  
  Int_t      CreateDetectorFolders(const char* evntfoldername,const char *name, const char* title);//Used by AliRunGetter
  Int_t      CreateDetectorFolders(TFolder* evntfolder,const char *name, const char* title);//Used by AliRunGetter
  Int_t      CreateDetectorTasks(const char *name, const char* title);
  
  static     AliConfig* Instance();
  
  TFolder*              BuildEventFolder(const char* name,const char* tilte);
  
  TFolder*              GetTopFolder(){return fTopFolder;}
  TFolder*              GetTaskFolder(){return fTaskFolder;}
  TFolder*              GetConstFolder(){return fConstFolder;}

  static const TString& GetModulesFolderName(){return fgkModuleFolderName;}
  static const TString& GetDefaultEventFolderName()
    {return fgkDefaultEventFolderName;}
  TString               GetQATaskName() const; //returns path to QA tasks
  TString               GetDigitizerTaskName () const;
  TString               GetSDigitizerTaskName () const;
  TString               GetReconstructionerTaskName () const;
  TString               GetTrackerTaskName () const;
  TString               GetPIDTaskName () const;
  
  
  const TString&        GetQAFolderName() const; //returns path to folder with QA output
  
  const TString&        GetDataFolderName() const;//returns name of data folder
  
  static const TString  fgkTopFolderName; //name of top AliRoot folder
 
  static const TString  fgkDefaultEventFolderName; //name of event folder
  static const TString  fgkTasksFolderName;        //name of task folder
  static const TString  fgkConstantsFolderName;    //name of constants folder
  
  static const TString  fgkDataFolderName;         //name of data folde
  static const TString  fgkConditionsFolderName;   //name of conditions folder
  static const TString  fgkConfigurationFolderName;//name of configuration foolder
  static const TString  fgkHeaderFolderName;       //name of header folder
  
  static const TString  fgkDigitizerTaskName;      //name of digitizer task
  static const TString  fgkSDigitizerTaskName;     //name of sdigitizer task
  static const TString  fgkQATaskName;             //name of Q-A task
  static const TString  fgkReconstructionerTaskName;//name of reconstructioner
                                                    //task
  static const TString  fgkTrackerTaskName;        //name of tracker task 
  static const TString  fgkPIDTaskName;            //name of PID task
  
  static const TString  fgkCalibrationFolderName;  //name of calibration folder
  static const TString  fgkAligmentFolderName;     //name of alignment folder
  static const TString  fgkQAFolderName;           //name of QA folder
  
  static const TString  fgkFieldFolderName;        //name of magn.field folder
  static const TString  fgkGeneratorsFolderName;   //name of generator folder
  static const TString  fgkVirtualMCFolderName;    //name of virtual MC folder

private:
  AliConfig(const char * name, const char * title );
  AliConfig(const AliConfig&);
  AliConfig& operator=(const AliConfig&);

  void          AddInFolder (const char * dir, TObject *obj);
  Int_t         AddSubTask(const char *taskname, const char* name, const char* title);
  Int_t         AddSubFolder(TFolder* topfolder, const char* infoler, //helper method
                     const char* newfoldname, const char* newfoldtitle);
  TObject*      FindInFolder (const char *dir, const char *name);
  
  // folders
  TFolder*              fTopFolder;    //pointer to top folder
  TFolder*              fTaskFolder;   //pointer to task folder
  TFolder*              fConstFolder;  //pointer to constants folder

  static const TString  fgkPDGFolderName; //name of PDG folder
  static const TString  fgkGeneratorFolderName; //name of generator name
  static const TString  fgkMCFolderName;        //name of MC folder
  static const TString  fgkModuleFolderName;    //name of module folder
  
  TString              *fDetectorTask;//!array with names for detector tasks
  TString              *fDetectorFolder;//!array with names for detector folders (where detector is going to be put)
  
  static AliConfig*     fgInstance; //pointer to the AliConfig instance
  
  
  ClassDef(AliConfig,2) //Configuration class for AliRun
};				// end class AliConfig

#endif
