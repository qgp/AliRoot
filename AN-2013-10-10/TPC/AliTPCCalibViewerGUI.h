#ifndef ALITPCCALIBVIEWERGUI_H
#define ALITPCCALIBVIEWERGUI_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id: AliTPCCalibViewerGUI.h,v */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  GUI for the AliTPCCalibViewer                                            //
//  used for the calibration monitor                                         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGButton
#include "TGWidget.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif

#include <TGButton.h>
#include <TGListBox.h>
#include <TGComboBox.h>
#include <TGNumberEntry.h>
#include <TRootEmbeddedCanvas.h>
#include <TGSplitter.h>
#include <TGButtonGroup.h>
#include <TGLabel.h>
#include <TGTab.h>
#include <TString.h>
class TROOTt;
class AliTPCCalibViewer;
class AliTPCPreprocessorOnline;
class TGTextEntry;


// class TGListBox;
// class TGNumberEntry;
// class TGSplitter;
// class TGTab;
// class TGWidget; // ???
// class TGLabel;
// class TGButtonGroup;
// class TGComboBox;
// class TRootEmbeddedCanvas;
// class TGButton;
// class TGRadioButton;
// class TGCheckButton;
// class TGTextEntry;
       
       
class AliTPCCalibViewerGUI : public TGCompositeFrame {
   
public:
   AliTPCCalibViewerGUI(const TGWindow *p, UInt_t w, UInt_t h, char* fileName);  // constructor; fileName specifies the ROOT tree used for drawing
   AliTPCCalibViewerGUI(const AliTPCCalibViewerGUI &c);                          // copy constructor
   AliTPCCalibViewerGUI &operator = (const AliTPCCalibViewerGUI &param);         // assignment operator

   virtual ~AliTPCCalibViewerGUI();
   // virtual void CloseWindow();
   
   void DrawGUI(const TGWindow *p, UInt_t w, UInt_t h);         // to be called by the costructor, here the windows is drawn
   void SetInitialValues();                                     // set the initial button states
   void Initialize(const char* fileName, const char* treeName = "calPads"); // initializes the GUI with default settings and opens tree for drawing
   void Initialize(AliTPCCalibViewer *viewer);                  // initializes the GUI with default settings and opens tree for drawing
   void Reload(){Initialize(fViewer);}                          // reload the viewr after it has been changed, e.g. added a new referenceTree, ...
   void Reset();
   TString* GetDrawString();                                    // create the draw string out of selection
   TString* GetCutString();                                     // create the cut string out of selection
   TString* GetSectorString();                                  // create the sector string out of selection
   AliTPCCalibViewer* GetViewer() {return fViewer;}             // returns the internal AliTPCCalibViewer object, which does the work
   static TObjArray* ShowGUI(const char* fileName = 0);             // initialize and show GUI for presentation, standalone
  //
  TGTextEntry* GetDrawEntry() {return fComboCustom->GetTextEntry();}
  TGTextEntry* GetCutsEntry() {return fComboAddCuts->GetTextEntry();}
  TGTextEntry* GetDrawOptEntry() {return fComboAddDrawOpt->GetTextEntry();}
  TGTextEntry* GetFitEntry()  {return fComboCustomFit->GetTextEntry();}
  //
   void HandleButtonsGeneral(Int_t id = -1); // handles mutual radio button exclusions for general Tab
   void HandleButtons1D(Int_t id = -1);      // handles mutual radio button exclusions for 1D Tab
   void HandleButtonsStat(Int_t id = -1);    // handles statistic check boxes 
   void HandleButtonsCuts(Int_t id = -1);    // handles mutual radio button exclusions for right side
   void HandleButtonsNoRedraw(Int_t id = -1);// handles label & scaling checkboxes without redrawing
   void ReplacePlaceHolders(TString &str);   // replace place holders of the draw variable and normalisation variable
   void DoNewSelection();                    // decides whether to redraw if user makes another selection
   void DoDraw();                            // main method for drawing according to user selection
   void DoFit();                             // main method for fitting
   void DoExport();                          // function to export a CalPad to Cint
   void DoDumpToFile();                      // function to dump a new calib tree to file
   void DoLoadTree();                        // function to load a new calib tree
   void DoExportNorm();                      // function to use a calPad for normalization
   void SavePicture();                       // method for saving
   void GetMinMax();                         // Read current Min & Max from the plot and set it to fTxtSetMin & fTxtSetMax
   void SetMinMaxLabel();                    // Set min, max and label without redrawing
   void ChangeSector();                      // function that is called, when the number of the sector is changed
   void AddFitFunction() const;              // adds the last fit function to the normalization list
   void MouseMove(Int_t event, Int_t x, Int_t y, TObject *selected); 
   void UnchekAllStat();                     // Disable all statistical legend entries, no statistical legend.
   
protected:   
   AliTPCCalibViewer   *fViewer;             // CalibViewer object used for drawing
   AliTPCPreprocessorOnline *fPreprocessor;  // PreprocessorOnline object, used to collect the exported CalPads and to save them into a new calibTree

   TGCompositeFrame    *fContTopBottom;      // container for all GUI elements, vertical divided
   TGCompositeFrame    *fContLCR;            // container for all GUI elements, horizontal divided
   TGCompositeFrame    *fContLeft;           // container for GUI elements on left side
   TGTab               *ftabLeft;            // Tabs on the left side for plot options
   TGCompositeFrame    *ftabLeft0;           // Tab 0 on the left side for general plot options
   TGCompositeFrame    *ftabLeft1;           // Tab 1 on the left side for 1D plot options
   TGTab               *ftabRight;           // Tabs on the right side
   TGCompositeFrame    *fTabRight0;          // Tab 0 on the right side for basic
   TGCompositeFrame    *fTabRight1;          // Tab 1 on the right side for advanced
   TGCompositeFrame    *fContRight;          // container for GUI elements on right side
   TGCompositeFrame    *fContCenter;         // container for GUI elements at the center
   TGCompositeFrame    *fContPlotOpt;        // container for plot options GUI elements
   TGCompositeFrame    *fContDrawOpt;        // container for draw options GUI elements
   TGCompositeFrame    *fContDrawOptSub1D2D; // container for 1D and 2D radio-button
   TGCompositeFrame    *fContNormalized;     // container for normalization options GUI elements
   TGCompositeFrame    *fContCustom;         // container for custom draw command GUI elements
   TGCompositeFrame    *fContCuts;           // container for cut options GUI elements
   TGCompositeFrame    *fContSector;         // container for sector GUI elements
   TGCompositeFrame    *fContAddCuts;        // container for additional cut command GUI elements
   TGCompositeFrame    *fContFit;            // container for fit GUI elements
   TGCompositeFrame    *fContAddFit;         // container for additional fit GUI elements
   TGCompositeFrame    *fContScaling;        // container for scaling GUI elements
   TGCompositeFrame    *fContSetMax;         // container for SetMaximum elements
   TGCompositeFrame    *fContSetMin;         // container for SetMinimum elements
   TGCompositeFrame    *fContAddDrawOpt;     // additional draw options container
   TGListBox           *fListVariables;      // listbox with possible variables
   TGTextButton        *fBtnDraw;            // draw button
   TGTextButton        *fBtnFit;             // fit button
   TGTextButton        *fBtnAddFitFunction;  // button to add fit function to normalization
   TGTextButton        *fBtnGetMinMax;       // GetMinMax-button
   TRootEmbeddedCanvas *fCanvMain;           // main drawing canvas
   TGRadioButton       *fRadioRaw;           // raw radio button
   TGRadioButton       *fRadioNormalized;    // normalized radio button
   TGRadioButton       *fRadioPredefined;    // predefined plot radio button
   TGRadioButton       *fRadioCustom;        // custom radio button
   TGRadioButton       *fRadio1D;            // 1D radio button
   TGRadioButton       *fRadio2D;            // 2D radio button
   TGRadioButton       *fRadioTPC;           // TPC radio button
   TGRadioButton       *fRadioSideA;         // side A radio button
   TGRadioButton       *fRadioSideC;         // side C radio button
   TGRadioButton       *fRadioROC;           // ROC radio button
   TGRadioButton       *fRadioSector;        // sector radio button
   TGComboBox          *fComboAddDrawOpt;    // additional draw options combo box
   TGCheckButton       *fChkAuto;            // automatic redraw checkbox
   TGCheckButton       *fChkAutoAppend;      // automatic appendign of "~" checkbox
   TGComboBox          *fComboMethod;        // normalization methods dropdown box
   TGListBox           *fListNormalization;  // listbox with possible normalization variables
   TGComboBox          *fComboCustom;        // combo box for custom draw commands
   TGLabel             *fLblCustomDraw;      // custom draw labal
   TGCheckButton       *fChkAddDrawOpt;      // additional draw options check box
   TGNumberEntry       *fNmbSector;          // number entry box for specifying the sector
   TGLabel             *fLblSector;          // label that shows the active sector
   TGCheckButton       *fChkCutZero;         // cut zeros check box
   TGCheckButton       *fChkAddCuts;         // additional cuts check box
   TGLabel             *fLblAddCuts;         // additional cuts label
   TGComboBox          *fComboAddCuts;       // additional cuts combo box
   TGComboBox          *fComboCustomFit;     // custom fit combo box
   TGCheckButton       *fChkSetMax;          // Set maximum check box
   TGCheckButton       *fChkSetMin;          // Set maximum check box
   TGCheckButton       *fChkGetMinMaxAuto;   // Get Min & Max automatically from plot
   TGTextEntry         *fTxtSetMax;          // custom maximum text box
   TGTextEntry         *fTxtSetMin;          // custom minimum text box
   TGGroupFrame        *fContDrawOpt1D;      // container in tabLeft1 
   TGCompositeFrame    *fcontDrawOpt1DSubLR; // container in tabLeft1 to divide L/R
   TGCompositeFrame    *fContDrawOpt1DSubNSC; // container in tabLeft1 for following radio buttons 
   TGRadioButton       *fRadioNorm;          // radio button for normal 1D drawing
   TGRadioButton       *fRadioSigma;         // radio button for sigma 1D drawing
   TGTextEntry         *fTxtSigmas;          // text box to specify sigmas
   TGCompositeFrame    *fContCumuLR;         // container in tabLeft1 for two colums for cumulative and integrative
   TGCompositeFrame    *fContCumLeft;        // container in tabLeft1 for cumulative, left
   TGCompositeFrame    *fContCumRight;       // container in tabLeft1 for cumulative, right
   TGLabel             *fLblSigmaMax;        // label to indicate sigmaMax
   TGTextEntry         *fTxtSigmaMax;        // text box to specify sigmaMax
   TGRadioButton       *fRadioCumulative;    // radio button for cumulative 1D drawing
   TGCheckButton       *fCheckCumulativePM;  // checkbox for plus/minus cumulative 1D drawing
   TGRadioButton       *fRadioIntegrate;     // radio button for integral 1D drawing
   TGCompositeFrame    *fContDrawOpt1DSubMML; // container in tabLeft1 for following check boxes
   TGCheckButton       *fChkMean;            // checkbox to plot mean
   TGCheckButton       *fChkMedian;          // checkbox to plot median
   TGCheckButton       *fChkLTM;             // checkbox to plot LTM
   TGGroupFrame        *fContStatOpt;        // container for statistic options in tabLeft1 
   TGCheckButton       *fChkStatName;        // checkbox to display histogram name in statistic legend
   TGCheckButton       *fChkStatEntries;     // checkbox to display entries in statistic legend
   TGCompositeFrame    *fContStatMean;       // container for mean and its error in stat opt
   TGCheckButton       *fChkStatMean;        // checkbox to display mean in statistic legend
   TGCheckButton       *fChkStatMeanPM;      // checkbox to display mean error in statistic legend
   TGCompositeFrame    *fContStatRMS;        // container for RMS and its error in stat opt
   TGCheckButton       *fChkStatRMS;         // checkbox to display RMS in statistic legend
   TGCheckButton       *fChkStatRMSPM;       // checkbox to display RMS error in statistic legend
   TGCheckButton       *fChkStatUnderflow;   // checkbox to display underflow error in statistic legend
   TGCheckButton       *fChkStatOverflow;    // checkbox to display overflow error in statistic legend
   TGCheckButton       *fChkStatIntegral;    // checkbox to display integral in statistic legend
   TGCompositeFrame    *fContStatSkew;       // container for skewness and its error in stat opt
   TGCheckButton       *fChkStatSkewness;    // checkbox to display skewness in statistic legend
   TGCheckButton       *fChkStatSkewnessPM;  // checkbox to display skewness error in statistic legend
   TGCompositeFrame    *fContStatKurt;       // container for kurtosis and its error in stat opt
   TGCheckButton       *fChkStatKurtosis;    // checkbox to display kurtosis in statistic legend
   TGCheckButton       *fChkStatKurtosisPM;  // checkbox to display kurtosis error in statistic legend
   TGButton            *fBtnUnchekAll;       // Button to uncheck all statistic entries
   TGGroupFrame        *fContLabeling;       // groupframe container for labeling
   TGCheckButton       *fChkLabelTitle;      // checkbox to display specified title
   TGTextEntry         *fTxtLabelTitle;      // text box to specify title
   TGCheckButton       *fChkLabelXaxis;      // checkbox to display specified xaxis label
   TGTextEntry         *fTxtLabelXaxis;      // text box to specify xaxis label
   TGCheckButton       *fChkLabelYaxis;      // checkbox to display specified yaxis label
   TGTextEntry         *fTxtLabelYaxis;      // text box to specify yaxis label
   TGCheckButton       *fChkLabelGetAuto;    // checkbox to get labels atuomatically from plot
   TGGroupFrame        *fContSave;           // container for save-button
   TGButton            *fBtnSave;            // Save button
   TGCompositeFrame    *fContAddSaveOpt;     // container for additional save options
   TGCheckButton       *fChkAddSaveOpt;      // checkbox for additional save options
   TGComboBox          *fComboAddSaveOpt;    // combobox for additional save options
   TGGroupFrame        *fContExport;         // container for cint-export
   TGCompositeFrame    *fContAddExport;      // container for dropdown list to enter export name
   TGComboBox          *fComboExportName;    // dropdownbox to enter a name for the exported CalPad
   TGTextButton        *fBtnExport;          // button to export a CalPad
   TGTextButton        *fBtnAddNorm;         // button to add a CalPad to the normalization
   TGCompositeFrame    *fContTree;           // container for tree functions
   TGTextButton        *fBtnDumpToFile;      // button to dump a new CalibTree to file
   TGTextButton        *fBtnLoadTree;        // button to load a new tree
   TGCheckButton       *fChkAddAsReference;  // checkbox to add a new tree as referenceTree
   TGTextEntry         *fTxtRefName;         // text box to specify the referenceTree's name
   
   private:
   Bool_t fInitialized;                      // has the GUI already been initialized?
   
   ClassDef(AliTPCCalibViewerGUI, 0)
};

#endif
