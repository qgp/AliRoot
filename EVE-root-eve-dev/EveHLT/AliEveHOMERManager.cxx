// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007
// Author: Jochen Thaeder <thaeder@kip.uni-heidelberg.de>                *
//         for The ALICE HLT Project.                                    *

/** @file   AliEveHOMERManager.cxx
    @author Jochen Thaeder
    @date
    @brief  Manger for HOMER in offline
*/

#if __GNUC__>= 3
   using namespace std;
#endif

#include "AliEveHOMERManager.h"

#define use_aliroot
#define use_root
#define ROWHOUGHPARAMS
#define use_reconstruction
#define use_newio
#define ROOTVERSION    "unchecked"
#define ALIROOTVERSION "unchecked"
#define __ROOT__
#define USE_ALILOG
#define LINUX
// -- -- -- -- -- -- -- 
#include "AliHLTHOMERLibManager.h"
#include "AliHLTHOMERSourceDesc.h"
#include "AliHLTHOMERBlockDesc.h"
// -- -- -- -- -- -- -- 
#include "AliEveHOMERSource.h"
// -- -- -- -- -- -- -- 
#include "TString.h"
#include <TApplication.h>
#include "Riostream.h"
#include "TXMLAttr.h"
#include "TCollection.h"
#include "TList.h"
#include "TObjString.h"
#include "TObjArray.h"
// -- -- -- -- -- -- -- 
#include "AliLog.h"
#include "AliTPCCalibPedestal.h"
#include "AliTPCCalibPulser.h"
#include "AliTPCCalibCE.h"
#include "AliTPCPreprocessorOnline.h"
#include "AliTPCCalROC.h"

//______________________________________________________________________________
//
// Manage connections to HLT data-sources.

ClassImp(AliEveHOMERManager)

/*
 * ---------------------------------------------------------------------------------
 *                            Constructor / Destructor
 * ---------------------------------------------------------------------------------
 */

//##################################################################################
AliEveHOMERManager::AliEveHOMERManager( TString xmlFile ) :
  TEveElementList("AliEveHOMERManager"),
  fLibManager(new AliHLTHOMERLibManager),
  fXMLFile(xmlFile),
  fXMLParser(NULL),
  fRootNode(NULL),
  fSourceList(NULL),
  fReader(NULL),
  fRealm("GPN"),
  fBlockList(NULL),
  fNBlks(0),
  fEventID(0),
  fCurrentBlk(0),
  fConnected(kFALSE),
  fStateHasChanged(kTRUE),
  fSrcList(NULL),
  fTPCPre(NULL) {
  // This Class should handle the communication
  // from the HLT to AliEVE. The HLT sends data via 
  // the HOMER interface on several TCP ports of nodes 
  // in the CERN GPN and DCS network.
  // All this communication is hidden from the user.
  // 
  // Right now, a xml file ( SCC1 ) is used to get the
  // configuration, this will/ has to change to a proxy
  // running on dedicated nodes.
}

//##################################################################################
AliEveHOMERManager::~AliEveHOMERManager() {
  // The destructor

  if ( fLibManager ) {
    if ( fReader )
      fLibManager->DeleteReader(fReader);
    delete fLibManager;
    fLibManager = NULL;
    fReader = NULL;
  }

  if ( fXMLParser )
    delete fXMLParser;
  fXMLParser = NULL;

  if ( fSourceList != NULL )
    delete fSourceList;
  fSourceList = NULL;

  if ( fBlockList != NULL )
    delete fBlockList;
  fBlockList = NULL;

 if ( fSrcList != NULL )
    delete fSrcList;
  fSrcList = NULL;
  
  if ( fTPCPre != NULL )
    delete fTPCPre;
  fTPCPre = NULL;
}

/*
 * ---------------------------------------------------------------------------------
 *                            Source Handling
 * ---------------------------------------------------------------------------------
 */

//##################################################################################
Int_t AliEveHOMERManager::CreateHOMERSourcesList() {
  // Create Sources List from HOMER-Proxy

  // -- Initialize XML parser
  if ( fXMLParser != NULL )
    delete fXMLParser;
  fXMLParser = NULL;

  fXMLParser = new TDOMParser();
  fXMLParser->SetValidate( kFALSE );

  Int_t iResult = fXMLParser->ParseFile( fXMLFile );
  if ( iResult < 0 ) {
    iResult = 1;
    AliError( Form("Parsing file with error: %s", fXMLParser->GetParseCodeMessage( fXMLParser->GetParseCode() )) );
    return iResult;
  }

  // -- Initialize sources list
  DestroyElements();
  if ( fSourceList != NULL )
    delete fSourceList;
  fSourceList = NULL;

  fSourceList = new TList();
  fSourceList->SetOwner( kTRUE );

  // -- Set ROOT node
  fRootNode = fXMLParser->GetXMLDocument()->GetRootNode();

  TXMLNode * node = NULL;
  TXMLNode * prevNode = fRootNode->GetChildren();

  // -- Loop over all nodes
  while ( ( node = prevNode->GetNextNode() ) ) {
    prevNode = node;

    // -- Find only "Process" nodes, otherwise continue to next node
    if ( strcmp( node->GetNodeName(), "Proc" ) != 0 )
      continue;

    // -- Get Attributes of current node
    TList *attrList = node->GetAttributes();
    TXMLAttr *attr = 0;
    TIter next(attrList);

    while ( ( attr = (TXMLAttr*)next() ) ) {

      // -- Find "ID" attribute, otherwise continue to next attribute
      if ( strcmp( attr->GetName(), "ID" ) != 0 )
	continue;

      TString nodeId( attr->GetValue() );

      // -- Find only TDS processes
      TObjArray * nodeIdTok = nodeId.Tokenize("_");

      for ( Int_t ii=0 ; ii < nodeIdTok->GetEntries() ; ii++ ) {
	if ( ! ( (TObjString*) nodeIdTok->At(ii) )->GetString().CompareTo("TDS") ) {
	  iResult = GetTDSAttributes( node->GetChildren() );
	  if ( iResult ) {
	    AliError( Form("Error processing TDS process : %s", nodeId.Data()) );
	  }
	}
      }
    } // while ( ( attr = (TXMLAttr*)next() ) ) {

  } // while ( ( node = prevNode->GetNextNode() ) ) {

  // -- New SourceList has been created --> All Sources are new --> State has changed
  fStateHasChanged = kTRUE;

  if ( iResult ) {
    AliWarning( Form("There have been errors, while creating the sources list.") );
  }
  else {
    AliInfo( Form("New sources list created.") );
 
    if ( fSrcList ) 
      delete fSrcList;

    // -- Create new AliEVE sources list 
    fSrcList = new AliEveHOMERSourceList("HOMER Sources");
    fSrcList->SetManager(this);
    
    AddElement(fSrcList);
    fSrcList->CreateByType();
  }

  return iResult;
}

//##################################################################################
void AliEveHOMERManager::SetSourceState( AliHLTHOMERSourceDesc * source, Bool_t state ) {
  // Set state of a source
  // * param source      Pointer to AliHLTHOMERSourceDesc object.
  // * param state       New (selected/not selected) state.
  
  if ( source->IsSelected() != state ) {
    source->SetState( state );
    fStateHasChanged = kTRUE;
  }

  return;
}

//##################################################################################
Int_t AliEveHOMERManager::GetTDSAttributes( TXMLNode * xmlNode ) {
  // Get Information out of a TDS process in XML file
  // * param xmlNode   Pointer to childs of TDS node
  // * return          0 on sucess, > 0 on errorsee header file for class documentation

  Int_t iResult = 0;

  TXMLNode * attrNode = NULL;
  TXMLNode * prevNode = xmlNode;

  TString xmlHostname = 0;
  TString xmlPort = 0;

  TString hostname = 0;
  Int_t port = 0;

  // -- Get hostname and port from TDS node out of XML
  while ( ( attrNode = prevNode->GetNextNode() ) ) {
    prevNode = attrNode;

    // -- Get port out of the commandline
    if ( strcmp( attrNode->GetNodeName(), "Cmd" ) == 0 ) {
      TString cmd( attrNode->GetText() );

      TObjArray * cmdTok = cmd.Tokenize(" ");
      xmlPort = ((TObjString*) cmdTok->At(2))->GetString();
    }
    // -- Get hostname
    else if ( strcmp( attrNode->GetNodeName(), "Node" ) == 0 )
      xmlHostname = attrNode->GetText();

  } // while ( ( attrNode = prevNode->GetNextNode() ) ) {

  // -- Resolve hostname and port information
  iResult = ResolveHostPortInformation ( xmlHostname, xmlPort, hostname, port );
  if ( iResult == 1 ) {
    AliError( Form("Error resolving hostname : %s", xmlHostname.Data()) );
    return iResult;
  }
  else if ( iResult == 2 ) {AliInfo( Form("Connection established") );
    AliError( Form("Error resolving port : %s", xmlPort.Data()) );
    return iResult;
  }

  // -- Reset loop to TDS node
  prevNode = xmlNode;

  // -- Get Sources out of XML, resolve sources, add to sources List
  while ( ( attrNode = prevNode->GetNextNode() ) ) {
    prevNode = attrNode;

    // Find only "Parent" tags, otherwise continue to next tag
    if ( strcmp( attrNode->GetNodeName(), "Parent" ) != 0 )
      continue;

    TString xmlParent = attrNode->GetText();

    AliHLTHOMERSourceDesc * source = new AliHLTHOMERSourceDesc( hostname, port );

    if ( ResolveSourceInformation( xmlParent, source ) ) {
      iResult = 3;
      AliError( Form("Error resolving source : %s", xmlParent.Data()) );

      delete source;
    }
    else {
      fSourceList->Add( source );
      AliInfo( Form("New Source added : %s", xmlParent.Data()) );
    }

  } // while ( ( attrNode = prevNode->GetNextNode() ) ) {

  return iResult;
}

//##################################################################################
Int_t AliEveHOMERManager::ResolveHostPortInformation ( TString xmlHostname, TString xmlPort, TString &hostname, Int_t &port ) {
  // Resolve Information of hostname and port for source which has to be used by HOMER
  // ( due to port mapping inside the HLT )
  // * param xmlHostname  Hostname out of the XML
  // * param xmlPort      Port out of the XML
  // * param hostname     Return of the hostname
  // * param port         Return of the port
  // * return             0 on sucess, 1 if hostname couldn't be resolved, 2 if port couldn't be resolved,

  Int_t iResult = 1;
  TString nodeName = 0;
  TXMLNode * node = NULL;
  TXMLNode * prevNode = fRootNode->GetChildren();

  // *** Resolve hostname

  while ( ( node = prevNode->GetNextNode() ) && iResult == 1 ) {
    prevNode = node;

    // -- Find only "Node" nodes, otherwise continue
    if ( strcmp( node->GetNodeName(), "Node" ) != 0 )
      continue;

    // -- Get Attributes of current node
    TList *attrList = node->GetAttributes();
    TXMLAttr *attr = 0;
    TIter next(attrList);

    TString nodeId = 0;

    // Get "nodeID" and "nodeName" of this "Node" node
    while ( ( attr = (TXMLAttr*)next() ) ) {
      if ( strcmp( attr->GetName(), "ID" ) == 0 )
	nodeId = attr->GetValue();
      else if ( strcmp( attr->GetName(), "hostname" ) == 0 )
	nodeName = attr->GetValue();
    }

    // -- if this is not the correct nodeID continue
    if ( nodeId != xmlHostname )
      continue;

    // -- Set hostname
    if ( ! fRealm.CompareTo( "ACR" ) ) 
      hostname = "alihlt-dcs0.cern.ch";
    else if ( ! fRealm.CompareTo( "GPN" ) ) 
      hostname = "alihlt-vobox0.cern.ch";
    else 
      hostname = nodeName;

    iResult = 0;

    break;
  } // while ( ( node = prevNode->GetNextNode() ) ) {

  if ( iResult ) {
    AliError( Form("Error resolving hostname : %s", xmlHostname.Data()) );
    return iResult;
  }

  // *** Resolve port

  if ( xmlPort.IsDigit() ) {

    if ( nodeName.CompareTo("feptriggerdet") ==0 ){
      if ( xmlPort.CompareTo("49152") == 0 ){
	port = 58140;
      } else if ( xmlPort.CompareTo("49153") == 0 ){
	port = 58141;
      }
    } else if ( nodeName.CompareTo("fepfmdaccorde") == 0 ){
      if ( xmlPort.CompareTo("49152") == 0 ){
	port = 58144;
      } else if ( xmlPort.CompareTo("49153") == 0 ){
	port = 58145;
      }
    } else if ( nodeName.CompareTo("feptpcao15") == 0 ){
      if ( xmlPort.CompareTo("49152") == 0 ){
	port = 50340;
      } else if ( xmlPort.CompareTo("49153") == 0 ){
	port = 50341;
      }
    } else if ( nodeName.CompareTo("fepphos2") == 0 ){
      if ( xmlPort.CompareTo("49152") == 0 ){
	port = 58656;
      } else if ( xmlPort.CompareTo("58656") == 0 ){
	port = 58656;
      }
   } else if ( nodeName.CompareTo("fepphos3") == 0 ){
      if ( xmlPort.CompareTo("49152") == 0 ){
	port = 58660;
      } else if ( xmlPort.CompareTo("58660") == 0 ){
	port = 58660;
      }
   } else if ( nodeName.CompareTo("fepphos4") == 0 ){
      if ( xmlPort.CompareTo("49152") == 0 ){
	port = 58664;
      } else if ( xmlPort.CompareTo("58664") == 0 ){
	port = 58664;
      }
    } else if ( nodeName.CompareTo("alihlt-vobox0") == 0 ){
      port = xmlPort.Atoi();
    }
  }
  else {
    AliError( Form("Error resolving port : %s", xmlPort.Data()) );
    iResult = 2;
  }

  // *** Summary

  if ( !iResult ) {
    AliInfo( Form("%s:%i resolved out of %s:%s", hostname.Data(), port, xmlHostname.Data(), xmlPort.Data()) );
  }

  return iResult;
}

//##################################################################################
Int_t AliEveHOMERManager::ResolveSourceInformation( TString xmlParent, AliHLTHOMERSourceDesc *source ) {
  // Resolve information of source
  // * param xmlParent   ParentString out of the XML
  // * param source      Return the filled AliHLTHOMERSourceDesc object
  // * return            0 on sucess, 1 on errorsee header file for class documentation

  Int_t iResult = 0;

  if ( ! xmlParent.Contains( "_" ) ) {
    AliError( Form("Source %s could not be resolved", xmlParent.Data() ) );
    iResult = 1;

    return iResult;
  }

  TObjArray * parentTokens = xmlParent.Tokenize("_");

  Int_t nEntries = parentTokens->GetEntries();

  TString detector = ((TObjString*) parentTokens->At(0) )->GetString();
  TString subDetector = "";
  TString subSubDetector = "";
  TString dataType = "";
  ULong_t specification = 0;

  TString name = ((TObjString*) parentTokens->At(1) )->GetString();
  TString objName = "";

  if ( nEntries == 3 )
    subDetector = ((TObjString*) parentTokens->At(2) )->GetString();
  else if ( nEntries == 4 ) {
    subDetector = ((TObjString*) parentTokens->At(2) )->GetString();
    subSubDetector = ((TObjString*) parentTokens->At(3) )->GetString();
  }

  // -- Corecct TPC subdetector, because in we have somtimes "A","C"
  if ( ! detector.CompareTo("TPC") ) {
    if ( subDetector.BeginsWith('A') ) {
      subDetector.Remove( TString::kLeading, 'A' );
    }
    else if ( subDetector.BeginsWith('C') ) {
      subDetector.Remove( TString::kLeading, 'C' );
      Int_t tmp = subDetector.Atoi() + 18;
      subDetector = "";
      subDetector += tmp;
    }
  }
  
  // -- Correct for MUON
  if ( ! detector.CompareTo("DIMU") ) {
    detector = "MUON";
    
    if ( ! subDetector.CompareTo("TRG") )
      subDetector = "1";
    else if ( ! subDetector.CompareTo("TRK") )
      subDetector = "2";
  }
  
  // -- Remove Leading '0' in sub detector and subsubdetector
  subDetector.Remove( TString::kLeading, '0' );
  subSubDetector.Remove( TString::kLeading, '0' );
  
  // -- Set Object Names
  
  // **** General ****
  if ( name == "RP" || name == "FP" || name == "Relay" ) {
    objName = "";
    dataType = "DDL_RAW";
    specification = 0;
  }
  
  // **** TPC ****
  else if ( detector == "TPC" ) {
    
    if ( name == "CalibPedestal" ) {
      objName = "AliTPCCalibPedestal";
      dataType = "HIS_CAL";
      specification = 0;
    }
    else if ( name == "CalibPulser" ) {
      objName = "AliTPCCalibPulser";
      dataType = "HIS_CAL";
      specification = 0;
    }
    else if ( name == "CF" || name == "RelayCF" ) {
      objName = "AliHLTTPCClusterDataFormat";
      dataType = "CLUSTERS";
      specification = 0;
    }
    else if ( name == "ESDConv" ) {
      objName = "TTree";
      dataType = "ESD_TREE";
      specification = 0;
    }
    else if ( name == "KryptonCF" ) {
      objName = "TObjArray";
      dataType = "KRPTHIST";
      specification = 0;
    }
    else {
      // not defined yet ...
      AliError( Form("Parent Process not defined yet : %s .", name.Data()) );
      iResult = 1;
    }

  } // if ( detector == "TPC" ) {

  // **** TRD ****
  else if ( detector == "TRD" ) {

    if ( name == "foo" ) {
      objName = "bar";
      dataType = "FOO_BAR";
      specification = 0;
    }
  } // else if ( detector == "TRD" ) {

  // **** PHOS ****
  else if ( detector == "PHOS" ) {

  } // else if ( detector == "PHOS" ) {

  // **** DIMU ****
  else if ( detector == "MUON" ) {
 Int_t UpdateSourcesFromSourcesList( );
  } // else if ( detector == "MUON" ) {

  // -- Fill object
  source->SetSourceName( name, objName );
  source->SetSourceType( specification, dataType );
  source->SetDetectors( detector, subDetector, subSubDetector );

  //  AliInfo( Form("Set Source %s , Type %s, ClassName %s .", name.Data(), dataType.Data(), objName.Data()) );
  //  AliInfo( Form("    Detector %s , SubDetector : %s, SubSubDetector %s .",
  //		detector.Data(), subDetector.Data(), subSubDetector.Data()) );

  return iResult;
}

/*
 * ---------------------------------------------------------------------------------
 *                            Connection Handling
 * ---------------------------------------------------------------------------------
 */

//##################################################################################
Int_t AliEveHOMERManager::ConnectHOMER(){
  // Connect to HOMER sources, out of Readout List, which gets created when state has changed
  // * return            0 on sucess, "HOMER" errors on error


  Int_t iResult = 0;

  fStateHasChanged = fSrcList->GetSelectedSources();

  // -- Check if already connected and state has not changed
  if ( fStateHasChanged == kFALSE && IsConnected() ) {
    AliInfo( Form("No need for reconnection.") );
    return iResult;
  }

  // -- If already connected, disconnect before connect
  if ( IsConnected() )
    DisconnectHOMER();

  // *** Create the Readoutlist

  UShort_t* sourcePorts = new UShort_t [fSourceList->GetEntries()];
  const char ** sourceHostnames = new const char* [fSourceList->GetEntries()];
  UInt_t sourceCount = 0;

  CreateReadoutList( sourceHostnames, sourcePorts, sourceCount );

  if ( sourceCount == 0 ) {
    AliError(Form("No sources selected, aborting.") );
    return iResult;
  }

  // *** Connect to data sources

  if ( !fReader ) {
    if ( fLibManager )
      fReader = fLibManager->OpenReader( sourceCount, sourceHostnames, sourcePorts );
  }

  iResult = fReader->GetConnectionStatus();

  if ( iResult ) {
    // -- Connection failed

    UInt_t ndx = fReader->GetErrorConnectionNdx();

    if ( ndx < sourceCount ) {
      AliError( Form("Error : Error establishing connection to TCP source %s:%hu: %s (%d)",
		     sourceHostnames[ndx], sourcePorts[ndx], strerror(iResult), iResult) );
    }
    else {
      AliError( Form("Error : Error establishing connection to unknown source with index %d: %s (%d)",
		     ndx, strerror(iResult), iResult) );
    }

    if ( fReader )
      fLibManager->DeleteReader( fReader );
    fReader = NULL;

  }
  else {
    // -- Connection ok - set reader
    fConnected = kTRUE;

    AliInfo( Form("Connection established") );
  }

  delete[] sourceHostnames;
  delete[] sourcePorts;


  // -- Get next event
  if ( ! iResult )
    NextEvent();

  return iResult;
}

//##################################################################################
void AliEveHOMERManager::DisconnectHOMER(){
  // Disconnect from HOMER sources

  if ( ! IsConnected() )
    return;

  if ( fReader )
    fLibManager->DeleteReader( fReader );
  fReader = NULL;

  fStateHasChanged = kTRUE;
  fConnected = kFALSE;

  AliInfo( Form("Connection closed") );

  return;
}

//##################################################################################
Int_t AliEveHOMERManager::ReconnectHOMER(){
  // Reconnect from HOMER sources
  // * return            0 on sucess, "ConnectHOMER()" errors on error

  Int_t iResult = 0;

  if ( IsConnected() )
    DisconnectHOMER();

  iResult = ConnectHOMER();
  if ( iResult ) {
    AliError( Form("Error connecting.") );
  }

  return iResult;
}


//##################################################################################
void AliEveHOMERManager::CreateReadoutList( const char** sourceHostnames, UShort_t *sourcePorts, UInt_t &sourceCount ){
  //  Create a readout list for Hostname and ports
  // * param socurceHostnames   Array of selected hostnames
  // * param socurcePorts       Array of selected ports
  // * param socurceCount       Number of selected hostname:port

  AliHLTHOMERSourceDesc * source= NULL;

  // -- Read all sources and check if they should be read out
  TIter next( fSourceList );
  while ( ( source = (AliHLTHOMERSourceDesc*)next() ) ) {

    if ( ! source->IsSelected() )
      continue;

    Bool_t exists = kFALSE;

    // -- Loop over existing entries and check if entry is already in readout list
    for ( UInt_t ii = 0; ii < sourceCount; ii++ ){
      if ( !strcmp( sourceHostnames[ii], source->GetHostname().Data() ) &&  sourcePorts[ii] == source->GetPort() ) {
	exists = kTRUE;
	break;
      }
    }

    // -- Add new entires to readout list
    if ( ! exists ) {
      sourcePorts[sourceCount] = source->GetPort();
      sourceHostnames[sourceCount] = source->GetHostname().Data();
      sourceCount++;
    }

  } // while ( ( source = (AliHLTHOMERSourceDesc*)next() ) ) {

  fStateHasChanged = kFALSE;

  return;
}

/*
 * ---------------------------------------------------------------------------------
 *                            Event Handling
 * ---------------------------------------------------------------------------------
 */

//##################################################################################
Int_t AliEveHOMERManager::NextEvent(){
  // Loads the next Event, after being connected
  // * return            0 on sucess, "HOMER" errors on error

  Int_t iResult = 0;

  if ( !fReader || ! IsConnected() ) {
    AliWarning( Form( "Not connected yet." ) );
    return 1;
  }

  // -- Read next event data and error handling for HOMER (error codes and empty blocks)
  while( 1 ) {
    iResult = fReader->ReadNextEvent( 20000000 /*timeout in us*/);

    if ( iResult == 111 || iResult == 32 || iResult == 6 ) {
      Int_t ndx = fReader->GetErrorConnectionNdx();
      AliError( Form("Error, No Connection to source %d: %s (%d)", 
		     ndx, strerror(iResult), iResult) );
      fConnected = kFALSE;
      return 2;
    }
    else if ( iResult == 110 ) {
      Int_t ndx = fReader->GetErrorConnectionNdx();
      AliError( Form("Timout occured, reading event from source %d: %s (%d)", 
		     ndx, strerror(iResult), iResult) );
      fConnected = kFALSE;
      return 3;
    }
    else if ( iResult == 56) {
      Int_t ndx = fReader->GetErrorConnectionNdx();
      AliError( Form("Retry: Error reading event from source %d: %s (%d)", 
		     ndx, strerror(iResult), iResult) );
      fConnected = kFALSE;
      continue;
    }
    else if ( iResult ) {
      Int_t ndx = fReader->GetErrorConnectionNdx();
      AliError( Form("General Error reading event from source %d: %s (%d)", 
		     ndx, strerror(iResult), iResult) );
      fConnected = kFALSE;
      return 2;
    }
    else {
      break;
    }
  } // while( 1 ) {

  if ( iResult )
    return iResult;
  
  // -- Get blockCnt and eventID
  fNBlks = (ULong_t) fReader->GetBlockCnt();
  fEventID = (ULong64_t) fReader->GetEventID();
  fCurrentBlk = 0;

  AliInfo( Form("Event 0x%016LX (%Lu) with %lu blocks", fEventID, fEventID, fNBlks) );

#if 0

  // Loop for Debug only
  for ( ULong_t i = 0; i < fNBlks; i++ ) {
    Char_t tmp1[9], tmp2[5];
    memset( tmp1, 0, 9 );
    memset( tmp2, 0, 5 );
    void *tmp11 = tmp1;
    ULong64_t* tmp12 = (ULong64_t*)tmp11;
    *tmp12 = fReader->GetBlockDataType( i );
    void *tmp21 = tmp2;
    ULong_t* tmp22 = (ULong_t*)tmp21;
    *tmp22 = fReader->GetBlockDataOrigin( i );
    AliInfo( Form("Block %lu length: %lu - type: %s - origin: %s",
		  i, fReader->GetBlockDataLength( i ), tmp1, tmp2) );
  } // end for ( ULong_t i = 0; i < fNBlks; i++ ) {

#endif

  // -- Create BlockList
  AliInfo( Form("Create Block List") );
  iResult = CreateBlockList();

  return iResult;
}

//##################################################################################
Int_t AliEveHOMERManager::CreateBlockList() {
  // Create a TList of blocks, which have been readout

  Int_t iResult = 0;

  // -- Initialize block list
  if ( fBlockList != NULL )
    delete fBlockList;
  fBlockList = NULL;

  fBlockList = new TList();
  fBlockList->SetOwner( kTRUE );

  void* iter = GetFirstBlk();

  // -- Fill block list
  while ( iter != NULL ){

    // -- Create new block
    AliHLTHOMERBlockDesc * block = new AliHLTHOMERBlockDesc( GetBlk(), GetBlkSize(), GetBlkOrigin(),
							     GetBlkType(), GetBlkSpecification() );

    // -- Check sources list if block is requested
    if ( CheckIfRequested( block ) )
      fBlockList->Add( block );
    else {
      delete block;
      block = NULL;
    }
    iter = GetNextBlk();

  } // while ( iter != NULL ){

  return iResult;
}

/*
 * ---------------------------------------------------------------------------------
 *                            BlockHandling
 * ---------------------------------------------------------------------------------
 */

//##################################################################################
void* AliEveHOMERManager::GetBlk( Int_t ndx ) {
  // Get pointer to current block in current event
  // * param ndx        Block index
  // * return           returns pointer to blk, NULL if no block present
   
  void* data = NULL;

  if ( !fReader || ! IsConnected() ) {
    AliError( Form("Not connected yet.") );
  }
  else {
    if ( ( ndx ) < (Int_t) fNBlks )
      data = (void*) fReader->GetBlockData( ndx );
  }

  return data;
}

//##################################################################################
ULong_t AliEveHOMERManager::GetBlkSize( Int_t ndx ) {
  // Get size of block ndx
  // * param ndx        Block index
  // * return           returns pointer to blk, 0 if no block present
   
  ULong_t length = 0;

  if ( !fReader || ! IsConnected() ) {
    AliError( Form("Not connected yet.") );
  }
  else {
    if ( ( ndx ) < (Int_t) fNBlks )
      length = (ULong_t) fReader->GetBlockDataLength( ndx );
  }

  return length;
}

//##################################################################################
TString AliEveHOMERManager::GetBlkOrigin( Int_t ndx ) {
  // Get origin of block ndx
  // * param ndx        Block index
  // * return           origin of block

  TString origin = "";

  // -- Check for Connection
  if ( !fReader || ! IsConnected() ) {
    AliError( Form("Not connected yet.") );
    return origin;
  }

  // -- Check block index
  if ( ( ndx ) >= (Int_t) fNBlks ) {
    AliError( Form("Block index %d out of range.", ndx ) );
    return origin;
  }

  // -- Get origin
  union{
    UInt_t data;
    Char_t array[4];
  } reverseOrigin;

  reverseOrigin.data = (UInt_t) fReader->GetBlockDataOrigin( ndx );

  // -- Reverse the order
  for (Int_t ii = 3; ii >= 0; ii-- )
    if ( reverseOrigin.array[ii] != ' ')
      origin.Append( reverseOrigin.array[ii] );

  return origin;
}

//##################################################################################
TString AliEveHOMERManager:: GetBlkType( Int_t ndx ) {
  // Get type of block ndx
  // * param ndx        Block index
  // * return           type of block

  TString type = "";

  // -- Check for Connection
  if ( !fReader || ! IsConnected() ) {
    AliError( Form("Not connected yet.") );
    return type;
  }

  // -- Check blockk index
  if ( ( ndx ) >= (Int_t) fNBlks ) {
    AliError( Form("Block index %d out of range.", ndx ) );
    return type;
  }

  // -- Get type
  union{
    ULong64_t data;
    Char_t array[8];
  } reverseType;

  reverseType.data = (ULong64_t) fReader->GetBlockDataType( ndx );

  // -- Reverse the order
  for (Int_t ii = 7; ii >= 0; ii-- )
    if ( reverseType.array[ii] != ' ')
      type.Append( reverseType.array[ii] );

  return type;
}

//##################################################################################
ULong_t AliEveHOMERManager:: GetBlkSpecification( Int_t ndx ) {
  // Get specification of block ndx
  // * param ndx        Block index
  // * return           specification of block

  ULong_t spec = 0;

  // -- Check for Connection
  if ( !fReader || ! IsConnected() ) {
    AliError( Form("Not connected yet.") );
    return spec;
  }

  // -- Check blockk index
  if ( ( ndx ) >= (Int_t) fNBlks ) {
    AliError( Form("Block index %d out of range.", ndx ) );
    return spec;
  }

  spec = (ULong_t) fReader->GetBlockDataSpec( ndx );

  return spec;
}

//##################################################################################
Bool_t AliEveHOMERManager::CheckIfRequested( AliHLTHOMERBlockDesc * block ) {
  // Checks if current Block should was requested
  // * return           returns kTRUE, if block should was requested

  Bool_t requested = kFALSE;

  AliHLTHOMERSourceDesc * source= NULL;

  // -- Read all sources and check if they should be read out
  TIter next( fSourceList );
  while ( ( source = (AliHLTHOMERSourceDesc*)next() ) ) {

    if ( ! source->IsSelected() )
      continue;

    if ( source->GetDetector().CompareTo( block->GetDetector() ) )
      continue;

    if ( source->GetDataType().CompareTo( block->GetDataType() ) )
      continue;

    if ( ! block->HasSubDetectorRange() ) {

      if ( source->GetSubDetector().Atoi() != block->GetSubDetector().Atoi() )
	continue;

      if ( ! block->HasSubSubDetectorRange() ) {

	if ( source->GetSubSubDetector().Atoi() != block->GetSubSubDetector().Atoi() )
	  continue;

      } // if ( ! block->HasSubSubDetectorRange ) {
    } //  if ( ! block->HasSubDetectorRange ) {

    requested = kTRUE;

  } // while ( ( source = (AliHLTHOMERSourceDesc*)next() ) ) {

  if ( requested) {
    AliInfo( Form("Block requested : %s - %s : %s/%s -> %s ", block->GetDetector().Data(), block->GetDataType().Data(),
		  block->GetSubDetector().Data(), block->GetSubSubDetector().Data(), block->GetClassName().Data() ) );
  }
  else
    AliInfo( Form("Block NOT requested : %s - %s : %s/%s -> %s ", block->GetDetector().Data(), block->GetDataType().Data(), 		  block->GetSubDetector().Data(), block->GetSubSubDetector().Data(), block->GetClassName().Data() ) );

  return requested;
}

/*
 * ---------------------------------------------------------------------------------
 *                            Test Realm ....
 * ---------------------------------------------------------------------------------
 */

//##################################################################################
void AliEveHOMERManager::DumpTPCCalib(TString objectName, Bool_t dumpToFile) {
  // Still under testing ...

  if ( fTPCPre != NULL )
    delete fTPCPre;

  fTPCPre = new AliTPCPreprocessorOnline();

  TList* blockList = GetBlockList();

  AliHLTHOMERBlockDesc *desc = 0;

  TIter next(blockList);

  while ( ( desc = (AliHLTHOMERBlockDesc*)next() ) ) {
    if ( ! desc->IsTObject() )
      continue;

    Int_t sectorTPC = 0;

    if ( desc->GetSubSubDetector().Atoi() <= 1 ) {
      sectorTPC = desc->GetSubDetector().Atoi();
    }
    else {
      sectorTPC = 36 + desc->GetSubDetector().Atoi();
    }

    if ( ! objectName.CompareTo( desc->GetClassName() ) ){

      //
      // AliTPCCalibPedestal
      //

      if ( ! objectName.CompareTo( "AliTPCCalibPedestal" ) ) {
	AliTPCCalROC* calROC = NULL;

	AliTPCCalibPedestal * cal = (AliTPCCalibPedestal*) desc->GetTObject();
	if ( cal == NULL ) {
	  cout << "error 1" << endl;
	  continue;
	}

	cal->Analyse();

	calROC = cal->GetCalRocRMS(sectorTPC);
	if ( calROC == NULL ) {
	  cout << "error 2" << endl;
	  continue;
	}

	calROC->SetName(Form("RMS_ROC%d", sectorTPC));
	fTPCPre->AddComponent((TObject*) calROC );

	calROC = cal->GetCalRocPedestal(sectorTPC);
	if ( calROC == NULL ) {
	  cout << "error 3" << endl;
	  continue;
	}


	calROC->SetName(Form("Pedestal_ROC%d", sectorTPC));
	cout << "added" << endl;
	fTPCPre->AddComponent((TObject*) calROC );
      }

      //
      // AliTPCCalibPulser
      //
      /*
      else if ( ! objectName.CompareTo( "AliTPCCalibPulser" ) ) {
	AliTPCCalROC* calROC = NULL;

	AliTPCCalibPulser * cal = (AliTPCCalibPulser*) desc->GetTObject();

	cal->Analyse();

	calROC = cal->GetCalRocT0(sectorTPC);
	calROC->SetName(Form("T0_ROC%d", sectorTPC));
	fTPCPre->AddComponent((TObject*) calROC );

	calROC = cal->GetCalRocQ(sectorTPC);
	calROC->SetName(Form("Q_ROC%d", sectorTPC));
	fTPCPre->AddComponent((TObject*) calROC );

	calROC = cal->GetCalRocRMS(sectorTPC);
	calROC->SetName(Form("RMS_ROC%d", sectorTPC));
	fTPCPre->AddComponent((TObject*) calROC );

	calROC = cal->GetCalRocOutliers(sectorTPC);
	calROC->SetName(Form("Outliers_ROC%d", sectorTPC));
	fTPCPre->AddComponent((TObject*) calROC );
      }

*/
      //
      // AliTPCCalibCE
      //
      /*
      else if ( ! objectName.CompareTo( "AliTPCCalibCE" ) ) {
	AliTPCCalROC* calROC = NULL;

	AliTPCCalibPulser * cal = (AliTPCCalibPulser*) desc->GetTObject();

	cal->Analyse();

	calROC = cal->GetCalRocT0(sectorTPC);
	calROC->SetName(Form("T0_ROC%d", sectorTPC));
	fTPCPre->AddComponent((TObject*) calROC );

	calROC = cal->GetCalRocQ(sectorTPC);
	calROC->SetName(Form("Q_ROC%d", sectorTPC));
	fTPCPre->AddComponent((TObject*) calROC );

	calROC = cal->GetCalRocRMS(sectorTPC);
	calROC->SetName(Form("RMS_ROC%d", sectorTPC));
	fTPCPre->AddComponent((TObject*) calROC );

	calROC = cal->GetCalRocOutliers(sectorTPC);
	calROC->SetName(Form("Outliers_ROC%d", sectorTPC));
	fTPCPre->AddComponent((TObject*) calROC );
      }
      */
    } // if ( ! objectName.CompareTo( desc->GetClassName() ) ) {

  } // while ( ( desc = (AliHLTHOMERBlockDesc*)next() ) ) {

  if ( dumpToFile ) {

    fTPCPre->DumpToFile("pedestals.root");
    cout << "DUMP" << endl;
  }


}
