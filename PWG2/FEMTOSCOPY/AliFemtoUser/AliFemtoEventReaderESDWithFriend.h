////////////////////////////////////////////////////////////////////////////////
///                                                                          ///
/// AliFemtoEventReaderESDWithFriend - the reader class for the Alice ESD    ///
/// Reads in ESD information and converts it into internal AliFemtoEvent     ///
/// Reads in AliESDfriend to create shared hit/quality information           ///
/// Authors: Marek Chojnacki mchojnacki@knf.pw.edu.pl                        ///
///          Adam Kisiel kisiel@mps.ohio-state.edu                           ///
///                                                                          ///
////////////////////////////////////////////////////////////////////////////////

#ifndef AliFemtoEventReaderESDWithFriend_hh
#define AliFemtoEventReaderESDWithFriend_hh

#include "AliFemtoEventReader.h"
#include "AliFemtoEnumeration.h"

#include <string>
#include <vector>
#include "TTree.h"
#include "AliESDEvent.h"
#include "AliESDfriend.h"
#include <list>

class AliFemtoEvent;

class AliFemtoEventReaderESDWithFriend : public AliFemtoEventReader 
{
 public:
  AliFemtoEventReaderESDWithFriend();
  AliFemtoEventReaderESDWithFriend(const AliFemtoEventReaderESDWithFriend &aReader);
  ~AliFemtoEventReaderESDWithFriend();

  AliFemtoEventReaderESDWithFriend& operator=(const AliFemtoEventReaderESDWithFriend& aReader);

  AliFemtoEvent* ReturnHbtEvent();
  AliFemtoString Report();
  //void SetFileName(const char* fileName);
  void SetInputFile(const char* inputFile);
  void SetConstrained(const bool constrained);
  bool GetConstrained() const;

 protected:

 private:
  bool           GetNextFile();     // setting next file to read 

  string         fInputFile;        // name of input file with ESD filenames
  string         fFileName;         // name of current ESD file
  bool           fConstrained;      // flag to set which momentum from ESD file will be use
  int            fNumberofEvent;    // number of Events in ESD file
  int            fCurEvent;         // number of current event
  unsigned int   fCurFile;          // number of current file
  vector<string> fListOfFiles;      // list of ESD files 		
  TTree*         fTree;             // ESD tree
  AliESDEvent*   fEvent;            // ESD event
  TFile*         fEsdFile;          // ESD file 
  AliESDfriend*  fEventFriend;      // ESD friend informaion

  list<Int_t>  **fSharedList;       //! Table (one list per padrow) of clusters which are shared
  list<Int_t>  **fClusterPerPadrow; //! Table (one list per padrow) of clusters in each padrow
		
#ifdef __ROOT__
  ClassDef(AliFemtoEventReaderESDWithFriend, 1)
#endif

    };
  
#endif


