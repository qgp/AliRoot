// @(#) $Id$

#ifndef ALIL3TRACKARRAY_H
#define ALIL3TRACKARRAY_H

#include "AliL3RootTypes.h"

class AliL3ConfMapTrack;
class AliL3Track;
class AliL3TrackSegmentData;

class AliL3TrackArray {

 private:

  Char_t fTrackType; //track type
  Int_t fSize; //size of arra
  Bool_t *fIsPresent;//!
  Int_t fNAbsent; //ntracks absent

  AliL3Track **fTrack;//!
  Int_t fNTracks; //ntracks in

  UInt_t WriteConfMapTracks(AliL3TrackSegmentData* tr); 
  void DeleteArray();

 public:
  AliL3TrackArray();
  AliL3TrackArray(Int_t ntrack);
  AliL3TrackArray(char* tracktype,Int_t ntrack);
  AliL3TrackArray(char* tracktype);
  virtual ~AliL3TrackArray();
  Int_t GetTrackType(){return fTrackType;}
  Int_t GetSize() const {return fSize;}
  Bool_t SetSize(Int_t newsize=2000);

  Int_t GetNPresent() const {return (fNTracks- fNAbsent);}
  Int_t GetNTracks() const {return fNTracks;}
  AliL3Track *NextTrack();
  AliL3Track *GetCheckedTrack(Int_t t){if(fIsPresent[t]) return fTrack[t]; return 0;}
  AliL3Track *GetTrack(Int_t t){return fTrack[t];}

  void Remove(Int_t track); 
  void RemoveLast() {fNTracks--;}
  void Compress();
  void Reset();
  void QSort();
  void QSort( AliL3Track **a, Int_t first, Int_t last);
  Int_t TrackCompare(AliL3Track *a, AliL3Track *b) const;

  void FillTracks(Int_t ntracks, AliL3TrackSegmentData* tr,Int_t slice); //Fill tracks and transform
  void FillTracks(Int_t ntracks, AliL3TrackSegmentData* tr); //Fill tracks
  UInt_t WriteTracks(AliL3TrackSegmentData* tr); //Write tracks
  UInt_t WriteTracks(UInt_t & ntracks,AliL3TrackSegmentData* tr); //Write tracks
  UInt_t GetOutSize();
  UInt_t GetOutCount(){return (UInt_t) GetNPresent();}
  void AddTracks(AliL3TrackArray *newtrack,Bool_t remove_old=kTRUE,Int_t slice=-1);//add all Tracks to this 
  void AddLast(AliL3Track *track);

  ClassDef(AliL3TrackArray,1) //Track array class
};

#endif
