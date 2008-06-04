// @(#)alimdc:$Name:  $:$Id$
// Author: Fons Rademakers  26/11/99

/**************************************************************************
 * Copyright(c) 1998-2003, ALICE Experiment at CERN, All rights reserved. *
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

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AliRawDB                                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <Riostream.h>

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,15,0)
#include <TBufferFile.h>
#else
#include <TBuffer.h>
#endif

#include <TSystem.h>
#include <TKey.h>

#include <TObjString.h>

#include "AliESDEvent.h"
#include "AliRawEvent.h"
#include "AliRawDataArray.h"
#include "AliRawEventHeaderBase.h"
#include "AliRawEquipment.h"
#include "AliRawEquipmentHeader.h"
#include "AliStats.h"

#include "AliRawDB.h"


ClassImp(AliRawDB)

const char *AliRawDB::fgkAliRootTag = "$Rev$";

// Split TPC into 9 branches in order to avoid problems with big memory
// consumption in case of TPC events w/o zero-suppression
Int_t AliRawDB::fgkDetBranches[AliDAQ::kNDetectors+1] = {1,1,1,18,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,10,1};

//______________________________________________________________________________
AliRawDB::AliRawDB(AliRawEvent *event,
		   AliESDEvent *esd, 
		   Int_t compress,
                   const char* fileName,
		   Int_t basketsize) :
  fRawDB(NULL),
  fTree(NULL),
  fEvent(event),
  fESDTree(NULL),
  fESD(esd),
  fCompress(compress),
  fBasketSize(basketsize),
  fMaxSize(-1),
  fFS1(""),
  fFS2(""),
  fDeleteFiles(kFALSE),
  fStop(kFALSE)
{
   // Create a new raw DB

  for (Int_t iDet = 0; iDet < AliDAQ::kNDetectors; iDet++) {
    fDetRawData[iDet] = new AliRawDataArray*[fgkDetBranches[iDet]];
    Int_t nDDLsPerBranch = AliDAQ::NumberOfDdls(iDet)/fgkDetBranches[iDet];
    for (Int_t iBranch = 0; iBranch < fgkDetBranches[iDet]; iBranch++)
      fDetRawData[iDet][iBranch] = new AliRawDataArray(nDDLsPerBranch);
  }

  fDetRawData[AliDAQ::kNDetectors] = new AliRawDataArray*[fgkDetBranches[AliDAQ::kNDetectors]];
  for (Int_t iBranch = 0; iBranch < fgkDetBranches[AliDAQ::kNDetectors]; iBranch++)
    fDetRawData[AliDAQ::kNDetectors][iBranch] = new AliRawDataArray(100);

   if (fileName) {
      if (!Create(fileName))
         MakeZombie();
   }
}


//______________________________________________________________________________
AliRawDB::~AliRawDB() {
  // Destructor

  if(Close()==-1) Error("~AliRawDB", "cannot close output file!");

  for (Int_t iDet = 0; iDet < (AliDAQ::kNDetectors + 1); iDet++) {
    for (Int_t iBranch = 0; iBranch < fgkDetBranches[iDet]; iBranch++)
      delete fDetRawData[iDet][iBranch];
    delete [] fDetRawData[iDet];
  }
}

//______________________________________________________________________________
Bool_t AliRawDB::FSHasSpace(const char *fs) const
{
   // Check for at least fMaxSize bytes of free space on the file system.
   // If the space is not available return kFALSE, kTRUE otherwise.

   Long_t id, bsize, blocks, bfree;

   if (gSystem->GetFsInfo(fs, &id, &bsize, &blocks, &bfree) == 1) {
      Error("FSHasSpace", "could not stat file system %s", fs);
      return kFALSE;
   }

   // Leave 5 percent of diskspace free
   Double_t avail = Double_t(bfree) * 0.95;
   if (avail*bsize > fMaxSize)
      return kTRUE;

   Warning("FSHasSpace", "no space on file system %s", fs);
   return kFALSE;
}

//______________________________________________________________________________
const char *AliRawDB::GetFileName() const
{
   // Return filename based on hostname and date and time. This will make
   // each file unique. Also makes sure (via FSHasSpace()) that there is
   // enough space on the file system to store the file. Returns 0 in
   // case of error or interrupt signal.

   static TString fname;
   static Bool_t  fstoggle = kFALSE;

   TString fs = fstoggle ? fFS2 : fFS1;
   TDatime dt;

   TString hostname = gSystem->HostName();
   Int_t pos;
   if ((pos = hostname.Index(".")) != kNPOS)
      hostname.Remove(pos);

   if (!FSHasSpace(fs)) {
      while (1) {
         fstoggle = !fstoggle;
         fs = fstoggle ? fFS2 : fFS1;
         if (FSHasSpace(fs)) break;
         Info("GetFileName", "sleeping 30 seconds before retrying...");
         gSystem->Sleep(30000);   // sleep for 30 seconds
         if (fStop) return 0;
      }
   }

   fname = fs + "/" + hostname + "_";
   fname += dt.GetDate();
   fname += "_";
   fname += dt.GetTime();
   fname += ".root";

   fstoggle = !fstoggle;

   return fname;
}

//______________________________________________________________________________
void AliRawDB::SetFS(const char* fs1, const char* fs2)
{
// set the file system location

  fFS1 = fs1;
  if (fs1 && !fFS1.Contains(":")) {
    gSystem->ResetErrno();
    gSystem->MakeDirectory(fs1);
    if (gSystem->GetErrno() && gSystem->GetErrno() != EEXIST) {
      SysError("SetFS", "mkdir %s", fs1);
    }
  }

  fFS2 = fs2;
  if (fs2) {
    gSystem->ResetErrno();
    gSystem->MakeDirectory(fs2);
    if (gSystem->GetErrno() && gSystem->GetErrno() != EEXIST) {
      SysError("SetFS", "mkdir %s", fs2);
    }
  }
}

//______________________________________________________________________________
Bool_t AliRawDB::Create(const char* fileName)
{
   // Create a new raw DB.

   const Int_t kMaxRetry = 1;
   const Int_t kMaxSleep = 1;      // seconds
   const Int_t kMaxSleepLong = 10; // seconds
   Int_t retry = 0;

again:
   if (fStop) return kFALSE;

   const char *fname = fileName;
   if (!fname) fname = GetFileName();
   if (!fname) {
      Error("Create", "error getting raw DB file name");
      return kFALSE;
   }

   retry++;

   fRawDB = TFile::Open(fname, GetOpenOption(),
			Form("ALICE raw-data file (%s)", GetAliRootTag()), fCompress,
			GetNetopt());
   if (!fRawDB) {
      if (retry < kMaxRetry) {
         Warning("Create", "failure to open file, sleeping %d %s before retrying...",
                 kMaxSleep, kMaxSleep==1 ? "second" : "seconds");
         gSystem->Sleep(kMaxSleep*1000);
         goto again;
      }
      Error("Create", "failure to open file %s after %d tries", fname, kMaxRetry);
      return kFALSE;
   }
   if (retry > 1)
      Warning("Create", "succeeded to open file after %d retries", retry);

   if (fRawDB->IsZombie()) {
      if (fRawDB->GetErrno() == ENOSPC ||
          fRawDB->GetErrno() == 1018   ||   // SECOMERR
          fRawDB->GetErrno() == 1027) {     // SESYSERR
         fRawDB->ResetErrno();
         delete fRawDB;
         Warning("Create", "file is a zombie (no space), sleeping %d %s before retrying...",
                 kMaxSleepLong, kMaxSleepLong==1 ? "second" : "seconds");
         gSystem->Sleep(kMaxSleepLong*1000);   // sleep 10 seconds before retrying
         goto again;
      }
      Error("Create", "file %s is zombie", fname);
      fRawDB->ResetErrno();
      delete fRawDB;
      fRawDB = 0;
      if (retry < kMaxRetry) {
         Warning("Create", "file is a zombie, sleeping %d %s before retrying...",
                 kMaxSleep, kMaxSleep==1 ? "second" : "seconds");
         gSystem->Sleep(kMaxSleep*1000);
         goto again;
      }
      Error("Create", "failure to open file %s after %d tries", fname, kMaxRetry);
      return kFALSE;
   }

   // Create raw data TTree
   MakeTree();

   return kTRUE;
}

//______________________________________________________________________________
void AliRawDB::MakeTree()
{
   // Create ROOT Tree object container.

   fTree = new TTree("RAW", Form("ALICE raw-data tree (%s)", GetAliRootTag()));
   fTree->SetAutoSave(21000000000LL);  // autosave when 21 Gbyte written

   fTree->BranchRef();

   // splitting 29.6 MB/s, no splitting 35.3 MB/s on P4 2GHz 15k SCSI
   //Int_t split   = 1;
   Int_t split   = 0;
   fTree->Branch("rawevent", "AliRawEvent", &fEvent, fBasketSize, split);

   // Make brach for each sub-detector
   for (Int_t iDet = 0; iDet < AliDAQ::kNDetectors; iDet++) {
     for (Int_t iBranch = 0; iBranch < fgkDetBranches[iDet]; iBranch++)
       fTree->Branch(Form("%s%d",AliDAQ::DetectorName(iDet),iBranch),"AliRawDataArray",
		     &fDetRawData[iDet][iBranch],fBasketSize,split);
   }
   // Make special branch for unrecognized raw-data payloads
   for (Int_t iBranch = 0; iBranch < fgkDetBranches[AliDAQ::kNDetectors]; iBranch++)
     fTree->Branch(Form("Common%d",iBranch),"AliRawDataArray",
		   &fDetRawData[AliDAQ::kNDetectors][iBranch],fBasketSize,split);

   // Create tree which will contain the HLT ESD information

   if (fESD) {
     fESDTree = new TTree("esdTree", Form("ALICE HLT ESD tree (%s)", GetAliRootTag()));
     fESDTree->SetAutoSave(21000000000LL);  // autosave when 21 Gbyte written
     split   = 0;
     fESDTree->Branch("ESD", "AliESDEvent", &fESD, fBasketSize, split);
   }

}

//______________________________________________________________________________
Long64_t AliRawDB::Close()
{
   // Close raw DB.
   if (!fRawDB) return 0;

   if (!fRawDB->IsOpen()) return 0;

   fRawDB->cd();

   // Write the tree.
   Bool_t error = kFALSE;
   if (fTree->Write() == 0)
     error = kTRUE;
   if (fESDTree)
     if (fESDTree->Write() == 0)
       error = kTRUE;

   // Close DB, this also deletes the fTree
   fRawDB->Close();

   Long64_t filesize = fRawDB->GetEND();

   if (fDeleteFiles) {
      gSystem->Unlink(fRawDB->GetName());
      delete fRawDB;
      fRawDB = 0;
      if(!error)
	return filesize;
      else
	return -1;
   }

   delete fRawDB;
   fRawDB = 0;
   if(!error)
     return filesize;
   else
     return -1;
}

//______________________________________________________________________________
Int_t AliRawDB::Fill()
{
   // Fill the trees and return the number of written bytes

  for (Int_t iDet = 0; iDet < (AliDAQ::kNDetectors + 1); iDet++)
    for (Int_t iBranch = 0; iBranch < fgkDetBranches[iDet]; iBranch++)
      fDetRawData[iDet][iBranch]->ClearData();

   // Move the raw-data payloads to the corresponding branches
  for(Int_t iSubEvent = 0; iSubEvent < fEvent->GetNSubEvents(); iSubEvent++) {
    AliRawEvent *subEvent = fEvent->GetSubEvent(iSubEvent);
    for(Int_t iEquipment = 0; iEquipment < subEvent->GetNEquipments(); iEquipment++) {
      AliRawEquipment *equipment = subEvent->GetEquipment(iEquipment);
      Int_t iDet = AliDAQ::kNDetectors;
      Int_t iBranch = 0; // can we split somehow the unrecognized data??? For the moment - no
      if(equipment->GetEquipmentHeader()->GetEquipmentSize()) {
	UInt_t eqId = equipment->GetEquipmentHeader()->GetId();
	Int_t ddlIndex = -1;
	iDet = AliDAQ::DetectorIDFromDdlID(eqId,ddlIndex);
	if (iDet < 0 || iDet >= AliDAQ::kNDetectors)
	  iDet = AliDAQ::kNDetectors;
	else
	  iBranch = (ddlIndex * fgkDetBranches[iDet])/AliDAQ::NumberOfDdls(iDet);
      }
      equipment->SetRawDataRef(fDetRawData[iDet][iBranch]);
    }
  }

   Double_t bytes = fRawDB->GetBytesWritten();
   Bool_t error = kFALSE;
   if (fTree->Fill() == -1)
     error = kTRUE;
   if (fESDTree) 
     if (fESDTree->Fill() == -1)
       error = kTRUE;
   if(!error)
     return Int_t(fRawDB->GetBytesWritten() - bytes);
   else
     return -1;
}

//______________________________________________________________________________
Long64_t AliRawDB::GetTotalSize()
{
   // Return the total size of the trees
  Long64_t total = 0;

  {
    Int_t skey = 0;
    TDirectory *dir = fTree->GetDirectory();
    if (dir) {
      TKey *key = dir->GetKey(fTree->GetName());
      if (key) skey = key->GetKeylen();
    }
    total += (Long64_t)skey + fTree->GetZipBytes();
  }

  if(fESDTree)
    {
      Int_t skey = 0;
      TDirectory *dir = fESDTree->GetDirectory();
      if (dir) {
	TKey *key = dir->GetKey(fESDTree->GetName());
	if (key) skey = key->GetKeylen();
      }
      total += (Long64_t)skey + fESDTree->GetZipBytes();
    }

  return total;
}

//______________________________________________________________________________
Long64_t AliRawDB::AutoSave()
{
  // Auto-save the raw-data and
  // esd (if any) trees

  Long64_t nbytes = fTree->AutoSave();

  if (fESDTree) nbytes += fESDTree->AutoSave();

  return nbytes;
}

//______________________________________________________________________________
void AliRawDB::WriteStats(AliStats* stats)
{
   // Write stats to raw DB, local run DB and global MySQL DB.

   AliRawEventHeaderBase &header = *GetEvent()->GetHeader();

   // Write stats into RawDB
   TDirectory *ds = gDirectory;
   GetDB()->cd();
   stats->SetEvents(GetEvents());
   stats->SetLastId(header.GetP("Id")[0]);
   stats->SetFileSize(GetBytesWritten());
   stats->SetCompressionFactor(GetCompressionFactor());
   stats->SetEndTime();
   stats->Write("stats");
   ds->cd();
}

//______________________________________________________________________________
Bool_t AliRawDB::NextFile(const char* fileName)
{
   // Close te current file and open a new one.
   // Returns kFALSE in case opening failed.

   Close();

   if (!Create(fileName)) return kFALSE;
   return kTRUE;
}

//______________________________________________________________________________
Float_t AliRawDB::GetCompressionFactor() const
{
   // Return compression factor.

   if (fTree->GetZipBytes() == 0.)
      return 1.0;
   else
      return fTree->GetTotBytes()/fTree->GetZipBytes();
}

//______________________________________________________________________________
const char *AliRawDB::GetAliRootTag()
{
  // Return the aliroot tag (version)
  // used to generate the raw data file.
  // Stored in the raw-data file title.

  static TString version = fgkAliRootTag;
  version.Remove(TString::kBoth,'$');
  version.ReplaceAll("Rev","AliRoot version");

  return version.Data();
}

//______________________________________________________________________________
Bool_t AliRawDB::WriteGuidFile(TString &guidFileFolder)
{
  // Write the guid file
  // in the specified folder or
  // in the folder where the raw data
  // file is.

   TString guidFileName;
   if (!guidFileFolder.IsNull()) {
     guidFileName = guidFileFolder;

     TString pathStr = fRawDB->GetName();
     TObjArray *pathArr = pathStr.Tokenize('/');
     guidFileName.Append("/");
     guidFileName.Append(((TObjString *)pathArr->Last())->String());
     pathArr->Delete();
     delete pathArr;
   }
   else
     guidFileName = fRawDB->GetName();

   guidFileName += ".guid";

   ofstream fguid(guidFileName.Data());
   if (!fguid.is_open()) {
     Error("WriteGuidFile", "failure to open guid file %s", guidFileName.Data());
     return kFALSE;
   }
   TString guid = fRawDB->GetUUID().AsString();
   fguid << "guid: \t" << guid.Data();
   fguid.close();

   return kTRUE;
}
