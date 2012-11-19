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

/* $Id$ */

//-----------------------------------------------------------------
//           Implementation of the AliXMLCollection class
//   This is the class that creates XML collections after querying the tags
//   Origin: Panos Christakoglou, UOA-CERN, Panos.Christakoglou@cern.ch
//-----------------------------------------------------------------

#include <cstdlib>
//ROOT
#include <Riostream.h>
#include <TEntryList.h>
#include <TList.h>
#include <TMap.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TString.h>
#include <TXMLEngine.h>
#include "AliLog.h"

#include "AliXMLCollection.h"

ClassImp(AliXMLCollection)
//___________________________________________________________________________
  AliXMLCollection::AliXMLCollection() :
    TGridCollection(),
    fXmlFile(),
    fEventList(0),
    fEventListIter(0),
    fCurrent(0),
    fCollectionName(),
    fout(),
    fTotalEvents(0),
    fAcceptedEvents(0),
    fRejectedRun(0),
    fRejectedLHC(0),
    fRejectedDet(0),
    fRejectedEvt(0)
{
  //Default constructor
}

//___________________________________________________________________________
AliXMLCollection::AliXMLCollection(const char *localcollectionfile) :
    TGridCollection(),
    fXmlFile(localcollectionfile),
    fEventList(0),
    fEventListIter(0),
    fCurrent(0),
    fCollectionName(),
    fout(),
    fTotalEvents(0),
    fAcceptedEvents(0),
    fRejectedRun(0),
    fRejectedLHC(0),
    fRejectedDet(0),
    fRejectedEvt(0)
 {
   // Create Alien event collection, by reading collection for the specified
   // file.

   fEventList = new TList();
   fEventList->SetOwner(kTRUE);
   fEventListIter = new TIter(fEventList);

   if (localcollectionfile!=0) {
     ParseXML();
   }
}

//___________________________________________________________________________
AliXMLCollection::AliXMLCollection(const AliXMLCollection& collection):
  TGridCollection(collection),
  fXmlFile(collection.fXmlFile),
  fEventList(0),
  fEventListIter(0),
  fCurrent(0),
  fCollectionName(collection.fCollectionName),
  fout(),
  fTotalEvents(0),
  fAcceptedEvents(0),
  fRejectedRun(0),
  fRejectedLHC(0),
  fRejectedDet(0),
  fRejectedEvt(0)
{
  //copy constructor

  if (collection.fEventList) fEventList = new TList();
  if (collection.fEventListIter) fEventListIter = new TIter(fEventList);
  if (collection.fCurrent) fCurrent = 0;
}

//___________________________________________________________________________
AliXMLCollection::~AliXMLCollection() 
{
  //Destructor
  delete fEventList;
  delete fEventListIter;
}

//___________________________________________________________________________
Bool_t AliXMLCollection::WriteHeader() {
  //Creates the xml output file

  TString xmlName = fCollectionName;
  xmlName += ".xml";

  TString collectionHeader = "<collection name=";
  collectionHeader += "\"";
  collectionHeader += fCollectionName;
  collectionHeader += "\"";
  collectionHeader += ">";
  
  // Open the output stream
  fout.open(xmlName);
  fout<<"<?xml version=\"1.0\"?>\n";
  fout<<"<alien>\n";
  fout<<"  "<<collectionHeader<<"\n";  

  return kTRUE;
}

//___________________________________________________________________________
Bool_t AliXMLCollection::WriteSummary(Int_t aTotal, Int_t aAccepted, Int_t aRejRun, Int_t aRejLHC, Int_t aRejDet, Int_t aRejEvt)
{
  // Write selection summary

  TString collectionSummary = "<summary";
  collectionSummary += " acceptedEvents=\"";
  collectionSummary += aAccepted;
  collectionSummary += "\" totalEvent=\"";
  collectionSummary += aTotal;
  collectionSummary += "\" rejectedRun=\"";
  collectionSummary += aRejRun;
  collectionSummary += "\" rejectedLHC=\"";
  collectionSummary += aRejLHC;
  collectionSummary += "\" rejectedDet=\"";
  collectionSummary += aRejDet;
  collectionSummary += "\" rejectedEvt=\"";
  collectionSummary += aRejEvt;
  collectionSummary += "\" />";
  
  // Open the output stream
  fout<<"  "<<collectionSummary<<"\n";  

  return kTRUE;
}

//___________________________________________________________________________
Bool_t AliXMLCollection::WriteBody(Int_t counter, const char* guid, const char* lfn, const char* turl, TEntryList* list) 
{
  //Writes the body of the xml collection
  TString listline;
  for(Int_t i = 0; i < list->GetN(); i++) {
    listline += list->GetEntry(i);
    listline += ",";
  }  
  listline = listline(0,listline.Length()-1);

  TString line0 = "<event name=\"";
  line0 += counter;
  line0 += "\">";
  
  TString line1 = "<file name=\"AliESDs.root\" ";
  line1 += "guid=\"";
  line1 += guid;
  line1 += "\" ";
  line1 += "lfn=\"";
  line1 += lfn;
  line1 += "\" ";
  line1 += "turl=\"";
  line1 += turl;
  line1 += "\" ";
  line1 += "evlist=\"";
  line1 += listline;
  line1 += "\"";
  line1 += " />";
  
  fout<<"    "<<line0<<"\n";
  fout<<"      "<<line1<<"\n";
  fout<<"    </event>\n";
  
  return kTRUE;
}
//___________________________________________________________________________
Bool_t AliXMLCollection::WriteBody(Int_t counter, const char* guid, const char *lfn, const char *turl, TEntryList* list, Int_t accSum, Int_t rejSum)
{
  //Writes the body of the xml collection with tag cuts summary
  TString listline;
  for(Int_t i = 0; i < list->GetN(); i++) {
    listline += list->GetEntry(i);
    listline += ",";
  }  
  listline = listline(0,listline.Length()-1);

  TString line0 = "<event name=\"";
  line0 += counter;
  line0 += "\">";
  
  TString line1 = "<file name=\"AliESDs.root\" ";
  line1 += "guid=\"";
  line1 += guid;
  line1 += "\" ";
  line1 += "lfn=\"";
  line1 += lfn;
  line1 += "\" ";
  line1 += "turl=\"";
  line1 += turl;
  line1 += "\" ";
  line1 += "evlist=\"";
  line1 += listline;
  line1 += "\" ";
  line1 += "cutsumm=\"";
  line1 += accSum;
  line1 += ",";
  line1 += rejSum;
  line1 += "\"";
  line1 += " />";
  
  fout<<"    "<<line0<<"\n";
  fout<<"      "<<line1<<"\n";
  fout<<"    </event>\n";
  
  return kTRUE;
  
}
//___________________________________________________________________________
Bool_t AliXMLCollection::Export() {
  //Closes the stream
  fout<<"  "<<"</collection>\n";
  fout<<"</alien>\n";

  fout.close();

  return kTRUE;
}

//___________________________________________________________________________
void AliXMLCollection::Reset() {
  // Reset file iterator.
  
  fEventListIter->Reset();
  fCurrent = 0;
}

//___________________________________________________________________________
TMap *AliXMLCollection::Next() {
  // Return next event file map.
  
  fCurrent = (TMap*)fEventListIter->Next();
  return fCurrent;
}

//___________________________________________________________________________
const char *AliXMLCollection::GetTURL(const char* filename) {
  // Get a file's transport URL (TURL). Returns 0 in case of error.
  
  if (fCurrent) {
    TMap *obj = (TMap*)fCurrent->GetValue(filename);
    if (obj) {
      if (obj->GetValue("turl")) {
	return ( ((TObjString*)obj->GetValue("turl"))->GetName());
      }
    }
  }
  AliError(Form("cannot get TURL of file %s",filename));
  return 0;
}

//___________________________________________________________________________
const char *AliXMLCollection::GetGUID(const char* filename) {
  // Get a file's transport UID. Returns 0 in case of error.
  
  if (fCurrent) {
    TMap *obj = (TMap*)fCurrent->GetValue(filename);
    if (obj) {
      if (obj->GetValue("guid")) {
	return ( ((TObjString*)obj->GetValue("guid"))->GetName());
      }
    }
  }
  AliError(Form("cannot get GUID of file %s",filename));
  return 0;
}

//___________________________________________________________________________
TEntryList *AliXMLCollection::GetEventList(const char *filename) const {
  // Get a file's event list. Returns 0 in case of error.

  if (fCurrent) {
    TMap *obj = (TMap *) fCurrent->GetValue(filename);
    if (obj) {
      if (obj->GetValue("evlist")) {
	return ((TEntryList *) obj->GetValue("evlist"));
      }
    }
  }
  AliError(Form("cannot get evelist of file %s", filename));
  return 0;
}

//___________________________________________________________________________
Bool_t AliXMLCollection::Remove(TMap * map) {
  // Return next event file map.
  if (fEventList->Remove(map)) {
    return kTRUE;
  } else {
    return kFALSE;
  }
}

//___________________________________________________________________________
const char *AliXMLCollection::GetLFN(const char* ) {
  // Get a file's LFN. Returns 0 in case of error.
  
  if (fCurrent) {
    TMap *obj = (TMap *) fCurrent->GetValue("");
    if (obj) {
      if (obj->GetValue("lfn")) {
	return (((TObjString *) obj->GetValue("lfn"))->GetName());
      }
    }
  }
  AliError("cannot get LFN");
  return 0;
}

//__________________________________________________________________________
const char *AliXMLCollection::GetCutSumm() {
  // Get a file's tag cuts summary. Returns 0 in case of error.
  
  if (fCurrent) {
    TMap *obj = (TMap *) fCurrent->GetValue("");
    if (obj) {
      if (obj->GetValue("cutsumm")) {
	return (((TObjString *) obj->GetValue("cutsumm"))->GetName());
      }
    }
  }
  AliError("cannot get Tag Cut Summary");
  return 0;

}
//__________________________________________________________________________
Bool_t AliXMLCollection::OverlapCollection(TGridCollection * comparator) {
  // return kTRUE if comparator overlaps with this
  if ((!comparator)) return kFALSE;
  
 loopagain:
  // loop over col1 and try to find it in col2
  this->Reset();
  // loop over all elements in reference (=this)
  TMap *overlapmap;
  while ((overlapmap = this->Next())) {
    comparator->Reset();
    Bool_t found = kFALSE;
    // try to find in the comparator collection
    while ((comparator->Next())) {
      TString s1 = this->GetLFN("");
      TString s2 = comparator->GetLFN("");
      if (s1 == s2) {
	found = kTRUE;
	break;
      }
    }
    if (!found) {
      this->Remove(overlapmap);
      goto loopagain;
    }
  }
  return kTRUE;
}

//___________________________________________________________________________
AliXMLCollection *AliXMLCollection::Open(const char *localcollectionfile) {
  // Static method used to create an Alien event collection, by reading
  // collection for the specified file.
  
  AliXMLCollection *collection = new AliXMLCollection(localcollectionfile);
  return collection;
}

//___________________________________________________________________________
void AliXMLCollection::ParseXML() {
  // Parse event file collection XML file.
  
  TXMLEngine xml;
  
  XMLDocPointer_t xdoc = xml.ParseFile(fXmlFile);
  if (!xdoc) {
    AliError(Form("cannot parse the xml file %s",fXmlFile.Data()));
    return;
  }

  XMLNodePointer_t xalien = xml.DocGetRootElement(xdoc);
  if (!xalien) {
    AliError(Form("cannot find the <alien> tag in %s",fXmlFile.Data()));
    return;
  }
  
  XMLNodePointer_t xcollection = xml.GetChild(xalien);
  if (!xcollection) {
    AliError(Form("cannot find the <collection> tag in %s",fXmlFile.Data()));
    return;
  }
  
  XMLNodePointer_t xevent = xml.GetChild(xcollection);;
  if (!xevent) {
    AliError(Form("cannot find the <event> tag in %s",fXmlFile.Data()));
    return;
  }
  
  do {
    if (xml.GetAttr(xevent, "name")) {
      TMap *files = new TMap();
            
      // files
      XMLNodePointer_t xfile = xml.GetChild(xevent);
      if (!xfile) continue;

      Bool_t firstfile=kTRUE;
      do {
	// here we have an event file
	// get the attributes;
	xml.GetAttr(xfile, "lfn");
	xml.GetAttr(xfile, "turl");
	
	TMap *attributes = new TMap();
	TObjString *oname = new TObjString(xml.GetAttr(xfile,"name"));
	TObjString *oturl = new TObjString(xml.GetAttr(xfile,"turl"));
	TObjString *olfn  = new TObjString(xml.GetAttr(xfile,"lfn"));
	TObjString *oguid = new TObjString(xml.GetAttr(xfile,"guid"));

	TObjString *oevlist;
	if (xml.GetAttr(xfile, "evlist"))
	  oevlist = new TObjString(xml.GetAttr(xfile, "evlist"));
	else
	  oevlist = 0;

	TObjString *otagsumm;
	if (xml.GetAttr(xfile, "cutsumm"))
	  otagsumm = new TObjString(xml.GetAttr(xfile, "cutsumm"));
	else 
	  otagsumm = 0;

	if (oevlist) {
	  AliDebug(1,Form("Collection: %s - turl: %s eventlist: %s",
                    fXmlFile.Data(),oturl->GetName(),oevlist->GetName()));
	  TEntryList *xmlevlist = new TEntryList(oturl->GetName(), oguid->GetName());
	  if (strcmp(oevlist->GetName(),"") != 0) {
	    TString stringevlist = oevlist->GetName();
	    TObjArray *evlist = stringevlist.Tokenize(",");
	    for (Int_t n = 0; n < evlist->GetEntries(); n++)  xmlevlist->Enter(atol(((TObjString *) evlist->At(n))->GetName()));
	  }
	  attributes->Add(new TObjString("evlist"), xmlevlist);
	}
	
	attributes->Add(new TObjString("name"),oname);
	attributes->Add(new TObjString("turl"),oturl);
	attributes->Add(new TObjString("lfn"),olfn);
	attributes->Add(new TObjString("guid"),oguid);
	if (otagsumm)
	  attributes->Add(new TObjString("cutsumm"),otagsumm);
	files->Add(new TObjString(xml.GetAttr(xfile,"name")) , attributes);
	
	// we add the first file always as a file without name to the map
	if (firstfile) {
	  files->Add(new TObjString(""),attributes);
	  firstfile=kFALSE;
	}
      } while ((xfile = xml.GetNext(xfile)));
      fEventList->Add(files);
    }
    else {
      if (xml.GetAttr(xevent, "acceptedEvents")) {
	// Read list summary
	fAcceptedEvents = atoi(xml.GetAttr(xevent, "acceptedEvents"));
	fTotalEvents = atoi(xml.GetAttr(xevent, "totalEvent"));
	fRejectedRun = atoi(xml.GetAttr(xevent, "rejectedRun"));
	fRejectedLHC = atoi(xml.GetAttr(xevent, "rejectedLHC"));
	fRejectedDet = atoi(xml.GetAttr(xevent, "rejectedDet"));
	fRejectedEvt = atoi(xml.GetAttr(xevent, "rejectedEvt"));
      }
    }
  } while ((xevent =  xml.GetNext(xevent)));
}

Bool_t AliXMLCollection::GetCollectionSummary(Int_t  *aTot, Int_t  *aAcc, Int_t  *aRejRun, Int_t  *aRejLHC, Int_t  *aRejDet, Int_t  *aRejEvt) const
{
  // Return read list summary
  *aTot = fTotalEvents;
  *aAcc = fAcceptedEvents;
  *aRejRun = fRejectedRun;
  *aRejLHC = fRejectedLHC;
  *aRejDet = fRejectedDet;
  *aRejEvt = fRejectedEvt;
  return kTRUE;
}
