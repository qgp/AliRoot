#include "AliRunLoader.h"

//
//This class aims to be the only one interface for manging data
//It stores Loaders for all modules which knows the filenames 
//of the data files to be stored.
//It aims to substitude AliRun in automatic maging of data positioning
//thus there won't be necessity of loading gAlice from file in order to 
//get fast ccess to the data
//
//logical place for putting the Loader specific to the given detector is detector itself
// but, to load detector one need to load gAlice, and by the way all other detectors
// with their geometrieces and so on. 
// So, if one need to open TPC clusters there is no principal nedd to read everything
//
// When RunLoader is read from the file it does not connect to the folder structure
// it must be connected (mounted) manualy in the macro or class code. 
// Default event folder is defined by AliConfig::fgkDefaultEventFolderName
// but can be mounted elsewhere. Usefull specially in merging, when more than 
//

/**************************************************************************/

#include <TString.h>
#include <TFolder.h>
#include <TFile.h>
#include <TTree.h>
#include <TROOT.h>
#include <TBranch.h>
#include <TGeometry.h>
#include <TTask.h>
#include <TError.h>

#include "AliRun.h"
#include "AliConfig.h"
#include "AliLoader.h"
#include "AliHeader.h"
#include "AliStack.h"
#include "TObjArray.h"
#include "AliDetector.h"
#include "AliRunDigitizer.h"

TDirectory * gKineDir = 0x0;

ClassImp(AliRunLoader)

const TString AliRunLoader::fgkRunLoaderName("RunLoader");

const TString AliRunLoader::fgkHeaderBranchName("Header");
const TString AliRunLoader::fgkHeaderContainerName("TE");
const TString AliRunLoader::fgkKineContainerName("TreeK");
const TString AliRunLoader::fgkTrackRefsContainerName("TreeTR");
const TString AliRunLoader::fgkKineBranchName("Particles");
const TString AliRunLoader::fgkDefaultKineFileName("Kinematics.root");
const TString AliRunLoader::fgkDefaultTrackRefsFileName("TrackRefs.root");
const TString AliRunLoader::fgkGAliceName("gAlice");
/**************************************************************************/

AliRunLoader::AliRunLoader():
 fLoaders(0x0),
 fEventFolder(0x0),
 fCurrentEvent(0),
 fGAFile(0x0),
 fHeader(0x0),
 fStack(0x0),
 fKineData(),
 fTrackRefsData(),
 fNEventsPerFile(1)
{
  AliConfig::Instance();//force to build the folder structure
}
/**************************************************************************/

AliRunLoader::AliRunLoader(const char* eventfoldername):
 TNamed(fgkRunLoaderName,fgkRunLoaderName),
 fLoaders(new TObjArray()),
 fEventFolder(0x0),
 fCurrentEvent(0),
 fGAFile(0x0),
 fHeader(0x0),
 fStack(0x0),
 fKineData(fgkDefaultKineFileName,fgkKineContainerName,"Kinematics"),
 fTrackRefsData(fgkDefaultTrackRefsFileName,fgkTrackRefsContainerName,"Track References"),
 fNEventsPerFile(1)
{
//ctor
  SetEventFolderName(eventfoldername);
}
/**************************************************************************/

AliRunLoader::~AliRunLoader()
{
  
  UnloadHeader();
  UnloadKinematics();
  UnloadTrackRefs();
  UnloadgAlice();
  
  if(fLoaders) {
    fLoaders->SetOwner();
    delete fLoaders;
  }

  RemoveEventFolder();
  //fEventFolder is deleted by the way of removing - TopAliceFolder owns it
  delete fHeader;
  delete fStack;
  delete fGAFile;
  delete fKineData.File();
  delete fTrackRefsData.File();
}
/**************************************************************************/

AliRunLoader::AliRunLoader(TFolder* topfolder):TNamed(fgkRunLoaderName,fgkRunLoaderName)
{
 if(topfolder == 0x0)
  {
    Fatal("AliRunLoader(TFolder*)","Parameter is NULL");
    return;
  }
 fEventFolder = topfolder;
 
 TObject* obj = fEventFolder->FindObject(fgkRunLoaderName);
 if (obj)
  { //if it is, then sth. is going wrong... exits aliroot session
    Fatal("AliRunLoader(const char*)",
          "In Event Folder Named %s object named %s already exists. I am confused ...",
           fEventFolder->GetName(),fgkRunLoaderName.Data());
    return;//never reached
  }
   
 fLoaders = new TObjArray();
 fEventFolder->Add(this);//put myself to the folder to accessible for all
  
}
/**************************************************************************/

Int_t AliRunLoader::GetEvent(Int_t evno)
{
//Gets event number evno
//Reloads all data properly
  if (fCurrentEvent == evno) return 0;
  
  if (evno < 0)
   {
     Error("GetEvent","Can not give the event with negative number");
     return 4;
   }

  if (evno >= GetNumberOfEvents())
   {
     Error("GetEvent","There is no event with number %d",evno);
     return 3;
   }
  
  if (GetDebug()) Info("GetEvent",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  if (GetDebug()) Info("GetEvent",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  if (GetDebug()) Info("GetEvent",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  if (GetDebug()) Info("GetEvent","          GETTING EVENT  %d",evno);
  if (GetDebug()) Info("GetEvent",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  if (GetDebug()) Info("GetEvent",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  if (GetDebug()) Info("GetEvent",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  fCurrentEvent = evno;

  Int_t retval;
  
  //Reload header (If header was loaded)
  if (GetHeader())
   {
     retval = TreeE()->GetEvent(fCurrentEvent);
     if ( retval == 0)
      {
        Error("GetEvent","Cannot find event: %d\n ",fCurrentEvent);
        return 5;
      }
   }
  //Reload stack (If header was loaded)
  if (TreeE()) fStack = GetHeader()->Stack();
  //Set event folder in stack (it does not mean that we read kinematics from file)
  if (fStack) 
   { 
     fStack->SetEventFolderName(fEventFolder->GetName());
   }
  else
   {
     Warning("GetEvent","Did not found stack in header");
   }
  
  retval = SetEvent();
  if (retval)
   {
     Error("GetEvent","Error occured while setting event %d",evno);
     return 1;
   }
   
  //Post Track References
  if (fTrackRefsData.File()) 
   {
    retval = PostTrackRefs();
   }

  //Read Kinematics if loaded
  if (fKineData.File()) 
   {
    retval = PostKinematics();
    if (fStack) fStack->GetEvent();
   }
  
  if (retval)
   {
     Error("GetEvent","Error occured while posting kinematics. Event %d",evno);
     return 2;
   }
  
  //Trigger data reloading in all loaders 
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next())) 
   {
     retval = loader->GetEvent();
     if (retval)
      {
       Error("GetEvent","Error occured while getting event for %s. Event %d.",
              loader->GetDetectorName().Data(), evno);
       return 3;
      }
   }
  return 0;
}
/**************************************************************************/

Int_t AliRunLoader::SetEventNumber(Int_t evno)
{
  //cleans folders and sets the root dirs in files 
  if (fCurrentEvent == evno) return 0;
  fCurrentEvent = evno;
  return SetEvent();
}
/**************************************************************************/
Int_t AliRunLoader::SetEvent()
{
 //if kinematocs was loaded Cleans folder data
 //change 

  TString tmp;
  
  //KINEMATICS
  if (TreeK()) CleanKinematics();
  if (fKineData.File())//if kinematics was loaded
   { 
     tmp = SetFileOffset(fKineData.FileName());
     if (tmp.CompareTo(fKineData.File()->GetName()) != 0) //check if we have to change the file
      { 
        if (GetDebug()) Info("SetEvent"," Reloading Kine: ev number %d Old filename: %s  option: %s new filename: %s",
              fCurrentEvent,fKineData.File()->GetName(),fKineData.FileOption().Data(),tmp.Data());
        
        UnloadKinematics();
        OpenKineFile(fKineData.FileOption());
      }
     //change directory to new event
     fKineData.Directory() = AliLoader::ChangeDir(fKineData.File(),fCurrentEvent);
     if (fKineData.Directory() == 0x0)
      {
        Error("SetEvent","Can not change to root directory in file %s",fKineData.File()->GetName());
        return 1;
      }
   }
  
  //TRACK REFERNCES
  if (TreeTR()) CleanTrackRefs();
  if (fTrackRefsData.File())
   {  
     tmp = SetFileOffset(fTrackRefsData.FileName());
     if (tmp.CompareTo(fTrackRefsData.File()->GetName()) != 0)
      { 
        UnloadTrackRefs();
        OpenTrackRefsFile(fTrackRefsData.FileOption());
      }

     fTrackRefsData.Directory() = AliLoader::ChangeDir(fTrackRefsData.File(),fCurrentEvent);
     if (fTrackRefsData.Directory() == 0x0)
      {
        Error("SetEvent","Can not change to root directory in file %s",fTrackRefsData.File()->GetName());
        return 2;
      }
   }

  
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next())) 
   {
     loader->SetEvent();
   }

  return 0;
}

/**************************************************************************/
AliRunLoader* AliRunLoader::Open
  (const char* filename, const char* eventfoldername, Option_t* option)
{
//Opens a desired file 'filename'
//gets the the run-Loader and mounts it desired folder
//returns the pointer to run Loader which can be further used for accessing data 
//in case of error returns NULL
 
 static const TString webaddress("http://alisoft.cern.ch/people/skowron/codedoc/split/index.html");
 if (AliLoader::fgDebug) 
  ::Info("AliRunLoader::Open",
         "\n\n\nNew I/O strcture: See more info:\n %s\n\n\n",webaddress.Data());
 
 AliRunLoader* result = 0x0;
 
 /* ************************************************ */
 /* Chceck if folder with given name already exists  */
 /* ************************************************ */
 
 TObject* obj = AliConfig::Instance()->GetTopFolder()->FindObject(eventfoldername);
 if(obj)
  {
    TFolder* fold = dynamic_cast<TFolder*>(obj);
    if (fold == 0x0)
     {
      ::Error("AliRunLoader::Open","Such a obejct already exists in top alice folder and it is not a folder.");
      return 0x0;
     }
    
    //check if we can get RL from that folder
    result = AliRunLoader::GetRunLoader(eventfoldername);
    if (result == 0x0)
     {
       ::Error("AliRunLoader::Open",
               "Folder %s already exists, and can not find session there. Can not mount.",eventfoldername);
       return 0x0;
     }

    if (result->GetFileName().CompareTo(filename) != 0)
     {
       ::Error("AliRunLoader::Open","Other file is mounted in demanded folder. Can not mount.");
       return 0x0;
     }

    //check if now is demanded (re)creation 
    if ( AliLoader::TestFileOption(option) == kFALSE)
     {
       ::Error("AliRunLoader::Open",
               "Session already exists in folder %s and this session option is %s. Unable to proceed.",
                eventfoldername,option);
       return 0x0;
     }
     
    //check if demanded option is update and existing one 
    TString tmpstr(option);
    if ( (tmpstr.CompareTo("update",TString::kIgnoreCase) == 0) && 
         (result->fGAFile->IsWritable() == kFALSE) )
     { 
       ::Error("AliRunLoader::Open",
               "Session already exists in folder %s and is not writable while this session option is %s. Unable to proceed.",
                eventfoldername,option);
       return 0x0;
     }
     
    ::Warning("AliRunLoader::Open","Session is already opened and mounted in demanded folder");	
    return result;
  } //end of checking in case of existance of object named identically that folder session is being opened
 
 
 TFile * gAliceFile = TFile::Open(filename,option);//open a file
 if (!gAliceFile) 
  {//null pointer returned
    ::Error("AliRunLoader::Open","Can not open file %s.",filename);
    return 0x0;
  }
  
 if (gAliceFile->IsOpen() == kFALSE)
  {//pointer to valid object returned but file is not opened
    ::Error("AliRunLoader::Open","Can not open file %s.",filename);
    return 0x0;
  }
 
 //if file is "read" or "update" than we try to find AliRunLoader there - if not found cry and exit
 //else create new AliRunLoader
 if ( AliLoader::TestFileOption(option) )
  { 
    if (AliLoader::fgDebug) 
     ::Info("AliRunLoader::Open","Reading RL from file");
    
    result = dynamic_cast<AliRunLoader*>(gAliceFile->Get(fgkRunLoaderName));//get the run Loader from the file
    if (result == 0x0)
     {//didn't get
       ::Error("AliRunLoader::Open","Can not find run-Loader in file %s.",filename);
       delete gAliceFile;//close the file
       return 0x0;
     }
    Int_t tmp = result->SetEventFolderName(eventfoldername);//mount a event folder   
    if (tmp)//if SetEvent  returned error
     {
       ::Error("AliRunLoader::Open","Can not mount event in folder %s.",eventfoldername);
       delete result; //delete run-Loader
       delete gAliceFile;//close the file
       return 0x0;
     }
  }
 else
  {
    if (AliLoader::fgDebug) 
      ::Info("AliRunLoader::Open","Creating new AliRunLoader. Folder name is %s",eventfoldername);
    result = new AliRunLoader(eventfoldername);
  }
 
//procedure for extracting dir name from the file name 
 TString fname(filename);
 Int_t  nsl = fname.Last('/');//look for slash in file name
 TString dirname;
 if (nsl < 0) 
  {//slash not found
    Int_t  nsl = fname.Last(':');//look for colon e.g. rfio:galice.root
    if (nsl < 0) dirname = ".";//not found
    else dirname = fname.Remove(nsl);//found
  }
 else dirname = fname.Remove(nsl);//slash found
 
 if (AliLoader::fgDebug) 
  ::Info("AliRunLoader::Open","Dir name is : %s",dirname.Data());
 
 result->SetDirName(dirname); 
 result->SetGAliceFile(gAliceFile);//set the pointer to gAliceFile
 return result;
}
/**************************************************************************/
Int_t AliRunLoader::GetNumberOfEvents()
{
 //returns number of events in Run
 Int_t retval;
 if( TreeE() == 0x0 )
  {
    retval = LoadHeader();
    if (retval) 
     {
       Error("GetNumberOfEvents","Error occured while loading header");
       return -1;
     }
  }
 return (Int_t)TreeE()->GetEntries();
}

/**************************************************************************/
void AliRunLoader::MakeHeader()
{
 //Makes header and connects it to header tree (if it exists)
  if (GetDebug()) Info("MakeHeader","");
  if(fHeader == 0x0)
   {
     if (GetDebug()) Info("MakeHeader","Creating new Header Object");
     fHeader= new AliHeader();
   }
  TTree* tree = TreeE();
  if (tree)
   {
     if (GetDebug()) Info("MakeHeader","Got Tree from folder.");
     TBranch* branch = tree->GetBranch(fgkHeaderBranchName);
     if (branch == 0x0)
      {
        if (GetDebug()) Info("MakeHeader","Creating new branch");
        branch = tree->Branch(fgkHeaderBranchName, "AliHeader", &fHeader, 4000, 0);
        branch->SetAutoDelete(kFALSE);
      }
     else
      {
        if (GetDebug()) Info("MakeHeader","Got Branch from Tree");
        branch->SetAddress(&fHeader);
        tree->GetEvent(fCurrentEvent);
        fStack = fHeader->Stack(); //should be safe - if we created Stack, header returns pointer to the same object
        fStack->SetEventFolderName(fEventFolder->GetName());
        if (TreeK()) fStack->GetEvent();
      }
   } 
  if (GetDebug()) Info("MakeHeader","Exiting MakeHeader method");
}
/**************************************************************************/

void AliRunLoader::MakeStack()
{
//Creates the stack object -  do not connect the tree
  if(fStack == 0x0)
   { 
     fStack = new AliStack(10000);
     fStack->SetEventFolderName(fEventFolder->GetName());
   }
}

/**************************************************************************/

void AliRunLoader::MakeTree(Option_t *option)
{
//Creates trees
  const char *oK = strstr(option,"K"); //Kine  
  const char *oE = strstr(option,"E"); //Header

  TTree* tree;

  if(oK && !TreeK())
   { 
     if (fKineData.Directory() == 0x0)
      {
        Error("MakeTree(\"K\")","Load Kinematics first");
      }
     else
      {
        fKineData.Directory()->cd();
        tree = new TTree(fgkKineContainerName,"Kinematics");
        GetEventFolder()->Add(tree);
        MakeStack();
        fStack->ConnectTree();
        WriteKinematics("OVERWRITE");
     }
   }
  
  if(oE && !TreeE())
   {
     fGAFile->cd();
     tree = new TTree(fgkHeaderContainerName,"Tree with Headers");
     GetEventFolder()->Add(tree);
     MakeHeader();
     WriteHeader("OVERWRITE");
   }
  
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   {
    loader->MakeTree(option);
   }

}
/**************************************************************************/
    
Int_t AliRunLoader::LoadgAlice()
{
//Loads gAlice from file
 if (GetAliRun())
  {
    Warning("LoadgAlice","AliRun is already in folder. Unload first.");
    return 0;
  }
 AliRun* alirun = dynamic_cast<AliRun*>(fGAFile->Get(fgkGAliceName));
 if (alirun == 0x0)
  {
    Error("LoadgAlice"," Can not find gAlice in file %s",fGAFile->GetName());
    return 2;
  }
 alirun->SetRunLoader(this);
 if (gAlice)
  {
    Warning("LoadgAlice","gAlice already exists. Putting retrived object in folder named %s",
             GetEventFolder()->GetName());
  }
 else
  {
    gAlice = alirun;
  }

 return 0; 
}
/**************************************************************************/

Int_t AliRunLoader::LoadHeader()
{
 if (TreeE())
  {
     Warning("LoadHeader","Header is already loaded. Use ReloadHeader to force reload. Nothing done");
     return 0;
  }
 
 if (GetEventFolder() == 0x0)
  {
    Error("LoadHaeder","Event folder not specified yet");
    return 1;
  }

 if (fGAFile == 0x0)
  {
    Error("LoadHaeder","Session not opened. Use AliRunLoader::Open");
    return 2;
  }
 
 if (fGAFile->IsOpen() == kFALSE)
  {
    Error("LoadHaeder","Session not opened. Use AliRunLoader::Open");
    return 2;
  }

 TTree* tree = dynamic_cast<TTree*>(fGAFile->Get(fgkHeaderContainerName));
 if (tree == TreeE()) return 0;
 if (tree == 0x0)
  {
    Fatal("LoadHaeder","Can not find header tree named %s in file %s",
           fgkHeaderContainerName.Data(),fGAFile->GetName());
    return 2;
  }

 CleanHeader();
 GetEventFolder()->Add(tree);
 MakeHeader();//creates header object and connects to tree
 return 0; 

}
/**************************************************************************/

Int_t AliRunLoader::LoadKinematics(Option_t* option)
{
//Loads the kinematics 
 if (TreeK())
  {
     Warning("LoadKinematics","Kinematics is already loaded. Use ReloadKinematics to force reload. Nothing done");
     return 0;
  }

 if (GetEventFolder() == 0x0)
  {
    Error("LoadKinematics","Event folder not specified yet");
    return 1;
  }
 Int_t retval = OpenKineFile(option);
 if (retval)
   {
    Error("LoadKinematics","Error occured while opening Kine file");
    return retval;
   }

 fKineData.FileOption() = option;
  
 if (AliLoader::TestFileOption(option) == kFALSE) return 0;
 
 retval = PostKinematics();
 if (retval)
   {//propagate error up 
    Error("LoadKinematics","PostKinematics returned error");
    return retval;
   }
 if (fStack) fStack->GetEvent();
 return 0;
}
/**************************************************************************/
Int_t AliRunLoader::OpenDataFile(const TString& filename,TFile*& file,TDirectory*& dir,Option_t* opt,Int_t cl)
{
//Opens File with kinematics
 if (file)
  {
    if (file->IsOpen() == kFALSE)
     {//pointer is not null but file is not opened
       Warning("OpenDataFile","Pointer to file is not null, but file is not opened");//risky any way
       delete file;
       file = 0x0; //proceed with opening procedure
     }
    else
     { 
       Warning("OpenDataFile","File  %s already opened",filename.Data());
       return 0;
     }
  }
//try to find if that file is opened somewere else
 file = (TFile *)( gROOT->GetListOfFiles()->FindObject(filename) );
 if (file)
  {
   if(file->IsOpen() == kTRUE)
    {
     Warning("OpenDataFile","File %s already opened by sombody else.",file->GetName());
     return 0;
    }
  }

 file = TFile::Open(filename,opt);
 if (file == 0x0)
  {//file is null
    Error("LoadKinematics","Can not open file %s",filename.Data());
    return 1;
  }
 if (file->IsOpen() == kFALSE)
  {//file is not opened
   Error("LoadKinematics","Can not open file %s",filename.Data());
   return 1;
  }
  
 file->SetCompressionLevel(cl);
 
 dir = AliLoader::ChangeDir(file,fCurrentEvent);
 if (dir == 0x0)
  {
    Error("OpenKineFile","Can not change to root directory in file %s",filename.Data());
    return 3;
  }
 return 0; 
}
/**************************************************************************/

Int_t AliRunLoader::PostKinematics()
{
  //takes the kine container from file and puts it in the folder
  
  if (GetDebug()) Info("PostKinematics","Posting Kinematics");
  
  if (fKineData.Directory() == 0x0)
   {
     Error("PostKinematics","Kinematics Directory is NULL. LoadKinematics before.");
     return 2; 
   }

  TObject* tree = fKineData.Directory()->Get(fgkKineContainerName);
  if (tree == TreeK()) return 0; //protection in case in folder is the same obj
  if(tree)
   {
     //if such an obejct already exists - remove it first
     CleanKinematics();//if kine tree already is in folder
     GetEventFolder()->Add(tree);
     return 0;
   }
  else
   {
    Warning("PostKinematics","Can not find Kinematics Contaioner object <%s> in file %s",
             fgkKineContainerName.Data(),fKineData.File()->GetName());
    return 1;
   }
}
/**************************************************************************/

Int_t AliRunLoader::PostTrackRefs()
{
  //takes the kine container from file and puts it in the folder
  
  if (GetDebug()) Info("PostTrackRefs","Posting TrackRefs");
  
  if (fTrackRefsData.Directory() == 0x0)
   {
     Error("PostTrackRefs","TrackRefs Directory is NULL. LoadTrackRefs before.");
     return 2; 
   }

  TObject* tree = fTrackRefsData.Directory()->Get(fgkTrackRefsContainerName);
  if (tree == TreeTR()) return 0; //protection in case in folder is the same obj
  if(tree)
   {
     //if such an obejct already exists - remove it first
     CleanTrackRefs();//if kine tree already is in folder
     GetEventFolder()->Add(tree);
     return 0;
   }
  else
   {
    Warning("PostTrackRefs","Can not find TrackRefs Contaioner object <%s> in file %s",
             fgkTrackRefsContainerName.Data(),fTrackRefsData.File()->GetName());
    return 1;
   }
}
/**************************************************************************/

TTree* AliRunLoader::TreeE() const
{
 //returns the tree from folder; shortcut method
 TObject *obj = fEventFolder->FindObject(fgkHeaderContainerName);
 return (obj)?dynamic_cast<TTree*>(obj):0x0;
}
/**************************************************************************/

AliHeader* AliRunLoader::GetHeader() const
{
 return fHeader;
}
/**************************************************************************/
 
TTree* AliRunLoader::TreeK() const
{
 //returns the tree from folder; shortcut method
 TObject *obj = GetEventFolder()->FindObject(fgkKineContainerName);
 return (obj)?dynamic_cast<TTree*>(obj):0x0;
}
/**************************************************************************/

TTree* AliRunLoader::TreeTR() const
{
 //returns the tree from folder; shortcut method
 TObject* obj = GetEventFolder()->FindObject(fgkTrackRefsContainerName);
 return (obj)?dynamic_cast<TTree*>(obj):0x0;
}
/**************************************************************************/

AliRun* AliRunLoader::GetAliRun() const
{
//returns AliRun which sits in the folder
 if (fEventFolder == 0x0) return 0x0;
 TObject *obj = fEventFolder->FindObject(fgkGAliceName);
 return (obj)?dynamic_cast<AliRun*>(obj):0x0;
}
/**************************************************************************/

Int_t AliRunLoader::WriteGeometry(Option_t* opt)
{
  fGAFile->cd();
  TGeometry* geo = GetAliRun()->GetGeometry();
  if (geo == 0x0)
   {
     Error("WriteGeometry","Can not get geometry from gAlice");
     return 1;
   }
  geo->Write();
  return 0;
}
/**************************************************************************/

Int_t AliRunLoader::WriteHeader(Option_t* opt)
{
  if (GetDebug()) Info("WriteHeader","  WRITING HEADER");
  
  TTree* tree = TreeE();
  if ( tree == 0x0)
   {
     Warning("WriteHeader","Can not find Header in Folder");
     return 0;
   } 
  if (fGAFile->IsWritable() == kFALSE)
   {
     Error("WriteHeader","File %s is not writable",fGAFile->GetName());
     return 1;
   }

  TObject* obj = fGAFile->Get(fgkHeaderContainerName);
  if (obj)
   { //if they exist, see if option OVERWRITE is used
     TString tmp(opt);
     if(tmp.Contains("OVERWRITE",TString::kIgnoreCase) == 0)
      {//if it is not used -  give an error message and return an error code
        Error("WriteHeader","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  fGAFile->cd();
  tree->SetDirectory(fGAFile);
  tree->Write(0,TObject::kOverwrite);
  fGAFile->Write(0,TObject::kOverwrite);

  if (GetDebug()) Info("WriteHeader","WRITTEN\n\n");
  
  return 0;
}
/**************************************************************************/

Int_t AliRunLoader::WriteAliRun(Option_t* opt)
{
  fGAFile->cd();
  if (gAlice) gAlice->Write();
  return 0;
}
/**************************************************************************/

Int_t AliRunLoader::WriteKinematics(Option_t* opt)
{

  if(TreeK() == 0x0)
   {//did not get, nothing to write
     Warning("WriteKinematics","Kinematics object named %s not found in folder %s.\nNothing to write. Returning",
              fgkKineContainerName.Data(), GetEventFolder()->GetName());
     return 0;
   }
  
  //check if file is opened
  if (fKineData.Directory() == 0x0)
   { 
     //if not open, try to open
     if (OpenKineFile("UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteKinematics","Can not open Kine file. Kinematics is NOT WRITTEN");
        return 1;
      }
   }

  if (fKineData.File()->IsWritable() == kFALSE)
   {
     Error("WriteKinematics","File %s is not writable",fKineData.File()->GetName());
     return 1;
   }
  
  //see if hits container already exists in this (root) directory
  TObject* tree = fKineData.Directory()->Get(fgkKineContainerName);
  if (tree)
   { //if they exist, see if option OVERWRITE is used
     TString tmp(opt);
     if(tmp.Contains("OVERWRITE",TString::kIgnoreCase) == 0)
      {//if it is not used -  give an error message and return an error code
        Error("WriteKinematics","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }

  if (GetDebug()) Info("WriteKinematics","name=%s opt=%s fKineData.File() =%s fKineData.Directory()=%s TreeK=%s",
                         GetName(),opt,fKineData.File()->GetName(),fKineData.Directory()->GetName(),TreeK()->GetName());
  
  fKineData.Directory()->cd();
  TreeK()->SetDirectory(fKineData.Directory()); //forces setting the directory to this directory (we changed dir few lines above)
  
  if (GetDebug()) Info("WriteKinematics","Writing tree");
  TreeK()->Write(0,TObject::kOverwrite);
  return 0;
   
}
/**************************************************************************/
Int_t AliRunLoader::WriteTrackRefs(Option_t* opt)
{
  if(TreeTR() == 0x0)
   {//did not get, nothing to write
     Warning("WriteTrackRefs","TrackRefs object named %s not found in folder %s.\nNothing to write. Returning",
              fgkTrackRefsContainerName.Data(), GetEventFolder()->GetName());
     return 0;
   }
  
  //check if file is opened
  if (fTrackRefsData.Directory() == 0x0)
   { 
     //if not open, try to open
     if (OpenTrackRefsFile("UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteTrackRefs","Can not open TrackRefs file. TrackRefs is NOT WRITTEN");
        return 1;
      }
   }

  if (fTrackRefsData.File()->IsWritable() == kFALSE)
   {
     Error("WriteTrackRefs","File %s is not writable",fTrackRefsData.File()->GetName());
     return 1;
   }
  
  //see if hits container already exists in this (root) directory
  TObject* tree = fTrackRefsData.Directory()->Get(fgkTrackRefsContainerName);
  if (tree)
   { //if they exist, see if option OVERWRITE is used
     TString tmp(opt);
     if(tmp.Contains("OVERWRITE",TString::kIgnoreCase) == 0)
      {//if it is not used -  give an error message and return an error code
        Error("WriteTrackRefs","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  
  if (GetDebug()) Info("WriteTrackRefs","name=%s opt=%s fTrackRefsData.File()=%s fTrackRefsData.Directory()=%s TreeTR()=%s",
                         GetName(),opt,fTrackRefsData.File()->GetName(),fTrackRefsData.Directory()->GetName(),TreeTR()->GetName());
  
  fTrackRefsData.Directory()->cd();
  TreeTR()->SetDirectory(fTrackRefsData.Directory()); //forces setting the directory to this directory (we changed dir few lines above)
  
  if (GetDebug()) Info("WriteTrackRefs","Writing tree");
  TreeTR()->Write(0,TObject::kOverwrite);

  return 0;
  
}
/**************************************************************************/

Int_t AliRunLoader::WriteHits(Option_t* opt)
{
  Int_t res;
  Int_t result = 0;
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   {
     res = loader->WriteHits(opt);
     if (res)
      {
        Error("WriteHits","Failed to write hits for %s.",loader->GetDetectorName().Data());
        result = 1;
      }
   }
  return result;
}
/**************************************************************************/

Int_t AliRunLoader::WriteSDigits(Option_t* opt)
{
  Int_t res;
  Int_t result = 0;
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   {
     res = loader->WriteSDigits(opt);
     if (res)
      {
        Error("WriteSDigits","Failed to write summable digits for %s.",loader->GetDetectorName().Data());
        result = 1;
      }
   }
  return result;
}
/**************************************************************************/

Int_t AliRunLoader::WriteDigits(Option_t* opt)
{
  Int_t res;
  Int_t result = 0;
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   { 
     res = loader->WriteDigits(opt);
     if (res)
      {
        Error("WriteDigits","Failed to write digits for %s.",loader->GetDetectorName().Data());
        result = 1;
      }
   }
  return result;
}
/**************************************************************************/

Int_t AliRunLoader::WriteRecPoints(Option_t* opt)
{
  Int_t res;
  Int_t result = 0;
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   {
     res = loader->WriteRecPoints(opt);
     if (res)
      {
        Error("WriteRecPoints","Failed to write Reconstructed Points for %s.",
              loader->GetDetectorName().Data());
        result = 1;
      }
   }
  return result;
}
/**************************************************************************/

Int_t AliRunLoader::WriteTracks(Option_t* opt)
{
  Int_t res;
  Int_t result = 0;
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   {
     res = loader->WriteTracks(opt);
     if (res)
      {
        Error("WriteTracks","Failed to write Tracks for %s.",
              loader->GetDetectorName().Data());
        result = 1;
      }
   }
  return result;
}
/**************************************************************************/

Int_t AliRunLoader::WriteRunLoader(Option_t* opt)
{
  CdGAFile();
  this->Write(0,TObject::kOverwrite);
  return 0;
}
/**************************************************************************/

Int_t AliRunLoader::SetEventFolderName(const TString& name)
{  //sets top folder name for this run; of alread
  if (name.IsNull())
   {
     Error("SetTopFolderName","Name is empty");
     return 1;
   }
  
  //check if such a folder already exists - try to find it in alice top folder
  TObject* obj = AliConfig::Instance()->GetTopFolder()->FindObject(name);
  if(obj)
   {
     TFolder* fold = dynamic_cast<TFolder*>(obj);
     if (fold == 0x0)
      {
       Error("SetTopFolderName","Such a obejct already exists in top alice folder and it is not a folder.");
       return 2;
      }
     //folder which was found is our folder
     if (fEventFolder == fold)
      {
       return 0;
      }
     else
      {
       Error("SetTopFolderName","Such a folder already exists in top alice folder. Can not mount.");
       return 2;
      }
   }

  //event is alredy connected, just change name of the folder
  if (fEventFolder) 
   {
     fEventFolder->SetName(name);
     return 0;
   }
   
  //build the event folder structure
  if (GetDebug()) Info("SetEventFolderName","Creting new event folder named %s",name.Data());
  fEventFolder = AliConfig::Instance()->BuildEventFolder(name,"Event Folder");
  fEventFolder->Add(this);//put myself to the folder to accessible for all
  
  if (Stack()) Stack()->SetEventFolderName(fEventFolder->GetName());
  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   {
     loader->SetEventFolder(fEventFolder);
     loader->Register();
   }
  
  fEventFolder->SetOwner();
  return 0;
}
/**************************************************************************/

void AliRunLoader::AddLoader(AliLoader* loader)
 {
 //Adds the Loader for given detector 
  if (loader == 0x0) //if null shout and exit
   {
     Error("AddLoader","Parameter is NULL");
     return;
   }
  if (fEventFolder) loader->SetEventFolder(fEventFolder); //if event folder is already defined, 
                                                          //pass information to the Loader
  fLoaders->Add(loader);//add the Loader to the array
 }
/**************************************************************************/

void AliRunLoader::AddLoader(AliDetector* det)
 {
//Asks module (detector) ro make a Loader and stores in the array
   if (det == 0x0) return;
   AliLoader* get = det->GetLoader();//try to get loader
   if (get == 0x0)  get = det->MakeLoader(fEventFolder->GetName());//if did not obtain, ask to make it

   if (get) 
    {
      if (GetDebug()) Info("AddLoader","Detector: %s   Loader : %s",det->GetName(),get->GetName());
      AddLoader(get);
    }
 }

/**************************************************************************/

AliLoader* AliRunLoader::GetLoader(const char* detname) const
{
  return (AliLoader*)fLoaders->FindObject(detname);
}

/**************************************************************************/

AliLoader* AliRunLoader::GetLoader(AliDetector* det) const
{
 if(det == 0x0) return 0x0;
 TString getname(det->GetName());
 getname+="Loader";
 if (GetDebug()) Info("GetLoader(AliDetector* det)"," Loader name is %s",getname.Data());
 return GetLoader(getname);
}

/**************************************************************************/

void AliRunLoader::CleanFolders()
{
//  fEventFolder->Add(this);//put myself to the folder to accessible for all

  CleanDetectors();
  CleanHeader();
  CleanKinematics();
}
/**************************************************************************/

void AliRunLoader::CleanDetectors()
{
//Calls CleanFolders for all detectors
  TIter next(fLoaders);
  AliLoader *Loader;
  while((Loader = (AliLoader*)next())) 
   {
     Loader->CleanFolders();
   }
}
/**************************************************************************/

void AliRunLoader::RemoveEventFolder()
{
//remove all the tree of event 
//all the stuff changing EbE stays untached (PDGDB, tasks, etc.)

 if (fEventFolder == 0x0) return;
 fEventFolder->SetOwner(kFALSE);//don't we want to deleted while removing the folder that we are sitting in
 fEventFolder->Remove(this);//remove us drom folder
 
 AliConfig::Instance()->GetTopFolder()->SetOwner(); //brings ownership back for fEventFolder since it sits in top folder
 AliConfig::Instance()->GetTopFolder()->Remove(fEventFolder); //remove the event tree
 delete fEventFolder;
}
/**************************************************************************/

void AliRunLoader::SetGAliceFile(TFile* gafile)
{
 fGAFile = gafile;
}

/**************************************************************************/

Int_t AliRunLoader::LoadHits(Option_t* detectors,Option_t* opt)
{
//LoadHits in selected detectors i.e. detectors="ITS TPC TRD" or "all"

  if (GetDebug()) Info("LoadHits","Loading Hits");
  TObjArray* loaders;
  TObjArray arr;

  char* oAll = strstr(detectors,"all");
  if (oAll)
   {
     if (GetDebug()) Info("LoadHits","Option is All");
     loaders = fLoaders;
   }
  else
   {
     GetListOfDetectors(detectors,arr);//this method looks for all Loaders corresponding to names (many) specified in detectors option
     loaders = &arr;//get the pointer array
   }   

  if (GetDebug()) Info("LoadHits","For detectors. Number of detectors chosen for loading %d",loaders->GetEntries());
  
  TIter next(loaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next())) 
   {
    if (GetDebug()) Info("LoadHits","    Calling LoadHits(%s) for %s",opt,loader->GetName());
    loader->LoadHits(opt);
   }
  if (GetDebug()) Info("LoadHits","Done");
  return 0;
} 

/**************************************************************************/

Int_t AliRunLoader::LoadSDigits(Option_t* detectors,Option_t* opt)
{
//LoadHits in selected detectors i.e. detectors="ITS TPC TRD" or "all"

  TObjArray* loaders;
  TObjArray arr;

  char* oAll = strstr(detectors,"all");
  if (oAll)
   {
     loaders = fLoaders;
   }
  else
   {
     GetListOfDetectors(detectors,arr);//this method looks for all Loaders corresponding to names (many) specified in detectors option
     loaders = &arr;//get the pointer array
   }   

  TIter next(loaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next())) 
   {
    loader->LoadSDigits(opt);
   }
  return 0;
} 

/**************************************************************************/

Int_t AliRunLoader::LoadDigits(Option_t* detectors,Option_t* opt)
{
//LoadHits in selected detectors i.e. detectors="ITS TPC TRD" or "all"

  TObjArray* Loaders;
  TObjArray arr;

  char* oAll = strstr(detectors,"all");
  if (oAll)
   {
     Loaders = fLoaders;
   }
  else
   {
     GetListOfDetectors(detectors,arr);//this method looks for all Loaders corresponding to names (many) specified in detectors option
     Loaders = &arr;//get the pointer array
   }   

  TIter next(Loaders);
  AliLoader *Loader;
  while((Loader = (AliLoader*)next())) 
   {
    Loader->LoadDigits(opt);
   }
  return 0;
} 

/**************************************************************************/

Int_t AliRunLoader::LoadRecPoints(Option_t* detectors,Option_t* opt)
{
//LoadHits in selected detectors i.e. detectors="ITS TPC TRD" or "all"

  TObjArray* Loaders;
  TObjArray arr;

  char* oAll = strstr(detectors,"all");
  if (oAll)
   {
     Loaders = fLoaders;
   }
  else
   {
     GetListOfDetectors(detectors,arr);//this method looks for all Loaders corresponding to names (many) specified in detectors option
     Loaders = &arr;//get the pointer array
   }   

  TIter next(Loaders);
  AliLoader *Loader;
  while((Loader = (AliLoader*)next())) 
   {
    Loader->LoadRecPoints(opt);
   }
  return 0;
} 

/**************************************************************************/

Int_t AliRunLoader::LoadTracks(Option_t* detectors,Option_t* opt)
{
//LoadHits in selected detectors i.e. detectors="ITS TPC TRD" or "all"

  TObjArray* Loaders;
  TObjArray arr;

  char* oAll = strstr(detectors,"all");
  if (oAll)
   {
     Loaders = fLoaders;
   }
  else
   {
     GetListOfDetectors(detectors,arr);//this method looks for all Loaders corresponding to names (many) specified in detectors option
     Loaders = &arr;//get the pointer array
   }   

  TIter next(Loaders);
  AliLoader *Loader;
  while((Loader = (AliLoader*)next())) 
   {
    Loader->LoadTracks(opt);
   }
  return 0;
} 

/**************************************************************************/

AliRunLoader* AliRunLoader::GetRunLoader(const char* eventfoldername)
 {
  TFolder* evfold= dynamic_cast<TFolder*>(AliConfig::Instance()->GetTopFolder()->FindObject(eventfoldername));
  if (evfold == 0x0)
   {
     return 0x0;
   }
  AliRunLoader* runget = dynamic_cast<AliRunLoader*>(evfold->FindObject(AliRunLoader::fgkRunLoaderName));
  return runget;
  
 }
/**************************************************************************/

void AliRunLoader::CdGAFile()
{
//sets gDirectory to galice file
//work around 
  if(fGAFile) fGAFile->cd();
}
 
/**************************************************************************/

void AliRunLoader::GetListOfDetectors(const char * namelist,TObjArray& pointerarray) const
 {
//this method looks for all Loaders corresponding 
//to names (many) specified in namelist i.e. namelist ("ITS TPC TRD")
  
   char buff[10];
   char dets [200];
   strcpy(dets,namelist);//compiler cries when char* = const Option_t*;
   dets[strlen(dets)+1] = '\n';//set endl at the end of string 
   char* pdet = dets;
   Int_t tmp;
   for(;;)
    {
      tmp = sscanf(pdet,"%s",buff);//read the string from the input string pdet into buff
      if ( (buff[0] == 0) || (tmp == 0) ) break; //if not read
     
      pdet = strstr(pdet,buff) + strlen(buff);;//move the input pointer about number of bytes (letters) read
      //I am aware that is a little complicated. I don't know the number of spaces between detector names
      //so I read the string, than I find where it starts (strstr) and move the pointer about length of a string
      //If there is a better way, please write me (Piotr.Skowronski@cern.ch)
      //construct the Loader name
      TString getname(buff);
      getname+="Loader";
      AliLoader* loader = GetLoader(getname);//get the Loader
      if (loader)
       {
        pointerarray.Add(loader);
       }
      else
       {
        Error("GetListOfDetectors","Can not find Loader for %s",buff);
       }
        
      buff[0] = 0;
    }
 }
/*****************************************************************************/ 

void AliRunLoader::Clean(const TString& name)
{
//removes object with given name from event folder and deletes it
  if (GetEventFolder() == 0x0) return;
  TObject* obj = GetEventFolder()->FindObject(name);
  if(obj)
   {
     if (GetDebug()) Info("Clean(const TString&)","name=%s, cleaning %s.",GetName(),name.Data());
     GetEventFolder()->Remove(obj);
     delete obj;
   }
}

/*****************************************************************************/ 

TTask* AliRunLoader::GetRunDigitizer()
{
//returns Run Digitizer from folder

 TFolder* topf = AliConfig::Instance()->GetTaskFolder();
 TObject* obj = topf->FindObjectAny(AliConfig::Instance()->GetDigitizerTaskName());
 return (obj)?dynamic_cast<TTask*>(obj):0x0;
}
/*****************************************************************************/ 

TTask* AliRunLoader::GetRunSDigitizer()
{
//returns SDigitizer Task from folder

 TFolder* topf = AliConfig::Instance()->GetTaskFolder();
 TObject* obj = topf->FindObjectAny(AliConfig::Instance()->GetSDigitizerTaskName());
 return (obj)?dynamic_cast<TTask*>(obj):0x0;
}
/*****************************************************************************/ 

TTask* AliRunLoader::GetRunReconstructioner()
{
//returns Reconstructioner Task from folder
 TFolder* topf = AliConfig::Instance()->GetTaskFolder();
 TObject* obj = topf->FindObjectAny(AliConfig::Instance()->GetReconstructionerTaskName());
 return (obj)?dynamic_cast<TTask*>(obj):0x0;
}
/*****************************************************************************/ 

TTask* AliRunLoader::GetRunTracker()
{
//returns Tracker Task from folder
 TFolder* topf = AliConfig::Instance()->GetTaskFolder();
 TObject* obj = topf->FindObjectAny(AliConfig::Instance()->GetTrackerTaskName());
 return (obj)?dynamic_cast<TTask*>(obj):0x0;
}
/*****************************************************************************/ 
TTask* AliRunLoader::GetRunQATask()
{
//returns Quality Assurance Task from folder
 TFolder* topf = AliConfig::Instance()->GetTaskFolder();
 if (topf == 0x0)
  {
    ::Error("AliRunLoader::GetRunQATask","Can not get task folder from AliConfig");
    return 0x0;
  }
 TObject* obj = topf->FindObjectAny(AliConfig::Instance()->GetQATaskName());
 return (obj)?dynamic_cast<TTask*>(obj):0x0;
}

/*****************************************************************************/ 

void AliRunLoader::SetCompressionLevel(Int_t cl)
{
//Sets Compression Level in all files
 if (fGAFile) fGAFile->SetCompressionLevel(cl);
 SetKineComprLevel(cl);
 SetTrackRefsComprLevel(cl);
 TIter next(fLoaders);
 AliLoader *loader;
 while((loader = (AliLoader*)next()))
  {
   loader->SetCompressionLevel(cl);
  }
}
/**************************************************************************/

void AliRunLoader::SetKineComprLevel(Int_t cl)
{
//Sets comression level in Kine File
  fKineData.CompressionLevel() = cl;
  if (fKineData.File()) fKineData.File()->SetCompressionLevel(cl);
}
/**************************************************************************/

void AliRunLoader::SetTrackRefsComprLevel(Int_t cl)
{
//Sets comression level in Track Refences File
  fTrackRefsData.CompressionLevel() = cl;
  if (fTrackRefsData.File()) fTrackRefsData.File()->SetCompressionLevel(cl);
}
/**************************************************************************/

void AliRunLoader::UnloadHeader()
{
 //removes TreeE from folder and deletes it
 // as well as fHeader object
 CleanHeader();
 delete fHeader;
 fHeader = 0x0;
}
/**************************************************************************/

void AliRunLoader::UnloadKinematics()
{
 CleanKinematics();//removes from folder and deletes
 delete fKineData.File();//closes Kine file
 fKineData.File() = 0x0;
 fKineData.Directory() = 0x0;
}
/**************************************************************************/

void AliRunLoader::UnloadTrackRefs()
{
 CleanTrackRefs();//removes from folder and deletes
 delete fTrackRefsData.File();//closes TrackRefs file
 fTrackRefsData.File() = 0x0;
 fTrackRefsData.Directory() = 0x0;
}
/**************************************************************************/

void AliRunLoader::UnloadgAlice()
{
 if (gAlice == GetAliRun())
  {
   if (GetDebug()) Info("UnloadgAlice","Set gAlice = 0x0");
   gAlice = 0x0;//if gAlice is the same that in folder (to be deleted by the way of folder)
  }
 AliRun* alirun = GetAliRun();
 if (GetEventFolder()) GetEventFolder()->Remove(alirun);
 delete alirun;
}
/**************************************************************************/

void  AliRunLoader::MakeTrackRefsContainer()
{
// Makes a tree for Track References
  if (TreeTR()) return;
  TTree* tree = new TTree(fgkTrackRefsContainerName,"Tree with Track References");
  GetEventFolder()->Add(tree);
  WriteTrackRefs("OVERWRITE");
}
/**************************************************************************/

Int_t AliRunLoader::LoadTrackRefs(Option_t* option)
{
//Load track references from file (opens file and posts tree to folder)

 if (TreeTR())
  {
     Warning("LoadTrackRefs","Track References are already loaded. Use ReloadTrackRefs to force reload. Nothing done");
     return 0;
  }

 if (GetEventFolder() == 0x0)
  {
    Error("LoadTrackRefs","Event folder not specified yet");
    return 1;
  }
 Int_t retval = OpenTrackRefsFile(option);
 if (retval)
   {
    Error("LoadTrackRefs","Error occured while opening Kine file");
    return retval;
   }
 
 fTrackRefsData.FileOption() = option;
 
 if (AliLoader::TestFileOption(option) == kFALSE) return 0;

 retval = PostTrackRefs();
 
 if (retval)
  {//propagate error up 
    Error("LoadTrackRefs","PostTrackRefs returned error");
    return retval;
  }
 return 0;
}
/**************************************************************************/

void  AliRunLoader::SetDirName(TString& dirname)
{
//sets directory name 
  if (dirname.IsNull()) return;
  
  fTrackRefsData.FileName() = dirname + "/" + fTrackRefsData.FileName();
  fKineData.FileName() = dirname + "/" + fKineData.FileName();

  TIter next(fLoaders);
  AliLoader *loader;
  while((loader = (AliLoader*)next()))
   {
    loader->SetDirName(dirname);
   }

}
/*****************************************************************************/ 

Int_t AliRunLoader::OpenKineFile(Option_t* opt)
 {
 //Opens file with kinematics
   return OpenDataFile(SetFileOffset(fKineData.FileName()),
                       fKineData.File(),fKineData.Directory(),
	   opt,fKineData.CompressionLevel());
 }
/*****************************************************************************/ 

Int_t AliRunLoader::OpenTrackRefsFile(Option_t* opt)
 {
  //opens file with Track References
   
   return OpenDataFile(SetFileOffset(fTrackRefsData.FileName()),
                       fTrackRefsData.File(),fTrackRefsData.Directory(),
	   opt,fTrackRefsData.CompressionLevel());
 }
/*****************************************************************************/ 

Int_t AliRunLoader::GetFileOffset() const
{
  return Int_t(fCurrentEvent/fNEventsPerFile);
}

/*****************************************************************************/ 
const TString AliRunLoader::SetFileOffset(const TString& fname)
{
  Long_t offset = (Long_t)GetFileOffset();
  if (offset < 1) return fname;
  TString soffset;
  soffset += offset;//automatic conversion to string
  TString dotroot(".root");
  const TString& offfsetdotroot = offset + dotroot;
  TString out = fname;
  out = out.ReplaceAll(dotroot,offfsetdotroot);
  if (GetDebug()) Info("SetFileOffset"," in=%s out=%s",fname.Data(),out.Data());
  return out;
}
/*****************************************************************************/ 

void AliRunLoader::SetDigitsFileNameSuffix(const TString& suffix)
{
//adds the suffix before ".root", 
//e.g. TPC.Digits.root -> TPC.DigitsMerged.root
//made on Jiri Chudoba demand

  TIter next(fLoaders);
  AliLoader *Loader;
  while((Loader = (AliLoader*)next())) 
   {
     Loader->SetDigitsFileNameSuffix(suffix);
   }
}
/*****************************************************************************/ 

TString AliRunLoader::GetFileName() const
{
//returns name of galice file
 TString result;
 if (fGAFile == 0x0) return result;
 result = fGAFile->GetName();
 return result;
}

/*****************************************************************************/ 
/*****************************************************************************/ 
/*****************************************************************************/ 
