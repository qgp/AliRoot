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
* about the suitability of this software for any purpeateose. It is      *
* provided "as is" without express or implied warranty.                  *
**************************************************************************/

// $Id$
// $MpId: AliMpTriggerReader.cxx,v 1.4 2006/05/24 13:58:52 ivana Exp $

#include "AliMpTriggerReader.h"

#include "AliLog.h"
#include "AliMpConstants.h"
#include "AliMpDataStreams.h"
#include "AliMpFiles.h"
#include "AliMpHelper.h"
#include "AliMpMotif.h"
#include "AliMpMotifPosition.h"
#include "AliMpMotifReader.h"
#include "AliMpMotifSpecial.h"
#include "AliMpMotifType.h"
#include "AliMpPCB.h"
#include "AliMpSlat.h"
#include "AliMpSlatMotifMap.h"
#include "AliMpSlatMotifMap.h"
#include "AliMpSt345Reader.h"
#include "AliMpTrigger.h"
#include "Riostream.h"
#include "TClass.h"
#include "TList.h"
#include "TObjString.h"
#include "TString.h"
#include <TArrayI.h>
#include <cstdlib>
#include <sstream>

//-----------------------------------------------------------------------------
/// \class AliMpTriggerReader
/// Read trigger slat ASCII files
/// Basically provides two methods:
/// - AliMpTrigger* ReadSlat()
/// - AliMpPCB* ReadPCB()
///
/// \author Laurent Aphecetche
//-----------------------------------------------------------------------------

/// \cond CLASSIMP
ClassImp(AliMpTriggerReader)
/// \endcond

//
// static private methods
//

//_____________________________________________________________________________
const TString& AliMpTriggerReader::GetKeywordLayer()
{
  /// Keyword: LAYER
  static const TString kKeywordLayer("LAYER");
  return kKeywordLayer;
}  

//_____________________________________________________________________________
const TString& AliMpTriggerReader::GetKeywordScale()
{
  /// Keyword: SCALE
  static const TString kKeywordScale("SCALE");
  return kKeywordScale;
}

//_____________________________________________________________________________
const TString& AliMpTriggerReader::GetKeywordPcb()
{
  /// Keyword : PCB
  static const TString kKeywordPcb("PCB");  
  return kKeywordPcb;
}    
  
//_____________________________________________________________________________
const TString& AliMpTriggerReader::GetKeywordFlipX()
{
  /// Keyword : FLIPX
  static const TString kKeywordFlipX("FLIP_X");
  return kKeywordFlipX;
}  
  
//_____________________________________________________________________________
const TString& AliMpTriggerReader::GetKeywordFlipY()
{
  /// Keyword : FLIPY
  static const TString kKeywordFlipY("FLIP_Y");
  return kKeywordFlipY;
}  

//
// ctors, dtor
//

//_____________________________________________________________________________
AliMpTriggerReader::AliMpTriggerReader(const AliMpDataStreams& dataStreams, AliMpSlatMotifMap* motifMap) 
: TObject(),
  fkDataStreams(dataStreams),
  fMotifMap(motifMap),
  fLocalBoardMap()
{
  ///
  /// Default ctor.
  ///
    fLocalBoardMap.SetOwner(kTRUE);
} 

//_____________________________________________________________________________
AliMpTriggerReader::~AliMpTriggerReader()
{
  ///
  /// Dtor.
  ///
  fLocalBoardMap.DeleteAll();
}

//_____________________________________________________________________________
AliMpSlat*
AliMpTriggerReader::BuildSlat(const char* slatName,
                              AliMp::PlaneType planeType,
                              const TList& lines,
                              Double_t scale)
{
  /// Construct a slat from the list of lines, taking into account
  /// the scale factor. The returned pointer must be deleted by the client

  AliDebug(1,Form("slat %s %s scale %e",
                  slatName,PlaneTypeName(planeType).Data(),scale))
  ;
  
  AliMpSlat* slat = new AliMpSlat(slatName, planeType);
    
  TIter it(&lines);
  
//  StdoutToAliDebug(3,lines.Print(););
  
  TObjString* osline;
  while ( ( osline = (TObjString*)it.Next() ) )
  {
    // note that at this stage lines should not be empty.
    TString sline(osline->String());
    
    TObjArray* tokens = sline.Tokenize(' ');
    
    TString& keyword = ((TObjString*)tokens->At(0))->String();
    
    if ( keyword == GetKeywordPcb() )
    {
      if ( tokens->GetEntriesFast() != 3 )
      {
        AliErrorClass(Form("Syntax error : expecting PCB type localboard-list"
                           " in following line:\n%s",sline.Data()));
        delete slat;
        delete tokens;
        return 0;
      }
      TString pcbName = ((TObjString*)tokens->At(1))->String();
      
      TObjArray* localBoardList = ((TObjString*)tokens->At(2))->String().Tokenize(',');
      
      if ( scale != 1.0 )
      {
        std::ostringstream s;
        s << pcbName.Data() << "x" << scale;
        pcbName = s.str().c_str();
      }
      
      AliMpPCB* pcbType = ReadPCB(pcbName.Data());	  
      if (!pcbType)
      {
        AliErrorClass(Form("Cannot read pcbType=%s",pcbName.Data()));
        delete slat;
	delete tokens;
        return 0;
      }      

      TArrayI allLocalBoards;
      
      for ( Int_t ilb = 0; ilb < localBoardList->GetEntriesFast(); ++ilb)
      {
        TArrayI localBoardNumbers;
        TString& localBoards = ((TObjString*)localBoardList->At(ilb))->String();
        Ssiz_t pos = localBoards.First('-');
        if ( pos < 0 ) 
        {
          pos = localBoards.Length();
        }
        AliMpHelper::DecodeName(localBoards(pos-1,localBoards.Length()-pos+1).Data(),
                                ';',localBoardNumbers);      
        for ( int i = 0; i < localBoardNumbers.GetSize(); ++i )
        {
          std::ostringstream name;
          name << localBoards(0,pos-1) << localBoardNumbers[i];
          AliDebugClass(3,name.str().c_str());
          localBoardNumbers[i] = LocalBoardNumber(name.str().c_str());
          AliDebugClass(3,Form("LOCALBOARDNUMBER %d\n",localBoardNumbers[i]));
          allLocalBoards.Set(allLocalBoards.GetSize()+1);
          allLocalBoards[allLocalBoards.GetSize()-1] = localBoardNumbers[i];
          if (localBoardNumbers[i] < 0 )
          {
            AliErrorClass(Form("Got a negative local board number in %s ? Unlikely"
                               " to be correct... : %s\n",slatName,name.str().c_str()));
          }
        }
      }
      AliDebug(3,"Deleting tokens");
      delete tokens;
      AliDebug(3,"Deleting localBoardList");
      delete localBoardList;
      AliDebug(3,"Adding pcb to slat");
      slat->Add(*pcbType,allLocalBoards);
      AliDebug(3,Form("Deleting pcbType=%p %s",pcbType,pcbName.Data()));
      delete pcbType;
    }
  }
  
  if ( slat->DX()== 0 || slat->DY() == 0 )
  {
    AliFatalClass(Form("Slat %s has invalid null size\n",slat->GetID()));
  }
  return slat;
}

//_____________________________________________________________________________
TString
AliMpTriggerReader::GetBoardNameFromPCBLine(const TString& s)
{
  /// Decode the string to get the board name
  TString boardName;
  
  TObjArray* tokens = s.Tokenize(' ');
  
  TString& keyword = ((TObjString*)tokens->At(0))->String();

  if ( keyword == GetKeywordPcb() &&
       tokens->GetEntriesFast() == 3 )
  {
    boardName = ((TObjString*)tokens->At(2))->String();
  }
  
  delete tokens;
  
  return boardName;
}
  
//_____________________________________________________________________________
void
AliMpTriggerReader::FlipLines(TList& lines, Bool_t flipX, Bool_t flipY,
                              Int_t srcLine, Int_t destLine)
{
  ///
  /// Change the local board names contained in lines, 
  /// to go from right to left, and/or
  /// from top to bottom
  ///
 

  if ( flipX )
  {
    // Simply swaps R(ight) and L(eft) in the first character of 
    // local board names

    TObjString* oline;
    TIter it(&lines);
    while ( ( oline = (TObjString*)it.Next() ) )
    {
      TString& s = oline->String();
      if ( s.Contains("RC") ) 
      {
        // Change right to left
        s.ReplaceAll("RC","LC");
      }
      else if ( s.Contains("LC") )
      {
        // Change left to right
        s.ReplaceAll("LC","RC");
      }
    }
  }
  
  if ( flipY )
  {
    // Change line number, according to parameters srcLine and destLine
    // Note that because of road opening (for planes 3 and 4 at least),
    // we loop for srcLine +-1
    //
    for ( Int_t line = -1; line <=1; ++line )
    {
      std::ostringstream src,dest;
      src << "L" << srcLine+line;
      dest << "L" << destLine-line;
      if ( src.str() == dest.str() ) continue;
      
      for ( Int_t i = 0; i < lines.GetSize(); ++i )
      {
        TObjString* oline = (TObjString*)lines.At(i);
        
        TString& s = oline->String();
        
        if ( !s.Contains(GetKeywordPcb()) )
        {
          // Only consider PCB lines.
          continue;
        }
        
        if ( s.Contains(src.str().c_str()) )
        {
          AliDebugClass(4,Form("Replacing %s by %s in %s\n",
                               src.str().c_str(),dest.str().c_str(),s.Data()));
          
          s.ReplaceAll(src.str().c_str(),dest.str().c_str());
          
          AliDebugClass(4,s.Data());
          
          TString boardName(GetBoardNameFromPCBLine(s));
          
          if ( line )
          {
            // We must also change board numbers, with the tricky
            // thing that up and down must be swapped...
            // Up can only be 1 card so it must be B1
            // Down must be the uppper card of the line before, so
            // the biggest possible board number for this Line,Column
            
            if (line>0)
            {
                // force to B1
              AliDebugClass(4,Form("Forcing B1 in %s\n",s.Data()));
              s.ReplaceAll(boardName(boardName.Length()-2,2),"B1");
              AliDebugClass(4,s.Data());
            }
            else
            {
              // find the largest valid board number
              for ( int b = 4; b>=1; --b )
              {
                std::ostringstream bs;
                bs << boardName(0,boardName.Length()-1) << b;
                if ( LocalBoardNumber(bs.str().c_str()) >= 0 )
                {
                  AliDebugClass(4,Form("Replacing %s by %s in %s\n",
                                  boardName(boardName.Length()-2,2).Data(),
                                  Form("B%d",b),
                                  s.Data()));
                  s.ReplaceAll(boardName(boardName.Length()-2,2),
                               Form("B%d",b));
                  AliDebugClass(4,s);
                  break;
                }
              }
            }  
            // Check that the replacement we did is ok. If not,
            // skip the line.
            Int_t lbn = LocalBoardNumber(GetBoardNameFromPCBLine(s));
            if ( lbn < 0 )
            {
              AliDebugClass(4,Form("Removing line %s\n",s.Data()));
              lines.Remove(oline);
            }
            
          } // if (line)          
        }
      }    
    }
  }
}

//___________________________________________________________________________
Int_t
AliMpTriggerReader::IsLayerLine(const TString& sline) const
{
  /// Whether sline contains LAYER keyword

  if ( sline.BeginsWith(GetKeywordLayer()) )
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

//___________________________________________________________________________
Int_t
AliMpTriggerReader::DecodeFlipLine(const TString& sline,
                                   TString& slatType2,
                                   Bool_t& flipX, Bool_t& flipY)
{
  /// Decode a line containing FLIP_X and/or FLIP_Y keywords

  Ssiz_t blankPos = sline.First(' ');
  if ( blankPos < 0 ) return 0;
  
  TString keyword(sline(0,blankPos));
  
  if ( keyword == GetKeywordFlipX() )
  {
    flipX = kTRUE;
  } else if ( keyword == GetKeywordFlipY() )
  {
    flipY = kTRUE;
  }
  else
  {
    return 0;
  }
  
  slatType2 = sline(blankPos+1,sline.Length()-blankPos-1);
  return 1;
}

//___________________________________________________________________________
Int_t
AliMpTriggerReader::DecodeScaleLine(const TString& sline, 
                                    Double_t& scale, TString& slatType)
{
  /// Decode sline containing SCALE keyword

  if ( sline(0,GetKeywordScale().Length()) == GetKeywordScale() )
  {
    TString tmp(sline(GetKeywordScale().Length()+1,
                      sline.Length()-GetKeywordScale().Length()-1));
    Ssiz_t blankPos = tmp.First(' ');
    if ( blankPos < 0 )
    {
      AliErrorClass(Form("Syntax error in slat file, should get a slatType after "
                    " SCALE keyword : %s\n",tmp.Data()));
      return -1;
    }
    else
    {
      slatType = tmp(0,blankPos);
      scale = TString(tmp(blankPos+1,tmp.Length()-blankPos-1)).Atof();
      return 1;
    }
  }
  scale = 1.0;
  return 0;
}

//_____________________________________________________________________________
Int_t
AliMpTriggerReader::GetLine(const TString& slatType)
{
  ///
  /// Assuming slatType is a 4 character string of the form XSLN
  /// where X=1,2,3 or 4
  /// S = R or L
  /// N is the line number
  /// returns N
  
  if ( isdigit(slatType[0]) && 
       ( slatType[1] == 'R' || slatType[1] == 'L' ) &&
       slatType[2] == 'L' )
  {
    return atoi(slatType(3,1).Data());
  }
  return -1;
}

//_____________________________________________________________________________
int
AliMpTriggerReader::LocalBoardNumber(const char* localBoardName)
{
  /// From local board name to local board number

  if ( !fLocalBoardMap.GetSize() ) 
  {
    ReadLocalBoardMapping();
  }
  
  TPair* pair = (TPair*)fLocalBoardMap.FindObject(localBoardName);
  
  if (pair)
  {
    return atoi(((TObjString*)pair->Value())->String().Data());
  }
  return -1;
}

//_____________________________________________________________________________
void 
AliMpTriggerReader::ReadLines(const char* slatType,
                              AliMp::PlaneType planeType,
                              TList& lines,
                              Double_t& scale,
                              Bool_t& flipX, Bool_t& flipY,
                              Int_t& srcLine, Int_t& destLine)
{
  ///
  /// Reads in lines from file for a given slat
  /// Returns the list of lines (lines), together with some global
  /// information as the scale, whether to flip the lines, etc...
  ///
  AliDebugClass(2,Form("SlatType %s Scale %e FlipX %d FlipY %d srcLine %d"
                       " destLine %d\n",slatType,scale,flipX,flipY,
                       srcLine,destLine));
  
  istream& in 
    = fkDataStreams.
        CreateDataStream(AliMpFiles::SlatFilePath(
                             AliMp::kStationTrigger,slatType, planeType));
  
  char line[80];
  
  while ( in.getline(line,80) )
  {
    TString sline(AliMpHelper::Normalize(line));

    if ( sline.Length() == 0 || sline[0] == '#' ) continue;
    
    Bool_t isKeywordThere = 
      sline.Contains(GetKeywordPcb()) || 
      sline.Contains(GetKeywordLayer()) ||
      sline.Contains(GetKeywordScale()) || 
      sline.Contains(GetKeywordFlipX()) || 
      sline.Contains(GetKeywordFlipY());
    
    if ( !isKeywordThere ) 
    {
      AliErrorClass(Form("Got a line with no keyword : %s."
                         "That's not valid\n",line));
      continue; 
    }
    
    Double_t scale2;
    TString slatType2;
    
    Int_t isScaleLine = DecodeScaleLine(sline,scale2,slatType2);
    
    scale *= scale2;

    if ( isScaleLine < 0 )
    {
      AliFatalClass(Form("Syntax error near %s keyword\n",GetKeywordScale().Data()));
    }
    else if ( isScaleLine > 0 && slatType2 != slatType )
    {
      ReadLines(slatType2.Data(),planeType,lines,scale,flipX,flipY,srcLine,destLine);
    }
    else    
    {
      Bool_t fx(kFALSE);
      Bool_t fy(kFALSE);
      Int_t isFlipLine = DecodeFlipLine(sline,slatType2,fx,fy);
      if ( isFlipLine )
      {
        if (fy)
        {
          srcLine = GetLine(slatType2);
          destLine = GetLine(slatType);
        }
        flipX |= fx;
        flipY |= fy;
        ReadLines(slatType2.Data(),planeType,lines,scale,flipX,flipY,srcLine,destLine);
      }
      else
      {
        lines.Add(new TObjString(sline.Data()));
      }
    }
  }
  
  delete &in;
}
                                        
//_____________________________________________________________________________
void
AliMpTriggerReader::ReadLocalBoardMapping()
{
  /// Reads the file that contains the mapping local board name <-> number

  fLocalBoardMap.DeleteAll();
  
  UShort_t mask;
  
  istream& in 
    = fkDataStreams.
        CreateDataStream(AliMpFiles::LocalTriggerBoardMapping());

  char line[80];
  Char_t localBoardName[20];
  Int_t j,localBoardId;
  UInt_t switches;
  Int_t nofBoards;

  while (!in.eof())
  {
    for (Int_t i = 0; i < 4; ++i)
      if (!in.getline(line,80)) continue; //skip 4 first lines
 
    // read mask
    if (!in.getline(line,80)) break;
    sscanf(line,"%hx",&mask);
 
   // read # boards
    if (!in.getline(line,80)) break;
    sscanf(line,"%d",&nofBoards);
   
    for ( Int_t i = 0; i < nofBoards; ++i ) 
    {      
  
      if (!in.getline(line,80)) break; 
      sscanf(line,"%02d %19s %03d %03x", &j, localBoardName, &localBoardId, &switches);
      if (localBoardId <= AliMpConstants::NofLocalBoards()) 
      {
	fLocalBoardMap.Add(new TObjString(localBoardName), new TObjString(Form("%d",localBoardId)));
	AliDebugClass(10,Form("Board %s has number %d\n", localBoardName, localBoardId));
      }
      // skip 2 following lines
      if (!in.getline(line,80)) break; 
      if (!in.getline(line,80)) break; 
       
    }
  }
  
  delete &in;      
}

//_____________________________________________________________________________
AliMpPCB*
AliMpTriggerReader::ReadPCB(const char* pcbType)
{ 
  ///
  /// Create a new AliMpPCB object, by reading it from file.
  /// Returned pointer must be deleted by client.
  
  AliDebugClass(2,Form("pcbType=%s\n",pcbType));
  
  TString pcbName(pcbType);
  
  Ssiz_t pos = pcbName.First('x');

  Double_t scale = 1.0;
  
  if ( pos > 0 )
  {
    scale = TString(pcbName(pos+1,pcbName.Length()-pos-1)).Atof();
    pcbName = pcbName(0,pos);
  }
  
  istream& in 
    = fkDataStreams.
        CreateDataStream(AliMpFiles::SlatPCBFilePath(
                             AliMp::kStationTrigger,pcbName));
 
  AliMpMotifReader reader(fkDataStreams,
                          AliMp::kStationTrigger, AliMq::kNotSt12, AliMp::kNonBendingPlane); 
  // note that the nonbending
  // parameter is of no use for trigger, as far as reading motif is 
  // concerned, as all motifs are supposed to be in the same directory
  // (as they are shared by bending/non-bending planes).
     
  char line[80];
  
  const TString kSizeKeyword("SIZES");
  const TString kMotifKeyword("MOTIF");
  const TString kMotifSpecialKeyword("SPECIAL_MOTIF");
  
  AliMpPCB* pcb(0x0);
  
  while ( in.getline(line,80) )
  {
    if ( line[0] == '#' ) continue;
    
    TString sline(line);
    
    if ( sline(0,kSizeKeyword.Length()) == kSizeKeyword )
    {
      std::istringstream sin(sline(kSizeKeyword.Length(),
                                   sline.Length()-kSizeKeyword.Length()-1).Data());
      float padSizeX = 0.0;
      float padSizeY = 0.0;
      float pcbSizeX = 0.0;
      float pcbSizeY = 0.0;
      sin >> padSizeX >> padSizeY >> pcbSizeX >> pcbSizeY;
      if (pcb)
      {
        AliError("pcb not null as expected");
      }
      pcb = new AliMpPCB(fMotifMap,pcbType,padSizeX*scale,padSizeY*scale,
                         pcbSizeX*scale,pcbSizeY*scale);
    }
    
    if ( sline(0,kMotifSpecialKeyword.Length()) == kMotifSpecialKeyword )
    {
      std::istringstream sin(sline(kMotifSpecialKeyword.Length(),
                                   sline.Length()-kMotifSpecialKeyword.Length()).Data());
      TString sMotifSpecial;
      TString sMotifType;
      sin >> sMotifSpecial >> sMotifType;
      
      TString id = reader.MotifSpecialName(sMotifSpecial,scale);
      
      AliMpMotifSpecial* specialMotif =
        dynamic_cast<AliMpMotifSpecial*>(fMotifMap->FindMotif(id));
      if (!specialMotif)
      {
        AliDebug(1,Form("Reading motifSpecial %s (%s) from file",
                        sMotifSpecial.Data(),id.Data()));
        AliMpMotifType* motifType = fMotifMap->FindMotifType(sMotifType.Data());
        if ( !motifType)
        {
          AliDebug(1,Form("Reading motifType %s (%s) from file",
                          sMotifType.Data(),id.Data()));
          motifType = reader.BuildMotifType(sMotifType.Data());
          fMotifMap->AddMotifType(motifType);
        }
        else
        {
          AliDebug(1,Form("Got motifType %s (%s) from motifMap",
                          sMotifType.Data(),id.Data()));        
        }
        specialMotif = reader.BuildMotifSpecial(sMotifSpecial,motifType,scale);
        fMotifMap->AddMotif(specialMotif);
      }
      else
      {
        AliDebug(1,Form("Got motifSpecial %s from motifMap",sMotifSpecial.Data()));
      }
      if (pcb)
      {
        AliError("pcb not null as expected");
      }
      pcb = new AliMpPCB(pcbType,specialMotif);
    }
    
    if ( sline(0,kMotifKeyword.Length()) == kMotifKeyword )
    {
      std::istringstream sin(sline(kMotifKeyword.Length(),
                                   sline.Length()-kMotifKeyword.Length()).Data());
      TString sMotifType;
      int ix;
      int iy;
      sin >> sMotifType >> ix >> iy;
      
      AliMpMotifType* motifType = fMotifMap->FindMotifType(sMotifType.Data());
      if ( !motifType)
      {
        AliDebug(1,Form("Reading motifType %s from file",sMotifType.Data()));
        motifType = reader.BuildMotifType(sMotifType.Data());
        fMotifMap->AddMotifType(motifType);
      }
      else
      {
        AliDebug(1,Form("Got motifType %s from motifMap",sMotifType.Data()));        
      }
      
      if (! pcb)
      {
        AliError("pcb null");
        continue;
      }
      pcb->Add(motifType,ix,iy);
    }
  }
  
  delete &in;
  
  return pcb;
}

//_____________________________________________________________________________
AliMpTrigger*
AliMpTriggerReader::ReadSlat(const char* slatType, AliMp::PlaneType planeType)
{
  ///
  /// Create a new AliMpTrigger object, by reading it from file.
  /// Returned object must be deleted by client.

  Double_t scale = 1.0;
  Bool_t flipX = kFALSE;
  Bool_t flipY = kFALSE;
  TList lines;
  lines.SetOwner(kTRUE);
  Int_t srcLine(-1);
  Int_t destLine(-1);
  
  // Read the file and its include (if any) and store the result
  // in a TObjArray of TObjStrings.
  ReadLines(slatType,planeType,lines,scale,flipX,flipY,srcLine,destLine);

  // Here some more sanity checks could be done.
  // For the moment we only insure that the first line contains 
  // a layer keyword.
  TString& firstLine = ((TObjString*)lines.First())->String();
  if ( !IsLayerLine(firstLine) ) 
  {
    std::ostringstream s;
    s << GetKeywordLayer();
    lines.AddFirst(new TObjString(s.str().c_str()));
  }
  
  AliDebugClass(2,Form("Scale=%g\n",scale));
  
  FlipLines(lines,flipX,flipY,srcLine,destLine);
  
  // Now splits the lines in packets corresponding to different layers 
  // (if any), and create sub-slats.
  TObjArray layers;
  layers.SetOwner(kTRUE);
  Int_t ilayer(-1);
  TIter it(&lines);
  TObjString* osline;
  
  while ( ( osline = (TObjString*)it.Next() ) )
  {
    TString& s = osline->String();
    if ( IsLayerLine(s) )
    {
      TList* list = new TList;
      list->SetOwner(kTRUE);
      layers.Add(list);
      ++ilayer;
    }
    else
    {
      ((TList*)layers.At(ilayer))->Add(new TObjString(s));
    }
  }

  AliDebugClass(2,Form("nlayers=%d\n",layers.GetEntriesFast()));

  AliMpTrigger* triggerSlat = new AliMpTrigger(slatType, planeType);
    
  for ( ilayer = 0; ilayer < layers.GetEntriesFast(); ++ilayer )
  {
    TList& lines1 = *((TList*)layers.At(ilayer));
    std::ostringstream slatName;
    slatName << slatType << "-LAYER" << ilayer;
    AliMpSlat* slat = BuildSlat(slatName.str().c_str(),planeType,lines1,scale);
    if ( slat )
    {
      Bool_t ok = triggerSlat->AdoptLayer(slat);
      if (!ok)
      {
        StdoutToAliError(cout << "could not add slat=" << endl;
                         slat->Print();
                         cout << "to the triggerSlat=" << endl;
                         triggerSlat->Print();
                         );
        AliError("Slat is=");
        for ( Int_t i = 0; i < slat->GetSize(); ++i )
        {
          AliMpPCB* pcb = slat->GetPCB(i);
          AliError(Form("ERR pcb %d size %e,%e (unscaled is %e,%e)",
                                i,pcb->DX()*2,pcb->DY()*2,
                                pcb->DX()*2/scale,pcb->DY()*2/scale));
        }
        AliError("TriggerSlat is=");
        for ( Int_t j = 0; j < triggerSlat->GetSize(); ++j )
        {
          AliMpSlat* slat1 = triggerSlat->GetLayer(j);
          AliError(Form("Layer %d",j));
          for ( Int_t i = 0; i < slat1->GetSize(); ++i )
          {
            AliMpPCB* pcb = slat1->GetPCB(i);
            AliError(Form("ERR pcb %d size %e,%e (unscaled is %e,%e)",
                          i,pcb->DX()*2,pcb->DY()*2,
                          pcb->DX()*2/scale,pcb->DY()*2/scale));
          }
        } 
        StdoutToAliError(fMotifMap->Print(););
      }
    }
    else
    {
      AliErrorClass(Form("Could not read %s\n",slatName.str().c_str()));
      delete triggerSlat;
      return 0;
    }
  }
  
  return triggerSlat;
}
