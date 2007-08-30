
#include "TROOT.h"
#include "TSystem.h"
#include "TApplication.h"
#include "TGClient.h"
#include "TGListTree.h"
#include "TGLayout.h"
#include "TGComboBox.h"
#include "TContextMenu.h"
#include "TGTextEntry.h"
#include "TGTab.h"
#include "TSystemDirectory.h"
#include "TGMimeTypes.h"
#include "TClass.h"
#include "TFile.h"
#include "TInterpreter.h"
#include "TRegexp.h"
#include "TEnv.h"
#include "TImage.h"
#include "TBrowser.h"
#include <time.h>
#include <string.h>

#include "TGFileBrowser.h"
#include "TGNewBrowser.h"

#ifdef WIN32
const char rootdir[] = "\\";
#else
const char rootdir[] = "/";
#endif

const char *filters[] = {
   "",
   "*.*",
   "*.[C|c|h]*",
   "*.root",
   "*.txt"
};

////////////////////////////////////////////////////////////////////////////////////
class TCursorSwitcher {
private:
   TGWindow *fW1;
   TGWindow *fW2;
public:
   TCursorSwitcher(TGWindow *w1, TGWindow *w2) : fW1(w1), fW2(w2) {
      if (w1) gVirtualX->SetCursor(w1->GetId(), gVirtualX->CreateCursor(kWatch));
      if (w2) gVirtualX->SetCursor(w2->GetId(), gVirtualX->CreateCursor(kWatch));
   }
   ~TCursorSwitcher() {
      if (fW1) gVirtualX->SetCursor(fW1->GetId(), gVirtualX->CreateCursor(kPointer));
      if (fW2) gVirtualX->SetCursor(fW2->GetId(), gVirtualX->CreateCursor(kPointer));
   }
};

ClassImp(TGFileBrowser)

//______________________________________________________________________________
TGFileBrowser::TGFileBrowser(TBrowser* b, const char *name, UInt_t w, UInt_t h) :
   TGMainFrame(gClient->GetRoot(), w, h), TBrowserImp(b),
   fNewBrowser(0)
{
   CreateBrowser(name);
   Resize(w, h);
   if (fBrowser) Show();
}

//______________________________________________________________________________
TGFileBrowser::TGFileBrowser(TBrowser* b, const char *name, Int_t x, Int_t y,
                             UInt_t w, UInt_t h) :
   TGMainFrame(gClient->GetRoot(), w, h), TBrowserImp(b),
   fNewBrowser(0)
{
   CreateBrowser(name);
   MoveResize(x, y, w, h);
   SetWMPosition(x, y);
   if (fBrowser) Show();
}

//______________________________________________________________________________
void TGFileBrowser::CreateBrowser(const char *name)
{
   fCachedPic  = 0;
   SetCleanup(kDeepCleanup);

   /*
   fNewBrowser = 0;
   if (p && p != gClient->GetDefaultRoot())
      fNewBrowser = (TGNewBrowser *)p->GetMainFrame();
   if (fNewBrowser)
      fNewBrowser->SetBrowserFrame(this);
   */

   fCanvas   = new TGCanvas(this, 100, 100);
   fListTree = new TGListTree(fCanvas, kHorizontalFrame);
   AddFrame(fCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop | 
                kLHintsExpandX | kLHintsExpandY));
   fListTree->Connect("DoubleClicked(TGListTreeItem *, Int_t)",
      "TGFileBrowser", this, "DoubleClicked(TGListTreeItem *, Int_t)");
   fListTree->Connect("Clicked(TGListTreeItem *, Int_t, Int_t, Int_t)",
      "TGFileBrowser", this, "Clicked(TGListTreeItem *, Int_t, Int_t, Int_t)");

   fRootIcon = gClient->GetPicture("rootdb_t.xpm");
   fFileIcon = gClient->GetPicture("doc_t.xpm");

   fFileType = new TGComboBox(this, " All Files (*.*)");
   Int_t dropt = 1;
   fFileType->AddEntry(" All Files (*.*)", dropt++);
   fFileType->AddEntry(" C/C++ Files (*.c;*.cxx;*.h;...)", dropt++);
   fFileType->AddEntry(" ROOT Files (*.root)", dropt++);
   fFileType->AddEntry(" Text Files (*.txt)", dropt++);
   fFileType->Resize(200, fFileType->GetTextEntry()->GetDefaultHeight());
   AddFrame(fFileType, new TGLayoutHints(kLHintsLeft | kLHintsTop | 
                kLHintsExpandX, 2, 2, 2, 2));
   fFileType->Connect("Selected(Int_t)", "TGFileBrowser", this, "ApplyFilter(Int_t)");

   fContextMenu = new TContextMenu("FileBrowserContextMenu") ;
   fFilter      = 0;
   fGroupSize   = 1000;
   fListLevel   = 0;
   fCurrentDir  = 0;
   fRootDir     = 0;
   TString gv = gEnv->GetValue("Browser.GroupView", "10000");
   Int_t igv = atoi(gv.Data());
   if (igv > 10)
      fGroupSize = igv;
   
   if (gEnv->GetValue("Browser.ShowHidden", 0))
      fShowHidden = kTRUE;
   else
      fShowHidden = kFALSE;

   MapSubwindows();
   Resize(GetDefaultSize());
   SetWindowName(name);
   SetIconName(name);
   MapWindow();
}

//______________________________________________________________________________
void TGFileBrowser::ReallyDelete()
{
   // Really delete the browser and the this GUI.

   gInterpreter->DeleteGlobal(fBrowser);
   delete fBrowser;    // will in turn delete this object
}

//______________________________________________________________________________
TGFileBrowser::~TGFileBrowser()
{
   // Destructor.

   delete fContextMenu;
   delete fListTree;
   Cleanup();
}


/**************************************************************************/
// TBrowserImp virtuals
/**************************************************************************/

//______________________________________________________________________________
void TGFileBrowser::Add(TObject *obj, const char *name, Int_t check)
{
   // Add items to the browser. This function has to be called
   // by the Browse() member function of objects when they are
   // called by a browser. If check < 0 (default) no check box is drawn,
   // if 0 then unchecked checkbox is added, if 1 checked checkbox is added.

   if (obj && obj->InheritsFrom("TSystemDirectory"))
      return;
   const TGPicture *pic=0;
   if (obj && obj->InheritsFrom("TKey"))
      AddKey(fListLevel, obj, name);
   else if (obj) {
      GetObjPicture(&pic, obj);
      if (!name) name = obj->GetName();
      if(check > -1) {
         if (!fListTree->FindChildByName(fListLevel, name)) {
            TGListTreeItem *item = fListTree->AddItem(fListLevel, name, obj, pic, pic, kTRUE);
            fListTree->CheckItem(item, (Bool_t)check);
            TString tip(obj->ClassName());
            if (obj->GetTitle()) {
               tip += " ";
               tip += obj->GetTitle();
            }
            fListTree->SetToolTipItem(item, tip.Data());
         }
      } 
      else {
         if (!fListTree->FindChildByName(fListLevel, name))
            fListTree->AddItem(fListLevel, name, obj, pic, pic);
      }
   }
   fListTree->ClearViewPort();
}

//______________________________________________________________________________
void TGFileBrowser::BrowseObj(TObject *obj)
{
   // Browse object. This, in turn, will trigger the calling of
   // TRootBrowser::Add() which will fill the IconBox and the tree.
   // Emits signal "BrowseObj(TObject*)".

   obj->Browse(fBrowser);
}

//______________________________________________________________________________
void TGFileBrowser::RecursiveRemove(TObject *obj)
{

   TGListTreeItem *itm = 0, *item = 0;
   if (obj->InheritsFrom("TFile")) {
      itm = fListTree->FindChildByData(0, gROOT->GetListOfFiles());
      if (itm)
         item = fListTree->FindChildByData(itm, obj);
      if (item)
         fListTree->DeleteItem(item);
      itm = fRootDir ? fRootDir->GetFirstChild() : 0;
      while (itm) {
         item = fListTree->FindItemByObj(itm, obj);
         if (item) {
            fListTree->DeleteChildren(item);
            item->SetUserData(0);
         }
         itm = itm->GetNextSibling();
      }
   }
   if (!obj->InheritsFrom("TFile") && fRootDir)
      fListTree->RecursiveDeleteItem(fRootDir, obj);
   gClient->NeedRedraw(fListTree, kTRUE);
}

//______________________________________________________________________________
void TGFileBrowser::Refresh(Bool_t /*force*/)
{

   TCursorSwitcher cursorSwitcher(this, fListTree);
   static UInt_t prev = 0;
   UInt_t curr =  gROOT->GetListOfBrowsables()->GetSize();
   if (!prev) prev = curr;

   if (prev != curr) { // refresh gROOT
      TGListTreeItem *sav = fListLevel;
      fListLevel = 0;
      BrowseObj(gROOT);
      fListLevel = sav;
      prev = curr;
   }
}


/**************************************************************************/
// Other
/**************************************************************************/

//______________________________________________________________________________
void TGFileBrowser::AddFSDirectory(const char* /*entry*/, const char* path)
{
   if (path == 0 && fRootDir == 0) {
      fRootDir = fListTree->AddItem(0, rootdir);
   } else {
      // MT: i give up! wanted to place entries for selected
      // directories like home, pwd, alice-macros.
      // TGListTreeItem *lti = fListTree->AddItem(0, entry);
      //
   }
}

//______________________________________________________________________________
void TGFileBrowser::AddKey(TGListTreeItem *itm, TObject *obj, const char *name)
{
   // display content of ROOT file

   // Int_t from, to;
   TGListTreeItem *where;
   static TGListTreeItem *olditem = itm;
   static TGListTreeItem *item = itm;
   static const TGPicture *pic  = 0;
   static const TGPicture *pic2 = gClient->GetPicture("leaf_t.xpm");

   if ((fCnt == 0) || (olditem != itm)) {
      olditem = item = itm;
   }
   if (!name) name = obj->GetName();
   if (fNKeys > fGroupSize) {
      where = itm->GetFirstChild();
      while (where) {
         if (fListTree->FindItemByObj(where, obj))
            return;
         where = where->GetNextSibling();
      }
   }
   if ((fNKeys > fGroupSize) && (fCnt % fGroupSize == 0)) {
      if (item != itm) {
         TString newname = Form("%s-%s", item->GetText(), name);
         item->Rename(newname.Data());
      }
      item = fListTree->AddItem(itm, name);
   }
   if ((fCnt > fGroupSize) && (fCnt >= fNKeys-1)) {
      TString newname = Form("%s-%s", item->GetText(), name);
      item->Rename(newname.Data());
   }
   GetObjPicture(&pic, obj);
   if (!pic) pic = pic2;
   if (!fListTree->FindChildByName(item, name)) {
      fListTree->AddItem(item, name, obj, pic, pic);
   }
   fCnt++;
}

//______________________________________________________________________________
void TGFileBrowser::ApplyFilter(Int_t id)
{
   // Apply filter selected in combo box to the file tree view.

   // Long64_t size;
   // Long_t fid, flags, modtime;

   if (fFilter) delete fFilter;
   fFilter = 0;
   if (id > 1)
      fFilter = new TRegexp(filters[id], kTRUE);
   TGListTreeItem *item = fCurrentDir;
   if (!item)
      item = fRootDir;
   fListTree->DeleteChildren(item);
   DoubleClicked(item, 1);
   //fListTree->AdjustPosition(item);
   fListTree->ClearViewPort();
}

//______________________________________________________________________________
void TGFileBrowser::Chdir(TGListTreeItem *item)
{
   // Make object associated with item the current directory.

   if (item) {
      TGListTreeItem *i = item;
      TString dir;
      while (i) {
         TObject *obj = (TObject*) i->GetUserData();
         if (obj) {
            if (obj->InheritsFrom("TDirectoryFile")) {
               dir = "/" + dir;
               dir = obj->GetName() + dir;
            }
            if (obj->InheritsFrom("TFile")) {
               dir = ":/" + dir;
               dir = obj->GetName() + dir;
            }
            if (obj->InheritsFrom("TKey")) {
               const char *clname = (const char *)gROOT->ProcessLine(Form("((TKey *)0x%lx)->GetClassName();", obj));
               if (clname && strcmp(clname, "TDirectoryFile") == 0) {
                  dir = "/" + dir;
                  dir = obj->GetName() + dir;
               }
            }
         }
         i = i->GetParent();
      }
      if (gDirectory && dir.Length()) gDirectory->cd(dir.Data());
   }
}

//______________________________________________________________________________
void TGFileBrowser::Clicked(TGListTreeItem *item, Int_t btn, Int_t x, Int_t y)
{

   char path[1024];
   Long64_t size = 0;
   Long_t id = 0, flags = 0, modtime = 0;
   fListLevel = item;
   if (item && btn == kButton3) {
      TObject *obj = (TObject *) item->GetUserData();
      if (obj) {
         if (obj->InheritsFrom("TKey")) {
            Chdir(item->GetParent());
            const char *clname = (const char *)gROOT->ProcessLine(Form("((TKey *)0x%lx)->GetClassName();", obj));
            if (clname) {
               TClass *cl = TClass::GetClass(clname);
               void *add = gROOT->FindObject((char *) obj->GetName());
               if (add && cl->IsTObject()) {
                  obj = (TObject*)add;
               }
            }
         }
         fContextMenu->Popup(x, y, obj);
      }
      else {
         fListTree->GetPathnameFromItem(item, path);
         if (strlen(path) > 3) {
            TString dirname = DirName(item);
            gSystem->GetPathInfo(dirname.Data(), &id, &size, &flags, &modtime);
            if (flags & 2) {
               fCurrentDir = item;
               TSystemDirectory d(item->GetText(), dirname.Data());
               fContextMenu->Popup(x, y, &d);
            }
            else {
               fCurrentDir = item->GetParent();
               TSystemFile f(item->GetText(), dirname.Data());
               fContextMenu->Popup(x, y, &f);
            }
         }
      }
   }
   else {
      if (!item->GetUserData()) {
         fListTree->GetPathnameFromItem(item, path);
         if (strlen(path) > 1) {
            TString dirname = DirName(item);
            gSystem->GetPathInfo(dirname.Data(), &id, &size, &flags, &modtime);
            if (flags & 2)
               fCurrentDir = item;
            else
               fCurrentDir = item->GetParent();
         }
      }
   }
}

//______________________________________________________________________________
TString TGFileBrowser::DirName(TGListTreeItem* item)
{
   // returns an absolute path

   TGListTreeItem* parent;
   TString dirname = item->GetText();

   while ((parent=item->GetParent())) {
      dirname = gSystem->ConcatFileName(parent->GetText(),dirname);
      item = parent;
   }

   return dirname;
}

//______________________________________________________________________________
static Bool_t IsTextFile(const char *candidate)
{
   // Returns true if given a text file
   // Uses the specification given on p86 of the Camel book
   // - Text files have no NULLs in the first block
   // - and less than 30% of characters with high bit set

   Int_t i;
   Int_t nchars;
   Int_t weirdcount = 0;
   char buffer[512];
   FILE *infile;
   FileStat_t buf;

   gSystem->GetPathInfo(candidate, buf);
   if (!(buf.fMode & kS_IFREG))
      return kFALSE;

   infile = fopen(candidate, "r");
   if (infile) {
      // Read a block
      nchars = fread(buffer, 1, 512, infile);
      fclose (infile);
      // Examine the block
      for (i = 0; i < nchars; i++) {
         if (buffer[i] & 128)
            weirdcount++;
         if (buffer[i] == '\0')
            // No NULLs in text files
            return kFALSE;
      }
      if ((nchars > 0) && ((weirdcount * 100 / nchars) > 30))
         return kFALSE;
   } else {
      // Couldn't open it. Not a text file then
      return kFALSE;
   }
   return kTRUE;
}

//______________________________________________________________________________
void TGFileBrowser::DoubleClicked(TGListTreeItem *item, Int_t /*btn*/)
{
   // Process double clicks in TGListTree.

   const TGPicture *pic=0;
   TString dirname = DirName(item);
   TGListTreeItem *itm;
   FileStat_t sbuf;
   Long64_t size;
   Long_t id, flags, modtime;

   TCursorSwitcher switcher(this, fListTree);
   fListLevel = item;
   TObject *obj = (TObject *) item->GetUserData();
   if (obj && !obj->InheritsFrom("TSystemFile")) {
      if (obj->InheritsFrom("TFile")) {
         fNKeys = ((TFile *)(obj))->GetListOfKeys()->GetEntries();
      }
      obj->Browse(fBrowser);
      fNKeys = 0;
      fCnt = 0;
      return;
   }
   flags = id = size = modtime = 0;
   gSystem->GetPathInfo(dirname.Data(), &id, &size, &flags, &modtime);
   Int_t isdir = (Int_t)flags & 2;

   TString savdir = gSystem->WorkingDirectory();
   if (isdir) {
      fCurrentDir = item;
      TSystemDirectory dir(item->GetText(),DirName(item));
      TList *files = dir.GetListOfFiles();
      if (files) {
         files->Sort();
         TIter next(files);
         TSystemFile *file;
         TString fname;
         // directories first 
         while ((file=(TSystemFile*)next())) {
            fname = file->GetName();
            if (file->IsDirectory()) {
               if (!fShowHidden && fname.BeginsWith("."))
                  continue;
               if ((fname!="..") && (fname!=".")) { // skip it
                  if (!fListTree->FindChildByName(item, fname)) {
                     itm = fListTree->AddItem(item, fname);
                  }
               }
            }
         }
         // then files... 
         TIter nextf(files);
         while ((file=(TSystemFile*)nextf())) {
            fname = file->GetName();
            if (!file->IsDirectory() && (fFilter == 0 ||
               (fFilter && fname.Index(*fFilter) != kNPOS))) {
               if (!fShowHidden && fname.BeginsWith("."))
                  continue;
               size = modtime = 0;
               if (gSystem->GetPathInfo(fname, sbuf) == 0) {
                  size    = sbuf.fSize;
                  modtime = sbuf.fMtime;
               }
               pic = gClient->GetMimeTypeList()->GetIcon(fname, kTRUE);
               if (!pic)
                  pic = fFileIcon;
               if (!fListTree->FindChildByName(item, fname)) {
                  itm = fListTree->AddItem(item,fname,pic,pic);
                  if (size && modtime) {
                     char *tiptext = FormatFileInfo(fname.Data(), size, modtime);
                     itm->SetTipText(tiptext);
                     delete [] tiptext;
                  }
               }
            }
         }
         delete files;
      }
   }
   else {
      fCurrentDir = item->GetParent();
      TSystemFile f(item->GetText(), dirname.Data());
      TString fname = f.GetName();
      if (fname.EndsWith(".root")) {
         TFile *rfile = 0;
         gSystem->ChangeDirectory(gSystem->DirName(dirname.Data()));
         rfile = (TFile *)gROOT->GetListOfFiles()->FindObject(obj);
         if (!rfile) {
            rfile = (TFile *)XXExecuteDefaultAction(&f);
            item->SetUserData(rfile);
         }
         if (rfile) {
            fNKeys = rfile->GetListOfKeys()->GetEntries();
            fCnt = 0;
            rfile->Browse(fBrowser);
            fNKeys = 0;
            fCnt = 0;
         }
         //DisplayFile(item, fname.Data());
      }
      else if (fname.EndsWith(".png")) {
         gSystem->ChangeDirectory(gSystem->DirName(dirname.Data()));
         XXExecuteDefaultAction(&f);
      }
      else if (IsTextFile(dirname.Data())) {
         gSystem->ChangeDirectory(gSystem->DirName(dirname.Data()));
         if (fNewBrowser) {
            TGTab *tabRight = fNewBrowser->GetTabRight();
            TGCompositeFrame *frame = tabRight->GetCurrentContainer();
            TGFrameElement *fe = (TGFrameElement *)frame->GetList()->First();
            if (fe) {
               TGCompositeFrame *embed = (TGCompositeFrame *)fe->fFrame;
               if (embed->InheritsFrom("TGTextEditor")) {
                  gROOT->ProcessLine(Form("((TGTextEditor *)0x%lx)->LoadFile(\"%s\");", 
                                     embed, f.GetName()));
               }
               else if (embed->InheritsFrom("TGTextEdit")) {
                  gROOT->ProcessLine(Form("((TGTextEdit *)0x%lx)->LoadFile(\"%s\");", 
                                     embed, f.GetName()));
               }
            }
            else {
               XXExecuteDefaultAction(&f);
            }
         }
      }
      else {
         gSystem->ChangeDirectory(gSystem->DirName(dirname.Data()));
         XXExecuteDefaultAction(&f);
      }
   }
   gSystem->ChangeDirectory(savdir.Data());
}

//____________________________________________________________________________
Long_t TGFileBrowser::XXExecuteDefaultAction(TObject *obj)
{
   // Execute default action for selected object (action is specified
   // in the $HOME/.root.mimes or $ROOTSYS/etc/root.mimes file.

   char action[512];
   TString act;
   TString ext = obj->GetName();

   if (gClient->GetMimeTypeList()->GetAction(obj->GetName(), action)) {
      act = action;
      act.ReplaceAll("%s", obj->GetName());
      gInterpreter->SaveGlobalsContext();

      if (act[0] == '!') {
         act.Remove(0, 1);
         gSystem->Exec(act.Data());
         return 0;
      } else {
         return gApplication->ProcessLine(act.Data());
      }
   }
   return 0;
}

//______________________________________________________________________________
char *TGFileBrowser::FormatFileInfo(const char *fname, Long64_t size, Long_t modtime)
{

   Long64_t fsize, bsize;
   TString infos = fname;
   infos += "\n";

   fsize = bsize = size;
   if (fsize > 1024) {
      fsize /= 1024;
      if (fsize > 1024) {
         // 3.7MB is more informative than just 3MB
         infos += Form("Size: %lld.%lldM", fsize/1024, (fsize%1024)/103);
      } else {
         infos += Form("Size: %lld.%lldK", bsize/1024, (bsize%1024)/103);
      }
   } else {
      infos += Form("Size: %lld", bsize);
   }
   struct tm *newtime;
   time_t loctime = (time_t) modtime;
   newtime = localtime(&loctime);
   infos += "\n";
   infos += Form("%d-%02d-%02d %02d:%02d", newtime->tm_year + 1900,
           newtime->tm_mon+1, newtime->tm_mday, newtime->tm_hour,
           newtime->tm_min);
   return StrDup(infos.Data());
}

//______________________________________________________________________________
void TGFileBrowser::GetObjPicture(const TGPicture **pic, TObject *obj)
{
   // Retrieve icons associated with class "name". Association is made
   // via the user's ~/.root.mimes file or via $ROOTSYS/etc/root.mimes.

   TClass *objClass = 0;
   static TImage *im = 0;
   if (!im) {
      im = TImage::Create();
   }

   if (obj->InheritsFrom("TKey")) {
      const char *clname = (const char *)gROOT->ProcessLine(Form("((TKey *)0x%lx)->GetClassName();", obj));
      if (clname)
         objClass = TClass::GetClass(clname);
   }
   else if (obj->InheritsFrom("TKeyMapFile")) {
      const char *title = (const char *)gROOT->ProcessLine(Form("((TKeyMapFile *)0x%lx)->GetTitle();", obj));
      if (title)
         objClass = TClass::GetClass(title);
   }
   else
      objClass = obj->IsA();
   const char *name = obj->GetIconName() ? obj->GetIconName() : objClass->GetName();
   TString xpm_magic(name, 3);
   Bool_t xpm = xpm_magic == "/* ";
   const char *iconname = xpm ? obj->GetName() : name;

   if (obj->IsA()->InheritsFrom("TGeoVolume")) {
      iconname = obj->GetIconName() ? obj->GetIconName() : obj->IsA()->GetName();
   }

   if (fCachedPicName == iconname) {
      *pic = fCachedPic;
      return;
   }
   *pic = gClient->GetMimeTypeList()->GetIcon(iconname, kTRUE);
   if (!(*pic) && xpm) {
      if (im && im->SetImageBuffer((char**)&name, TImage::kXpm)) {
         *pic = gClient->GetPicturePool()->GetPicture(iconname, im->GetPixmap(),
                                                      im->GetMask());
      }
      gClient->GetMimeTypeList()->AddType("[thumbnail]", iconname, iconname, iconname, "->Browse()");
      return;
   }
   if (*pic == 0) {
      if (!obj->IsFolder())
         *pic = fFileIcon;
   }
   fCachedPic = *pic;
   fCachedPicName = iconname;
}

//______________________________________________________________________________
void TGFileBrowser::GotoDir(const char *path)
{

   TGListTreeItem *item, *itm;
   char *token;
   const char seps[] = "/\\";
   // Establish string and get the first token:
   token = strtok(strdup(path), seps);
   // Note: strtok is deprecated; consider using strtok_s instead
   item = fRootDir;
   if (item == 0) return;
   fListTree->HighlightItem(item);
   fListTree->OpenItem(item);
   DoubleClicked(item, 1);
   gSystem->ProcessEvents();
   while (token) {
      // while there are tokens in path
      itm = fListTree->FindChildByName(item, token);
      if (itm) {
         item = itm;
         fListTree->HighlightItem(item);
         fListTree->OpenItem(item);
         DoubleClicked(item, 1);
         gSystem->ProcessEvents();
      }
      // get next token:
      token = strtok( NULL, seps );
   }
   gClient->NeedRedraw(fListTree);
   gSystem->ProcessEvents();
   fListTree->AdjustPosition(item);
}
