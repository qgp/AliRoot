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
Revision 1.3.2.3  2002/11/22 14:19:31  hristov
Merging NewIO-01 with v3-09-04 (part one) (P.Skowronski)

Revision 1.3.2.2  2002/05/31 10:26:37  hristov
Changes from v3-08-02 merged with NewIO branch

Revision 1.3.2.1  2002/05/31 09:37:55  hristov
First set of changes done by Piotr

Revision 1.6  2002/10/14 14:55:35  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.4.4.1  2002/06/10 14:57:41  hristov
Merged with v3-08-02

Revision 1.5  2002/04/26 10:37:23  morsch
Method RewindEvent() added. (N. Carrer)

Revision 1.4  2002/03/22 08:25:33  morsch
TreeE connected correctly.

Revision 1.3  2001/12/12 11:21:37  morsch
Dummy copy constructor added.

Revision 1.2  2001/11/12 14:31:00  morsch
Memory leaks fixed. (M. Bondila)

Revision 1.1  2001/11/09 09:11:24  morsch
Realisation of AliGenReader that reads the kine tree (TreeK).

*/
#include <TFile.h>
#include <TTree.h>
#include <TParticle.h>
#include <TObjString.h>
#include <TObjArray.h>

#include "AliGenReaderTreeK.h"
#include "AliHeader.h"
#include "AliRun.h"
#include "AliRunLoader.h"

ClassImp(AliGenReaderTreeK);

const TString AliGenReaderTreeK::fgkEventFolderName("GenReaderTreeK");

AliGenReaderTreeK::AliGenReaderTreeK():
 AliGenReader(),
 fNcurrent(0),
 fNparticle(0),
 fNp(0),
 fInRunLoader(0),
 fBaseFile(0),
 fStack(0),
 fOnlyPrimaries(kFALSE),
 fDirs(0x0),
 fCurrentDir(0)
{
//  Default constructor
}

AliGenReaderTreeK::AliGenReaderTreeK(const AliGenReaderTreeK &reader):
 fNcurrent(0),
 fNparticle(0),
 fNp(0),
 fInRunLoader(0),
 fBaseFile(0),
 fStack(0),
 fOnlyPrimaries(kFALSE),
 fDirs(0x0),
 fCurrentDir(0)
{
    ;
}


AliGenReaderTreeK::~AliGenReaderTreeK() 
{
// Destructor
    delete fInRunLoader;//it cleans all the loaded data
    delete fDirs;
}

void AliGenReaderTreeK::Init() 
{
// Initialization
// Connect base file and file to read from

    TTree *ali = gAlice->TreeE();
    if (ali) {
      fBaseFile = ali->GetCurrentFile();
    } else {
      printf("\n Warning: Basefile cannot be found !\n");
    }
    //if (!fFile) fFile  = new TFile(fFileName);
    if (fInRunLoader == 0x0) 
     {
       fInRunLoader = AliRunLoader::Open((GetDirName(fCurrentDir++)+"/")+fFileName,fgkEventFolderName);
       fInRunLoader->LoadHeader();
       fInRunLoader->LoadKinematics("READ");
     }
}

Int_t AliGenReaderTreeK::NextEvent() 
{
//  Read the next event  
//  cd to file with old kine tree    
    if (!fBaseFile) Init();
//  Get next event
    
    if (fNcurrent >= fInRunLoader->GetNumberOfEvents())
     {
      if (fCurrentDir >= fDirs->GetEntries())
       {
         Warning("NextEvent","No more events");
         return 0;
       }
      delete fInRunLoader;
      fInRunLoader = AliRunLoader::Open((GetDirName(fCurrentDir++)+"/")+fFileName,fgkEventFolderName);
      fInRunLoader->LoadHeader();
      fInRunLoader->LoadKinematics("READ");
      fNcurrent = 0;
     }
    fInRunLoader->GetEvent(fNcurrent);
    fStack = fInRunLoader->Stack();
    
//  cd back to base file
    fBaseFile->cd();
//
    fNcurrent++;
    fNparticle = 0;
    fNp =  fStack->GetNtrack();
    printf("\n Next event contains %d particles", fNp);
//    
    return  fNp;
}

TParticle* AliGenReaderTreeK::NextParticle() 
{
//Return next particle
  TParticle* part = GetParticle(fNparticle++);
  if (part == 0x0) return 0x0;
  //if only primaries are to be read, and this particle is not primary enter loop  
  if (fOnlyPrimaries && ( part->GetFirstMother() > -1) ) 
   for (;;)
    { //look for a primary
      part = GetParticle(fNparticle++);
      if (part == 0x0) return 0x0;
      if (part->GetFirstMother() == -1) return part;
    }

  return part;
}

void AliGenReaderTreeK::RewindEvent()
{
  // Go back to the first particle of the event
  fNparticle = 0;
}


AliGenReaderTreeK& AliGenReaderTreeK::operator=(const  AliGenReaderTreeK& rhs)
{
// Assignment operator
    return *this;
}



TString& AliGenReaderTreeK::GetDirName(Int_t entry)
 {
   TString* retval;//return value
   if (fDirs ==  0x0)
    {
      retval = new TString(".");
      return *retval;
    }
   
   if ( (entry>fDirs->GetEntries()) || (entry<0))//if out of bounds return empty string
    {                                            //note that entry==0 is accepted even if array is empty (size=0)
      Error("GetDirName","Name out of bounds");
      retval = new TString();
      return *retval;
    }
   
   if (fDirs->GetEntries() == 0)
    { 
      retval = new TString(".");
      return *retval;
    }
   
   TObjString *dir = dynamic_cast<TObjString*>(fDirs->At(entry));
   if(dir == 0x0)
    {
      Error("GetDirName","Object in TObjArray is not a TObjString or its descendant");
      retval = new TString();
      return *retval;
    }
   if (gDebug > 0) Info("GetDirName","Returned ok %s",dir->String().Data());
   return dir->String();
 }
 
void AliGenReaderTreeK::AddDir(const char* dirname)
{
  //adds a directory to the list of directories where data are looked for
  if(fDirs == 0x0) 
   {
     fDirs = new TObjArray();
     fDirs->SetOwner(kTRUE);
   }
  TObjString *odir= new TObjString(dirname);
  fDirs->Add(odir);
}
