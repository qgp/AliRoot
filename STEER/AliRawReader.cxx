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

///////////////////////////////////////////////////////////////////////////////
//
// This is the base class for reading a raw data file and providing
// information about digits
//
///////////////////////////////////////////////////////////////////////////////

#include "AliRawReader.h"

ClassImp(AliRawReader)


AliRawReader::AliRawReader(const char* fileName, Bool_t addNumber)
{
// create an object to read digits from the given input file(s)
// if addNumber is true, a number starting at 1 is appended to the file name

  fFileName = fileName;
  if (!addNumber) {
    fFileNumber = -1;
#ifndef __DECCXX
    fStream = new fstream(fileName, ios::binary|ios::in);
#else
    fStream = new fstream(fileName, ios::in);
#endif
  } else {
    fFileNumber = 0;
    fStream = NULL;
    OpenNextFile();
  }
  fCount = 0;
}

AliRawReader::~AliRawReader()
{
// close the input file

  if (fStream) {
#if defined(__HP_aCC) || defined(__DECCXX)
    if (fStream->rdbuf()->is_open()) fStream->close();
#else
    if (fStream->is_open()) fStream->close();
#endif
    delete fStream;
  }
}


Bool_t AliRawReader::OpenNextFile()
{
  if (fStream) {
#if defined(__HP_aCC) || defined(__DECCXX)
    if (fStream->rdbuf()->is_open()) fStream->close();
#else
    if (fStream->is_open()) fStream->close();
#endif
    delete fStream;
    fStream = NULL;
  }
  if (fFileNumber < 0) return kFALSE;

  fFileNumber++;
  char fileName[256];
  sprintf(fileName, "%s%d", fFileName, fFileNumber);
#ifndef __DECCXX 
  fStream = new fstream(fileName, ios::binary|ios::in);
#else
  fStream = new fstream(fileName, ios::in);
#endif
#if defined(__HP_aCC) || defined(__DECCXX)
  return (fStream->rdbuf()->is_open());
#else
  return (fStream->is_open());
#endif
}


Bool_t AliRawReader::ReadMiniHeader()
{
// read a mini header at the current stream position
// returns kFALSE if the mini header could not be read

  if (!fStream) return kFALSE;
  while (!fStream->read((char*) &fMiniHeader, sizeof(fMiniHeader))) {
    if (!OpenNextFile()) return kFALSE;
  }
  if ((fMiniHeader.fMagicWord[2] != 0x12) ||
      (fMiniHeader.fMagicWord[1] != 0x34) ||
      (fMiniHeader.fMagicWord[0] != 0x56))
    Error("ReadMiniHeader", "wrong magic word!");
  fCount = fMiniHeader.fSize;
  return kTRUE;
}

Bool_t AliRawReader::ReadNextInt(UInt_t& data)
{
// reads the next 4 bytes at the current stream position
// returns kFALSE if the data not be read

  while (fCount == 0) {
    if (!ReadMiniHeader()) return kFALSE;
  }
  if (!fStream->read((char*) &data, sizeof(data))) {
    Error("ReadNextInt", "could not read data!");
    return kFALSE;
  }
  fCount -= sizeof(data);
  return kTRUE;
}

Bool_t AliRawReader::ReadNextShort(UShort_t& data)
{
// reads the next 2 bytes at the current stream position
// returns kFALSE if the data not be read

  while (fCount == 0) {
    if (!ReadMiniHeader()) return kFALSE;
  }
  if (!fStream->read((char*) &data, sizeof(data))) {
    Error("ReadNextShort", "could not read data!");
    return kFALSE;
  }
  fCount -= sizeof(data);
  return kTRUE;
}

Bool_t AliRawReader::ReadNextChar(UChar_t& data)
{
// reads the next 1 byte at the current stream position
// returns kFALSE if the data not be read

  while (fCount == 0) {
    if (!ReadMiniHeader()) return kFALSE;
  }
  if (!fStream->read((char*) &data, sizeof(data))) {
    Error("ReadNextChar", "could not read data!");
    return kFALSE;
  }
  fCount -= sizeof(data);
  return kTRUE;
}

