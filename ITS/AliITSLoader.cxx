#include <Riostream.h>
#include "AliITSLoader.h"
#include <AliRunLoader.h>
#include <TTree.h>
#include <TFile.h>

const TString AliITSLoader::fgkDefaultRawClustersContainerName = "TreeC";

ClassImp(AliITSLoader)

/*****************************************************************************/ 
AliITSLoader::AliITSLoader(const Char_t *name,const Char_t *topfoldername):
 AliLoader(name,topfoldername),
 fRawClustersInfo(fDetectorName + ".RawCl.root",fgkDefaultRawClustersContainerName,"Raw Clusters")
{
}
/*****************************************************************************/ 

AliITSLoader::AliITSLoader(const Char_t *name,TFolder *topfolder):
 AliLoader(name,topfolder),
 fRawClustersInfo(fDetectorName + ".RawCl.root",fgkDefaultRawClustersContainerName,"Raw Clusters")
{
}
/*****************************************************************************/ 

void AliITSLoader::CleanFolders() {
  // Cleans all posted objects
  AliLoader::CleanFolders();
  CleanRawClusters();
}

/*****************************************************************************/ 
Int_t AliITSLoader::GetEvent() {
  //changes to proper root directory and tries to load data from files to folders
  // 
  // 
  //returns:
  //     0  - in case of no error
  //     1  - event not found
  //     
   
  Int_t rval = AliLoader::GetEvent();
  if( rval != 0) return rval;
  if(fRawClustersInfo.File()) PostRawClusters();
  return 0;
}

/*****************************************************************************/ 
/*****************************************************************************/ 

void AliITSLoader::MakeTree(Option_t *opt){
  // invokes AliLoader::MakeTree + specific ITS tree(s)
  // Valid options: H,S,D,R,T and C (C=raw clusters)
  AliLoader::MakeTree(opt);
  const char *oC = strstr(opt,"C");
  if (oC) MakeRawClustersContainer();
}
/*****************************************************************************/ 

void AliITSLoader::SetCompressionLevel(Int_t cl){
  // Sets compression level for all the opened files
  AliLoader::SetCompressionLevel(cl);
  if(fRawClustersInfo.File()) fRawClustersInfo.File()->SetCompressionLevel(cl);
}
/*****************************************************************************/ 

Int_t AliITSLoader::SetEvent() {
  //basically the same that GetEvent but do not post data to folders

  Int_t rval = AliLoader::SetEvent();
  if( rval != 0) return rval;

  AliRunLoader* rg = GetRunLoader();
  Int_t evno = rg->GetEventNumber();

  if(fRawClustersInfo.File()) 
   {
     if (CheckReload(fRawClustersInfo.File(),fRawClustersInfo.FileName()))
      {
        UnloadRawClusters();
        Info("SetEvent","Reloading new file for Raw Clusters. file opt is %s",
              fRawClustersInfo.FileOption().Data());
        OpenRawClustersFile(fRawClustersInfo.FileOption());
      }
     fRawClustersInfo.Directory() = ChangeDir(fRawClustersInfo.File(),evno);
     if (fRawClustersInfo.Directory() == 0x0)
        {
          Error("SetEvent","Can not chage directory to file %s",fRawClustersInfo.File()->GetName());
          return 1;
        }
   }
  return 0;
}
