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


const TString AliLoader::fgkDefaultHitsContainerName("TreeH");
const TString AliLoader::fgkDefaultDigitsContainerName = "TreeD";
const TString AliLoader::fgkDefaultSDigitsContainerName = "TreeS";
const TString AliLoader::fgkDefaultRecPointsContainerName = "TreeR";
const TString AliLoader::fgkDefaultTracksContainerName = "TreeT";
const TString AliLoader::fgkLoaderBaseName("Loader");

ClassImp(AliLoader)
 
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
   Info("AliLoader(const Char_t* detname,const Char_t* eventfoldername)",
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

  fDataInfo[kHits].fFileName = fDetectorName + ".Hits.root";
  fDataInfo[kSDigits].fFileName = fDetectorName + ".SDigits.root";
  fDataInfo[kDigits].fFileName = fDetectorName + ".Digits.root";
  fDataInfo[kRecPoints].fFileName = fDetectorName + ".RecPoints.root";
  fDataInfo[kTracks].fFileName = fDetectorName + ".Tracks.root";
  
  fDataInfo[kHits].fContainerName = fgkDefaultHitsContainerName;
  fDataInfo[kSDigits].fContainerName = fgkDefaultSDigitsContainerName;
  fDataInfo[kDigits].fContainerName = fgkDefaultDigitsContainerName;
  fDataInfo[kRecPoints].fContainerName = fgkDefaultRecPointsContainerName;
  fDataInfo[kTracks].fContainerName = fgkDefaultTracksContainerName;

  fDataInfo[kHits].fName = "Hits";
  fDataInfo[kSDigits].fName = "Summable Digits";
  fDataInfo[kDigits].fName = "Digits";
  fDataInfo[kRecPoints].fName = "Reconstructed Points";
  fDataInfo[kTracks].fName = "Tracks";

}
/*****************************************************************************/ 

Int_t AliLoader::LoadData(EDataTypes dt,Option_t* opt)
 {
  Info("LoadData","name=%s, data type = %s, option = %s",GetName(),DataName(dt).Data(),opt);
  if (Tree(dt))
   {
    TString sopt(opt);
    if( sopt.Contains("force",TString::kIgnoreCase) != 0)
     {
       Warning("LoadData","Data <<%s>> are already loaded. Use FORCE option to force reload. Nothing done",DataName(dt).Data());
       return 0;
     }
    //else force option was used
   }
  SetFileOption(dt,opt);

  Int_t retval;
  retval = OpenDataFile(dt,opt);
  if (retval) 
   {
    Error("LoadData","Error occured while opening <<%s>> file",DataName(dt).Data());
    return retval;
   }

  //if file is recreated there is no sense to search for data to post and get Error message
  if (AliLoader::TestFileOption(opt) == kFALSE)
   {
    MakeTree(dt);
    return 0;
   }

  retval = PostData(dt);
  if (retval)
   {
    Error("LoadData","Error occured while posting %s from file to folder",DataName(dt).Data());
    return retval;
   }

  return 0;
 }
/*****************************************************************************/ 

Int_t AliLoader::LoadSDigitizer(Option_t* opt)
{
  if (SDigitizer())
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
  
  retval = PostSDigitizer();      
  if (retval) 
   {
    Error("LoadSDigits","Error occured while posting SDigits from file to folder");
    return retval;
   }
  return 0;

}
/*****************************************************************************/ 

Int_t AliLoader::LoadDigitizer(Option_t* opt)
{
  if (Digitizer())
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
  
  retval = PostDigitizer();      
  if (retval) 
   {
    Error("LoadDigitizer","Error occured while posting Digitizer from file to task");
    return retval;
   }
  return 0;

}
/*****************************************************************************/ 

Int_t AliLoader::LoadReconstructioner(Option_t* opt)
{
   if (Reconstructioner())
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
  
  retval = PostReconstructioner();
  if (retval) 
   {
    Error("LoadReconstructioner","Error occured while posting Reconstructioner from file to Task");
    return retval;
   }
  return 0;

}
/*****************************************************************************/ 

Int_t AliLoader::LoadTracker(Option_t* opt)
{
   if (Tracker())
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
  
  retval = PostReconstructioner();
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
         Info("SetEvent","Reloading new file for hits. file opt is %s",FileOption(kHits).Data());
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
         Info("SetEvent","Reloading new file for sdigits. file opt is %s",FileOption(kSDigits).Data());
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

Int_t AliLoader::GetEvent()
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
   
   if(File(kHits)) PostHits();
   if(File(kSDigits)) PostSDigits();
   if(File(kDigits)) PostDigits();
   if(File(kRecPoints)) PostRecPoints();
   if(File(kTracks)) PostTracks();
 
   return 0;
 
 }
/******************************************************************/
Int_t AliLoader::WriteData(EDataTypes dt,Option_t* opt)
{
//assumes that data is tree
  TTree *data = dynamic_cast<TTree*>(GetDetectorDataFolder()->FindObject(ContainerName(dt)));
  if(data == 0x0)
   {//did not get, nothing to write
     Warning("WriteData","%s object named %s not found in folder %s.\nNothing to write. Returning",
              DataName(dt).Data(),ContainerName(dt).Data(), 
              GetDataFolder()->FindFullPathName(GetDetectorDataFolder()->GetName()));
     return 0;
   }
  
  //check if file is opened
  if (Directory(dt) == 0x0)
   { 
     //if not open, try to open
     SetFileOption(dt,"UPDATE");
     if (OpenDataFile(dt,"UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteHits","Can not open hits file. HITS ARE NOT WRITTEN");
        return 1;
      }
   }

  if (File(dt)->IsWritable() == kFALSE)
   {
     Error("WriteHits","File %s is not writable",File(dt)->GetName());
     return 2;
   }
  
  Directory(dt)->cd(); //set the proper directory active

  //see if hits container already exists in this (root) directory
  TObject* obj = Directory(dt)->Get(ContainerName(dt));
  if (obj)
   { //if they exist, see if option OVERWRITE is used
     const char *oOverWrite = strstr(opt,"OVERWRITE");
     if(!oOverWrite)
      {//if it is not used -  give an error message and return an error code
        Error("WriteData","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  
  Info("WriteData","name = %s, DataName = %s, opt = %s, data object name = %s",
                   GetName(),DataName(dt).Data(),opt,data->GetName());
  Info("WriteData","File Name = %s, Directory Name = %s Directory's File Name = %s",
                   File(dt)->GetName(),Directory(dt)->GetName(),
                   Directory(dt)->GetFile()->GetName());

  data->SetDirectory(Directory(dt)); //forces setting the directory to this directory (we changed dir few lines above)
  
  Info("WriteData","Writing tree");
  data->Write(0,TObject::kOverwrite);
  Info("WriteData","Writing File");
  File(dt)->Write(0,TObject::kOverwrite);
  return 0;

}
/*****************************************************************************/ 
Int_t  AliLoader::WriteSDigitizer(Option_t* opt)
{
//Writes SDigitizer to detector Summbale Digits file/current event ROOT directory
//Reads it from "ROOT://fgkTopFolderName/fgkTasksFolderName/fgkSDigitizerTaskName/fDetName/'fDetName+SDigitizer'
//  TTask *sdgzer = SDigitizer();
  
  TTask* sder = SDigitizer(); 
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
  TString name(fDetectorName + AliConfig::Instance()->GetSDigitizerTaskName());

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
  Info("WriteSDigitizer",
       "name = %s, opt = %s, File(kSDigits) = %s, Sum. digitizer = %s",
       GetName(),opt,File(kSDigits)->GetName(),name.Data());

  Directory(kSDigits)->cd();
  sder->Write(0,TObject::kOverwrite);
  return 0;
}
/*****************************************************************************/ 

Int_t  AliLoader::WriteDigitizer(Option_t* opt)
{
//Writes Digitizer to detector "Digits file"/"current event ROOT directory"
//Reads it from "ROOT://fgkTopFolderName/fgkTasksFolderName/fgkDigitizerTaskName/fDetName/'fDetName+Digitizer'
  TTask* sder = Digitizer(); 
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
  TString name(fDetectorName + AliConfig::Instance()->GetDigitizerTaskName());

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
  Info("WriteDigitizer",
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

  Info("WriteTracker",
       "name = %s, opt = %s, File(kTracks) = %s, Directory(kTracks) =%s, Reconstructioner = %s",
       GetName(),opt,File(kTracks)->GetName(),Directory(kTracks)->GetName(),name.Data());

  Directory(kTracks)->cd();
  sder->Write(0,TObject::kOverwrite);
  return 0;
}
/*****************************************************************************/ 

Int_t AliLoader::OpenDataFile(const TString& filename,TFile*& file,TDirectory*& dir,Option_t* opt)
{
//Opens file named 'filename', and assigns pointer to it to 'file'
//jumps to directory corresponding to current event and stores the pointer to it in 'dir'
//option 'opt' is passed to TFile::Open
  if (file)
   {
     if(file->IsOpen() == kTRUE)
       {
         Warning("OpenDataFile>"," File %s already opened. First close it.",file->GetName());
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
     fQAFolder = dynamic_cast<TFolder*>(GetEventFolder()->FindObjectAny(AliConfig::Instance()->GetQAFolderName()));

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

  TTask* rsd = AliRunLoader::GetRunSDigitizer();
   if (rsd == 0x0)
    {
      return 0x0;
    }
  TString name(fDetectorName + AliConfig::Instance()->GetSDigitizerTaskName());
  TObject* obj = rsd->GetListOfTasks()->FindObject(name);
  return (obj)?dynamic_cast<TTask*>(obj):0x0;
}
/*****************************************************************************/ 

AliDigitizer* AliLoader::Digitizer()
{
//returns Digitizer task for this detector
  TTask* rd = AliRunLoader::GetRunDigitizer();
   if (rd == 0x0)
    {
      return 0x0;
    }
  TString name(fDetectorName + AliConfig::Instance()->GetDigitizerTaskName());
  TObject* obj = rd->GetListOfTasks()->FindObject(name);
  return (obj)?dynamic_cast<AliDigitizer*>(obj):0x0;
}
/*****************************************************************************/ 

TTask* AliLoader::Reconstructioner()
{
//returns Recontructioner (Cluster Finder, Cluster Maker, 
//or whatever you want to call it) task for this detector
  TTask* rrec = AliRunLoader::GetRunReconstructioner();
  if (rrec == 0x0)
    {
      return 0x0;
    }
  TString name(fDetectorName + AliConfig::Instance()->GetReconstructionerTaskName());
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

TTask* AliLoader::Tracker()
{
  TTask* tracker = AliRunLoader::GetRunTracker();
  if (tracker == 0x0)
    {
      return 0x0;
    }
  TString name(fDetectorName + AliConfig::Instance()->GetTrackerTaskName());
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
 ::Info("AliLoader::ChangeDir","Changing Dir to %s in file %s.",dirname.Data(),file->GetName());

 Bool_t result;
 
 TDirectory* dir = dynamic_cast<TDirectory*>(file->Get(dirname));

 if (dir == 0x0)
  {
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
void AliLoader::MakeTree(EDataTypes dt)
{
//this virtual method creates the tree for hits in the file
  if (Tree(dt)) return;//tree already made 
  Info("MakeTree","name = %s, Making %s Tree named %s.",
        GetName(),DataName(dt).Data(),ContainerName(dt).Data());
   
  TTree* tree = new TTree(ContainerName(dt), DataName(dt) + "Container"); //make a tree
  if (tree == 0x0)
   {
     Error("MakeTree","Can not create %s Container for %s",DataName(dt).Data(),GetName());
     return;
   }
  tree->SetAutoSave(1000000000); //no autosave
  GetDetectorDataFolder()->Add(tree);
  WriteData(dt,"OVERWRITE");//write tree to the file
}

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
//Make standard container for digits
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
Int_t AliLoader::PostData(EDataTypes dt)
{
//Posts the SDigits container to proper folder
//  Warning("PostSDigits","Called");

  TDirectory *dir = Directory(dt);
  if ( dir == 0x0)
   {
     Error("PostData","%s Directory is NULL. Load %s before.",
           DataName(dt).Data(),DataName(dt).Data());
     return 2; 
   }

  TObject* tree = dir->Get(ContainerName(dt));
  if(tree)
   {
     //if such an obejct already exists - remove it first
     TObject* obj = GetDetectorDataFolder()->FindObject(ContainerName(dt));
     if (obj)
      {
        Warning("PostData","Object named %s already exitsts in %s data folder. Removing it",
                 ContainerName(dt).Data(), fDetectorName.Data());
        Clean(dt);
      }
     GetDetectorDataFolder()->Add(tree);
     return 0;
   }
  else
   {
   
    Warning("PostData","Can not find %s in file %s",
             ContainerName(dt).Data(),File(dt)->GetName());
   }
  return 0;
}


/*****************************************************************************/ 

Int_t AliLoader::PostSDigitizer()
 {
  
   if (Directory(kSDigits) == 0x0)
    {
     Error("PostSDigitizer","SDigitsDir is Null. Probobly SDigits not loaded");
     return 1;
    }
   
   TString name(fDetectorName + AliConfig::Instance()->GetSDigitizerTaskName());
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
  Info("PostSDigitizer","Adding S Digitizer named %s.",sdzer->GetName());
  rsd->Add(sdzer);
  return 0;
}

/*****************************************************************************/ 

Int_t AliLoader::PostDigitizer()
 {
   if (Directory(kDigits) == 0x0)
    {
     Error("PostDigitizer","Directory(kDigits) is Null. Probobly Digits not loaded");
     return 1;
    }
   
   TString name(fDetectorName + AliConfig::Instance()->GetDigitizerTaskName());
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

Int_t AliLoader::PostReconstructioner()
 {
   if ( Directory(kRecPoints) == 0x0)
    {
     Error("PostReconstructioner","RecPointsDir is Null. Probobly RecPoints not loaded");
     return 1;
    }
   
   TString name(fDetectorName + AliConfig::Instance()->GetReconstructionerTaskName());
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
     Error("PostReconstructioner","There already exists sub task named %s in Run Reconstructioner",task->GetName());
     return 2;
   }
  rrec->Add(task);
  return 0;
 }
/*****************************************************************************/ 

Int_t AliLoader::PostTracker()
 {
   if (Directory(kTracks) == 0x0)
    {
     Error("PostTracker","Directory(kTracks) is Null. Probobly Tracks or Tracker not loaded");
     return 1;
    }
   
   TString name(fDetectorName + AliConfig::Instance()->GetTrackerTaskName());
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
 
  Info("CleanFolders","name = %s Hits",GetName());
  CleanHits();
  Info("CleanFolders","name = %s SDigits",GetName());
  CleanSDigits();
  Info("CleanFolders","name = %s Digits",GetName());
  CleanDigits();
  Info("CleanFolders","name = %s RecPoints",GetName());
  CleanRecPoints();
  Info("CleanFolders","name = %s Tracks",GetName());
  CleanTracks();
  Info("CleanFolders","name = %s Done",GetName());
  
 }
/*****************************************************************************/ 

void AliLoader::Clean(const TString& name)
{
  TObject* obj = GetDetectorDataFolder()->FindObject(name);
  if(obj)
   { 
     Info("Clean(const TString&)","name=%s, cleaning %s.",GetName(),name.Data());
     GetDetectorDataFolder()->Remove(obj);
     delete obj;
   }
}
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

  Info("CleanSDigitizer","Attempting to delete S Digitizer");
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

  Info("CleanDigitizer","Attempting to delete Digitizer %x",task->GetListOfTasks()->Remove(Digitizer()));
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

  Info("CleanTracker","Attempting to delete Tracker %x",task->GetListOfTasks()->Remove(Tracker()));
  delete task->GetListOfTasks()->Remove(Tracker()); //TTList::Remove does not delete object
}
/*****************************************************************************/ 

TTree* AliLoader::Tree(EDataTypes dt)
{
  //Get a hits container from the detector data folder
  TObject* obj = GetDetectorDataFolder()->FindObject(ContainerName(dt));
  if (obj)
   return dynamic_cast<TTree*>(obj);
  else return 0x0;
}

/*****************************************************************************/ 

TTree* AliLoader::TreeH()
{
  //Get a hits container from the detector data folder
  return Tree(kHits);
}
/*****************************************************************************/ 
TTree* AliLoader::TreeS()
{
  //Get a hits container from the detector data folder
  return Tree(kSDigits);
}
/*****************************************************************************/ 
TTree* AliLoader::TreeD()
{
  //Get a hits container from the detector data folder
  return Tree(kDigits);
}
/*****************************************************************************/ 
TTree* AliLoader::TreeR()
{
  //Get a hits container from the detector data folder
  return Tree(kRecPoints);
}
/*****************************************************************************/ 

TTree* AliLoader::TreeT()
{
  //Get a hits container from the detector data folder
  return Tree(kTracks);
}
/*****************************************************************************/ 

void AliLoader::SetCompressionLevel(Int_t cl)
{
//Sets compression level in all opened files
 if (File(kHits)) File(kHits)->SetCompressionLevel(cl);
 if (File(kSDigits)) File(kSDigits)->SetCompressionLevel(cl);
 if (File(kDigits)) File(kDigits)->SetCompressionLevel(cl);
 if (File(kRecPoints)) File(kRecPoints)->SetCompressionLevel(cl);
 if (File(kTracks)) File(kTracks)->SetCompressionLevel(cl);
}
/*****************************************************************************/ 
Int_t  AliLoader::ReloadData(EDataTypes dt)
 {
   if ( File(dt) == 0x0 ) return 0;
   UnloadData(dt);
   return LoadData(dt,FileOption(dt));
 }
/*****************************************************************************/ 

Int_t  AliLoader::ReloadHits()
{
 return ReloadData(kHits);
}
/*****************************************************************************/ 

Int_t  AliLoader::ReloadSDigits()
{
 return ReloadData(kSDigits);
}
/*****************************************************************************/ 

Int_t  AliLoader::ReloadDigits()
{
 return ReloadData(kDigits);
}
/*****************************************************************************/ 

Int_t  AliLoader::ReloadRecPoints()
{
 return ReloadData(kRecPoints);
}
/*****************************************************************************/ 

Int_t  AliLoader::ReloadTracks()
{
 return ReloadData(kTracks);
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
 Info("Register","Name is %s.",GetName());
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
  if (option.Contains("recreate",TString::kIgnoreCase)) return kFALSE;
  if (option.Contains("new",TString::kIgnoreCase)) return kFALSE;
  if (option.Contains("create",TString::kIgnoreCase)) return kFALSE;
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
  Info("SetFileOffset","in=%s  out=%s.",fname.Data(),out.Data());
  return out;

}
/*****************************************************************************/ 

void AliLoader::SetDigitsFileNameSuffix(const TString& suffix)
{
  //adds the suffix before ".root", 
  //e.g. TPC.Digits.root -> TPC.DigitsMerged.root
  //made on Jiri Chudoba demand
  Info("SetDigitsFileNameSuffix","suffix=%s",suffix.Data());
  Info("SetDigitsFileNameSuffix","   Digits File Name before: %s",FileName(kDigits).Data());
  TString dotroot(".root");
  const TString& suffixdotroot = suffix + dotroot;
  FileName(kDigits) = FileName(kDigits).ReplaceAll(dotroot,suffixdotroot);
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

void AliLoader::SetFileOption(EDataTypes dt,Option_t* newopt)
{
  if (FileOption(dt).CompareTo(newopt) == 0) return;
  FileOption(dt) = newopt;
  ReloadData(dt);
}
void AliLoader::SetHitsFileOption(Option_t* newopt)
{
//Hits File Option in open
  SetFileOption(kHits,newopt);
}
/*****************************************************************************/ 

void AliLoader::SetSDigitsFileOption(Option_t* newopt)
{
//SDigits File Option in open
  SetFileOption(kSDigits,newopt);
}
/*****************************************************************************/ 

void AliLoader::SetDigitsFileOption(Option_t* newopt)
{
//Digits File Option in open
  SetFileOption(kDigits,newopt);
}
/*****************************************************************************/ 

void AliLoader::SetRecPointsFileOption(Option_t* newopt)
{
//RecPoints File Option in open
  SetFileOption(kRecPoints,newopt);
}
/*****************************************************************************/ 

void AliLoader::SetTracksFileOption(Option_t* newopt)
{
//Tracks File Option in open
  SetFileOption(kTracks,newopt);
}
/*****************************************************************************/ 
/*****************************************************************************/ 
/*****************************************************************************/ 

ClassImp(AliLoaderDataInfo)

AliLoaderDataInfo::AliLoaderDataInfo():
 fFileName(0),
 fFile(0x0),
 fDirectory(0x0),
 fFileOption(0),
 fContainerName(0),
 fName(0)
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
 fName(name)
{
}
