#ifndef ALIDATALOADER_H
#define ALIDATALOADER_H
//__________________________________________
////////////////////////////////////////////
//                                        //
//  class AliDataLoader                   //
//                                        //
//  Loader responsible for one data type  //
//  i.e. Hits, Kine, etc.                 //
//  many objects type can be assciated    //
//  with one data type: storing object    //
//  (usually tree), task producing it,    //
//  Quality Assurance(QA), QA Task, and   //
//  others.                               //
//                                        //
//                                        //
////////////////////////////////////////////
#include <TNamed.h>
#include <TString.h>
#include <TTree.h>
#include <TTask.h>
#include <TDirectory.h>

class AliLoader;
class TFile;
class TFolder;
class AliRunLoader;
class AliBaseLoader;
class AliObjectLoader;
class AliTreeLoader;
class AliTaskLoader;

class AliDataLoader: public TNamed
 {
  public:
   AliDataLoader();
   AliDataLoader(const char* filename, const char* contname, const char* name, Option_t* opt = "t");
   virtual ~AliDataLoader();

   virtual Int_t      SetEvent();
   virtual Int_t      GetEvent();

   //shrtcuts method to basic data base loader 0  
   virtual Int_t      Load(Option_t* opt="");
   virtual void       Unload();
   virtual Int_t      Reload(); 
   virtual Int_t      WriteData(Option_t* opt="");
   virtual TTree*     Tree() const;
   virtual void       Clean();
   virtual void       MakeTree();
   virtual Int_t      OpenFile(Option_t* opt);

   virtual void       CloseFile();
   void               UnloadAll();
   void               CleanAll();
   const TString&     GetFileName() const {return fFileName;}
   TFile*             GetFile() const {return fFile;}
   TDirectory*        GetDirectory() const {return fDirectory;}
   const TString&     GetFileOption() const {return fFileOption;}
   const Int_t&       GetCompressionLevel() const {return fCompressionLevel;}
   
   Bool_t             cd(){return (fDirectory)?fDirectory->cd():kFALSE;}
   
   virtual void       SetFileName(const TString& filename){fFileName = filename;}
   virtual void       SetFileOption(const Option_t* fileopt);
   virtual void       SetCompressionLevel(Int_t cl);

   Int_t              SetEventFolder(TFolder* eventfolder);//sets the event folder
   Int_t              SetFolder(TFolder* folder);//sets the data folder ??????
   TFolder*           GetEventFolder();
   TFolder*           GetFolder() const {return fFolder;}
   
//   TObject*           GetFromDirectory(const char *name){return (fDirectory)?fDirectory->Get(name):0x0;}
   void               SetFileNameSuffix(const TString& suffix);//adds the suffix before ".root", 
                                                               //e.g. TPC.Digits.root -> TPC.DigitsMerged.root
                                                               //made on Jiri Chudoba demand
   const TString      SetFileOffset(const TString& fname);//adds the proper number before .root extension suffix
   void               SetDirName(TString& dirname);

   void               AddBaseLoader(AliBaseLoader* bl);
   enum EStdBasicLoaders {kData = 0,kTask,kQA,kQATask};//standard basic loaders identifiers

   AliBaseLoader*     GetBaseLoader(const TString& name) const;
   AliBaseLoader*     GetBaseLoader(Int_t n) const;
   AliObjectLoader*   GetBaseDataLoader();
   AliTaskLoader*     GetBaseTaskLoader();
   AliBaseLoader*     GetBaseQALoader();
   AliTaskLoader*     GetBaseQATaskLoader();
   
   void               SetBaseDataLoader(AliBaseLoader* bl);
   void               SetBaseTaskLoader(AliTaskLoader* bl);
   void               SetBaseQALoader(AliBaseLoader* bl);
   void               SetBaseQATaskLoader(AliTaskLoader* bl);
   
   Bool_t             CheckReload();//checks if we have to reload given file
   Bool_t             IsFileWritable() const;
   Bool_t             IsFileOpen() const;
   Bool_t             IsOptionContrary(const TString& option) const;
  protected:
   Int_t              GetDebug() const;
   AliRunLoader*      GetRunLoader();//gets the run-loader from event folder

  private:
      
   TString      fFileName; //name of the file 
   TFile*       fFile; //! pointer to file 
   TDirectory*  fDirectory; //!pointer to TDirectory
   TString      fFileOption; //!file option while opened 
   Int_t        fCompressionLevel; //Compression Level of File
   
   TObjArray*   fBaseLoaders;//base loaders
   Bool_t       fHasTask;// flag if has a task
   TString      fTaskName;// name of the task
   TTask*       fParentalTask;//Parental task

   TFolder*     fEventFolder;//!event folder
   TFolder*     fFolder;//! folder with data
   
  public:
   ClassDef(AliDataLoader,1)
 };


//__________________________________________
////////////////////////////////////////////
//                                        //
//  class AliBaseLoader                   //
//                                        //
//                                        //
////////////////////////////////////////////


class AliBaseLoader: public TNamed
{
  public:
    AliBaseLoader();
    AliBaseLoader(const TString& name, AliDataLoader* dl, Bool_t storeontop = kFALSE);
    
    virtual ~AliBaseLoader(){};
     
    virtual Int_t      Load(Option_t* opt="");
    virtual void       Unload();
    virtual Int_t      Reload();
    virtual Int_t      WriteData(Option_t* opt="");
    virtual void       Clean();
    virtual Int_t      Post();
    virtual Int_t      Post(TObject* data);
    virtual TObject*   Get() const = 0; 
    Bool_t             IsLoaded()const{return fIsLoaded;}
    void               SetDataLoader(AliDataLoader* dl){fDataLoader = dl;}
    void               SetEventFolder(TFolder* ef){;}
    void               SetDoNotReload(Bool_t flag){fDoNotReload = flag;}
    Bool_t             DoNotReload() const {return fDoNotReload;}
    TDirectory*        GetDirectory();//returns pointer to directory where data are stored. 
    TObject*           GetFromDirectory(const char *name){return (GetDirectory())?GetDirectory()->Get(name):0x0;}    
   protected:
    
    virtual Int_t      AddToBoard(TObject* obj) = 0;//add to white board - board can be TTask or TFolder
    virtual void       RemoveFromBoard(TObject* obj) = 0;
    
    AliDataLoader*     GetDataLoader() const;
    Int_t              GetDebug() const;

    Bool_t             fIsLoaded;    //!  flag indicating if data are loaded
    Bool_t             fStoreInTopOfFile;// if true, data are stored in top of file ->Indicates fDoNotReload == kTRUE

   private:
    Bool_t             fDoNotReload; // if this flag is on object is not reloaded while GetEvent is called.
                                     //Specially important for tasks. Task loops over events while producing data, 
	                 //and has a base loader which writes it to file every processed event.
	                 //If this flag is not on, while taking next event, loader deletes task
	                 // and tries to get new one from file
    AliDataLoader*     fDataLoader;  //! pointer to Data Loader this Base Loader belongs to

 ClassDef(AliBaseLoader,1)    
};

//__________________________________________
////////////////////////////////////////////
//                                        //
//  class AliObjectLoader                 //
//                                        //
//                                        //
////////////////////////////////////////////

class AliObjectLoader: public AliBaseLoader
 {
   public:
     AliObjectLoader(){};
     AliObjectLoader(const TString& name, AliDataLoader* dl, Bool_t storeontop = kFALSE);
     virtual          ~AliObjectLoader(){};
     TObject*          Get() const;

   protected:
     TFolder*          GetFolder() const;
     Int_t             AddToBoard(TObject* obj);
     void              RemoveFromBoard(TObject* obj);

 ClassDef(AliObjectLoader,1)    
  
 };

//__________________________________________
////////////////////////////////////////////
//                                        //
//  class AliTreeLoader                   //
//                                        //
//                                        //
////////////////////////////////////////////

class AliTreeLoader: public AliObjectLoader
 {
   public:
     AliTreeLoader(){};
     AliTreeLoader(const TString& name, AliDataLoader* dl, Bool_t storeontop = kFALSE);
     virtual ~AliTreeLoader(){};
     
     virtual TTree*     Tree() const {return dynamic_cast<TTree*>(Get());}
     virtual void       MakeTree();
     virtual Int_t      WriteData(Option_t* opt="");

   ClassDef(AliTreeLoader,1)    
 };

//__________________________________________
////////////////////////////////////////////
//                                        //
//  class AliTaskLoader                   //
//                                        //
//                                        //
////////////////////////////////////////////
 
class AliTaskLoader: public AliBaseLoader
 {
  public:
    AliTaskLoader():fParentalTask(0x0){};
    AliTaskLoader(const TString& name, AliDataLoader* dl, TTask* parentaltask, Bool_t storeontop = kFALSE);
    virtual ~AliTaskLoader(){};
    
    TObject*           Get() const; 
    virtual TTask*     Task() const {return dynamic_cast<TTask*>(Get());}

  protected:
    Int_t              AddToBoard(TObject* obj);
    void               RemoveFromBoard(TObject* obj);
    TTask*             GetParentalTask() const;

  private:
    TTask*             fParentalTask;

  ClassDef(AliTaskLoader,1)    
 };

#endif


