#ifndef ALIITSGETTER_H
#define ALIITSGETTER_H



#include <AliLoader.h>



class AliITSLoader: public AliLoader
 {
   public:
    AliITSLoader(){};
    AliITSLoader(const Char_t *name,const Char_t *topfoldername);
    AliITSLoader(const Char_t *name,TFolder *topfolder);
    
    virtual ~AliITSLoader(){UnloadRawClusters();}

    virtual Int_t  GetEvent();//changes to root directory
    virtual void   CleanFolders();
    const TString& GetRawClustersContainerName() { return fRawClustersInfo.ContainerName();}
    virtual void   CleanRawClusters() {Clean(fRawClustersInfo);}
    Int_t          LoadRawClusters(Option_t* opt=""){return LoadData(fRawClustersInfo,opt);}
    void           MakeTree(Option_t* opt);
    virtual void   SetCompressionLevel(Int_t cl);
    void           SetRawClustersFileName(const TString& fname){fRawClustersInfo.FileName() = fname;}
    virtual Int_t  SetEvent();
    TTree*         TreeC(){ return Tree(fRawClustersInfo);} // returns a pointer to the tree of  RawClusters
    void           UnloadRawClusters(){CleanRawClusters(); CloseRawClustersFile();}
    virtual Int_t  WriteRawClusters(Option_t* opt=""){return WriteData(fRawClustersInfo,opt);}


   protected:

    // METHODS
    void           CloseRawClustersFile(){CloseDataFile(fRawClustersInfo);}
    virtual void   MakeRawClustersContainer() {AliLoader::MakeTree(fRawClustersInfo);}
    Int_t          OpenRawClustersFile(Option_t *opt){return OpenDataFile(fRawClustersInfo,opt);}
    Int_t          PostRawClusters(){return PostData(fRawClustersInfo);}

    // DATA
    AliLoaderDataInfo fRawClustersInfo;
    static const TString fgkDefaultRawClustersContainerName;

   public:
     ClassDef(AliITSLoader,1)
 };
 
#endif


