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

#include "AliMUONPainterDataSourceFrame.h"

#include "AliCDBEntry.h"
#include "AliCDBManager.h"
#include "AliCDBStorage.h"
#include "AliLog.h"
#include "AliMUONChamberPainter.h"
#include "AliMUONMchViewApplication.h"
#include "AliMUONPainterDataRegistry.h"
#include "AliMUONPainterDataSourceItem.h"
#include "AliMUONPainterEnv.h"
#include "AliMUONPainterHelper.h"
#include "AliMUONPainterMatrix.h"
#include "AliMUONPainterRegistry.h"
#include "AliMUONRecoParam.h"
#include "AliMUONTrackerConditionDataMaker.h"
#include "AliMUONTrackerDataMaker.h"
#include "AliMUONTrackerDataSourceTypes.h"
#include "AliMUONVTrackerData.h"
#include "AliRawReader.h"
#include "Riostream.h"
#include <cstdlib>
#include <TCanvas.h>
#include <TGButton.h>
#include <TGComboBox.h>
#include <TGFileDialog.h>
#include <TGNumberEntry.h>
#include <TGrid.h>
#include <TGTextEntry.h>
#include <TMath.h>
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
  fRawSelector24(new TGCompositeFrame(fRawSelector2,w,h,kHorizontalFrame)),
  fRawSelector23(new TGCompositeFrame(fRawSelector2,w,h,kHorizontalFrame)),
  fCalibrateNoGain(new TGCheckButton(fRawSelector22,"Ped sub")),
  fHistogramButton(new TGCheckButton(fRawSelector23,"Histogram")),
  fHistoMin(new TGNumberEntry(fRawSelector23,0)),
  fHistoMax(new TGNumberEntry(fRawSelector23,4096)),
  fEventRangeButton(new TGCheckButton(fRawSelector23,"Event range")),
  fEventMin(new TGNumberEntry(fRawSelector23,-1,10)),
  fEventMax(new TGNumberEntry(fRawSelector23,-1,10)),
  fRawOCDBPath(0x0),
  fOCDBSelector(new TGGroupFrame(this,"OCDB Path",kHorizontalFrame)),
  fDataReaders(new TGGroupFrame(this,"Data sources")),
  fFilePath(new TGTextEntry(fRawSelector21,"")),
  fOCDBPath(0x0),
  fRunSelector(new TGNumberEntry(fOCDBSelector,0,10)),
  fOCDBTypes(new TGComboBox(fOCDBSelector)),
  fRecentSources(new TGComboBox(fRecentSourceSelector)),
  fCreateRecentButton(new TGTextButton(fRecentSourceSelector,"Create data source")),
  fItems(new TObjArray),
  fACFSelector(new TGGroupFrame(this,"ASCII Calib File",kHorizontalFrame)),
  fACFPath(new TGTextEntry(fACFSelector,"")),
  fACFTypes(new TGComboBox(fACFSelector))
{
  /// Ctor
  
  AliCDBStorage* storage = AliCDBManager::Instance()->GetDefaultStorage();
  TString uri = storage->GetURI();
  
  fRawOCDBPath = new TGTextEntry(fRawSelector24,uri.Data());
  fOCDBPath = new TGTextEntry(fOCDBSelector,uri.Data());
  
  AliMUONPainterDataRegistry* reg = AliMUONPainterDataRegistry::Instance();
  
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

  fCreateRecentButton->Connect("Clicked()",
                              "AliMUONPainterDataSourceFrame",
                              this,
                              "OpenRecentSource()");
  
  fRecentSourceSelector->AddFrame(fRecentSources,new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));
  fRecentSourceSelector->AddFrame(fCreateRecentButton,new TGLayoutHints(kLHintsTop,5,5,5,5));

  fRecentSources->SetEnabled(kFALSE);
  fCreateRecentButton->SetEnabled(kFALSE);
  
  for ( Int_t i = 0; i < Env()->NumberOfDataSources(); ++i )
  {
    AddRecentSource(Env()->DataSourceDescriptor(i));
  }

  fRecentSources->Resize(100,20);
  
  /// Raw file selection
  
  TGButton* openButton = new TGPictureButton(fRawSelector21,
                                             gClient->GetPicture("fileopen.xpm"));
  openButton->SetToolTipText("Click to open file dialog");
  
  fRawSelector2->AddFrame(fRawSelector21, new TGLayoutHints(kLHintsExpandX,5,5,5,5));
  fRawSelector2->AddFrame(fRawSelector22, new TGLayoutHints(kLHintsExpandX,5,5,5,5));
  fRawSelector2->AddFrame(fRawSelector24, new TGLayoutHints(kLHintsTop,5,5,5,5));
  fRawSelector2->AddFrame(fRawSelector23, new TGLayoutHints(kLHintsExpandX,5,5,5,5));
  
  fRawSelector21->AddFrame(openButton,new TGLayoutHints(kLHintsTop,5,5,5,5));
  fRawSelector21->AddFrame(fFilePath, new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));
  
  fRawSelector22->AddFrame(fCalibrateNoGain, new TGLayoutHints(kLHintsTop,5,5,5,5));
  
  fRawSelector24->AddFrame(fRawOCDBPath, new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));
  fRawOCDBPath->SetEnabled(kFALSE);
  
  fRawSelector23->AddFrame(fHistogramButton,new TGLayoutHints(kLHintsTop,5,5,5,5));
  fHistogramButton->Connect("Clicked()","AliMUONPainterDataSourceFrame",this,"HistogramButtonClicked()");
  fHistoMin->SetState(kFALSE);
  fHistoMax->SetState(kFALSE);
  fRawSelector23->AddFrame(fHistoMin,new TGLayoutHints(kLHintsTop,5,5,5,5));
  fRawSelector23->AddFrame(fHistoMax,new TGLayoutHints(kLHintsTop,5,5,5,5));
  
  
  fRawSelector23->AddFrame(fEventRangeButton,new TGLayoutHints(kLHintsTop,5,5,5,5));
  fEventRangeButton->Connect("Clicked()","AliMUONPainterDataSourceFrame",this,"EventRangeButtonClicked()");
  fEventMin->SetState(kFALSE);
  fEventMax->SetState(kFALSE);
  
  fEventMin->SetFormat(TGNumberFormat::kNESInteger);
  fEventMax->SetFormat(TGNumberFormat::kNESInteger);
  
  fRawSelector23->AddFrame(fEventMin,new TGLayoutHints(kLHintsTop,5,5,5,5));
  fRawSelector23->AddFrame(fEventMax,new TGLayoutHints(kLHintsTop,5,5,5,5));
  
  TGButton* createRawButton = new TGTextButton(fRawSelector,"Create data source");
  
  fRawSelector->AddFrame(fRawSelector2, new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));
  fRawSelector->AddFrame(createRawButton, new TGLayoutHints(kLHintsCenterY,5,5,5,5));
  
  fCalibrateNoGain->Connect("Clicked()","AliMUONPainterDataSourceFrame",this,"CalibrateButtonClicked()");
  
  openButton->Connect("Clicked()",
                      "AliMUONPainterDataSourceFrame",
                      this,
                      "OpenFileDialog()");
  
  createRawButton->Connect("Clicked()",
                           "AliMUONPainterDataSourceFrame",
                           this,
                           "CreateRawDataSource()");
  
  /// OCDB selection
  
  fOCDBTypes->AddEntry(AliMUONTrackerDataSourceTypes::ShortNameForConfig(),7);
  fOCDBTypes->AddEntry(AliMUONTrackerDataSourceTypes::ShortNameForOccupancy(),4);
  fOCDBTypes->AddEntry(AliMUONTrackerDataSourceTypes::ShortNameForHV(),3);
  fOCDBTypes->AddEntry(AliMUONTrackerDataSourceTypes::ShortNameForPedestals(),0);
  fOCDBTypes->AddEntry(AliMUONTrackerDataSourceTypes::ShortNameForStatus(),5);
  fOCDBTypes->AddEntry(AliMUONTrackerDataSourceTypes::ShortNameForStatusMap(),6);
  fOCDBTypes->AddEntry(AliMUONTrackerDataSourceTypes::ShortNameForRejectList(),8);
  fOCDBTypes->Select(0);
  fOCDBTypes->Resize(80,20);
  
  TGButton* createOCDBButton = new TGTextButton(fOCDBSelector,"Create data source");
  createOCDBButton->Connect("Clicked()",
                            "AliMUONPainterDataSourceFrame",
                            this,
                            "CreateOCDBDataSource()");
  
  const char* ocdbToolTip = "Use URL style for either alien or local OCDB (foo://bar). For example :\n"
  "alien://folder=/alice/data.../OCDB\n"
  "or\nlocal:///home/user/aliroot (mind the 3 slashes there !)";
  
  fRawOCDBPath->SetToolTipText(ocdbToolTip);
  fOCDBPath->SetToolTipText(ocdbToolTip);
  
  fOCDBSelector->AddFrame(fOCDBPath,new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));
  fOCDBSelector->AddFrame(fRunSelector,new TGLayoutHints(kLHintsTop,5,5,5,5));
  fOCDBSelector->AddFrame(fOCDBTypes,new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));
  fOCDBSelector->AddFrame(createOCDBButton,new TGLayoutHints(kLHintsTop,5,5,5,5));
  
  
  /// ASCII calibration file selection
  
  TGButton* openButtonACF = new TGPictureButton(fACFSelector,
                                                gClient->GetPicture("fileopen.xpm"));
  openButtonACF->SetToolTipText("Click to open file dialog");
  
  fACFTypes->AddEntry(AliMUONTrackerDataSourceTypes::ShortNameForConfig(),7);
  fACFTypes->AddEntry(AliMUONTrackerDataSourceTypes::ShortNameForOccupancy(),4);
  fACFTypes->AddEntry(AliMUONTrackerDataSourceTypes::ShortNameForPedestals(),0);
  fACFTypes->Select(0);
  fACFTypes->Resize(100,20);
  
  fACFSelector->AddFrame(openButtonACF,new TGLayoutHints(kLHintsTop,5,5,5,5));
  fACFSelector->AddFrame(fACFPath, new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));
  fACFSelector->AddFrame(fACFTypes,new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));
  
  TGButton* createACFButton = new TGTextButton(fACFSelector,"Create data source");
  createACFButton->Connect("Clicked()",
                           "AliMUONPainterDataSourceFrame",
                           this,
                           "CreateACFDataSource()");
  
  openButtonACF->Connect("Clicked()",
                         "AliMUONPainterDataSourceFrame",
                         this,
                         "OpenFileDialogACF()");
  
  fACFSelector->AddFrame(createACFButton,new TGLayoutHints(kLHintsTop,5,5,5,5));
  
  AddFrame(fRecentSourceSelector,new TGLayoutHints(kLHintsExpandX,10,10,10,10));
  
  AddFrame(fRawSelector,new TGLayoutHints(kLHintsExpandX,10,10,10,10));
  
  AddFrame(fOCDBSelector,new TGLayoutHints(kLHintsExpandX,10,10,10,10));
  
  AddFrame(fACFSelector,new TGLayoutHints(kLHintsExpandX,10,10,10,10));
  
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
  
  fRecentSources->SetEnabled(kTRUE);
  fCreateRecentButton->SetEnabled(kTRUE);

  fRecentSources->AddEntry(name,lb->GetNumberOfEntries());
  fRecentSources->MapSubwindows();
  fRecentSources->Layout();
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::CalibrateButtonClicked()
{
  /// Calibrate button was clicked.
  
  if ( fCalibrateNoGain->IsOn() )
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
AliMUONPainterDataSourceFrame::CreateACFDataSource()
{
  /// Create an ACF data source (using information from the widgets)
  
  TString acfPath = fACFPath->GetText();
  TGTextLBEntry* t = static_cast<TGTextLBEntry*>(fACFTypes->GetSelectedEntry());
  TString type = t->GetText()->GetString();
  
  CreateACFDataSource(acfPath,type);
  
  fACFPath->SetText("");
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::CreateACFDataSource(const TString& acfPath, const TString& type)
{
  /// Create an ACF data source for a given (path,type)
  
  AliMUONVTrackerDataMaker* reader = new AliMUONTrackerConditionDataMaker(acfPath.Data(),
                                                                          type.Data());
  
  RegisterDataSource(reader,Form("FILE;%s;%s",acfPath.Data(),type.Data()));
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::CreateACFDataSource(const TString& uri)
{
  /// Create an ACF data source, given it's URI
  
  TObjArray* a = uri.Tokenize(";");
  TString acfPath = static_cast<TObjString*>(a->At(1))->String();
  TString type = static_cast<TObjString*>(a->At(2))->String();
  
  CreateACFDataSource(acfPath,type);
  
  delete a;
}

//_____________________________________________________________________________
AliMUONPainterMatrix*
AliMUONPainterDataSourceFrame::CreateFullTracker(AliMUONVTrackerData* data,
                                                 Int_t dim,
                                                 Double_t xmin, Double_t xmax,
                                                 const AliMUONAttPainter& att)
{
  /// Generate, draw and register a matrix of 10 painters to show all the tracker
  /// chambers
  
  AliMUONPainterMatrix* matrix = new AliMUONPainterMatrix("Tracker",5,2);
  
  for ( Int_t ichamber = 0; ichamber < 10; ++ichamber )
  {
    AliMUONVPainter* painter = new AliMUONChamberPainter(att,ichamber);
    
    painter->SetResponder("BUSPATCH");
    
    painter->SetOutlined("*",kFALSE);
    
    matrix->Adopt(painter);
  }
  
  matrix->SetData("MANU",data,dim);
  matrix->SetDataRange(xmin,xmax);
  
  AliMUONPainterRegistry::Instance()->Register(matrix);
  
  return matrix;
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
  
  CreateOCDBDataSource(cdbPath,runNumber,type,"");
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::CreateOCDBDataSource(const TString& dataSourceDescriptor)
{
  /// Create an OCDB data source, given it's full descriptor

  TString sid = Env()->Descriptor2ID(dataSourceDescriptor);
  
  TString ranges = Env()->Descriptor2Ranges(dataSourceDescriptor);
  
  TString uri = Env()->ID2URI(sid);
  
  TString cdbPath = Env()->TupleFirst(uri,Env()->SeparatorWithinPart());
  
  TString type = Env()->ID2Type(sid);
  
  TString srun = Env()->TupleMiddle(uri,Env()->SeparatorWithinPart());
  
  CreateOCDBDataSource(cdbPath,atoi(srun.Data()),type,ranges);
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::CreateOCDBDataSource(const TString& cdbPath,
                                                    Int_t runNumber,
                                                    const TString& type,
                                                    const TString& ranges)
{
  /// Create an OCDB data source for a given (path,runnumber,type) triplet
  
  AliMUONVTrackerDataMaker* reader = new AliMUONTrackerConditionDataMaker(runNumber,
                                                                          cdbPath.Data(),
                                                                          type.Data());
  if ( reader->Data() )
  {
    TString sourceID="OCDB";
    
    sourceID += Env()->SeparatorWithinPart();
    sourceID += cdbPath;
    sourceID += Env()->SeparatorWithinPart();
    sourceID += Form("%d",runNumber);
    sourceID += Env()->SeparatorWithinPart();
    sourceID += type;
    
    RegisterDataSource(reader,Form("%s%s%s%s%s",sourceID.Data(),
                                   Env()->SeparatorBetweenDescriptorParts(),
                                   reader->Data()->GetName(),
                                   Env()->SeparatorBetweenDescriptorParts(),
                                   ranges.Data()));
  }
  else
  {
    AliError(Form("Could not create OCDB data source OCDB %s %d %s",cdbPath.Data(),runNumber,type.Data()));
  }
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::CreatePedestalCanvases(AliMUONVTrackerData* data,
                                                      Double_t pedMin, Double_t pedMax,
                                                      Double_t sigmaMin, Double_t sigmaMax)
{
  /// Create 4 canvases with the pedestals contained in data
  /// to show mean and sigma, for bending and non bending, with given limits
  
  TList matrices;
  
  AliMUONAttPainter att[2];
  
  att[0].SetViewPoint(kTRUE,kFALSE);
  att[0].SetCathode(kFALSE,kFALSE);
  att[0].SetPlane(kTRUE,kFALSE);
  
  att[1].SetViewPoint(kTRUE,kFALSE);
  att[1].SetCathode(kFALSE,kFALSE);
  att[1].SetPlane(kFALSE,kTRUE);
  
  for ( Int_t iatt = 0; iatt < 2; ++iatt )
  {
    matrices.Add(CreateFullTracker(data,0,pedMin,pedMax,att[iatt]));
    matrices.Add(CreateFullTracker(data,1,sigmaMin,sigmaMax,att[iatt]));
  }
  
  TIter next(&matrices);
  AliMUONPainterMatrix* matrix;
  
  Int_t w = TMath::Nint(gClient->GetDisplayWidth()*0.9);
  Int_t h = TMath::Nint(gClient->GetDisplayHeight()*0.9);
  
  Int_t x[] = { 0, 0, 20 + w/2, 20 + w/2 };
  Int_t y[] = { 0, h/2+30, 0, h/2+30 };
  
  Int_t i(0);
  
  while ( ( matrix = static_cast<AliMUONPainterMatrix*>(next())) )
  {
    TCanvas* c = matrix->CreateCanvas(x[i],y[i],w/2,h/2);
    c->Draw();
    c->SaveAs(Form("%s.png",c->GetName()));
    ++i;
  }
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::CreateRawDataSource()
{
  /// Create a new raw data source (using info from the widgets)
  
  TString uri(gSystem->ExpandPathName(fFilePath->GetText()));
  
  TString name("RAW");
  Bool_t fromMemory(kFALSE);
  
  if ( uri.Contains(TRegexp("^mem")) )
  {
    fromMemory = kTRUE;
  }
  else
  {
    if ( gSystem->AccessPathName(uri.Data()) && !uri.BeginsWith("collection://") )
    {
      AliError(Form("File %s does not exist",uri.Data()));
      fFilePath->SetText("");
      return;
    }
  }
  
  TString calibMode("");
  
  if ( fCalibrateNoGain->IsOn() )
  {
    calibMode = "NOGAIN";
    name = "CALZ";
  }
  
  uri = Form("%s%s%s;%s;%s;%s;%s;%s;%s;%s",
             ( fHistogramButton->IsOn() ? "H":""),
             ( fromMemory ? "M" : ""),
             name.Data(),uri.Data(),
             ( strlen(fRawOCDBPath->GetText()) > 0 ? fRawOCDBPath->GetText() : " "),
             ( calibMode.Length() > 0 ? calibMode.Data() : " "),
             Form("%e",fHistoMin->GetNumber()),
             Form("%e",fHistoMax->GetNumber()),
             Form("%d",(Int_t)(fEventMin->GetIntNumber())),
             Form("%d",(Int_t)(fEventMax->GetIntNumber())));
  
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
  TString emin("-1");
  TString emax("-1");
  
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
  
  if ( a->GetLast() > 5 )
  {
    emin = static_cast<TObjString*>(a->At(6))->String();
  }
  
  if ( a->GetLast() > 6 )
  {
    emax = static_cast<TObjString*>(a->At(7))->String();
  }
  
  delete a;
  
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
  /// and to get run numbre
  Int_t runNumber(-1);
  Bool_t ok = rawReader->NextEvent();
  if (!ok)
  {
    AliError(Form("File %s does not seem to be a raw data file",filename.Data()));
    fFilePath->SetText("");
    return kFALSE;
  }
  else
  {
    runNumber = rawReader->GetRunNumber();
  }
  
  rawReader->RewindEvents();
  
  AliMUONVTrackerDataMaker* reader(0x0);
  Bool_t histogram(kFALSE);
  
  if ( uri.Contains(TRegexp("^H")) ) histogram = kTRUE;
  
  if ( ocdbPath.Length() > 0 )
  {
    
    AliMUONRecoParam* recoParam(0x0);
    
    AliCDBEntry* e = AliCDBManager::Instance()->Get("MUON/Calib/RecoParam",runNumber);
    if (e)
    {
      TObject* o = e->GetObject();
      if ( o->IsA() == TObjArray::Class() )
      {
        TIter next(static_cast<TObjArray*>(o));
        AliMUONRecoParam* p;
        while ( ( p = static_cast<AliMUONRecoParam*>(next()) ))
        {
          if ( p->IsDefault()) recoParam = p;
        }
      }
      else
      {
        recoParam = static_cast<AliMUONRecoParam*>(o);
      }
    }
    
    reader = new AliMUONTrackerDataMaker(recoParam,
                                         rawReader,
                                         ocdbPath.Data(),
                                         calibMode.Data(),
                                         histogram,
                                         sxmin.Atof(),
                                         sxmax.Atof());
  }
  else
  {
    reader = new AliMUONTrackerDataMaker(rawReader,histogram);
  }
  
  reader->SetEventRange(emin.Atoi(),emax.Atoi());
  
  reader->SetSource(filename.Data());
  
  TString dsName(uri);
  
  if ( emin.Atoi() <= emax.Atoi() )
  {
    // we have an event range
    if ( emin.Atoi() == emax.Atoi())
    {
      dsName += Form("[%d]",emin.Atoi());
    }
    else
    {
      dsName += Form("[%d,%d]",emin.Atoi(),emax.Atoi());
    }
  }
  
  RegisterDataSource(reader,dsName.Data());
  
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
                "StartRunning()");
  
  item->Connect("StopRunning()",
                "AliMUONPainterDataSourceFrame",
                this,
                "StopRunning()");
  
  fDataReaders->AddFrame(item);
  
  fItems->Add(item);
  
  fDataReaders->MapSubwindows();
  fDataReaders->Resize();
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::DataMakerWasUnregistered(const AliMUONVTrackerDataMaker* maker)
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
AliMUONPainterEnv*
AliMUONPainterDataSourceFrame::Env()
{
  return AliMUONPainterHelper::Instance()->Env();
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::EventRangeButtonClicked()
{
  /// EventRange button was clicked.
  
  if ( fEventRangeButton->IsOn() )
  {
    fEventMin->SetState(kTRUE);
    fEventMax->SetState(kTRUE);
  }
  else
  {
    fEventMin->SetIntNumber(-1);
    fEventMax->SetIntNumber(-1);
    fEventMin->SetState(kFALSE);
    fEventMax->SetState(kFALSE);
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

  fileInfo.fIniDir = StrDup(Env()->String("LastOpenDir","."));
  
  new TGFileDialog(gClient->GetRoot(),gClient->GetRoot(),
                   kFDOpen,&fileInfo);
  
  fFilePath->SetText(gSystem->ExpandPathName(Form("%s",fileInfo.fFilename)));
  
  Env()->Set("LastOpenDir",fileInfo.fIniDir);
  Env()->Save();
}


//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::OpenFileDialogACF()
{
  /// Open a file dialog to select an ASCII calibration file to be read
  
  TGFileInfo fileInfo;
  
  const char* fileTypes[] = { 
    "All files","*",
    0,0 };
  
  fileInfo.fFileTypes = fileTypes;
  delete[] fileInfo.fIniDir;
  
  fileInfo.fIniDir = StrDup(Env()->String("LastOpenDirACF","."));
  
  new TGFileDialog(gClient->GetRoot(),gClient->GetRoot(),
                   kFDOpen,&fileInfo);
  
  fACFPath->SetText(gSystem->ExpandPathName(Form("%s",fileInfo.fFilename)));
  
  Env()->Set("LastOpenDirACF",fileInfo.fIniDir);
  Env()->Save();
}


//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::OpenRecentSource()
{
  /// Open one source from the recently used ones
  
  TGTextLBEntry* t = (TGTextLBEntry*)fRecentSources->GetSelectedEntry();

  TString dataSourceDescriptor(t->GetText()->GetString());
  
  TString name = Env()->Descriptor2Name(dataSourceDescriptor);
  
  if (AliMUONPainterDataRegistry::Instance()->DataSource(name))
  {
    // source already registered.
    return;
  }
  
  TString sid = Env()->Descriptor2ID(dataSourceDescriptor);
  
  TString origin = Env()->ID2Origin(sid);
  
  if ( origin == "OCDB" )
  {
    CreateOCDBDataSource(dataSourceDescriptor);
  }
  else if ( origin == "RAW" )
  {
    CreateRawDataSource(dataSourceDescriptor);
  }
  else if ( origin == "ACF" )
  {
    CreateACFDataSource(dataSourceDescriptor);
  }
  
  fRecentSources->Select(-1);
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::RegisterDataSource(AliMUONVTrackerDataMaker* reader,
                                                  const char* dataSourceDescriptor)
{
  /// Register a *new* data source
  
  if ( reader && reader->IsValid() )
  {
    AliMUONMchViewApplication* app = dynamic_cast<AliMUONMchViewApplication*>(gApplication);
    if (!app)
    {
      AliError("Could not cast application to the expected type ! CHECK THAT !");
    }
    
    AliMUONPainterDataRegistry::Instance()->Register(reader);
    
    TString desc = Env()->AddDataSource(dataSourceDescriptor);
    
    AddRecentSource(desc.Data());
    
    if ( app )
    {
      TString sid = Env()->Descriptor2ID(desc.Data());
      TString type = Env()->ID2Type(sid);
      
      if ( AliMUONTrackerDataSourceTypes::IsPedestals(type) )
      {
        if ( Env()->Integer("disableAutoPedCanvas",0)==0)
        {
          CreatePedestalCanvases(reader->Data());
        }
      }
    }
  }  
}

//_____________________________________________________________________________
void
AliMUONPainterDataSourceFrame::StartRunning()
{
  /// One data source starts running. Disable the Run button of the other ones
  
  AliMUONPainterDataSourceItem* item = reinterpret_cast<AliMUONPainterDataSourceItem*> (gTQSender);
  
  AliInfo("");
  
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
AliMUONPainterDataSourceFrame::StopRunning()
{
  /// One data source stops running. Enable the Run button of all items
  
  TIter next(fItems);
  AliMUONPainterDataSourceItem* o;
  while ( ( o = static_cast<AliMUONPainterDataSourceItem*>(next()) ) )
  {
    o->EnableRun();
  }
}
  
