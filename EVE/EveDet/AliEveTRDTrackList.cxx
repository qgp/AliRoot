// Author: Benjamin Hess   23/09/2008

/*************************************************************************
 * Copyright (C) 2008, Alexandru Bercuci, Benjamin Hess.                 *
 * All rights reserved.                                                  *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AliEveTRDTrackList                                                   //
//                                                                      //
// An AliEveTRDTrackList is, in principal, a TEveElementList with some  //
// sophisticated features. You can add macros to this list, which then  //
// can be applied to the list of tracks (these tracks can be added to   //
// the list in the same way as for the TEveElementList). In general,    //
// please use AddMacro(...) for this purpose.                           //
// Macros that are no longer needed can be removed from the list via    //
// RemoveSelectionMacros(...) or RemoveProcessMacros(...) respectively. //
// This function takes an iterator of the list of entries that are to   //
// be removed. An entry looks like:                                     //
// "MacroName.C (Path: MacroPath)". This is the way, the information    //
// about a macro is stored in the AliEveTRDTrackList. If you have path  //
// and name of a macro, use MakeMacroEntry(...) to get the corresponding//
// entry. The type of the macros is stored in a map. You can get the    //
// macro type via GetMacroType(...).                                    //
// With ApplySTSelectionMacros(...) or ApplyProcessMacros(...)          //
// respectively you can apply the macros to the track list via          //
// iterators (same style like for RemoveProcessMacros(...) (cf. above)).//
// Selection macros (de-)select macros according to a selection rule    //
// by setting the rnr-state of the tracks.                              //
// If multiple selection macros are applied, a track is selected, if    //
// all selection macros select the track.                               //
// Process macros create data or histograms, which will be stored in    //
// a temporary file. The editor of this class will access this file     //
// and draw all the stuff within it's DrawHistos() function. The file   //
// will be deleted by the destructor.                                   //
//                                                                      //
// Currently, the following macro types are supported:                  //
// Selection macros:                                                    //
// Bool_t YourMacro(const AliTRDtrackV1*);                              //
// Bool_t YourMacro(const AliTRDtrackV1*, const AliTRDtrackV1*);        //
//                                                                      //
// Process macros:                                                      //
// void YourMacro(const AliTRDtrackV1*, Double_t*&, Int_t&);            //
// void YourMacro(const AliTRDtrackV1*, const AliTRDtrackV1*,           //
//                Double_t*&, Int_t&);                                  //
// TH1* YourMacro(const AliTRDtrackV1*);                                //
// TH1* YourMacro(const AliTRDtrackV1*, const AliTRDtrackV1*);          //
//                                                                      //
// The macros which take 2 tracks are applied to all pairs              //
// fullfilling the selection criteria.                                  //
//////////////////////////////////////////////////////////////////////////


// Uncomment to display debugging infos
//#define ALIEVETRDTRACKLIST_DEBUG

#include "AliEveTRDTrackList.h"

#include <AliTRDReconstructor.h>
#include <TFile.h>
#include <TFunction.h>
#include <TH1.h>
#include <TList.h>
#include <TMap.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TTree.h>
#include <TTreeStream.h>

ClassImp(AliEveTRDTrackList)

///////////////////////////////////////////////////////////
/////////////   AliEveTRDTrackList ////////////////////////
///////////////////////////////////////////////////////////
AliEveTRDTrackList::AliEveTRDTrackList(const Text_t* n, const Text_t* t, Bool_t doColor):
  TEveElementList(n, t, doColor),
  fMacroList(0),
  fMacroSelList(0),
  fDataFromMacroList(0),
  fMacroTypes(0),
  fDataTree(0),
  fHistoDataSelected(0),
  fMacroListSelected(0),
  fMacroSelListSelected(0),
  fSelectedTab(1),                              // Standard tab: "Apply macros" (index 1)
  fSelectedStyle(0)
{
  // Creates the AliEveTRDTrackList.

  // Only accept childs of type AliEveTRDTrack
  SetChildClass(AliEveTRDTrack::Class());

  // Allocate memory for the lists and declare them as owners of their contents
  fMacroList = new TList();
  fMacroList->TCollection::SetOwner(kTRUE);
  fMacroSelList = new TList();
  fMacroSelList->TCollection::SetOwner(kTRUE);
  fDataFromMacroList = new TList();
  fDataFromMacroList->TCollection::SetOwner(kTRUE);

  fMacroTypes = new TMap();
  // Set map to owner of it's objects to delete them, if they are removed from the map
  fMacroTypes->SetOwnerKeyValue(kTRUE, kTRUE);

  // Set the build directory for AClic
  gSystem->SetBuildDir("$HOME/.trdQArec");

  AddStandardMacros();
}

//______________________________________________________
AliEveTRDTrackList::~AliEveTRDTrackList()
{
  // Frees allocated memory (lists etc.).

  if (fMacroList != 0)
  {
    fMacroList->Delete();
    delete fMacroList;
    fMacroList = 0;
  }
  if (fMacroSelList != 0)
  {
    fMacroSelList->Delete();
    delete fMacroSelList;
    fMacroSelList = 0;
  } 
  if (fDataFromMacroList != 0)
  {
    fDataFromMacroList->Delete();
    delete fDataFromMacroList;
    fDataFromMacroList = 0;
  } 
  if (fDataTree != 0)
  {
    delete fDataTree;
    fDataTree = 0;
  } 
  if (fMacroTypes != 0)
  {
    fMacroTypes->DeleteAll();
    delete fMacroTypes;
    fMacroTypes = 0;
  }
  // Note: gSystem->AccessPathName(...) returns kTRUE, if the access FAILED!
  if(!gSystem->AccessPathName(Form("/tmp/TRD.TrackListMacroData_%s.root", gSystem->Getenv("USER")))) 
    gSystem->Exec(Form("rm /tmp/TRD.TrackListMacroData_%s.root", gSystem->Getenv("USER")));
}

//______________________________________________________
Int_t AliEveTRDTrackList::AddMacro(const Char_t* path, const Char_t* nameC, Bool_t forceReload)
{
  // Checks, if the file exists and if the signature is correct.
  // If these criteria are fullfilled, the library for this macro is built
  // and the macro is added to the corresponding list.
  // Supported macro types:
  // Selection macros:                                                    
  // Bool_t YourMacro(const AliTRDtrackV1*)
  // Bool_t YourMacro(const AliTRDtrackV1*, const AliTRDtrackV1*)
  //
  // Process macros:                                                      
  // void YourMacro(const AliTRDtrackV1*, Double_t*&, Int_t&)             
  // void YourMacro(const AliTRDtrackV1*, const AliTRDtrackV1*,           
  //                Double_t*&, Int_t&)                                   
  // TH1* YourMacro(const AliTRDtrackV1*)                                 
  // TH1* YourMacro(const AliTRDtrackV1*, const AliTRDtrackV1*)           
  //                                                                      
  // The macros which take 2 tracks are applied to all pairs              
  // fullfilling the selection criteria.                                  

  Char_t* entryName = MakeMacroEntry(path, nameC);

  Char_t pathname[fkMaxMacroPathNameLength];
  memset(pathname, '\0', sizeof(Char_t) * fkMaxMacroPathNameLength);

  // Expand the path and create the pathname
  Char_t* systemPath = gSystem->ExpandPathName(path);
  sprintf(pathname, "%s/%s", systemPath, nameC);
  delete systemPath;
  systemPath = 0;

  // Delete ".C" from filename
  Char_t name[fkMaxMacroNameLength];
  memset(name, '\0', sizeof(Char_t) * fkMaxMacroNameLength);
  
  for (UInt_t ind = 0; ind < fkMaxMacroNameLength && ind < strlen(nameC) - 2; ind++)  name[ind] = nameC[ind];

  // Check, if files exists
  FILE* fp = 0;

  fp = fopen(pathname, "rb");
  if (fp != 0)
  {
    fclose(fp);
    fp = 0;
  }
  else
  {
    if (entryName != 0)  delete entryName;
    entryName = 0;

    return NOT_EXIST_ERROR;
  }

  // Clean up root, load the desired macro and then check the type of the macro
  gROOT->Reset();
 
  if (forceReload)  gROOT->ProcessLineSync(Form(".L %s++", pathname));
  else              gROOT->ProcessLineSync(Form(".L %s+", pathname));

  AliEveTRDTrackListMacroType type = GetMacroType(entryName, kFALSE);

  // Clean up again
  gROOT->Reset();
  
  // Has not the correct signature!
  if (type == kUnknown) 
  {
    if (entryName != 0)  delete entryName;
    entryName = 0;
    return SIGNATURE_ERROR;
  }

  Int_t returnValue = WARNING;

  // Only add macro, if it is not already in the list
  if ((type == kSingleTrackAnalyse || type == kSingleTrackHisto 
      || type == kCorrelTrackAnalyse || type == kCorrelTrackHisto) && fMacroList->FindObject(entryName) == 0)
  {
    fMacroList->Add(new TObjString(entryName));
    fMacroList->Sort();

    fMacroTypes->Add(new TObjString(entryName), new TObjString(Form("%d", type)));

    // We do not know, where the element has been inserted - deselect this list
    fMacroListSelected = 0;

    returnValue = SUCCESS;
  }
  else if ((type == kSingleTrackSelect || type == kCorrelTrackSelect) && fMacroSelList->FindObject(entryName) == 0)
  {
    fMacroSelList->Add(new TObjString(entryName));
    fMacroSelList->Sort();

    fMacroTypes->Add(new TObjString(entryName), new TObjString(Form("%d", type)));
  
    // We do not know, where the element has been inserted - deselect this list
    fMacroSelListSelected = 0;
    
    returnValue = SUCCESS;
  }
  else  returnValue = WARNING;

  if (entryName != 0)  delete entryName;
  entryName = 0;

  return returnValue;
}

//______________________________________________________
void AliEveTRDTrackList::AddMacroFast(const Char_t* entry, AliEveTRDTrackListMacroType type)
{
  // Adds an entry to the corresponding list (cf. overloaded function).

  switch (type)
  {
    case kSingleTrackSelect:
    case kCorrelTrackSelect:
      fMacroSelList->Add(new TObjString(entry));
      fMacroSelList->Sort();

      fMacroTypes->Add(new TObjString(entry), new TObjString(Form("%d", type)));

      // We do not know, where the element has been inserted - deselect this list
      fMacroSelListSelected = 0;

      break;
    case kSingleTrackAnalyse:
    case kSingleTrackHisto:
    case kCorrelTrackAnalyse:
    case kCorrelTrackHisto:
      fMacroList->Add(new TObjString(entry));
      fMacroList->Sort();

      fMacroTypes->Add(new TObjString(entry), new TObjString(Form("%d", type)));

      // We do not know, where the element has been inserted - deselect this list
      fMacroListSelected = 0;
      break;
    default:
      Error("AliEveTRDTrackList::AddMacroFast", Form("Unknown macro type for entry \"%s\"!", entry));
      break;
  }
}

//______________________________________________________
void AliEveTRDTrackList::AddMacroFast(const Char_t* path, const Char_t* name, AliEveTRDTrackListMacroType type)
{
  // Adds a macro (path/name) to the list associated with the "type" parameter.
  // No checks are performed (fast) and no libraries are loaded. 
  // Do use only, if library already exists!

  Char_t* entry = MakeMacroEntry(path, name);
  if (entry != 0)
  {
    AddMacroFast(entry, type);

#ifdef ALIEVETRDTRACKLIST_DEBUG
    // Successfull add will only be displayed in debug mode
    printf("#AliEveTRDTrackList::AddMacroFast: Added macro \"%s/%s\" to the corresponding list\n", path, name);
#endif
    
    delete entry;
    entry = 0;
  }
  else
  {
    // Error will always be displayed
    printf("#AliEveTRDTrackList::AddMacroFast: ERROR: Could not add macro \"%s/%s\" to the corresponding list\n", 
           path, name);
  }
}

//______________________________________________________
void AliEveTRDTrackList::AddStandardMacros()
{
  // Adds standard macros to the lists.

  // Add your standard macros here, e.g.: 
  // To add a macro use:
  // AddMacro("$(ALICE_ROOT)/myFolder", "myMacroName.C");
  // -> If the file does not exist, nothing happens. So if you want to handle this,
  // use the return value of AddMacro (NOT_EXIST_ERROR is returned, if file does not exist)
  // (-> You can also check for other return values (see AddMacro(...)))
  AddMacro("$(ALICE_ROOT)/TRD/qaRec/macros", "clusterSelection.C");
  AddMacro("$(ALICE_ROOT)/TRD/qaRec/macros", "chargeDistr.C");
  AddMacro("$(ALICE_ROOT)/TRD/qaRec/macros", "clusterResiduals.C");
  AddMacro("$(ALICE_ROOT)/TRD/qaRec/macros", "PH.C");
}

//______________________________________________________
Bool_t AliEveTRDTrackList::ApplyProcessMacros(const TList* selIterator, const TList* procIterator)
{
  // Uses the procIterator (for the selected process macros) to apply the selected macros to the data.
  // Returns kTRUE on success, otherwise kFALSE. If there no process macros selected, kTRUE is returned 
  // (this is no error!).
  // The single track process macros are applied to all selected tracks.
  // The selIterator (for the selected selection macros) will be used to apply the correlated tracks selection
  // macros to all track pairs (whereby BOTH tracks have to be selected, otherwise they will be skipped).
  // All track pairs that have been selected by ALL correlated tracks selection macros will be processed by
  // the correlated tracks process macros.

  // No process macros need to be processed
  if (procIterator->GetEntries() <= 0)  return kTRUE;

  // Clear root
  gROOT->Reset();
  
  // Clear old data and re-allocate
  if (fDataTree == 0) fDataTree = new TTreeSRedirector(Form("/tmp/TRD.TrackListMacroData_%s.root", 
                                                            gSystem->Getenv("USER")));
  if (!fDataTree)
  {
    Error("Apply process macros", Form("File \"/tmp/TRD.TrackListMacroData_%s.root\" could not be accessed properly!", 
                                       gSystem->Getenv("USER")));
    return kFALSE;
  }
  
  if (fDataFromMacroList != 0)
  {
    fDataFromMacroList->Delete();
    delete fDataFromMacroList;
  }
  fDataFromMacroList = new TList();
  fDataFromMacroList->TCollection::SetOwner(kTRUE);

  fHistoDataSelected = 0;


  Char_t name[fkMaxMacroNameLength];
  Char_t** procCmds = new Char_t*[procIterator->GetEntries()];
  Char_t** selCmds  = new Char_t*[selIterator->GetEntries()];
  AliEveTRDTrackListMacroType* mProcType = new AliEveTRDTrackListMacroType[procIterator->GetEntries()];
  AliEveTRDTrackListMacroType* mSelType = new AliEveTRDTrackListMacroType[selIterator->GetEntries()];

  Bool_t selectedByCorrSelMacro = kFALSE;

  AliEveTRDTrackListMacroType macroType = kUnknown;
  Int_t numHistoMacros = 0;
  TH1** histos = 0;

  AliEveTRDTrack* track1 = 0;
  AliEveTRDTrack* track2 = 0;
  TH1* returnedHist = 0x0;

  // Collect the commands for each process macro and add them to "data-from-list"
  for (Int_t i = 0; i < procIterator->GetEntries(); i++)
  {
    memset(name, '\0', sizeof(Char_t) * fkMaxMacroNameLength);
    
    procCmds[i] = new Char_t[(fkMaxMacroPathNameLength + fkMaxApplyCommandLength)];
    memset(procCmds[i], '\0', sizeof(Char_t) * (fkMaxMacroNameLength + fkMaxApplyCommandLength));

#ifdef ALIEVETRDTRACKLIST_DEBUG
    printf("AliEveTRDTrackList: Applying process macro: %s\n", procIterator->At(i)->GetTitle());
#endif
 
    // Extract the name
    sscanf(procIterator->At(i)->GetTitle(), "%s (Path: %*s)", name);
   
    // Delete ".C" at the end 
    // -> Note: Physical address pointer, do NOT delete. / Changes "name" as well!
    Char_t* dotC = (Char_t*)strrchr(name, '.');
    if (dotC != 0)
    {
      *dotC = '\0';
      dotC++;
      *dotC = '\0';
    }
       
    // Find the type of the process macro
    macroType = GetMacroType(procIterator->At(i)->GetTitle(), kTRUE);
    if (macroType == kSingleTrackHisto)
    {
      mProcType[i] = macroType;
      numHistoMacros++;
      // Create the command 
      sprintf(procCmds[i], "%s(automaticTrackV1_1);", name);

      // Add to "data-from-list" -> Mark as a histo macro with the substring "(histo macro)"
      fDataFromMacroList->Add(new TObjString(Form("%s (histo macro)", name)));
    }
    else if (macroType == kSingleTrackAnalyse)
    {
      mProcType[i] = macroType;
      // Create the command 
      sprintf(procCmds[i], "%s(automaticTrackV1_1, results, n);", name);

      // Add to "data-from-list"
      fDataFromMacroList->Add(new TObjString(name));
    }
    else if (macroType == kCorrelTrackHisto)
    {
      mProcType[i] = macroType;
      numHistoMacros++;
      // Create the command 
      sprintf(procCmds[i], "%s(automaticTrackV1_1, automaticTrackV1_2);", name);

      // Add to "data-from-list" -> Mark as a histo macro with the substring "(histo macro)"
      fDataFromMacroList->Add(new TObjString(Form("%s (histo macro)", name)));
    }
    else if (macroType == kCorrelTrackAnalyse)
    {
      mProcType[i] = macroType;
      // Create the command 
      sprintf(procCmds[i], "%s(automaticTrackV1_1, automaticTrackV1_2, results, n);", name);

      // Add to "data-from-list"
      fDataFromMacroList->Add(new TObjString(name));
    }
    else
    {
      Error("Apply process macros", 
            Form("Process macro list corrupted: Macro \"%s\" is not registered as a process macro!", name));
      mProcType[i] = kUnknown;
    } 
  }  

  // Collect the commands for each selection macro and add them to "data-from-list"
  for (Int_t i = 0; i < selIterator->GetEntries(); i++)
  {
    memset(name, '\0', sizeof(Char_t) * fkMaxMacroNameLength);
    
    selCmds[i] = new Char_t[(fkMaxMacroPathNameLength + fkMaxApplyCommandLength)];
    memset(selCmds[i], '\0', sizeof(Char_t) * (fkMaxMacroNameLength + fkMaxApplyCommandLength));

#ifdef ALIEVETRDTRACKLIST_DEBUG
    printf("AliEveTRDTrackList: Applying selection macro (correlated tracks): %s\n", selIterator->At(i)->GetTitle());
#endif
 
    // Extract the name
    sscanf(selIterator->At(i)->GetTitle(), "%s (Path: %*s)", name);
   
    // Delete ".C" at the end 
    // -> Note: Physical address pointer, do NOT delete. / Changes "name" as well!
    Char_t* dotC = (Char_t*)strrchr(name, '.');
    if (dotC != 0)
    {
      *dotC = '\0';
      dotC++;
      *dotC = '\0';
    }
       
    // Find the type of the process macro
    macroType = GetMacroType(selIterator->At(i)->GetTitle(), kTRUE);
    // Single track select macro
    if (macroType == kSingleTrackSelect)
    {
      // Has already been processed by ApplySTSelectionMacros(...)
      mSelType[i] = macroType;         
    }
    // Correlated tracks select macro
    else if (macroType == kCorrelTrackSelect)
    {
      mSelType[i] = macroType;  
 
      // Create the command
      sprintf(selCmds[i], "%s(automaticTrackV1_1, automaticTrackV1_2);", name);
    }
    else
    {
      Error("Apply process macros", 
            Form("Selection macro list corrupted: Macro \"%s\" is not registered as a selection macro!", name));
      mProcType[i] = kUnknown;
    } 
  }  

  // Allocate memory for the histograms
  if (numHistoMacros > 0)  histos = new TH1*[numHistoMacros];
  for (Int_t i = 0; i < numHistoMacros; i++)  histos[i] = 0;
  
  // Walk through the list of tracks     
  for (TEveElement::List_i iter = this->BeginChildren(); iter != this->EndChildren(); ++iter)
  {
    track1 = dynamic_cast<AliEveTRDTrack*>(*iter);

    if (!track1)  continue;
    
    // Skip tracks that have not been selected
    if (!track1->GetRnrState())  continue;
      
    track1->ExportToCINT((Text_t*)"automaticTrack");
    // Cast to AliTRDtrackV1
    gROOT->ProcessLineSync("AliTRDtrackV1* automaticTrackV1_1 = (AliTRDtrackV1*)automaticTrack->GetUserData();");

    // Collect data for each macro
    for (Int_t i = 0, histoIndex = 0; i < procIterator->GetEntries(); i++)
    {
      // Single track histo
      if (mProcType[i] == kSingleTrackHisto)
      {
        returnedHist = (TH1*)gROOT->ProcessLineSync(procCmds[i]);
        if (returnedHist != 0x0)
        {
          if (histos[histoIndex] == 0)  histos[histoIndex] = returnedHist;
          else  
          {
            histos[histoIndex]->Add((const TH1*)returnedHist);
            delete returnedHist;
            returnedHist = 0;
          }
        }
        histoIndex++;
      }
      // Correlated tracks histo
      else if (mProcType[i] == kCorrelTrackHisto)
      {
        // Loop over all pairs behind the current one - together with the other loop this will be a loop
        // over all pairs. We have a pair of tracks, if and only if both tracks of the pair are selected (Rnr-state)
        // and are not equal.
        // The correlated tracks process macro will applied to all pairs that will be additionally selected by
        // all correlated tracks selection macros.
        TEveElement::List_i iter2 = iter;
        iter2++;
        for ( ; iter2 != this->EndChildren(); ++iter2)
        {
          track2 = dynamic_cast<AliEveTRDTrack*>(*iter2);

          if (!track2)  continue;
    
          // Skip tracks that have not been selected
          if (!track2->GetRnrState())  continue;
      
          track2->ExportToCINT((Text_t*)"automaticTrack");
          // Cast to AliTRDtrackV1
          gROOT->ProcessLineSync("AliTRDtrackV1* automaticTrackV1_2 = (AliTRDtrackV1*)automaticTrack->GetUserData();");

          // Select track by default (so it will be processed, if there are no correlated tracks selection macros!)
          selectedByCorrSelMacro = kTRUE;
          for (Int_t j = 0; j < selIterator->GetEntries(); j++)
          {
            if (mSelType[j] == kCorrelTrackSelect)
            {
              selectedByCorrSelMacro = (Bool_t)gROOT->ProcessLineSync(selCmds[j]);
              if (!selectedByCorrSelMacro)  break;
            }
          }       

          // If the pair has not been selected by the correlated tracks selection macros, skip it!
          if (!selectedByCorrSelMacro) continue;
          
          returnedHist = (TH1*)gROOT->ProcessLineSync(procCmds[i]);
          if (returnedHist != 0x0)
          {
            if (histos[histoIndex] == 0)  histos[histoIndex] = returnedHist;
            else  
            {
              histos[histoIndex]->Add((const TH1*)returnedHist);

              delete returnedHist;
              returnedHist = 0;
            }
          }
        }
        histoIndex++;
      }
      // Single track analyse
      else if (mProcType[i] == kSingleTrackAnalyse)
      {
        // Create data pointers in CINT, execute the macro and get the data
        gROOT->ProcessLineSync("Double_t* results = 0;");
        gROOT->ProcessLineSync("Int_t n = 0;");
        gROOT->ProcessLineSync(procCmds[i]);
        Double_t* results = (Double_t*)gROOT->ProcessLineSync("results;");
        Int_t nResults = (Int_t)gROOT->ProcessLineSync("n;");
        
        if (results == 0)
        {
          Error("Apply macros", Form("Error reading data from macro \"%s\"", procIterator->At(i)->GetTitle()));
          continue;
        }
        for (Int_t resInd = 0; resInd < nResults; resInd++)
        {
          (*fDataTree) << Form("TrackData%d", i) << Form("Macro%d=", i) << results[resInd] << (Char_t*)"\n";   
        }

        delete results;
        results = 0;
      }
      // Correlated tracks analyse
      else if (mProcType[i] == kCorrelTrackAnalyse)
      {
        // Loop over all pairs behind the current one - together with the other loop this will be a loop
        // over all pairs. We have a pair of tracks, if and only if both tracks of the pair are selected (Rnr-state)
        // and are not equal.
        // The correlated tracks process macro will applied to all pairs that will be additionally selected by
        // all correlated tracks selection macros.
        TEveElement::List_i iter2 = iter;
        iter2++;
        for ( ; iter2 != this->EndChildren(); ++iter2)
        {
          track2 = dynamic_cast<AliEveTRDTrack*>(*iter2);

          if (!track2)  continue;
    
          // Skip tracks that have not been selected
          if (!track2->GetRnrState())  continue;
      
          track2->ExportToCINT((Text_t*)"automaticTrack");
          // Cast to AliTRDtrackV1
          gROOT->ProcessLineSync("AliTRDtrackV1* automaticTrackV1_2 = (AliTRDtrackV1*)automaticTrack->GetUserData();");

          // Select track by default (so it will be processed, if there are no correlated tracks selection macros!)
          selectedByCorrSelMacro = kTRUE;
          for (Int_t j = 0; j < selIterator->GetEntries(); j++)
          {
            if (mSelType[j] == kCorrelTrackSelect)
            {
              selectedByCorrSelMacro = (Bool_t)gROOT->ProcessLineSync(selCmds[j]);
              if (!selectedByCorrSelMacro)  break;
            }
          }       

          // If the pair has not been selected by the correlated tracks selection macros, skip it!
          if (!selectedByCorrSelMacro) continue;
          
          // Create data pointers in CINT, execute the macro and get the data
          gROOT->ProcessLineSync("Double_t* results = 0;");
          gROOT->ProcessLineSync("Int_t n = 0;");
          gROOT->ProcessLineSync(procCmds[i]);
          Double_t* results = (Double_t*)gROOT->ProcessLineSync("results;");
          Int_t nResults = (Int_t)gROOT->ProcessLineSync("n;");
          
          if (results == 0)
          {
            Error("Apply macros", Form("Error reading data from macro \"%s\"", procIterator->At(i)->GetTitle()));
            continue;
          }
          for (Int_t resInd = 0; resInd < nResults; resInd++)
          {
            (*fDataTree) << Form("TrackData%d", i) << Form("Macro%d=", i) << results[resInd] << (Char_t*)"\n";   
          }

          delete results;
          results = 0;
        }
      }
    }
  }    

  for (Int_t i = 0, histoIndex = 0; i < procIterator->GetEntries() && histoIndex < numHistoMacros; i++)
  {
    if (mProcType[i] == kSingleTrackHisto || mProcType[i] == kCorrelTrackHisto)
    {
      // Might be empty (e.g. no tracks have been selected)!
      if (histos[histoIndex] != 0)
      {
        (*fDataTree) << Form("TrackData%d", i) << Form("Macro%d=", i) << histos[histoIndex] << (Char_t*)"\n";
      }
      histoIndex++;
    }
  }

  if (fDataTree != 0) delete fDataTree;
  fDataTree = 0;

  if (procCmds != 0)  delete [] procCmds;
  if (mProcType != 0)  delete mProcType;
  mProcType = 0;

  if (selCmds != 0)  delete [] selCmds;
  if (mSelType != 0)  delete mSelType;
  mSelType = 0;

  if (histos != 0)  delete [] histos;
  histos = 0;

  // Clear root
  gROOT->Reset();
  
  // If there is data, select the first data set
  if (procIterator->GetEntries() > 0) SETBIT(fHistoDataSelected, 0);

  // Now the data is stored in "/tmp/TRD.TrackListMacroData_$USER.root"
  // The editor will access this file to display the data
  return kTRUE;
}

//______________________________________________________
void AliEveTRDTrackList::ApplySTSelectionMacros(const TList* iterator)
{
  // Uses the iterator (for the selected selection macros) to apply the selected macros to the data.
  // The rnr-states of the tracks are set according to the result of the macro calls (kTRUE, if all
  // macros return kTRUE for this track, otherwise: kFALSE).
  // "ST" stands for "single track". This means that only single track selection macros are applied.
  // Correlated tracks selection macros will be used inside the call of ApplyProcessMacros(...)!

  Char_t name[fkMaxMacroNameLength];
  Char_t cmd[(fkMaxMacroNameLength + fkMaxApplyCommandLength)];

  AliEveTRDTrackListMacroType macroType = kUnknown;
  AliEveTRDTrack* track1 = 0;
  Bool_t selectedByMacro = kFALSE;

  // Clear root
  gROOT->Reset();

  // Select all tracks at first. A track is then deselect, if at least one selection macro
  // returns kFALSE for this track
  // Enable all tracks (Note: EnableListElements(..) will call "ElementChanged", which will cause unforeseen behavior!)
  for (TEveElement::List_i iter = this->BeginChildren(); iter != this->EndChildren(); ++iter)
  {
    ((TEveElement*)(*iter))->SetRnrState(kTRUE);
  }
  SetRnrState(kTRUE);
  
  for (Int_t i = 0; i < iterator->GetEntries(); i++)
  {

    memset(name, '\0', sizeof(Char_t) * fkMaxMacroNameLength);
    memset(cmd, '\0', sizeof(Char_t) * (fkMaxMacroNameLength + fkMaxApplyCommandLength));

#ifdef ALIEVETRDTRACKLIST_DEBUG
    printf("AliEveTRDTrackList: Applying selection macro: %s\n", iterator->At(i)->GetTitle());
#endif
    
    // Extract the name
    sscanf(iterator->At(i)->GetTitle(), "%s (Path: %*s)", name);
    // Delete ".C" at the end 
    // -> Note: Physical address pointer, do NOT delete. / Changes "name" as well!
    Char_t* dotC = (Char_t*)strrchr(name, '.');
    if (dotC != 0)
    {
      *dotC = '\0';
      dotC++;
      *dotC = '\0';
    }

    // Determine macro type
    macroType = GetMacroType(iterator->At(i)->GetTitle(), kTRUE);

    // Single track select macro
    if (macroType == kSingleTrackSelect)
    {
      // Create the command
      sprintf(cmd, "%s(automaticTrackV1);", name);

      // Walk through the list of tracks
      for (TEveElement::List_i iter = this->BeginChildren(); iter != this->EndChildren(); ++iter)
      {
        track1 = dynamic_cast<AliEveTRDTrack*>(*iter);

        if (!track1) continue;

        // If the track has already been deselected, nothing is to do here
        if (!track1->GetRnrState()) continue;

        track1->ExportToCINT((Text_t*)"automaticTrack");
        // Cast to AliTRDtrackV1
        gROOT->ProcessLineSync("AliTRDtrackV1* automaticTrackV1 = (AliTRDtrackV1*)automaticTrack->GetUserData();");
        selectedByMacro = (Bool_t)gROOT->ProcessLineSync(cmd);
        track1->SetRnrState(selectedByMacro && track1->GetRnrState());               
      }
    }
    // Correlated tracks select macro
    else if (macroType == kCorrelTrackSelect)
    {
      // Will be processed in ApplyProcessMacros(...)
      continue;
    }
    else
    {
      Error("Apply selection macros", 
            Form("Selection macro list corrupted: Macro \"%s\" is not registered as a selection macro!", name));
    } 
  }

  // Clear root
  gROOT->Reset();  
}

//______________________________________________________
AliEveTRDTrackList::AliEveTRDTrackListMacroType AliEveTRDTrackList::GetMacroType(const Char_t* entry, Bool_t UseList) const
{
  // Returns the type of the macro of the corresponding entry (i.e. "macro.C (Path: path)"). 
  // If you have only the name and the path, you can simply use MakeMacroEntry.
  // If "UseList" is kTRUE, the type will be looked up in the internal list (very fast). But if this list
  // does not exist, you have to use kFALSE for this parameter. Then the type will be determined by the
  // prototype! NOTE: It is assumed that the macro has been compiled! If not, the return value is not
  // predictable, but normally will be kUnknown.
  // Note: AddMacro(Fast) will update the internal list and RemoveProcess(/Selection)Macros respectively.

  AliEveTRDTrackListMacroType type = kUnknown;

  // Re do the check of the macro type
  if (!UseList)
  {
    Char_t name[fkMaxMacroNameLength];
  
    memset(name, '\0', sizeof(Char_t) * fkMaxMacroNameLength);

    // Extract the name
    sscanf(entry, "%s (Path: %*s)", name);
   
    // Delete ".C" at the end 
    // -> Note: Physical address pointer, do NOT delete. / Changes "name" as well!
    Char_t* dotC = (Char_t*)strrchr(name, '.');
    if (dotC != 0)
    {
      *dotC = '\0';
      dotC++;
      *dotC = '\0';
    }

    // Single track select macro or single track histo macro?
    TFunction* f = gROOT->GetGlobalFunctionWithPrototype(name, "const AliTRDtrackV1*", kTRUE);
    if (f != 0x0)
    {
      // Some additional check (is the parameter EXACTLY of the desired type?)
      if (strstr(f->GetMangledName(), "oPconstsPAliTRDtrackV1mUsP") != 0x0)
      {
        // Single track select macro?
        if (!strcmp(f->GetReturnTypeName(), "Bool_t")) 
        { 
          type = kSingleTrackSelect;     
        }
        // single track histo macro?
        else if (!strcmp(f->GetReturnTypeName(), "TH1*"))
        {
          type = kSingleTrackHisto;
        }
      }
    }
    // Single track analyse macro?
    else if ((f = gROOT->GetGlobalFunctionWithPrototype(name, "const AliTRDtrackV1*, Double_t*&, Int_t&", kTRUE)) 
             != 0x0)
    {
      if (!strcmp(f->GetReturnTypeName(), "void"))
      {
        // Some additional check (are the parameters EXACTLY of the desired type?)
        if (strstr(f->GetMangledName(), "oPconstsPAliTRDtrackV1mUsP") != 0x0 &&
            strstr(f->GetMangledName(), "cODouble_tmUaNsP") != 0x0 &&
            strstr(f->GetMangledName(), "cOInt_taNsP") != 0x0)
        {
          type = kSingleTrackAnalyse;
        }
      }
    }    
    // Correlated tracks select macro or correlated tracks histo macro?
    else if ((f = gROOT->GetGlobalFunctionWithPrototype(name, "const AliTRDtrackV1*, const AliTRDtrackV1*", kTRUE)) 
             != 0x0)
    {
      // Some additional check (is the parameter EXACTLY of the desired type?)
      if (strstr(f->GetMangledName(), "oPconstsPAliTRDtrackV1mUsP") != 0x0 &&
          strstr(f->GetMangledName(), "cOconstsPAliTRDtrackV1mUsP") != 0x0)
      {
        // Single track select macro?
        if (!strcmp(f->GetReturnTypeName(), "Bool_t")) 
        { 
          type = kCorrelTrackSelect;     
        }
        // single track histo macro?
        else if (!strcmp(f->GetReturnTypeName(), "TH1*"))
        {
          type = kCorrelTrackHisto;
        }
      }
    }    
    // Correlated tracks analyse macro?
    else if ((f = gROOT->GetGlobalFunctionWithPrototype(name, 
                              "const AliTRDtrackV1*, const AliTRDtrackV1*, Double_t*&, Int_t&", kTRUE)) 
             != 0x0)
    {
      if (!strcmp(f->GetReturnTypeName(), "void"))
      {
        // Some additional check (is the parameter EXACTLY of the desired type?)
        if (strstr(f->GetMangledName(), "oPconstsPAliTRDtrackV1mUsP") != 0x0 &&
            strstr(f->GetMangledName(), "cOconstsPAliTRDtrackV1mUsP") != 0x0 &&
            strstr(f->GetMangledName(), "cODouble_tmUaNsP") != 0x0 &&
            strstr(f->GetMangledName(), "cOInt_taNsP") != 0x0)
        {
          type = kCorrelTrackAnalyse;
        }
      }
    }    
  }
  // Use list to look up the macro type
  else
  {
    TObjString* objEntry = 0;
    objEntry = (TObjString*)fMacroTypes->GetValue(entry);
    if (objEntry == 0)  return kUnknown; 
    
    type = (AliEveTRDTrackListMacroType)objEntry->GetString().Atoi();
    switch (type)
    {
      case kSingleTrackSelect:
      case kSingleTrackAnalyse:
      case kSingleTrackHisto:
      case kCorrelTrackSelect:
      case kCorrelTrackAnalyse:
      case kCorrelTrackHisto:      
        break;
    default:
      type = kUnknown;
      break;
    }
  }

  return type;
}

//______________________________________________________
Char_t* AliEveTRDTrackList::MakeMacroEntry(const Char_t* path, const Char_t* name) const
{
  // Constructs an entry for the macro lists with path and name.  

  Char_t* entry = new Char_t[(fkMaxMacroPathNameLength + 30)];
  memset(entry, '\0', sizeof(Char_t) * (fkMaxMacroPathNameLength + 30));

  Char_t* systemPath = gSystem->ExpandPathName(path);
  sprintf(entry, "%s (Path: %s)", name, systemPath);
  delete systemPath;
  systemPath = 0;

  return entry;
}

//______________________________________________________
void AliEveTRDTrackList::RemoveProcessMacros(const TList* iterator) 
{
  // Uses the iterator (for the selected process macros) to remove the process macros from 
  // the corresponding list.
   
  TObjString* obj = 0;
  for (Int_t i = 0; i < iterator->GetEntries(); i++)
  {
    // Key and value will be deleted, too, since fMacroTypes is the owner of them
    fMacroTypes->DeleteEntry(fMacroTypes->FindObject(iterator->At(i)->GetTitle()));

    obj = (TObjString*)fMacroList->Remove(fMacroList->FindObject(iterator->At(i)->GetTitle()));   
    if (obj != 0) delete obj;
  }
  obj = 0;
}

//______________________________________________________
void AliEveTRDTrackList::RemoveSelectionMacros(const TList* iterator) 
{
  // Uses the iterator (for the selected selection macros) to remove the selection macros from 
  // the corresponding list.
  
  TObjString* obj = 0;
  for (Int_t i = 0; i < iterator->GetEntries(); i++)
  {
    // Key and value will be deleted, too, since fMacroTypes is the owner of them
    fMacroTypes->DeleteEntry(fMacroTypes->FindObject(iterator->At(i)->GetTitle()));

    obj = (TObjString*)fMacroSelList->Remove(fMacroSelList->FindObject(iterator->At(i)->GetTitle()));
    if (obj != 0) delete obj;
  }
  obj = 0;
}

//______________________________________________________
void AliEveTRDTrackList::UpdateTrackStyle(AliEveTRDTrack::AliEveTRDTrackState s, UChar_t ss)
{
  // Updates the track style and sets this style for each track.

  switch(s)
  {
    case AliEveTRDTrack::kSource:
      SETBIT(fSelectedStyle, AliEveTRDTrack::kSource);
      break;  
    case AliEveTRDTrack::kPID:
      CLRBIT(fSelectedStyle, AliEveTRDTrack::kSource);
      switch(ss)
      {
      case AliTRDReconstructor::kLQPID:
        CLRBIT(fSelectedStyle, AliEveTRDTrack::kPID);
        break;
      case AliTRDReconstructor::kNNPID:
        SETBIT(fSelectedStyle, AliEveTRDTrack::kPID);
        break;
      }
      break;  
    case AliEveTRDTrack::kTrackCosmics:
      SETBIT(fSelectedStyle, AliEveTRDTrack::kTrackCosmics);
      break;  
    case AliEveTRDTrack::kTrackModel:
      CLRBIT(fSelectedStyle, AliEveTRDTrack::kTrackCosmics);
      switch(ss)
      {
      case AliEveTRDTrack::kRieman:
        CLRBIT(fSelectedStyle, AliEveTRDTrack::kTrackModel);
        break;
      case AliEveTRDTrack::kKalman:
        AliWarning("Kalman fit under testing for the moment.");
        //SETBIT(fSelectedStyle, AliEveTRDTrack::kTrackModel);
        break;
      }
      break;  
  }


  // Walk through the list of tracks     
  AliEveTRDTrack* track = 0x0;
  for (TEveElement::List_i iter = this->BeginChildren(); iter != this->EndChildren(); ++iter) 
  {
    if (!(track = dynamic_cast<AliEveTRDTrack*>(*iter)))  continue;

    track->SetStatus(fSelectedStyle);
  }
}
