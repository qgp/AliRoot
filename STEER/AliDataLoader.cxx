#include <AliDataLoader.h>
//__________________________________________
/////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                         //
//  class AliDataLoader                                                                    //
//                                                                                         //
//  Container of all data needed for full                                                  //
//  description of each data type                                                          //
//  (Hits, Kine, ...)                                                                      //
//                                                                                         //
//  Each data loader has a basic standard setup of BaseLoaders                             //
//  which can be identuified by indexes (defined by EStdBasicLoaders)                      //
//  Data managed by these standard base loaders has fixed naming convention                //
//  e.g. - tree with hits is always named TreeH                                            //
//                     (defined in AliLoader::fgkDefaultHitsContainerName)                 //
//       - task DtectorName+Name defined                                                   //
//                                                                                         //
//  EStdBasicLoaders   idx     Object Type        Description                              //
//      kData           0    TTree or TObject     main data itself (hits,digits,...)       //
//      kTask           1        TTask            object producing main data               //
//      kQA             2        TTree                quality assurance tree               //
//      kQATask         3        TTask            task producing QA object                 //
//                                                                                         //
//                                                                                         //
//  User can define and add more basic loaders even Run Time.                              //
//  Caution: in order to save information about added base loader                          //
//  user must rewrite Run Loader to galice.file, overwriting old setup                     //
//                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////

#include <TROOT.h>
#include <TFile.h>
#include <TString.h>
#include <TBits.h>
#include <TList.h>

#include "AliRunLoader.h"

ClassImp(AliDataLoader)

AliDataLoader::AliDataLoader():
 fFileName(0),
 fFile(0x0),
 fDirectory(0x0),
 fFileOption(),
 fCompressionLevel(2),
 fBaseLoaders(0x0),
 fHasTask(kFALSE),
 fTaskName(),
 fParentalTask(0x0),
 fEventFolder(0x0),
 fFolder(0x0)
{
  
}
/*****************************************************************************/ 

AliDataLoader::AliDataLoader(const char* filename, const char* contname, const char* name, Option_t* opt):
 TNamed(name,name),
 fFileName(filename),
 fFile(0x0),
 fDirectory(0x0),
 fFileOption(0),
 fCompressionLevel(2),
 fBaseLoaders(new TObjArray(4)),
 fHasTask(kFALSE),
 fTaskName(),
 fParentalTask(0x0),
 fEventFolder(0x0),
 fFolder(0x0)
{
//constructor
// creates a 0 loader, depending on option, default "T" is specialized loader for trees
// else standard object loader
// trees needs special care, becouse we need to set dir before writing
  if (GetDebug())
   Info("AliDataLoader","File name is %s",fFileName.Data());
   
  TString option(opt);
  AliBaseLoader* bl;
  if (option.CompareTo("T",TString::kIgnoreCase) == 0)
    bl = new AliTreeLoader(contname,this);
  else 
    bl = new AliObjectLoader(contname,this);
  fBaseLoaders->AddAt(bl,kData);
  
}
/*****************************************************************************/ 

AliDataLoader::~AliDataLoader()
{
//dtor
 UnloadAll();
}
/*****************************************************************************/ 

Int_t  AliDataLoader::SetEvent()
{
//basically the same that GetEvent but do not post data to folders
 AliRunLoader* rl = GetRunLoader();
 if (rl == 0x0)
  {
    Error("SetEvent","Can not get RunGettr");
    return 1;
  }
 
 Int_t evno = rl->GetEventNumber();

 TIter next(fBaseLoaders);
 AliBaseLoader* bl;
 while ((bl = (AliBaseLoader*)next()))
  {
    if (bl->DoNotReload() == kFALSE) bl->Clean();
  }

 if(fFile)
  {
    if (CheckReload())
     {
       delete fFile;
       fFile = 0x0;
       if (GetDebug()) Info("SetEvent","Reloading new file. File opt is %s",fFileOption.Data());
       OpenFile(fFileOption);
     }

    fDirectory = AliLoader::ChangeDir(fFile,evno);
    if (fDirectory == 0x0)
      {
        Error("SetEvent","Can not chage directory in file %s",fFile->GetName());
        return 1;
      }
   }
 return 0;
}
/*****************************************************************************/ 

Int_t  AliDataLoader::GetEvent()
{
 // posts all loaded data from files to White Board
 // event number is defined in RunLoader
 // 
 //returns:
 //     0  - in case of no error
 //     1  - event not found
 //     
 //for each base laoder post, if was loaded before GetEvent
 
 //call set event to switch to new directory in file


 //post all data that were loaded before 
 // ->SetEvent does not call Unload, but only cleans White Board
 // such IsLoaded flag stays untached
 
 if ( AliLoader::TestFileOption(fFileOption) == kTRUE ) //if file is read or update mode try to post
  {                                                     //in other case there is no sense to post: file is new
   TIter nextbl(fBaseLoaders);
   AliBaseLoader* bl;
   while ((bl = (AliBaseLoader*)nextbl()))
    {
     if (bl->IsLoaded())
      {
        if (bl->DoNotReload() == kFALSE) bl->Post();
      }
    } 
  }
 return 0;
}
/*****************************************************************************/ 

Int_t AliDataLoader::OpenFile(Option_t* opt)
{
//Opens file named 'filename', and assigns pointer to it to 'file'
//jumps to fDirectoryectory corresponding to current event and stores the pointer to it in 'fDirectory'
//option 'opt' is passed to TFile::Open
  if (fFile)
   {
     if(fFile->IsOpen() == kTRUE)
       {
         Warning("OpenFile"," File %s already opened. First close it.",fFile->GetName());
         return 0;
       }
     else
       {
         Warning("OpenFile","Pointer to file %s is not null, but file is not opened",
                fFile->GetName());
         delete fFile;
         fFile = 0x0;
       }
   }
  
  TString fname(SetFileOffset(fFileName));
  
  fFile = (TFile *)(gROOT->GetListOfFiles()->FindObject(fname));
  if (fFile)
   {
     if(fFile->IsOpen() == kTRUE)
       {
         Warning("OpenFile","File %s already opened by sombody else. First close it.",
                 fFile->GetName());
         return 0;
       }
   }
  
  fFileOption = opt;
  fFile = TFile::Open(fname,fFileOption);//open the file
  if (fFile == 0x0)
   {//file is null
     Error("OpenFile","Can not open file %s",fname.Data());
     return 1;
   }
  if (fFile->IsOpen() == kFALSE)
   {//file is null
     Error("OpenFile","Can not open file %s",fname.Data());
     return 1;
   }

  fFile->SetCompressionLevel(fCompressionLevel);
  
  AliRunLoader* rg = GetRunLoader();
  if (rg == 0x0)
   {
     Error("OpenFile","Can not find Run-Loader in folder.");
     return 2;
   }
  Int_t evno = rg->GetEventNumber();
  
  fDirectory = AliLoader::ChangeDir(fFile,evno);
  if (fDirectory == 0x0)
   {
     Error("OpenFile","Can not chage fDirectory in file %s.",fFile->GetName());
     return 3; 
   }
  return 0;
}
/*****************************************************************************/ 

void AliDataLoader::Unload()
{
 //unloads main data -  shortcut method 
  GetBaseLoader(0)->Unload();
}
/*****************************************************************************/ 

void AliDataLoader::UnloadAll()
{
//Unloads all data and tasks
 TIter next(fBaseLoaders);
 AliBaseLoader* bl;
 while ((bl = (AliBaseLoader*)next()))
  {
    bl->Unload();
  }
}
/*****************************************************************************/ 

Int_t AliDataLoader::Reload()
{
 //Unloads and loads data again
 if ( fFile == 0x0 ) return 0;
   
 TBits loaded(fBaseLoaders->GetEntries());  
 TIter next(fBaseLoaders);
 AliBaseLoader* bl;

 Int_t i = 0;
 while ((bl = (AliBaseLoader*)next()))
  {
    if (bl->IsLoaded())
     {
       loaded.SetBitNumber(i++,kTRUE);
       bl->Unload();
     }
  }
 
 Int_t retval;
 i = 0;  
 next.Reset();
 while ((bl = (AliBaseLoader*)next()))
  {
    if (loaded.TestBitNumber(i++))
     {
       retval = bl->Load(fFileOption);
       if (retval) 
        {
         Error("Reload","Error occur while loading %s",bl->GetName());
         return retval;
        }
     }
  }
 

 return 0;
 }
/*****************************************************************************/ 
Int_t AliDataLoader::WriteData(Option_t* opt)
{
//Writes primary data ==  first BaseLoader
  if (GetDebug())
   Info("WriteData","Writing %s container for %s data. Option is %s.",
          GetBaseLoader(0)->GetName(),GetName(),opt);
  return GetBaseLoader(0)->WriteData(opt);
}
/*****************************************************************************/ 

Int_t AliDataLoader::Load(Option_t* opt)
{
//Writes primary data ==  first BaseLoader
  return GetBaseLoader(0)->Load(opt);
}
/*****************************************************************************/ 

Int_t  AliDataLoader::SetEventFolder(TFolder* eventfolder)
{
 //sets the event folder
 if (eventfolder == 0x0)
  {
    Error("SetEventFolder","Stupid joke. Argument is NULL");
    return 1;
  }
 if (GetDebug()) 
   Info("SetFolder","name = %s Setting Event Folder named %s.",
         GetName(),eventfolder->GetName());

 fEventFolder = eventfolder;
 return 0;
}
/*****************************************************************************/ 

Int_t  AliDataLoader::SetFolder(TFolder* folder)
{

 if (folder == 0x0)
  {
    Error("SetFolder","Stupid joke. Argument is NULL");
    return 1;
  }
 
 if (GetDebug()) Info("SetFolder","name = %s Setting folder named %s.",GetName(),folder->GetName());
 
 fFolder = folder;
 TIter next(fBaseLoaders);
 AliBaseLoader* bl;

 while ((bl = (AliBaseLoader*)next()))
  {
   bl->SetDataLoader(this);
  }  

 return 0;
}
/******************************************************************/

TFolder* AliDataLoader::GetEventFolder()
{
//get EVENT folder (data that are changing from event to event, even in single run)
  if (GetDebug()) Info("GetEventFolder","EF = %#x");
  return fEventFolder;
}
/*****************************************************************************/ 

AliRunLoader* AliDataLoader::GetRunLoader()
{
//gets the run-loader from event folder
  AliRunLoader* rg = 0x0;
  TFolder* ef = GetEventFolder();
  if (ef == 0x0)
   {
     Error("GetRunLoader","Can not get event folder.");
     return 0;
   }
  rg = dynamic_cast<AliRunLoader*>(ef->FindObject(AliRunLoader::fgkRunLoaderName));
  return rg;
}

/*****************************************************************************/ 
void AliDataLoader::CloseFile()
{
  //closes file
  TIter next(fBaseLoaders);
  AliBaseLoader* bl;
  while ((bl = (AliBaseLoader*)next()))
   {
     if (bl->IsLoaded()) return;
   }
  
  if (GetDebug())
    Info("CloseFile","Closing and deleting (object) file.");
    
  delete fFile;
  fFile = 0x0;
  fDirectory = 0x0;
}
/*****************************************************************************/ 

void AliDataLoader::Clean()
{
  //Cleans main data
  GetBaseLoader(0)->Clean();
}  
/*****************************************************************************/ 

void AliDataLoader::CleanAll()
{
  //Cleans all folders and tasks
  TIter next(fBaseLoaders);
  AliBaseLoader* bl;
  while ((bl = (AliBaseLoader*)next()))
   {
      bl->Clean();
   }
}
/*****************************************************************************/ 

void AliDataLoader::SetFileNameSuffix(const TString& suffix)
{
  //adds the suffix before ".root", 
  //e.g. TPC.Digits.root -> TPC.DigitsMerged.root
  //made on Jiri Chudoba demand
  if (GetDebug()) 
   {
     Info("SetFileNameSuffix","suffix=%s",suffix.Data());
     Info("SetFileNameSuffix","   Digits File Name before: %s",fFileName.Data());
   }
   
  static TString dotroot(".root");
  const TString& suffixdotroot = suffix + dotroot;
  fFileName = fFileName.ReplaceAll(dotroot,suffixdotroot);

  if (GetDebug()) 
    Info("SetDigitsFileNameSuffix","                    after : %s",fFileName.Data());
}
/*****************************************************************************/ 

Bool_t AliDataLoader::CheckReload()
{
//checks if we have to reload given file
 if (fFile == 0x0) return kFALSE;
 TString tmp = SetFileOffset(fFileName);
 if (tmp.CompareTo(fFile->GetName())) return kTRUE;  //file must be reloaded
 return  kFALSE;
}
/*****************************************************************************/ 

const TString AliDataLoader::SetFileOffset(const TString& fname)
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

void AliDataLoader::SetFileOption(Option_t* newopt)
{
  //sets file option
  if (fFileOption.CompareTo(newopt) == 0) return;
  fFileOption = newopt;
  Reload();
}
/*****************************************************************************/ 

void AliDataLoader::SetCompressionLevel(Int_t cl)
{
//sets comression level for data defined by di
  fCompressionLevel = cl;
  if (fFile) fFile->SetCompressionLevel(cl);
}
/*****************************************************************************/ 

Int_t AliDataLoader::GetDebug() const 
{ 
 return (Int_t)AliLoader::fgDebug;
}
/*****************************************************************************/ 

void AliDataLoader::MakeTree()
{
  AliTreeLoader* tl = dynamic_cast<AliTreeLoader*>(fBaseLoaders->At(0));
  if (tl == 0x0)
   {
     Error("MakeTree","Can not make a tree because main base loader is not a tree loader");
     return;
   }
  tl->MakeTree();
}
/*****************************************************************************/ 

Bool_t AliDataLoader::IsFileWritable() const
{
//returns true if file is writable
 return (fFile)?fFile->IsWritable():kFALSE;
}
/*****************************************************************************/ 

Bool_t AliDataLoader::IsFileOpen() const
{
//returns true if file is writable
 return (fFile)?fFile->IsOpen():kFALSE;
}
/*****************************************************************************/ 

Bool_t AliDataLoader::IsOptionContrary(const TString& option) const
{
//Checks if passed option is contrary with file open option 
//which is passed option "writable" and existing option not wriable
//in reverse case it is no harm so it is NOT contrary
  if (fFile == 0x0) return kFALSE; //file is not opened - no problem
  
  if ( ( AliLoader::IsOptionWritable(option)      == kTRUE  ) &&     // passed option is writable and 
       ( AliLoader::IsOptionWritable(fFileOption) == kFALSE )    )   // existing one is not writable
    {
      return kTRUE;
    }

  return kFALSE;
}
/*****************************************************************************/ 
void AliDataLoader::AddBaseLoader(AliBaseLoader* bl)
{
//Adds a base loader to lits of base loaders managed by this data loader
//Managed data/task will be stored in proper root directory,
//and posted to 
// - in case of tree/object - data folder connected with detector associated with this data loader
// - in case of task - parental task which defined in this AliTaskLoader 

 if (bl == 0x0)
  {
    Warning("AddBaseLoader","Pointer is null.");
    return;
  }
 
 TObject* obj = fBaseLoaders->FindObject(bl->GetName());
 if (obj)
  {
    Error("AddBaseLoader","Can not add this base loader.");
    Error("AddBaseLoader","There exists already base loader which manages data named %s for this detector.");
    return;
  }
 
 
 fBaseLoaders->Add(bl);
}

/*****************************************************************************/ 

AliBaseLoader* AliDataLoader::GetBaseLoader(const TString& name) const
{
  return dynamic_cast<AliBaseLoader*>(fBaseLoaders->FindObject(name));
}
/*****************************************************************************/ 

AliBaseLoader* AliDataLoader::GetBaseLoader(Int_t n) const
{
 return dynamic_cast<AliBaseLoader*>(fBaseLoaders->At(n));
}
/*****************************************************************************/ 

TTree* AliDataLoader::Tree() const
{
//returns tree from the main base loader
//it is just shortcut method for comfort of user
//main storage object does not have to be Tree  - 
//that is why first we need to check if it is a TreeLoader 
 AliTreeLoader* tl = dynamic_cast<AliTreeLoader*>(GetBaseLoader(0));
 if (tl == 0x0) return 0x0;
 return tl->Tree();
}
/*****************************************************************************/ 

void  AliDataLoader::SetDirName(TString& dirname)
{
  if (GetDebug()) Info("SetDirName","FileName before %s",fFileName.Data());

  Int_t n = fFileName.Last('/');

  if (GetDebug()) Info("SetDirName","Slash found on pos %d",n);

  if (n > 0) fFileName = fFileName.Remove(0,n);

  if (GetDebug()) Info("SetDirName","Core FileName %s",fFileName.Data());

  fFileName = dirname + "/" + fFileName;

  if (GetDebug()) Info("SetDirName","FileName after %s",fFileName.Data());
}
/*****************************************************************************/ 
AliObjectLoader* AliDataLoader::GetBaseDataLoader()
{
 return dynamic_cast<AliObjectLoader*>(GetBaseLoader(kData));
}
/*****************************************************************************/ 
AliTaskLoader* AliDataLoader::GetBaseTaskLoader()
{
 return dynamic_cast<AliTaskLoader*>(GetBaseLoader(kTask));
}
/*****************************************************************************/ 
AliBaseLoader* AliDataLoader::GetBaseQALoader()
{
  return GetBaseLoader(kQA);
}
/*****************************************************************************/ 
AliTaskLoader* AliDataLoader::GetBaseQATaskLoader()
{
//returns pointer to QA base loader
 return dynamic_cast<AliTaskLoader*>(GetBaseLoader(kQATask));
}
/*****************************************************************************/ 
void AliDataLoader::SetBaseDataLoader(AliBaseLoader* bl)
{
//sets data base loader
  if (bl == 0x0)
   {
     Error("SetBaseDataLoader","Parameter is null");
     return;
   }
  if (GetBaseDataLoader()) delete GetBaseDataLoader();
  fBaseLoaders->AddAt(bl,kData);
}
/*****************************************************************************/ 
void AliDataLoader::SetBaseTaskLoader(AliTaskLoader* bl)
{
//sets Task base loader
  if (bl == 0x0)
   {
     Error("SetBaseTaskLoader","Parameter is null");
     return;
   }
  if (GetBaseTaskLoader()) delete GetBaseTaskLoader();
  fBaseLoaders->AddAt(bl,kTask);
}
/*****************************************************************************/ 
void AliDataLoader::SetBaseQALoader(AliBaseLoader* bl)
{
//sets QA base loader
  if (bl == 0x0)
   {
     Error("SetBaseQALoader","Parameter is null");
     return;
   }
  if (GetBaseQALoader()) delete GetBaseQALoader();
  fBaseLoaders->AddAt(bl,kQA);
}
/*****************************************************************************/ 
void AliDataLoader::SetBaseQATaskLoader(AliTaskLoader* bl)
{
//sets QA Task base loader
  if (bl == 0x0)
   {
     Error("SetBaseQATaskLoader","Parameter is null");
     return;
   }
  if (GetBaseQATaskLoader()) delete GetBaseQATaskLoader();
  fBaseLoaders->AddAt(bl,kQATask);
}

/*****************************************************************************/ 
/*****************************************************************************/ 
/*****************************************************************************/ 
//__________________________________________
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  class AliBaseLoader                                                      //
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
ClassImp(AliBaseLoader)

AliBaseLoader::AliBaseLoader():
 fIsLoaded(kFALSE),
 fDoNotReload(kFALSE),
 fDataLoader(0x0)
{
}
/*****************************************************************************/ 

AliBaseLoader::AliBaseLoader(const TString& name,  AliDataLoader* dl):
 TNamed(name,name+" Base Loader"),
 fIsLoaded(kFALSE),
 fDoNotReload(kFALSE),
 fDataLoader(dl)
{
}

/*****************************************************************************/ 

Int_t AliBaseLoader::Load(Option_t* opt)
{
  if (GetDebug())
    Info("Load","data type = %s, option = %s",GetName(),opt);

  if (Get())
   {
      Warning("Load","Data <<%s>> are already loaded. Use ReloadData to force reload. Nothing done",GetName());
      return 0;
   }
  
  Int_t retval;
  
  if (GetDataLoader()->IsFileOpen() == kTRUE)
   {
     if (GetDataLoader()->IsOptionContrary(opt) == kTRUE)
       {
         Error("Load","Data Type %s, Container Name %s", GetDataLoader()->GetName(),GetName());
         Error("Load","File was already opened in READ-ONLY mode, while now WRITEABLE access is requested.");
         Error("Load","Use AliDataLoader::SetOption to enforce change of access mode OR");
         Error("Load","Load previosly loaded data with coherent option.");
         return 10;
       }
   }
  else
   {
     retval = GetDataLoader()->OpenFile(opt);
     if (retval) 
      {
        Error("Load","Error occured while opening <<%s>> file",GetName());
        return retval;
      }
   }
  //if file is recreated there is no sense to search for data to post and get Error message
  if (AliLoader::TestFileOption(opt) == kFALSE)
   {
    AliTreeLoader* tl = dynamic_cast<AliTreeLoader*>(this);
    if (tl) tl->MakeTree();
    fIsLoaded = kTRUE;
    return 0;
   }

  retval = Post();
  if (retval)
   {
    Error("Load","Error occured while posting %s from file to folder.",GetName());
    return retval;
   }
  
  fIsLoaded = kTRUE;
  return 0;
}
/*****************************************************************************/ 

Int_t AliBaseLoader::Post()
{
//Posts data container to proper folders

  if ( GetDataLoader()->GetDirectory() == 0x0)
   {
     Error("Post","%s directory is NULL. Load before.",GetDataLoader()->GetName());
     return 2; 
   }
  
  TObject* data = GetDataLoader()->GetFromDirectory(fName);
  if(data)
   {
     //if such an obejct already exists - remove it first
     return Post(data);
   }
  else
   {
    //check if file is in update mode
    Int_t fileupdate = GetDataLoader()->GetFileOption().CompareTo("update",TString::kIgnoreCase);
    if ( fileupdate == 0)
     { //if it is, it is normal that there is no data yet
       if (GetDebug())
        {
         Info("Post","Can not find %s in file %s (file is opened in UPDATE mode).",
               GetName(),GetDataLoader()->GetFile()->GetName());
        }
     }
    else
     {
        Warning("Post","Can not find %s in file %s", GetName(),GetDataLoader()->GetFile()->GetName());
     }
   }
  return 0;
}
/*****************************************************************************/ 

Int_t AliBaseLoader::Post(TObject* data)
{
//Posts data container to proper folders
 if (data == 0x0)
  {
    Error("Post","Pointer to object is NULL");
    return 1;
  }
 TObject* obj = Get();
 if (data == obj)
  {
    if (GetDebug()) Warning("Post","This object was already posted.");
    return 0;
  }
 if (obj)
  {
    Warning("PostData","Object named %s already exitsts in data folder. Removing it",GetName());
    Clean();
  }
 return AddToBoard(data);
}
/*****************************************************************************/ 

Int_t AliBaseLoader::WriteData(Option_t* opt)
{
//Writes data defined by di object
//opt might be "OVERWRITE" in case of forcing overwriting
  if (GetDebug()) 
    Info("WriteData","Writing %s container for %s data. Option is %s.",
          GetName(),GetDataLoader()->GetName(),opt);
  
  TObject *data = Get();
  if(data == 0x0)
   {//did not get, nothing to write
     Warning("WriteData","Tree named %s not found in folder. Nothing to write.",GetName());
     return 0;
   }
  
  //check if file is opened
  if (GetDataLoader()->GetDirectory() == 0x0)
   { 
     //if not try to open
     GetDataLoader()->SetFileOption("UPDATE");
     if (GetDataLoader()->OpenFile("UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteData","Can not open hits file. %s ARE NOT WRITTEN",GetName());
        return 1;
      }
   }

  if (GetDataLoader()->IsFileWritable() == kFALSE)
   {
     Error("WriteData","File %s is not writable",GetDataLoader()->GetFileName().Data());
     return 2;
   }
  
  GetDataLoader()->cd(); //set the proper directory active

  //see if hits container already exists in this (root) directory
  TObject* obj = GetDataLoader()->GetFromDirectory(GetName());
  if (obj)
   { //if they exist, see if option OVERWRITE is used
     const char *oOverWrite = strstr(opt,"OVERWRITE");
     if(!oOverWrite)
      {//if it is not used -  give an error message and return an error code
        Error("WriteData","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  
  if (GetDebug()) Info("WriteData","DataName = %s, opt = %s, data object name = %s",
                   GetName(),opt,data->GetName());
  if (GetDebug()) Info("WriteData","File Name = %s, Directory Name = %s Directory's File Name = %s",
                   GetDataLoader()->GetFile()->GetName(),GetDataLoader()->GetDirectory()->GetName(),
                   GetDataLoader()->GetDirectory()->GetFile()->GetName());
  
  if (GetDebug()) Info("WriteData","Writing tree");
  data->Write(0,TObject::kOverwrite);

  fIsLoaded = kTRUE;  // Just to ensure flag is on. Object can be posted manually to folder structure, not using loader.

  return 0;
 
}
/*****************************************************************************/ 

Int_t AliBaseLoader::Reload()
{
//Unloads and loads datat again - if loaded before
 if (IsLoaded())
  {
    Unload();
    return Load(GetDataLoader()->GetFileOption());
  }
  return 0;
}
/*****************************************************************************/ 

void AliBaseLoader::Clean()
{
//removes objects from folder/task
  if (GetDebug()) Info("Clean","%s %s",GetName(),GetDataLoader()->GetName());
  TObject* obj = Get();
  if(obj)
   { 
     if (GetDebug()) 
       Info("Clean","cleaning %s.",GetName());
     RemoveFromBoard(obj);
     delete obj;
   }
}
/*****************************************************************************/ 

void AliBaseLoader::Unload()
{
  Clean();
  fIsLoaded = kFALSE;
  GetDataLoader()->CloseFile();
}
/*****************************************************************************/ 
AliDataLoader* AliBaseLoader::GetDataLoader() const
{
 if (fDataLoader == 0x0) 
  {
    Fatal("GetDataLoader","Pointer to Data Loader is NULL");
  }
 return fDataLoader;
}
/*****************************************************************************/ 

Int_t AliBaseLoader::GetDebug() const 
{ 
 return (Int_t)AliLoader::fgDebug;
}

/*****************************************************************************/ 
/*****************************************************************************/ 
/*****************************************************************************/ 
//__________________________________________
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  class AliObjectLoader                                                      //
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

ClassImp(AliObjectLoader)

AliObjectLoader::AliObjectLoader(const TString& name, AliDataLoader* dl):
 AliBaseLoader(name,dl)
{
//constructor
}
/*****************************************************************************/ 

TFolder* AliObjectLoader::GetFolder() const
{
  TFolder* df = GetDataLoader()->GetFolder();
  if (df == 0x0)
   {
     Fatal("GetFolder","Data Folder is NULL");
   }
  return df;
}
/*****************************************************************************/ 

void AliObjectLoader::RemoveFromBoard(TObject* obj)
{
  GetFolder()->Remove(obj);
}
/*****************************************************************************/ 
Int_t AliObjectLoader::AddToBoard(TObject* obj)
{
  GetFolder()->Add(obj);
  return 0;
}
/*****************************************************************************/ 

TObject* AliObjectLoader::Get() const
{
  return (GetFolder()) ? GetFolder()->FindObject(GetName()) : 0x0;
}

/*****************************************************************************/ 
/*****************************************************************************/ 
/*****************************************************************************/ 
//__________________________________________
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  class AliTreeLoader                                                      //
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

ClassImp(AliTreeLoader)

AliTreeLoader::AliTreeLoader(const TString& name, AliDataLoader* dl):
 AliObjectLoader(name,dl)
{
//constructor
}

/*****************************************************************************/ 

Int_t AliTreeLoader::WriteData(Option_t* opt)
{
//Writes data defined by di object
//opt might be "OVERWRITE" in case of forcing overwriting

  if (GetDebug()) 
    Info("WriteData","Writing %s container for %s data. Option is %s.",
          GetName(),GetDataLoader()->GetName(),opt);

  TObject *data = Get();
  if(data == 0x0)
   {//did not get, nothing to write
     Warning("WriteData","Tree named %s not found in folder. Nothing to write.",GetName());
     return 0;
   }
  
  //check if file is opened
  if (GetDataLoader()->GetDirectory() == 0x0)
   { 
     //if not try to open
     GetDataLoader()->SetFileOption("UPDATE");
     if (GetDataLoader()->OpenFile("UPDATE"))
      {  
        //oops, can not open the file, give an error message and return error code
        Error("WriteData","Can not open hits file. %s ARE NOT WRITTEN",GetName());
        return 1;
      }
   }

  if (GetDataLoader()->IsFileWritable() == kFALSE)
   {
     Error("WriteData","File %s is not writable",GetDataLoader()->GetFileName().Data());
     return 2;
   }
  
  GetDataLoader()->cd(); //set the proper directory active

  //see if hits container already exists in this (root) directory
  TObject* obj = GetDataLoader()->GetFromDirectory(GetName());
  if (obj)
   { //if they exist, see if option OVERWRITE is used
     const char *oOverWrite = strstr(opt,"OVERWRITE");
     if(!oOverWrite)
      {//if it is not used -  give an error message and return an error code
        Error("WriteData","Tree already exisists. Use option \"OVERWRITE\" to overwrite previous data");
        return 3;
      }
   }
  
  if (GetDebug()) Info("WriteData","DataName = %s, opt = %s, data object name = %s",
                   GetName(),opt,data->GetName());
  if (GetDebug()) Info("WriteData","File Name = %s, Directory Name = %s Directory's File Name = %s",
                   GetDataLoader()->GetFile()->GetName(),GetDataLoader()->GetDirectory()->GetName(),
                   GetDataLoader()->GetDirectory()->GetFile()->GetName());
  
  //if a data object is a tree set the directory
  TTree* tree = dynamic_cast<TTree*>(data);
  if (tree) tree->SetDirectory(GetDataLoader()->GetDirectory()); //forces setting the directory to this directory (we changed dir few lines above)
  
  if (GetDebug()) Info("WriteData","Writing tree");
  data->Write(0,TObject::kOverwrite);

  fIsLoaded = kTRUE;  // Just to ensure flag is on. Object can be posted manually to folder structure, not using loader.
  
  return 0;
 
}
/*****************************************************************************/ 

void AliTreeLoader::MakeTree()
{
//this virtual method creates the tree for hits in the file
  if (Tree()) 
   {
    if (GetDebug()) 
      Info("MakeTree","name = %s, Data Name = %s Tree already exists.",
            GetName(),GetDataLoader()->GetName());
    return;//tree already made 
   }
  if (GetDebug()) 
    Info("MakeTree","Making Tree named %s.",GetName());
   
  TString dtypename(GetDataLoader()->GetName());
  TTree* tree = new TTree(GetName(), dtypename + " Container"); //make a tree
  if (tree == 0x0)
   {
     Error("MakeTree","Can not create %s tree.",GetName());
     return;
   }
  tree->SetAutoSave(1000000000); //no autosave
  GetFolder()->Add(tree);
  WriteData("OVERWRITE");//write tree to the file
}


/*****************************************************************************/ 
/*****************************************************************************/ 
/*****************************************************************************/ 
//__________________________________________
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  class AliTaskLoader                                                      //
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

ClassImp(AliTaskLoader)

AliTaskLoader::AliTaskLoader(const TString& name, AliDataLoader* dl, TTask* parentaltask):
 AliBaseLoader(name,dl),
 fParentalTask(parentaltask)
{
//constructor
}

/*****************************************************************************/ 

void AliTaskLoader::RemoveFromBoard(TObject* obj)
{
  GetParentalTask()->GetListOfTasks()->Remove(obj);
}
/*****************************************************************************/ 
Int_t AliTaskLoader::AddToBoard(TObject* obj)
{
  TTask* task = dynamic_cast<TTask*>(obj);
  if (task == 0x0)
   {
     Error("AddToBoard","To TTask board can be added only tasks.");
     return 1;
   }
  GetParentalTask()->Add(task);
  return 0;
}

TObject* AliTaskLoader::Get() const
{
  return (GetParentalTask()) ? GetParentalTask()->GetListOfTasks()->FindObject(GetName()) : 0x0;
}

/*****************************************************************************/ 

TTask* AliTaskLoader::GetParentalTask() const
{
//returns parental tasks for this task
  if (fParentalTask) return fParentalTask;

//  Error("GetParentalTask","!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
//  Error("GetParentalTask","Don't Foget to Implement SKOWRON");
//  Error("GetParentalTask","!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
  return 0x0;
}

/*****************************************************************************/ 

/*****************************************************************************/ 
/*****************************************************************************/ 
/*****************************************************************************/ 


