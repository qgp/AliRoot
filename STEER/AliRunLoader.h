#ifndef ALIRUNLoader_H
#define ALIRUNLoader_H

//___________________________________________________________________
/////////////////////////////////////////////////////////////////////
//                                                                 //
//  class AliRunLoader                                             //
//                                                                 //
//  This class aims to be the only one interface for manging data  //
//  It stores Loaders for all modules which knows the filenames    //
//  of the data files to be stored.                                //
//  It aims to substitude AliRun in automatic managing of data     //
//  positioning thus there won't be necessity of loading gAlice    //
//  from file in order to get fast access to the data              //
//                                                                 //
//  Logical place to put the specific Loader to the given          //
//  detector is detector  itself (i.e ITSLoader in ITS).           //
//  But, to load detector object one need to load gAlice, and      //
//  by the way all other detectors with their geometrieces and     //
//  so on. So, if one need to open TPC clusters there is no        //
//  principal nedd to read everything.                             //
//                                                                 //
/////////////////////////////////////////////////////////////////////

#include <TNamed.h>
#include <TFile.h>
#include "AliConfig.h"
#include "AliLoader.h"

class TString;
class TFolder;
class TObjArray;
class TTree;
class TTask;
class TParticle;

class AliRun;
class AliLoader;
class AliDetector;
class AliHeader;
#include "AliStack.h"
class AliRunDigitizer;


class AliRunLoader: public TNamed
{
  public:
    
    AliRunLoader();
    AliRunLoader(const char* topfoldername);
    AliRunLoader(TFolder* topfolder);
    
    virtual ~AliRunLoader();
    
    static AliRunLoader* Open(const char* filename = "galice.root",
                              const char* eventfoldername = AliConfig::fgkDefaultEventFolderName,
	          Option_t* option = "READ");

    Int_t       GetEventNumber() const {return fCurrentEvent;}

    Int_t       GetEvent(const Int_t evno, const char * opt= "HSDRP");//sets the event number and reloads data in folders properly, 
                                                             //with the possibility to select which kind of data to laod
    Int_t       GetNextEvent(){return GetEvent(fCurrentEvent+1);}//gets next event 
    Int_t       SetEventNumber(Int_t evno); //cleans folders and sets the root dirs in files (do not reload data)
    Int_t       SetNextEvent(){return SetEventNumber(fCurrentEvent+1);}
    
    Int_t       GetNumberOfEvents();
    
    void        MakeTree(Option_t *option);
    void        MakeHeader();
    void        MakeStack();
    
    Int_t       LoadgAlice();
    Int_t       LoadHeader();
    Int_t       LoadKinematics(Option_t* option = "READ");
    Int_t       LoadTrackRefs(Option_t* option = "READ");
    
    void        UnloadHeader();
    void        UnloadKinematics();
    void        UnloadgAlice();
    void        UnloadTrackRefs();
    
    void        SetKineFileName(const TString& fname){fKineData.FileName() = fname;}
    TString&    GetKineFileName() {return fKineData.FileName() ; } const 
    void        SetTrackRefsFileName(const TString& fname){fTrackRefsData.FileName() = fname;}
    
    TTree*      TreeE() const; //returns the tree from folder; shortcut method
    AliHeader*  GetHeader() const;
    
    AliStack*   Stack() const {return fStack;}
    Int_t       GetNtrack() const { return Stack()->GetNtrack() ; } 
    TParticle * Particle(const Int_t i) const { return  Stack()->Particle(i) ; } 
    TTree*      TreeK() const; //returns the tree from folder; shortcut method
    TTree*      TreeTR() const; //returns the tree from folder; shortcut method    
    
    AliRun*     GetAliRun()const;
        
    Int_t       WriteGeometry(Option_t* opt="");
    Int_t       WriteHeader(Option_t* opt="");
    Int_t       WriteAliRun(Option_t* opt="");
    Int_t       WriteKinematics(Option_t* opt="");
    Int_t       WriteTrackRefs(Option_t* opt="");
    Int_t       WriteRunLoader(Option_t* opt="");
    
    Int_t       WriteHits(Option_t* opt=""); 
    Int_t       WriteSDigits(Option_t* opt="");
    Int_t       WriteDigits(Option_t* opt="");
    Int_t       WriteRecPoints(Option_t* opt="");
    Int_t       WriteTracks(Option_t* opt="");
    
    Int_t       LoadHits(Option_t* detectors = "all",Option_t* opt = "RAED");
    Int_t       LoadSDigits(Option_t* detectors = "all",Option_t* opt = "RAED");
    Int_t       LoadDigits(Option_t* detectors = "all",Option_t* opt = "RAED");
    Int_t       LoadRecPoints(Option_t* detectors = "all",Option_t* opt = "RAED");
    Int_t       LoadTracks(Option_t* detectors = "all",Option_t* opt = "RAED");
    
    
    void        AddLoader(AliLoader* loader);
    void        AddLoader(AliDetector* det);
    AliLoader*  GetLoader(const char* detname) const;
    AliLoader*  GetLoader(AliDetector* det) const;
    Int_t       SetEventFolderName(const TString& name = AliConfig::fgkDefaultEventFolderName);//sets top folder name for this run; of alread
    void        CleanFolders();//removes all abjects from folder structure
    void        CleanDetectors();
    void        CleanKinematics(){Clean(fgkKineContainerName);}
    void        CleanTrackRefs(){Clean(fgkTrackRefsContainerName);}
    
    void        RemoveEventFolder(); //remove folder structure from top folder 
    void        SetCompressionLevel(Int_t cl);
    void        SetKineComprLevel(Int_t cl);
    void        SetTrackRefsComprLevel(Int_t cl);

    TFolder*    GetEventFolder() const {return fEventFolder;}
    void        CdGAFile();

    void        MakeTrackRefsContainer();
    void        SetDirName(TString& dirname);
    Int_t       GetFileOffset() const;
    void        SetNumberOfEventsPerFile(Int_t nevpf){fNEventsPerFile = nevpf;}
    
    void        SetDigitsFileNameSuffix(const TString& suffix);//adds the suffix before ".root", 
                                                               //e.g. TPC.Digits.root -> TPC.DigitsMerged.root
                                                               //made on Jiri Chudoba demand
    TString     GetFileName() const;//returns name of galice file
    const TObjArray* GetArrayOfLoaders() const {return fLoaders;}
    Int_t GetDebug() const {return (Int_t)AliLoader::AliLoader::fgDebug;}
    void cd(){fgRunLoader = this;}

  protected:
    /**********************************************/
    /************    PROTECTED      ***************/
    /*********        D A T A          ************/
    /**********************************************/

    TObjArray     *fLoaders;          //  List of Detectors
    TFolder       *fEventFolder;      //!top folder for this run
    
    Int_t          fCurrentEvent;//!Number of current event
    
    TFile         *fGAFile;//!  pointer to main file with AliRun and Run Loader -> galice.root 
    AliHeader     *fHeader;//!  pointer to header
    AliStack      *fStack; //!  pointer to stack
    
    AliLoaderDataInfo fKineData;//data connected to kinematics data
    AliLoaderDataInfo fTrackRefsData;//data connected to track reference data
    
    Int_t          fNEventsPerFile;  //defines number of events stored per one file
    
    static const TString   fgkDefaultKineFileName;//default file name with kinamatics
    static const TString   fgkDefaultTrackRefsFileName;//default file name with kinamatics


    /*********************************************/
    /************    PROTECTED      **************/
    /*********     M E T H O D S       ***********/
    /*********************************************/

    void           SetGAliceFile(TFile* gafile);//sets the pointer to gAlice file
    Int_t          PostKinematics();
    Int_t          PostTrackRefs();
    Int_t          OpenKineFile(Option_t* opt);
    Int_t          OpenTrackRefsFile(Option_t* opt);

    Int_t          OpenDataFile(const TString& filename,TFile*& file,TDirectory*& dir,Option_t* opt,Int_t cl);
    void           SetUnixDir(const TString& udirname);
    const TString  SetFileOffset(const TString& fname);//adds the proper number before .root

  private:
    void  GetListOfDetectors(const char * namelist,TObjArray& pointerarray) const;

    void  CleanHeader(){Clean(fgkHeaderContainerName);}
    void  Clean(const TString& name);
    
    Int_t SetEvent();
   
    static AliRunLoader* fgRunLoader;

  public:
  /******************************************/
  /*****   Public S T A T I C Stuff   *******/
  /******************************************/
    static AliRunLoader* GetRunLoader(const char* eventfoldername);
    static AliRunLoader* GetRunLoader(){return fgRunLoader;}

//    static AliRunDigitizer* GetRunDigitizer();
//  Tasks are supposed to be singletons, that is why static
    static TTask*           GetRunDigitizer();        //
    static TTask*           GetRunSDigitizer();       //
    static TTask*           GetRunReconstructioner(); //
    static TTask*           GetRunTracker();          //
    static TTask*           GetRunQATask();           // 
    static const TString   fgkRunLoaderName;
    static const TString   fgkHeaderContainerName;    //default name of the kinematics container (TREE) name - TreeE
    static const TString   fgkKineContainerName;      //default name of the kinematics container (TREE) name - TreeK
    static const TString   fgkTrackRefsContainerName; //default name of the track references container (TREE) name - TreeTR
    static const TString   fgkHeaderBranchName;
    static const TString   fgkKineBranchName;
    static const TString   fgkGAliceName;
    
    ClassDef(AliRunLoader,1)
};

#endif
