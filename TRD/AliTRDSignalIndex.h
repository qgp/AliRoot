#ifndef ALITRDSIGNALINDEX_H
#define ALITRDSIGNALINDEX_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
 
#include "TObject.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  General container for data from TRD detector segments                 //
//  Adapted from AliDigits, origin M.Ivanov                               //
//                                                                        //
//  Author:                                                               //
//    Mateusz Ploskon (ploskon@ikf.uni-frankfurt.de)                      //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class AliTRDSignalIndex : public TObject
{
protected:

  union RowCol{
    Short_t rc;
    struct{
      UChar_t col;
      Char_t row;
    }s;
  };

public:

  AliTRDSignalIndex(); 
  AliTRDSignalIndex(Int_t nrow, Int_t ncol,Int_t ntime);
  AliTRDSignalIndex(const AliTRDSignalIndex &d);
  virtual ~AliTRDSignalIndex();
  AliTRDSignalIndex &operator=(const AliTRDSignalIndex &d); 

  void     Copy(TObject &d) const;
  void     Allocate(const Int_t nrow, const Int_t ncol, const Int_t ntime);

  void     Reset();
  void     ResetContentConditional(const Int_t nrow, const Int_t ncol, const Int_t ntime);
  void     ResetContent();
  void     ResetCounters();
  void     ResetTbinCounter() const { };

  void     ResetArrays();

  // Store the index row-column as an interesting one
  inline void AddIndexRC(const Int_t row, const Int_t col);
  // Get the next pad (row and column) and return kTRUE on success
  inline Bool_t NextRCIndex(Int_t &row, Int_t &col); 
  // Get the next timebin of a pad (row and column) and return kTRUE on success
  Bool_t   NextRCTbinIndex(Int_t &row, Int_t &col, Int_t &tbin); 
  // Get the next active timebin and return kTRUE on success
  Bool_t   NextTbinIndex(Int_t &tbin); 

  Bool_t CheckSorting(Int_t &row, Int_t &col);

  Int_t    GetCurrentRow() const  { return fCurrRow; }
  Int_t    GetCurrentCol() const  { return fCurrCol; }
  Int_t    GetCurrentTbin() const { return fCurrTbin; }

  Bool_t   IsBoolIndex(Int_t row, Int_t col) const {return fBoolIndex[row*fNcols+col];};
  void     InitSortedIndex();

  // Clear the array, actually destroy and recreate w/o allocating
  void     ClearAll(); 
  // Return kTRUE if array allocated and there is no need to call allocate
  Bool_t   IsAllocated() const    { if (!fBoolIndex)    return kFALSE; 
                                    if (fMaxLimit <= 0) return kFALSE; 
                                    else                return kTRUE;}

  void     SetSM(const Int_t ix)        { fSM      =    ix; }
  void     SetStack(const Int_t ix)     { fStack   =    ix; }
  void     SetLayer(const Int_t ix)     { fLayer   =    ix; }
  void     SetDetNumber(const Int_t ix) { fDet     =    ix; }
  
  Int_t    GetDetNumber() const   { return fDet;      } // Get Det number
  Int_t    GetLayer() const       { return fLayer;    } // Layer position of the chamber in TRD
  Int_t    GetStack() const       { return fStack;    } // Stack position of the chamber in TRD
  Int_t    GetSM() const          { return fSM;       } // Super module of the TRD
  Short_t *GetArray() const       { return (Short_t*)fSortedIndex; } // Get the array pointer for god knows what reason
  Int_t    GetNoOfIndexes() const { return fCountRC-1;  }

  Bool_t   HasEntry() const       { return fCountRC > 1 ? kTRUE : kFALSE; } // Return status if has an entry

  Int_t    GetNrow() const        { return fNrows;    } // Get Nrows
  Int_t    GetNcol() const        { return fNcols;    } // Get Ncols
  Int_t    GetNtime() const       { return fNtbins;   } // Get Ntbins

 private:

  Int_t     fDet;                //  Detector number
  Int_t     fLayer;              //  Layer position in the full TRD
  Int_t     fStack;              //  Stack position in the full TRD
  Int_t     fSM;                 //  Super module - position in the full TRD

  Bool_t   *fBoolIndex;          //  Indices
  RowCol   *fSortedIndex;        //  Sorted indices
  Int_t     fMaxLimit;           //  Max number of things in the array
  Int_t     fPositionRC;         //  Position in the SortedIndex
  Int_t     fCountRC;            //  the number of added rc combinations
  Bool_t    fSortedWasInit;      //  Was SortedIndex initialized?

  Int_t     fCurrRow;            //  Last Row read out of SortedIndex
  Int_t     fCurrCol;            //  Last Col read out of SortedIndex
  Int_t     fCurrTbin;           //  Last outgiven Tbin
  
  Int_t     fNrows;              //  Number of rows in the chamber
  Int_t     fNcols;              //  Number of cols in the chamber
  Int_t     fNtbins;             //  Number of tbins in the chamber 

  ClassDef(AliTRDSignalIndex,2)  //  Data container for one TRD detector segment

};

void AliTRDSignalIndex::AddIndexRC(const Int_t row, const Int_t col)
{
  //
  // Adds RC combination to array
  //

  const Int_t num=row*fNcols+col;
  if(fBoolIndex[num])return;
  fBoolIndex[num]=kTRUE;
  fSortedIndex[fCountRC].s.col=col;
  fSortedIndex[fCountRC].s.row=row;
  fCountRC++;
}

Bool_t AliTRDSignalIndex::NextRCIndex(Int_t &row, Int_t &col)
{
  //
  // Returns next used RC combination
  //

  if(fSortedIndex[fPositionRC].rc>-1){
    row = fCurrRow = fSortedIndex[fPositionRC].s.row;
    col = fCurrCol = fSortedIndex[fPositionRC].s.col;
    fPositionRC++;
    return kTRUE;
  }
  else
    return CheckSorting(row, col);
}

#endif

/*
Comment from 22 Dec 2008

The structure of the Index was changed. Now no Tbin is saved anymore,
only RC combination are saved! (reasons see below)

For the readout, all tbins for a RC combination must be read out to find 
the time bin of signal > 0.

THE WRITING PROCEDURE:
AddIndexTBin is now obsolate, use AddIndexRC instead as AddIndexTBin will
be deleted in future.

example that gives exactely the same output as before:
as it was: 
           AliTRDSignalIndexes *indexes;
           AliTRDarrayADC *Signal; //or AliTRDarraySignal *Signal;
	   if(Signal->GetDataB(row, col, time)>0)
               indexes->AddIndexTBin(row, col, time);

as it should be from no on: 
           AliTRDSignalIndexes *indexes;
           AliTRDarrayADC *Signal; //or AliTRDarraySignal *Signal;
	   if(Signal->GetDataB(row, col, time)>0)
               indexes->AddIndexRC(row, col);



THE READING PROCEDURE:
In most cases you can leave anything as it is.
See more in the example.

example:
as it was: 
           AliTRDSignalIndexes *indexes;
           AliTRDarraySignal *Signal;
           while(indexes->NextRCTbinIndex(row, col, time)) 
           {...}

as it should be from no on to get the exactely the same output as before: 
           AliTRDSignalIndexes *indexes;
           AliTRDarraySignal *Signal;
           while(indexes->NextRCTbinIndex(row, col, time)) 
              if(Signal->GetData(row, col, time)>0)
                 {...}

as it should be idealy:
           AliTRDSignalIndexes *indexes;
           AliTRDarraySignal *Signal;
           for(time = 0; time < Ntime; time++)
              while(indexes->NextRCIndex(row, col, time)) 
                 if(Signal->GetData(row, col, time)>0)
                    {...}


REASON OF THE CHANGES:

The array saved the information nicely, but it turned out that sorting 
the array by column would have many benefits.
I.e. it is crucial for fivePadClusters and it if much faster to allocate.
But the sorting is not fast if the tbin is also saved.
Moreover the tbin information was alsmost useless because, 
whenever an RC index existed, many of the possible tbins where used.

Theodor Rascanu

*/
