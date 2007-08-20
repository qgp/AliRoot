#ifndef REVE_RGBrowser_H
#define REVE_RGBrowser_H

#include <TGFrame.h>
#include <TGButton.h>
#include <TGListTree.h>
#include <TGNumberEntry.h>
#include <TGColorSelect.h>

#include <TParticle.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TGeoVolume.h>
#include <TContextMenu.h>

class TGLViewer;

namespace Reve {

class RGEditor;

class RGBrowser : public TGCompositeFrame
{
  RGBrowser(const RGBrowser&);            // Not implemented
  RGBrowser& operator=(const RGBrowser&); // Not implemented

protected:
  TGCompositeFrame* fMainFrame;
  TGVerticalFrame*  fV1;
  TGVerticalFrame*  fV2;

  TGCompositeFrame* fSelectionFrame; // in fact list-tree frame
  TGCanvas*         fTreeView;

  TGListTree*       fListTree;
  TContextMenu*     fCtxMenu;

 protected:
  void SetupCintExport(TClass* cl);
  void CalculateReparentXY(TGObject* parent, Int_t& x, Int_t& y);

 public:
  RGBrowser(const TGWindow *p, UInt_t w, UInt_t h);
  virtual ~RGBrowser() {}

  void       SetupEditorLook  (RGEditor*& editor);
  TGLViewer* SetupGLViewerLook(RGEditor*& editor);

  void RedrawListTree();

  void ItemClicked(TGListTreeItem *entry, Int_t btn, Int_t x, Int_t y);
  void DbClickListItem(TGListTreeItem* item, Int_t btn);
  void KeyPressListItem(TGListTreeItem *entry, UInt_t keysym, UInt_t mask);

  void ExportToCINT(Text_t* var_name, TObject* obj);

  void UpdateListItems(TGListTreeItem* item, Int_t btn);

  TGListTree* GetListTree() { return fListTree; }

  ClassDef(RGBrowser, 1);
};

} // namespace Reve

#endif
