
#include "TROOT.h"
#include "TSystem.h"
#include "TRint.h"
#include "TApplication.h"
#include "TGClient.h"
#include "TGLabel.h"
#include "TGFrame.h"
#include "TGLayout.h"
#include "TGComboBox.h"
#include "TGTextView.h"
#include "TGTextEntry.h"
#include "TGTextEdit.h"
#include "TInterpreter.h"
#include "Getline.h"

#include "TGCommandPlugin.h"

ClassImp(TGCommandPlugin)

//______________________________________________________________________________
TGCommandPlugin::TGCommandPlugin(const TGWindow *p, UInt_t w, UInt_t h) :
      TGMainFrame(p, w, h)
{

   SetCleanup(kDeepCleanup);
   fHf = new TGHorizontalFrame(this, 100, 20);
   fCommandBuf = new TGTextBuffer(256);
   fComboCmd   = new TGComboBox(fHf, "", 1);
   fCommand    = fComboCmd->GetTextEntry();
   fCommandBuf = fCommand->GetBuffer();
   fComboCmd->Resize(200, fCommand->GetDefaultHeight());
   fHf->AddFrame(fComboCmd, new TGLayoutHints(kLHintsCenterY |
                 kLHintsRight | kLHintsExpandX, 5, 5, 1, 1));
   fHf->AddFrame(fLabel = new TGLabel(fHf, "Command (local):"),
                 new TGLayoutHints(kLHintsCenterY | kLHintsRight, 
                 5, 5, 1, 1));
   AddFrame(fHf, new TGLayoutHints(kLHintsLeft | kLHintsTop | 
            kLHintsExpandX, 3, 3, 3, 3));
   fCommand->Connect("ReturnPressed()", "TGCommandPlugin", this,
                     "HandleCommand()");

   Pixel_t pxl;
   gClient->GetColorByName("#ccccff", pxl);
   fStatus = new TGTextView(this, 10, 100, 1);
   fStatus->SetSelectBack(pxl);
   fStatus->SetSelectFore(TGFrame::GetBlackPixel());
   AddFrame(fStatus, new TGLayoutHints(kLHintsLeft | kLHintsTop | 
            kLHintsExpandX | kLHintsExpandY, 3, 3, 3, 3));
   fPid = gSystem->GetPid();
   TString defhist(Form("%s/.root_hist", gSystem->UnixPathName(
                        gSystem->HomeDirectory())));
   FILE *lunin = fopen(defhist.Data(), "rt");
   if (lunin) {
      char histline[256];
      while (fgets(histline, 256, lunin)) {
         histline[strlen(histline)-1] = 0; // remove trailing "\n"
         fComboCmd->InsertEntry(histline, 0, -1);
      }
      fclose(lunin);
   }
   MapSubwindows();
   Resize(GetDefaultSize());
   MapWindow();
}

//______________________________________________________________________________
TGCommandPlugin::~TGCommandPlugin()
{
   // Destructor.

   Cleanup();
}

//______________________________________________________________________________
void TGCommandPlugin::CheckRemote(const char * /*str*/)
{
   // Check if actual ROOT session is a remote one or a local one.

   Pixel_t pxl;
   TString sPrompt = ((TRint*)gROOT->GetApplication())->GetPrompt();
   Int_t end = sPrompt.Index(":root [", 0);
   if (end > 0 && end != kNPOS) {
      // remote session
      sPrompt.Remove(end);
      gClient->GetColorByName("#ff0000", pxl);
      fLabel->SetTextColor(pxl);
      fLabel->SetText(Form("Command (%s):", sPrompt.Data()));
   }
   else {
      // local session
      gClient->GetColorByName("#000000", pxl);
      fLabel->SetTextColor(pxl);
      fLabel->SetText("Command (local):");
   }
   fHf->Layout();
}

//______________________________________________________________________________
void TGCommandPlugin::HandleCommand()
{
   // Handle command line from the "command" combo box.

   const char *string = fCommandBuf->GetString();
   if (strlen(string) > 1) {
      // form temporary file path
      TString pathtmp = Form("%s/ride.%d.log", gSystem->TempDirectory(), fPid);
      TString sPrompt = ((TRint*)gROOT->GetApplication())->GetPrompt();
      FILE *lunout = fopen(pathtmp.Data(), "a+t");
      if (lunout) {
         fputs(Form("%s%s\n",sPrompt.Data(), string), lunout);
         fclose(lunout);
      }
      gSystem->RedirectOutput(pathtmp.Data(), "a");
      gApplication->SetBit(TApplication::kProcessRemotely);
      gROOT->ProcessLine(string);
      fComboCmd->InsertEntry(string, 0, -1);
      Gl_histadd((char *)string);
      gSystem->RedirectOutput(0);
      fStatus->LoadFile(pathtmp.Data());
      fStatus->ShowBottom();
      CheckRemote(string);
      fCommand->Clear();
   }

}

