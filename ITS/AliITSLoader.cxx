#include "AliITSLoader.h"
#include <AliRunLoader.h>

#include <TTree.h>
#include <TFile.h>

const TString AliITSLoader::fgkDefaultRawClustersContainerName = "TreeC";
const TString AliITSLoader::fgkDefaultBackTracksContainerName = "TreeB";

ClassImp(AliITSLoader)

/*****************************************************************************/ 
AliITSLoader::AliITSLoader(const Char_t *name,const Char_t *topfoldername):
 AliLoader(name,topfoldername),
 fRawClustersDataLoader(fDetectorName + ".RawCl.root",fgkDefaultRawClustersContainerName,"Raw Clusters"),
 fBackTracksDataLoader(fDetectorName + ".BackTracks.root",fgkDefaultBackTracksContainerName,"Back Propagated Tracks")
{
//ctor   
   fDataLoaders->Add(&fRawClustersDataLoader);
   fRawClustersDataLoader.SetEventFolder(fEventFolder);
   fRawClustersDataLoader.SetFolder(GetDetectorDataFolder());

   fDataLoaders->Add(&fBackTracksDataLoader);
   fBackTracksDataLoader.SetEventFolder(fEventFolder);
   fBackTracksDataLoader.SetFolder(GetDetectorDataFolder());
   
}
/*****************************************************************************/ 

AliITSLoader::AliITSLoader(const Char_t *name,TFolder *topfolder):
 AliLoader(name,topfolder),
 fRawClustersDataLoader(fDetectorName + ".RawCl.root",fgkDefaultRawClustersContainerName,"Raw Clusters")
{
//ctor   
   fDataLoaders->Add(&fRawClustersDataLoader);
   fRawClustersDataLoader.SetEventFolder(fEventFolder);
   fRawClustersDataLoader.SetFolder(GetDetectorDataFolder());

   fDataLoaders->Add(&fRawClustersDataLoader);
   fRawClustersDataLoader.SetEventFolder(fEventFolder);
   fRawClustersDataLoader.SetFolder(GetDetectorDataFolder());

}
/*****************************************************************************/ 
AliITSLoader::~AliITSLoader()
{
 //destructor
  UnloadRawClusters();
  fDataLoaders->Remove(&fRawClustersDataLoader);

  UnloadBackTracks();
  fDataLoaders->Remove(&fBackTracksDataLoader);
}

void AliITSLoader::MakeTree(Option_t *opt){
  // invokes AliLoader::MakeTree + specific ITS tree(s)
  // Valid options: H,S,D,R,T and C (C=raw clusters)
  AliLoader::MakeTree(opt);
  const char *oC = strstr(opt,"C");
  if (oC) MakeRawClustersContainer();

  const char *oB = strstr(opt,"B");
  if (oB) MakeBackTracksContainer();
  
}
