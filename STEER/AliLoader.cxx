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
   TObject* fobj = GetTopFolder()->FindObject(eventfoldername);
   fEventFolder = (fobj)?dynamic_cast<TFolder*>(fobj):0x0;//in case FindObject returns NULL dynamic cast cause seg. fault
   if (fEventFolder == 0x0)
    {
      Fatal("AliLoader","Can not find event folder %s. Aborting",eventfoldername);
    }

   fDetectorName = detname;
   fName = fDetectorName+"Loader";

   InitDefaults();
   //fileoption's don't need to initialized because default TString ctor does it correctly
 }
/*****************************************************************************/ 

AliLoader::AliLoader(const Char_t * detname,TFolder* eventfolder):
 fDetectorName(""),
 fEventFolder(0x0),
 fDataFolder(0x0),
 fDetectorDataFolder(0x0),
 fModuleFolder(0x0),
 fTasksFolder(0x0),
 fQAFolder(0x0)
{

   fEventFolder = eventfolder;
   fDetectorName = detname;
   fName = fDetectorName+"Loader";

   InitDefaults();
   //fileoption's don't need to initialized because default TString ctor does it correctly
}
/*****************************************************************************/ 

AliLoader::~AliLoader()
{
  CleanFolders();
  CloseFiles();
}
/*****************************************************************************/ 

void AliLoader::ResetDataInfo()
{
  fDataInfo[kHits].File() = 0x0;
  fDataInfo[kSDigits].File() = 0x0;
  fDataInfo[kDigits].File() = 0x0;
  fDataInfo[kRecPoints].File() = 0x0;
  fDataInfo[kTracks].File() = 0x0;
   
  fDataInfo[kHits].Directory() = 0x0;
  fDataInfo[kSDigits].Directory() = 0x0;
  fDataInfo[kDigits].Directory() = 0x0;
  fDataInfo[kRecPoints].Directory() = 0x0;
  fDataInfo[kTracks].Directory() = 0x0;
 
}
/*****************************************************************************/ 

void AliLoader::InitDefaults()
{

  fDataInfo[kHits].FileName() = fDetectorName + ".Hits.root";
  fDataInfo[kSDigits].FileName() = fDetectorName + ".SDigits.root";
  fDataInfo[kDigits].FileName() = fDetectorName + ".Digits.root";
  fDataInfo[kRecPoints].FileName() = fDetectorName + ".RecPoints.root";
  fDataInfo[kTracks].FileName() = fDetectorName + ".Tracks.root";
  
  fDataInfo[kHits].ContainerName() = fgkDefaultHitsContainerName;
  fDataInfo[kSDigits].ContainerName() = fgkDefaultSDigitsContainerName;
  fDataInfo[kDigits].ContainerName() = fgkDefaultDigitsContainerName;
  fDataInfo[kRecPoints].ContainerName() = fgkDefaultRecPointsContainerName;
  fDataInfo[kTracks].ContainerName() = fgkDefaultTracksContainerName;

  fDataInfo[kHits].Name() = "Hits";
  fDataInfo[kSDigits].Name() = "Summable Digits";
  fDataInfo[kDigits].Name() = "Digits";
  fDataInfo[kRecPoints].Name() = "Reconstructed Points";
  fDataInfo[kTracks].Name() = "Tracks";

}
/*****************************************************************************/ 

Int_t AliLoader::LoadData(AliLoaderDataInfo& di,Option_t* opt)
{
  if (GetDebug()) Info("LoadData","name=%s, data type = %s, option = %s",GetName(),di.Name().Data(),opt);
  if (Tree(di))
   {
      Warning("LoadData","Data <<%s>> are already loaded. Use ReloadData to force reload. Nothing done",di.Name().Data());
      return 0;
   }
    //else force option was used
  SetFileOption(di,opt);

  Int_t retval;
  retval = OpenDataFile(di,opt);
  if (retval) 
   {
    Error("LoadData","Error occured while opening <<%s>> file",di.Name().Data());
    return retval;
   }

  //if file is recreated there is no sense to search for data to post and get Error message
  if (AliLoader::TestFileOption(opt) == kFALSE)
   {
     MakeTree(di);
    return 0;
   }

  retval = PostData(di);
  if (retval)
   {
    Error("LoadData","Error occured while posting %s from file to folder",di.Name().Data());
    return retval;
   }

  return 0;
 
}

/*****************************************************************************/ 

Int_t AliLoader::LoadSDigitizer(const TString name, Option_t* opt)
{
  if (SDigitizer(name))
   {
     Warning("LoadSDigitizer","%s S. Digitzier Task already exists",GetDetectorName().Data());
     Warning("LoadSDigitizer","Clean it first to load one from a file. Nothing done");
     return 0;
   }
  TTask* rsd = AliRunLoader::GetRunSDigitizer();
  if (rsd == 0x0)
   {
     Error("LoadSDigitizer","Can not get RunSDigitizer from folder. Can not load");
     return 1;
   }
  
  Int_t retval; //returned value
  if (File(kSDigits)  == 0x0)
   {
      retval = OpenSDigitsFile(opt);
      if (retval) 
       {
         Error("LoadSDigitizer","Error occured while opening Summable Digits file");
         return retval;
       }
   }
  //test if there is sense to look for data in file
  //if it is recreates
  if (AliLoader::TestFileOption(opt) == kFALSE) return 0;
  
  retval = PostSDigitizer(name);      
  if (retval) 
   {
    Error("LoadSDigits","Error occured while posting SDigits from file to folder");
    return retval;
   }
  return 0;

}
/*****************************************************************************/ 

Int_t AliLoader::LoadDigitizer(const TString name, Option_t* opt)
{
  if (Digitizer(name))
   {
     Warning("LoadDigitizer","%s Digitzier Task already exists",GetDetectorName().Data());
     Warning("LoadDigitizer","Clean it first to load one from a file. Nothing done");
     return 0;
   }
  TTask* rsd = AliRunLoader::GetRunDigitizer();
  if (rsd == 0x0)
   {
     Error("LoadDigitizer","Can not get RunDigitizer from folder. Can not load");
     return 1;
   }
  
  Int_t retval; //returned value
  if (File(kDigits)  == 0x0)
   {
      retval = OpenDigitsFile(opt);
      if (retval) 
       {
         Error("LoadDigitizer","Error occured while opening Digits file");
         return retval;
       }
   }
  //test if there is sense to look for data in file
  //if it is recreates
  if (AliLoader::TestFileOption(opt) == kFALSE) return 0;
  
  retval = PostDigitizer(name);
  if (retval) 
   {
    Error("LoadDigitizer","Error occured while posting Digitizer from file to task");
    return retval;
   }
  return 0;

}
/*****************************************************************************/ 

Int_t AliLoader::LoadReconstructioner(const TString name, Option_t* opt)
{
   if (Reconstructioner(name))
   {
     Warning("LoadReconstructioner","%s Reconstructioner Task already exists",GetDetectorName().Data());
     Warning("LoadReconstructioner","Clean it first to load one from a file. Nothing done");
     return 0;
   }
  TTask* rsd = AliRunLoader::GetRunReconstructioner();
  if (rsd == 0x0)
   {
     Error("LoadReconstructioner","Can not get Run Reconstructioner from folder. Can not load");
     return 1;
   }
  
  Int_t retval; //returned value
  if (File(kRecPoints)  == 0x0)
   {
      retval = OpenRecPointsFile(opt);
      if (retval) 
       {
         Error("LoadReconstructioner","Error occured while opening Reconstructed Points file");
         return retval;
       }
   }
  //test if there is sense to look for data in file
  //if it is recreates
  if (AliLoader::TestFileOption(opt) == kFALSE) return 0;
  
  retval = PostReconstructioner(name);
  if (retval) 
   {
    Error("LoadReconstructioner","Error occured while posting Reconstructioner from file to Task");
    return retval;
   }
  return 0;

}
/*****************************************************************************/ 

Int_t AliLoader::LoadTracker(const TString name, Option_t* opt)
{
   if (Tracker(name))
   {
     Warning("LoadTracker","%s Tracker Task already exists",GetDetectorName().Data());
     Warning("LoadTracker","Clean it first to load one from a file. Nothing done");
     return 0;
   }
  TTask* rsd = AliRunLoader::GetRunReconstructioner();
  if (rsd == 0x0)
   {
     Error("LoadReconstructioner","Can not get Run Reconstructioner from folder. Can not load");
     return 1;
   }
  
  Int_t retval; //returned value
  if (File(kRecPoints)  == 0x0)
   {
      retval = OpenRecPointsFile(opt);
      if (retval) 
       {
         Error("LoadReconstructioner","Error occured while opening Reconstructed Points file");
         return retval;
       }
   }
  //test if there is sense to look for data in file
  //if it is recreates
  if (AliLoader::TestFileOption(opt) == kFALSE) return 0;
  
  retval = PostReconstructioner(name);
  if (retval) 
   {
    Error("LoadReconstructioner","Error occured while posting Reconstructioner from file to Task");
    return retval;
   }
  return 0;
}

/*****************************************************************************/ 


Int_t AliLoader::SetEvent()
{
 //basically the same that GetEvent but do not post data to folders
   CleanFolders();
   
   AliRunLoader* rg = GetRunLoader();
   if (rg == 0x0)
    {
      Error("SetEvent","Can not get RunGettr");
      return 1;
    }
   Int_t evno = rg->GetEventNumber();
   
   if(File(kHits))
    {
      if (CheckReload(File(kHits),FileName(kHits)))
       {
         UnloadHits();
         if (GetDebug()) Info("SetEvent","Reloading new file for hits. file opt is %s",FileOption(kHits).Data());
         OpenHitsFile(FileOption(kHits));
       }
      Directory(kHits) = ChangeDir(File(kHits),evno);
      if (Directory(kHits) == 0x0)
        {
          Error("SetEvent","Can not chage directory to file %s",File(kHits)->GetName());
          return 1;
        }
    }

   if(File(kSDigits))
    {
      if (CheckReload(File(kSDigits),FileName(kSDigits)))
       {
         UnloadSDigits();
         if (GetDebug()) Info("SetEvent","Reloading new file for sdigits. file opt is %s",FileOption(kSDigits).Data());
         OpenSDigitsFile(FileOption(kSDigits));
       }

      Directory(kSDigits) = ChangeDir(File(kSDigits),evno);
      if (Directory(kSDigits) == 0x0)
        {
          Error("SetEvent","Can not chage directory to file %s",File(kSDigits)->GetName());
          return 1;
        }
    }
   
   if(File(kDigits))
    {
      if (CheckReload(File(kDigits),FileName(kDigits)))
       {
         UnloadDigits();
         OpenDigitsFile(FileOption(kDigits));
       }
    
      Directory(kDigits) = ChangeDir(File(kDigits),evno);
      if (Directory(kDigits) == 0x0)
        {
          Error("SetEvent","Can not chage directory to file %s",File(kDigits)->GetName());
          return 1;
        }
    }
   
   if(File(kRecPoints))
    {
      if (CheckReload(File(kRecPoints),FileName(kRecPoints)))
       {
         UnloadRecPoints();
         OpenRecPointsFile(FileOption(kRecPoints));
       }
    
      Directory(kRecPoints) = ChangeDir(File(kRecPoints),evno);
      if (Directory(kRecPoints) == 0x0)
        {
          Error("SetEvent","Can not chage directory to file %s",File(kRecPoints)->GetName());
          return 1;
        }
    }

   if(File(kTracks))
    {
      if (CheckReload(File(kTracks),FileName(kTracks)))
       {
         UnloadTracks();
         OpenTracksFile(FileOption(kTracks));
       }

      Directory(kTracks) = ChangeDir(File(kTracks),evno);
      if (Directory(kTracks) == 0x0)
        {
          Error("SetEvent","Can not chage directory to file %s",File(kTracks)->GetName());
          return 1;
        }
    }
  return 0;
 
}
/******************************************************************/

Int_t AliLoader::GetEvent(const char * opt)
 {
 //changes to proper root  directory and tries to load data from files to folders
 // 
 // 
 //returns:
 //     0  - in case of no error
 //     1  - event not found
 //     
   
   
   if (SetEvent())
    {
      Error("GetEvent","Set Event returned error");
      return 1;
    }
   
   if( strstr(opt,"H") )
     if(File(kHits)) 
       PostHits();
   if( strstr(opt,"S") )
     if(File(kSDigits)) 
       PostSDigits();
   if( strstr(opt,"D") )
     if(File(kDigits)) 
       PostDigits();
   if( strstr(opt,"R") ) {
     if(File(kRecPoints)) 
       PostRecPoints();
     if(File(kTracks)) 
       PostTracks();
   }
   return 0;
 
 }
/******************************************************************/

Int_t AliLoader::WriteData(AliLoaderDataInfo& di,Option_t* opt)
{
//Writes data defined by di object
//opt might be "OVERWRITE" in case of forcing overwriting

  TObject *data = GetDetectorDataFolder()->FindObject(di.ContainerName());
  if(data == 0x0)
   {//did not get, nothing to write
     Warning("WriteData","%s tree named %s not found in folder %s.\nNothing to write. Returning",
              di.Name().Data(),di.ContainerName().Data(), 
              GetDataFolder()->FindFullPathName(GetDetectorDataFolder()->GetName()));
     return 0;
   }
  
  //check if file is opened
  if (di.Directory() == 0x0)
   { 
     //if not try to open
     SetFileOption(di,"UPDATE");
     if (OpenDataFile(di,"UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteData","Can not open hits file. %s ARE NOT WRITTEN",di.Name().Data());
        return 1;
      }
   }

  if (di.File()->IsWritable() == kFALSE)
   {
     Error("WriteData","File %s is not writable",di.File()->GetName());
     return 2;
   }
  
  di.Directory()->cd(); //set the proper directory active

  //see if hits container already exists in this (root) directory
  TObject* obj = di.Directory()->Get(di.ContainerName());
  if (obj)
   { //if they exist, see if option OVERWRITE is used
     const char *oOverWrite = strstr(opt,"OVERWRITE");
     if(!oOverWrite)
      {//if it is not used -  give an error message and return an error code
        Error("WriteData","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  
  if (GetDebug()) Info("WriteData","name = %s, DataName = %s, opt = %s, data object name = %s",
                   GetName(),di.Name().Data(),opt,data->GetName());
  if (GetDebug()) Info("WriteData","File Name = %s, Directory Name = %s Directory's File Name = %s",
                   di.File()->GetName(),di.Directory()->GetName(),
                   di.Directory()->GetFile()->GetName());
  
  //if a data object is a tree set the directory
  TTree* tree = dynamic_cast<TTree*>(data);
  if (tree) tree->SetDirectory(di.Directory()); //forces setting the directory to this directory (we changed dir few lines above)
  
  if (GetDebug()) Info("WriteData","Writing tree");
  data->Write(0,TObject::kOverwrite);
  return 0;
 
}
/*****************************************************************************/ 

Int_t  AliLoader::WriteSDigitizer(const TString name, Option_t* opt)
{
//Writes SDigitizer to detector Summbale Digits file/current event ROOT directory
//Reads it from "ROOT://fgkTopFolderName/fgkTasksFolderName/fgkSDigitizerTaskName/fDetName/'fDetName+SDigitizer'
//  TTask *sdgzer = SDigitizer();
  
  TTask* sder = SDigitizer(name); 
  if (sder == 0x0)
   {
     Warning("WriteSDigitizer","Can not get SDigitizer. Nothing to write");
     return 0;
   }
  if (Directory(kSDigits) == 0x0)
   { 
     //if not open, try to open
     SetSDigitsFileOption("UPDATE");
     if (OpenSDigitsFile("UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteSDigitizer","Can not open sdigits file. SDIGITIZER IS NOT WRITTEN");
        return 1;
      }
   }
  if (File(kSDigits)->IsWritable() == kFALSE)
   {
     Error("WriteSDigits","File %s is not writable",File(kSDigits)->GetName());
     return 2;
   }
  //TString name(fDetectorName + AliConfig::Instance()->GetSDigitizerTaskName());
  //TString name(AliConfig::Instance()->GetSDigitizerTaskName());
  
  TObject* task = Directory(kSDigits)->Get(name);
  if (task)
   { //if they exist, see if option OVERWRITE is used
     const char *oOverWrite = strstr(opt,"OVERWRITE");
     if(!oOverWrite)
      {//if it is not used -  give an error message and return an error code
        Error("WriteSDigits","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  if (GetDebug()) Info("WriteSDigitizer",
       "name = %s, opt = %s, File(kSDigits) = %s, Sum. digitizer = %s",
       GetName(),opt,File(kSDigits)->GetName(),name.Data());

  Directory(kSDigits)->cd();
  sder->Write(0,TObject::kOverwrite);
  return 0;
}
/*****************************************************************************/ 

Int_t  AliLoader::WriteDigitizer(const TString name, Option_t* opt)
{
//Writes Digitizer to detector "Digits file"/"current event ROOT directory"
//Reads it from "ROOT://fgkTopFolderName/fgkTasksFolderName/fgkDigitizerTaskName/fDetName/'fDetName+Digitizer'
  TTask* sder = Digitizer(name); 
  if (sder == 0x0)
   {
     Warning("WriteDigitizer","Can not get Digitizer. Nothing to write");
     return 0;
   }
  if (Directory(kDigits) == 0x0)
   { 
     //if not open, try to open
     SetDigitsFileOption("UPDATE");
     if (OpenDigitsFile("UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteDigitizer","Can not open digits file. SDIGITIZER IS NOT WRITTEN");
        return 1;
      }
   }
  if (File(kDigits)->IsWritable() == kFALSE)
   {
     Error("WriteDigits","File %s is not writable",File(kDigits)->GetName());
     return 2;
   }
  //TString name(fDetectorName + AliConfig::Instance()->GetDigitizerTaskName());

  TObject* task = Directory(kDigits)->Get(name);
  if (task)
   { //if they exist, see if option OVERWRITE is used
     const char *oOverWrite = strstr(opt,"OVERWRITE");
     if(!oOverWrite)
      {//if it is not used -  give an error message and return an error code
        Error("WriteDigits","Task already exisists in file. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  if (GetDebug()) Info("WriteDigitizer",
       "name = %s, opt = %s, File(kDigits) = %s, Directory(kDigits) =%s, Digitizer = %s",
       GetName(),opt,File(kDigits)->GetName(),Directory(kDigits)->GetName(),name.Data());

  Directory(kDigits)->cd();
  sder->Write(0,TObject::kOverwrite);
  return 0;
}
/*****************************************************************************/ 
Int_t  AliLoader::WriteReconstructioner(Option_t* opt)
{
//Writes Tracker to detector "RecPoint file"/"current event ROOT directory"
//Reads it from "ROOT://fgkTopFolderName/fgkTasksFolderName/fgkTrackerTaskName/fDetName/'fDetName+Tracker'
//Writes Digitizer to detector "Digits file"/"current event ROOT directory"
//Reads it from "ROOT://fgkTopFolderName/fgkTasksFolderName/fgkDigitizerTaskName/fDetName/'fDetName+Digitizer'
  TTask* sder = Reconstructioner(); 
  if (sder == 0x0)
   {
     Warning("WriteReconstructioner","Can not get Reconstructioner. Nothing to write");
     return 0;
   }
  if (Directory(kRecPoints) == 0x0)
   { 
     //if not open, try to open
     SetRecPointsFileOption("UPDATE");
     if (OpenRecPointsFile("UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteReconstructioner","Can not open digits file. RECONSTRUCTIONER IS NOT WRITTEN");
        return 1;
      }
   }
  if (File(kRecPoints)->IsWritable() == kFALSE)
   {
     Error("WriteReconstructioner","File %s is not writable",File(kRecPoints)->GetName());
     return 2;
   }
  TString name(fDetectorName + AliConfig::Instance()->GetReconstructionerTaskName());

  TObject* task = Directory(kRecPoints)->Get(name);
  if (task)
   { //if they exist, see if option OVERWRITE is used
     const char *oOverWrite = strstr(opt,"OVERWRITE");
     if(!oOverWrite)
      {//if it is not used -  give an error message and return an error code
        Error("WriteReconstructioner","Task already exisists in file. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }

  if (GetDebug()) 
   Info("WriteReconstructioner",
        "name = %s, opt = %s, File(kRecPoints) = %s, Directory(kRecPoints) =%s, Reconstructioner = %s",
         GetName(),opt,File(kRecPoints)->GetName(),Directory(kRecPoints)->GetName(),name.Data());

  Directory(kRecPoints)->cd();
  sder->Write(0,TObject::kOverwrite);
  return 0;
}
/*****************************************************************************/ 

Int_t  AliLoader::WriteTracker(Option_t* opt)
{
//Writes Tracker to detector "Tracks file"/"current event ROOT directory"
//Reads it from "ROOT://fgkTopFolderName/fgkTasksFolderName/fgkTrackerTaskName/fDetName/'fDetName+Tracker'

  TTask* sder = Tracker(); 
  if (sder == 0x0)
   {
     Warning("WriteTracker","Can not get Tracker. Nothing to write");
     return 0;
   }
  if (Directory(kTracks) == 0x0)
   { 
     //if not open, try to open
     SetTracksFileOption("UPDATE");
     if (OpenTracksFile("UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteTracker","Can not open Tracks file. STracker IS NOT WRITTEN");
        return 1;
      }
   }
  if (File(kTracks)->IsWritable() == kFALSE)
   {
     Error("WriteTracks","File %s is not writable",File(kTracks)->GetName());
     return 2;
   }
  TString name(fDetectorName + AliConfig::Instance()->GetTrackerTaskName());

  TObject* task = Directory(kTracks)->Get(name);
  if (task)
   { //if they exist, see if option OVERWRITE is used
     const char *oOverWrite = strstr(opt,"OVERWRITE");
     if(!oOverWrite)
      {//if it is not used -  give an error message and return an error code
        Error("WriteTracks","Task already exisists in file. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }

  if (GetDebug()) 
   Info("WriteTracker",
        "name = %s, opt = %s, File(kTracks) = %s, Directory(kTracks) =%s, Reconstructioner = %s",
         GetName(),opt,File(kTracks)->GetName(),Directory(kTracks)->GetName(),name.Data());

  Directory(kTracks)->cd();
  sder->Write(0,TObject::kOverwrite);
  return 0;
}
/*****************************************************************************/ 

Int_t AliLoader::OpenDataFile(const TString& filename,TFile*& file,TDirectory*& dir,Option_t* opt,Int_t cl)
{
//Opens file named 'filename', and assigns pointer to it to 'file'
//jumps to directory corresponding to current event and stores the pointer to it in 'dir'
//option 'opt' is passed to TFile::Open
  if (file)
   {
     if(file->IsOpen() == kTRUE)
       {
         //Warning("OpenDataFile>"," File %s already opened. First close it.",file->GetName());
         return 0;
       }
     else
       {
         Warning("OpenDataFile","Pointer to file %s is not null, but file is not opened",
                file->GetName());
         delete file;
         file = 0x0;
       }
   }
  
  file = (TFile *)(gROOT->GetListOfFiles()->FindObject(filename));
  if (file)
   {
     if(file->IsOpen() == kTRUE)
       {
         Warning("OpenDataFile","File %s already opened by sombody else. First close it.",
                 file->GetName());
         return 0;
       }
   }
  
  file = TFile::Open(filename,opt);//open the file
  if (file == 0x0)
   {//file is null
     Error("OpenDataFile","Can not open file %s",filename.Data());
     return 1;
   }
  if (file->IsOpen() == kFALSE)
   {//file is null
     Error("OpenDataFile","Can not open file %s",filename.Data());
     return 1;
   }

  file->SetCompressionLevel(cl);
  
  AliRunLoader* rg = GetRunLoader();
  if (rg == 0x0)
   {
     Error("OpenDataFile","Can not find Run-Loader in folder");
     return 2;
   }
  Int_t evno = rg->GetEventNumber();
  
  dir = ChangeDir(file,evno);
  if (dir == 0x0)
   {
     Error("OpenDataFile","Can not chage directory in file %s.",file->GetName());
     return 3; 
   }

  return 0;
}
/******************************************************************/

void  AliLoader::CloseDataFile(TFile*& file,TDirectory*& dir)
{
//deletes file and sets pointers to 0x0
  if (file)
   {
    delete file;
    file = 0x0;
   }
  dir = 0x0;
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
//returns the folder speciofic to given detector e.g. /Folders/Event/Data/TPC/
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
TTask* AliLoader::SDigitizer(const TString name )
{
//returns SDigitizer task for this detector

  TTask* rsd = AliRunLoader::GetRunSDigitizer();
   if (rsd == 0x0)
    {
      return 0x0;
    }
   //TString name(fDetectorName + AliConfig::Instance()->GetSDigitizerTaskName());
   TObject* obj = rsd->GetListOfTasks()->FindObject(name);
   return (obj)?dynamic_cast<TTask*>(obj):0x0;
}
/*****************************************************************************/ 

AliDigitizer* AliLoader::Digitizer(const TString name)
{
//returns Digitizer task for this detector
  TTask* rd = AliRunLoader::GetRunDigitizer();
   if (rd == 0x0)
    {
      return 0x0;
    }
   //TString name(fDetectorName + AliConfig::Instance()->GetDigitizerTaskName());
  TObject* obj = rd->GetListOfTasks()->FindObject(name);
  return (obj)?dynamic_cast<AliDigitizer*>(obj):0x0;
}
/*****************************************************************************/ 

TTask* AliLoader::Reconstructioner(const TString name)
{
//returns Recontructioner (Cluster Finder, Cluster Maker, 
//or whatever you want to call it) task for this detector
  TTask* rrec = AliRunLoader::GetRunReconstructioner();
  if (rrec == 0x0)
    {
      return 0x0;
    }
  //TString name(fDetectorName + AliConfig::Instance()->GetReconstructionerTaskName());
  TObject* obj = rrec->GetListOfTasks()->FindObject(name);
  return (obj)?dynamic_cast<TTask*>(obj):0x0;
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

TTask* AliLoader::Tracker(const TString name)
{
  TTask* tracker = AliRunLoader::GetRunTracker();
  if (tracker == 0x0)
    {
      return 0x0;
    }
  //TString name(fDetectorName + AliConfig::Instance()->GetTrackerTaskName());
  TObject* obj = tracker->GetListOfTasks()->FindObject(name);
  return (obj)?dynamic_cast<TTask*>(obj):0x0;
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

void AliLoader::MakeTree(AliLoaderDataInfo& di)
{
//this virtual method creates the tree for hits in the file
  if (Tree(di)) return;//tree already made 
  if (GetDebug()) Info("MakeTree","name = %s, Making %s Tree named %s.",
        GetName(),di.Name().Data(),di.ContainerName().Data());
   
  TTree* tree = new TTree(di.ContainerName(), di.Name() + "Container"); //make a tree
  if (tree == 0x0)
   {
     Error("MakeTree","Can not create %s Container for %s",di.Name().Data(),GetName());
     return;
   }
  tree->SetAutoSave(1000000000); //no autosave
  GetDetectorDataFolder()->Add(tree);
  WriteData(di,"OVERWRITE");//write tree to the file
}

/*****************************************************************************/ 

void AliLoader::MakeHitsContainer()
{
//this virtual method creates the tree for hits in the file
  MakeTree(kHits);
}

/*****************************************************************************/ 

void AliLoader::MakeSDigitsContainer()
 {
//Make standard container for Summable Dogits
//if some detector needs other container - 
//overload this method in detector specific loader i.e. AliTPCLoader
  MakeTree(kSDigits);
 }

/*****************************************************************************/ 

void AliLoader::MakeDigitsContainer()
 {
//Make standard container for digits
//if some detector needs other container - 
//overload this method in detector specific loader i.e. AliTPCLoader

//this virtual method creates the tree for hits in the file
  MakeTree(kDigits);
 }
/*****************************************************************************/ 

void AliLoader::MakeRecPointsContainer()
 {
//Make standard container for RecPoints (Clusters)
//if some detector needs other container - 
//overload this method in detector specific loader i.e. AliTPCLoader

//this virtual method creates the tree for hits in the file
  MakeTree(kRecPoints);
 }
/*****************************************************************************/ 

void AliLoader::MakeTracksContainer()
 {
//Make standard container for tracks
//if some detector needs other container - 
//overload this method in detector specific loader i.e. AliTPCLoader
  MakeTree(kTracks);
 }
/*****************************************************************************/ 

Int_t AliLoader::PostData(AliLoaderDataInfo& di)
{
//Posts the SDigits container to proper folder
//  Warning("PostSDigits","Called");

  TDirectory *dir = di.Directory();
  if ( dir == 0x0)
   {
     Error("PostData","%s Directory is NULL. Load %s before.",
           di.Name().Data(),di.Name().Data());
     return 2; 
   }
  
  TObject* tree = dir->Get(di.ContainerName());
  if(tree)
   {
     //if such an obejct already exists - remove it first
     TObject* obj = GetDetectorDataFolder()->FindObject(di.ContainerName());
     if (tree == obj)
      {
        if (GetDebug()) Info("PostData","This object is already in folder.");
        return 0;
      }
     if (obj)
      {
        Warning("PostData","Object named %s already exitsts in %s data folder. Removing it",
                 di.ContainerName().Data(), fDetectorName.Data());
        Clean(di);
      }
     GetDetectorDataFolder()->Add(tree);
     return 0;
   }
  else
   {
    Warning("PostData","Can not find %s in file %s",
             di.ContainerName().Data(),di.File()->GetName());
   }
  return 0;
}
/*****************************************************************************/ 

Int_t AliLoader::PostSDigitizer(const TString name)
 {
  
   if (Directory(kSDigits) == 0x0)
    {
     Error("PostSDigitizer","SDigitsDir is Null. Probobly SDigits not loaded");
     return 1;
    }
   
   //TString name(fDetectorName + AliConfig::Instance()->GetSDigitizerTaskName());
   TTask* sdigitizer = dynamic_cast<TTask*>(Directory(kSDigits)->Get(name));
   
   if (sdigitizer == 0x0)
    {
     Error("PostSDigitizer","Can not find SDigitizer named %s in file %s for event %d",
           name.Data(),File(kSDigits)->GetName(),GetRunLoader()->GetEventNumber());
     return 2;
    }
  
   return PostSDigitizer(sdigitizer);

 }
/*****************************************************************************/ 
Int_t AliLoader::PostSDigitizer(TTask* sdzer)
{
//adds it to Run SDigitizer
  TTask* rsd = AliRunLoader::GetRunSDigitizer();
  if (rsd == 0x0)
   {
     Error("PostSDigitizer","Can not get RunSDigitizer from folder. Can not post");
     return 1;
   }
  if (GetDebug()) Info("PostSDigitizer","Adding S Digitizer named %s.",sdzer->GetName());
  rsd->Add(sdzer);
  return 0;
}

/*****************************************************************************/ 

Int_t AliLoader::PostDigitizer(const TString name)
 {
   if (Directory(kDigits) == 0x0)
    {
     Error("PostDigitizer","Directory(kDigits) is Null. Probobly Digits not loaded");
     return 1;
    }
   
   // TString name(fDetectorName + AliConfig::Instance()->GetDigitizerTaskName());
   TObject* obj = Directory(kDigits)->Get(name);
   if (obj == 0x0)
    {
     Error("PostDigitizer","Can not find object named % in file %s for event %d",
           name.Data(),File(kSDigits)->GetName(),GetRunLoader()->GetEventNumber());
     return 2;
    }

   AliDigitizer* digitizer = dynamic_cast<AliDigitizer*>(obj);
   if (digitizer == 0x0)
    {
     Error("PostDigitizer","Can not find Digitizer named % in file %s for event %d",
           name.Data(),File(kSDigits)->GetName(),GetRunLoader()->GetEventNumber());
     return 2;
    }
   return PostDigitizer(digitizer);
 }
/*****************************************************************************/ 

Int_t AliLoader::PostDigitizer(AliDigitizer* task)
 {
 //adds it to Run Digitizer
  TTask* rsd = AliRunLoader::GetRunDigitizer();
  if (rsd == 0x0)
   {
     Error("PostDigitizer","Can not get RunDigitizer from folder. Can not post");
     return 1;
   }
  rsd->Add(task);
  return 0;
 }
/*****************************************************************************/ 

Int_t AliLoader::PostReconstructioner(const TString name)
 {
   if ( Directory(kRecPoints) == 0x0)
    {
     Error("PostReconstructioner","RecPointsDir is Null. Probobly RecPoints not loaded");
     return 1;
    }
   
   //TString name(fDetectorName + AliConfig::Instance()->GetReconstructionerTaskName());
   TTask* task = dynamic_cast<TTask*>(Directory(kRecPoints)->Get(name));
   
   if (task == 0x0)
    {
     Error("PostReconstructioner","Can not find Reconstructioner named %s in file %s for event %d",
           name.Data(),File(kRecPoints)->GetName(),GetRunLoader()->GetEventNumber());
     return 2;
    }
  
   return PostReconstructioner(task);
 }
/*****************************************************************************/ 

Int_t AliLoader::PostReconstructioner(TTask* task)
 {
   //adds it to Run Reconstructioner
  TTask* rrec = AliRunLoader::GetRunReconstructioner();
  if (rrec == 0x0)
   {
     Error("PostReconstructioner","Can not get RunReconstructioner from folder. Can not post");
     return 1;
   }
  if (rrec->GetListOfTasks()->FindObject(task->GetName()))
   {
     Error("PostReconstructioner",
           "There already exists sub task named %s in Run Reconstructioner",task->GetName());
     return 2;
   }
  rrec->Add(task);
  return 0;
 }
/*****************************************************************************/ 

Int_t AliLoader::PostTracker(const TString name)
 {
   if (Directory(kTracks) == 0x0)
    {
     Error("PostTracker","Directory(kTracks) is Null. Probobly Tracks or Tracker not loaded");
     return 1;
    }
   
   //TString name(fDetectorName + AliConfig::Instance()->GetTrackerTaskName());
   TObject* obj = Directory(kTracks)->Get(name);
   if (obj == 0x0)
    {
     Error("PostTracker","Can not find object named %s in file %s for event %d",
           name.Data(),File(kTracks)->GetName(),GetRunLoader()->GetEventNumber());
     return 2;
    }

   TTask* task = dynamic_cast<TTask*>(obj);
   if (task == 0x0)
    {
     Error("PostTracker","Can not find Tracker named %s in file %s for event %d",
           name.Data(),File(kTracks)->GetName(),GetRunLoader()->GetEventNumber());
     return 2;
    }

   return PostTracker(task);
 }
/*****************************************************************************/ 

Int_t AliLoader::PostTracker(TTask* task)
 {
  //adds it to Run Tracker
  TTask* runtracker = AliRunLoader::GetRunTracker();
  if (runtracker == 0x0)
   {
     Error("PostTracker","Can not get RunTracker from folder. Can not post");
     return 1;
   }
  
  runtracker->Add(task);
  return 0;

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
 //return rd->GetListOfTasks()->GetObjectRef(Digitizer()) ;
 Fatal("DigitizerRef", "Check where this is needed") ; 
 return 0 ; 
}
/*****************************************************************************/ 

TObject** AliLoader::ReconstructionerRef()
{
  TTask* rrec = AliRunLoader::GetRunReconstructioner();
  if (rrec == 0x0)
   {
     return 0x0;
   }
  //return rrec->GetListOfTasks()->GetObjectRef(Reconstructioner());
  Fatal("DigitizerRef", "Check where this is needed") ; 
  return 0 ; 
}
/*****************************************************************************/ 

TObject** AliLoader::TrackerRef()
{
   TTask* rrec = AliRunLoader::GetRunTracker();
   if (rrec == 0x0)
    {
      return 0x0;
    }
   //return rrec->GetListOfTasks()->GetObjectRef(Tracker());
   Fatal("DigitizerRef", "Check where this is needed") ; 
   return 0 ; 
}

/*****************************************************************************/ 
void AliLoader::CleanFolders()
 {
 //Cleans all posted objects == removes from folders and deletes them
 
  if (GetDebug()) Info("CleanFolders","name = %s Hits",GetName());
  CleanHits();
  if (GetDebug()) Info("CleanFolders","name = %s SDigits",GetName());
  CleanSDigits();
  if (GetDebug()) Info("CleanFolders","name = %s Digits",GetName());
  CleanDigits();
  if (GetDebug()) Info("CleanFolders","name = %s RecPoints",GetName());
  CleanRecPoints();
  if (GetDebug()) Info("CleanFolders","name = %s Tracks",GetName());
  CleanTracks();
  if (GetDebug()) Info("CleanFolders","name = %s Done",GetName());
  
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

void AliLoader::CleanSDigitizer(const TString name)
{
//removes and deletes detector task from Run Task
  TTask* task = AliRunLoader::GetRunSDigitizer();
  if (task == 0x0)
   {
     Error("CleanSDigitizer","Can not get RunSDigitizer from folder. Can not clean");
     return;
   }

  if (GetDebug()) Info("CleanSDigitizer","Attempting to delete S Digitizer");
  delete task->GetListOfTasks()->Remove(SDigitizer(name)); //TTList::Remove does not delete object
}
/*****************************************************************************/ 

void AliLoader::CleanDigitizer(const TString name)
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
         task->GetListOfTasks()->Remove(Digitizer(name)));
  delete task->GetListOfTasks()->Remove(Digitizer(name)); //TTList::Remove does not delete object
}
/*****************************************************************************/ 

void AliLoader::CleanReconstructioner(const TString name)
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
  delete task->GetListOfTasks()->Remove(Reconstructioner(name)); //TTList::Remove does not delete object
}
/*****************************************************************************/ 

void AliLoader::CleanTracker(const TString name)
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
         task->GetListOfTasks()->Remove(Tracker(name)));
  delete task->GetListOfTasks()->Remove(Tracker(name)); //TTList::Remove does not delete object
}
/*****************************************************************************/ 

TTree* AliLoader::Tree(AliLoaderDataInfo& di)
{
  //Get a data container from the detector data folder
  TObject* obj = GetDetectorDataFolder()->FindObject(di.ContainerName());
  return (obj)?dynamic_cast<TTree*>(obj):0x0;
}
/*****************************************************************************/ 

void AliLoader::SetCompressionLevel(Int_t cl)
{
//Sets compression level in all opened files
 for (Int_t i = 0; i < kNDataTypes; i++)
  SetCompressionLevel(fDataInfo[i],cl);
}
/*****************************************************************************/ 

Int_t  AliLoader::ReloadData(AliLoaderDataInfo& di)
 {
  //Unloads and loads data again
   if ( di.File() == 0x0 ) return 0;
   UnloadData(di);
   return LoadData(di,di.FileOption());
 }
/*****************************************************************************/ 

Int_t AliLoader::ReloadAll()
{
  return ( ReloadHits() && ReloadSDigits() && ReloadDigits() &&
           ReloadRecPoints() && ReloadTracks() );
}
/*****************************************************************************/ 

void AliLoader::CloseFiles()
{
 CloseHitsFile();
 CloseSDigitsFile();
 CloseDigitsFile();
 CloseRecPointsFile();
 CloseTracksFile();
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
 
 return 0;
}//sets the event folder
/*****************************************************************************/ 

Int_t AliLoader::Register()
{
//triggers creation of subfolders for a given detector
 if (GetDebug()) Info("Register","Name is %s.",GetName());
 if (fEventFolder == 0x0)
  {
    Error("Register","Event folder is not set");
    return 1;
  }
 Int_t retval = AliConfig::Instance()->AddDetector(fEventFolder,fDetectorName,fDetectorName);
 if(retval)
  {
    Error("SetEventFolder","Can not create tasks and/or folders for %s. Event folder name is %s",
          fDetectorName.Data(),fEventFolder->GetName());
    return retval;
  }
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
  
  FileName(kHits) = dirname + "/" + FileName(kHits);
  FileName(kSDigits) = dirname + "/" + FileName(kSDigits);
  FileName(kDigits) = dirname + "/" + FileName(kDigits);
  FileName(kRecPoints) = dirname + "/" + FileName(kRecPoints);
  FileName(kTracks) = dirname + "/" + FileName(kTracks);
}

/*****************************************************************************/ 
const TString AliLoader::SetFileOffset(const TString& fname)
{

//return fname;
  Long_t offset = (Long_t)GetRunLoader()->GetFileOffset();
  if (offset < 1) return fname;

  TString soffset;
  soffset += offset;//automatic conversion to string
  TString dotroot(".root");
  const TString& offfsetdotroot = offset + dotroot;
  TString out = fname;
  out = out.ReplaceAll(dotroot,offfsetdotroot);
  if (GetDebug()) Info("SetFileOffset","in=%s  out=%s.",fname.Data(),out.Data());
  return out;

}
/*****************************************************************************/ 

void AliLoader::SetDigitsFileNameSuffix(const TString& suffix)
{
  //adds the suffix before ".root", 
  //e.g. TPC.Digits.root -> TPC.DigitsMerged.root
  //made on Jiri Chudoba demand
  if (GetDebug()) 
   Info("SetDigitsFileNameSuffix","suffix=%s",suffix.Data());
  if (GetDebug()) 
   Info("SetDigitsFileNameSuffix","   Digits File Name before: %s",FileName(kDigits).Data());
   
  TString dotroot(".root");
  const TString& suffixdotroot = suffix + dotroot;
  FileName(kDigits) = FileName(kDigits).ReplaceAll(dotroot,suffixdotroot);
  if (GetDebug()) 
    Info("SetDigitsFileNameSuffix","                    after : %s",FileName(kDigits).Data());
}
/*****************************************************************************/ 

Bool_t AliLoader::CheckReload(const TFile* file, const TString& basefilename)
{
//checks if we have to reload given file
 if (file == 0x0) return kFALSE;
 TString tmp = SetFileOffset(basefilename);
 if (tmp.CompareTo(file->GetName())) return kTRUE;  //file must be reloaded
 return  kFALSE;
}
/*****************************************************************************/ 

void AliLoader::SetFileOption(AliLoaderDataInfo& di, Option_t* newopt)
{
  //sets file option for data defined by di
  if (di.FileOption().CompareTo(newopt) == 0) return;
  di.FileOption() = newopt;
  ReloadData(di);
}
/*****************************************************************************/ 

void AliLoader::SetCompressionLevel(AliLoaderDataInfo& di,Int_t cl)
{
//sets comression level for data defined by di
  di.CompressionLevel() = cl;
  if (di.File()) di.File()->SetCompressionLevel(cl);
}

/*****************************************************************************/ 
/*****************************************************************************/ 
/*****************************************************************************/ 

ClassImp(AliLoaderDataInfo)
//__________________________________________
////////////////////////////////////////////
//                                        //
//  class AliLoaderDataInfo               //
//                                        //
//  Container of all data needed for full //
//  description of each data type         //
//  (Hits, Kine, ...)                     //
//                                        //
////////////////////////////////////////////


AliLoaderDataInfo::AliLoaderDataInfo():
 fFileName(0),
 fFile(0x0),
 fDirectory(0x0),
 fFileOption(0),
 fContainerName(0),
 fName(0),
 fCompressionLevel(2)
{
  
}
/*****************************************************************************/ 

AliLoaderDataInfo::AliLoaderDataInfo
     (const char* filename, const char* contname, const char* name):
 fFileName(filename),
 fFile(0x0),
 fDirectory(0x0),
 fFileOption(0),
 fContainerName(contname),
 fName(name),
 fCompressionLevel(2)
{
}
