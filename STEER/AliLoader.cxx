//
//Class (base) responsible for management of data:
//    - opening and closing proper files
//    - posting data to folders
//    - writing data from folders to proper files
//
//Author: Alice Offline Group http://alisoft.cern.ch
//Responsible: Piotr.Skowronski@cern.ch
#include <AliLoader.h>

//Root includes
#include <TROOT.h>
#include <TFolder.h>
#include <TFile.h>
#include <TTree.h>
#include <TTask.h>
#include <TString.h>
#include <TError.h>

//AliRoot includes
#include <AliRun.h>
#include <AliRunLoader.h>
#include <AliRunDigitizer.h>
#include <AliDigitizer.h>

Bool_t  AliLoader::fgDebug = kFALSE;

const TString AliLoader::fgkDefaultHitsContainerName("TreeH");
const TString AliLoader::fgkDefaultDigitsContainerName = "TreeD";
const TString AliLoader::fgkDefaultSDigitsContainerName = "TreeS";
const TString AliLoader::fgkDefaultRecPointsContainerName = "TreeR";
const TString AliLoader::fgkDefaultTracksContainerName = "TreeT";
const TString AliLoader::fgkLoaderBaseName("Loader");

ClassImp(AliLoader)
//___________________________________________________________________
/////////////////////////////////////////////////////////////////////
//                                                                 //
//  class AliLoader                                                //
//                                                                 //
//  Base class for Loaders.                                        //
//  Loader provides base I/O fascilities for "standard" data.      //
//  Each detector has a laoder data member                         //
//  loader is accessible via folder structure as well              //
//                                                                 //
/////////////////////////////////////////////////////////////////////
 
/*****************************************************************************/ 

AliLoader::AliLoader():
 fDataLoaders(0x0),
 fDetectorName(""),
 fEventFolder(0x0),
 fDataFolder(0x0),
 fDetectorDataFolder(0x0),
 fModuleFolder(0x0),
 fTasksFolder(0x0),
 fQAFolder(0x0)
 {
//default constructor

 }
/******************************************************************/

AliLoader::AliLoader(const Char_t* detname,const Char_t* eventfoldername):
 fDataLoaders(new TObjArray(kNDataTypes)),
 fDetectorName(""),
 fEventFolder(0x0),
 fDataFolder(0x0),
 fDetectorDataFolder(0x0),
 fModuleFolder(0x0),
 fTasksFolder(0x0),
 fQAFolder(0x0)
{
  //ctor
   if (GetDebug()) Info("AliLoader(const Char_t* detname,const Char_t* eventfoldername)",
        "detname = %s eventfoldername = %s",detname,eventfoldername);

   //try to find folder eventfoldername in top alice folder
   //safe because GetTopFolder will abort in case of failure

   fDetectorName = detname;
   fName = fDetectorName+"Loader";
   InitDefaults();

   TObject* fobj = GetTopFolder()->FindObject(eventfoldername);
   fEventFolder = (fobj)?dynamic_cast<TFolder*>(fobj):0x0;//in case FindObject returns NULL dynamic cast cause seg. fault
   SetEventFolder(fEventFolder);
   
 }
/*****************************************************************************/ 

AliLoader::AliLoader(const Char_t * detname,TFolder* eventfolder):
 fDataLoaders(new TObjArray(kNDataTypes)),
 fDetectorName(detname),
 fEventFolder(0x0),
 fDataFolder(0x0),
 fDetectorDataFolder(0x0),
 fModuleFolder(0x0),
 fTasksFolder(0x0),
 fQAFolder(0x0)
{
//constructor
   fDetectorName = detname;
   fName = fDetectorName+"Loader";
   InitDefaults();
   SetEventFolder(eventfolder);
   //fileoption's don't need to initialized because default TString ctor does it correctly
}
/*****************************************************************************/ 

AliLoader::~AliLoader()
{
//detructor
  if (fDataLoaders) fDataLoaders->SetOwner();
  delete fDataLoaders;
}
/*****************************************************************************/ 

void AliLoader::InitDefaults()
{
  // H I T S 
  AliDataLoader* dl;
  dl = new AliDataLoader(fDetectorName + ".Hits.root",fgkDefaultHitsContainerName, "Hits" );
  fDataLoaders->AddAt(dl,kHits);
  
  
  // S U M M A B L E   D I G I T S
  dl = new AliDataLoader(fDetectorName + ".SDigits.root",fgkDefaultSDigitsContainerName, "Summable Digits");
  AliTaskLoader* tl = new AliTaskLoader(fDetectorName + AliConfig::Instance()->GetSDigitizerTaskName(),
                                        dl,AliRunLoader::GetRunSDigitizer());
  dl->SetBaseTaskLoader(tl);
  fDataLoaders->AddAt(dl,kSDigits);

  // D I G I T S  
  dl = new AliDataLoader(fDetectorName + ".Digits.root",fgkDefaultDigitsContainerName, "Digits");
  tl = new AliTaskLoader(fDetectorName + AliConfig::Instance()->GetDigitizerTaskName(),
                                        dl,AliRunLoader::GetRunDigitizer());
  dl->SetBaseTaskLoader(tl);
  fDataLoaders->AddAt(dl,kDigits);
  
  dl = new AliDataLoader(fDetectorName + ".RecPoints.root",fgkDefaultRecPointsContainerName, "Reconstructed Points");
  tl = new AliTaskLoader(fDetectorName + AliConfig::Instance()->GetReconstructionerTaskName(),
                                        dl,AliRunLoader::GetRunReconstructioner());
  dl->SetBaseTaskLoader(tl);
  fDataLoaders->AddAt(dl,kRecPoints);
  
  dl = new AliDataLoader(fDetectorName + ".Tracks.root",fgkDefaultTracksContainerName, "Tracks");
  fDataLoaders->AddAt(dl,kTracks);
  tl = new AliTaskLoader(fDetectorName + AliConfig::Instance()->GetTrackerTaskName(),
                                        dl,AliRunLoader::GetRunTracker());
  dl->SetBaseTaskLoader(tl);
  
  
}
/*****************************************************************************/ 

AliDataLoader* AliLoader::GetDataLoader(const char* name)
{
//returns Data Loader with specified name
  return dynamic_cast<AliDataLoader*>(fDataLoaders->FindObject(name));
}
/*****************************************************************************/ 

Int_t AliLoader::SetEvent()
{
 //basically the same that GetEvent but do not post data to folders
 TIter next(fDataLoaders);
 AliDataLoader* dl;
 while ((dl = (AliDataLoader*)next()))
  {
    dl->SetEvent();
  }
 return 0;
}
/******************************************************************/

Int_t AliLoader::GetEvent()
{
 //changes to proper root  directory and tries to load data from files to folders
 // event number is defined in RunLoader
 // 
 //returns:
 //     0  - in case of no error
 //     1  - event not found
 //     
 
 Int_t retval;   
 TIter next(fDataLoaders);
 AliDataLoader* dl;
 while ((dl = (AliDataLoader*)next()))
  {
    retval = dl->GetEvent();
    if (retval)
     {
       Error("GetEvent","Error occured while GetEvent for %s",dl->GetName());
       return retval;
     }
  }

 return 0;
}

/******************************************************************/

TFolder* AliLoader::GetTopFolder()
{
//returns TOP aliroot folder, just a simple interface to AliConfig (gives shorter notation)
 return AliConfig::Instance()->GetTopFolder();
}

/******************************************************************/

TFolder* AliLoader::GetEventFolder()
{
//get EVENT folder (data that are changing from event to event, even in single run)
  return fEventFolder;
}
/******************************************************************/
TFolder* AliLoader::GetDataFolder()
{
//returns the folder speciofic to given detector e.g. /Folders/Event/Data/
 if (!fDataFolder)
  {
   fDataFolder =  dynamic_cast<TFolder*>(GetEventFolder()->FindObject(AliConfig::fgkDataFolderName));
   
   if (!fDataFolder)
    {
     Fatal("GetDataFolder","Can not find AliRoot data folder. Aborting");
     return 0x0;
    }
  }
  return fDataFolder;
}

/*****************************************************************************/ 

TFolder* AliLoader::GetTasksFolder()
{
//Returns pointer to Folder with Alice Tasks
 if (!fTasksFolder)
  {
   fTasksFolder =  dynamic_cast<TFolder*>(GetTopFolder()->FindObject(AliConfig::fgkTasksFolderName));
   
   if (!fTasksFolder)
    {
     Fatal("GetTasksFolder","Can not find tasks folder. Aborting");
     return 0x0;
    }
  }
  return fTasksFolder;
   
}
/*****************************************************************************/ 

TFolder* AliLoader::GetModulesFolder()
{
 if (!fModuleFolder)
  {
   fModuleFolder =  dynamic_cast<TFolder*>(GetEventFolder()->FindObjectAny(AliConfig::GetModulesFolderName()));
   
   if (!fModuleFolder)
    {
     Fatal("GetModulesFolder","Can not find modules folder. Aborting");
     return 0x0;
    }
  }
 return fModuleFolder;
   
}
/*****************************************************************************/ 

TFolder* AliLoader::GetQAFolder()
{ 
  //returns folder with Quality assurance 
  if (fQAFolder == 0x0)
   {
     TObject *obj = GetEventFolder()->FindObjectAny(AliConfig::Instance()->GetQAFolderName());
     fQAFolder = (obj)?dynamic_cast<TFolder*>(obj):0x0;

     if (fQAFolder == 0x0)
      {
       Fatal("GetQAFolder","Can not find Quality Assurance folder. Aborting");
       return 0x0;
      }
   }
  return fQAFolder;
  
}
/*****************************************************************************/ 
TTask* AliLoader::SDigitizer()
{
//returns SDigitizer task for this detector
  return GetSDigitsDataLoader()->GetBaseTaskLoader()->Task();

}
/*****************************************************************************/ 

AliDigitizer* AliLoader::Digitizer()
{
//returns Digitizer task for this detector
  return dynamic_cast<AliDigitizer*>(GetDigitsDataLoader()->GetBaseTaskLoader()->Task());
}
/*****************************************************************************/ 

TTask* AliLoader::Reconstructioner()
{
//returns Recontructioner (Cluster Finder, Cluster Maker, 
//or whatever you want to call it) task for this detector
  return GetRecPointsDataLoader()->GetBaseTaskLoader()->Task();
}
/*****************************************************************************/ 

TTask* AliLoader::QAtask(const char* name)
{
  TTask* qat = AliRunLoader::GetRunQATask();
  if ( qat == 0x0 ) 
   {
    Error("QAtask","Can not get RunQATask. (Name:%s)",GetName());
    return 0x0;
   }
  
  TString dqatname(fDetectorName + AliConfig::Instance()->GetQATaskName());
  TTask* dqat = dynamic_cast<TTask*>(qat->GetListOfTasks()->FindObject(dqatname));
  
  if ( dqat == 0x0 ) 
   {
    Error("QAtask","Can not find QATask in RunQATask for %s",GetDetectorName().Data());
    return 0x0;
   }
  
  if (strlen(name) == 0) return dqat;
  
  TList* list = dqat->GetListOfTasks();
  
  TIter it(list) ;
  TTask * task = 0 ; 
  while((task = static_cast<TTask *>(it.Next()) ))
   {
    TString taskname(task->GetName()) ;
    if(taskname.BeginsWith(name))
      return task ;
   }
  Error("QAtask","Can not find sub-task with name starting with %s in task %s",name,dqat->GetName());
  return 0x0;   
}

/*****************************************************************************/ 

TTask* AliLoader::Tracker()
{
//returns tracker
  return dynamic_cast<TTask*>(GetTracksDataLoader()->GetBaseTaskLoader()->Task());
}

/*****************************************************************************/ 
/*****************************************************************************/ 

TDirectory* AliLoader::ChangeDir(TFile* file, Int_t eventno)
{
//changes the root directory in "file" to "dirname" which corresponds to event 'eventno'
//in case of success returns the pointer to directory
//else NULL
 
 if (file == 0x0)
  {
    ::Error("AliLoader::ChangeDir","File is null");
    return 0x0;
  }
 if (file->IsOpen() == kFALSE)
  {
    ::Error("AliLoader::ChangeDir","File is not opened");
    return 0x0;
  }

 TString dirname("Event");
 dirname+=eventno;
 if (AliLoader::fgDebug) 
   ::Info("AliLoader::ChangeDir","Changing Dir to %s in file %s.",dirname.Data(),file->GetName());

 Bool_t result;
 
 TDirectory* dir = dynamic_cast<TDirectory*>(file->Get(dirname));

 if (dir == 0x0)
  {
    if (AliLoader::fgDebug)
     ::Info("AliLoader::ChangeDir","Can not find directory %s in file %s, creating...",
            dirname.Data(),file->GetName());
    
    if (file->IsWritable() == kFALSE)
     {
       ::Error("AliLoader::ChangeDir","Can not create directory. File %s in not writable.",
                file->GetName());
       return 0x0;
     }
            
    TDirectory* newdir = file->mkdir(dirname);
    if (newdir == 0x0)
     {
       ::Error("AliLoader::ChangeDir","Failed to create new directory in file %s.",
               file->GetName());
       return 0x0;
     }
    result = file->cd(dirname);
    if (result == kFALSE)
     {
       return 0x0;
     }
  }
 else
  {
   file->cd();//make a file active 
   file->cd(dirname);//cd event dir
  }

 return gDirectory;
}
/*****************************************************************************/ 

TString AliLoader::GetUnixDir()
 {
 //This Method will manage jumping through unix directories in case of 
 //run with more events per run than defined in gAlice
 
   TString dir;
   
   return dir;
 }
/*****************************************************************************/ 
/************************************************************/

void AliLoader::MakeTree(Option_t *option)
 {
//Makes a tree depending on option 
//   H: - Hits
//   D: - Digits
//   S: - Summable Digits
//   R: - Reconstructed Points (clusters)
//   T: - Tracks (tracklets)

  const char *oH = strstr(option,"H");
  const char *oD = strstr(option,"D");
  const char *oS = strstr(option,"S");
  const char *oR = strstr(option,"R");
  const char *oT = strstr(option,"T");
  
  if (oH) MakeHitsContainer();
  if (oD) MakeDigitsContainer();
  if (oS) MakeSDigitsContainer();
  if (oR) MakeRecPointsContainer();
  if (oT) MakeTracksContainer();
 }

/*****************************************************************************/ 
Int_t  AliLoader::WriteHits(Option_t* opt)
 {
   AliDataLoader* dl = GetHitsDataLoader();
   Int_t ret = dl->WriteData(opt);
   return ret;
 }
/*****************************************************************************/ 

Int_t AliLoader::WriteSDigits(Option_t* opt)
 {
   AliDataLoader* dl = GetSDigitsDataLoader();
   Int_t ret = dl->WriteData(opt);
   return ret;
 }
 
/*****************************************************************************/ 

Int_t AliLoader::PostSDigitizer(TTask* sdzer)
{
  return GetSDigitsDataLoader()->GetBaseTaskLoader()->Post(sdzer);
}
/*****************************************************************************/ 

Int_t AliLoader::PostDigitizer(AliDigitizer* task)
 {
  return GetDigitsDataLoader()->GetBaseTaskLoader()->Post(task);
 }
/*****************************************************************************/ 

Int_t AliLoader::PostReconstructioner(TTask* task)
 {
  return GetRecPointsDataLoader()->GetBaseTaskLoader()->Post(task);
 }
/*****************************************************************************/ 

Int_t AliLoader::PostTracker(TTask* task)
 {
  return GetTracksDataLoader()->GetBaseTaskLoader()->Post(task);
 }
/*****************************************************************************/ 

TObject** AliLoader::GetDetectorDataRef(TObject *obj)
{
 if (obj == 0x0)
  {
    return 0x0;
  }
 return GetDetectorDataFolder()->GetListOfFolders()->GetObjectRef(obj) ;
}
/*****************************************************************************/ 

TObject** AliLoader::SDigitizerRef()
{
  TTask* rsd = AliRunLoader::GetRunSDigitizer();
  if (rsd == 0x0)
   {
     return 0x0;
   }
  return rsd->GetListOfTasks()->GetObjectRef(SDigitizer());
}
/*****************************************************************************/ 

TObject** AliLoader::DigitizerRef()
{
 TTask* rd = AliRunLoader::GetRunDigitizer();
 if (rd == 0x0)
  {
    return 0x0;
  }
 return rd->GetListOfTasks()->GetObjectRef(Digitizer()) ;
}
/*****************************************************************************/ 

TObject** AliLoader::ReconstructionerRef()
{
  TTask* rrec = AliRunLoader::GetRunReconstructioner();
  if (rrec == 0x0)
   {
     return 0x0;
   }
  return rrec->GetListOfTasks()->GetObjectRef(Reconstructioner());
}
/*****************************************************************************/ 

TObject** AliLoader::TrackerRef()
{
   TTask* rrec = AliRunLoader::GetRunTracker();
   if (rrec == 0x0)
    {
      return 0x0;
    }
   return rrec->GetListOfTasks()->GetObjectRef(Tracker());
}

/*****************************************************************************/ 
void AliLoader::CleanFolders()
 {
 //Cleans all posted objects == removes from folders and deletes them
   TIter next(fDataLoaders);
   AliDataLoader* dl;
   while ((dl = (AliDataLoader*)next()))
    { 
      if (GetDebug()) Info("CleanFolders","name = %s cleaning",dl->GetName());
      dl->Clean();
    }
 }
/*****************************************************************************/ 

/*****************************************************************************/ 

void AliLoader::CleanSDigitizer()
{
//removes and deletes detector task from Run Task
  TTask* task = AliRunLoader::GetRunSDigitizer();
  if (task == 0x0)
   {
     Error("CleanSDigitizer","Can not get RunSDigitizer from folder. Can not clean");
     return;
   }

  if (GetDebug()) Info("CleanSDigitizer","Attempting to delete S Digitizer");
  delete task->GetListOfTasks()->Remove(SDigitizer()); //TTList::Remove does not delete object
}
/*****************************************************************************/ 

void AliLoader::CleanDigitizer()
{
//removes and deletes detector task from Run Task
  TTask* task = AliRunLoader::GetRunDigitizer();
  if (task == 0x0)
   {
     Error("CleanDigitizer","Can not get RunDigitizer from folder. Can not clean");
     return;
   }

  if (GetDebug()) 
   Info("CleanDigitizer","Attempting to delete Digitizer %X",
         task->GetListOfTasks()->Remove(Digitizer()));
  delete task->GetListOfTasks()->Remove(Digitizer()); //TTList::Remove does not delete object
}
/*****************************************************************************/ 

void AliLoader::CleanReconstructioner()
{
//removes and deletes detector Reconstructioner from Run Reconstructioner
  TTask* task = AliRunLoader::GetRunReconstructioner();
  if (task == 0x0)
   {
     Error("CleanReconstructioner","Can not get RunReconstructioner from folder. Can not clean");
     return;
   }

  if (GetDebug()) 
   Info("CleanReconstructioner","Attempting to delete Reconstructioner");
  delete task->GetListOfTasks()->Remove(Reconstructioner()); //TTList::Remove does not delete object
}
/*****************************************************************************/ 

void AliLoader::CleanTracker()
{
//removes and deletes detector tracker from Run Tracker
  TTask* task = AliRunLoader::GetRunTracker();
  if (task == 0x0)
   {
     Error("CleanTracker","Can not get RunTracker from folder. Can not clean");
     return;
   }

  if (GetDebug()) 
   Info("CleanTracker","Attempting to delete Tracker %X",
         task->GetListOfTasks()->Remove(Tracker()));
  delete task->GetListOfTasks()->Remove(Tracker()); //TTList::Remove does not delete object
}
/*****************************************************************************/ 

Int_t AliLoader::ReloadAll()
{
 TIter next(fDataLoaders);
 AliDataLoader* dl;
 
 while((dl = (AliDataLoader*)next()))
  {
   Int_t err = dl->Reload();
   if (err)
    {
      Error("ReloadAll","Reload returned error for %s",dl->GetName());
      return err;
    }
  }
 return 0;
}
/*****************************************************************************/ 

void AliLoader::CloseFiles()
{
//close files for data loaders
 TIter next(fDataLoaders);
 AliDataLoader* dl;
 while((dl = (AliDataLoader*)next()))
  {
   dl->CloseFile();
  }
}
/*****************************************************************************/ 

Int_t  AliLoader::SetEventFolder(TFolder* eventfolder)
{
 if (eventfolder == 0x0)
  {
    Error("SetEventFolder","Stupid joke. Argument is NULL");
    return 1;
  }

 fEventFolder = eventfolder;
 TIter next(fDataLoaders);
 AliDataLoader* dl;
 
 while((dl = (AliDataLoader*)next()))
  {
    dl->SetEventFolder(fEventFolder);
    dl->SetFolder(GetDetectorDataFolder()); //Must exists - ensure register is called before
  }

 return 0;
}//sets the event folder
/*****************************************************************************/ 

Int_t AliLoader::Register(TFolder* eventFolder)
{
//triggers creation of subfolders for a given detector
//this method is called when session is read from disk
//
//warning: AliDetector in constructor (not default) calls
//creation of folder structure as well (some detectors needs folders 
//alrady in constructors)

 if (GetDebug()) Info("Register","Name is %s.",GetName());
 if (eventFolder == 0x0)
  {
    Error("Register","Event folder is not set.");
    return 1;
  }
 Int_t retval = AliConfig::Instance()->AddDetector(eventFolder,fDetectorName,fDetectorName);
 if(retval)
  {
    Error("SetEventFolder","Can not create tasks and/or folders for %s. Event folder name is %s",
          fDetectorName.Data(),eventFolder->GetName());
    return retval;
  }
 SetEventFolder(eventFolder);
 return 0;
}
/*****************************************************************************/ 
AliRunLoader* AliLoader::GetRunLoader()
{
//gets the run-loader from event folder
  AliRunLoader* rg = 0x0;
  TObject * obj = GetEventFolder()->FindObject(AliRunLoader::fgkRunLoaderName);
  if (obj) rg = dynamic_cast<AliRunLoader*>(obj);
  return rg;
}
/*****************************************************************************/ 
Bool_t  AliLoader::TestFileOption(Option_t* opt)
{
//tests the TFile::Option
//if file is truncated at opening moment ("recreate", "new" or "create") returns kFALSE;
//else kTRUE (means opened with "read" or "update" mode)
  TString option(opt);
  if (option.CompareTo("recreate",TString::kIgnoreCase) == 0) return kFALSE;
  if (option.CompareTo("new",TString::kIgnoreCase) == 0) return kFALSE;
  if (option.CompareTo("create",TString::kIgnoreCase) == 0) return kFALSE;
  return kTRUE;
}
/*****************************************************************************/ 
void  AliLoader::SetDirName(TString& dirname)
{
//adds "dirname/" to each file 
  TIter next(fDataLoaders);
  AliDataLoader* dl;
  while((dl = (AliDataLoader*)next()))
   {
    dl->SetDirName(dirname);
   }
}

/*****************************************************************************/ 

void AliLoader::SetDigitsFileNameSuffix(const TString& suffix)
{
  //adds the suffix before ".root", 
  //e.g. TPC.Digits.root -> TPC.DigitsMerged.root
  //made on Jiri Chudoba demand
  GetDigitsDataLoader()->SetFileNameSuffix(suffix);
}
/*****************************************************************************/ 

void AliLoader::SetCompressionLevel(Int_t cl)
{
//sets comression level for data defined by di
  TIter next(fDataLoaders);
  AliDataLoader* dl;
  while((dl = (AliDataLoader*)next()))
   {
     dl->SetCompressionLevel(cl);
   }
}
/*****************************************************************************/ 

void AliLoader::Clean()
{
//Cleans all data loaders
  TIter next(fDataLoaders);
  AliDataLoader* dl;
  while((dl = (AliDataLoader*)next()))
   {
     dl->Clean();
   }
}
/*****************************************************************************/

void AliLoader::Clean(const TString& name)
{
  TObject* obj = GetDetectorDataFolder()->FindObject(name);
  if(obj)
   {
     if (GetDebug())
       Info("Clean(const TString&)","name=%s, cleaning %s.",GetName(),name.Data());
     GetDetectorDataFolder()->Remove(obj);
     delete obj;
   }
}

/*****************************************************************************/ 

Bool_t AliLoader::IsOptionWritable(const TString& opt)
{
  if (opt.CompareTo("recreate",TString::kIgnoreCase)) return kTRUE;
  if (opt.CompareTo("new",TString::kIgnoreCase)) return kTRUE;
  if (opt.CompareTo("create",TString::kIgnoreCase)) return kTRUE;
  if (opt.CompareTo("update",TString::kIgnoreCase)) return kTRUE;
  return kFALSE;
}

/*****************************************************************************/ 
/*****************************************************************************/ 
/*****************************************************************************/ 

