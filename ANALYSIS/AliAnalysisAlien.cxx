/**************************************************************************
 * Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

// Author: Mihaela Gheata, 01/09/2008

//==============================================================================
//   AliAnalysisAlien - AliEn utility class. Provides interface for creating
// a personalized JDL, finding and creating a dataset.
//==============================================================================

#include "Riostream.h"
#include "TEnv.h"
#include "TBits.h"
#include "TError.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TFileCollection.h"
#include "TChain.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TGrid.h"
#include "TGridResult.h"
#include "TGridCollection.h"
#include "TGridJDL.h"
#include "TGridJobStatusList.h"
#include "TGridJobStatus.h"
#include "TFileMerger.h"
#include "AliAnalysisManager.h"
#include "AliVEventHandler.h"
#include "AliAnalysisDataContainer.h"
#include "AliAnalysisAlien.h"

ClassImp(AliAnalysisAlien)

//______________________________________________________________________________
AliAnalysisAlien::AliAnalysisAlien()
                 :AliAnalysisGrid(),
                  fGridJDL(NULL),
                  fMergingJDL(NULL),
                  fPrice(0),
                  fTTL(0),
                  fSplitMaxInputFileNumber(0),
                  fMaxInitFailed(0),
                  fMasterResubmitThreshold(0),
                  fNtestFiles(0),
                  fNrunsPerMaster(0),
                  fMaxMergeFiles(0),
                  fNsubmitted(0),
                  fProductionMode(0),
                  fOutputToRunNo(0),
                  fMergeViaJDL(0),
                  fFastReadOption(0),
                  fOverwriteMode(1),
                  fNreplicas(2),
                  fNproofWorkers(0),
                  fNproofWorkersPerSlave(0),
                  fProofReset(0),
                  fRunNumbers(),
                  fExecutable(),
                  fExecutableCommand(),
                  fArguments(),
                  fExecutableArgs(),
                  fAnalysisMacro(),
                  fAnalysisSource(),
                  fValidationScript(),
                  fAdditionalRootLibs(),
                  fAdditionalLibs(),
                  fSplitMode(),
                  fAPIVersion(),
                  fROOTVersion(),
                  fAliROOTVersion(),
                  fExternalPackages(),
                  fUser(),
                  fGridWorkingDir(),
                  fGridDataDir(),
                  fDataPattern(),
                  fGridOutputDir(),
                  fOutputArchive(),
                  fOutputFiles(),
                  fInputFormat(),
                  fDatasetName(),
                  fJDLName(),
                  fTerminateFiles(),
		            fMergeExcludes(),
                  fIncludePath(),
                  fCloseSE(),
                  fFriendChainName(),
                  fJobTag(),
                  fOutputSingle(),
                  fRunPrefix(),
                  fProofCluster(),
                  fProofDataSet(),
                  fFileForTestMode(),
                  fRootVersionForProof(),
                  fAliRootMode(),
                  fInputFiles(0),
                  fPackages(0)
{
// Dummy ctor.
   SetDefaults();
}

//______________________________________________________________________________
AliAnalysisAlien::AliAnalysisAlien(const char *name)
                 :AliAnalysisGrid(name),
                  fGridJDL(NULL),
                  fMergingJDL(NULL),
                  fPrice(0),
                  fTTL(0),
                  fSplitMaxInputFileNumber(0),
                  fMaxInitFailed(0),
                  fMasterResubmitThreshold(0),
                  fNtestFiles(0),
                  fNrunsPerMaster(0),
                  fMaxMergeFiles(0),
                  fNsubmitted(0),
                  fProductionMode(0),
                  fOutputToRunNo(0),
                  fMergeViaJDL(0),
                  fFastReadOption(0),
                  fOverwriteMode(1),
                  fNreplicas(2),
                  fNproofWorkers(0),
                  fNproofWorkersPerSlave(0),
                  fProofReset(0),
                  fRunNumbers(),
                  fExecutable(),
                  fExecutableCommand(),
                  fArguments(),
                  fExecutableArgs(),
                  fAnalysisMacro(),
                  fAnalysisSource(),
                  fValidationScript(),
                  fAdditionalRootLibs(),
                  fAdditionalLibs(),
                  fSplitMode(),
                  fAPIVersion(),
                  fROOTVersion(),
                  fAliROOTVersion(),
                  fExternalPackages(),
                  fUser(),
                  fGridWorkingDir(),
                  fGridDataDir(),
                  fDataPattern(),
                  fGridOutputDir(),
                  fOutputArchive(),
                  fOutputFiles(),
                  fInputFormat(),
                  fDatasetName(),
                  fJDLName(),
                  fTerminateFiles(),
                  fMergeExcludes(),
                  fIncludePath(),
                  fCloseSE(),
                  fFriendChainName(),
                  fJobTag(),
                  fOutputSingle(),
                  fRunPrefix(),
                  fProofCluster(),
                  fProofDataSet(),
                  fFileForTestMode(),
                  fRootVersionForProof(),
                  fAliRootMode(),
                  fInputFiles(0),
                  fPackages(0)
{
// Default ctor.
   SetDefaults();
}

//______________________________________________________________________________
AliAnalysisAlien::AliAnalysisAlien(const AliAnalysisAlien& other)
                 :AliAnalysisGrid(other),
                  fGridJDL(NULL),
                  fMergingJDL(NULL),
                  fPrice(other.fPrice),
                  fTTL(other.fTTL),
                  fSplitMaxInputFileNumber(other.fSplitMaxInputFileNumber),
                  fMaxInitFailed(other.fMaxInitFailed),
                  fMasterResubmitThreshold(other.fMasterResubmitThreshold),
                  fNtestFiles(other.fNtestFiles),
                  fNrunsPerMaster(other.fNrunsPerMaster),
                  fMaxMergeFiles(other.fMaxMergeFiles),
                  fNsubmitted(other.fNsubmitted),
                  fProductionMode(other.fProductionMode),
                  fOutputToRunNo(other.fOutputToRunNo),
                  fMergeViaJDL(other.fMergeViaJDL),
                  fFastReadOption(other.fFastReadOption),
                  fOverwriteMode(other.fOverwriteMode),
                  fNreplicas(other.fNreplicas),
                  fNproofWorkers(other.fNproofWorkers),
                  fNproofWorkersPerSlave(other.fNproofWorkersPerSlave),
                  fProofReset(other.fProofReset),
                  fRunNumbers(other.fRunNumbers),
                  fExecutable(other.fExecutable),
                  fExecutableCommand(other.fExecutableCommand),
                  fArguments(other.fArguments),
                  fExecutableArgs(other.fExecutableArgs),
                  fAnalysisMacro(other.fAnalysisMacro),
                  fAnalysisSource(other.fAnalysisSource),
                  fValidationScript(other.fValidationScript),
                  fAdditionalRootLibs(other.fAdditionalRootLibs),
                  fAdditionalLibs(other.fAdditionalLibs),
                  fSplitMode(other.fSplitMode),
                  fAPIVersion(other.fAPIVersion),
                  fROOTVersion(other.fROOTVersion),
                  fAliROOTVersion(other.fAliROOTVersion),
                  fExternalPackages(other.fExternalPackages),
                  fUser(other.fUser),
                  fGridWorkingDir(other.fGridWorkingDir),
                  fGridDataDir(other.fGridDataDir),
                  fDataPattern(other.fDataPattern),
                  fGridOutputDir(other.fGridOutputDir),
                  fOutputArchive(other.fOutputArchive),
                  fOutputFiles(other.fOutputFiles),
                  fInputFormat(other.fInputFormat),
                  fDatasetName(other.fDatasetName),
                  fJDLName(other.fJDLName),
                  fTerminateFiles(other.fTerminateFiles),
                  fMergeExcludes(other.fMergeExcludes),
                  fIncludePath(other.fIncludePath),
                  fCloseSE(other.fCloseSE),
                  fFriendChainName(other.fFriendChainName),
                  fJobTag(other.fJobTag),
                  fOutputSingle(other.fOutputSingle),
                  fRunPrefix(other.fRunPrefix),
                  fProofCluster(other.fProofCluster),
                  fProofDataSet(other.fProofDataSet),
                  fFileForTestMode(other.fFileForTestMode),
                  fRootVersionForProof(other.fRootVersionForProof),
                  fAliRootMode(other.fAliRootMode),
                  fInputFiles(0),
                  fPackages(0)
{
// Copy ctor.
   fGridJDL = (TGridJDL*)gROOT->ProcessLine("new TAlienJDL()");
   fMergingJDL = (TGridJDL*)gROOT->ProcessLine("new TAlienJDL()");
   fRunRange[0] = other.fRunRange[0];
   fRunRange[1] = other.fRunRange[1];
   if (other.fInputFiles) {
      fInputFiles = new TObjArray();
      TIter next(other.fInputFiles);
      TObject *obj;
      while ((obj=next())) fInputFiles->Add(new TObjString(obj->GetName()));
      fInputFiles->SetOwner();
   }   
   if (other.fPackages) {
      fPackages = new TObjArray();
      TIter next(other.fPackages);
      TObject *obj;
      while ((obj=next())) fPackages->Add(new TObjString(obj->GetName()));
      fPackages->SetOwner();
   }   
}

//______________________________________________________________________________
AliAnalysisAlien::~AliAnalysisAlien()
{
// Destructor.
   if (fGridJDL) delete fGridJDL;
   if (fMergingJDL) delete fMergingJDL;
   if (fInputFiles) delete fInputFiles;
   if (fPackages) delete fPackages;
}   

//______________________________________________________________________________
AliAnalysisAlien &AliAnalysisAlien::operator=(const AliAnalysisAlien& other)
{
// Assignment.
   if (this != &other) {
      AliAnalysisGrid::operator=(other);
      fGridJDL = (TGridJDL*)gROOT->ProcessLine("new TAlienJDL()");
      fMergingJDL = (TGridJDL*)gROOT->ProcessLine("new TAlienJDL()");
      fPrice                   = other.fPrice;
      fTTL                     = other.fTTL;
      fSplitMaxInputFileNumber = other.fSplitMaxInputFileNumber;
      fMaxInitFailed           = other.fMaxInitFailed;
      fMasterResubmitThreshold = other.fMasterResubmitThreshold;
      fNtestFiles              = other.fNtestFiles;
      fNrunsPerMaster          = other.fNrunsPerMaster;
      fMaxMergeFiles           = other.fMaxMergeFiles;
      fNsubmitted              = other.fNsubmitted;
      fProductionMode          = other.fProductionMode;
      fOutputToRunNo           = other.fOutputToRunNo;
      fMergeViaJDL             = other.fMergeViaJDL;
      fFastReadOption          = other.fFastReadOption;
      fOverwriteMode           = other.fOverwriteMode;
      fNreplicas               = other.fNreplicas;
      fNproofWorkers           = other.fNproofWorkers;
      fNproofWorkersPerSlave   = other.fNproofWorkersPerSlave;
      fProofReset              = other.fProofReset;
      fRunNumbers              = other.fRunNumbers;
      fExecutable              = other.fExecutable;
      fExecutableCommand       = other.fExecutableCommand;
      fArguments               = other.fArguments;
      fExecutableArgs          = other.fExecutableArgs;
      fAnalysisMacro           = other.fAnalysisMacro;
      fAnalysisSource          = other.fAnalysisSource;
      fValidationScript        = other.fValidationScript;
      fAdditionalRootLibs      = other.fAdditionalRootLibs;
      fAdditionalLibs          = other.fAdditionalLibs;
      fSplitMode               = other.fSplitMode;
      fAPIVersion              = other.fAPIVersion;
      fROOTVersion             = other.fROOTVersion;
      fAliROOTVersion          = other.fAliROOTVersion;
      fExternalPackages        = other.fExternalPackages;
      fUser                    = other.fUser;
      fGridWorkingDir          = other.fGridWorkingDir;
      fGridDataDir             = other.fGridDataDir;
      fDataPattern             = other.fDataPattern;
      fGridOutputDir           = other.fGridOutputDir;
      fOutputArchive           = other.fOutputArchive;
      fOutputFiles             = other.fOutputFiles;
      fInputFormat             = other.fInputFormat;
      fDatasetName             = other.fDatasetName;
      fJDLName                 = other.fJDLName;
      fTerminateFiles          = other.fTerminateFiles;
      fMergeExcludes           = other.fMergeExcludes;
      fIncludePath             = other.fIncludePath;
      fCloseSE                 = other.fCloseSE;
      fFriendChainName         = other.fFriendChainName;
      fJobTag                  = other.fJobTag;
      fOutputSingle            = other.fOutputSingle;
      fRunPrefix               = other.fRunPrefix;
      fProofCluster            = other.fProofCluster;
      fProofDataSet            = other.fProofDataSet;
      fFileForTestMode         = other.fFileForTestMode;
      fRootVersionForProof     = other.fRootVersionForProof;
      fAliRootMode             = other.fAliRootMode;
      if (other.fInputFiles) {
         fInputFiles = new TObjArray();
         TIter next(other.fInputFiles);
         TObject *obj;
         while ((obj=next())) fInputFiles->Add(new TObjString(obj->GetName()));
         fInputFiles->SetOwner();
      }   
      if (other.fPackages) {
         fPackages = new TObjArray();
         TIter next(other.fPackages);
         TObject *obj;
         while ((obj=next())) fPackages->Add(new TObjString(obj->GetName()));
         fPackages->SetOwner();
      }   
   }
   return *this;
}

//______________________________________________________________________________
void AliAnalysisAlien::AddIncludePath(const char *path)
{
// Add include path in the remote analysis macro.
   TString p(path);
   if (p.Contains("-I")) fIncludePath += Form("%s ", path);
   else                  fIncludePath += Form("-I%s ", path);
}

//______________________________________________________________________________
void AliAnalysisAlien::AddRunNumber(Int_t run)
{
// Add a run number to the list of runs to be processed.
   if (fRunNumbers.Length()) fRunNumbers += " ";
   fRunNumbers += Form("%s%d", fRunPrefix.Data(), run);
}   

//______________________________________________________________________________
void AliAnalysisAlien::AddRunNumber(const char* run)
{
// Add a run number to the list of runs to be processed.
   if (fRunNumbers.Length()) fRunNumbers += " ";
   fRunNumbers += run;
}   

//______________________________________________________________________________
void AliAnalysisAlien::AddDataFile(const char *lfn)
{
// Adds a data file to the input to be analysed. The file should be a valid LFN
// or point to an existing file in the alien workdir.
   if (!fInputFiles) fInputFiles = new TObjArray();
   fInputFiles->Add(new TObjString(lfn));
}

//______________________________________________________________________________
void AliAnalysisAlien::AddExternalPackage(const char *package)
{
// Adds external packages w.r.t to the default ones (root,aliroot and gapi)
   if (fExternalPackages) fExternalPackages += " ";
   fExternalPackages += package;
}   
      
//______________________________________________________________________________
Bool_t AliAnalysisAlien::Connect()
{
// Try to connect to AliEn. User needs a valid token and /tmp/gclient_env_$UID sourced.
   if (gGrid && gGrid->IsConnected()) return kTRUE;
   if (fProductionMode) return kTRUE;
   if (!gGrid) {
      Info("Connect", "Trying to connect to AliEn ...");
      TGrid::Connect("alien://");
   }
   if (!gGrid || !gGrid->IsConnected()) {
      Error("Connect", "Did not managed to connect to AliEn. Make sure you have a valid token.");
      return kFALSE;
   }  
   fUser = gGrid->GetUser();
   Info("Connect", "\n#####   Connected to AliEn as user %s. Setting analysis user to <%s>", fUser.Data(), fUser.Data());
   return kTRUE;
}

//______________________________________________________________________________
void AliAnalysisAlien::CdWork()
{
// Check validity of alien workspace. Create directory if possible.
   if (!Connect()) {
      Error("CdWork", "Alien connection required");
      return;
   } 
   TString homedir = gGrid->GetHomeDirectory();
   TString workdir = homedir + fGridWorkingDir;
   if (DirectoryExists(workdir)) {
      gGrid->Cd(workdir);
      return;
   }   
   // Work directory not existing - create it
   gGrid->Cd(homedir);
   if (gGrid->Mkdir(workdir, "-p")) {
      gGrid->Cd(fGridWorkingDir);
      Info("CdWork", "\n#####   Created alien working directory %s", fGridWorkingDir.Data());
   } else {
      Warning("CdWork", "Working directory %s cannot be created.\n Using %s instead.",
              workdir.Data(), homedir.Data());
      fGridWorkingDir = "";
   }          
}

//______________________________________________________________________________
Bool_t AliAnalysisAlien::CheckFileCopy(const char *alienpath)
{
// Check if file copying is possible.
   if (fProductionMode) return kTRUE;
   if (!Connect()) {
      Error("CheckFileCopy", "Not connected to AliEn. File copying cannot be tested.");
      return kFALSE;
   }
   Info("CheckFileCopy", "Checking possibility to copy files to your AliEn home directory... \
        \n +++ NOTE: You can disable this via: plugin->SetCheckCopy(kFALSE);");
   // Check if alien_CLOSE_SE is defined
   TString closeSE = gSystem->Getenv("alien_CLOSE_SE");
   if (!closeSE.IsNull()) {
      Info("CheckFileCopy", "Your current close storage is pointing to: \
           \n      alien_CLOSE_SE = \"%s\"", closeSE.Data());
   } else {
      Warning("CheckFileCopy", "Your current close storage is empty ! Depending on your location, file copying may fail.");
   }        
   // Check if grid directory exists.
   if (!DirectoryExists(alienpath)) {
      Error("CheckFileCopy", "Alien path %s does not seem to exist", alienpath);
      return kFALSE;
   }
   TFile f("plugin_test_copy", "RECREATE");
   // User may not have write permissions to current directory 
   if (f.IsZombie()) {
      Error("CheckFileCopy", "Cannot create local test file. Do you have write access to current directory: <%s> ?",
            gSystem->WorkingDirectory());
      return kFALSE;
   }
   f.Close();
   if (FileExists(Form("alien://%s/%s",alienpath, f.GetName()))) gGrid->Rm(Form("alien://%s/%s",alienpath, f.GetName()));
   if (!TFile::Cp(f.GetName(), Form("alien://%s/%s",alienpath, f.GetName()))) {
      Error("CheckFileCopy", "Cannot copy files to Alien destination: <%s> This may be temporary, or: \
           \n# 1. Make sure you have write permissions there. If this is the case: \
           \n# 2. Check the storage availability at: http://alimonitor.cern.ch/stats?page=SE/table \
           \n#    Do:           export alien_CLOSE_SE=\"working_disk_SE\" \
           \n#    To make this permanent put in in your .bashrc (in .alienshrc is not enough) \
           \n#    Redo token:   rm /tmp/x509up_u$UID then: alien-token-init <username>", alienpath);
      gSystem->Unlink(f.GetName());
      return kFALSE;
   }   
   gSystem->Unlink(f.GetName());
   gGrid->Rm(Form("%s%s",alienpath,f.GetName()));
   Info("CheckFileCopy", "### ...SUCCESS ###");
   return kTRUE;
}   

//______________________________________________________________________________
Bool_t AliAnalysisAlien::CheckInputData()
{
// Check validity of input data. If necessary, create xml files.
   if (fProductionMode) return kTRUE;
   if (!fInputFiles && !fRunNumbers.Length() && !fRunRange[0]) {
      if (!fGridDataDir.Length()) {
         Error("CkeckInputData", "AliEn path to base data directory must be set.\n = Use: SetGridDataDir()");
         return kFALSE;
      }
      Info("CheckInputData", "Analysis will make a single xml for base data directory %s",fGridDataDir.Data());
      if (fDataPattern.Contains("tag") && TestBit(AliAnalysisGrid::kTest))
         TObject::SetBit(AliAnalysisGrid::kUseTags, kTRUE); // ADDED (fix problem in determining the tag usage in test mode) 
      return kTRUE;
   }
   // Process declared files
   Bool_t isCollection = kFALSE;
   Bool_t isXml = kFALSE;
   Bool_t useTags = kFALSE;
   Bool_t checked = kFALSE;
   if (!TestBit(AliAnalysisGrid::kTest)) CdWork();
   TString file;
   TString workdir = gGrid->GetHomeDirectory();
   workdir += fGridWorkingDir;
   if (fInputFiles) {
      TObjString *objstr;
      TIter next(fInputFiles);
      while ((objstr=(TObjString*)next())) {
         file = workdir;
         file += "/";
         file += objstr->GetString();
         // Store full lfn path
         if (FileExists(file)) objstr->SetString(file);
         else {
            file = objstr->GetName();
            if (!FileExists(objstr->GetName())) {
               Error("CheckInputData", "Data file %s not found or not in your working dir: %s",
                     objstr->GetName(), workdir.Data());
               return kFALSE;
            }         
         }
         Bool_t iscoll, isxml, usetags;
         CheckDataType(file, iscoll, isxml, usetags);
         if (!checked) {
            checked = kTRUE;
            isCollection = iscoll;
            isXml = isxml;
            useTags = usetags;
            TObject::SetBit(AliAnalysisGrid::kUseTags, useTags);
         } else {
            if ((iscoll != isCollection) || (isxml != isXml) || (usetags != useTags)) {
               Error("CheckInputData", "Some conflict was found in the types of inputs");
               return kFALSE;
            } 
         }
      }
   }
   // Process requested run numbers
   if (!fRunNumbers.Length() && !fRunRange[0]) return kTRUE;
   // Check validity of alien data directory
   if (!fGridDataDir.Length()) {
      Error("CkeckInputData", "AliEn path to base data directory must be set.\n = Use: SetGridDataDir()");
      return kFALSE;
   }
   if (!DirectoryExists(fGridDataDir)) {
      Error("CheckInputData", "Data directory %s not existing.", fGridDataDir.Data());
      return kFALSE;
   }
   if (isCollection) {
      Error("CheckInputData", "You are using raw AliEn collections as input. Cannot process run numbers.");
      return kFALSE;   
   }
   
   if (checked && !isXml) {
      Error("CheckInputData", "Cannot mix processing of full runs with non-xml files");
      return kFALSE;   
   }
   // Check validity of run number(s)
   TObjArray *arr;
   TObjString *os;
   Int_t nruns = 0;
   TString schunk, schunk2;
   TString path;
   if (!checked) {
      checked = kTRUE;
      useTags = fDataPattern.Contains("tag");
      TObject::SetBit(AliAnalysisGrid::kUseTags, useTags);
   }   
   if (useTags != fDataPattern.Contains("tag")) {
      Error("CheckInputData", "Cannot mix input files using/not using tags");
      return kFALSE;
   }
   if (fRunNumbers.Length()) {
      Info("CheckDataType", "Using supplied run numbers (run ranges are ignored)");
      arr = fRunNumbers.Tokenize(" ");
      TIter next(arr);
      while ((os=(TObjString*)next())) {
         path = Form("%s/%s ", fGridDataDir.Data(), os->GetString().Data());
         if (!DirectoryExists(path)) {
            Warning("CheckInputData", "Run number %s not found in path: <%s>", os->GetString().Data(), path.Data());
            continue;
         }
         path = Form("%s/%s.xml", workdir.Data(),os->GetString().Data());
         TString msg = "\n#####   file: ";
         msg += path;
         msg += " type: xml_collection;";
         if (useTags) msg += " using_tags: Yes";
         else          msg += " using_tags: No";
         Info("CheckDataType", "%s", msg.Data());
         if (fNrunsPerMaster<2) {
            AddDataFile(Form("%s.xml", os->GetString().Data()));
         } else {
            nruns++;
            if (((nruns-1)%fNrunsPerMaster) == 0) {
               schunk = os->GetString();
            }   
            if ((nruns%fNrunsPerMaster)!=0 && os!=arr->Last()) continue;
            schunk += Form("_%s.xml", os->GetString().Data());
            AddDataFile(schunk);
         }   
      }
      delete arr;   
   } else {
      Info("CheckDataType", "Using run range [%d, %d]", fRunRange[0], fRunRange[1]);
      for (Int_t irun=fRunRange[0]; irun<=fRunRange[1]; irun++) {
         path = Form("%s/%s%d ", fGridDataDir.Data(), fRunPrefix.Data(), irun);
         if (!DirectoryExists(path)) {
//            Warning("CheckInputData", "Run number %d not found in path: <%s>", irun, path.Data());
            continue;
         }
         path = Form("%s/%s%d.xml", workdir.Data(),fRunPrefix.Data(),irun);
         TString msg = "\n#####   file: ";
         msg += path;
         msg += " type: xml_collection;";
         if (useTags) msg += " using_tags: Yes";
         else          msg += " using_tags: No";
         Info("CheckDataType", "%s", msg.Data());
         if (fNrunsPerMaster<2) {
            AddDataFile(Form("%s%d.xml",fRunPrefix.Data(),irun));
         } else {
            nruns++;
            if (((nruns-1)%fNrunsPerMaster) == 0) {
               schunk = Form("%s%d", fRunPrefix.Data(),irun);
            }
            schunk2 = Form("_%s%d.xml", fRunPrefix.Data(), irun);
            if ((nruns%fNrunsPerMaster)!=0 && irun != fRunRange[1]) continue;
            schunk += schunk2;
            AddDataFile(schunk);
         }   
      }
      if (!fInputFiles) {
         schunk += schunk2;
         AddDataFile(schunk);
      }   
   }
   return kTRUE;      
}   

//______________________________________________________________________________
Bool_t AliAnalysisAlien::CreateDataset(const char *pattern)
{
// Create dataset for the grid data directory + run number.
   if (fProductionMode || TestBit(AliAnalysisGrid::kOffline)) return kTRUE;
   if (!Connect()) {
      Error("CreateDataset", "Cannot create dataset with no grid connection");
      return kFALSE;
   }   

   // Cd workspace
   if (!TestBit(AliAnalysisGrid::kTest)) CdWork();
   TString workdir = gGrid->GetHomeDirectory();
   workdir += fGridWorkingDir;

   // Compose the 'find' command arguments
   TString command;
   TString options = "-x collection ";
   if (TestBit(AliAnalysisGrid::kTest)) options += Form("-l %d ", fNtestFiles);
   TString conditions = "";
   
   TString file;
   TString path;
   Int_t nruns = 0;
   TString schunk, schunk2;
   TGridCollection *cbase=0, *cadd=0;
   if (!fRunNumbers.Length() && !fRunRange[0]) {
      if (fInputFiles && fInputFiles->GetEntries()) return kTRUE;
      // Make a single data collection from data directory.
      path = fGridDataDir;
      if (!DirectoryExists(path)) {
         Error("CreateDataset", "Path to data directory %s not valid",fGridDataDir.Data());
         return kFALSE;
      }   
//      CdWork();
      if (TestBit(AliAnalysisGrid::kTest)) file = "wn.xml";
      else file = Form("%s.xml", gSystem->BaseName(path));
      if (gSystem->AccessPathName(file) || TestBit(AliAnalysisGrid::kTest) || fOverwriteMode) {
         command = "find ";
         command += options;
         command += path;
         command += " ";
         command += pattern;
         command += conditions;
         printf("command: %s\n", command.Data());
         TGridResult *res = gGrid->Command(command);
         if (res) delete res;
         // Write standard output to file
         gROOT->ProcessLine(Form("gGrid->Stdout(); > %s", file.Data()));
         Bool_t hasGrep = (gSystem->Exec("grep --version 2>/dev/null > /dev/null")==0)?kTRUE:kFALSE;
         Bool_t nullFile = kFALSE;
         if (!hasGrep) {
            Warning("CreateDataset", "'grep' command not available on this system - cannot validate the result of the grid 'find' command");
         } else {
            nullFile = (gSystem->Exec(Form("grep /event %s 2>/dev/null > /dev/null",file.Data()))==0)?kFALSE:kTRUE;
            if (nullFile) {
               Error("CreateDataset","Dataset %s produced by the previous find command is empty !", file.Data());
               return kFALSE;
            }   
         }         
      }
      Bool_t fileExists = FileExists(file);
      if (!TestBit(AliAnalysisGrid::kTest) && (!fileExists || fOverwriteMode)) {
         // Copy xml file to alien space
         if (fileExists) gGrid->Rm(file);
         TFile::Cp(Form("file:%s",file.Data()), Form("alien://%s/%s",workdir.Data(), file.Data()));
         if (!FileExists(file)) {
            Error("CreateDataset", "Command %s did NOT succeed", command.Data());
            return kFALSE;
         }
         // Update list of files to be processed.
      }
      AddDataFile(Form("%s/%s", workdir.Data(), file.Data()));
      return kTRUE;
   }   
   // Several runs
   Bool_t nullResult = kTRUE;
   if (fRunNumbers.Length()) {
      TObjArray *arr = fRunNumbers.Tokenize(" ");
      TObjString *os;
      TIter next(arr);
      while ((os=(TObjString*)next())) {
         path = Form("%s/%s ", fGridDataDir.Data(), os->GetString().Data());
         if (!DirectoryExists(path)) continue;
//         CdWork();
         if (TestBit(AliAnalysisGrid::kTest)) file = "wn.xml";
         else file = Form("%s.xml", os->GetString().Data());
         // If local collection file does not exist, create it via 'find' command.
         if (gSystem->AccessPathName(file) || TestBit(AliAnalysisGrid::kTest) || fOverwriteMode) {
            command = "find ";
            command += options;
            command += path;
            command += pattern;
            command += conditions;
            TGridResult *res = gGrid->Command(command);
            if (res) delete res;
            // Write standard output to file
            gROOT->ProcessLine(Form("gGrid->Stdout(); > %s", file.Data()));
            Bool_t hasGrep = (gSystem->Exec("grep --version 2>/dev/null > /dev/null")==0)?kTRUE:kFALSE;
            Bool_t nullFile = kFALSE;
            if (!hasGrep) {
               Warning("CreateDataset", "'grep' command not available on this system - cannot validate the result of the grid 'find' command");
            } else {
               nullFile = (gSystem->Exec(Form("grep /event %s 2>/dev/null > /dev/null",file.Data()))==0)?kFALSE:kTRUE;
               if (nullFile) {
                  Warning("CreateDataset","Dataset %s produced by: <%s> is empty !", file.Data(), command.Data());
                  fRunNumbers.ReplaceAll(os->GetString().Data(), "");
                  continue;
               }   
            }
            nullResult = kFALSE;         
         }
         if (TestBit(AliAnalysisGrid::kTest)) break;
         // Check if there is one run per master job.
         if (fNrunsPerMaster<2) {
            if (FileExists(file)) {
               if (fOverwriteMode) gGrid->Rm(file);
               else {
                  Info("CreateDataset", "\n#####   Dataset %s exist. Skipping creation...", file.Data());
                  continue;
               }   
            }        
            // Copy xml file to alien space
            TFile::Cp(Form("file:%s",file.Data()), Form("alien://%s/%s",workdir.Data(), file.Data()));
            if (!FileExists(file)) {
               Error("CreateDataset", "Command %s did NOT succeed", command.Data());
               delete arr;
               return kFALSE;
            }
         } else {
            nruns++;
            if (((nruns-1)%fNrunsPerMaster) == 0) {
               schunk = os->GetString();
               cbase = (TGridCollection*)gROOT->ProcessLine(Form("new TAlienCollection(\"%s\", 1000000);",file.Data()));
            } else {
               cadd = (TGridCollection*)gROOT->ProcessLine(Form("new TAlienCollection(\"%s\", 1000000);",file.Data()));
               printf("   Merging collection <%s> into masterjob input...\n", file.Data());
               cbase->Add(cadd);
               delete cadd;
            }
            if ((nruns%fNrunsPerMaster)!=0 && os!=arr->Last()) {
               continue;
            }   
            schunk += Form("_%s.xml", os->GetString().Data());
            if (FileExists(schunk)) {               
               if (fOverwriteMode) gGrid->Rm(file);
               else {
                  Info("CreateDataset", "\n#####   Dataset %s exist. Skipping creation...", schunk.Data());
                  continue;
               }   
            }        
            printf("Exporting merged collection <%s> and copying to AliEn\n", schunk.Data());
            cbase->ExportXML(Form("file://%s", schunk.Data()),kFALSE,kFALSE, schunk, "Merged runs");
            TFile::Cp(Form("file:%s",schunk.Data()), Form("alien://%s/%s",workdir.Data(), schunk.Data()));
            if (!FileExists(schunk)) {
               Error("CreateDataset", "Copy command did NOT succeed for %s", schunk.Data());
               delete arr;
               return kFALSE;
            }
         }
      }   
      delete arr;
      if (nullResult) {
         Error("CreateDataset", "No valid dataset corresponding to the query!");
         return kFALSE;
      }
   } else {
      // Process a full run range.
      for (Int_t irun=fRunRange[0]; irun<=fRunRange[1]; irun++) {
         path = Form("%s/%s%d ", fGridDataDir.Data(), fRunPrefix.Data(), irun);
         if (!DirectoryExists(path)) continue;
//         CdWork();
         if (TestBit(AliAnalysisGrid::kTest)) file = "wn.xml";
         else file = Form("%s%d.xml", fRunPrefix.Data(), irun);
         if (FileExists(file) && fNrunsPerMaster<2 && !TestBit(AliAnalysisGrid::kTest)) {         
            if (fOverwriteMode) gGrid->Rm(file);
            else {
               Info("CreateDataset", "\n#####   Dataset %s exist. Skipping creation...", file.Data());
               continue;
            }   
         }
         // If local collection file does not exist, create it via 'find' command.
         if (gSystem->AccessPathName(file) || TestBit(AliAnalysisGrid::kTest) || fOverwriteMode) {
            command = "find ";
            command += options;
            command += path;
            command += pattern;
            command += conditions;
            TGridResult *res = gGrid->Command(command);
            if (res) delete res;
            // Write standard output to file
            gROOT->ProcessLine(Form("gGrid->Stdout(); > %s", file.Data()));
            Bool_t hasGrep = (gSystem->Exec("grep --version 2>/dev/null > /dev/null")==0)?kTRUE:kFALSE;
            Bool_t nullFile = kFALSE;
            if (!hasGrep) {
               Warning("CreateDataset", "'grep' command not available on this system - cannot validate the result of the grid 'find' command");
            } else {
               nullFile = (gSystem->Exec(Form("grep /event %s 2>/dev/null > /dev/null",file.Data()))==0)?kFALSE:kTRUE;
               if (nullFile) {
                  Warning("CreateDataset","Dataset %s produced by: <%s> is empty !", file.Data(), command.Data());
                  continue;
               }   
            }
            nullResult = kFALSE;         
         }   
         if (TestBit(AliAnalysisGrid::kTest)) break;
         // Check if there is one run per master job.
         if (fNrunsPerMaster<2) {
            if (FileExists(file)) {
               if (fOverwriteMode) gGrid->Rm(file);
               else {
                  Info("CreateDataset", "\n#####   Dataset %s exist. Skipping creation...", file.Data());
                  continue;
               }   
            }        
            // Copy xml file to alien space
            TFile::Cp(Form("file:%s",file.Data()), Form("alien://%s/%s",workdir.Data(), file.Data()));
            if (!FileExists(file)) {
               Error("CreateDataset", "Command %s did NOT succeed", command.Data());
               return kFALSE;
            }
         } else {
            nruns++;
            // Check if the collection for the chunk exist locally.
            Int_t nchunk = (nruns-1)/fNrunsPerMaster;
            if (FileExists(fInputFiles->At(nchunk)->GetName())) {
               if (fOverwriteMode) gGrid->Rm(fInputFiles->At(nchunk)->GetName());
               else continue;
            }   
            printf("   Merging collection <%s> into %d runs chunk...\n",file.Data(),fNrunsPerMaster);
            if (((nruns-1)%fNrunsPerMaster) == 0) {
               schunk = Form("%s%d", fRunPrefix.Data(), irun);
               cbase = (TGridCollection*)gROOT->ProcessLine(Form("new TAlienCollection(\"%s\", 1000000);",file.Data()));
            } else {
               cadd = (TGridCollection*)gROOT->ProcessLine(Form("new TAlienCollection(\"%s\", 1000000);",file.Data()));
               cbase->Add(cadd);
               delete cadd;
            }
            schunk2 = Form("%s_%s%d.xml", schunk.Data(), fRunPrefix.Data(), irun);
            if ((nruns%fNrunsPerMaster)!=0 && irun!=fRunRange[1] && schunk2 != fInputFiles->Last()->GetName()) {
               continue;
            }   
            schunk = schunk2;
            if (FileExists(schunk)) {
               if (fOverwriteMode) gGrid->Rm(schunk);
               else {
                  Info("CreateDataset", "\n#####   Dataset %s exist. Skipping creation...", schunk.Data());
                  continue;
               }   
            }        
            printf("Exporting merged collection <%s> and copying to AliEn.\n", schunk.Data());
            cbase->ExportXML(Form("file://%s", schunk.Data()),kFALSE,kFALSE, schunk, "Merged runs");
            if (FileExists(schunk)) {
               if (fOverwriteMode) gGrid->Rm(schunk);
               else {
                  Info("CreateDataset", "\n#####   Dataset %s exist. Skipping copy...", schunk.Data());
                  continue;
               }   
            }   
            TFile::Cp(Form("file:%s",schunk.Data()), Form("alien://%s/%s",workdir.Data(), schunk.Data()));
            if (!FileExists(schunk)) {
               Error("CreateDataset", "Copy command did NOT succeed for %s", schunk.Data());
               return kFALSE;
            }
         }   
      }
      if (nullResult) {
         Error("CreateDataset", "No valid dataset corresponding to the query!");
         return kFALSE;
      }      
   }      
   return kTRUE;
}

//______________________________________________________________________________
Bool_t AliAnalysisAlien::CreateJDL()
{
// Generate a JDL file according to current settings. The name of the file is 
// specified by fJDLName.
   Bool_t error = kFALSE;
   TObjArray *arr = 0;
   Bool_t copy = kTRUE;
   if (fProductionMode || TestBit(AliAnalysisGrid::kOffline) || TestBit(AliAnalysisGrid::kTest)) copy = kFALSE;
   Bool_t generate = kTRUE;
   if (TestBit(AliAnalysisGrid::kTest) || TestBit(AliAnalysisGrid::kSubmit)) generate = kFALSE;
   if (!Connect()) {
      Error("CreateJDL", "Alien connection required");
      return kFALSE;
   }   
   // Check validity of alien workspace
   TString workdir;
   if (!fProductionMode && !fGridWorkingDir.BeginsWith("/alice")) workdir = gGrid->GetHomeDirectory();
   if (!fProductionMode &&  !TestBit(AliAnalysisGrid::kTest)) CdWork();
   workdir += fGridWorkingDir;
   if (generate) {
      TObjString *os;
      if (!fInputFiles) {
         Error("CreateJDL()", "Define some input files for your analysis.");
         error = kTRUE;
      }
      // Compose list of input files   
      // Check if output files were defined
      if (!fOutputFiles.Length()) {
         Error("CreateJDL", "You must define at least one output file");
         error = kTRUE;
      }   
      // Check if an output directory was defined and valid
      if (!fGridOutputDir.Length()) {
         Error("CreateJDL", "You must define AliEn output directory");
         error = kTRUE;
      } else {
         if (!fProductionMode) {
            if (!fGridOutputDir.Contains("/")) fGridOutputDir = Form("%s/%s", workdir.Data(), fGridOutputDir.Data());
            if (!DirectoryExists(fGridOutputDir)) {
               if (gGrid->Mkdir(fGridOutputDir,"-p")) {
                  Info("CreateJDL", "\n#####   Created alien output directory %s", fGridOutputDir.Data());
               } else {
                  Error("CreateJDL", "Could not create alien output directory %s", fGridOutputDir.Data());
                  // error = kTRUE;
               }
            }   
            gGrid->Cd(workdir);
         }   
      }   
      // Exit if any error up to now
      if (error) return kFALSE;   
      // Set JDL fields
      if (!fUser.IsNull()) {
         fGridJDL->SetValue("User", Form("\"%s\"", fUser.Data()));
         fMergingJDL->SetValue("User", Form("\"%s\"", fUser.Data()));
      }   
      fGridJDL->SetExecutable(fExecutable, "This is the startup script");
      TString mergeExec = fExecutable;
      mergeExec.ReplaceAll(".sh", "_merge.sh");
      fMergingJDL->SetExecutable(mergeExec, "This is the startup script");
      mergeExec.ReplaceAll(".sh", ".C");
      fMergingJDL->AddToInputSandbox(Form("LF:%s/%s", workdir.Data(),mergeExec.Data()), "List of input files to be uploaded to workers");
      if (!fArguments.IsNull())
         fGridJDL->SetArguments(fArguments, "Arguments for the executable command");
      if (IsOneStageMerging()) fMergingJDL->SetArguments(fGridOutputDir);
      else fMergingJDL->SetArguments("$1 $2 $3"); 
      fGridJDL->SetValue("TTL", Form("\"%d\"",fTTL));
      fGridJDL->SetDescription("TTL", Form("Time after which the job is killed (%d min.)", fTTL/60));
      fMergingJDL->SetValue("TTL", Form("\"%d\"",fTTL));
      fMergingJDL->SetDescription("TTL", Form("Time after which the job is killed (%d min.)", fTTL/60));
        
      if (fMaxInitFailed > 0) {
         fGridJDL->SetValue("MaxInitFailed", Form("\"%d\"",fMaxInitFailed));
         fGridJDL->SetDescription("MaxInitFailed", "Maximum number of first failing jobs to abort the master job");
      }   
      if (fSplitMaxInputFileNumber > 0) {
         fGridJDL->SetValue("SplitMaxInputFileNumber", Form("\"%d\"", fSplitMaxInputFileNumber));
         fGridJDL->SetDescription("SplitMaxInputFileNumber", "Maximum number of input files to be processed per subjob");
      }   
      if (fSplitMode.Length()) {
         fGridJDL->SetValue("Split", Form("\"%s\"", fSplitMode.Data()));
         fGridJDL->SetDescription("Split", "We split per SE or file");
      }   
      if (!fAliROOTVersion.IsNull()) {
         fGridJDL->AddToPackages("AliRoot", fAliROOTVersion,"VO_ALICE", "List of requested packages");
         fMergingJDL->AddToPackages("AliRoot", fAliROOTVersion, "VO_ALICE", "List of requested packages");
      }   
      if (!fROOTVersion.IsNull()) {
         fGridJDL->AddToPackages("ROOT", fROOTVersion);
         fMergingJDL->AddToPackages("ROOT", fROOTVersion);
      }   
      if (!fAPIVersion.IsNull()) {
         fGridJDL->AddToPackages("APISCONFIG", fAPIVersion);
         fMergingJDL->AddToPackages("APISCONFIG", fAPIVersion);
      }   
      if (!fExternalPackages.IsNull()) {
         arr = fExternalPackages.Tokenize(" ");
         TIter next(arr);
         while ((os=(TObjString*)next())) {
            TString pkgname = os->GetString();
            Int_t index = pkgname.Index("::");
            TString pkgversion = pkgname(index+2, pkgname.Length());
            pkgname.Remove(index);
            fGridJDL->AddToPackages(pkgname, pkgversion);
            fMergingJDL->AddToPackages(pkgname, pkgversion);
         }   
         delete arr;   
      }   
      fGridJDL->SetInputDataListFormat(fInputFormat, "Format of input data");
      fGridJDL->SetInputDataList("wn.xml", "Collection name to be processed on each worker node");
      fGridJDL->AddToInputSandbox(Form("LF:%s/%s", workdir.Data(), fAnalysisMacro.Data()), "List of input files to be uploaded to workers");
      TString analysisFile = fExecutable;
      analysisFile.ReplaceAll(".sh", ".root");
      fGridJDL->AddToInputSandbox(Form("LF:%s/%s", workdir.Data(),analysisFile.Data()));
      fMergingJDL->AddToInputSandbox(Form("LF:%s/%s", workdir.Data(),analysisFile.Data()));
      if (IsUsingTags() && !gSystem->AccessPathName("ConfigureCuts.C"))
         fGridJDL->AddToInputSandbox(Form("LF:%s/ConfigureCuts.C", workdir.Data()));
      if (fAdditionalLibs.Length()) {
         arr = fAdditionalLibs.Tokenize(" ");
         TIter next(arr);
         while ((os=(TObjString*)next())) {
            if (os->GetString().Contains(".so")) continue;
            fGridJDL->AddToInputSandbox(Form("LF:%s/%s", workdir.Data(), os->GetString().Data()));
            fMergingJDL->AddToInputSandbox(Form("LF:%s/%s", workdir.Data(), os->GetString().Data()));
         }   
         delete arr;   
      }
      if (fPackages) {
         TIter next(fPackages);
         TObject *obj;
         while ((obj=next())) {
            fGridJDL->AddToInputSandbox(Form("LF:%s/%s", workdir.Data(), obj->GetName()));
            fMergingJDL->AddToInputSandbox(Form("LF:%s/%s", workdir.Data(), obj->GetName()));
         }
      }
      if (fOutputArchive.Length()) {
         arr = fOutputArchive.Tokenize(" ");
         TIter next(arr);
         Bool_t first = kTRUE;
         const char *comment = "Files to be archived";
         const char *comment1 = comment;
         while ((os=(TObjString*)next())) {
            if (!first) comment = NULL;
            if (!os->GetString().Contains("@") && fCloseSE.Length())
               fGridJDL->AddToOutputArchive(Form("%s@%s",os->GetString().Data(), fCloseSE.Data()), comment); 
            else
               fGridJDL->AddToOutputArchive(os->GetString(), comment);
            first = kFALSE;   
         }      
         delete arr;
         // Output archive for the merging jdl
         TString outputArchive;
         if (TestBit(AliAnalysisGrid::kDefaultOutputs)) {
            outputArchive = "log_archive.zip:std*@disk=1 ";
            // Add normal output files, extra files + terminate files
            TString files = GetListOfFiles("outextter");
            // Do not register merge excludes
            if (!fMergeExcludes.IsNull()) {
               arr = fMergeExcludes.Tokenize(" ");
               TIter next1(arr);
               while ((os=(TObjString*)next1())) {
                  files.ReplaceAll(Form("%s,",os->GetString().Data()),"");
                  files.ReplaceAll(os->GetString(),"");
               }   
               delete arr;
            }
            files.ReplaceAll(".root", "*.root");
            outputArchive += Form("root_archive.zip:%s@disk=%d",files.Data(),fNreplicas);
         } else {
            outputArchive = fOutputArchive;
         }   
         arr = outputArchive.Tokenize(" ");
         TIter next2(arr);
         comment = comment1;
         first = kTRUE;
         while ((os=(TObjString*)next2())) {
            if (!first) comment = NULL;
            TString currentfile = os->GetString();
            if (!IsOneStageMerging()) currentfile.ReplaceAll(".zip", "-Stage$2_$3.zip");
            if (!currentfile.Contains("@") && fCloseSE.Length())
               fMergingJDL->AddToOutputArchive(Form("%s@%s",currentfile.Data(), fCloseSE.Data()), comment);
            else
               fMergingJDL->AddToOutputArchive(currentfile, comment);
            first = kFALSE;   
         }      
         delete arr;         
      }      
      arr = fOutputFiles.Tokenize(",");
      TIter next(arr);
      Bool_t first = kTRUE;
      const char *comment = "Files to be saved";
      while ((os=(TObjString*)next())) {
         // Ignore ouputs in jdl that are also in outputarchive
         TString sout = os->GetString();
         sout.ReplaceAll("*", "");
         sout.ReplaceAll(".root", "");
         if (sout.Index("@")>0) sout.Remove(sout.Index("@"));
         if (fOutputArchive.Contains(sout)) continue;
         if (!first) comment = NULL;
         if (!os->GetString().Contains("@") && fCloseSE.Length())
            fGridJDL->AddToOutputSandbox(Form("%s@%s",os->GetString().Data(), fCloseSE.Data()), comment); 
         else
            fGridJDL->AddToOutputSandbox(os->GetString(), comment);
         first = kFALSE;   
         if (fMergeExcludes.Contains(sout)) continue;   
         if (!os->GetString().Contains("@") && fCloseSE.Length())
            fMergingJDL->AddToOutputSandbox(Form("%s@%s",os->GetString().Data(), fCloseSE.Data()), comment); 
         else
            fMergingJDL->AddToOutputSandbox(os->GetString(), comment);
      }   
      delete arr;
      fGridJDL->SetPrice((UInt_t)fPrice, "AliEn price for this job");
      fMergingJDL->SetPrice((UInt_t)fPrice, "AliEn price for this job");
      TString validationScript = fValidationScript;
      fGridJDL->SetValidationCommand(Form("%s/%s", workdir.Data(),validationScript.Data()), "Validation script to be run for each subjob");
      validationScript.ReplaceAll(".sh", "_merge.sh");
      fMergingJDL->SetValidationCommand(Form("%s/%s", workdir.Data(),validationScript.Data()), "Validation script to be run for each subjob");
      if (fMasterResubmitThreshold) {
         fGridJDL->SetValue("MasterResubmitThreshold", Form("\"%d%%\"", fMasterResubmitThreshold));
         fGridJDL->SetDescription("MasterResubmitThreshold", "Resubmit failed jobs until DONE rate reaches this percentage");
      }   
      // Write a jdl with 2 input parameters: collection name and output dir name.
      WriteJDL(copy);
   }
   // Copy jdl to grid workspace   
   if (copy) {
      // Check if an output directory was defined and valid
      if (!fGridOutputDir.Length()) {
         Error("CreateJDL", "You must define AliEn output directory");
         return kFALSE;
      } else {
         if (!fGridOutputDir.Contains("/")) fGridOutputDir = Form("%s/%s", workdir.Data(), fGridOutputDir.Data());
         if (!fProductionMode && !DirectoryExists(fGridOutputDir)) {
            if (gGrid->Mkdir(fGridOutputDir,"-p")) {
               Info("CreateJDL", "\n#####   Created alien output directory %s", fGridOutputDir.Data());
            } else {
               Error("CreateJDL", "Could not create alien output directory %s", fGridOutputDir.Data());
               return kFALSE;
            }
         }
         gGrid->Cd(workdir);
      }   
      if (TestBit(AliAnalysisGrid::kSubmit)) {
         TString mergeJDLName = fExecutable;
         mergeJDLName.ReplaceAll(".sh", "_merge.jdl");
         TString locjdl = Form("%s/%s", fGridOutputDir.Data(),fJDLName.Data());
         TString locjdl1 = Form("%s/%s", fGridOutputDir.Data(),mergeJDLName.Data());
         if (fProductionMode) {
            locjdl = Form("%s/%s", workdir.Data(),fJDLName.Data());
            locjdl1 = Form("%s/%s", workdir.Data(),mergeJDLName.Data());
         }   
         if (FileExists(locjdl)) gGrid->Rm(locjdl);
         if (FileExists(locjdl1)) gGrid->Rm(locjdl1);
         Info("CreateJDL", "\n#####   Copying JDL file <%s> to your AliEn output directory", fJDLName.Data());
         TFile::Cp(Form("file:%s",fJDLName.Data()), Form("alien://%s", locjdl.Data()));
         if (fMergeViaJDL) {
            Info("CreateJDL", "\n#####   Copying merging JDL file <%s> to your AliEn output directory", mergeJDLName.Data());
            TFile::Cp(Form("file:%s",mergeJDLName.Data()), Form("alien://%s", locjdl1.Data()));
         }   
      }
      if (fAdditionalLibs.Length()) {
         arr = fAdditionalLibs.Tokenize(" ");
         TObjString *os;
         TIter next(arr);
         while ((os=(TObjString*)next())) {
            if (os->GetString().Contains(".so")) continue;
            Info("CreateJDL", "\n#####   Copying dependency: <%s> to your alien workspace", os->GetString().Data());
            if (FileExists(os->GetString())) gGrid->Rm(os->GetString());
            TFile::Cp(Form("file:%s",os->GetString().Data()), Form("alien://%s/%s", workdir.Data(), os->GetString().Data()));
         }   
         delete arr;   
      }
      if (fPackages) {
         TIter next(fPackages);
         TObject *obj;
         while ((obj=next())) {
            if (FileExists(obj->GetName())) gGrid->Rm(obj->GetName());
            Info("CreateJDL", "\n#####   Copying dependency: <%s> to your alien workspace", obj->GetName());
            TFile::Cp(Form("file:%s",obj->GetName()), Form("alien://%s/%s", workdir.Data(), obj->GetName()));
         }   
      }      
   } 
   return kTRUE;
}

//______________________________________________________________________________
Bool_t AliAnalysisAlien::WriteJDL(Bool_t copy)
{
// Writes one or more JDL's corresponding to findex. If findex is negative,
// all run numbers are considered in one go (jdl). For non-negative indices
// they correspond to the indices in the array fInputFiles.
   if (!fInputFiles) return kFALSE;
   TObject *os;
   TString workdir;
   if (!fProductionMode && !fGridWorkingDir.BeginsWith("/alice")) workdir = gGrid->GetHomeDirectory();
   workdir += fGridWorkingDir;
   
   if (fProductionMode) {
      TIter next(fInputFiles);
      while ((os=next()))
         fGridJDL->AddToInputDataCollection(Form("LF:%s,nodownload", os->GetName()), "Input xml collections");
      fGridJDL->SetOutputDirectory(Form("%s/#alien_counter_04i#", fGridOutputDir.Data()));
      fMergingJDL->SetOutputDirectory(fGridOutputDir);  
   } else {            
      if (!fRunNumbers.Length() && !fRunRange[0]) {
         // One jdl with no parameters in case input data is specified by name.
         TIter next(fInputFiles);
         while ((os=next()))
            fGridJDL->AddToInputDataCollection(Form("LF:%s,nodownload", os->GetName()), "Input xml collections");
         if (!fOutputSingle.IsNull())
            fGridJDL->SetOutputDirectory(Form("#alienfulldir#/../%s",fOutputSingle.Data()), "Output directory");
         else {
            fGridJDL->SetOutputDirectory(Form("%s/#alien_counter_03i#", fGridOutputDir.Data()), "Output directory");
            fMergingJDL->SetOutputDirectory(fGridOutputDir);         
         }   
      } else {
         // One jdl to be submitted with 2 input parameters: data collection name and output dir prefix
         fGridJDL->AddToInputDataCollection(Form("LF:%s/$1,nodownload", workdir.Data()), "Input xml collections");
         if (!fOutputSingle.IsNull()) {
            if (!fOutputToRunNo) fGridJDL->SetOutputDirectory(Form("#alienfulldir#/%s",fOutputSingle.Data()), "Output directory");
            else fGridJDL->SetOutputDirectory(Form("%s/$2",fGridOutputDir.Data()), "Output directory");
         } else {   
            fGridJDL->SetOutputDirectory(Form("%s/$2/#alien_counter_03i#", fGridOutputDir.Data()), "Output directory");
            fMergingJDL->SetOutputDirectory("$1", "Output directory");
         }   
      }
   }
      
   // Generate the JDL as a string
   TString sjdl = fGridJDL->Generate();
   TString sjdl1 = fMergingJDL->Generate();
   Int_t index;
   sjdl.ReplaceAll("\"LF:", "\n   \"LF:");
   sjdl.ReplaceAll("(member", "\n   (member");
   sjdl.ReplaceAll("\",\"VO_", "\",\n   \"VO_");
   sjdl.ReplaceAll("{", "{\n   ");
   sjdl.ReplaceAll("};", "\n};");
   sjdl.ReplaceAll("{\n   \n", "{\n");
   sjdl.ReplaceAll("\n\n", "\n");
   sjdl.ReplaceAll("OutputDirectory", "OutputDir");
   sjdl1.ReplaceAll("\"LF:", "\n   \"LF:");
   sjdl1.ReplaceAll("(member", "\n   (member");
   sjdl1.ReplaceAll("\",\"VO_", "\",\n   \"VO_");
   sjdl1.ReplaceAll("{", "{\n   ");
   sjdl1.ReplaceAll("};", "\n};");
   sjdl1.ReplaceAll("{\n   \n", "{\n");
   sjdl1.ReplaceAll("\n\n", "\n");
   sjdl1.ReplaceAll("OutputDirectory", "OutputDir");
   sjdl += "JDLVariables = \n{\n   \"Packages\",\n   \"OutputDir\"\n};\n";
   sjdl.Prepend(Form("Jobtag = {\n   \"comment:%s\"\n};\n", fJobTag.Data()));
   index = sjdl.Index("JDLVariables");
   if (index >= 0) sjdl.Insert(index, "\n# JDL variables\n");
   sjdl += "Workdirectorysize = {\"5000MB\"};";
   sjdl1 += "JDLVariables = \n{\n   \"Packages\",\n   \"OutputDir\"\n};\n";
   index = fJobTag.Index(":");
   if (index < 0) index = fJobTag.Length();
   TString jobTag = fJobTag;
   jobTag.Insert(index, "_Merging");
   sjdl1.Prepend(Form("Jobtag = {\n   \"comment:%s_Merging\"\n};\n", jobTag.Data()));
   sjdl1.Prepend("# Generated merging jdl\n# $1 = full alien path to output directory to be merged\n# $2 = merging stage\n# $3 = merged chunk\n");
   index = sjdl1.Index("JDLVariables");
   if (index >= 0) sjdl1.Insert(index, "\n# JDL variables\n");
   sjdl1 += "Workdirectorysize = {\"5000MB\"};";
   // Write jdl to file
   ofstream out;
   out.open(fJDLName.Data(), ios::out);
   if (out.bad()) {
      Error("WriteJDL", "Bad file name: %s", fJDLName.Data());
      return kFALSE;
   }
   out << sjdl << endl;
   TString mergeJDLName = fExecutable;
   mergeJDLName.ReplaceAll(".sh", "_merge.jdl");
   if (fMergeViaJDL) {
      ofstream out1;
      out1.open(mergeJDLName.Data(), ios::out);
      if (out.bad()) {
         Error("WriteJDL", "Bad file name: %s", mergeJDLName.Data());
         return kFALSE;
      }
      out1 << sjdl1 << endl;
   }   

   // Copy jdl to grid workspace   
   if (!copy) {
      Info("WriteJDL", "\n#####   You may want to review jdl:%s and analysis macro:%s before running in <submit> mode", fJDLName.Data(), fAnalysisMacro.Data());
   } else {
      TString locjdl = Form("%s/%s", fGridOutputDir.Data(),fJDLName.Data());
      TString locjdl1 = Form("%s/%s", fGridOutputDir.Data(),mergeJDLName.Data());
      if (fProductionMode) {
         locjdl = Form("%s/%s", workdir.Data(),fJDLName.Data());
         locjdl1 = Form("%s/%s", workdir.Data(),mergeJDLName.Data());
      }   
      if (FileExists(locjdl)) gGrid->Rm(locjdl);
      if (FileExists(locjdl1)) gGrid->Rm(locjdl1);
      Info("WriteJDL", "\n#####   Copying JDL file <%s> to your AliEn output directory", fJDLName.Data());
      TFile::Cp(Form("file:%s",fJDLName.Data()), Form("alien://%s", locjdl.Data()));
      if (fMergeViaJDL) {
         Info("WriteJDL", "\n#####   Copying merging JDL file <%s> to your AliEn output directory", mergeJDLName.Data());
         TFile::Cp(Form("file:%s",mergeJDLName.Data()), Form("alien://%s", locjdl1.Data()));
      }   
   } 
   return kTRUE;
}

//______________________________________________________________________________
Bool_t AliAnalysisAlien::FileExists(const char *lfn)
{
// Returns true if file exists.
   if (!gGrid) return kFALSE;
   TGridResult *res = gGrid->Ls(lfn);
   if (!res) return kFALSE;
   TMap *map = dynamic_cast<TMap*>(res->At(0));
   if (!map) {
      delete res;
      return kFALSE;
   }   
   TObjString *objs = dynamic_cast<TObjString*>(map->GetValue("name"));
   if (!objs || !objs->GetString().Length()) {
      delete res;
      return kFALSE;
   }
   delete res;   
   return kTRUE;
}

//______________________________________________________________________________
Bool_t AliAnalysisAlien::DirectoryExists(const char *dirname)
{
// Returns true if directory exists. Can be also a path.
   if (!gGrid) return kFALSE;
   // Check if dirname is a path
   TString dirstripped = dirname;
   dirstripped = dirstripped.Strip();
   dirstripped = dirstripped.Strip(TString::kTrailing, '/');
   TString dir = gSystem->BaseName(dirstripped);
   dir += "/";
   TString path = gSystem->DirName(dirstripped);
   TGridResult *res = gGrid->Ls(path, "-F");
   if (!res) return kFALSE;
   TIter next(res);
   TMap *map;
   TObject *obj;
   while ((map=dynamic_cast<TMap*>(next()))) {
      obj = map->GetValue("name");
      if (!obj) break;
      if (dir == obj->GetName()) {
         delete res;
         return kTRUE;
      }
   }
   delete res;
   return kFALSE;
}      

//______________________________________________________________________________
void AliAnalysisAlien::CheckDataType(const char *lfn, Bool_t &isCollection, Bool_t &isXml, Bool_t &useTags)
{
// Check input data type.
   isCollection = kFALSE;
   isXml = kFALSE;
   useTags = kFALSE;
   if (!gGrid) {
      Error("CheckDataType", "No connection to grid");
      return;
   }
   isCollection = IsCollection(lfn);
   TString msg = "\n#####   file: ";
   msg += lfn;
   if (isCollection) {
      msg += " type: raw_collection;";
   // special treatment for collections
      isXml = kFALSE;
      // check for tag files in the collection
      TGridResult *res = gGrid->Command(Form("listFilesFromCollection -z -v %s",lfn), kFALSE);
      if (!res) {
         msg += " using_tags: No (unknown)";
         Info("CheckDataType", "%s", msg.Data());
         return;
      }   
      const char* typeStr = res->GetKey(0, "origLFN");
      if (!typeStr || !strlen(typeStr)) {
         msg += " using_tags: No (unknown)";
         Info("CheckDataType", "%s", msg.Data());
         return;
      }   
      TString file = typeStr;
      useTags = file.Contains(".tag");
      if (useTags) msg += " using_tags: Yes";
      else          msg += " using_tags: No";
      Info("CheckDataType", "%s", msg.Data());
      return;
   }
   TString slfn(lfn);
   slfn.ToLower();
   isXml = slfn.Contains(".xml");
   if (isXml) {
   // Open xml collection and check if there are tag files inside
      msg += " type: xml_collection;";
      TGridCollection *coll = (TGridCollection*)gROOT->ProcessLine(Form("TAlienCollection::Open(\"alien://%s\",1);",lfn));
      if (!coll) {
         msg += " using_tags: No (unknown)";
         Info("CheckDataType", "%s", msg.Data());
         return;
      }   
      TMap *map = coll->Next();
      if (!map) {
         msg += " using_tags: No (unknown)";
         Info("CheckDataType", "%s", msg.Data());
         return;
      }   
      map = (TMap*)map->GetValue("");
      TString file;
      if (map && map->GetValue("name")) file = map->GetValue("name")->GetName();
      useTags = file.Contains(".tag");
      delete coll;
      if (useTags) msg += " using_tags: Yes";
      else          msg += " using_tags: No";
      Info("CheckDataType", "%s", msg.Data());
      return;
   }
   useTags = slfn.Contains(".tag");
   if (slfn.Contains(".root")) msg += " type: root file;";
   else                        msg += " type: unknown file;";
   if (useTags) msg += " using_tags: Yes";
   else          msg += " using_tags: No";
   Info("CheckDataType", "%s", msg.Data());
}

//______________________________________________________________________________
void AliAnalysisAlien::EnablePackage(const char *package)
{
// Enables a par file supposed to exist in the current directory.
   TString pkg(package);
   pkg.ReplaceAll(".par", "");
   pkg += ".par";
   if (gSystem->AccessPathName(pkg)) {
      Fatal("EnablePackage", "Package %s not found", pkg.Data());
      return;
   }
   if (!TObject::TestBit(AliAnalysisGrid::kUsePars))
      Info("EnablePackage", "AliEn plugin will use .par packages");
   TObject::SetBit(AliAnalysisGrid::kUsePars, kTRUE);
   if (!fPackages) {
      fPackages = new TObjArray();
      fPackages->SetOwner();
   }
   fPackages->Add(new TObjString(pkg));
}      

//______________________________________________________________________________
TChain *AliAnalysisAlien::GetChainForTestMode(const char *treeName) const
{
// Make a tree from files having the location specified in fFileForTestMode. 
// Inspired from JF's CreateESDChain.
   if (fFileForTestMode.IsNull()) {
      Error("GetChainForTestMode", "For proof test mode please use SetFileForTestMode() pointing to a file that contains data file locations.");
      return NULL;
   }
   if (gSystem->AccessPathName(fFileForTestMode)) {
      Error("GetChainForTestMode", "File not found: %s", fFileForTestMode.Data());
      return NULL;
   }   
   // Open the file
   ifstream in;
   in.open(fFileForTestMode);
   Int_t count = 0;
    // Read the input list of files and add them to the chain
    TString line;
    TChain *chain = new TChain(treeName);
    while (in.good())
    {
      in >> line;
      if (line.IsNull()) continue;
      if (count++ == fNtestFiles) break;
      TString esdFile(line);
      TFile *file = TFile::Open(esdFile);
      if (file) {
         if (!file->IsZombie()) chain->Add(esdFile);
         file->Close();
      } else {
         Error("GetChainforTestMode", "Skipping un-openable file: %s", esdFile.Data());
      }   
    }
    in.close();
    if (!chain->GetListOfFiles()->GetEntries()) {
       Error("GetChainForTestMode", "No file from %s could be opened", fFileForTestMode.Data());
       delete chain;
       return NULL;
    }
//    chain->ls();
    return chain;
}    

//______________________________________________________________________________
const char *AliAnalysisAlien::GetJobStatus(Int_t jobidstart, Int_t lastid, Int_t &nrunning, Int_t &nwaiting, Int_t &nerror, Int_t &ndone)
{
// Get job status for all jobs with jobid>jobidstart.
   static char mstatus[20];
   mstatus[0] = '\0';
   nrunning = 0;
   nwaiting = 0;
   nerror   = 0;
   ndone    = 0;
   TGridJobStatusList *list = gGrid->Ps("");
   if (!list) return mstatus;
   Int_t nentries = list->GetSize();
   TGridJobStatus *status;
   Int_t pid;
   for (Int_t ijob=0; ijob<nentries; ijob++) {
      status = (TGridJobStatus *)list->At(ijob);
      pid = gROOT->ProcessLine(Form("atoi(((TAlienJobStatus*)0x%lx)->GetKey(\"queueId\"));", (ULong_t)status));
      if (pid<jobidstart) continue;
      if (pid == lastid) {
         gROOT->ProcessLine(Form("sprintf((char*)0x%lx,((TAlienJobStatus*)0x%lx)->GetKey(\"status\"));",(ULong_t)mstatus, (ULong_t)status));
      }   
      switch (status->GetStatus()) {
         case TGridJobStatus::kWAITING:
            nwaiting++; break;
         case TGridJobStatus::kRUNNING:
            nrunning++; break;
         case TGridJobStatus::kABORTED:
         case TGridJobStatus::kFAIL:
         case TGridJobStatus::kUNKNOWN:
            nerror++; break;
         case TGridJobStatus::kDONE:
            ndone++;
      }
   }
   list->Delete();
   delete list;
   return mstatus;
}

//______________________________________________________________________________
Bool_t AliAnalysisAlien::IsCollection(const char *lfn) const
{
// Returns true if file is a collection. Functionality duplicated from
// TAlien::Type() because we don't want to directly depend on TAlien.
   if (!gGrid) {
      Error("IsCollection", "No connection to grid");
      return kFALSE;
   }
   TGridResult *res = gGrid->Command(Form("type -z %s",lfn),kFALSE);
   if (!res) return kFALSE;
   const char* typeStr = res->GetKey(0, "type");
   if (!typeStr || !strlen(typeStr)) return kFALSE;
   if (!strcmp(typeStr, "collection")) return kTRUE;
   delete res;
   return kFALSE;
}   

//______________________________________________________________________________
Bool_t AliAnalysisAlien::IsSingleOutput() const
{
// Check if single-ouput option is on.
   return (!fOutputSingle.IsNull());
}
   
//______________________________________________________________________________
void AliAnalysisAlien::Print(Option_t *) const
{
// Print current plugin settings.
   printf("### AliEn analysis plugin current settings ###\n");
   AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
   if (mgr && mgr->IsProofMode()) {
      TString proofType = "=   PLUGIN IN PROOF MODE ON CLUSTER:_________________";
      if (TestBit(AliAnalysisGrid::kTest))
         proofType = "=   PLUGIN IN PROOF LITE MODE ON CLUSTER:____________";
      printf("%s %s\n", proofType.Data(), fProofCluster.Data());
      if (!fProofDataSet.IsNull())
      printf("=   Requested data set:___________________________ %s\n", fProofDataSet.Data());
      if (fProofReset==1)
      printf("=   Soft reset signal will be send to master______ CHANGE BEHAVIOR AFTER COMPLETION\n");      
      if (fProofReset>1)   
      printf("=   Hard reset signal will be send to master______ CHANGE BEHAVIOR AFTER COMPLETION\n");      
      if (!fRootVersionForProof.IsNull())
      printf("=   ROOT version requested________________________ %s\n", fRootVersionForProof.Data());
      else
      printf("=   ROOT version requested________________________ default\n");
      printf("=   AliRoot version requested_____________________ %s\n", fAliROOTVersion.Data());
      if (!fAliRootMode.IsNull())
      printf("=   Requested AliRoot mode________________________ %s\n", fAliRootMode.Data());  
      if (fNproofWorkers)
      printf("=   Number of PROOF workers limited to____________ %d\n", fNproofWorkers);
      if  (fNproofWorkersPerSlave)
      printf("=   Maximum number of workers per slave___________ %d\n", fNproofWorkersPerSlave);
      if (TestSpecialBit(kClearPackages))
      printf("=   ClearPackages requested...\n");
      if (fIncludePath.Data())
      printf("=   Include path for runtime task compilation: ___ %s\n", fIncludePath.Data());
      printf("=   Additional libs to be loaded or souces to be compiled runtime: <%s>\n",fAdditionalLibs.Data());
      if (fPackages && fPackages->GetEntries()) {
         TIter next(fPackages);
         TObject *obj;
         TString list;
         while ((obj=next())) list += obj->GetName();
         printf("=   Par files to be used: ________________________ %s\n", list.Data());
      } 
      if (TestSpecialBit(kProofConnectGrid))
      printf("=   Requested PROOF connection to grid\n");
      return;
   }
   printf("=   OverwriteMode:________________________________ %d\n", fOverwriteMode);
   if (fOverwriteMode) {
      printf("***** NOTE: Overwrite mode will overwrite the input generated datasets and partial results from previous analysis. \
            \n*****       To disable, use: plugin->SetOverwriteMode(kFALSE);\n");
   }
   printf("=   Copy files to grid: __________________________ %s\n", (IsUseCopy())?"YES":"NO");
   printf("=   Check if files can be copied to grid: ________ %s\n", (IsCheckCopy())?"YES":"NO");
   printf("=   Production mode:______________________________ %d\n", fProductionMode);
   printf("=   Version of API requested: ____________________ %s\n", fAPIVersion.Data());
   printf("=   Version of ROOT requested: ___________________ %s\n", fROOTVersion.Data());
   printf("=   Version of AliRoot requested: ________________ %s\n", fAliROOTVersion.Data());
   if (fUser.Length()) 
   printf("=   User running the plugin: _____________________ %s\n", fUser.Data());
   printf("=   Grid workdir relative to user $HOME: _________ %s\n", fGridWorkingDir.Data());
   printf("=   Grid output directory relative to workdir: ___ %s\n", fGridOutputDir.Data());
   printf("=   Data base directory path requested: __________ %s\n", fGridDataDir.Data());
   printf("=   Data search pattern: _________________________ %s\n", fDataPattern.Data());
   printf("=   Input data format: ___________________________ %s\n", fInputFormat.Data());
   if (fRunNumbers.Length()) 
   printf("=   Run numbers to be processed: _________________ %s\n", fRunNumbers.Data());
   if (fRunRange[0])
   printf("=   Run range to be processed: ___________________ %s%d-%s%d\n", fRunPrefix.Data(), fRunRange[0], fRunPrefix.Data(), fRunRange[1]);
   if (!fRunRange[0] && !fRunNumbers.Length()) {
      TIter next(fInputFiles);
      TObject *obj;
      TString list;
      while ((obj=next())) list += obj->GetName();
      printf("=   Input files to be processed: _________________ %s\n", list.Data());
   }
   if (TestBit(AliAnalysisGrid::kTest))
   printf("=   Number of input files used in test mode: _____ %d\n", fNtestFiles);
   printf("=   List of output files to be registered: _______ %s\n", fOutputFiles.Data());
   printf("=   List of outputs going to be archived: ________ %s\n", fOutputArchive.Data());
   printf("=   List of outputs that should not be merged: ___ %s\n", fMergeExcludes.Data());
   printf("=   List of outputs produced during Terminate: ___ %s\n", fTerminateFiles.Data());
   printf("=====================================================================\n");
   printf("=   Job price: ___________________________________ %d\n", fPrice);
   printf("=   Time to live (TTL): __________________________ %d\n", fTTL);
   printf("=   Max files per subjob: ________________________ %d\n", fSplitMaxInputFileNumber);
   if (fMaxInitFailed>0) 
   printf("=   Max number of subjob fails to kill: __________ %d\n", fMaxInitFailed);
   if (fMasterResubmitThreshold>0) 
   printf("=   Resubmit master job if failed subjobs >_______ %d\n", fMasterResubmitThreshold);
   printf("=   Number of replicas for the output files_______ %d\n", fNreplicas);
   if (fNrunsPerMaster>0)
   printf("=   Number of runs per master job: _______________ %d\n", fNrunsPerMaster);
   printf("=   Number of files in one chunk to be merged: ___ %d\n", fMaxMergeFiles);
   printf("=   Name of the generated execution script: ______ %s\n", fExecutable.Data());
   printf("=   Executable command: __________________________ %s\n", fExecutableCommand.Data());
   if (fArguments.Length()) 
   printf("=   Arguments for the execution script: __________ %s\n",fArguments.Data());
   if (fExecutableArgs.Length()) 
   printf("=   Arguments after macro name in executable______ %s\n",fExecutableArgs.Data());
   printf("=   Name of the generated analysis macro: ________ %s\n",fAnalysisMacro.Data());
   printf("=   User analysis files to be deployed: __________ %s\n",fAnalysisSource.Data());
   printf("=   Additional libs to be loaded or souces to be compiled runtime: <%s>\n",fAdditionalLibs.Data());
   printf("=   Master jobs split mode: ______________________ %s\n",fSplitMode.Data());
   if (fDatasetName)
   printf("=   Custom name for the dataset to be created: ___ %s\n", fDatasetName.Data());
   printf("=   Name of the generated JDL: ___________________ %s\n", fJDLName.Data());
   if (fIncludePath.Data())
   printf("=   Include path for runtime task compilation: ___ %s\n", fIncludePath.Data());
   if (fCloseSE.Length())
   printf("=   Force job outputs to storage element: ________ %s\n", fCloseSE.Data());
   if (fFriendChainName.Length())
   printf("=   Open friend chain file on worker: ____________ %s\n", fFriendChainName.Data());
   if (fPackages && fPackages->GetEntries()) {
      TIter next(fPackages);
      TObject *obj;
      TString list;
      while ((obj=next())) list += obj->GetName();
      printf("=   Par files to be used: ________________________ %s\n", list.Data());
   }   
}

//______________________________________________________________________________
void AliAnalysisAlien::SetDefaults()
{
// Set default values for everything. What cannot be filled will be left empty.
   if (fGridJDL) delete fGridJDL;
   fGridJDL = (TGridJDL*)gROOT->ProcessLine("new TAlienJDL()");
   fMergingJDL = (TGridJDL*)gROOT->ProcessLine("new TAlienJDL()");
   fPrice                      = 1;
   fTTL                        = 30000;
   fSplitMaxInputFileNumber    = 100;
   fMaxInitFailed              = 0;
   fMasterResubmitThreshold    = 0;
   fNtestFiles                 = 10;
   fNreplicas                  = 2;
   fRunRange[0]                = 0;
   fRunRange[1]                = 0;
   fNrunsPerMaster             = 1;
   fMaxMergeFiles              = 100;
   fRunNumbers                 = "";
   fExecutable                 = "analysis.sh";
   fExecutableCommand          = "root -b -q";
   fArguments                  = "";
   fExecutableArgs             = "";
   fAnalysisMacro              = "myAnalysis.C";
   fAnalysisSource             = "";
   fAdditionalLibs             = "";
   fSplitMode                  = "se";
   fAPIVersion                 = "";
   fROOTVersion                = "";
   fAliROOTVersion             = "";
   fUser                       = "";  // Your alien user name
   fGridWorkingDir             = "";
   fGridDataDir                = "";  // Can be like: /alice/sim/PDC_08a/LHC08c9/
   fDataPattern                = "*AliESDs.root";  // Can be like: *AliESDs.root, */pass1/*AliESDs.root, ...
   fFriendChainName            = "";
   fGridOutputDir              = "output";
   fOutputArchive              = "log_archive.zip:std*@disk=1 root_archive.zip:*.root@disk=2";
   fOutputFiles                = "";  // Like "AliAODs.root histos.root"
   fInputFormat                = "xml-single";
   fJDLName                    = "analysis.jdl";
   fJobTag                     = "Automatically generated analysis JDL";
   fMergeExcludes              = "";
   fMergeViaJDL                = 0;
   SetUseCopy(kTRUE);
   SetCheckCopy(kTRUE);
   SetDefaultOutputs(kTRUE);
   fOverwriteMode              = 1;
}   

//______________________________________________________________________________
Bool_t AliAnalysisAlien::CheckMergedFiles(const char *filename, const char *aliendir, Int_t nperchunk, Bool_t submit, const char *jdl)
{
// Static method that checks the status of merging. This can submit merging jobs that did not produced the expected
// output. If <submit> is false (checking) returns true only when the final merged file was found. If submit is true returns
// true if the jobs were successfully submitted.
   Int_t countOrig = 0;
   Int_t countStage = 0;
   Int_t stage = 0;
   Int_t i;
   Bool_t doneFinal = kFALSE;
   TBits chunksDone;
   TString saliendir(aliendir);
   TString sfilename, stmp;
   saliendir.ReplaceAll("//","/");
   saliendir = saliendir.Strip(TString::kTrailing, '/');
   if (!gGrid) {
      ::Error("GetNregisteredFiles", "You need to be connected to AliEn.");
      return kFALSE;
   }
   sfilename = filename;
   sfilename.ReplaceAll(".root", "*.root");
   printf("Checking directory <%s> for merged files <%s> ...\n", aliendir, sfilename.Data());
   TString command = Form("find %s/ *%s", saliendir.Data(), sfilename.Data());
   TGridResult *res = gGrid->Command(command);
   if (!res) {
      ::Error("GetNregisteredFiles","Error: No result for the find command\n");
      return kFALSE;
   }     
   TIter nextmap(res);
   TMap *map = 0;   
   while ((map=(TMap*)nextmap())) {
      TString turl = map->GetValue("turl")->GetName();
      if (!turl.Length()) {
         // Nothing found
         delete res;
         return kFALSE;
      }
      turl.ReplaceAll("alien://", "");
      turl.ReplaceAll(saliendir, "");
      sfilename = gSystem->BaseName(turl);
      turl = turl.Strip(TString::kLeading, '/');
      // Now check to what the file corresponds to: 
      //    original output           - aliendir/%03d/filename
      //    merged file (which stage) - aliendir/filename-Stage%02d_%04d
      //    final merged file         - aliendir/filename
      if (sfilename == turl) {
         if (sfilename == filename) {
            doneFinal = kTRUE;
         } else {   
            // check stage
            Int_t index = sfilename.Index("Stage");
            if (index<0) continue;
            stmp = sfilename(index+5,2);
            Int_t istage = atoi(stmp);
            stmp = sfilename(index+8,4);
            Int_t ijob = atoi(stmp);
            if (istage<stage) continue; // Ignore lower stages
            if (istage>stage) {
               countStage = 0;
               chunksDone.ResetAllBits();
               stage = istage;
            }
            countStage++;
            chunksDone.SetBitNumber(ijob);
         }     
      } else {
         countOrig++;
      }
      if (doneFinal) {
         delete res;
         printf("=> Removing files from previous stages...\n");
         gGrid->Rm(Form("%s/*Stage*.root", aliendir));
         for (i=1; i<stage; i++)
            gGrid->Rm(Form("%s/*Stage%d*.zip", aliendir, i));
         return kTRUE;
      }               
   }
   delete res;
   // Compute number of jobs that were submitted for the current stage
   Int_t ntotstage = countOrig;
   for (i=1; i<=stage; i++) {
      if (ntotstage%nperchunk) ntotstage = (ntotstage/nperchunk)+1;
      else                     ntotstage = (ntotstage/nperchunk);
   }   
   // Now compare with the number of set bits in the chunksDone array
   Int_t nmissing = (stage>0)?(ntotstage - countStage):0;
   // Print the info
   printf("*** Found %d original files\n", countOrig);
   if (stage==0) printf("*** No merging completed so far.\n");
   else          printf("*** Found %d out of %d files merged for stage %d\n", countStage, ntotstage, stage);
   if (nmissing) printf("*** Number of merged files missing for this stage: %d -> check merging job completion\n", nmissing);
   if (!submit) return doneFinal;
   // Sumbit merging jobs for all missing chunks for the current stage.
   TString query = Form("submit %s %s", jdl, aliendir);
   Int_t ichunk = -1;
   if (nmissing) {
      for (i=0; i<nmissing; i++) {
         ichunk = chunksDone.FirstNullBit(ichunk+1);
         Int_t jobId = SubmitSingleJob(Form("%s %d %d", query.Data(), stage, ichunk));
         if (!jobId) return kFALSE;
      }
      return kTRUE;
   }
   // Submit next stage of merging
   if (stage==0) countStage = countOrig;
   Int_t nchunks = (countStage/nperchunk);
   if (countStage%nperchunk) nchunks += 1;
   for (i=0; i<nchunks; i++) {
      Int_t jobId = SubmitSingleJob(Form("%s %d %d", query.Data(), stage+1, i));
      if (!jobId) return kFALSE;
   }        
   return kTRUE;
}      

//______________________________________________________________________________
Int_t AliAnalysisAlien::SubmitSingleJob(const char *query)
{
// Submits a single job corresponding to the query and returns job id. If 0 submission failed.
   if (!gGrid) return 0;
   printf("=> %s ------> ",query);
   TGridResult *res = gGrid->Command(query);
   if (!res) return 0;
   TString jobId = res->GetKey(0,"jobId");
   delete res;
   if (jobId.IsNull()) {
      printf("submission failed. Reason:\n");
      gGrid->Stdout();
      gGrid->Stderr();
      ::Error("SubmitSingleJob", "Your query %s could not be submitted", query);
      return 0;
   }
   printf(" Job id: %s\n", jobId.Data());
   return atoi(jobId);
}  

//______________________________________________________________________________
Bool_t AliAnalysisAlien::MergeOutput(const char *output, const char *basedir, Int_t nmaxmerge, Int_t stage, Int_t ichunk)
{
// Merge given output files from basedir. The file merger will merge nmaxmerge
// files in a group. Merging can be done in stages:
// stage=0 : will merge all existing files in a single stage
// stage=1 : does a find command for all files that do NOT contain the string "Stage". 
//           If their number is bigger that nmaxmerge, only the files from 
//           ichunk*nmaxmerge to ichunk*(nmaxmerge+1)-1 will get merged as output_stage_<ichunk>
// stage=n : does a find command for files named <output>Stage<stage-1>_*. If their number is bigger than
//           nmaxmerge, merge just the chunk ichunk, otherwise write the merged output to the file 
//           named <output>.
   TString outputFile = output;
   TString command;
   TString outputChunk;
   TString previousChunk = "";
   Int_t countChunk = 0;
   Int_t countZero = nmaxmerge;
   Bool_t merged = kTRUE;
   Int_t index = outputFile.Index("@");
   if (index > 0) outputFile.Remove(index);
   TString inputFile = outputFile;
   if (stage>1) inputFile.ReplaceAll(".root", Form("-Stage%02d_*.root", stage-1));
   command = Form("find %s/ *%s", basedir, inputFile.Data());
   printf("command: %s\n", command.Data());
   TGridResult *res = gGrid->Command(command);
   if (!res) {
      ::Error("MergeOutput","No result for the find command\n");
      return kFALSE;
   }     

   TFileMerger *fm = 0;
   TIter nextmap(res);
   TMap *map = 0;
   // Check if there is a merge operation to resume. Works only for stage 0 or 1.
   outputChunk = outputFile;
   outputChunk.ReplaceAll(".root", "_*.root");
   // Check for existent temporary merge files
   // Check overwrite mode and remove previous partial results if needed
   // Preserve old merging functionality for stage 0.
   if (stage==0) {
      if (!gSystem->Exec(Form("ls %s 2>/dev/null", outputChunk.Data()))) {
         while (1) {
            // Skip as many input files as in a chunk
            for (Int_t counter=0; counter<nmaxmerge; counter++) map = (TMap*)nextmap();
            if (!map) {
               ::Error("MergeOutput", "Cannot resume merging for <%s>, nentries=%d", outputFile.Data(), res->GetSize());
               delete res;
               return kFALSE;
            }
            outputChunk = outputFile;
            outputChunk.ReplaceAll(".root", Form("_%04d.root", countChunk));
            countChunk++;
            if (gSystem->AccessPathName(outputChunk)) continue;
            // Merged file with chunks up to <countChunk> found
            ::Info("MergeOutput", "Resume merging of <%s> from <%s>\n", outputFile.Data(), outputChunk.Data());
            previousChunk = outputChunk;
            break;
         }
      }   
      countZero = nmaxmerge;
   
      while ((map=(TMap*)nextmap())) {
      // Loop 'find' results and get next LFN
         if (countZero == nmaxmerge) {
            // First file in chunk - create file merger and add previous chunk if any.
            fm = new TFileMerger(kFALSE);
            fm->SetFastMethod(kTRUE);
            if (previousChunk.Length()) fm->AddFile(previousChunk.Data());
            outputChunk = outputFile;
            outputChunk.ReplaceAll(".root", Form("_%04d.root", countChunk));
         }
         // If last file found, put merged results in the output file
         if (map == res->Last()) outputChunk = outputFile;
         TObjString *objs = dynamic_cast<TObjString*>(map->GetValue("turl"));
         if (!objs || !objs->GetString().Length()) {
            // Nothing found - skip this output
            delete res;
            delete fm;
            return kFALSE;
         } 
         // Add file to be merged and decrement chunk counter.
         fm->AddFile(objs->GetString());
         countZero--;
         if (countZero==0 || map == res->Last()) {            
            if (!fm->GetMergeList() || !fm->GetMergeList()->GetSize()) {
            // Nothing found - skip this output
               ::Warning("MergeOutput", "No <%s> files found.", inputFile.Data());
               delete res;
               delete fm;
               return kFALSE;
            }
            fm->OutputFile(outputChunk);
            // Merge the outputs, then go to next chunk      
            if (!fm->Merge()) {
               ::Error("MergeOutput", "Could not merge all <%s> files", outputFile.Data());
               delete res;
               delete fm;
               return kFALSE;
            } else {
               ::Info("MergeOutputs", "\n#####   Merged %d output files to <%s>", fm->GetMergeList()->GetSize(), outputChunk.Data());
               gSystem->Unlink(previousChunk);
            }
            if (map == res->Last()) {
               delete res;
               delete fm;
               break;
            }      
            countChunk++;
            countZero = nmaxmerge;
            previousChunk = outputChunk;
         }
      }
      return merged;
   }
   // Merging stage different than 0.
   // Move to the begining of the requested chunk.
   outputChunk = outputFile;
   if (nmaxmerge < res->GetSize()) {
      if (ichunk*nmaxmerge >= res->GetSize()) {
         ::Error("MergeOutput", "Cannot merge merge chunk %d grouping %d files from %d total.", ichunk, nmaxmerge, res->GetSize());
         delete res;
         return kFALSE;
      }   
      for (Int_t counter=0; counter<ichunk*nmaxmerge; counter++) map = (TMap*)nextmap();
      outputChunk.ReplaceAll(".root", Form("-Stage%02d_%04d.root", stage, ichunk));
   }
   countZero = nmaxmerge;  
   fm = new TFileMerger(kFALSE);
   fm->SetFastMethod(kTRUE);
   while ((map=(TMap*)nextmap())) {
      // Loop 'find' results and get next LFN
      TObjString *objs = dynamic_cast<TObjString*>(map->GetValue("turl"));
      if (!objs || !objs->GetString().Length()) {
         // Nothing found - skip this output
         delete res;
         delete fm;
         return kFALSE;
      } 
      // Add file to be merged and decrement chunk counter.
      fm->AddFile(objs->GetString());
      countZero--;
      if (countZero==0) break;
   }
   delete res;
   if (!fm->GetMergeList() || !fm->GetMergeList()->GetSize()) {
      // Nothing found - skip this output
      ::Warning("MergeOutput", "No <%s> files found.", inputFile.Data());
      delete fm;
      return kFALSE;
   }
   fm->OutputFile(outputChunk);
   // Merge the outputs
   if (!fm->Merge()) {
      ::Error("MergeOutput", "Could not merge all <%s> files", outputFile.Data());
      delete fm;
      return kFALSE;
   } else {
      ::Info("MergeOutput", "\n#####   Merged %d output files to <%s>", fm->GetMergeList()->GetSize(), outputChunk.Data());
   }
   delete fm;
   return kTRUE;
} 

//______________________________________________________________________________
Bool_t AliAnalysisAlien::MergeOutputs()
{
// Merge analysis outputs existing in the AliEn space.
   if (TestBit(AliAnalysisGrid::kTest)) return kTRUE;
   if (TestBit(AliAnalysisGrid::kOffline)) return kFALSE;
   if (!Connect()) {
      Error("MergeOutputs", "Cannot merge outputs without grid connection. Terminate will NOT be executed");
      return kFALSE;
   }
   if (fMergeViaJDL) {
      if (!TestBit(AliAnalysisGrid::kMerge)) {
         Info("MergeOutputs", "### Re-run with <MergeViaJDL> option in terminate mode of the plugin to submit merging jobs ###");
         return kFALSE; 
      }     
      if (fProductionMode) {
         Info("MergeOutputs", "### Merging will be submitted by LPM manager... ###");
         return kFALSE;
      }
      Info("MergeOutputs", "Submitting merging JDL");
      if (!SubmitMerging()) return kFALSE;
      Info("MergeOutputs", "### Re-run with <MergeViaJDL> off to collect results after merging jobs are done ###");
      Info("MergeOutputs", "### The Terminate() method is executed by the merging jobs");
      return kFALSE;
   }   
   // Get the output path
   if (!fGridOutputDir.Contains("/")) fGridOutputDir = Form("/%s/%s/%s", gGrid->GetHomeDirectory(), fGridWorkingDir.Data(), fGridOutputDir.Data());
   if (!DirectoryExists(fGridOutputDir)) {
      Error("MergeOutputs", "Grid output directory %s not found. Terminate() will NOT be executed", fGridOutputDir.Data());
      return kFALSE;
   }
   if (!fOutputFiles.Length()) {
      Error("MergeOutputs", "No output file names defined. Are you running the right AliAnalysisAlien configuration ?");
      return kFALSE;
   }
   // Check if fast read option was requested
   Info("MergeOutputs", "Started local merging of output files from: alien://%s \
        \n======= overwrite mode = %d", fGridOutputDir.Data(), (Int_t)fOverwriteMode);
   if (fFastReadOption) {
      Warning("MergeOutputs", "You requested FastRead option. Using xrootd flags to reduce timeouts. This may skip some files that could be accessed ! \
             \n+++ NOTE: To disable this option, use: plugin->SetFastReadOption(kFALSE)");
      gEnv->SetValue("XNet.ConnectTimeout",10);
      gEnv->SetValue("XNet.RequestTimeout",10);
      gEnv->SetValue("XNet.MaxRedirectCount",2);
      gEnv->SetValue("XNet.ReconnectTimeout",10);
      gEnv->SetValue("XNet.FirstConnectMaxCnt",1);
   }   
   // Make sure we change the temporary directory
   gSystem->Setenv("TMPDIR", gSystem->pwd());
   TObjArray *list = fOutputFiles.Tokenize(",");
   TIter next(list);
   TObjString *str;
   TString outputFile;
   Bool_t merged = kTRUE;
   while((str=(TObjString*)next())) {
      outputFile = str->GetString();
      Int_t index = outputFile.Index("@");
      if (index > 0) outputFile.Remove(index);
      TString outputChunk = outputFile;
      outputChunk.ReplaceAll(".root", "_*.root");
      // Skip already merged outputs
      if (!gSystem->AccessPathName(outputFile)) {
         if (fOverwriteMode) {
            Info("MergeOutputs", "Overwrite mode. Existing file %s was deleted.", outputFile.Data());
            gSystem->Unlink(outputFile);
            if (!gSystem->Exec(Form("ls %s 2>/dev/null", outputChunk.Data()))) {
               Info("MergeOutput", "Overwrite mode: partial merged files %s will removed",
                     outputChunk.Data());
               gSystem->Exec(Form("rm -f %s", outputChunk.Data()));
            }
         } else {   
            Info("MergeOutputs", "Output file <%s> found. Not merging again.", outputFile.Data());
            continue;
         }   
      } else {
         if (!gSystem->Exec(Form("ls %s 2>/dev/null", outputChunk.Data()))) {
            Info("MergeOutput", "Overwrite mode: partial merged files %s will removed",
                  outputChunk.Data());
            gSystem->Exec(Form("rm -f %s", outputChunk.Data()));
         }   
      }
      if (fMergeExcludes.Length() &&
          fMergeExcludes.Contains(outputFile.Data())) continue;
      // Perform a 'find' command in the output directory, looking for registered outputs    
      merged = MergeOutput(outputFile, fGridOutputDir, fMaxMergeFiles);
      if (!merged) {
         Error("MergeOutputs", "Terminate() will  NOT be executed");
         return kFALSE;
      }
      TFile *fileOpened = (TFile*)gROOT->GetListOfFiles()->FindObject(outputFile);
      if (fileOpened) fileOpened->Close();
   } 
   return kTRUE;
}   

//______________________________________________________________________________
void AliAnalysisAlien::SetDefaultOutputs(Bool_t flag)
{
// Use the output files connected to output containers from the analysis manager
// rather than the files defined by SetOutputFiles
   if (flag && !TObject::TestBit(AliAnalysisGrid::kDefaultOutputs))
      Info("SetDefaultOutputs", "Plugin will use the output files taken from analysis manager");
   TObject::SetBit(AliAnalysisGrid::kDefaultOutputs, flag);
}
      
//______________________________________________________________________________
void AliAnalysisAlien::SetOutputFiles(const char *list)
{
// Manually set the output files list.
// Removes duplicates. Not allowed if default outputs are not disabled.
   if (TObject::TestBit(AliAnalysisGrid::kDefaultOutputs)) {
      Fatal("SetOutputFiles", "You have to explicitly call SetDefaultOutputs(kFALSE) to manually set output files.");
      return;
   }
   Info("SetOutputFiles", "Output file list is set manually - you are on your own.");
   fOutputFiles = "";
   TString slist = list;
   if (slist.Contains("@")) Warning("SetOutputFiles","The plugin does not allow explicit SE's. Please use: SetNumberOfReplicas() instead.");
   TObjArray *arr = slist.Tokenize(" "); 
   TObjString *os;
   TIter next(arr);
   TString sout;
   while ((os=(TObjString*)next())) {
      sout = os->GetString();
      if (sout.Index("@")>0) sout.Remove(sout.Index("@"));
      if (fOutputFiles.Contains(sout)) continue;
      if (!fOutputFiles.IsNull()) fOutputFiles += ",";
      fOutputFiles += sout;
   }
   delete arr;   
}

//______________________________________________________________________________
void AliAnalysisAlien::SetOutputArchive(const char *list)
{
// Manually set the output archive list. Free text - you are on your own...
// Not allowed if default outputs are not disabled.
   if (TObject::TestBit(AliAnalysisGrid::kDefaultOutputs)) {
      Fatal("SetOutputArchive", "You have to explicitly call SetDefaultOutputs(kFALSE) to manually set the output archives.");
      return;
   }
   Info("SetOutputArchive", "Output archive is set manually - you are on your own.");
   fOutputArchive = list;
}

//______________________________________________________________________________
void AliAnalysisAlien::SetPreferedSE(const char */*se*/)
{
// Setting a prefered output SE is not allowed anymore.
   Warning("SetPreferedSE", "Setting a preferential SE is not allowed anymore via the plugin. Use SetNumberOfReplicas() and SetDefaultOutputs()");
}

//______________________________________________________________________________
Bool_t AliAnalysisAlien::StartAnalysis(Long64_t /*nentries*/, Long64_t /*firstEntry*/)
{
// Start remote grid analysis.
   AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
   Bool_t testMode = TestBit(AliAnalysisGrid::kTest);
   if (!mgr || !mgr->IsInitialized()) {
      Error("StartAnalysis", "You need an initialized analysis manager for this");
      return kFALSE;
   }
   // Are we in PROOF mode ?
   if (mgr->IsProofMode()) {
      Info("StartAnalysis", "##### Starting PROOF analysis on cluster <%s> via the plugin #####", fProofCluster.Data());
      if (fProofCluster.IsNull()) {
         Error("StartAnalysis", "You need to specify the proof cluster name via SetProofCluster");
         return kFALSE;
      }   
      if (fProofDataSet.IsNull() && !testMode) {
         Error("StartAnalysis", "You need to specify a dataset using SetProofDataSet()");
         return kFALSE;
      }   
      // Set the needed environment
      gEnv->SetValue("XSec.GSI.DelegProxy","2");
      // Do we need to reset PROOF ? The success of the Reset operation cannot be checked
      if (fProofReset && !testMode) {
         if (fProofReset==1) {
            Info("StartAnalysis", "Sending soft reset signal to proof cluster %s", fProofCluster.Data());
            gROOT->ProcessLine(Form("TProof::Reset(\"%s\", kFALSE);", fProofCluster.Data()));
         } else {         
            Info("StartAnalysis", "Sending hard reset signal to proof cluster %s", fProofCluster.Data());
            gROOT->ProcessLine(Form("TProof::Reset(\"%s\", kTRUE);", fProofCluster.Data()));
         }
         Info("StartAnalysis", "Stopping the analysis. Please use SetProofReset(0) to resume.");
         return kFALSE;
      }
      // Do we need to change the ROOT version ? The success of this cannot be checked.
      if (!fRootVersionForProof.IsNull() && !testMode) {
         gROOT->ProcessLine(Form("TProof::Mgr(\"%s\")->SetROOTVersion(\"%s\");", 
                            fProofCluster.Data(), fRootVersionForProof.Data()));
      }
      // Connect to PROOF and check the status
      Long_t proof = 0;
      TString sworkers;
      if (fNproofWorkersPerSlave) sworkers = Form("workers=%dx", fNproofWorkersPerSlave);
      else if (fNproofWorkers) sworkers = Form("workers=%d", fNproofWorkers);
      if (!testMode) {
         if (!sworkers.IsNull()) 
            proof = gROOT->ProcessLine(Form("TProof::Open(\"%s\", \"%s\");", fProofCluster.Data(), sworkers.Data()));
         else   
            proof = gROOT->ProcessLine(Form("TProof::Open(\"%s\");", fProofCluster.Data()));
      } else {
         proof = gROOT->ProcessLine("TProof::Open(\"\");");
         if (!proof) {
            Error("StartAnalysis", "Could not start PROOF in test mode");
            return kFALSE;
         }   
      }
      if (!proof) {
         Error("StartAnalysis", "Could not connect to PROOF cluster <%s>", fProofCluster.Data());
         return kFALSE;
      }   
      if (fNproofWorkersPerSlave*fNproofWorkers > 0)
         gROOT->ProcessLine(Form("gProof->SetParallel(%d);", fNproofWorkers));
      // Is dataset existing ?
      if (!testMode) {
         TString dataset = fProofDataSet;
         Int_t index = dataset.Index("#");
         if (index>=0) dataset.Remove(index);
//         if (!gROOT->ProcessLine(Form("gProof->ExistsDataSet(\"%s\");",fProofDataSet.Data()))) {
//            Error("StartAnalysis", "Dataset %s not existing", fProofDataSet.Data());
//            return kFALSE;
//         }
//         Info("StartAnalysis", "Dataset %s found", dataset.Data());
      }
      // Is ClearPackages() needed ?
      if (TestSpecialBit(kClearPackages)) {
         Info("StartAnalysis", "ClearPackages signal sent to PROOF. Use SetClearPackages(kFALSE) to reset this.");
         gROOT->ProcessLine("gProof->ClearPackages();");
      }
      // Is a given aliroot mode requested ?
      TList optionsList;
      if (!fAliRootMode.IsNull()) {
         TString alirootMode = fAliRootMode;
         if (alirootMode == "default") alirootMode = "";
         Info("StartAnalysis", "You are requesting AliRoot mode: %s", fAliRootMode.Data());
         optionsList.SetOwner();
         optionsList.Add(new TNamed("ALIROOT_MODE", alirootMode.Data()));
         // Check the additional libs to be loaded
         TString extraLibs;
         if (!alirootMode.IsNull()) extraLibs = "ANALYSIS:ANALYSISalice";
         // Parse the extra libs for .so
         if (fAdditionalLibs.Length()) {
            TObjArray *list = fAdditionalLibs.Tokenize(" ");
            TIter next(list);
            TObjString *str;
            while((str=(TObjString*)next()) && str->GetString().Contains(".so")) {
               TString stmp = str->GetName();
               if (stmp.BeginsWith("lib")) stmp.Remove(0,3);
               stmp.ReplaceAll(".so","");
               if (!extraLibs.IsNull()) extraLibs += ":";
               extraLibs += stmp;
            }
            if (list) delete list;            
         }
         if (!extraLibs.IsNull()) optionsList.Add(new TNamed("ALIROOT_EXTRA_LIBS",extraLibs.Data()));
         // Check extra includes
         if (!fIncludePath.IsNull()) {
            TString includePath = fIncludePath;
            includePath.ReplaceAll(" ",":");
            includePath.Strip(TString::kTrailing, ':');
            Info("StartAnalysis", "Adding extra includes: %s",includePath.Data()); 
            optionsList.Add(new TNamed("ALIROOT_EXTRA_INCLUDES",includePath.Data()));
         }
         // Check if connection to grid is requested
         if (TestSpecialBit(kProofConnectGrid)) 
            optionsList.Add(new TNamed("ALIROOT_ENABLE_ALIEN", "1"));
         // Enable AliRoot par
         if (testMode) {
         // Enable proof lite package
            TString alirootLite = gSystem->ExpandPathName("$ALICE_ROOT/ANALYSIS/macros/AliRootProofLite.par");
            for (Int_t i=0; i<optionsList.GetSize(); i++) {
               TNamed *obj = (TNamed*)optionsList.At(i);
               printf("%s  %s\n", obj->GetName(), obj->GetTitle());
            }   
            if (!gROOT->ProcessLine(Form("gProof->UploadPackage(\"%s\");",alirootLite.Data()))
              && !gROOT->ProcessLine(Form("gProof->EnablePackage(\"%s\", (TList*)0x%lx);",alirootLite.Data(),(ULong_t)&optionsList))) {
                  Info("StartAnalysis", "AliRootProofLite enabled");
            } else {                      
               Error("StartAnalysis", "There was an error trying to enable package AliRootProofLite.par");
               return kFALSE;
            }   
         } else {
            if (gROOT->ProcessLine(Form("gProof->EnablePackage(\"VO_ALICE@AliRoot::%s\", (TList*)0x%lx);", 
                                   fAliROOTVersion.Data(), (ULong_t)&optionsList))) {
               Error("StartAnalysis", "There was an error trying to enable package VO_ALICE@AliRoot::%s", fAliROOTVersion.Data());
               return kFALSE;
            }         
         }
      } else {
         if (fAdditionalLibs.Contains(".so") && !testMode) {
            Error("StartAnalysis", "You request additional libs to be loaded but did not enabled any AliRoot mode. Please refer to: \
                   \n http://aaf.cern.ch/node/83 and use a parameter for SetAliRootMode()");
            return kFALSE;       
         }
      }
      // Enable par files if requested
      if (fPackages && fPackages->GetEntries()) {
         TIter next(fPackages);
         TObject *package;
         while ((package=next())) {
            if (gROOT->ProcessLine(Form("gProof->UploadPackage(\"%s\");", package->GetName()))) {
               if (gROOT->ProcessLine(Form("gProof->EnablePackage(\"%s\",kTRUE);", package->GetName()))) {
                  Error("StartAnalysis", "There was an error trying to enable package %s", package->GetName());
                  return kFALSE;
               }
            } else {
               Error("StartAnalysis", "There was an error trying to upload package %s", package->GetName());
               return kFALSE;
            }
         }
      }
      // Do we need to load analysis source files ?
      // NOTE: don't load on client since this is anyway done by the user to attach his task.
      if (fAnalysisSource.Length()) {
         TObjArray *list = fAnalysisSource.Tokenize(" ");
         TIter next(list);
         TObjString *str;
         while((str=(TObjString*)next())) {
            gROOT->ProcessLine(Form("gProof->Load(\"%s+g\", kTRUE);", str->GetName()));
         }   
         if (list) delete list;
      }
      if (testMode) {
      // Register dataset to proof lite.
         if (fFileForTestMode.IsNull()) {
            Error("GetChainForTestMode", "For proof test mode please use SetFileForTestMode() pointing to a file that contains data file locations.");
            return kFALSE;
         }
         if (gSystem->AccessPathName(fFileForTestMode)) {
            Error("GetChainForTestMode", "File not found: %s", fFileForTestMode.Data());
            return kFALSE;
         }   
         TFileCollection *coll = new TFileCollection();
         coll->AddFromFile(fFileForTestMode);
         gROOT->ProcessLine(Form("gProof->RegisterDataSet(\"test_collection\", (TFileCollection*)0x%lx, \"OV\");", (ULong_t)coll));
         gROOT->ProcessLine("gProof->ShowDataSets()");
      }
      return kTRUE;
   }
   
   // Check if output files have to be taken from the analysis manager
   if (TestBit(AliAnalysisGrid::kDefaultOutputs)) {
      // Add output files and AOD files
      fOutputFiles = GetListOfFiles("outaod");
      // Add extra files registered to the analysis manager
      TString extra = GetListOfFiles("ext");
      if (!extra.IsNull()) {
         extra.ReplaceAll(".root", "*.root");
         if (!fOutputFiles.IsNull()) fOutputFiles += ",";
         fOutputFiles += extra;
      }
      // Compose the output archive.
      fOutputArchive = "log_archive.zip:std*@disk=1 ";
      fOutputArchive += Form("root_archive.zip:%s@disk=%d",fOutputFiles.Data(),fNreplicas);
   }
//   if (!fCloseSE.Length()) fCloseSE = gSystem->Getenv("alien_CLOSE_SE");
   if (TestBit(AliAnalysisGrid::kOffline)) {
      Info("StartAnalysis","\n##### OFFLINE MODE ##### Files to be used in GRID are produced but not copied \
      \n                         there nor any job run. You can revise the JDL and analysis \
      \n                         macro then run the same in \"submit\" mode.");
   } else if (TestBit(AliAnalysisGrid::kTest)) {
      Info("StartAnalysis","\n##### LOCAL MODE #####   Your analysis will be run locally on a subset of the requested \
      \n                         dataset.");
   } else if (TestBit(AliAnalysisGrid::kSubmit)) {
      Info("StartAnalysis","\n##### SUBMIT MODE #####  Files required by your analysis are copied to your grid working \
      \n                         space and job submitted.");
   } else if (TestBit(AliAnalysisGrid::kMerge)) {
      Info("StartAnalysis","\n##### MERGE MODE #####   The registered outputs of the analysis will be merged");
      if (fMergeViaJDL) CheckInputData();
      return kTRUE;
   } else {
      Info("StartAnalysis","\n##### FULL ANALYSIS MODE ##### Producing needed files and submitting your analysis job...");   
   }   
      
   Print();   
   if (!Connect()) {
      Error("StartAnalysis", "Cannot start grid analysis without grid connection");
      return kFALSE;
   }
   if (IsCheckCopy() && gGrid) CheckFileCopy(gGrid->GetHomeDirectory());
   if (!CheckInputData()) {
      Error("StartAnalysis", "There was an error in preprocessing your requested input data");
      return kFALSE;
   }   
   if (!CreateDataset(fDataPattern)) {
      TString serror;
      if (!fRunNumbers.Length() && !fRunRange[0]) serror = Form("path to data directory: <%s>", fGridDataDir.Data());
      if (fRunNumbers.Length()) serror = "run numbers";
      if (fRunRange[0]) serror = Form("run range [%d, %d]", fRunRange[0], fRunRange[1]);
      serror += Form("\n   or data pattern <%s>", fDataPattern.Data());
      Error("StartAnalysis", "No data to process. Please fix %s in your plugin configuration.", serror.Data());
      return kFALSE;
   }   
   WriteAnalysisFile();   
   WriteAnalysisMacro();
   WriteExecutable();
   WriteValidationScript();
   if (fMergeViaJDL) {
      WriteMergingMacro();
      WriteMergeExecutable();
      WriteValidationScript(kTRUE);
   }   
   if (!CreateJDL()) return kFALSE;
   if (TestBit(AliAnalysisGrid::kOffline)) return kFALSE;
   if (testMode) {
      // Locally testing the analysis
      Info("StartAnalysis", "\n_______________________________________________________________________ \
      \n   Running analysis script in a daughter shell as on a worker node \
      \n_______________________________________________________________________");
      TObjArray *list = fOutputFiles.Tokenize(",");
      TIter next(list);
      TObjString *str;
      TString outputFile;
      while((str=(TObjString*)next())) {
         outputFile = str->GetString();
         Int_t index = outputFile.Index("@");
         if (index > 0) outputFile.Remove(index);         
         if (!gSystem->AccessPathName(outputFile)) gSystem->Exec(Form("rm %s", outputFile.Data()));
      }
      delete list;
      gSystem->Exec(Form("bash %s 2>stderr", fExecutable.Data()));
      gSystem->Exec(Form("bash %s",fValidationScript.Data()));
//      gSystem->Exec("cat stdout");
      return kFALSE;
   }
   // Check if submitting is managed by LPM manager
   if (fProductionMode) {
      TString prodfile = fJDLName;
      prodfile.ReplaceAll(".jdl", ".prod");
      WriteProductionFile(prodfile);
      Info("StartAnalysis", "Job submitting is managed by LPM. Rerun in terminate mode after jobs finished.");
      return kFALSE;
   }   
   // Submit AliEn job(s)
   gGrid->Cd(fGridOutputDir);
   TGridResult *res;
   TString jobID = "";
   if (!fRunNumbers.Length() && !fRunRange[0]) {
      // Submit a given xml or a set of runs
      res = gGrid->Command(Form("submit %s", fJDLName.Data()));
      printf("*************************** %s\n",Form("submit %s", fJDLName.Data()));
      if (res) {
         const char *cjobId = res->GetKey(0,"jobId");
         if (!cjobId) {
            gGrid->Stdout();
            gGrid->Stderr();
            Error("StartAnalysis", "Your JDL %s could not be submitted", fJDLName.Data());
            return kFALSE;
         } else {
            Info("StartAnalysis", "\n_______________________________________________________________________ \
            \n#####   Your JDL %s was successfully submitted. \nTHE JOB ID IS: %s \
            \n_______________________________________________________________________",
                   fJDLName.Data(), cjobId);
            jobID = cjobId;      
         }          
         delete res;
      } else {
         Error("StartAnalysis", "No grid result after submission !!! Bailing out...");
         return kFALSE;      
      }   
   } else {
      // Submit for a range of enumeration of runs.
      if (!Submit()) return kFALSE;
   }   
         
   Info("StartAnalysis", "\n#### STARTING AN ALIEN SHELL FOR YOU. EXIT WHEN YOUR JOB %s HAS FINISHED. #### \
   \n You may exit at any time and terminate the job later using the option <terminate> \
   \n ##################################################################################", jobID.Data());
   gSystem->Exec("aliensh");
   return kTRUE;
}

//______________________________________________________________________________
const char *AliAnalysisAlien::GetListOfFiles(const char *type)
{
// Get a comma-separated list of output files of the requested type.
// Type can be (case unsensitive):
//    aod - list of aod files (std, extensions and filters)
//    out - list of output files connected to containers (but not aod's or extras)
//    ext - list of extra files registered to the manager
//    ter - list of files produced in terminate
   static TString files;
   files = "";
   TString stype = type;
   stype.ToLower();
   TString aodfiles, extra;
   AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
   if (!mgr) {
      ::Error("GetListOfFiles", "Cannot call this without analysis manager");
      return files.Data();
   }
   if (mgr->GetOutputEventHandler()) {
      aodfiles = mgr->GetOutputEventHandler()->GetOutputFileName();
      TString extraaod = mgr->GetOutputEventHandler()->GetExtraOutputs();
      if (!extraaod.IsNull()) {
         aodfiles += ",";
         aodfiles += extraaod;
      }
   }
   if (stype.Contains("aod")) {
      files = aodfiles;
      if (stype == "aod") return files.Data();
   }  
   // Add output files that are not in the list of AOD files 
   TString outputfiles = "";
   TIter next(mgr->GetOutputs());
   AliAnalysisDataContainer *output;
   const char *filename = 0;
   while ((output=(AliAnalysisDataContainer*)next())) {
      filename = output->GetFileName();
      if (!(strcmp(filename, "default"))) continue;
      if (outputfiles.Contains(filename)) continue;
      if (aodfiles.Contains(filename))    continue;
      if (!outputfiles.IsNull()) outputfiles += ",";
      outputfiles += filename;
   }
   if (stype.Contains("out")) {
      if (!files.IsNull()) files += ",";
      files += outputfiles;
      if (stype == "out") return files.Data();
   }   
   // Add extra files registered to the analysis manager
   TString sextra;
   extra = mgr->GetExtraFiles();
   if (!extra.IsNull()) {
      extra.Strip();
      extra.ReplaceAll(" ", ",");
      TObjArray *fextra = extra.Tokenize(",");
      TIter nextx(fextra);
      TObject *obj;
      while ((obj=nextx())) {
         if (aodfiles.Contains(obj->GetName())) continue;
         if (outputfiles.Contains(obj->GetName())) continue;
         if (sextra.Contains(obj->GetName())) continue;
         if (!sextra.IsNull()) sextra += ",";
         sextra += obj->GetName();
      }
      delete fextra;
      if (stype.Contains("ext")) {
         if (!files.IsNull()) files += ",";
         files += sextra;
      }
   }   
   if (stype == "ext") return files.Data();
   TString termfiles;
   if (!fTerminateFiles.IsNull()) {
      fTerminateFiles.Strip();
      fTerminateFiles.ReplaceAll(" ",",");
      TObjArray *fextra = fTerminateFiles.Tokenize(",");
      TIter nextx(fextra);
      TObject *obj;
      while ((obj=nextx())) {
         if (aodfiles.Contains(obj->GetName())) continue;
         if (outputfiles.Contains(obj->GetName())) continue;
         if (termfiles.Contains(obj->GetName())) continue;
         if (sextra.Contains(obj->GetName())) continue;
         if (!termfiles.IsNull()) termfiles += ",";
         termfiles += obj->GetName();
      }
      delete fextra;
   }   
   if (stype.Contains("ter")) {
      if (!files.IsNull() && !termfiles.IsNull()) {
         files += ",";
         files += termfiles;
      }   
   }   
   return files.Data();
}   

//______________________________________________________________________________
Bool_t AliAnalysisAlien::Submit()
{
// Submit all master jobs.
   Int_t nmasterjobs = fInputFiles->GetEntries();
   Long_t tshoot = gSystem->Now();
   if (!fNsubmitted && !SubmitNext()) return kFALSE;
   while (fNsubmitted < nmasterjobs) {
      Long_t now = gSystem->Now();
      if ((now-tshoot)>30000) {
         tshoot = now;
         if (!SubmitNext()) return kFALSE;
      }   
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t AliAnalysisAlien::SubmitMerging()
{
// Submit all merging jobs.
   if (!fGridOutputDir.Contains("/")) fGridOutputDir = Form("/%s/%s/%s", gGrid->GetHomeDirectory(), fGridWorkingDir.Data(), fGridOutputDir.Data());
   gGrid->Cd(fGridOutputDir);
   TString mergeJDLName = fExecutable;
   mergeJDLName.ReplaceAll(".sh", "_merge.jdl");
   Int_t ntosubmit = fInputFiles->GetEntries();
   for (Int_t i=0; i<ntosubmit; i++) {
      TString runOutDir = gSystem->BaseName(fInputFiles->At(i)->GetName());
      runOutDir.ReplaceAll(".xml", "");
      if (fOutputToRunNo) {
         // The output directory is the run number
         printf("### Submitting merging job for run <%s>\n", runOutDir.Data());
         runOutDir = Form("%s/%s", fGridOutputDir.Data(), runOutDir.Data());
      } else {
         // The output directory is the master number in 3 digits format
         printf("### Submitting merging job for master <%03d>\n", i);
         runOutDir = Form("%s/%03d",fGridOutputDir.Data(), i);
      }
      // Check now the number of merging stages.
      TObjArray *list = fOutputFiles.Tokenize(",");
      TIter next(list);
      TObjString *str;
      TString outputFile;
      while((str=(TObjString*)next())) {
         outputFile = str->GetString();
         Int_t index = outputFile.Index("@");
         if (index > 0) outputFile.Remove(index);
         if (!fMergeExcludes.Contains(outputFile)) break;
      }
      delete list;
      Bool_t done = CheckMergedFiles(outputFile, runOutDir, fMaxMergeFiles, kTRUE, mergeJDLName);
      if (!done) return kFALSE;
   }
   if (!ntosubmit) return kTRUE;
   Info("StartAnalysis", "\n#### STARTING AN ALIEN SHELL FOR YOU. EXIT WHEN YOUR MERGING JOBS HAVE FINISHED. #### \
   \n You may exit at any time and terminate the job later using the option <terminate> but disabling SetMergeViaJDL\
   \n ##################################################################################");
   gSystem->Exec("aliensh");
   return kTRUE;
}

//______________________________________________________________________________
Bool_t AliAnalysisAlien::SubmitNext()
{
// Submit next bunch of master jobs if the queue is free. The first master job is
// submitted right away, while the next will not be unless the previous was split.
// The plugin will not submit new master jobs if there are more that 500 jobs in
// waiting phase.
   static Bool_t iscalled = kFALSE;
   static Int_t firstmaster = 0;
   static Int_t lastmaster = 0;
   static Int_t npermaster  = 0;
   if (iscalled) return kTRUE;
   iscalled = kTRUE;
   Int_t nrunning=0, nwaiting=0, nerror=0, ndone=0;
   Int_t ntosubmit = 0;
   TGridResult *res;
   TString jobID = "";
   Int_t nmasterjobs = fInputFiles->GetEntries();
   if (!fNsubmitted) {
      ntosubmit = 1;
      if (!IsUseSubmitPolicy()) {
         if (nmasterjobs>5)
            Info("SubmitNext","### Warning submit policy not used ! Submitting too many jobs at a time may be prohibitted. \
                \n### You can use SetUseSubmitPolicy() to enable if you have problems.");
         ntosubmit = nmasterjobs;
      }   
   } else {
      TString status = GetJobStatus(firstmaster, lastmaster, nrunning, nwaiting, nerror, ndone);
      printf("=== master %d: %s\n", lastmaster, status.Data());
      // If last master not split, just return
      if (status != "SPLIT") {iscalled = kFALSE; return kTRUE;}
      // No more than 100 waiting jobs
      if (nwaiting>500) {iscalled = kFALSE; return kTRUE;}
      npermaster = (nrunning+nwaiting+nerror+ndone)/fNsubmitted;      
      if (npermaster) ntosubmit = (500-nwaiting)/npermaster;
      if (!ntosubmit) ntosubmit = 1;
      printf("=== WAITING(%d) RUNNING(%d) DONE(%d) OTHER(%d) NperMaster=%d => to submit %d jobs\n", 
             nwaiting, nrunning, ndone, nerror, npermaster, ntosubmit);
   }
   for (Int_t i=0; i<ntosubmit; i++) {
      // Submit for a range of enumeration of runs.
      if (fNsubmitted>=nmasterjobs) {iscalled = kFALSE; return kTRUE;}
      TString query;
      TString runOutDir = gSystem->BaseName(fInputFiles->At(fNsubmitted)->GetName());
      runOutDir.ReplaceAll(".xml", "");
      if (fOutputToRunNo)
         query = Form("submit %s %s %s", fJDLName.Data(), fInputFiles->At(fNsubmitted)->GetName(), runOutDir.Data());
      else
         query = Form("submit %s %s %03d", fJDLName.Data(), fInputFiles->At(fNsubmitted)->GetName(), fNsubmitted);
      printf("********* %s\n",query.Data());
      res = gGrid->Command(query);
      if (res) {
         TString cjobId1 = res->GetKey(0,"jobId");
         if (!cjobId1.Length()) {
            iscalled = kFALSE;
            gGrid->Stdout();
            gGrid->Stderr();
            Error("StartAnalysis", "Your JDL %s could not be submitted. The message was:", fJDLName.Data());
            return kFALSE;
         } else {
            Info("StartAnalysis", "\n_______________________________________________________________________ \
            \n#####   Your JDL %s submitted (%d to go). \nTHE JOB ID IS: %s \
            \n_______________________________________________________________________",
                fJDLName.Data(), nmasterjobs-fNsubmitted-1, cjobId1.Data());
            jobID += cjobId1;
            jobID += " ";
            lastmaster = cjobId1.Atoi();
            if (!firstmaster) firstmaster = lastmaster;
            fNsubmitted++;
         }          
         delete res;
      } else {
         Error("StartAnalysis", "No grid result after submission !!! Bailing out...");
         return kFALSE;
      }   
   }
   iscalled = kFALSE;
   return kTRUE;
}

//______________________________________________________________________________
void AliAnalysisAlien::WriteAnalysisFile()
{
// Write current analysis manager into the file <analysisFile>
   TString analysisFile = fExecutable;
   analysisFile.ReplaceAll(".sh", ".root");
   if (!TestBit(AliAnalysisGrid::kSubmit)) {  
      AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
      if (!mgr || !mgr->IsInitialized()) {
         Error("WriteAnalysisFile", "You need an initialized analysis manager for this");
         return;
      }
      // Check analysis type
      TObject *handler;
      if (mgr->GetMCtruthEventHandler()) TObject::SetBit(AliAnalysisGrid::kUseMC);
      handler = (TObject*)mgr->GetInputEventHandler();
      if (handler) {
         if (handler->InheritsFrom("AliESDInputHandler")) TObject::SetBit(AliAnalysisGrid::kUseESD);
         if (handler->InheritsFrom("AliAODInputHandler")) TObject::SetBit(AliAnalysisGrid::kUseAOD);
      }
      TDirectory *cdir = gDirectory;
      TFile *file = TFile::Open(analysisFile, "RECREATE");
      if (file) {
         // Skip task Terminate calls for the grid job (but not in test mode, where we want to check also the terminate mode
         if (!TestBit(AliAnalysisGrid::kTest)) mgr->SetSkipTerminate(kTRUE);
         // Unless merging makes no sense
         if (IsSingleOutput()) mgr->SetSkipTerminate(kFALSE);
         mgr->Write();
         delete file;
         // Enable termination for local jobs
         mgr->SetSkipTerminate(kFALSE);
      }
      if (cdir) cdir->cd();
      Info("WriteAnalysisFile", "\n#####   Analysis manager: %s wrote to file <%s>\n", mgr->GetName(),analysisFile.Data());
   }   
   Bool_t copy = kTRUE;
   if (fProductionMode || TestBit(AliAnalysisGrid::kOffline) || TestBit(AliAnalysisGrid::kTest)) copy = kFALSE;
   if (copy) {
      CdWork();
      TString workdir = gGrid->GetHomeDirectory();
      workdir += fGridWorkingDir;
      Info("WriteAnalysisFile", "\n#####   Copying file <%s> containing your initialized analysis manager to your alien workspace", analysisFile.Data());
      if (FileExists(analysisFile)) gGrid->Rm(analysisFile);
      TFile::Cp(Form("file:%s",analysisFile.Data()), Form("alien://%s/%s", workdir.Data(),analysisFile.Data()));
   }   
}

//______________________________________________________________________________
void AliAnalysisAlien::WriteAnalysisMacro()
{
// Write the analysis macro that will steer the analysis in grid mode.
   if (!TestBit(AliAnalysisGrid::kSubmit)) {  
      ofstream out;
      out.open(fAnalysisMacro.Data(), ios::out);
      if (!out.good()) {
         Error("WriteAnalysisMacro", "could not open file %s for writing", fAnalysisMacro.Data());
         return;
      }
      Bool_t hasSTEERBase = kFALSE;
      Bool_t hasESD = kFALSE;
      Bool_t hasAOD = kFALSE;
      Bool_t hasANALYSIS = kFALSE;
      Bool_t hasANALYSISalice = kFALSE;
      Bool_t hasCORRFW = kFALSE;
      TString func = fAnalysisMacro;
      TString type = "ESD";
      TString comment = "// Analysis using ";
      if (TObject::TestBit(AliAnalysisGrid::kUseESD)) comment += "ESD";
      if (TObject::TestBit(AliAnalysisGrid::kUseAOD)) {
         type = "AOD";
         comment += "AOD";
      }   
      if (type!="AOD" && fFriendChainName!="") {
         Error("WriteAnalysisMacro", "Friend chain can be attached only to AOD");
         return;
      }
      if (TObject::TestBit(AliAnalysisGrid::kUseMC)) comment += "/MC";
      else comment += " data";
      out << "const char *anatype = \"" << type.Data() << "\";" << endl << endl;
      func.ReplaceAll(".C", "");
      out << "void " << func.Data() << "()" << endl; 
      out << "{" << endl;
      out << comment.Data() << endl;
      out << "// Automatically generated analysis steering macro executed in grid subjobs" << endl << endl;
      out << "   TStopwatch timer;" << endl;
      out << "   timer.Start();" << endl << endl;
      // Change temp directory to current one
      out << "// Set temporary merging directory to current one" << endl;
      out << "   gSystem->Setenv(\"TMPDIR\", gSystem->pwd());" << endl << endl;   
      if (!fExecutableCommand.Contains("aliroot")) {
         out << "// load base root libraries" << endl;
         out << "   gSystem->Load(\"libTree\");" << endl;
         out << "   gSystem->Load(\"libGeom\");" << endl;
         out << "   gSystem->Load(\"libVMC\");" << endl;
         out << "   gSystem->Load(\"libPhysics\");" << endl << endl;
         out << "   gSystem->Load(\"libMinuit\");" << endl << endl;
      }   
      if (fAdditionalRootLibs.Length()) {
         // in principle libtree /lib geom libvmc etc. can go into this list, too
         out << "// Add aditional libraries" << endl;
         TObjArray *list = fAdditionalRootLibs.Tokenize(" ");
         TIter next(list);
         TObjString *str;
         while((str=(TObjString*)next())) {
            if (str->GetString().Contains(".so"))
            out << "   gSystem->Load(\"" << str->GetString().Data() << "\");" << endl;
         }
         if (list) delete list;
      }
      out << "// include path" << endl;
      if (fIncludePath.Length()) out << "   gSystem->AddIncludePath(\"" << fIncludePath.Data() << "\");" << endl;
      out << "   gSystem->AddIncludePath(\"-I$ALICE_ROOT/include\");" << endl << endl;
      out << "// Load analysis framework libraries" << endl;
      TString setupPar = "AliAnalysisAlien::SetupPar";
      if (!fPackages) {
         if (!fExecutableCommand.Contains("aliroot")) {         
            out << "   gSystem->Load(\"libSTEERBase\");" << endl;
            out << "   gSystem->Load(\"libESD\");" << endl;
            out << "   gSystem->Load(\"libAOD\");" << endl;
         }   
         out << "   gSystem->Load(\"libANALYSIS\");" << endl;
         out << "   gSystem->Load(\"libANALYSISalice\");" << endl;
         out << "   gSystem->Load(\"libCORRFW\");" << endl << endl;
      } else {
         TIter next(fPackages);
         TObject *obj;
         TString pkgname;
         while ((obj=next())) {
            pkgname = obj->GetName();
            if (pkgname == "STEERBase" ||
                pkgname == "STEERBase.par") hasSTEERBase = kTRUE;
            if (pkgname == "ESD" ||
                pkgname == "ESD.par")       hasESD = kTRUE;
            if (pkgname == "AOD" ||
                pkgname == "AOD.par")       hasAOD = kTRUE;
            if (pkgname == "ANALYSIS" ||
                pkgname == "ANALYSIS.par")  hasANALYSIS = kTRUE;
            if (pkgname == "ANALYSISalice" ||
                pkgname == "ANALYSISalice.par") hasANALYSISalice = kTRUE;
            if (pkgname == "CORRFW" ||
                pkgname == "CORRFW.par")    hasCORRFW = kTRUE;
         }
         if (hasANALYSISalice) setupPar = "SetupPar";   
         if (!hasSTEERBase) out << "   gSystem->Load(\"libSTEERBase\");" << endl;
         else out << "   if (!" << setupPar << "(\"STEERBase\")) return;" << endl;
         if (!hasESD)       out << "   gSystem->Load(\"libESD\");" << endl;
         else out << "   if (!" << setupPar << "(\"ESD\")) return;" << endl;
         if (!hasAOD)       out << "   gSystem->Load(\"libAOD\");" << endl;
         else out << "   if (!" << setupPar << "(\"AOD\")) return;" << endl;
         if (!hasANALYSIS)  out << "   gSystem->Load(\"libANALYSIS\");" << endl;
         else out << "   if (!" << setupPar << "(\"ANALYSIS\")) return;" << endl;
         if (!hasANALYSISalice)   out << "   gSystem->Load(\"libANALYSISalice\");" << endl;
         else out << "   if (!" << setupPar << "(\"ANALYSISalice\")) return;" << endl;
         if (!hasCORRFW)    out << "   gSystem->Load(\"libCORRFW\");" << endl << endl;
         else out << "   if (!" << setupPar << "(\"CORRFW\")) return;" << endl << endl;
         out << "// Compile other par packages" << endl;
         next.Reset();
         while ((obj=next())) {
            pkgname = obj->GetName();
            if (pkgname == "STEERBase" ||
                pkgname == "STEERBase.par" ||
                pkgname == "ESD" ||
                pkgname == "ESD.par" ||
                pkgname == "AOD" ||
                pkgname == "AOD.par" ||
                pkgname == "ANALYSIS" ||
                pkgname == "ANALYSIS.par" ||
                pkgname == "ANALYSISalice" ||
                pkgname == "ANALYSISalice.par" ||
                pkgname == "CORRFW" ||
                pkgname == "CORRFW.par") continue;
            out << "   if (!" << setupPar << "(\"" << obj->GetName() << "\")) return;" << endl;
         }   
      }   
      if (fAdditionalLibs.Length()) {
         out << "// Add aditional AliRoot libraries" << endl;
         TObjArray *list = fAdditionalLibs.Tokenize(" ");
         TIter next(list);
         TObjString *str;
         while((str=(TObjString*)next())) {
            if (str->GetString().Contains(".so"))
               out << "   gSystem->Load(\"" << str->GetString().Data() << "\");" << endl;
            if (str->GetString().Contains(".par"))
               out << "   if (!" << setupPar << "(\"" << str->GetString() << "\")) return;" << endl;
         }
         if (list) delete list;
      }
      out << endl;
      out << "// analysis source to be compiled at runtime (if any)" << endl;
      if (fAnalysisSource.Length()) {
         TObjArray *list = fAnalysisSource.Tokenize(" ");
         TIter next(list);
         TObjString *str;
         while((str=(TObjString*)next())) {
            out << "   gROOT->ProcessLine(\".L " << str->GetString().Data() << "+g\");" << endl;
         }   
         if (list) delete list;
      }
      out << endl;
      if (fFastReadOption) {
         Warning("WriteAnalysisMacro", "!!! You requested FastRead option. Using xrootd flags to reduce timeouts in the grid jobs. This may skip some files that could be accessed !!! \
                \n+++ NOTE: To disable this option, use: plugin->SetFastReadOption(kFALSE)");
         out << "// fast xrootd reading enabled" << endl;
         out << "   printf(\"!!! You requested FastRead option. Using xrootd flags to reduce timeouts. Note that this may skip some files that could be accessed !!!\");" << endl;
         out << "   gEnv->SetValue(\"XNet.ConnectTimeout\",10);" << endl;
         out << "   gEnv->SetValue(\"XNet.RequestTimeout\",10);" << endl;
         out << "   gEnv->SetValue(\"XNet.MaxRedirectCount\",2);" << endl;
         out << "   gEnv->SetValue(\"XNet.ReconnectTimeout\",10);" << endl;
         out << "   gEnv->SetValue(\"XNet.FirstConnectMaxCnt\",1);" << endl << endl;
      }   
      out << "// connect to AliEn and make the chain" << endl;
      out << "   if (!TGrid::Connect(\"alien://\")) return;" << endl;
      out << "// read the analysis manager from file" << endl;
      TString analysisFile = fExecutable;
      analysisFile.ReplaceAll(".sh", ".root");
      out << "   TFile *file = TFile::Open(\"" << analysisFile << "\");" << endl;
      out << "   if (!file) return;" << endl; 
      out << "   TIter nextkey(file->GetListOfKeys());" << endl;
      out << "   AliAnalysisManager *mgr = 0;" << endl;
      out << "   TKey *key;" << endl;
      out << "   while ((key=(TKey*)nextkey())) {" << endl;
      out << "      if (!strcmp(key->GetClassName(), \"AliAnalysisManager\"))" << endl;
      out << "         mgr = (AliAnalysisManager*)file->Get(key->GetName());" << endl;
      out << "   };" << endl;
      out << "   if (!mgr) {" << endl;
      out << "      ::Error(\"" << func.Data() << "\", \"No analysis manager found in file " << analysisFile <<"\");" << endl;
      out << "      return;" << endl;
      out << "   }" << endl << endl;
      out << "   mgr->PrintStatus();" << endl;
      if (AliAnalysisManager::GetAnalysisManager()) {
         if (AliAnalysisManager::GetAnalysisManager()->GetDebugLevel()>3) {
            out << "   gEnv->SetValue(\"XNet.Debug\", \"1\");" << endl;
         } else {
            if (TestBit(AliAnalysisGrid::kTest))            
               out << "   AliLog::SetGlobalLogLevel(AliLog::kWarning);" << endl;
            else
               out << "   AliLog::SetGlobalLogLevel(AliLog::kError);" << endl;
         }
      }   
      if (IsUsingTags()) {
         out << "   TChain *chain = CreateChainFromTags(\"wn.xml\", anatype);" << endl << endl;
      } else {
         out << "   TChain *chain = CreateChain(\"wn.xml\", anatype);" << endl << endl;   
      }   
      out << "   mgr->StartAnalysis(\"localfile\", chain);" << endl;
      out << "   timer.Stop();" << endl;
      out << "   timer.Print();" << endl;
      out << "}" << endl << endl;
      if (IsUsingTags()) {
         out << "TChain* CreateChainFromTags(const char *xmlfile, const char *type=\"ESD\")" << endl;
         out << "{" << endl;
         out << "// Create a chain using tags from the xml file." << endl;
         out << "   TAlienCollection* coll = TAlienCollection::Open(xmlfile);" << endl;
         out << "   if (!coll) {" << endl;
         out << "      ::Error(\"CreateChainFromTags\", \"Cannot create an AliEn collection from %s\", xmlfile);" << endl;
         out << "      return NULL;" << endl;
         out << "   }" << endl;
         out << "   TGridResult* tagResult = coll->GetGridResult(\"\",kFALSE,kFALSE);" << endl;
         out << "   AliTagAnalysis *tagAna = new AliTagAnalysis(type);" << endl;
         out << "   tagAna->ChainGridTags(tagResult);" << endl << endl;
         out << "   AliRunTagCuts      *runCuts = new AliRunTagCuts();" << endl;
         out << "   AliLHCTagCuts      *lhcCuts = new AliLHCTagCuts();" << endl;
         out << "   AliDetectorTagCuts *detCuts = new AliDetectorTagCuts();" << endl;
         out << "   AliEventTagCuts    *evCuts  = new AliEventTagCuts();" << endl;
         out << "   // Check if the cuts configuration file was provided" << endl;
         out << "   if (!gSystem->AccessPathName(\"ConfigureCuts.C\")) {" << endl;
         out << "      gROOT->LoadMacro(\"ConfigureCuts.C\");" << endl;
         out << "      ConfigureCuts(runCuts, lhcCuts, detCuts, evCuts);" << endl;
         out << "   }" << endl;
         if (fFriendChainName=="") {
            out << "   TChain *chain = tagAna->QueryTags(runCuts, lhcCuts, detCuts, evCuts);" << endl;
         } else {
            out << "   TString tmpColl=\"tmpCollection.xml\";" << endl;
            out << "   tagAna->CreateXMLCollection(tmpColl.Data(),runCuts, lhcCuts, detCuts, evCuts);" << endl;
            out << "   TChain *chain = CreateChain(tmpColl.Data(),type);" << endl;
         }
         out << "   if (!chain || !chain->GetNtrees()) return NULL;" << endl;
         out << "   chain->ls();" << endl;
         out << "   return chain;" << endl;
         out << "}" << endl << endl;
         if (gSystem->AccessPathName("ConfigureCuts.C")) {
            TString msg = "\n#####   You may want to provide a macro ConfigureCuts.C with a method:\n";
            msg += "   void ConfigureCuts(AliRunTagCuts *runCuts,\n";
            msg += "                      AliLHCTagCuts *lhcCuts,\n";
            msg += "                      AliDetectorTagCuts *detCuts,\n";
            msg += "                      AliEventTagCuts *evCuts)";
            Info("WriteAnalysisMacro", "%s", msg.Data());
         }
      } 
      if (!IsUsingTags() || fFriendChainName!="") {
         out <<"//________________________________________________________________________________" << endl;
         out << "TChain* CreateChain(const char *xmlfile, const char *type=\"ESD\")" << endl;
         out << "{" << endl;
         out << "// Create a chain using url's from xml file" << endl;
         out << "   TString filename;" << endl;
         out << "   Int_t run = 0;" << endl;
         out << "   TString treename = type;" << endl;
         out << "   treename.ToLower();" << endl;
         out << "   treename += \"Tree\";" << endl;
         out << "   printf(\"***************************************\\n\");" << endl;
         out << "   printf(\"    Getting chain of trees %s\\n\", treename.Data());" << endl;
         out << "   printf(\"***************************************\\n\");" << endl;
         out << "   TAlienCollection *coll = TAlienCollection::Open(xmlfile);" << endl;
         out << "   if (!coll) {" << endl;
         out << "      ::Error(\"CreateChain\", \"Cannot create an AliEn collection from %s\", xmlfile);" << endl;
         out << "      return NULL;" << endl;
         out << "   }" << endl;
         out << "   AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();" << endl;
         out << "   TChain *chain = new TChain(treename);" << endl;
         if(fFriendChainName!="") {
            out << "   TChain *chainFriend = new TChain(treename);" << endl;
         }
         out << "   coll->Reset();" << endl;
         out << "   while (coll->Next()) {" << endl;
         out << "      filename = coll->GetTURL("");" << endl;
         out << "      if (mgr) {" << endl;
         out << "         Int_t nrun = AliAnalysisManager::GetRunFromAlienPath(filename);" << endl;
         out << "         if (nrun && nrun != run) {" << endl;
         out << "            printf(\"### Run number detected from chain: %d\\n\", nrun);" << endl;
         out << "            mgr->SetRunFromPath(nrun);" << endl;
         out << "            run = nrun;" << endl;
         out << "         }" << endl;
         out << "      }" << endl;
         out << "      chain->Add(filename);" << endl;
         if(fFriendChainName!="") {
            out << "      TString fileFriend=coll->GetTURL(\"\");" << endl;
            out << "      fileFriend.ReplaceAll(\"AliAOD.root\",\""<<fFriendChainName.Data()<<"\");" << endl;
            out << "      fileFriend.ReplaceAll(\"AliAODs.root\",\""<<fFriendChainName.Data()<<"\");" << endl;
            out << "      chainFriend->Add(fileFriend.Data());" << endl;
         }
         out << "   }" << endl;
         out << "   if (!chain->GetNtrees()) {" << endl;
         out << "      ::Error(\"CreateChain\", \"No tree found from collection %s\", xmlfile);" << endl;
         out << "      return NULL;" << endl;
         out << "   }" << endl;
         if(fFriendChainName!="") {
            out << "   chain->AddFriend(chainFriend);" << endl;
         }
         out << "   return chain;" << endl;
         out << "}" << endl << endl;
      }   
      if (hasANALYSISalice) {
         out <<"//________________________________________________________________________________" << endl;
         out << "Bool_t SetupPar(const char *package) {" << endl;
         out << "// Compile the package and set it up." << endl;
         out << "   TString pkgdir = package;" << endl;
         out << "   pkgdir.ReplaceAll(\".par\",\"\");" << endl;
         out << "   gSystem->Exec(Form(\"tar xvzf %s.par\", pkgdir.Data()));" << endl;
         out << "   TString cdir = gSystem->WorkingDirectory();" << endl;
         out << "   gSystem->ChangeDirectory(pkgdir);" << endl;
         out << "   // Check for BUILD.sh and execute" << endl;
         out << "   if (!gSystem->AccessPathName(\"PROOF-INF/BUILD.sh\")) {" << endl;
         out << "      printf(\"*******************************\\n\");" << endl;
         out << "      printf(\"*** Building PAR archive    ***\\n\");" << endl;
         out << "      printf(\"*******************************\\n\");" << endl;
         out << "      if (gSystem->Exec(\"PROOF-INF/BUILD.sh\")) {" << endl;
         out << "         ::Error(\"SetupPar\", \"Cannot build par archive %s\", pkgdir.Data());" << endl;
         out << "         gSystem->ChangeDirectory(cdir);" << endl;
         out << "         return kFALSE;" << endl;
         out << "      }" << endl;
         out << "   } else {" << endl;
         out << "      ::Error(\"SetupPar\",\"Cannot access PROOF-INF/BUILD.sh for package %s\", pkgdir.Data());" << endl;
         out << "      gSystem->ChangeDirectory(cdir);" << endl;
         out << "      return kFALSE;" << endl;
         out << "   }" << endl;
         out << "   // Check for SETUP.C and execute" << endl;
         out << "   if (!gSystem->AccessPathName(\"PROOF-INF/SETUP.C\")) {" << endl;
         out << "      printf(\"*******************************\\n\");" << endl;
         out << "      printf(\"***    Setup PAR archive    ***\\n\");" << endl;
         out << "      printf(\"*******************************\\n\");" << endl;
         out << "      gROOT->Macro(\"PROOF-INF/SETUP.C\");" << endl;
         out << "   } else {" << endl;
         out << "      ::Error(\"SetupPar\",\"Cannot access PROOF-INF/SETUP.C for package %s\", pkgdir.Data());" << endl;
         out << "      gSystem->ChangeDirectory(cdir);" << endl;
         out << "      return kFALSE;" << endl;
         out << "   }" << endl;
         out << "   // Restore original workdir" << endl;
         out << "   gSystem->ChangeDirectory(cdir);" << endl;
         out << "   return kTRUE;" << endl;
         out << "}" << endl;
      }
      Info("WriteAnalysisMacro", "\n#####   Analysis macro to run on worker nodes <%s> written",fAnalysisMacro.Data());
   }   
   Bool_t copy = kTRUE;
   if (fProductionMode || TestBit(AliAnalysisGrid::kOffline) || TestBit(AliAnalysisGrid::kTest)) copy = kFALSE;
   if (copy) {
      CdWork();
      TString workdir = gGrid->GetHomeDirectory();
      workdir += fGridWorkingDir;
      if (FileExists(fAnalysisMacro)) gGrid->Rm(fAnalysisMacro);
      if (IsUsingTags() && !gSystem->AccessPathName("ConfigureCuts.C")) {
         if (FileExists("ConfigureCuts.C")) gGrid->Rm("ConfigureCuts.C");
         Info("WriteAnalysisMacro", "\n#####   Copying cuts configuration macro: <ConfigureCuts.C> to your alien workspace");
         TFile::Cp("file:ConfigureCuts.C", Form("alien://%s/ConfigureCuts.C", workdir.Data()));
      }   
      Info("WriteAnalysisMacro", "\n#####   Copying analysis macro: <%s> to your alien workspace", fAnalysisMacro.Data());
      TFile::Cp(Form("file:%s",fAnalysisMacro.Data()), Form("alien://%s/%s", workdir.Data(), fAnalysisMacro.Data()));
   }
}

//______________________________________________________________________________
void AliAnalysisAlien::WriteMergingMacro()
{
// Write a macro to merge the outputs per master job.
   if (!fMergeViaJDL) return;
   if (!fOutputFiles.Length()) {
      Error("WriteMergingMacro", "No output file names defined. Are you running the right AliAnalysisAlien configuration ?");
      return;
   }   
   TString mergingMacro = fExecutable;
   mergingMacro.ReplaceAll(".sh","_merge.C");
   if (!fGridOutputDir.Contains("/")) fGridOutputDir = Form("/%s/%s/%s", gGrid->GetHomeDirectory(), fGridWorkingDir.Data(), fGridOutputDir.Data());
   if (!TestBit(AliAnalysisGrid::kSubmit)) {  
      ofstream out;
      out.open(mergingMacro.Data(), ios::out);
      if (!out.good()) {
         Error("WriteMergingMacro", "could not open file %s for writing", fAnalysisMacro.Data());
         return;
      }
      Bool_t hasSTEERBase = kFALSE;
      Bool_t hasESD = kFALSE;
      Bool_t hasAOD = kFALSE;
      Bool_t hasANALYSIS = kFALSE;
      Bool_t hasANALYSISalice = kFALSE;
      Bool_t hasCORRFW = kFALSE;
      TString func = mergingMacro;
      TString comment;
      func.ReplaceAll(".C", "");
      out << "void " << func.Data() << "(const char *dir, Int_t stage=0, Int_t ichunk=0)" << endl;
      out << "{" << endl;
      out << "// Automatically generated merging macro executed in grid subjobs" << endl << endl;
      out << "   TStopwatch timer;" << endl;
      out << "   timer.Start();" << endl << endl;
      if (!fExecutableCommand.Contains("aliroot")) {
         out << "// load base root libraries" << endl;
         out << "   gSystem->Load(\"libTree\");" << endl;
         out << "   gSystem->Load(\"libGeom\");" << endl;
         out << "   gSystem->Load(\"libVMC\");" << endl;
         out << "   gSystem->Load(\"libPhysics\");" << endl << endl;
         out << "   gSystem->Load(\"libMinuit\");" << endl << endl;
      }   
      if (fAdditionalRootLibs.Length()) {
         // in principle libtree /lib geom libvmc etc. can go into this list, too
         out << "// Add aditional libraries" << endl;
         TObjArray *list = fAdditionalRootLibs.Tokenize(" ");
         TIter next(list);
         TObjString *str;
         while((str=(TObjString*)next())) {
            if (str->GetString().Contains(".so"))
            out << "   gSystem->Load(\"" << str->GetString().Data() << "\");" << endl;
         }
         if (list) delete list;
      }
      out << "// include path" << endl;
      if (fIncludePath.Length()) out << "   gSystem->AddIncludePath(\"" << fIncludePath.Data() << "\");" << endl;
      out << "   gSystem->AddIncludePath(\"-I$ALICE_ROOT/include\");" << endl << endl;
      out << "// Load analysis framework libraries" << endl;
      if (!fPackages) {
         if (!fExecutableCommand.Contains("aliroot")) {
            out << "   gSystem->Load(\"libSTEERBase\");" << endl;
            out << "   gSystem->Load(\"libESD\");" << endl;
            out << "   gSystem->Load(\"libAOD\");" << endl;
         }
         out << "   gSystem->Load(\"libANALYSIS\");" << endl;
         out << "   gSystem->Load(\"libANALYSISalice\");" << endl;
         out << "   gSystem->Load(\"libCORRFW\");" << endl << endl;
      } else {
         TIter next(fPackages);
         TObject *obj;
         TString pkgname;
         TString setupPar = "AliAnalysisAlien::SetupPar";
         while ((obj=next())) {
            pkgname = obj->GetName();
            if (pkgname == "STEERBase" ||
                pkgname == "STEERBase.par") hasSTEERBase = kTRUE;
            if (pkgname == "ESD" ||
                pkgname == "ESD.par")       hasESD = kTRUE;
            if (pkgname == "AOD" ||
                pkgname == "AOD.par")       hasAOD = kTRUE;
            if (pkgname == "ANALYSIS" ||
                pkgname == "ANALYSIS.par")  hasANALYSIS = kTRUE;
            if (pkgname == "ANALYSISalice" ||
                pkgname == "ANALYSISalice.par") hasANALYSISalice = kTRUE;
            if (pkgname == "CORRFW" ||
                pkgname == "CORRFW.par")    hasCORRFW = kTRUE;
         }   
         if (hasANALYSISalice) setupPar = "SetupPar";   
         if (!hasSTEERBase) out << "   gSystem->Load(\"libSTEERBase\");" << endl;
         else out << "   if (!" << setupPar << "(\"STEERBase\")) return;" << endl;
         if (!hasESD)       out << "   gSystem->Load(\"libESD\");" << endl;
         else out << "   if (!" << setupPar << "(\"ESD\")) return;" << endl;
         if (!hasAOD)       out << "   gSystem->Load(\"libAOD\");" << endl;
         else out << "   if (!" << setupPar << "(\"AOD\")) return;" << endl;
         if (!hasANALYSIS)  out << "   gSystem->Load(\"libANALYSIS\");" << endl;
         else out << "   if (!" << setupPar << "(\"ANALYSIS\")) return;" << endl;
         if (!hasANALYSISalice)   out << "   gSystem->Load(\"libANALYSISalice\");" << endl;
         else out << "   if (!" << setupPar << "(\"ANALYSISalice\")) return;" << endl;
         if (!hasCORRFW)    out << "   gSystem->Load(\"libCORRFW\");" << endl << endl;
         else out << "   if (!" << setupPar << "(\"CORRFW\")) return;" << endl << endl;
         out << "// Compile other par packages" << endl;
         next.Reset();
         while ((obj=next())) {
            pkgname = obj->GetName();
            if (pkgname == "STEERBase" ||
                pkgname == "STEERBase.par" ||
                pkgname == "ESD" ||
                pkgname == "ESD.par" ||
                pkgname == "AOD" ||
                pkgname == "AOD.par" ||
                pkgname == "ANALYSIS" ||
                pkgname == "ANALYSIS.par" ||
                pkgname == "ANALYSISalice" ||
                pkgname == "ANALYSISalice.par" ||
                pkgname == "CORRFW" ||
                pkgname == "CORRFW.par") continue;
            out << "   if (!" << setupPar << "(\"" << obj->GetName() << "\")) return;" << endl;
         }   
      }   
      if (fAdditionalLibs.Length()) {
         out << "// Add aditional AliRoot libraries" << endl;
         TObjArray *list = fAdditionalLibs.Tokenize(" ");
         TIter next(list);
         TObjString *str;
         while((str=(TObjString*)next())) {
            if (str->GetString().Contains(".so"))
               out << "   gSystem->Load(\"" << str->GetString().Data() << "\");" << endl;
         }
         if (list) delete list;
      }
      out << endl;
      out << "// Analysis source to be compiled at runtime (if any)" << endl;
      if (fAnalysisSource.Length()) {
         TObjArray *list = fAnalysisSource.Tokenize(" ");
         TIter next(list);
         TObjString *str;
         while((str=(TObjString*)next())) {
            out << "   gROOT->ProcessLine(\".L " << str->GetString().Data() << "+g\");" << endl;
         }   
         if (list) delete list;
      }
      out << endl;      

      if (fFastReadOption) {
         Warning("WriteMergingMacro", "!!! You requested FastRead option. Using xrootd flags to reduce timeouts in the grid merging jobs. Note that this may skip some files that could be accessed !!!");
         out << "// fast xrootd reading enabled" << endl;
         out << "   printf(\"!!! You requested FastRead option. Using xrootd flags to reduce timeouts. Note that this may skip some files that could be accessed !!!\");" << endl;
         out << "   gEnv->SetValue(\"XNet.ConnectTimeout\",10);" << endl;
         out << "   gEnv->SetValue(\"XNet.RequestTimeout\",10);" << endl;
         out << "   gEnv->SetValue(\"XNet.MaxRedirectCount\",2);" << endl;
         out << "   gEnv->SetValue(\"XNet.ReconnectTimeout\",10);" << endl;
         out << "   gEnv->SetValue(\"XNet.FirstConnectMaxCnt\",1);" << endl << endl;
      }
      // Change temp directory to current one
      out << "// Set temporary merging directory to current one" << endl;
      out << "   gSystem->Setenv(\"TMPDIR\", gSystem->pwd());" << endl << endl;   
      out << "// Connect to AliEn" << endl;
      out << "   if (!TGrid::Connect(\"alien://\")) return;" << endl;
      out << "   Bool_t laststage = kFALSE;" << endl;
      out << "   TString outputDir = dir;" << endl;  
      out << "   TString outputFiles = \"" << GetListOfFiles("out") << "\";" << endl;
      out << "   TString mergeExcludes = \"" << fMergeExcludes << "\";" << endl;
      out << "   TObjArray *list = outputFiles.Tokenize(\",\");" << endl;
      out << "   TIter *iter = new TIter(list);" << endl;
      out << "   TObjString *str;" << endl;
      out << "   TString outputFile;" << endl;
      out << "   Bool_t merged = kTRUE;" << endl;
      out << "   while((str=(TObjString*)iter->Next())) {" << endl;
      out << "      outputFile = str->GetString();" << endl;
      out << "      if (outputFile.Contains(\"*\")) continue;" << endl;
      out << "      Int_t index = outputFile.Index(\"@\");" << endl;
      out << "      if (index > 0) outputFile.Remove(index);" << endl;
      out << "      // Skip already merged outputs" << endl;
      out << "      if (!gSystem->AccessPathName(outputFile)) {" << endl;
      out << "         printf(\"Output file <%s> found. Not merging again.\",outputFile.Data());" << endl;
      out << "         continue;" << endl;
      out << "      }" << endl;
      out << "      if (mergeExcludes.Contains(outputFile.Data())) continue;" << endl;
      out << "      merged = AliAnalysisAlien::MergeOutput(outputFile, outputDir, " << fMaxMergeFiles << ", stage, ichunk);" << endl;
      out << "      if (!merged) {" << endl;
      out << "         printf(\"ERROR: Cannot merge %s\\n\", outputFile.Data());" << endl;
      out << "         return;" << endl;
      out << "      }" << endl;
      out << "      // Check if this was the last stage. If yes, run terminate for the tasks." << endl;
      out << "      if (!gSystem->AccessPathName(outputFile)) laststage = kTRUE;" << endl;
      out << "   }" << endl;
      out << "   // all outputs merged, validate" << endl;
      out << "   ofstream out;" << endl;
      out << "   out.open(\"outputs_valid\", ios::out);" << endl;
      out << "   out.close();" << endl;
      out << "   // read the analysis manager from file" << endl;
      TString analysisFile = fExecutable;
      analysisFile.ReplaceAll(".sh", ".root");
      out << "   if (!laststage) return;" << endl;
      out << "   TFile *file = TFile::Open(\"" << analysisFile << "\");" << endl;
      out << "   if (!file) return;" << endl;
      out << "   TIter nextkey(file->GetListOfKeys());" << endl;
      out << "   AliAnalysisManager *mgr = 0;" << endl;
      out << "   TKey *key;" << endl;
      out << "   while ((key=(TKey*)nextkey())) {" << endl;
      out << "      if (!strcmp(key->GetClassName(), \"AliAnalysisManager\"))" << endl;
      out << "         mgr = (AliAnalysisManager*)file->Get(key->GetName());" << endl;
      out << "   };" << endl;
      out << "   if (!mgr) {" << endl;
      out << "      ::Error(\"" << func.Data() << "\", \"No analysis manager found in file" << analysisFile <<"\");" << endl;
      out << "      return;" << endl;
      out << "   }" << endl << endl;
      out << "   mgr->SetSkipTerminate(kFALSE);" << endl;
      out << "   mgr->PrintStatus();" << endl;
      if (AliAnalysisManager::GetAnalysisManager()) {
         if (AliAnalysisManager::GetAnalysisManager()->GetDebugLevel()>3) {
            out << "   gEnv->SetValue(\"XNet.Debug\", \"1\");" << endl;
         } else {
            if (TestBit(AliAnalysisGrid::kTest))            
               out << "   AliLog::SetGlobalLogLevel(AliLog::kWarning);" << endl;
            else
               out << "   AliLog::SetGlobalLogLevel(AliLog::kError);" << endl;
         }
      }   
      out << "   TTree *tree = NULL;" << endl;
      out << "   mgr->StartAnalysis(\"gridterminate\", tree);" << endl;
      out << "}" << endl << endl;
      if (hasANALYSISalice) {
         out <<"//________________________________________________________________________________" << endl;
         out << "Bool_t SetupPar(const char *package) {" << endl;
         out << "// Compile the package and set it up." << endl;
         out << "   TString pkgdir = package;" << endl;
         out << "   pkgdir.ReplaceAll(\".par\",\"\");" << endl;
         out << "   gSystem->Exec(Form(\"tar xvzf %s.par\", pkgdir.Data()));" << endl;
         out << "   TString cdir = gSystem->WorkingDirectory();" << endl;
         out << "   gSystem->ChangeDirectory(pkgdir);" << endl;
         out << "   // Check for BUILD.sh and execute" << endl;
         out << "   if (!gSystem->AccessPathName(\"PROOF-INF/BUILD.sh\")) {" << endl;
         out << "      printf(\"*******************************\\n\");" << endl;
         out << "      printf(\"*** Building PAR archive    ***\\n\");" << endl;
         out << "      printf(\"*******************************\\n\");" << endl;
         out << "      if (gSystem->Exec(\"PROOF-INF/BUILD.sh\")) {" << endl;
         out << "         ::Error(\"SetupPar\", \"Cannot build par archive %s\", pkgdir.Data());" << endl;
         out << "         gSystem->ChangeDirectory(cdir);" << endl;
         out << "         return kFALSE;" << endl;
         out << "      }" << endl;
         out << "   } else {" << endl;
         out << "      ::Error(\"SetupPar\",\"Cannot access PROOF-INF/BUILD.sh for package %s\", pkgdir.Data());" << endl;
         out << "      gSystem->ChangeDirectory(cdir);" << endl;
         out << "      return kFALSE;" << endl;
         out << "   }" << endl;
         out << "   // Check for SETUP.C and execute" << endl;
         out << "   if (!gSystem->AccessPathName(\"PROOF-INF/SETUP.C\")) {" << endl;
         out << "      printf(\"*******************************\\n\");" << endl;
         out << "      printf(\"***    Setup PAR archive    ***\\n\");" << endl;
         out << "      printf(\"*******************************\\n\");" << endl;
         out << "      gROOT->Macro(\"PROOF-INF/SETUP.C\");" << endl;
         out << "   } else {" << endl;
         out << "      ::Error(\"SetupPar\",\"Cannot access PROOF-INF/SETUP.C for package %s\", pkgdir.Data());" << endl;
         out << "      gSystem->ChangeDirectory(cdir);" << endl;
         out << "      return kFALSE;" << endl;
         out << "   }" << endl;
         out << "   // Restore original workdir" << endl;
         out << "   gSystem->ChangeDirectory(cdir);" << endl;
         out << "   return kTRUE;" << endl;
         out << "}" << endl;
      }
   }   
   Bool_t copy = kTRUE;
   if (fProductionMode || TestBit(AliAnalysisGrid::kOffline) || TestBit(AliAnalysisGrid::kTest)) copy = kFALSE;
   if (copy) {
      CdWork();
      TString workdir = gGrid->GetHomeDirectory();
      workdir += fGridWorkingDir;
      if (FileExists(mergingMacro)) gGrid->Rm(mergingMacro);
      Info("WriteMergingMacro", "\n#####   Copying merging macro: <%s> to your alien workspace", mergingMacro.Data());
      TFile::Cp(Form("file:%s",mergingMacro.Data()), Form("alien://%s/%s", workdir.Data(), mergingMacro.Data()));
   }
}

//______________________________________________________________________________
Bool_t AliAnalysisAlien::SetupPar(const char *package)
{
// Compile the par file archive pointed by <package>. This must be present in the current directory.
// Note that for loading the compiled library. The current directory should have precedence in
// LD_LIBRARY_PATH
   TString pkgdir = package;
   pkgdir.ReplaceAll(".par","");
   gSystem->Exec(Form("tar xvzf %s.par", pkgdir.Data()));
   TString cdir = gSystem->WorkingDirectory();
   gSystem->ChangeDirectory(pkgdir);
   // Check for BUILD.sh and execute
   if (!gSystem->AccessPathName("PROOF-INF/BUILD.sh")) {
      printf("**************************************************\n");
      printf("*** Building PAR archive %s\n", package);
      printf("**************************************************\n");
      if (gSystem->Exec("PROOF-INF/BUILD.sh")) {
         ::Error("SetupPar", "Cannot build par archive %s", pkgdir.Data());
         gSystem->ChangeDirectory(cdir);
         return kFALSE;
      }
   } else {
      ::Error("SetupPar","Cannot access PROOF-INF/BUILD.sh for package %s", pkgdir.Data());
      gSystem->ChangeDirectory(cdir);
      return kFALSE;
   }
   // Check for SETUP.C and execute
   if (!gSystem->AccessPathName("PROOF-INF/SETUP.C")) {
      printf("**************************************************\n");
      printf("*** Setup PAR archive %s\n", package);
      printf("**************************************************\n");
      gROOT->Macro("PROOF-INF/SETUP.C");
      printf("*** Loaded library: %s\n", gSystem->GetLibraries(pkgdir,"",kFALSE));
   } else {
      ::Error("SetupPar","Cannot access PROOF-INF/SETUP.C for package %s", pkgdir.Data());
      gSystem->ChangeDirectory(cdir);
      return kFALSE;
   }   
   // Restore original workdir
   gSystem->ChangeDirectory(cdir);
   return kTRUE;
}

//______________________________________________________________________________
void AliAnalysisAlien::WriteExecutable()
{
// Generate the alien executable script.
   if (!TestBit(AliAnalysisGrid::kSubmit)) {  
      ofstream out;
      out.open(fExecutable.Data(), ios::out);
      if (out.bad()) {
         Error("WriteExecutable", "Bad file name for executable: %s", fExecutable.Data());
         return;
      }
      out << "#!/bin/bash" << endl;
      out << "echo \"=========================================\"" << endl; 
      out << "echo \"############## PATH : ##############\"" << endl;
      out << "echo $PATH" << endl;
      out << "echo \"############## LD_LIBRARY_PATH : ##############\"" << endl;
      out << "echo $LD_LIBRARY_PATH" << endl;
      out << "echo \"############## ROOTSYS : ##############\"" << endl;
      out << "echo $ROOTSYS" << endl;
      out << "echo \"############## which root : ##############\"" << endl;
      out << "which root" << endl;
      out << "echo \"############## ALICE_ROOT : ##############\"" << endl;
      out << "echo $ALICE_ROOT" << endl;
      out << "echo \"############## which aliroot : ##############\"" << endl;
      out << "which aliroot" << endl;
      out << "echo \"############## system limits : ##############\"" << endl;
      out << "ulimit -a" << endl;
      out << "echo \"############## memory : ##############\"" << endl;
      out << "free -m" << endl;
      out << "echo \"=========================================\"" << endl << endl;
      // Make sure we can properly compile par files
      if (TObject::TestBit(AliAnalysisGrid::kUsePars)) out << "export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH" << endl;
      out << fExecutableCommand << " "; 
      out << fAnalysisMacro.Data() << " " << fExecutableArgs.Data() << endl << endl;
      out << "echo \"======== " << fAnalysisMacro.Data() << " finished with exit code: $? ========\"" << endl;
      out << "echo \"############## memory after: ##############\"" << endl;
      out << "free -m" << endl;
   }   
   Bool_t copy = kTRUE;
   if (fProductionMode || TestBit(AliAnalysisGrid::kOffline) || TestBit(AliAnalysisGrid::kTest)) copy = kFALSE;
   if (copy) {
      CdWork();
      TString workdir = gGrid->GetHomeDirectory();
      TString bindir = Form("%s/bin", workdir.Data());
      if (!DirectoryExists(bindir)) gGrid->Mkdir(bindir,"-p");
      workdir += fGridWorkingDir;
      TString executable = Form("%s/bin/%s", gGrid->GetHomeDirectory(), fExecutable.Data());
      if (FileExists(executable)) gGrid->Rm(executable);
      Info("WriteExecutable", "\n#####   Copying executable file <%s> to your AliEn bin directory", fExecutable.Data());
      TFile::Cp(Form("file:%s",fExecutable.Data()), Form("alien://%s", executable.Data()));
   } 
}

//______________________________________________________________________________
void AliAnalysisAlien::WriteMergeExecutable()
{
// Generate the alien executable script for the merging job.
   if (!fMergeViaJDL) return;
   TString mergeExec = fExecutable;
   mergeExec.ReplaceAll(".sh", "_merge.sh");
   if (!TestBit(AliAnalysisGrid::kSubmit)) {
      ofstream out;
      out.open(mergeExec.Data(), ios::out);
      if (out.bad()) {
         Error("WriteMergingExecutable", "Bad file name for executable: %s", mergeExec.Data());
         return;
      }
      out << "#!/bin/bash" << endl;
      out << "echo \"=========================================\"" << endl; 
      out << "echo \"############## PATH : ##############\"" << endl;
      out << "echo $PATH" << endl;
      out << "echo \"############## LD_LIBRARY_PATH : ##############\"" << endl;
      out << "echo $LD_LIBRARY_PATH" << endl;
      out << "echo \"############## ROOTSYS : ##############\"" << endl;
      out << "echo $ROOTSYS" << endl;
      out << "echo \"############## which root : ##############\"" << endl;
      out << "which root" << endl;
      out << "echo \"############## ALICE_ROOT : ##############\"" << endl;
      out << "echo $ALICE_ROOT" << endl;
      out << "echo \"############## which aliroot : ##############\"" << endl;
      out << "which aliroot" << endl;
      out << "echo \"############## system limits : ##############\"" << endl;
      out << "ulimit -a" << endl;
      out << "echo \"############## memory : ##############\"" << endl;
      out << "free -m" << endl;
      out << "echo \"=========================================\"" << endl << endl;
      // Make sure we can properly compile par files
      if (TObject::TestBit(AliAnalysisGrid::kUsePars)) out << "export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH" << endl;
      TString mergeMacro = fExecutable;
      mergeMacro.ReplaceAll(".sh", "_merge.C");
      if (IsOneStageMerging())
         out << "export ARG=\"" << mergeMacro << "(\\\"$1\\\")\"" << endl;
      else
         out << "export ARG=\"" << mergeMacro << "(\\\"$1\\\",$2,$3)\"" << endl;
      out << fExecutableCommand << " " << "$ARG" << endl; 
      out << "echo \"======== " << mergeMacro.Data() << " finished with exit code: $? ========\"" << endl;
      out << "echo \"############## memory after: ##############\"" << endl;
      out << "free -m" << endl;
   }   
   Bool_t copy = kTRUE;
   if (fProductionMode || TestBit(AliAnalysisGrid::kOffline) || TestBit(AliAnalysisGrid::kTest)) copy = kFALSE;
   if (copy) {
      CdWork();
      TString workdir = gGrid->GetHomeDirectory();
      TString bindir = Form("%s/bin", workdir.Data());
      if (!DirectoryExists(bindir)) gGrid->Mkdir(bindir,"-p");
      workdir += fGridWorkingDir;
      TString executable = Form("%s/bin/%s", gGrid->GetHomeDirectory(), mergeExec.Data());
      if (FileExists(executable)) gGrid->Rm(executable);
      Info("WriteMergeExecutable", "\n#####   Copying executable file <%s> to your AliEn bin directory", mergeExec.Data());
      TFile::Cp(Form("file:%s",mergeExec.Data()), Form("alien://%s", executable.Data()));
   } 
}

//______________________________________________________________________________
void AliAnalysisAlien::WriteProductionFile(const char *filename) const
{
// Write the production file to be submitted by LPM manager. The format is:
// First line: full_path_to_jdl estimated_no_subjobs_per_master
// Next lines: full_path_to_dataset XXX (XXX is a string)
// To submit, one has to: submit jdl XXX for all lines
   ofstream out;
   out.open(filename, ios::out);
   if (out.bad()) {
      Error("WriteProductionFile", "Bad file name: %s", filename);
      return;
   }
   TString workdir;
   if (!fProductionMode && !fGridWorkingDir.BeginsWith("/alice"))
      workdir = gGrid->GetHomeDirectory();
   workdir += fGridWorkingDir;
   Int_t njobspermaster = 1000*fNrunsPerMaster/fSplitMaxInputFileNumber;
   TString locjdl = Form("%s/%s", workdir.Data(),fJDLName.Data());
   out << locjdl << " " << njobspermaster << endl;
   Int_t nmasterjobs = fInputFiles->GetEntries();
   for (Int_t i=0; i<nmasterjobs; i++) {
      TString runOutDir = gSystem->BaseName(fInputFiles->At(i)->GetName());
      runOutDir.ReplaceAll(".xml", "");
      if (fOutputToRunNo)
         out << Form("%s", fInputFiles->At(i)->GetName()) << " " << runOutDir << endl;
      else
         out << Form("%s", fInputFiles->At(i)->GetName()) << " " << Form("%03d", i) << endl;
   }
   if (gGrid) {
      Info("WriteProductionFile", "\n#####   Copying production file <%s> to your work directory", filename);
      if (FileExists(filename)) gGrid->Rm(filename);
      TFile::Cp(Form("file:%s",filename), Form("alien://%s/%s", workdir.Data(),filename));
   }   
}

//______________________________________________________________________________
void AliAnalysisAlien::WriteValidationScript(Bool_t merge)
{
// Generate the alien validation script.
   // Generate the validation script
   TObjString *os;
   if (fValidationScript.IsNull()) {
      fValidationScript = fExecutable;
      fValidationScript.ReplaceAll(".sh", "_validation.sh");
   }   
   TString validationScript = fValidationScript;
   if (merge) validationScript.ReplaceAll(".sh", "_merge.sh");
   if (!Connect()) {
      Error("WriteValidationScript", "Alien connection required");
      return;
   }
   if (!fTerminateFiles.IsNull()) {
      fTerminateFiles.Strip();
      fTerminateFiles.ReplaceAll(" ",",");
   }   
   TString outStream = "";
   if (!TestBit(AliAnalysisGrid::kTest)) outStream = " >> stdout";
   if (!TestBit(AliAnalysisGrid::kSubmit)) {  
      ofstream out;
      out.open(validationScript, ios::out);
      out << "#!/bin/bash" << endl;
      out << "##################################################" << endl;
      out << "validateout=`dirname $0`" << endl;
      out << "validatetime=`date`" << endl;
      out << "validated=\"0\";" << endl;
      out << "error=0" << endl;
      out << "if [ -z $validateout ]" << endl;
      out << "then" << endl;
      out << "    validateout=\".\"" << endl;
      out << "fi" << endl << endl;
      out << "cd $validateout;" << endl;
      out << "validateworkdir=`pwd`;" << endl << endl;
      out << "echo \"*******************************************************\"" << outStream << endl;
      out << "echo \"* Automatically generated validation script           *\""  << outStream << endl;
      out << "" << endl;
      out << "echo \"* Time:    $validatetime \""  << outStream << endl;
      out << "echo \"* Dir:     $validateout\""  << outStream << endl;
      out << "echo \"* Workdir: $validateworkdir\""  << outStream << endl;
      out << "echo \"* ----------------------------------------------------*\""  << outStream << endl;
      out << "ls -la ./"  << outStream << endl;
      out << "echo \"* ----------------------------------------------------*\""  << outStream << endl << endl;
      out << "##################################################" << endl;
      out << "" << endl;

      out << "if [ ! -f stderr ] ; then" << endl;
      out << "   error=1" << endl;
      out << "   echo \"* ########## Job not validated - no stderr  ###\" " << outStream << endl;
      out << "   echo \"Error = $error\" " << outStream << endl;
      out << "fi" << endl;

      out << "parArch=`grep -Ei \"Cannot Build the PAR Archive\" stderr`" << endl;
      out << "segViol=`grep -Ei \"Segmentation violation\" stderr`" << endl;
      out << "segFault=`grep -Ei \"Segmentation fault\" stderr`" << endl;
      out << "glibcErr=`grep -Ei \"*** glibc detected ***\" stderr`" << endl;
      out << "" << endl;

      out << "if [ \"$parArch\" != \"\" ] ; then" << endl;
      out << "   error=1" << endl;
      out << "   echo \"* ########## Job not validated - PAR archive not built  ###\" " << outStream << endl;
      out << "   echo \"$parArch\" " << outStream << endl;
      out << "   echo \"Error = $error\" " << outStream << endl;
      out << "fi" << endl;

      out << "if [ \"$segViol\" != \"\" ] ; then" << endl;
      out << "   error=1" << endl;
      out << "   echo \"* ########## Job not validated - Segment. violation  ###\" " << outStream << endl;
      out << "   echo \"$segViol\" " << outStream << endl;
      out << "   echo \"Error = $error\" " << outStream << endl;
      out << "fi" << endl;

      out << "if [ \"$segFault\" != \"\" ] ; then" << endl;
      out << "   error=1" << endl;
      out << "   echo \"* ########## Job not validated - Segment. fault  ###\" " << outStream << endl;
      out << "   echo \"$segFault\" " << outStream << endl;
      out << "   echo \"Error = $error\" " << outStream << endl;
      out << "fi" << endl;

      out << "if [ \"$glibcErr\" != \"\" ] ; then" << endl;
      out << "   error=1" << endl;
      out << "   echo \"* ########## Job not validated - *** glibc detected ***  ###\" " << outStream << endl;
      out << "   echo \"$glibcErr\" " << outStream << endl;
      out << "   echo \"Error = $error\" " << outStream << endl;
      out << "fi" << endl;

      // Part dedicated to the specific analyses running into the train

      TString outputFiles = fOutputFiles;
      if (merge && !fTerminateFiles.IsNull()) {
         outputFiles += ",";
         outputFiles += fTerminateFiles;
      }
      TObjArray *arr = outputFiles.Tokenize(",");
      TIter next1(arr);
      TString outputFile;
      while (!merge && (os=(TObjString*)next1())) { 
         // No need to validate outputs produced by merging since the merging macro does this
         outputFile = os->GetString();
         Int_t index = outputFile.Index("@");
         if (index > 0) outputFile.Remove(index);
         if (fTerminateFiles.Contains(outputFile)) continue;
         if (outputFile.Contains("*")) continue;
         out << "if ! [ -f " << outputFile.Data() << " ] ; then" << endl;
         out << "   error=1" << endl;
         out << "   echo \"Output file " << outputFile << " not found. Job FAILED !\""  << outStream << endl;
         out << "   echo \"Output file " << outputFile << " not found. Job FAILED !\" >> stderr" << endl;
         out << "fi" << endl;
      }   
      delete arr;
      out << "if ! [ -f outputs_valid ] ; then" << endl;
      out << "   error=1" << endl;
      out << "   echo \"Output files were not validated by the analysis manager\" >> stdout" << endl;
      out << "   echo \"Output files were not validated by the analysis manager\" >> stderr" << endl;
      out << "fi" << endl;
      
      out << "if [ $error = 0 ] ; then" << endl;
      out << "   echo \"* ----------------   Job Validated  ------------------*\""  << outStream << endl;
      if (!IsKeepLogs()) {
         out << "   echo \"* === Logs std* will be deleted === \"" << endl;
         outStream = "";
         out << "   rm -f std*" << endl;
      }            
      out << "fi" << endl;

      out << "echo \"* ----------------------------------------------------*\""  << outStream << endl;
      out << "echo \"*******************************************************\""  << outStream << endl;
      out << "cd -" << endl;
      out << "exit $error" << endl;
   }    
   Bool_t copy = kTRUE;
   if (fProductionMode || TestBit(AliAnalysisGrid::kOffline) || TestBit(AliAnalysisGrid::kTest)) copy = kFALSE;
   if (copy) {
      CdWork();
      TString workdir = gGrid->GetHomeDirectory();
      workdir += fGridWorkingDir;
      Info("WriteValidationScript", "\n#####   Copying validation script <%s> to your AliEn working space", validationScript.Data());
      if (FileExists(validationScript)) gGrid->Rm(validationScript);
      TFile::Cp(Form("file:%s",validationScript.Data()), Form("alien://%s/%s", workdir.Data(),validationScript.Data()));
   } 
}
