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

#include <stdio.h>
#include <stdlib.h>
#include <Riostream.h>
#include <TObjArray.h>
#include <TRandom.h>
#include <TMath.h>

#include "AliITSpList.h"

//______________________________________________________________________

ClassImp(AliITSpList);
//______________________________________________________________________
AliITSpList::AliITSpList(){
    // Default constructor
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    A zeroed/empty AliITSpList class.

    fNi = 0;
    fNj = 0;
    fa  = 0;
}
//______________________________________________________________________
AliITSpList::AliITSpList(Int_t imax,Int_t jmax){
    // Standard constructor
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    A setup AliITSpList class.

    fNi = imax;
    fNj = jmax;
    fEnteries = 0;
    fa  = new TObjArray(fNi*fNj); // elements are zeroed by 
                                  // TObjArray creator
}
//______________________________________________________________________
AliITSpList::~AliITSpList(){
    // Default destructor
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    a properly destroyed class

    for(Int_t i=0;i<GetMaxIndex();i++) if(fa->At(i)!=0){
        delete fa->At(i);
        fa->AddAt(0,i); // zero content
    } // end for i && if
    fNi = 0;
    fNj = 0;
    delete fa;
    fa  = 0;
    fEnteries = 0;
}
//______________________________________________________________________
void AliITSpList::ClearMap(){
    // Delete all AliITSpListItems and zero TObjArray.
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    A zeroed AliITSpList class.

    fa->Delete();
    /*
    for(Int_t i=0;i<GetMaxIndex();i++) if(fa->At(i)!=0){
        delete fa->At(i);
        fa->AddAt(0,i); // zero content
    } // end for i && if
    */
    fEnteries = 0;
}
//______________________________________________________________________
void AliITSpList::DeleteHit(Int_t i,Int_t j){
    // Delete a particular AliITSpListItems and zero TObjArray.
    // Inputs:
    //    Int_t i   Row number
    //    Int_t j   Columns number
    // Outputs:
    //    none.
    // Return:
    //    none.
    Int_t k = GetIndex(i,j);

    if(fa->At(k)!=0){
        delete fa->At(k);
        fa->AddAt(0,k); // zero content
    } // end for i && if
    if(k==fEnteries-1) fEnteries--;
}
//______________________________________________________________________
AliITSpList& AliITSpList::operator=(const AliITSpList &source){
    // = operator
    // Inputs:
    //    const AliITSpList &source    A AliITSpList object.
    // Outputs:
    //    none.
    // Return:
    //    A copied AliITSpList object.

    if(this == &source) return *this;

    if(this->fa!=0){ // if this->fa exists delete it first.
        for(Int_t i=0;i<GetMaxIndex();i++) if(fa->At(i)!=0){
            delete fa->At(i);
            fa->AddAt(0,i); // zero content
        } // end for i && if
        delete this->fa;
    } // end if this->fa!=0
    this->fNi = source.fNi;
    this->fNj = source.fNj;
    this->fa = new TObjArray(*(source.fa));
    this->fEnteries = source.fEnteries;

    return *this;
}
//______________________________________________________________________
AliITSpList::AliITSpList(AliITSpList &source){
    // Copy operator
    // Inputs:
    //    AliITSpList &source   A AliITSpList Object
    // Outputs:
    //    none.
    // Return:
    //    A copied AliITSpList object

    *this = source;
}
//______________________________________________________________________
void AliITSpList::AddItemTo(Int_t fileIndex, AliITSpListItem *pl) {
    // Adds the contents of pl to the list with track number off set given by
    // fileIndex.
    // Creates the AliITSpListItem if needed.
    // Inputs:
    //    Int_t fileIndex      track number offset value
    //    AliITSpListItem *pl  an AliITSpListItem to be added to this class.
    // Outputs:
    //    none.
    // Return:
    //    none.
    Int_t index = pl->GetIndex();

    if( fa->At( index ) == 0 ) { // most create AliITSpListItem
        fa->AddAt(new AliITSpListItem(-2,-1,pl->GetModule(),index,0.0),index);
    } // end if
 
    ((AliITSpListItem*)(fa->At(index)))->AddTo( fileIndex,pl);
    if(index>=fEnteries) fEnteries = index +1;
}
//______________________________________________________________________
void AliITSpList::AddSignal(Int_t i,Int_t j,Int_t trk,Int_t ht,Int_t mod,
                       Double_t signal){
    // Adds a Signal value to the TObjArray at i,j. Creates the AliITSpListItem
    // if needed.
    // Inputs:
    //    Int_t i         Row number for this signal
    //    Int_t j         Column number for this signal
    //    Int_t trk       Track number creating this signal
    //    Int_t ht        Hit number creating this signal
    //    Int_t mod       The module where this signal is in
    //    Double_t signal The signal (ionization)
    // Outputs:
    //    none.
    // Return:
    //    none.
    Int_t index = GetIndex(i,j);

    if(GetpListItem(index)==0){ // most create AliITSpListItem
        fa->AddAt(new AliITSpListItem(trk,ht,mod,index,signal),index);
    }else{ // AliITSpListItem exists, just add signal to it.
        GetpListItem(index)->AddSignal(trk,ht,mod,index,signal);
    } // end if
    if(index>=fEnteries) fEnteries = index +1;
}
//______________________________________________________________________
void AliITSpList::AddNoise(Int_t i,Int_t j,Int_t mod,Double_t noise){
    // Adds a noise value to the TObjArray at i,j. Creates the AliITSpListItem
    // if needed.
    // Inputs:
    //    Int_t i        Row number for this noise
    //    Int_t j        Column number for this noise
    //    Double_t noise The noise signal value.
    // Outputs:
    //    none.
    // Return:
    //    none.
    Int_t index = GetIndex(i,j);

    if(GetpListItem(index)==0){ // most create AliITSpListItem
        fa->AddAt(new AliITSpListItem(mod,index,noise),index);
    }else{ // AliITSpListItem exists, just add signal to it.
        GetpListItem(index)->AddNoise(mod,index,noise);
    } // end if
    if(index>=fEnteries) fEnteries = index +1;
}
//______________________________________________________________________

ClassImp(AliITSpListItem)
//______________________________________________________________________
AliITSpListItem::AliITSpListItem(){
    // Default constructor
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    A zeroed/empty AliITSpListItem class.

    fmodule = -1;
    findex  = -1;
    for(Int_t i=0;i<this->fkSize;i++){
        this->fTrack[i]  = -2;
        this->fHits[i]   = -1;
        this->fSignal[i] = 0.0;
    } // end if i
    fTsignal = 0.0;
    fNoise   = 0.0;
    fSignalAfterElect = 0.0;
}
//______________________________________________________________________
AliITSpListItem::AliITSpListItem(Int_t module,Int_t index,Double_t noise){
    // Standard noise constructor
    // Inputs:
    //    Int_t module   The module where this noise occurred
    //    Int_t index    The cell index where this noise occurred
    //    Double_t noise The value of the noise.
    // Outputs:
    //    none.
    // Return:
    //    A setup and noise filled AliITSpListItem class.

    this->fmodule    = module;
    this->findex     = index;
    for(Int_t i=0;i<this->fkSize;i++){
        this->fTrack[i]  = -2;
        this->fSignal[i] = 0.0;
        this->fHits[i]   = -1;
    } // end if i
    this->fTsignal = 0.0;
    this->fSignalAfterElect = 0.0;
    this->fNoise   = noise;
}
//______________________________________________________________________
AliITSpListItem::AliITSpListItem(Int_t track,Int_t hit,Int_t module,
                               Int_t index,Double_t signal){
    // Standard signal constructor
    // Inputs:
    //    Int_t track     The track number which produced this signal
    //    Int_t hit       The hit number which produced this signal
    //    Int_t module    The module where this signal occurred
    //    Int_t index     The cell index where this signal occurred
    //    Double_t signal The value of the signal (ionization)
    // Outputs:
    //    none.
    // Return:
    //    A setup and signal filled  AliITSpListItem class.

    this->fmodule    = module;
    this->findex     = index;
    this->fTrack[0]  = track;
    this->fHits[0]   = hit;
    this->fSignal[0] = signal;
    for(Int_t i=1;i<this->fkSize;i++){
        this->fTrack[i]  = -2;
        this->fSignal[i] = 0.0;
        this->fHits[i]   = -1;
    } // end if i
    this->fTsignal = signal;
    this->fNoise   = 0.0;
    this->fSignalAfterElect   = 0.0;
}
//______________________________________________________________________
AliITSpListItem::~AliITSpListItem(){
    // Destructor
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    A properly destroyed AliITSpListItem class.

}
//______________________________________________________________________
AliITSpListItem& AliITSpListItem::operator=(const AliITSpListItem &source){
    // = operator
    // Inputs:
    //    AliITSpListItem &source   A AliITSpListItem Object
    // Outputs:
    //    none.
    // Return:
    //    A copied AliITSpListItem object
    Int_t i;

    if(this == &source) return *this;

    this->fmodule = source.fmodule;
    this->findex  = source.findex;
    for(i=0;i<this->fkSize;i++){
        this->fTrack[i]  = source.fTrack[i];
        this->fSignal[i] = source.fSignal[i];
        this->fHits[i]   = source.fHits[i];
    } // end if i
    this->fTsignal = source.fTsignal;
    this->fNoise   = source.fNoise;
    this->fSignalAfterElect   = source.fSignalAfterElect;
    /*
    cout <<"this fTrack[0-9]=";
    for(i=0;i<this->fkSize;i++) cout <<this->fTrack[i]<<",";
    cout <<" fHits[0-9]=";
    for(i=0;i<this->fkSize;i++) cout <<this->fHits[i]<<",";
    cout <<" fSignal[0-9]=";
    for(i=0;i<this->fkSize;i++) cout <<this->fSignal[i]<<",";
    cout << endl;
    cout <<"source fTrack[0-9]=";
    for(i=0;i<this->fkSize;i++) cout <<source.fTrack[i]<<",";
    cout <<" fHits[0-9]=";
    for(i=0;i<this->fkSize;i++) cout <<source.fHits[i]<<",";
    cout <<" fSignal[0-9]=";
    for(i=0;i<this->fkSize;i++) cout <<source.fSignal[i]<<",";
    cout << endl;
    */
    return *this;
}
//______________________________________________________________________
AliITSpListItem::AliITSpListItem(AliITSpListItem &source){
    // Copy operator
    // Inputs:
    //    AliITSpListItem &source   A AliITSpListItem Object
    // Outputs:
    //    none.
    // Return:
    //    A copied AliITSpListItem object

    *this = source;
}
//______________________________________________________________________
void AliITSpListItem::AddSignal(Int_t track,Int_t hit,Int_t module,
                               Int_t index,Double_t signal){
    // Adds this track number and signal to the pList and orders them
    // Inputs:
    //    Int_t track     The track number which produced this signal
    //    Int_t hit       The hit number which produced this signal
    //    Int_t module    The module where this signal occurred
    //    Int_t index     The cell index where this signal occurred
    //    Double_t signal The value of the signal (ionization)
    // Outputs:
    //    none.
    // Return:
    //    none.
    Int_t    i,j,trk,hts;
    Double_t sig;
    Bool_t   flg=kFALSE;

    if(findex!=index || fmodule!=module) 
        Warning("AddSignal","index=%d != findex=%d or module=%d != fmodule=%d",
                 index,findex,module,fmodule);
    fTsignal += signal; // Keep track of sum signal.

    //    for(i=0;i<fkSize;i++) if( track==fTrack[i] && hit==fHits[i] ){
    for(i=0;i<fkSize;i++) if( track==fTrack[i]  ){
        fSignal[i] += signal;
        flg = kTRUE;
    } // end for i & if.
    //cout << "track="<<track<<endl;
    if(flg){ // resort arrays.  
        for(i=1;i<fkSize;i++){
            j = i;
            while(j>0 && fSignal[j]>fSignal[j-1]){
                trk = fTrack[j-1];
                hts = fHits[j-1];
                sig = fSignal[j-1];
                fTrack[j-1]  = fTrack[j];
                fHits[j-1]   = fHits[j];
                fSignal[j-1] = fSignal[j];                
                fTrack[j]  = trk;
                fHits[j]   = hts;
                fSignal[j] = sig;
		//cout << "#fTrack["<<j-1<<"]="<<fTrack[j-1]<< " fTrack["<<
		// j<<"]="<<fTrack[j]<<endl;
                j--;
            } // end while
        } // end if i
        return;
    } // end if added to existing and resorted array

    // new entry add it in order.
    // if this signal is <= smallest then don't add it.
    if(signal <= fSignal[fkSize-1]) return;
    for(i=fkSize-2;i>=0;i--){
        if(signal > fSignal[i]){
            fSignal[i+1] = fSignal[i];
            fTrack[i+1]  = fTrack[i];
            fHits[i+1]   = fHits[i];
        }else{
            fSignal[i+1] = signal;
            fTrack[i+1]  = track;
            fHits[i+1]   = hit;
            return; // put it in the right place, now exit.
        } //  end if
	//cout << "$fTrack["<<i+1<<"]="<<fTrack[i+1]<< " fTrack["<<i<<"]="
	//<<fTrack[i]<< " fHits["<<i+1<<"]="<<fHits[i+1]<< " fHits["<<i<<"]="
	//<<fHits[i]<< " fSignal["<<i+1<<"]="<<fSignal[i+1]<< " fSignal["<<i
	//<<"]="<<fSignal[i]<<endl;
    } // end if; end for i
    // Still haven't found the right place. Must be at top of list.
    fSignal[0] = signal;
    fTrack[0]  = track;
    fHits[0]   = hit;
    //cout << "$fTrack["<<0<<"]="<<fTrack[0]<<" fHits["<<0<<"]="<<fHits[0]
    //<<" fSignal["<<0<<"]="<<fSignal[0]<<endl;
    return;
}
//______________________________________________________________________
void AliITSpListItem::AddNoise(Int_t module,Int_t index,Double_t noise){
    // Adds noise to this existing list.
    // Inputs:
    //    Int_t module   The module where this noise occurred
    //    Int_t index    The cell index where this noise occurred
    //    Double_t noise The value of the noise.
    // Outputs:
    //    none.
    // Return:
    //    none.

    if(findex!=index || fmodule!=module) 
        Warning("AddNoise","index=%d != findex=%d or module=%d != fmodule=%d",
            index,findex,module,fmodule);
    fNoise += noise; // Keep track of sum signal.
}
//______________________________________________________________________
void AliITSpListItem::AddSignalAfterElect(Int_t module,Int_t index,Double_t signal){
    // Adds signal after electronics to this existing list.
    // Inputs:
    //    Int_t module   The module where this noise occurred
    //    Int_t index    The cell index where this noise occurred
    //    Double_t signal The value of the signal.
    // Outputs:
    //    none.
    // Return:
    //    none.

    if(findex!=index || fmodule!=module) 
        Warning("AddSignalAfterElect","index=%d != findex=%d or module=%d "
		"!= fmodule=%d",index,findex,module,fmodule);
    fSignalAfterElect += signal; // Keep track of sum signal.
}
//______________________________________________________________________
void AliITSpListItem::Add(AliITSpListItem *pl){
    // Adds the contents of pl to this
    // pl could come from different module and index 
    // Inputs:
    //    AliITSpListItem *pl  an AliITSpListItem to be added to this class.
    // Outputs:
    //    none.
    // Return:
    //    none.
    Int_t i;
    Double_t sig  = 0.0;
    Double_t sigT = 0.0;

    for(i=0;i<pl->GetNsignals();i++){
        sig = pl->GetSignal(i); 
        if( sig <= 0.0 ) break; // no more signals
        AddSignal(pl->GetTrack(i),pl->GetHit(i),fmodule,findex,sig);
        sigT += sig;
    } // end for i
    fTsignal += (pl->fTsignal - sigT);
    fNoise   += pl->fNoise;
    return;
}
//______________________________________________________________________
void AliITSpListItem::AddTo(Int_t fileIndex,AliITSpListItem *pl){
    // Adds the contents of pl to this with track number off set given by
    // fileIndex.
    // Inputs:
    //    Int_t fileIndex      track number offset value
    //    AliITSpListItem *pl  an AliITSpListItem to be added to this class.
    // Outputs:
    //    none.
    // Return:
    //    none.
    Int_t i,trk;
    Double_t sig  = 0.0;

    Int_t module = pl->GetModule();
    Int_t index  = pl->GetIndex();
    for(i=0;i<pl->GetNsignals();i++){
        sig = pl->GetSignal(i); 
        if( sig <= 0.0 ) break; // no more signals
        trk = pl->GetTrack(i);
        trk += fileIndex; 
        AddSignal(trk,pl->GetHit(i),module,index,sig);
    } // end for i
    fSignalAfterElect += (pl->fSignalAfterElect + pl->fNoise - fNoise);
    fNoise = pl->fNoise;
    return;
}
//______________________________________________________________________
Int_t AliITSpListItem::ShiftIndex(Int_t in,Int_t trk){
    // Shift an index number to occupy the upper four bits. No longer used.
    // Inputs:
    //    Int_t in   The file number
    //    Int_t trk  The track number
    // Outputs:
    //    none.
    // Return:
    //    Int_t The track number with the file number in the upper bits.
    Int_t si = sizeof(Int_t) * 8;
    UInt_t uin,utrk; // use UInt_t to avoid interger overflow-> goes negative.

    uin = in;
    utrk = trk;
    for(Int_t i=0;i<si-4;i++) uin *= 2;
    uin += utrk;
    in = uin;
    return in;
}
//______________________________________________________________________
void AliITSpListItem::Print(ostream *os){
    //Standard output format for this class
    // Inputs:
    //    ostream *os  The output stream
    // Outputs:
    //    none.
    // Return:
    //    none.
    Int_t i;

    *os << fmodule <<","<<findex<<",";
    *os << fkSize <<",";
    for(i=0;i<fkSize;i++) *os << fTrack[i] <<",";
    for(i=0;i<fkSize;i++) *os << fHits[i] <<",";
    for(i=0;i<fkSize;i++) *os << fSignal[i] <<",";
    *os << fTsignal <<","<< fNoise << "," << fSignalAfterElect;
}
//______________________________________________________________________
void AliITSpListItem::Read(istream *is){
    // Standard output streaming function.
    // Inputs:
    //    istream *is The input stream
    // Outputs:
    //    none.
    // Return:
    //    none.
    Int_t i,iss;

    *is >> fmodule >> findex;
    *is >> iss; // read in fkSize
    for(i=0;i<fkSize&&i<iss;i++) *is >> fTrack[i];
    for(i=0;i<fkSize&&i<iss;i++) *is >> fHits[i];
    for(i=0;i<fkSize&&i<iss;i++) *is >> fSignal[i];
    *is >> fTsignal >> fNoise >> fSignalAfterElect;
}
//______________________________________________________________________
ostream &operator<<(ostream &os,AliITSpListItem &source){
    // Standard output streaming function.
    // Inputs:
    //    ostream &os             The output stream
    //    AliITSpListItem &source The AliITSpListItem object to be written out.
    // Outputs:
    //    none.
    // Return:
    //    ostream  The output stream

    source.Print(&os);
    return os;
}
//______________________________________________________________________
istream &operator>>(istream &os,AliITSpListItem &source){
    // Standard output streaming function.
    // Inputs:
    //    istream os              The input stream
    //    AliITSpListItem &source The AliITSpListItem object to be inputted
    // Outputs:
    //    none.
    // Return:
    //    istream The input stream.

    source.Read(&os);
    return os;
}

