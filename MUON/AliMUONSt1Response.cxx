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

// Authors: David Guez, Ivana Hrivnacova, Marion MacCormick; IPN Orsay
//
// Class AliMUONSt1Response
// ----------------------------
// Response class for station 1 including electronics and detector response. 
// Individual pedestals or noise levels can be controlled separately. 
// The current pulse height responses do not contain any physics

#include <vector>
#include <TMath.h>
#include <TRandom.h>
#include <TSystem.h>
#include <Riostream.h>
#include "AliMpIntPair.h"
#include "AliMpPlaneSegmentation.h"
#include "AliMpPad.h"
#include "AliMpMotifMap.h"
#include "AliMpSector.h"
#include "AliMpPlane.h"
#include "AliMpZone.h"
#include "AliMpSubZone.h"
#include "AliMpVRowSegment.h"
#include "AliMUONSt1Response.h"
#include "AliMUONSt1ResponseParameter.h"
#include "AliMUONSt1ResponseRule.h"
#include "AliMUONSt1IniReader.h"
#include "AliMUONSt1Decoder.h"
#include "AliMUONTransientDigit.h"

ClassImp(AliMUONSt1Response);

const TString AliMUONSt1Response::fgkTopDir = getenv("ALICE_ROOT");
const TString AliMUONSt1Response::fgkDataDir = "/MUON/data/";
const TString AliMUONSt1Response::fgkConfigBaseName = "configChamber";
const TString AliMUONSt1Response::fgkStandardIniFileName = "st1StdParameter.ini";

const TString AliMUONSt1Response::fgkBaseName ="base";
const TString AliMUONSt1Response::fgkIncludeName ="include";
const TString AliMUONSt1Response::fgkParameterName ="parameter";
const TString AliMUONSt1Response::fgkRegionName ="region";
const TString AliMUONSt1Response::fgkRuleName ="rule";
const TString AliMUONSt1Response::fgkNameName ="name";
const TString AliMUONSt1Response::fgkPedestalName ="pedestal";
const TString AliMUONSt1Response::fgkNoiseName ="noise";
const TString AliMUONSt1Response::fgkStateName ="state";
const TString AliMUONSt1Response::fgkMName ="padM";
const TString AliMUONSt1Response::fgkMGName ="padMG";
const TString AliMUONSt1Response::fgkMGCName ="padMGC";
const TString AliMUONSt1Response::fgkIJName ="padIJ";
const TString AliMUONSt1Response::fgkXYName ="padXY";
const TString AliMUONSt1Response::fgkZoneName ="zone";
const TString AliMUONSt1Response::fgkStickyOnName ="stickyOn";
const TString AliMUONSt1Response::fgkStickyOffName ="stickyOff";
const TString AliMUONSt1Response::fgkFileName ="file";
const TString AliMUONSt1Response::fgkValueName ="value";
const TString AliMUONSt1Response::fgkGausName ="gaus";
const TString AliMUONSt1Response::fgkNotName ="no";
const TString AliMUONSt1Response::fgkNofSigmaName ="nofSigma";



//__________________________________________________________________________
AliMUONSt1Response::AliMUONSt1Response(Int_t chamber)
  :AliMUONResponseV0()
  ,fChamber(chamber),fParams(),fRegions(),fTrashList()
  
{
   // default pedestal value
   fCountNofCalls=0;
   fCountUnknownZone=0;
   fCountUnknownIndices=0;

   Int_t i;
   for (i=0;i<2;i++){
     fIniFileName[i]="";
     fPlane[0]=0;
     fPlaneSegmentation[i]=0;
     for (Int_t j=0;j<fgkNofZones;j++)
     {
       fDefaultParameters[i][j]=0;
     }
   }
   fTrashList.SetOwner(kTRUE);
}


//__________________________________________________________________________
AliMUONSt1Response::~AliMUONSt1Response()
{
//destructor
  Int_t i;
  for (i=0;i<2;i++){
    if (fPlaneSegmentation[i]) delete fPlaneSegmentation[i];
    if (fPlane[i]) delete fPlane[i];
    fTrashList.Delete();
  }
}

//__________________________________________________________________________
void AliMUONSt1Response::SetIniFileName(Int_t plane,const TString& fileName)
{
// Set the file to be read for the response parameters
  if ((plane>=0) && (plane<=1)) fIniFileName[plane] = fileName;
}


//__________________________________________________________________________
void AliMUONSt1Response::ReadCouplesOfIntRanges(const string& value,TList* list,AliMUONSt1ElectronicElement::TDescription descr) 
{
// Decode couplets of integer ranges (enclosed within parenthesis and 
// separated by a comma, eg. (12/20,33/60) for ranges 12 to 20 and 33 to 60) 
// and save these ranges in <list> 
  vector<string> lstCpl = decoder::SplitNtuples(value);
  for (unsigned int n=0;n<lstCpl.size();n++){ // for each (..,..) couplet
    vector<string> lst = decoder::SplitList(lstCpl[n],","); 
                                              // should have 2 elements
    if (lst.size() != 2) {
      Warning("ReadIniFile","Bad pad definition");
      continue;
    }
    vector<pair <int,int> > lst1 = decoder::DecodeListOfIntRanges(lst[0],";");
    vector<pair <int,int> > lst2 = decoder::DecodeListOfIntRanges(lst[1],";");
    for (unsigned int u1=0;u1<lst1.size();u1++){
      for (unsigned int u2=0;u2<lst2.size();u2++){
	AliMUONSt1ElectronicElement* elem 
      	  = new AliMUONSt1ElectronicElement(descr);
	fTrashList.Add(elem);
	elem->SetRange(0,lst1[u1].first,lst1[u1].second);
	elem->SetRange(1,lst2[u2].first,lst2[u2].second);
	list->Add(elem);
      }
    }
  }
}


//__________________________________________________________________________
void AliMUONSt1Response::ReadCouplesOfFloatRanges(const string& value,TList* list)
{
// Decode couplets of floating point ranges (enclosed within parenthesis and 
// separated by a comma, eg. (12./20.,33./60.) for ranges 12. to 20. and 33. to 60.) 
// and save these ranges in <list> 
  vector<string> lstCpl = decoder::SplitNtuples(value);
  for (unsigned int n=0;n<lstCpl.size();n++){ // for each (..,..) couplets
    vector<string> lst = decoder::SplitList(lstCpl[n],","); 
                                              // should have 2 elements
    if (lst.size() != 2) {
      Warning("ReadIniFile","Bad pad definition");
      continue;
    }
    vector<pair <double,double> > lst1 = decoder::DecodeListOfFloatRanges(lst[0],";");
    vector<pair <double,double> > lst2 = decoder::DecodeListOfFloatRanges(lst[1],";");
    for (unsigned int u1=0;u1<lst1.size();u1++){
      for (unsigned int u2=0;u2<lst2.size();u2++){
	AliMUONSt1ElectronicElement* elem 
      	  = new AliMUONSt1ElectronicElement(AliMUONSt1ElectronicElement::kXY);
	fTrashList.Add(elem);
	elem->SetRange(0,lst1[u1].first,lst1[u1].second);
	elem->SetRange(1,lst2[u2].first,lst2[u2].second);
	list->Add(elem);
      }
    }
  }
}


//__________________________________________________________________________
void AliMUONSt1Response::SetPairToParam(const string& name,const string& value,AliMUONSt1ResponseParameter* param) const
{
// set a (name,value) pair to <param>
  TString path = fgkTopDir + fgkDataDir ;
  const char* nm = name.c_str();
  if (fgkStateName.CompareTo(nm,TString::kIgnoreCase)==0){
    param->SetState(atoi(value.c_str()));
  } else if (fgkPedestalName.CompareTo(nm,TString::kIgnoreCase)==0){
    vector<string> lst = decoder::SplitList(value," ");
    if ((lst.size()>0) && (fgkNotName.CompareTo(lst[0].c_str(),TString::kIgnoreCase)==0)){
      param->UnSetPedestal();
    } else if ((lst.size()>1) && (fgkValueName.CompareTo(lst[0].c_str(),TString::kIgnoreCase)==0)){
      param->SetPedestal(atof(lst[1].c_str()));
    } else if ((lst.size()>1) && (fgkFileName.CompareTo(lst[0].c_str(),TString::kIgnoreCase)==0)){
      param->SetPedestal(path+lst[1].c_str());
    } else if ((lst.size()>2) && (fgkGausName.CompareTo(lst[0].c_str(),TString::kIgnoreCase)==0)){
      param->SetPedestal(atof(lst[1].c_str()),atof(lst[2].c_str()));
    }
  } else if (fgkNoiseName.CompareTo(nm,TString::kIgnoreCase)==0){
    vector<string> lst = decoder::SplitList(value," ");
    if ((lst.size()>1) && (fgkValueName.CompareTo(lst[0].c_str(),TString::kIgnoreCase)==0)){
      param->SetNoise(atof(lst[1].c_str()));
    } else if ((lst.size()>1) && (fgkFileName.CompareTo(lst[0].c_str(),TString::kIgnoreCase)==0)){
      param->SetNoise(path+lst[1].c_str());
    } else if ((lst.size()>2) && (fgkGausName.CompareTo(lst[0].c_str(),TString::kIgnoreCase)==0)){
      param->SetNoise(atof(lst[1].c_str()),atof(lst[2].c_str()));
    }
  } else if (fgkNofSigmaName.CompareTo(nm,TString::kIgnoreCase)==0){
    param->SetNofSigma(atoi(value.c_str()));
  } else if (fgkStickyOnName.CompareTo(nm,TString::kIgnoreCase)==0){
    vector< pair<int,int> > lst = decoder::DecodeListOfIntRanges(value);
    for (unsigned int i=0;i<lst.size();i++){
      for (int j=lst[i].first;(j<12) && (j<=lst[i].second);j++){
	param->SetStickyBitOn(j);
      }
    }
  } else if (fgkStickyOffName.CompareTo(nm,TString::kIgnoreCase)==0){
    vector< pair<int,int> > lst = decoder::DecodeListOfIntRanges(value);
    for (unsigned int i=0;i<lst.size();i++){
      for (int j=lst[i].first;(j<12) && (j<=lst[i].second);j++){
	param->SetStickyBitOff(j);
      }
    }
  }
}


//__________________________________________________________________________
void AliMUONSt1Response::SetPairToListElem(const string& name,const string& value,TList* list)
{
// set a (name,value) pair to <list>
  const char* nm = name.c_str();
  if (fgkIJName.CompareTo(nm,TString::kIgnoreCase)==0){
    ReadCouplesOfIntRanges(value,list,AliMUONSt1ElectronicElement::kIJ);
  } else if (fgkMGCName.CompareTo(nm,TString::kIgnoreCase)==0){
    ReadCouplesOfIntRanges(value,list,AliMUONSt1ElectronicElement::kMGC);
  } else if (fgkMGName.CompareTo(nm,TString::kIgnoreCase)==0){
    ReadCouplesOfIntRanges(value,list,AliMUONSt1ElectronicElement::kMG);
  } else if (fgkMName.CompareTo(nm,TString::kIgnoreCase)==0){
    vector< pair<int,int> > lst = decoder::DecodeListOfIntRanges(value);
    for (unsigned int i=0;i<lst.size();i++){
      AliMUONSt1ElectronicElement* elem 
        = new AliMUONSt1ElectronicElement(AliMUONSt1ElectronicElement::kM);
      fTrashList.Add(elem);
      elem->SetRange(0,lst[i].first,lst[i].second);
      list->Add(elem);
    }
  } else if (fgkXYName.CompareTo(nm,TString::kIgnoreCase)==0){
    ReadCouplesOfFloatRanges(value,list);
  }
}


//__________________________________________________________________________
void AliMUONSt1Response::ReadIniFile(Int_t plane)
{
  //Read the ini file and fill the <plane>th structures 
  TString path = fgkTopDir + fgkDataDir ;
  //read .ini file
  if (gSystem->AccessPathName(path+fIniFileName[plane],kReadPermission)){
    Fatal("ReadIniFile",
          Form("Unable to Read the file %s",fIniFileName[plane].Data()));
    return;
  }
  fRegions.clear();
  fParams.clear();
  ReadIniFile(plane,path+fIniFileName[plane],kTRUE,kTRUE,kTRUE);
}


//__________________________________________________________________________
void AliMUONSt1Response::ReadIniFile(Int_t plane,const TString& fileName,
                                    Bool_t rdParam,Bool_t rdRegion,Bool_t rdRule)
{
  //Read the given ini file and fill the <plane>th structures 
  cout<<"Reading parameter file "<<fileName<<endl;
  AliMUONSt1IniReader iniFile(fileName.Data());
  AliMUONSt1IniReader::TChapter chap;
  AliMUONSt1IniReader::TValueList vals;
  AliMUONSt1IniReader::TValueList::iterator itValue;
  while (!iniFile.Eof()){
    chap = iniFile.MakeCurrentChapter();
    TString chapName = chap.first.c_str();
    vals = chap.second;
    if (fgkBaseName.CompareTo(chapName,TString::kIgnoreCase)==0){
      for (itValue = vals.begin() ; itValue != vals.end(); ++itValue){
        string name =  (*itValue).first;
        string value = (*itValue).second;
        if (fgkIncludeName.CompareTo(name.c_str(),TString::kIgnoreCase)==0){
          vector<string> lst = decoder::SplitList(value,":");
          if (lst.size()>0){
            TString inFileName = TString(gSystem->DirName(fileName))+"/" + lst[0].c_str();
            Bool_t inParam=kFALSE,inRegion=kFALSE,inRule=kFALSE;
            if (lst.size()>1) {
              vector<string> lst2 = decoder::SplitList(lst[1],",");
              for (unsigned int k=0;k<lst2.size();k++){
                if (fgkParameterName.CompareTo(lst2[k].c_str(),TString::kIgnoreCase)==0){
                  inParam=kTRUE;
                } else if (fgkRegionName.CompareTo(lst2[k].c_str(),TString::kIgnoreCase)==0){
                  inRegion=kTRUE;
                } else if (fgkRuleName.CompareTo(lst2[k].c_str(),TString::kIgnoreCase)==0){
                  inRule=kTRUE;
                }
              }
            } else {
              inParam=inRegion=inRule=kTRUE;
            }
            ReadIniFile(plane,inFileName,inParam,inRegion,inRule);
          }
        }
      }
    } else if (rdParam && fgkParameterName.CompareTo(chapName,TString::kIgnoreCase)==0){
      AliMUONSt1ResponseParameter* param = new AliMUONSt1ResponseParameter();
      fTrashList.Add(param);
      string paramName=Form("Parameter %d",fParams.size()+1);
      for (itValue = vals.begin() ; itValue != vals.end(); ++itValue){
        string name =  (*itValue).first;
        string value = (*itValue).second;
        if (fgkNameName.CompareTo(name.c_str(),TString::kIgnoreCase)==0){
          paramName=value;
        } else SetPairToParam(name,value,param);
      }
      fParams[paramName]=param;
    } else if (rdRegion && fgkRegionName.CompareTo(chapName,TString::kIgnoreCase)==0){
      TList* lstElem = new TList;
      string listName=Form("Region %d",fRegions.size()+1);
      for (itValue = vals.begin() ; itValue != vals.end(); ++itValue){
        string name =  (*itValue).first;
        string value = (*itValue).second;
        if (fgkNameName.CompareTo(name.c_str(),TString::kIgnoreCase)==0){
          listName=value;
        } else SetPairToListElem(name,value,lstElem);
      }
      fRegions[listName]=lstElem;
    }
  }
  iniFile.Reset();
  while (!iniFile.Eof()){
    chap = iniFile.MakeCurrentChapter();
    TString chapName = chap.first.c_str();
    vals = chap.second;
    if (rdRule && fgkRuleName.CompareTo(chapName,TString::kIgnoreCase)==0){
      Int_t i;
      Bool_t zones[fgkNofZones];
      for (i=0;i<fgkNofZones;i++) zones[i]=kFALSE;
      AliMUONSt1ResponseRule* rule=0;
      for (itValue = vals.begin() ; itValue != vals.end(); ++itValue){
        string name =  (*itValue).first;
        string value = (*itValue).second;
        if (fgkZoneName.CompareTo(name.c_str(),TString::kIgnoreCase)==0){
          vector< pair<int,int> > lst = decoder::DecodeListOfIntRanges(value);
          for (unsigned int i=0;i<lst.size();i++){
            for (int j=lst[i].first;(j<=fgkNofZones) && (j<=lst[i].second);j++) {
              if (j>0) zones[j-1] = kTRUE;
            }
          }
        } else if (fgkRegionName.CompareTo(name.c_str(),TString::kIgnoreCase)==0){
          TListMap::iterator it = fRegions.find(value);
          if (it != fRegions.end()){
            if (!rule) {
	      rule = new AliMUONSt1ResponseRule();
	      fTrashList.Add(rule);
	    }
            TIter next((*it).second);
            AliMUONSt1ElectronicElement* el;
            while ((el = static_cast<AliMUONSt1ElectronicElement*>(next()))){
              rule->AddElement(el);
            }
          } else Warning("ReadIniFile",Form("Can't find region named %s",value.c_str()));
        }
      }
      for (itValue = vals.begin() ; itValue != vals.end(); ++itValue){
        string name =  (*itValue).first;
        string value = (*itValue).second;
        if (fgkParameterName.CompareTo(name.c_str(),TString::kIgnoreCase)==0){
          TParamsMap::iterator it = fParams.find(value);
          if (it != fParams.end()){
            AliMUONSt1ResponseParameter* param = (*it).second;
            for (i=0;i<fgkNofZones;i++) if (zones[i]) {
              fDefaultParameters[plane][i]=param;
            }
            if (rule) rule->AddParameter(param);
          } else Warning("ReadIniFile",Form("Can't find parameter named %s",value.c_str()));
        }
      }
      if (rule) fRulesList[plane].AddFirst(rule);
    }
  }
  for (TListMap::iterator it = fRegions.begin() ; it != fRegions.end(); ++it) delete (*it).second;
}


//__________________________________________________________________________
void AliMUONSt1Response::ReadFiles()
{
// Define the current response rules with respect to the description
// given in the "configChamber1.ini" and "configChamber2.ini" files.
  Int_t i;
  TString path = fgkTopDir + fgkDataDir ;

  TString configFileName = path + fgkConfigBaseName + Form("%d.ini",fChamber);
  if (gSystem->AccessPathName(configFileName,kReadPermission)){
    // no configChamberI.ini file exists
    SetIniFileName(0,fgkStandardIniFileName);
    SetIniFileName(1,fgkStandardIniFileName);
  } else {
    cout<<"Reading configuration file "<<configFileName<<endl;
    AliMUONSt1IniReader iniFile(configFileName.Data());
    while (!iniFile.Eof()) {
      iniFile.ReadNextLine();
      if (iniFile.GetCurrentType() != AliMUONSt1IniReader::kValue) continue;
      Int_t plane;
      if ((sscanf(iniFile.GetCurrentName().c_str()
                 ,"file%d",&plane)==1) && (plane>=0) && (plane<=1)){
      	SetIniFileName(plane,iniFile.GetCurrentValue().c_str());
      }
    }
  }
  //book memory and fill them with .ini files
  fPlane[0]=AliMpPlane::Create(kBendingPlane);
  fPlane[1]=AliMpPlane::Create(kNonBendingPlane);
  for (i=0;i<2;i++){
    fPlaneSegmentation[i]= new AliMpPlaneSegmentation(fPlane[i]);
    ReadIniFile(i);
  }
}

//__________________________________________________________________________
Float_t AliMUONSt1Response::IntPH(Float_t eloss)
{
  // Calculate charge from given ionization energy lost.
  Int_t nel;
  nel= Int_t(eloss*1.e9/20); 
  Float_t charge=0;
  if (nel == 0) nel=1;
  for (Int_t i=1;i<=nel;i++) {
      Float_t arg=0.;
      while(!arg) arg = gRandom->Rndm();
      charge -= fChargeSlope*TMath::Log(arg);    
  }
  return charge;
}


//__________________________________________________________________________
AliMpZone* AliMUONSt1Response::FindZone(AliMpSector* sector,Int_t posId)
{
// to be moved to AliMpSector::
  for (Int_t izone=1;izone<=sector->GetNofZones();izone++){
    AliMpZone* zone = sector->GetZone(izone);
    for (Int_t isub=0;isub<zone->GetNofSubZones();isub++){
      AliMpSubZone* sub=zone->GetSubZone(isub);
      for (Int_t iseg=0;iseg<sub->GetNofRowSegments();iseg++){
	if (sub->GetRowSegment(iseg)->HasMotifPosition(posId)) return zone;
      }
    }
  }
  return 0;
}


//__________________________________________________________________________

Int_t  AliMUONSt1Response::DigitResponse(Int_t digit,AliMUONTransientDigit* where)
{
  // returns the electronic response of pad located at <where>, when
  // a charge <digit> is present
  
    //cout<<"electronic of pad "<<where->PadX()<<' '<<where->PadY()
    //                          <<" on plane "<<where->Cathode()<<endl;
    
    //read the files the first time this function is called
    if (!fPlane[0]) ReadFiles();

    fCountNofCalls++;
    
    AliMpIntPair indices(where->PadX(),where->PadY());
    AliMpPad pad = fPlaneSegmentation[where->Cathode()]->PadByIndices(indices,kFALSE);
    Int_t GC=0;
    Int_t numZone=0;
    AliMpZone* zone=0;

    if (pad.IsValid()) {
      AliMpIntPair location = pad.GetLocation();
      //cout<<location.GetFirst()<<endl;
      Int_t posId=abs(location.GetFirst());
      AliMpSector* sector=0;
      if (fPlane[0]->GetFrontSector()->GetMotifMap()->FindMotifPosition(posId))
        	sector=(AliMpSector*)fPlane[0]->GetFrontSector();
      else if (fPlane[0]->GetBackSector()->GetMotifMap()->FindMotifPosition(posId))
         	sector=(AliMpSector*)fPlane[0]->GetBackSector();

      if (sector) zone=FindZone(sector,posId);
      if (zone){
      	numZone=zone->GetID()-1;
      	GC=location.GetSecond();
      } else {
	fCountUnknownZone++;
      }
    } else {
      fCountUnknownIndices++;
    }

    if (!zone) {
      cout<<"Probleme electronic of pad "<<where->PadX()<<' '<<where->PadY()
      	  <<" on plane "<<where->Cathode()<<endl;
      return 6666;
    }
    TList listParams;
    TIter next(&fRulesList[where->Cathode()]);
    AliMUONSt1ResponseRule* rule;
    while ( (rule = static_cast<AliMUONSt1ResponseRule*>(next())))
      if (rule->Contains(pad)) listParams.AddAll(rule->GetParameters());
    if (fDefaultParameters[where->Cathode()][numZone])
      listParams.Add(fDefaultParameters[where->Cathode()][numZone]);

    AliMUONSt1ResponseParameter* param;
    TIter nextParam(&listParams);
    while ( (param = static_cast<AliMUONSt1ResponseParameter*>(nextParam()))){
      if (param->GetState()==kFALSE) {
        return 0;
      }
    }
    nextParam.Reset();
    while ( (param = static_cast<AliMUONSt1ResponseParameter*>(nextParam()))){
      if (param->HasPedestal()) {
        digit  = param->ApplyPedestal(digit,GC);
        break; // Apply pedestals just once -->  break the loop once a pedestal 
//                                               rule is applied
      }
    }
    if ( digit < 0) digit=0;
    if (digit >  MaxAdc()) digit=MaxAdc();
    nextParam.Reset();
    while ( (param = static_cast<AliMUONSt1ResponseParameter*>(nextParam()))){
      digit  = param->ApplyStickyBits(digit);
    }
   
    //cout<<digit<<endl;
    return digit;
}


//__________________________________________________________________________
void AliMUONSt1Response::PrintStatistics() const
{
// Show the results of the statistics
  cout<<"The DigitResponse() method was called "<<fCountNofCalls<<" times"<<endl;
  cout<<" it was unable to find the pad corresponding to the given indices "
      <<fCountUnknownIndices<<" times ("
      <<(Double_t)100.*fCountUnknownIndices/fCountNofCalls
      <<"%)"<<endl;
  cout<<" it was unable to find the zone corresponding to the found pad "
      <<fCountUnknownZone<<" times ("
      <<(Double_t)100.*fCountUnknownZone/fCountNofCalls
      <<"%)"<<endl;
}







