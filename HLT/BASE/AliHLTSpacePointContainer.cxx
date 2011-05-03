// $Id$

//**************************************************************************
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//*                                                                        *
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *
//*                  for The ALICE HLT Project.                            *
//*                                                                        *
//* Permission to use, copy, modify and distribute this software and its   *
//* documentation strictly for non-commercial purposes is hereby granted   *
//* without fee, provided that the above copyright notice appears in all   *
//* copies and that both the copyright notice and this permission notice   *
//* appear in the supporting documentation. The authors make no claims     *
//* about the suitability of this software for any purpose. It is          *
//* provided "as is" without express or implied warranty.                  *
//**************************************************************************

/// @file   AliHLTSpacePointContainer.cxx
/// @author Matthias Richter
/// @date   2011-04-29
/// @brief  Base helper class for handling of HLT space point data blocks
///

#include "AliHLTSpacePointContainer.h"
#include "AliHLTComponent.h"
#include "TFile.h"
#include "TString.h"
#include "TArrayC.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TMath.h"
#include <memory>
#include <iostream>

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTSpacePointContainer)

AliHLTSpacePointContainer::AliHLTSpacePointContainer()
  : TObject(), AliHLTLogging()
  , fBuffers()
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTSpacePointContainer::AliHLTSpacePointContainer(const AliHLTSpacePointContainer& c)
  : TObject(c), AliHLTLogging()
  , fBuffers()
{
  /// copy constructor

  // currently no need to copy or allocate, the fBuffers of the source object are expected
  // to be persistent throughout the lifetime of the new object. Only pointers are copied.
}

AliHLTSpacePointContainer& AliHLTSpacePointContainer::operator=(const AliHLTSpacePointContainer& c)
{
  /// assignment operator
  if (&c==this) return *this;

  // currently no need to copy or allocate
  return *this;
}

AliHLTSpacePointContainer::~AliHLTSpacePointContainer()
{
  // destructor
  Clear();
}

void AliHLTSpacePointContainer::Clear(Option_t * /*option*/)
{
  // internal cleanup
  vector<TArrayC*>::iterator element=fBuffers.begin();
  while (element!=fBuffers.end()) {
    TArrayC* pBuffer=*element;
    element=fBuffers.erase(element);
    delete pBuffer;
  }
}

void AliHLTSpacePointContainer::Print(Option_t *option) const
{
  // print info
  Print(cout, option);
}

void AliHLTSpacePointContainer::Print(ostream& out, Option_t */*option*/) const
{
  // print to stream
  out << "AliHLTSpacePointContainer::Print" << endl;
}

int AliHLTSpacePointContainer::AddInputBlock(const char* filename, AliHLTComponentDataType dt, unsigned specification)
{
  // open block from file and add to collection
  if (!filename) return -EINVAL;
  
  TString input=filename;
  input+="?filetype=raw";
  std::auto_ptr<TFile> pFile(new TFile(input));
  if (!pFile.get()) return -ENOMEM;
  if (pFile->IsZombie()) return -ENOENT;

  int iResult=0;
  pFile->Seek(0);
  std::auto_ptr<TArrayC> buffer(new TArrayC);
  if (!buffer.get()) return -ENOMEM;

  buffer->Set(pFile->GetSize());
  if (pFile->ReadBuffer(buffer->GetArray(), buffer->GetSize())==0) {
    AliHLTComponentBlockData bd;
    AliHLTComponent::FillBlockData(bd);
    bd.fPtr=buffer->GetArray();
    bd.fSize=buffer->GetSize();
    bd.fDataType=dt;
    bd.fSpecification=specification;
    HLTDebug("adding data block %d byte(s) from file %s", pFile->GetSize(), filename);
    iResult=AddInputBlock(&bd);
  } else {
    HLTError("failed reading %d byte(s) from file %s", pFile->GetSize(), filename);
    iResult=-ENODATA;
  }

  fBuffers.push_back(buffer.release());
  return iResult;
}

int AliHLTSpacePointContainer::AddInputBlocks(const char* listfile, AliHLTComponentDataType dt)
{
  // open blank separated list of files and add data
  ifstream list(listfile);
  if (!list.good()) return -ENOENT;

  int count=0;
  TString file;
  while (file.ReadLine(list)) {
    HLTInfo("adding data from file %s", file.Data());
    int iResult=AddInputBlock(file.Data(), dt, kAliHLTVoidDataSpec);
    if (iResult<0) {
      HLTInfo("failed to add data from file %s: error %d", file.Data(), iResult);
      return iResult;
    }
    count+=iResult;
  }

  // std::auto_ptr<TObjArray> tokens(files.Tokenize(" "));
  // if (!tokens.get()) return 0;
  // for (int i=0; i<tokens->GetEntriesFast(); i++) {
  //   if (!tokens->At(i)) continue;
  //   cout << "adding data from file " << tokens->At(i)->GetName() << endl;
  //   AddInputBlock(tokens->At(i)->GetName(), dt, kAliHLTVoidDataSpec);
  // }

  return count;
}

AliHLTSpacePointContainer* AliHLTSpacePointContainer::SelectByTrack(int /*trackId*/, bool /*bAlloc*/) const
{
  /// create a collection of clusters for a specific track
  /// default implementation, nothing to do
  return NULL;
}

AliHLTSpacePointContainer* AliHLTSpacePointContainer::SelectByMC(int /*mcId*/, bool /*bAlloc*/) const
{
  /// create a collection of clusters for a specific MC track
  /// default implementation, nothing to do
  return NULL;
}

AliHLTSpacePointContainer* AliHLTSpacePointContainer::UsedClusters(bool /*bAlloc*/) const
{
  /// create a collection of all used clusters
  /// default implementation, nothing to do
  return NULL;
}

AliHLTSpacePointContainer* AliHLTSpacePointContainer::UnusedClusters(bool /*bAlloc*/) const
{
  /// create a collection of all unused clusters
  /// default implementation, nothing to do
  return NULL;
}

int AliHLTSpacePointContainer::MarkUsed(const AliHLTUInt32_t* /*clusterIDs*/, int /*arraySize*/)
{
  /// default implementation, nothing to do
  return -ENOSYS;
}

int AliHLTSpacePointContainer::SetTrackID(int /*trackID*/, const AliHLTUInt32_t* /*clusterIDs*/, int /*arraySize*/)
{
  /// default implementation, nothing to do
  return -ENOSYS;
}

int AliHLTSpacePointContainer::SetMCID(int /*mcID*/, const AliHLTUInt32_t* /*clusterIDs*/, int /*arraySize*/)
{
  /// default implementation, nothing to do
  return -ENOSYS;
}

TH1* AliHLTSpacePointContainer::DrawProjection(const char* plane, const vector<AliHLTUInt32_t>& selection) const
{
  // draw the projection of space points in a specified plane
  // "xy", "xz", "yz"
  
  int mode=0;
  if (strcmp(plane, "xy")==0) mode=0;
  else if (strcmp(plane, "xz")==0) mode=1;
  else if (strcmp(plane, "yz")==0) mode=2;
  else {
    HLTError("invalid plane specification %s, allowed 'xy', 'xz', 'yz'", plane);
    return NULL;
  }
  
  float maxXAxis=100.0;
  float maxYAxis=100.0;
  vector<AliHLTUInt32_t> clusterIDs;
  GetClusterIDs(clusterIDs);
  vector<AliHLTUInt32_t>::iterator clusterID=clusterIDs.begin();
  while (clusterID!=clusterIDs.end()) {
    vector<AliHLTUInt32_t>::const_iterator element=selection.begin();
    for (; element!=selection.end(); element++) {
      if (((*element)&0xffff0000)==((*clusterID)&0xffff0000)) break;
    }
    if (element==selection.end() && selection.size()>0) {
      clusterID=clusterIDs.erase(clusterID);
      continue;
    }
    float XValue=0.0;
    float YValue=0.0;
    if (mode==0) {
      XValue=GetX(*clusterID);
      YValue=GetY(*clusterID);
    } else if (mode==1) {
      XValue=GetX(*clusterID);
      YValue=GetZ(*clusterID);
    } else if (mode==2) {
      XValue=GetY(*clusterID);
      YValue=GetZ(*clusterID);
    }
    if (maxXAxis<XValue) maxXAxis=XValue;
    if (maxYAxis<YValue) maxYAxis=YValue;
    clusterID++;
  }
  if (maxXAxis<maxYAxis) maxXAxis=maxYAxis;
  else maxYAxis=maxXAxis;

  TH2F* histogram=new TH2F(plane, plane, 1000, -maxXAxis, maxXAxis, 1000, -maxYAxis, maxYAxis);
  if (!histogram) return NULL;
    if (mode==0) {
      histogram->GetXaxis()->SetTitle("X [cm]");
      histogram->GetYaxis()->SetTitle("Y [cm]");
    } else if (mode==1) {
      histogram->GetXaxis()->SetTitle("Z [cm]");
      histogram->GetYaxis()->SetTitle("X [cm]");
    } else if (mode==2) {
      histogram->GetXaxis()->SetTitle("Z [cm]");
      histogram->GetYaxis()->SetTitle("Y [cm]");
    }

  for (clusterID=clusterIDs.begin(); clusterID!=clusterIDs.end(); clusterID++) {
    float XValue=GetX(*clusterID);
    float YValue=GetY(*clusterID);
    float ZValue=GetZ(*clusterID);
    Float_t phi     = GetPhi(*clusterID);
    Float_t cos     = TMath::Cos( phi );
    Float_t sin     = TMath::Sin( phi );
    if (mode==0) {
      histogram->SetTitle("XY projection");
      histogram->Fill(cos*XValue - sin*YValue, sin*XValue + cos*YValue);
    } else if (mode==1) {
      // should be maybe 'ZX' but somehow 'XZ' is more 'lingual convenient'
      histogram->SetTitle("XZ projection");
      histogram->Fill(ZValue, cos*XValue - sin*YValue);
    } else if (mode==2) {
      // same comment
      histogram->SetTitle("YZ projection");
      histogram->Fill(ZValue, sin*XValue + cos*YValue);
    }
  }

  return histogram;
}

ostream& operator<<(ostream &out, const AliHLTSpacePointContainer& c)
{
  c.Print(out);
  return out;
}
