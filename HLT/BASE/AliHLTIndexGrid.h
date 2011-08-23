//-*- Mode: C++ -*-
// $Id$
#ifndef ALIHLTINDEXGRID_H
#define ALIHLTINDEXGRID_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/// @file   AliHLTIndexGrid.h
/// @author Matthias Richter
/// @date   2011-08-14
/// @brief  Index grid for 3 dimensional coordinates
///

#include "AliHLTDataTypes.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <memory>

template <typename T, typename V>
class AliHLTIndexGrid {
 public:
  AliHLTIndexGrid(T maxX, T stepX,
		  T maxY, T stepY,
		  T maxZ, T stepZ,
		  int initialDataSize=-1)
    : fMaxX(maxX)
    , fStepX(stepX)
    , fMaxY(maxY)
    , fStepY(stepY)
    , fMaxZ(maxZ)
    , fStepZ(stepZ)
    , fDimX(0)
    , fDimY(0)
    , fDimZ(0)
    , fCells(NULL)
    , fCellDimension(0)
    , fData(NULL)
    , fDataDimension(initialDataSize)
    , fCount(0)
    , fIterator()
    , fIteratorEnd()
  {
    // constructor
    if (fMaxX>0. && fMaxY>0. && fMaxZ>0 &&
	fStepX>0. && fStepY>0. && fStepZ>0) {
      fDimX=(int)ceil(fMaxX/fStepX);
      fDimY=(int)ceil(fMaxY/fStepY);
      fDimZ=(int)ceil(fMaxZ/fStepZ);

      fCellDimension=fDimX*fDimY*fDimZ;
      fCells=new AliHLTIndexGridCell[fCellDimension];
      if (fDataDimension<0) fDataDimension=fgkDefaultDataSize;
      fData=new V[fDataDimension];
      Clear();
    }
  }

  virtual ~AliHLTIndexGrid() {
    // destructor
    if (fData) delete [] fData;
    if (fCells) delete [] fCells;
  }

  // for now array of spacepoint ids
  typedef V ValueType;

  int GetDimensionX() const {return fDimX;}
  int GetDimensionY() const {return fDimY;}
  int GetDimensionZ() const {return fDimZ;}
  int GetXIndex(T x) const {
    if (x>fMaxX) return fDimX-1;
    if (x<0) return 0;
    return (int)(x/fStepX);
  }
  int GetYIndex(T y) const {
    if (y>fMaxY) return fDimY-1;
    if (y<0) return 0;
    return (int)(y/fStepY);
  }
  int GetZIndex(T z) const {
    if (z>fMaxZ) return fDimZ-1;
    if (z<0) return 0;
    return (int)(z/fStepZ);
  }
  T GetLowerBoundX(int cell) const {
    if (fDimX==0 || fDimY==0 ||fDimZ==0) return 0.;
    int index=cell/(fDimY*fDimZ);
    return index*fStepX;
  }
  T GetLowerBoundY(int cell) const {
    if (fDimX==0 || fDimY==0 ||fDimZ==0) return 0.;
    int index=cell%(fDimY*fDimZ); index/=fDimZ;
    return index*fStepY;
  }
  T GetLowerBoundZ(int cell) const {
    if (fDimX==0 || fDimY==0 ||fDimZ==0) return 0.;
    int index=cell%(fDimY*fDimZ); index%=fDimZ;
    return index*fStepZ;
  }
  int GetCellIndex(T x, T y, T z) const {
    return GetXIndex(x)*fDimY*fDimZ + (y<0?0:GetYIndex(y))*fDimZ + (z<0?0:GetZIndex(z));
  }
  int GetNumberOfSpacePoints(int index, int endIndex) const {
    if (!fCells) return 0;
    int count=0;
    for (int cell=index; cell<endIndex && cell<fCellDimension && count<fCount; cell++) if (fCells[cell].fCount>0) count+=fCells[cell].fCount;
    return count;
  }

  // increment counter of the cell where the spacepoint is
  int CountSpacePoint(T x, T y, T z) {
    // increment counter of the cell where the spacepoint is
    int cell=GetCellIndex(x, y, z);
    if (cell<0 || !fCells || cell>=fCellDimension) return -EFAULT;
    if (fCells[cell].fCount<0) fCells[cell].fCount=1;
    else fCells[cell].fCount++;
    return 0;
  }


  // add spacepoint, all spacepoints must have been counted before
  int AddSpacePoint(ValueType t, T x, T y, T z) {
    // add spacepoint, all spacepoints must have been counted before
    int cell=GetCellIndex(x, y, z);
    if (cell<0 || !fCells || cell>=fCellDimension) return -EFAULT;
    if (fCells[cell].fFilled==fCells[cell].fCount) return -ENOSPC;
    if (fCells[cell].fStartIndex<0 && IndexCells()<0) return -EACCES;
    int offset=fCells[cell].fStartIndex+fCells[cell].fFilled;
    fData[offset]=t;
    fCells[cell].fFilled++;
    fCount++;
    return 0;
  }

  void Clear(const char* /*option*/="") {
    // clear internal data
    if (fCells) memset(fCells, 0xff, fCellDimension*sizeof(AliHLTIndexGridCell));
    if (fData) memset(fData, 0, fDataDimension*sizeof(V));
    fCount=0;
  }

  void Print(const char* /*option*/="") {
  // print info
  bool bPrintEmpty=false;
  std::stringstream str;
  str << "AliHLTIndexGrid: " << (fCells?fCellDimension:0) << " cells" << endl;
  str << "   x: " << fDimX << " [0," << fMaxX << "]" << endl;
  str << "   y: " << fDimY << " [0," << fMaxY << "]" << endl;
  str << "   z: " << fDimZ << " [0," << fMaxZ << "]" << endl;
  str << "   " << GetNumberOfSpacePoints(0, fCellDimension) << " point(s)" << endl;
  if (fCells) {
    for (int i=0; i<fCellDimension; i++) {
      if (!bPrintEmpty && fCells[i].fCount<=0) continue;
      str << "     " << setfill(' ') << setw(7) << setprecision(0) << i << " (" 
	  << " " << setw(3) << GetLowerBoundX(i)
	  << " " << setw(3) << GetLowerBoundY(i)
	  << " " << setw(4) << GetLowerBoundZ(i)
	  << "): ";
      str << setw(3) << fCells[i].fCount << " entries, " << setw(3) << fCells[i].fFilled << " filled";
      str << "  start index " << setw(5) << fCells[i].fStartIndex;
      str << endl;
      if (fCells[i].fCount>0) {
	str << "          ";
	for (iterator id=begin(GetLowerBoundX(i), GetLowerBoundY(i), GetLowerBoundZ(i));
	     id!=end(); id++) {
	  str << " 0x" << hex << setw(8) << setfill('0') << id.Data();
	}
	str  << dec << endl;
      }
    }
  }
  cout << str;
}


  class iterator {
  public:
  iterator()
    : fData(NULL) {}
  iterator(ValueType* pData)
    : fData(pData) {}
  iterator(const iterator& i)
    : fData(i.fData) {}
    iterator& operator=(const iterator& i)
      { fData=i.fData; return *this;}
    ~iterator() {fData=NULL;}

    bool operator==(const iterator& i) const  {return (fData!=NULL) && (fData==i.fData);}
    bool operator!=(const iterator& i) const  {return (fData!=NULL) && (fData!=i.fData);}
    // prefix operators
    iterator& operator++() {fData++; return *this;}
    iterator& operator--() {fData--; return *this;}
    // postfix operators
    iterator operator++(int) {iterator i(*this); fData++; return i;}
    iterator operator--(int) {iterator i(*this); fData--; return i;}

    iterator& operator+=(int step) {fData+=step; return *this;}

    const ValueType& Data() const {return *fData;}
    ValueType& Data() {return *fData;}

  protected:
  private:
    ValueType* fData; //! data
  };

  // prepare iterator and end marker
  iterator& begin(T x=(T)-1, T y=(T)-1, T z=(T)-1) {
    fIterator.~iterator();
    fIteratorEnd.~iterator();

    int startIndex=0;
    if (x<0) {
      // get all data
      if (fData) {
	new (&fIterator) iterator(fData);
	fIteratorEnd=fIterator;
	fIteratorEnd+=fCount;
      }
      return fIterator;
    }

    // only search for the start index if specific x selected
    int cell=GetCellIndex(x, y, z);
    if (cell<0 || !fCells || cell>=fCellDimension) return fIterator;
    // get the index of the cell
    startIndex=fCells[cell].fStartIndex;
    if (startIndex<0 || !fData || startIndex>=fDataDimension) return fIterator;

    // get the range end position
    int endCell=cell+1;
    if (x<0) endCell=fCellDimension;
    else if (y<0) endCell=GetCellIndex(x+fStepX, -1., -1.); // all entries for fixed x
    else if (z<0) endCell=GetCellIndex(x, y+fStepY, -1.); // all entries for fixed x and y
    if (endCell<=cell) {
      // cell index returned is never outside the array
      // so this is a special case where we get to the bounds of the array
      endCell=fCellDimension;
    }

    new (&fIterator) iterator(fData+startIndex);
    fIteratorEnd=fIterator;
    fIteratorEnd+=GetNumberOfSpacePoints(cell, endCell);
    return fIterator;
  }

  // get loop end marker
  iterator& end() {
    return fIteratorEnd;
  }

  struct AliHLTIndexGridCell {
    int fCount;
    int fFilled;
    int fStartIndex;
  };

 protected:
 private:
  // standard constructor prohibited
  AliHLTIndexGrid();
  // copy constructor prohibited
  AliHLTIndexGrid(const AliHLTIndexGrid&);
  // assignment operator prohibited
  AliHLTIndexGrid& operator=(const AliHLTIndexGrid&);

  int IndexCells() {
    // set the start index for data of every cell based on the counts
    if (!fCells || fCellDimension<=0) return -ENOBUFS;
    int offset=0;
    int cell=0;
    for (; cell<fCellDimension; cell++) {
      if (fCells[cell].fCount<0) continue;
      fCells[cell].fStartIndex=offset;
      offset+=fCells[cell].fCount;
      fCells[cell].fFilled=0;
    }

    if (offset>fDataDimension) {
      // grow the data array
      auto_ptr<V> newArray(new V[offset]);
      if (newArray.get()) {
	memcpy(newArray.get(), fData, fDataDimension);
	memset(newArray.get()+fDataDimension, 0, (offset-fDataDimension)*sizeof(V));
	delete fData;
	fData=newArray.release();
	fDataDimension=offset;
      } else {
	for (cell=0; cell<fCellDimension; cell++) {
	  fCells[cell].fStartIndex=-1;
	}
      }
    }
    return 0;
  }


  T fMaxX;
  T fStepX;
  T fMaxY;
  T fStepY;
  T fMaxZ;
  T fStepZ;

  int fDimX;
  int fDimY;
  int fDimZ;

  AliHLTIndexGridCell* fCells; //! cell array
  int fCellDimension; //! size of cell array
  ValueType* fData; //! spacepoint data
  int fDataDimension; //! size of spacepoint data
  int fCount;

  iterator fIterator; //! iterator
  iterator fIteratorEnd; //! end marker iterator

  static const int fgkDefaultDataSize=10000; //! the default data size

  ClassDef(AliHLTIndexGrid, 0)
};

#endif
