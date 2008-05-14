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

// $Id$

#include <cstdlib>
#include "AliMUONPainterDataSourceFrame.h"


#include "AliLog.h"
#include "AliMUONPainterDataSourceItem.h"
#include "AliMUONPainterEnv.h"
#include "AliMUONPainterHelper.h"
#include "AliMUONPainterRegistry.h"
#include "AliMUONTrackerCalibratedDataMaker.h"
#include "AliMUONTrackerOCDBDataMaker.h"
#include "AliMUONTrackerRawDataMaker.h"
#include "AliRawReader.h"
#include <TGButton.h>
#include <TGComboBox.h>
#include <TGFileDialog.h>
#include <TGNumberEntry.h>
#include <TGTextEntry.h>
#include <TGrid.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TRegexp.h>
#include <TString.h>
#include <TSystem.h>

///\class AliMUONPainterDataSourceFrame
///
/// A complete frame to select and display various data sources to 
/// be displayed : either raw data or OCDB data. 
/// Later on we might add digits and clusters for instance.
///
///\author Laurent Aphecetche, Subatech

const char* AliMUONPainterDataSourceFrame::fgkNumberOfDataSourcesKey = "NumberOfDataSources";
const char* AliMUONPainterDataSourceFrame::fgkDataSourceURIKey = "DataSourceURI.%d";

///\cond CLASSIMP
ClassImp(AliMUONPainterDataSourceFrame)
///\endcond

//_____________________________________________________________________________
AliMUONPainterDataSourceFrame::AliMUONPainterDataSourceFrame(const TGWindow* p, UInt_t w, UInt_t h)
: TGCompositeFrame(p,w,h,kVerticalFrame),
  fRecentSourceSelector(new TGGroupFrame(this,"Recent sources",kHorizontalFrame)),
  fRawSelector(new TGGroupFrame(this,"Raw file URI",kHorizontalFrame)),
  fRawSelector2(new TGCompositeFrame(fRawSelector,w,h,kVerticalFrame)),
  fRawSelector21(new TGCompositeFrame(fRawSelector2,w,h,kHorizontalFrame)),
  fRawSelector22(new TGCompositeFrame(fRawSelector2,w,h,kHorizontalFrame)),
  fRawSelector23(new TGCompositeFrame(fRawSelector2,w,h,kHorizontalFrame)),
  fCalibrateNoGain(new TGCheckButton(fRawSelector22,"Ped subraction")),
  fCalibrateGainConstantCapa(new TGCheckButton(fRawSelector22,"Ped subraction + gain (capa cste)")),
  fCalibrateGain(new TGCheckButton(fRawSelector22,"Full calib (Ped subraction + gain with capa)")),
  fHistogramButton(new TGCheckButton(fRawSelector23,"Histogram")),
  fHistoMin(new TGNumberEntry(fRawSelector23,0)),
  fHistoMax(new TGNumberEntry(fRawSelector23,4096)),
  fRawOCDBPath(new TGTextEntry(fRawSelector22,"")),
  fOCDBSelector(new TGGroupFrame(this,"OCDB Path",kHorizontalFrame)),
  fDataReaders(new TGGroupFrame(this,"Data sources")),
  fFilePath(new TGTextEntry(fRawSelector21,"")),
  fOCDBPath(new TGTextEntry(fOCDBSelector,"")),
  fRunSelector(new TGNumberEntry(fOCDBSelector,0)),
  fOCDBTypes(new TGComboBox(fOCDBSelector)),
  fRecentSources(new TGComboBox(fRecentSourceSelector)),
  fItems(new TObjArray)
{
  /// Ctor
  
    AliMUONPainterRegistry* reg = AliMUONPainterRegistry::Instance();
    
    reg->Connect("DataMakerWasRegistered(AliMUONVTrackerDataMaker*)",
                 "AliMUONPainterDataSourceFrame",
                 this,
                 "DataMakerWasRegistered(AliMUONVTrackerDataMaker*)");
    
    reg->Connect("DataMakerWasUnregistered(AliMUONVTrackerDataMaker*)",
                 "AliMUONPainterDataSourceFrame",
                 this,
                 "DataMakerWasUnregistered(AliMUONVTrackerDataMaker*)");
    
    fItems->SetOwner(kFALSE);
    
    /// Recent source selection
    
    AliMUONPainterEnv* env = AliMUONPainterHelper::Instance()->Env();
    
    Int_t nsources = env->Integer(fgkNumberOfDataSourcesKey);
    
    for ( Int_t i = 0; i < nsources; ++i )
    {
      AddRecentSource(env->String(Form(fgkDataSourceURIKey,i)));
    }

    fRecentSources->Resize(100,20);
    
    TGButton* createRecentButton = new TGTextButton(fRecentSourceSelector,"Create data source");
    createRecentButton->Connect("Clicked()",
                                "AliMUONPainterDataSourceFrame",
                                this,
                                "OpenRecentSource()");
    
    fRecentSourceSelector->AddFrame(fRecentSources,new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));
    fRecentSourceSelector->AddFrame(createRecentButton,new TGLayoutHints(kLHintsTop,5,5,5,5));
                                    
    /// Raw file selection
    
    TGButton* openButton = new TGPictureButton(fRawSelector21,
                                           gClient->GetPicture("fileopen.xpm"));
    openButton->SetToolTipText("Click to open file dialog");
                                        
    fRawSelector2->AddFrame(fRawSelector21, new TGLayoutHints(kLHintsExpandX,5,5,5,5));
    fRawSelector2->AddFrame(fRawSelector22, new TGLayoutHints(kLHintsExpandX,5,5,5,5));
    fRawSelector2->AddFrame(fRawSelector23, new TGLayoutHints(kLHintsExpandX,5,5,5,5));

    fRawSelector21->AddFrame(openButton,new TGLayoutHints(kLHintsTop,5,5,5,5));
    fRawSelector21->AddFrame(fFilePath, new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));

    fRawSelector22->AddFrame(fCalibrateNoGain, new TGLayoutHints(kLHintsTop,5,5,5,5));
    fRawSelector22->AddFrame(fCalibrateGainConstantCapa, new TGLayoutHints(kLHintsTop,5,5,5,5));
    fRawSelector22->AddFrame(fCalibrateGain, new TGLayoutHints(kLHintsTop,5,5,5,5));
    fRawSelector22->AddFrame(fRawOCDBPath, new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));
    fRawOCDBPath->SetEnabled(kFALSE);
    
    fRawSelector23->AddFrame(fHistogramButton,new TGLayoutHints(kLHintsTop,5,5,5,5));
    
    fHistogramButton->Connect("Clicked()","AliMUONPainterDataSourceFrame",this,"HistogramButtonClicked()");
    
    fHistoMin->SetState(kFALSE);
    fHistoMax->SetState(kFALSE);
    
    fRawSelector23->AddFrame(fHistoMin,new TGLayoutHints(kLHintsTop,5,5,5,5));
    fRawSelector23->AddFrame(fHistoMax,new TGLayoutHints(kLHintsTop,5,5,5,5));
    
    TGButton* createRawButton = new TGTextButton(fRawSelector,"Create data source");
    
    fRawSelector->AddFrame(fRawSelector2, new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));
    fRawSelector->AddFrame(createRawButton, new TGLayoutHints(kLHintsCenterY,5,5,5,5));
        
    fCalibrateNoGain->Connect("Clicked()","AliMUONPainterDataSourceFrame",this,"CalibrateButtonClicked()");
    fCalibrateGainConstantCapa->Connect("Clicked()","AliMUONPainterDataSourceFrame",this,"CalibrateButtonClicked()");
    fCalibrateGain->Connect("Clicked()","AliMUONPainterDataSourceFrame",this,"CalibrateButtonClicked()");

    openButton->Connect("Clicked()",
                        "AliMUONPainterDataSourceFrame",
                        this,
                        "OpenFileDialog()");

    createRawButton->Connect("Clicked()",
                        "AliMUONPainterDataSourceFrame",
                        this,
                        "CreateRawDataSource()");
    
    /// OCDB selection
    
    fOCDBTypes->AddEntry("Pedestals",0);
    fOCDBTypes->AddEntry("Gains",1);
    fOCDBTypes->AddEntry("Capacitances",2);
    fOCDBTypes->Select(0);
    fOCDBTypes->Resize(100,20);
    
    TGButton* createOCDBButton = new TGTextButton(fOCDBSelector,"Create data source");
    createOCDBButton->Connect("Clicked()",
                             "AliMUONPainterDataSourceFrame",
                             this,
                             "CreateOCDBDataSource()");
    
    
    fOCDBSelector->AddFrame(fOCDBPath,new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));    
    fOCDBSelector->AddFrame(fRunSelector,new TGLayoutHints(kLHintsTop,5,5,5,5));
    fOCDBSelector->AddFrame(fOCDBTypes,new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));
    fOCDBSelector->AddFrame(createOCDBButton,new TGLayoutHints(kLHintsTop,5,5,5,5));

    AddFrame(fRecentSourceSelector,new TGLayoutHints(kLHintsExpandX,10,10,10,10));

    AddFrame(fRawSelector,new TGLayoutHints(kLHintsExpandX,10,10,10,10));

    AddFrame(fOCDBSelector,new TGLayoutHints(kLHintsExpandX,10,10,10,10));

    AddFrame(fDataReaders, new TGLayoutHints(kLHintsExpandX,10,10,10,10));
    
}

//_____________________________________________________________________________
AliMUONPainterDataSourceFrame::~AliMUONPainterDataSourceFrame()
{
  /// dtor
  
  delete fItems;
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::AddRecentSource(const char* name)
{  
  /// Add a source to the list of recently used sources
  
  TGListBox* lb = fRecentSources->GetListBox();
  
  for ( Int_t i = 0; i < lb->GetNumberOfEntries(); ++i ) 
  {
    TGTextLBEntry* t = (TGTextLBEntry*)lb->GetEntry(i);
    TString s(t->GetText()->GetString());
    if ( s == name ) 
    {
      return;
    }
  }
  
  fRecentSources->AddEntry(name,lb->GetNumberOfEntries());
  fRecentSources->MapSubwindows();
  fRecentSources->Layout();
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::CalibrateButtonClicked()
{
  /// Calibrate button was clicked.
  
  if ( fCalibrateNoGain->IsOn() ||
       fCalibrateGainConstantCapa->IsOn() ||
       fCalibrateGain->IsOn() ) 
  {
    fRawOCDBPath->SetEnabled(kTRUE);
    fRawOCDBPath->SetFocus();
  }
  else
  {
    fRawOCDBPath->SetEnabled(kFALSE);
  }
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::HistogramButtonClicked()
{
  /// Histogram button was clicked.
  
  if ( fHistogramButton->IsOn() )
  {
    fHistoMin->SetState(kTRUE);
    fHistoMax->SetState(kTRUE);
  }
  else
  {
    fHistoMin->SetState(kFALSE);
    fHistoMax->SetState(kFALSE);
  }
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::CreateOCDBDataSource()
{
  /// Create an OCDB data source (using information from the widgets)
  
  TString cdbPath = fOCDBPath->GetText();
  Int_t runNumber = fRunSelector->GetIntNumber();
  TGTextLBEntry* t = static_cast<TGTextLBEntry*>(fOCDBTypes->GetSelectedEntry());
  TString type = t->GetText()->GetString();
  
  CreateOCDBDataSource(cdbPath,runNumber,type);
  
  fOCDBPath->SetText("");
  fRunSelector->SetNumber(0);  
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::CreateOCDBDataSource(const TString& uri)
{
  /// Create an OCDB data source, given it's URI
  
  TObjArray* a = uri.Tokenize(";");
  TString cdbPath = static_cast<TObjString*>(a->At(1))->String();
  TString srun = static_cast<TObjString*>(a->At(2))->String();
  TString type = static_cast<TObjString*>(a->At(3))->String();
  
  CreateOCDBDataSource(cdbPath,atoi(srun.Data()),type);
  
  delete a;
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::CreateOCDBDataSource(const TString& cdbPath,
                                                    Int_t runNumber,
                                                    const TString& type)
{
  /// Create an OCDB data source for a given (path,runnumber,type) triplet
  
  AliMUONVTrackerDataMaker* reader = new AliMUONTrackerOCDBDataMaker(cdbPath.Data(),
                                                                       runNumber,
                                                                       type.Data());
  
  if ( reader->IsValid() ) 
  {
    AliMUONPainterRegistry::Instance()->Register(reader);
    
    AliMUONPainterEnv* env = AliMUONPainterHelper::Instance()->Env();
    
    Int_t n = env->Integer(fgkNumberOfDataSourcesKey);
    
    env->Set(fgkNumberOfDataSourcesKey,n+1);
    
    TString ds(Form("OCDB;%s;%d;%s",cdbPath.Data(),runNumber,type.Data()));
    
    env->Set(Form(fgkDataSourceURIKey,n),ds.Data());
    
    env->Save();
    
    AddRecentSource(ds.Data());
  }
}

//_____________________________________________________________________________
void 
AliMUONPainterDataSourceFrame::CreateRawDataSource()
{
  /// Create a new raw data source (using info from the widgets)
  
  TString uri(gSystem->ExpandPathName(fFilePath->GetText()));
  
  if ( gSystem->AccessPathName(uri.Data()) )
  {
    AliError(Form("File %s does not exist",uri.Data()));
    fFilePath->SetText("");
    return;
  }

  TString calibMode("");
  TString name("RAW");
  
  if ( fCalibrateGain->IsOn() ) 
  {
    calibMode = "GAIN";
    name = "CALC";
  }
  
  if ( fCalibrateGainConstantCapa->IsOn() ) 
  {
    calibMode = "GAINCONSTANTCAPA";
    name = "CALG";
  }
  
  if ( fCalibrateNoGain->IsOn() ) 
  {
    calibMode = "NOGAIN";
    name = "CALZ";
  }
  
  uri = Form("%s%s;%s;%s;%s;%s;%s",
             ( fHistogramButton->IsOn() ? "H":""),
             name.Data(),uri.Data(),
             ( strlen(fRawOCDBPath->GetText()) > 0 ? fRawOCDBPath->GetText() : " "),
             ( calibMode.Length() > 0 ? calibMode.Data() : " "),
             Form("%e",fHistoMin->GetNumber()),
             Form("%e",fHistoMax->GetNumber()));
  
  if ( CreateRawDataSource(uri) )
  {
    fFilePath->SetText("");
    fRawOCDBPath->SetText("");
  }
}

//_____________________________________________________________________________
Bool_t 
AliMUONPainterDataSourceFrame::CreateRawDataSource(const TString& uri)
{
  /// Create a new raw data source, given its URI
  
  TString filename;
  TString ocdbPath;
  TString calibMode;
  TString sxmin("0.0");
  TString sxmax("4096.0");
  
  TObjArray* a = uri.Tokenize(";");
  
  filename = static_cast<TObjString*>(a->At(1))->String();
  
  if ( a->GetLast() > 1 ) 
  {
    ocdbPath = static_cast<TObjString*>(a->At(2))->String();
    if ( ocdbPath == " " ) ocdbPath = "";
  }

  if ( a->GetLast() > 2 ) 
  {
    calibMode = static_cast<TObjString*>(a->At(3))->String();
    if ( calibMode == " " ) calibMode = "";
  }

  if ( a->GetLast() > 3 ) 
  {
    sxmin = static_cast<TObjString*>(a->At(4))->String();
  }

  if ( a->GetLast() > 4 ) 
  {
    sxmax = static_cast<TObjString*>(a->At(5))->String();
  }
  
  AliRawReader* rawReader = 0x0;

  if ( filename.Contains(TRegexp("^alien")) )
  {
    // insure we've initialized the grid...
    if (!gGrid)
    {
      TGrid::Connect("alien://");
    }
  }
  
  rawReader = AliRawReader::Create(filename.Data());

  if (!rawReader)
  {
    AliError(Form("Could not open file %s",filename.Data()));
    fFilePath->SetText("");
    return kFALSE;
  }
  
  /// Basic test to see if the file is correct
  Bool_t ok = rawReader->NextEvent();
  if (!ok)
  {
    AliError(Form("File %s does not seem to be a raw data file",filename.Data()));
    fFilePath->SetText("");
    return kFALSE;
  }
  
  rawReader->RewindEvents();
  
  AliMUONVTrackerDataMaker* reader(0x0);
  Bool_t histogram(kFALSE);
  
  if ( uri.Contains(TRegexp("^H")) ) histogram = kTRUE;

  if ( ocdbPath.Length() > 0 ) 
  {
    reader = new AliMUONTrackerCalibratedDataMaker(rawReader,ocdbPath.Data(),
                                                   calibMode.Data(),
                                                   histogram,
                                                   sxmin.Atof(),
                                                   sxmax.Atof());
  }
  else
  {
    reader = new AliMUONTrackerRawDataMaker(rawReader,histogram);
  }
  
  reader->SetSource(filename.Data());
  
  AliMUONPainterRegistry::Instance()->Register(reader);
  
  AliMUONPainterEnv* env = AliMUONPainterHelper::Instance()->Env();
  
  Int_t n = env->Integer(fgkNumberOfDataSourcesKey);
  
  env->Set(fgkNumberOfDataSourcesKey,n+1);
  
  env->Set(Form(fgkDataSourceURIKey,n),uri.Data());
  
  AddRecentSource(uri.Data());
  
  env->Save();

  return kTRUE;
}

//_____________________________________________________________________________
void 
AliMUONPainterDataSourceFrame::DataMakerWasRegistered(AliMUONVTrackerDataMaker* reader)
{
  /// Update ourselves as a new data reader was created
  
  AliMUONPainterDataSourceItem* item = new AliMUONPainterDataSourceItem(fDataReaders,100,20,reader);
      
  item->Connect("StartRunning()",
                "AliMUONPainterDataSourceFrame",
                this,
                Form("StartRunning(=(AliMUONPainterDataSourceItem*)(0x%x))",item));

  item->Connect("StopRunning()",
                "AliMUONPainterDataSourceFrame",
                this,
                Form("StopRunning(=(AliMUONPainterDataSourceItem*)(0x%x))",item));
  
  fDataReaders->AddFrame(item);
  
  fItems->Add(item);

  fDataReaders->MapSubwindows();
  fDataReaders->Resize();
}

//_____________________________________________________________________________
void 
AliMUONPainterDataSourceFrame::DataMakerWasUnregistered(AliMUONVTrackerDataMaker* maker)
{
  /// Update ourselves as a data reader was deleted
  
  AliMUONPainterDataSourceItem* theItem(0x0);
  
  TIter next(fItems);
  AliMUONPainterDataSourceItem* item;
  
  while ( ( item = static_cast<AliMUONPainterDataSourceItem*>(next()) ) && !theItem )
  {
    if ( item->DataMaker() == maker ) 
    {
      theItem = item;
    }
  }
  
  if  (!theItem) return;
  
  fDataReaders->RemoveFrame(theItem);
  fItems->Remove(theItem);
  theItem->DestroyWindow();
  delete theItem;
  
  fDataReaders->MapSubwindows();
  fDataReaders->Resize();

}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::OpenFileDialog()
{
  /// Open a file dialog to select a file to be read
  
  TGFileInfo fileInfo;
  
  const char* fileTypes[] = { 
    "ROOT files","*.root",
    "DATE files","*.raw",
    "All files","*",
    0,0 };
  
  fileInfo.fFileTypes = fileTypes;
  delete[] fileInfo.fIniDir;

  AliMUONPainterEnv* env = AliMUONPainterHelper::Instance()->Env();
  
  fileInfo.fIniDir = StrDup(env->String("LastOpenDir","."));
  
  new TGFileDialog(gClient->GetRoot(),gClient->GetRoot(),
                   kFDOpen,&fileInfo);
  
  fFilePath->SetText(gSystem->ExpandPathName(Form("%s",fileInfo.fFilename)));
  
  env->Set("LastOpenDir",fileInfo.fIniDir);
  env->Save();  
}


//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::OpenRecentSource()
{
  /// Open one source from the recently used ones
  
  TGTextLBEntry* t = (TGTextLBEntry*)fRecentSources->GetSelectedEntry();

  TString uri(t->GetText()->GetString());
  
  if ( uri.Contains(TRegexp("^RAW")) || uri.Contains(TRegexp("^HRAW")) || 
       uri.Contains(TRegexp("^CAL")) || uri.Contains(TRegexp("^HCAL")) )
  {
    CreateRawDataSource(uri);
  }
  else if ( uri.Contains(TRegexp("^OCDB")) )
  {
    CreateOCDBDataSource(uri);
  }
  
  fRecentSources->Select(-1);
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::StartRunning(AliMUONPainterDataSourceItem* item)
{
  /// One data source starts running. Disable the Run button of the other ones
  TIter next(fItems);
  AliMUONPainterDataSourceItem* o;
  while ( ( o = static_cast<AliMUONPainterDataSourceItem*>(next()) ) )
  {
    if ( o != item ) 
    {
      o->DisableRun();
    }
  }
}  

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::StopRunning(AliMUONPainterDataSourceItem* /*item*/)
{
  /// One data source stops running. Enable the Run button of all items
  TIter next(fItems);
  AliMUONPainterDataSourceItem* o;
  while ( ( o = static_cast<AliMUONPainterDataSourceItem*>(next()) ) )
  {
    o->EnableRun();
  }
}
  
