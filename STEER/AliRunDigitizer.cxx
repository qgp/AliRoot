/**************************************************************************
* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
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

/*
$Log$
Revision 1.13.4.5  2002/12/11 10:00:34  hristov
Merging with v3-09-04 (P.Skowronski)

Revision 1.13.4.4  2002/11/22 14:19:50  hristov
Merging NewIO-01 with v3-09-04 (part one) (P.Skowronski)

Revision 1.13.4.3  2002/06/21 15:22:13  hristov
RunDigitizer uses AliRunLoader for the output

Revision 1.13.4.2  2002/06/18 10:18:32  hristov
Important update (P.Skowronski)

Revision 1.13.4.1  2002/05/31 09:37:59  hristov
First set of changes done by Piotr

Revision 1.22  2002/10/29 14:26:49  hristov
Code clean-up (F.Carminati)

Revision 1.21  2002/10/22 15:02:15  alibrary
Introducing Riostream.h

Revision 1.20  2002/10/14 14:57:32  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.13.6.2  2002/07/24 10:08:13  alibrary
Updating VirtualMC

Revision 1.19  2002/07/19 12:46:05  hristov
Write file instead of closing it

Revision 1.18  2002/07/17 08:59:39  jchudoba
Do not delete subtasks when AliRunDigitizer is deleted. Owner should delete them itself.

Revision 1.17  2002/07/16 13:47:53  jchudoba
Add methods to get access to names of files used in merging.

Revision 1.16  2002/06/07 09:18:47  jchudoba
Changes to enable merging of ITS fast rec points. Although this class should be responsible for a creation of digits only, other solutions would be more complicated.

Revision 1.15  2002/04/09 13:38:47  jchudoba
Add const to the filename argument

Revision 1.14  2002/04/04 09:28:04  jchudoba
Change default names of TPC trees. Use update instead of recreate for the output file. Overwrite the AliRunDigitizer object in the output if it exists.

Revision 1.13  2002/02/13 09:03:32  jchudoba
Pass option to subtasks. Delete input TTrees. Use gAlice from memory if it is present (user must delete the default one created by aliroot if he/she wants to use gAlice from the input file!). Add new data member to store name of the special TPC TTrees.

Revision 1.12  2001/12/10 16:40:52  jchudoba
Import gAlice from the signal file before InitGlobal() to allow detectors to use it during initialization

Revision 1.11  2001/12/03 07:10:13  jchudoba
Default ctor cannot create new objects, create dummy default ctor which leaves object in not well defined state - to be used only by root for I/O

Revision 1.10  2001/11/15 11:07:25  jchudoba
Set to zero new pointers to TPC and TRD special trees in the default ctor. Add const to all Get functions. Remove unused constant, rename constant according coding rules.

Revision 1.9  2001/11/15 09:00:11  jchudoba
Add special treatment for TPC and TRD, they use different trees than other detectors

Revision 1.8  2001/10/21 18:38:43  hristov
Several pointers were set to zero in the default constructors to avoid memory management problems

Revision 1.7  2001/10/04 15:56:07  jchudoba
TTask inheritance

Revision 1.4  2001/09/19 06:23:50  jchudoba
Move some tasks to AliStream and AliMergeCombi classes

Revision 1.3  2001/07/30 14:04:18  jchudoba
correct bug in the initialization

Revision 1.2  2001/07/28 10:44:32  hristov
Loop variable declared once; typos corrected

Revision 1.1  2001/07/27 12:59:00  jchudoba
Manager class for merging/digitization

*/

//_______________________________________________________________________
//
// AliRunDigitizer.cxx
//
// Manager object for merging/digitization
//
// Instance of this class manages the digitization and/or merging of
// Sdigits into Digits. 
//
// Only one instance of this class is created in the macro:
//   AliRunDigitizer * manager = 
//      new AliRunDigitizer(nInputStreams,SPERB);
// where nInputStreams is number of input streams and SPERB is
// signals per background variable, which determines how combinations
// of signal and background events are generated.
// Then instances of specific detector digitizers are created:
//   AliMUONDigitizer *dMUON  = new AliMUONDigitizer(manager)
// and the I/O configured (you have to specify input files 
// and an output file). The manager connects appropriate trees from 
// the input files according a combination returned by AliMergeCombi 
// class. It creates TreeD in the output and runs once per 
// event Digitize method of all existing AliDetDigitizers 
// (without any option). AliDetDigitizers ask manager
// for a TTree with input (manager->GetInputTreeS(Int_t i),
// merge all inputs, digitize it, and save it in the TreeD 
// obtained by manager->GetTreeD(). Output events are stored with 
// numbers from 0, this default can be changed by 
// manager->SetFirstOutputEventNr(Int_t) method. The particle numbers
// in the output are shifted by MASK, which is taken from manager.
//
// The default output is to the signal file (stream 0). This can be 
// changed with the SetOutputFile(TString fn)  method.
//
// Single input file is permitted. Maximum kMaxStreamsToMerge can be merged.
// Input from the memory (on-the-fly merging) is not yet 
// supported, as well as access to the input data by invoking methods
// on the output data.
//
// Access to the some data is via gAlice for now (supposing the 
// same geometry in all input files), gAlice is taken from the first 
// input file on the first stream.
//
// Example with MUON digitizer, no merging, just digitization
//
//  AliRunDigitizer * manager = new AliRunDigitizer(1,1);
//  manager->SetInputStream(0,"galice.root");
//  AliMUONDigitizer *dMUON  = new AliMUONDigitizer(manager);
//  manager->Exec("");
//
// Example with MUON digitizer, merge all events from 
//   galice.root (signal) file with events from bgr.root 
//   (background) file. Number of merged events is
//   min(number of events in galice.root, number of events in bgr.root)
//
//  AliRunDigitizer * manager = new AliRunDigitizer(2,1);
//  manager->SetInputStream(0,"galice.root");
//  manager->SetInputStream(1,"bgr.root");
//  AliMUONDigitizer *dMUON  = new AliMUONDigitizer(manager);
//  manager->Exec("");
//
// Example with MUON digitizer, save digits in a new file digits.root,
//   process only 1 event
//
//  AliRunDigitizer * manager = new AliRunDigitizer(2,1);
//  manager->SetInputStream(0,"galice.root");
//  manager->SetInputStream(1,"bgr.root");
//  manager->SetOutputFile("digits.root");
//  AliMUONDigitizer *dMUON  = new AliMUONDigitizer(manager);
//  manager->SetNrOfEventsToWrite(1);
//  manager->Exec("");
//
//_______________________________________________________________________ 

// system includes

#include <Riostream.h>

// ROOT includes

#include "TFile.h"
#include "TList.h"
#include "TParticle.h"
#include "TTree.h"

// AliROOT includes

#include "AliDigitizer.h"
#include "AliHeader.h"
#include "AliMergeCombi.h"
#include "AliRunLoader.h"
#include "AliLoader.h"
#include "AliRun.h"
#include "AliRunDigitizer.h"
#include "AliStream.h"

ClassImp(AliRunDigitizer)

const TString AliRunDigitizer::fgkDefOutFolderName("Output");
const TString AliRunDigitizer::fgkBaseInFolderName("Input");


//_______________________________________________________________________
AliRunDigitizer::AliRunDigitizer():
 fkMASKSTEP(0),
 fOutputFileName(0),
 fOutputDirName(0),
 fEvent(0),
 fNrOfEventsToWrite(0),
 fNrOfEventsWritten(0),
 fCopyTreesFromInput(0),
 fNinputs(0),
 fNinputsGiven(0),
 fInputStreams(0x0),
 fOutRunLoader(0x0),
 fCombi(0),
 fCombination(0),
 fCombinationFileName(0),
 fDebug(0)
{
// root requires default ctor, where no new objects can be created
// do not use this ctor, it is supplied only for root needs

//  fOutputStream = 0x0;
}

//_______________________________________________________________________
AliRunDigitizer::AliRunDigitizer(Int_t nInputStreams, Int_t sperb):
 TTask("AliRunDigitizer","The manager for Merging"),
 fkMASKSTEP(10000000),
 fOutputFileName(""),
 fOutputDirName("."),
 fEvent(0),
 fNrOfEventsToWrite(-1),
 fNrOfEventsWritten(0),
 fCopyTreesFromInput(-1),
 fNinputs(nInputStreams),
 fNinputsGiven(0),
 fInputStreams(new TClonesArray("AliStream",nInputStreams)),
 fOutRunLoader(0x0),
 fCombi(new AliMergeCombi(nInputStreams,sperb)),
 fCombination(kMaxStreamsToMerge),
 fCombinationFileName(0),
 fDebug(0)

{
// ctor which should be used to create a manager for merging/digitization
  if (nInputStreams == 0) 
   {//kidding
    Fatal("AliRunDigitizer","Specify nr of input streams");
    return;
   }
  Int_t i;
  
  fkMASK[0] = 0;
  
  for (i=1;i<kMaxStreamsToMerge;i++) 
   {
    fkMASK[i] = fkMASK[i-1] + fkMASKSTEP;
   }
  
  TClonesArray &lInputStreams = *fInputStreams;

// the first Input is open RW to be output as well
  new(lInputStreams[0]) AliStream(fgkBaseInFolderName + "0","UPDATE");
  //fOutputStream =  (AliStream*)lInputStreams.At(0);//redirects output to first input
  
  
  for (i=1;i<nInputStreams;i++) {
    new(lInputStreams[i]) AliStream(fgkBaseInFolderName+(Long_t)i,"READ");
  }
}
//_______________________________________________________________________

AliRunDigitizer::AliRunDigitizer(const AliRunDigitizer& dig):
 fkMASKSTEP(0),
 fOutputFileName(0),
 fOutputDirName(0),
 fEvent(0),
 fNrOfEventsToWrite(0),
 fNrOfEventsWritten(0),
 fCopyTreesFromInput(0),
 fNinputs(0),
 fNinputsGiven(0),
 fInputStreams(0x0),
 fOutRunLoader(0x0),
 fCombi(0),
 fCombination(0),
 fCombinationFileName(0),
 fDebug(0)
{
  //
  // Copy ctor
  //
  dig.Copy(*this);
}
//_______________________________________________________________________

void AliRunDigitizer::Copy(AliRunDigitizer&) const
{
  Fatal("Copy","Not installed\n");
}

//_______________________________________________________________________

AliRunDigitizer::~AliRunDigitizer() {
// dtor
  if (GetListOfTasks()) 
    GetListOfTasks()->Clear("nodelete");
  delete fInputStreams;
  delete fCombi;
  delete fOutRunLoader;
}
//_______________________________________________________________________
void AliRunDigitizer::AddDigitizer(AliDigitizer *digitizer)
{
// add digitizer to the list of active digitizers
  this->Add(digitizer);
}
//_______________________________________________________________________
void AliRunDigitizer::SetInputStream(Int_t i, const char *inputFile, TString foldername)
{
  if (i > fInputStreams->GetLast()) {
    Error("SetInputStream","Input stream number too high");
    return;
  }
  AliStream * stream = static_cast<AliStream*>(fInputStreams->At(i)) ; 
  if ( !foldername.IsNull() ) {
    if ( i > 0 ) 
      foldername += i ; // foldername should stay unchanged for the default output 
    stream->SetFolderName(foldername) ;
  } 
  stream->AddFile(inputFile);
}

//_______________________________________________________________________
void AliRunDigitizer::Digitize(Option_t* option)
{
// get a new combination of inputs, loads events to folders

// take gAlice from the first input file. It is needed to access
//  geometry data
// If gAlice is already in memory, use it
  SetDebug(10);
  
  if (gAlice == 0x0)
   {
    if (!static_cast<AliStream*>(fInputStreams->At(0))->ImportgAlice()) 
     {
       Error("Digitize","Error occured while getting gAlice from Input 0");
       return;
     }
   }
    
  if (!InitGlobal()) //calls Init() for all (sub)digitizers
   {
     Error("Digitize","InitGlobal returned error");
     return;
   }
   
  Int_t eventsCreated = 0;
// loop until there is anything on the input in case fNrOfEventsToWrite < 0
  while ((eventsCreated++ < fNrOfEventsToWrite) || (fNrOfEventsToWrite < 0)) 
   {
     
    if (!ConnectInputTrees()) break;
    InitEvent();
    ExecuteTasks(option);// loop over all registered digitizers and let them do the work
    CleanTasks();
    FinishEvent();
  }
  FinishGlobal();
}

//_______________________________________________________________________
Bool_t AliRunDigitizer::ConnectInputTrees()
{
//loads events 
  Int_t eventNr[kMaxStreamsToMerge], delta[kMaxStreamsToMerge];
  fCombi->Combination(eventNr, delta);
  for (Int_t i=0;i<fNinputs;i++) 
   {
    if (delta[i] == 1)
     {
      AliStream *iStream = static_cast<AliStream*>(fInputStreams->At(i));//gets the "i" defined  in combination
      if (!iStream->NextEventInStream()) return kFALSE; //sets serial number
     } 
    else if (delta[i] != 0) 
     {
      Error("ConnectInputTrees","Only delta 0 or 1 is implemented");
      return kFALSE;
     }
  }
  return kTRUE;
}

//_______________________________________________________________________
Bool_t AliRunDigitizer::InitGlobal()
{
// called once before Digitize() is called, initialize digitizers and output

  TList* subTasks = this->GetListOfTasks();
  if (subTasks) {
    subTasks->ForEach(AliDigitizer,Init)();
  }
  return kTRUE;
}

//_______________________________________________________________________

void AliRunDigitizer::SetOutputFile(TString fn)
// the output will be to separate file, not to the signal file
{
 //here should be protection to avoid setting the same file as any input 
  Info("SetOutputFile","Setting Output File Name %s ",fn.Data());
  fOutputFileName = fn;
  InitOutputGlobal();
}

//_______________________________________________________________________
Bool_t AliRunDigitizer::InitOutputGlobal()
{
// Creates the output file, called by InitEvent()

  
//  fOutputStream = new AliStream(fgkDefOutFolderName,"recreate");
//  fOutputStream->AddFile(fOutputFileName);
  if ( !fOutputFileName.IsNull())
   {
    fOutRunLoader = AliRunLoader::Open(fOutputFileName,fgkDefOutFolderName,"recreate");
    
    if (fOutRunLoader == 0x0)
     {
       Error("InitOutputGlobal","Can not open ooutput");
       return kFALSE;
     }
    Info("InitOutputGlobal", " 1 %s = ", GetInputFolderName(0).Data()) ; 
    AliRunLoader* inrl = AliRunLoader::GetRunLoader(GetInputFolderName(0));
	 Info("InitOutputGlobal", " 2 %d = ", inrl) ; 
    const TObjArray* inloaders = inrl->GetArrayOfLoaders();

    TIter next(inloaders);
    AliLoader *loader;
    while((loader = (AliLoader*)next()))
     {
       GetOutRunLoader()->AddLoader(loader);
     }
 
    if (GetDebug()>2)  Info("InitOutputGlobal","file %s was opened.",fOutputFileName.Data());
   }
  return kTRUE;
}
//_______________________________________________________________________

void AliRunDigitizer::InitEvent()
{
//redirects output properly
  if (GetDebug()>2)
   {
    Info("InitEvent","fEvent = %d",fEvent);
    Info("InitEvent","fOutputFileName \"%s\"",fOutputFileName.Data());
   }
// if fOutputFileName was not given, write output to signal directory
}
//_______________________________________________________________________

void AliRunDigitizer::FinishEvent()
{
// called at the end of loop over digitizers

  Int_t i;
  
  if (GetOutRunLoader() == 0x0)
   {
     Error("FinishEvent","fOutRunLoader is null");
     return;
   }
  
  fEvent++;
  fNrOfEventsWritten++;
  
  if (fCopyTreesFromInput > -1) 
   {
    cout<<"Copy trees from input: Copy or link files manually"<<endl;
    return;
    
    i = fCopyTreesFromInput;

    TFolder* outfolder = GetOutRunLoader()->GetEventFolder();
    if (outfolder == 0x0)
     {
       Error("FinishEvent","Can not get Event Folder");
       return;
     }
    
    AliStream* instream = (AliStream*)fInputStreams->At(fCopyTreesFromInput);
    AliRunLoader* inRL = AliRunLoader::GetRunLoader(instream->GetFolderName());
    
    inRL->LoadKinematics("read");
    outfolder->Add( inRL->TreeK() );
    GetOutRunLoader()->WriteKinematics("OVERWRITE");
    inRL->UnloadKinematics();
    GetOutRunLoader()->UnloadKinematics();
    
    inRL->LoadTrackRefs("read");
    outfolder->Add( inRL->TreeTR() );
    GetOutRunLoader()->WriteTrackRefs("OVERWRITE");
    inRL->UnloadTrackRefs();
    GetOutRunLoader()->UnloadTrackRefs();
    
    const TObjArray* inloaders = inRL->GetArrayOfLoaders();
    const TObjArray* outloaders = GetOutRunLoader()->GetArrayOfLoaders();
    
    TIter next(inloaders);
    AliLoader *inloader;
    while((inloader = (AliLoader*)next()))
     {
       //find detector loader in out RL corresponding to inloader
       AliLoader* outloader =  dynamic_cast<AliLoader*>(outloaders->FindObject(inloader->GetName()));
       if (outloader == 0x0)
        {
          Warning("FinishEvent","Can not find %s in out Out Run Loader");
          continue;
        }
       
       inloader->LoadHits("read");//load hits in read mode for input
       outfolder = outloader->GetDetectorDataFolder();//get folder for detector data
       outfolder->Add(inloader->TreeH());//put in out folder tree from in
       outloader->WriteHits("OVERWRITE");//write out 

       inloader->UnloadHits();
       outloader->UnloadHits();
       
     }

//    outfolder->Add( inRL->TreeH() );
//    GetOutRunLoader()->WriteKine("OVERWRITE");
      
   }
}
//_______________________________________________________________________

void AliRunDigitizer::FinishGlobal()
{
// called at the end of Exec
// save unique objects to the output file

  if (GetOutRunLoader() == 0x0)
   {
     Error("FinishGlobal","Can not get RunLoader from Output Stream folder");
     return;
   }
  GetOutRunLoader()->CdGAFile();
  
  this->Write(0,TObject::kOverwrite);
   
  if (fCopyTreesFromInput > -1) 
   {
    TFolder* outfolder = GetOutRunLoader()->GetEventFolder();
    if (outfolder == 0x0)
     {
       Error("FinishEvent","Can not get Event Folder");
       return;
     }    
    AliStream* instream = (AliStream*)fInputStreams->At(fCopyTreesFromInput);
    AliRunLoader* inRN = AliRunLoader::GetRunLoader(instream->GetFolderName());
    
    outfolder->Add ( inRN->GetAliRun() );
    GetOutRunLoader()->WriteAliRun();

    outfolder->Add ( inRN->TreeE() );
    GetOutRunLoader()->WriteHeader();
   }
}
//_______________________________________________________________________

Int_t  AliRunDigitizer::GetNParticles(Int_t event) const
{
// return number of particles in all input files for a given
// event (as numbered in the output file)
// return -1 if some file cannot be accessed

  Int_t sum = 0;
  Int_t sumI;
  for (Int_t i = 0; i < fNinputs; i++) {
    sumI = GetNParticles(GetInputEventNumber(event,i), i);
    if (sumI < 0) return -1;
    sum += sumI;
  }
  return sum;
}
//_______________________________________________________________________

Int_t  AliRunDigitizer::GetNParticles(Int_t event, Int_t input) const
{
// return number of particles in input file input for a given
// event (as numbered in this input file)
// return -1 if some error

// Must be revised in the version with AliStream

  return -1;

}

//_______________________________________________________________________
Int_t* AliRunDigitizer::GetInputEventNumbers(Int_t event) const
{
// return pointer to an int array with input event numbers which were
// merged in the output event event

// simplified for now, implement later
  Int_t * a = new Int_t[kMaxStreamsToMerge];
  for (Int_t i = 0; i < fNinputs; i++) {
    a[i] = event;
  }
  return a;
}
//_______________________________________________________________________
Int_t AliRunDigitizer::GetInputEventNumber(Int_t event, Int_t input) const
{
// return an event number of an eventInput from input file input
// which was merged to create output event event

// simplified for now, implement later
  return event;
}
//_______________________________________________________________________
TParticle* AliRunDigitizer::GetParticle(Int_t i, Int_t event) const
{
// return pointer to particle with index i (index with mask)

// decode the MASK
  Int_t input = i/fkMASKSTEP;
  return GetParticle(i,input,GetInputEventNumber(event,input));
}

//_______________________________________________________________________
TParticle* AliRunDigitizer::GetParticle(Int_t i, Int_t input, Int_t event) const
{
// return pointer to particle with index i in the input file input
// (index without mask)
// event is the event number in the file input
// return 0 i fit does not exist

// Must be revised in the version with AliStream

  return 0;
}

//_______________________________________________________________________
void AliRunDigitizer::ExecuteTask(Option_t* option)
{
// overwrite ExecuteTask to do Digitize only

  if (!IsActive()) return;
  Digitize(option);
  fHasExecuted = kTRUE;
  return;
}

//_______________________________________________________________________
const TString& AliRunDigitizer::GetInputFolderName(Int_t i) const
{
  AliStream* stream = dynamic_cast<AliStream*>(fInputStreams->At(i));
  if (stream == 0x0)
   {
     Fatal("GetInputFolderName","Can not get the input stream. Index = %d. Exiting",i);
   }
  return stream->GetFolderName();
}
//_______________________________________________________________________

const char* AliRunDigitizer::GetOutputFolderName()
{
  return GetOutRunLoader()->GetEventFolder()->GetName();
}
//_______________________________________________________________________

AliRunLoader* AliRunDigitizer::GetOutRunLoader()
{
  if (fOutRunLoader) return fOutRunLoader;
  
  if ( fOutputFileName.IsNull() )
   {//guard that sombody calls it without settting file name
    cout<<"Output file name is empty. Using Input 0 for output\n";
    return AliRunLoader::GetRunLoader(GetInputFolderName(0));
   }
  InitOutputGlobal();
  return fOutRunLoader;
}
//_______________________________________________________________________

TString AliRunDigitizer::GetInputFileName(const Int_t input, const Int_t order) const 
{
// returns file name of the order-th file in the input stream input
// returns empty string if such file does not exist
// first input stream is 0
// first file in the input stream is 0
  TString fileName("");
  if (input >= fNinputs) return fileName;
  AliStream * stream = static_cast<AliStream*>(fInputStreams->At(input));
  if (order > stream->GetNInputFiles()) return fileName;
  fileName = stream->GetFileName(order);
  return fileName;
}
