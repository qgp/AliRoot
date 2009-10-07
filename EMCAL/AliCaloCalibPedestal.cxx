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

//________________________________________________________________________
//
// A help class for monitoring and calibration tools: MOOD, AMORE etc.,
// It can be created and used a la (ctor):
/*
  //Create the object for making the histograms
  fPedestals = new AliCaloCalibPedestal( fDetType );
  // AliCaloCalibPedestal knows how many modules we have for PHOS or EMCAL
  fNumModules = fPedestals->GetModules();
*/
// fed an event:
//  fPedestals->ProcessEvent(fCaloRawStream);
// asked to draw histograms:
//  fPedestals->GetDeadMap(i)->Draw("col");
// or
//  fPedestals->GetPeakProfileHighGainRatio((i < fNumModules) ? i : fVisibleModule)->Draw("colz");
// etc.
// The pseudo-code examples above were from the first implementation in MOOD (summer 2007).
//________________________________________________________________________

#include "TH1.h"
#include "TFile.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

#include "AliCaloRawStreamV3.h"

//The include file
#include "AliCaloCalibPedestal.h"

ClassImp(AliCaloCalibPedestal)

using namespace std;

// ctor; initialize everything in order to avoid compiler warnings
AliCaloCalibPedestal::AliCaloCalibPedestal(kDetType detectorType) :  
  TObject(),
  fPedestalLowGain(),
  fPedestalHighGain(),
  fPedestalRMSLowGain(),
  fPedestalRMSHighGain(),
  fPeakMinusPedLowGain(),
  fPeakMinusPedHighGain(),
  fPedestalLowGainDiff(),
  fPedestalHighGainDiff(),
  fPeakMinusPedLowGainDiff(),
  fPeakMinusPedHighGainDiff(),
  fPedestalLowGainRatio(),
  fPedestalHighGainRatio(),
  fPeakMinusPedLowGainRatio(),
  fPeakMinusPedHighGainRatio(),
  fDeadMap(),
  fNEvents(0),
  fNChanFills(0),
  fDeadTowers(0),
  fNewDeadTowers(0),
  fResurrectedTowers(0),
  fReference(0),
  fDetType(kNone),
  fColumns(0),
  fRows(0),
  fModules(0),
  fRowMin(0),
  fRowMax(0),
  fRowMultiplier(0),
  fCaloString(),
  fMapping(NULL),
  fRunNumber(-1),
  fSelectPedestalSamples(kTRUE), 
  fFirstPedestalSample(0),
  fLastPedestalSample(15)
{
  //Default constructor. First we set the detector-type related constants.
  if (detectorType == kPhos) {
    fColumns = fgkPhosCols;
    fRows = fgkPhosRows;
    fModules = fgkPhosModules;
    fCaloString = "PHOS";
    fRowMin = -1*fRows;
    fRowMax = 0;
    fRowMultiplier = -1;
  } 
  else {
    //We'll just trust the enum to keep everything in line, so that if detectorType
    //isn't kPhos then it is kEmCal. Note, however, that this is not necessarily the
    //case, if someone intentionally gives another number
    fColumns = AliEMCALGeoParams::fgkEMCALCols;
    fRows = AliEMCALGeoParams::fgkEMCALRows;
    fModules = AliEMCALGeoParams::fgkEMCALModules;
    fCaloString = "EMCAL";
    fRowMin = 0;
    fRowMax = fRows;
    fRowMultiplier = 1;
  } 
  fDetType = detectorType;
 
  //Then, loop for the requested number of modules
  TString title, name;
  for (int i = 0; i < fModules; i++) {
    //Pedestals, low gain
    name = "hPedlowgain";
    name += i;
    title = "Pedestals, low gain, module ";
    title += i; 
    fPedestalLowGain.Add(new TProfile2D(name, title,
					fColumns, 0.0, fColumns, 
					fRows, fRowMin, fRowMax,"s"));
  
    //Pedestals, high gain
    name = "hPedhighgain";
    name += i;
    title = "Pedestals, high gain, module ";
    title += i; 
    fPedestalHighGain.Add(new TProfile2D(name, title,
					 fColumns, 0.0, fColumns, 
					 fRows, fRowMin, fRowMax,"s"));
    //All Samples, low gain
    name = "hPedestalRMSlowgain";
    name += i;
    title = "Pedestal RMS, low gain, module ";
    title += i; 
    fPedestalRMSLowGain.Add(new TProfile2D(name, title,
					fColumns, 0.0, fColumns, 
					fRows, fRowMin, fRowMax,"s"));
  
    //All Samples, high gain
    name = "hPedestalRMShighgain";
    name += i;
    title = "Pedestal RMS, high gain, module ";
    title += i; 
    fPedestalRMSHighGain.Add(new TProfile2D(name, title,
					 fColumns, 0.0, fColumns, 
					 fRows, fRowMin, fRowMax,"s"));
  
    //Peak-Pedestals, low gain
    name = "hPeakMinusPedlowgain";
    name += i;
    title = "Peak-Pedestal, low gain, module ";
    title += i; 
    fPeakMinusPedLowGain.Add(new TProfile2D(name, title,
					    fColumns, 0.0, fColumns, 
					    fRows, fRowMin, fRowMax,"s"));
  
    //Peak-Pedestals, high gain
    name = "hPeakMinusPedhighgain";
    name += i;
    title = "Peak-Pedestal, high gain, module ";
    title += i; 
    fPeakMinusPedHighGain.Add(new TProfile2D(name, title,
					     fColumns, 0.0, fColumns, 
					     fRows, fRowMin, fRowMax,"s"));
  
    name = "hDeadMap";
    name += i;
    title = "Dead map, module ";
    title += i;
    fDeadMap.Add(new TH2D(name, title, fColumns, 0.0, fColumns, 
			  fRows, fRowMin, fRowMax));
  
  }//end for nModules create the histograms
 
  //Compress the arrays, in order to remove the empty objects (a 16 slot array is created by default)
  fPedestalLowGain.Compress();
  fPedestalHighGain.Compress();
  fPedestalRMSLowGain.Compress();
  fPedestalRMSHighGain.Compress();
  fPeakMinusPedLowGain.Compress();
  fPeakMinusPedHighGain.Compress();
  fDeadMap.Compress();
  //Make them the owners of the profiles, so we don't need to care about deleting them
  //fPedestalLowGain.SetOwner();
  //fPedestalHighGain.SetOwner();
  //fPeakMinusPedLowGain.SetOwner();
  //fPeakMinusPedHighGain.SetOwner();
  
}

// dtor
//_____________________________________________________________________
AliCaloCalibPedestal::~AliCaloCalibPedestal()
{
  if (fReference) delete fReference;//Delete the reference object, if it has been loaded
  //TObjArray will delete the histos/profiles when it is deleted.
}

// copy ctor
//_____________________________________________________________________
AliCaloCalibPedestal::AliCaloCalibPedestal(const AliCaloCalibPedestal &ped) :
  TObject(ped),
  fPedestalLowGain(),
  fPedestalHighGain(),
  fPedestalRMSLowGain(),
  fPedestalRMSHighGain(),
  fPeakMinusPedLowGain(),
  fPeakMinusPedHighGain(),
  fPedestalLowGainDiff(),
  fPedestalHighGainDiff(),
  fPeakMinusPedLowGainDiff(),
  fPeakMinusPedHighGainDiff(),
  fPedestalLowGainRatio(),
  fPedestalHighGainRatio(),
  fPeakMinusPedLowGainRatio(),
  fPeakMinusPedHighGainRatio(),
  fDeadMap(),
  fNEvents(ped.GetNEvents()),
  fNChanFills(ped.GetNChanFills()),
  fDeadTowers(ped.GetDeadTowerCount()),
  fNewDeadTowers(ped.GetDeadTowerNew()),
  fResurrectedTowers(ped.GetDeadTowerResurrected()),
  fReference( 0 ), //! note that we do not try to copy the reference info here
  fDetType(ped.GetDetectorType()),
  fColumns(ped.GetColumns()),
  fRows(ped.GetRows()),
  fModules(ped.GetModules()),
  fRowMin(ped.GetRowMin()),
  fRowMax(ped.GetRowMax()),
  fRowMultiplier(ped.GetRowMultiplier()),
  fCaloString(ped.GetCaloString()),
  fMapping(NULL), //! note that we are not copying the map info
  fRunNumber(ped.GetRunNumber()),
  fSelectPedestalSamples(ped.GetSelectPedestalSamples()),
  fFirstPedestalSample(ped.GetFirstPedestalSample()),
  fLastPedestalSample(ped.GetLastPedestalSample())
{
  // Then the ObjArray ones; we add the histograms rather than trying TObjArray = assignment
  //DS: this has not really been tested yet..
  for (int i = 0; i < fModules; i++) {
    fPedestalLowGain.Add( ped.GetPedProfileLowGain(i) );
    fPedestalHighGain.Add( ped.GetPedProfileHighGain(i) );
    fPedestalRMSLowGain.Add( ped.GetPedRMSProfileLowGain(i) );
    fPedestalRMSHighGain.Add( ped.GetPedRMSProfileHighGain(i) );
    fPeakMinusPedLowGain.Add( ped.GetPeakProfileLowGain(i) );
    fPeakMinusPedHighGain.Add( ped.GetPeakProfileHighGain(i) );

    fDeadMap.Add( ped.GetDeadMap(i) );  
  }//end for nModules 
 
  //Compress the arrays, in order to remove the empty objects (a 16 slot array is created by default)
  fPedestalLowGain.Compress();
  fPedestalHighGain.Compress();
  fPedestalRMSLowGain.Compress();
  fPedestalRMSHighGain.Compress();
  fPeakMinusPedLowGain.Compress();
  fPeakMinusPedHighGain.Compress();
  fDeadMap.Compress();
}

// assignment operator; use copy ctor to make life easy..
//_____________________________________________________________________
AliCaloCalibPedestal& AliCaloCalibPedestal::operator = (const AliCaloCalibPedestal &source)
{
  // assignment operator; use copy ctor
  if (&source == this) return *this;

  new (this) AliCaloCalibPedestal(source);
  return *this;
}

//_____________________________________________________________________
void AliCaloCalibPedestal::Reset()
{
  // Reset all arrays/histograms
  for (int i = 0; i < fModules; i++) {
    GetPedProfileLowGain(i)->Reset();
    GetPedProfileHighGain(i)->Reset();
    GetPeakProfileLowGain(i)->Reset();
    GetPeakProfileHighGain(i)->Reset();
    GetDeadMap(i)->Reset();
    
    if (!fPedestalLowGainDiff.IsEmpty()) {
      //This means that the comparison profiles have been created.
  
      GetPedProfileLowGainDiff(i)->Reset();
      GetPedProfileHighGainDiff(i)->Reset();
      GetPeakProfileLowGainDiff(i)->Reset();
      GetPeakProfileHighGainDiff(i)->Reset();
      
      GetPedProfileLowGainRatio(i)->Reset();
      GetPedProfileHighGainRatio(i)->Reset();
      GetPeakProfileLowGainRatio(i)->Reset();
      GetPeakProfileHighGainRatio(i)->Reset();
    }
  }
  fNEvents = 0;
  fNChanFills = 0;
  fDeadTowers = 0;
  fNewDeadTowers = 0;
  fResurrectedTowers = 0;
 
  //To think about: should fReference be deleted too?... let's not do it this time, at least...
}

// Parameter/cut handling
//_____________________________________________________________________
void AliCaloCalibPedestal::SetParametersFromFile(const char *parameterFile)
{
  static const string delimitor("::");
	
  // open, check input file
  ifstream in( parameterFile );
  if( !in ) {
    printf("in AliCaloCalibPedestal::SetParametersFromFile - Using default/run_time parameters.\n");
    return;
  } 

  // Note: this method is a bit more complicated than it really has to be
  // - allowing for multiple entries per line, arbitrary order of the
  // different variables etc. But I wanted to try and do this in as
  // correct a C++ way as I could (as an exercise).

  // read in
  char readline[1024];
  while ((in.rdstate() & ios::failbit) == 0 ) {
    
    // Read into the raw char array and then construct a string
    // to do the searching
    in.getline(readline, 1024);
    istringstream s(readline);		
		
    while ( ( s.rdstate() & ios::failbit ) == 0 ) {
			
      string key_value; 
      s >> key_value;
      
      // check stream status
      if( s.rdstate() & ios::failbit ) break;
			
      // skip rest of line if comments found
      if( key_value.substr( 0, 2 ) == "//" ) break;
			
      // look for "::" in key_value pair
      size_t position = key_value.find( delimitor );
      if( position == string::npos ) {
	printf("wrong format for key::value pair: %s\n", key_value.c_str());
      }
				
      // split key_value pair
      string key( key_value.substr( 0, position ) );
      string value( key_value.substr( position+delimitor.size(), 
				      key_value.size()-delimitor.size() ) );
			
      // check value does not contain a new delimitor
      if( value.find( delimitor ) != string::npos ) {
	printf("wrong format for key::value pair: %s\n", key_value.c_str());
      }
      
      // debug: check key value pair
      // printf("AliCaloCalibPedestal::SetParametersFromFile - key %s value %s\n", key.c_str(), value.c_str());

      // if the key matches with something we expect, we assign the new value
      istringstream iss(value);
      // the comparison strings defined at the beginning of this method
      if ( (key == "fFirstPedestalSample") || (key == "fLastPedestalSample") ) {
	printf("AliCaloCalibPedestal::SetParametersFromFile - key %s value %s\n", key.c_str(), value.c_str());

	if (key == "fFirstPedestalSample") { 
	  iss >> fFirstPedestalSample; 
	}
	else if (key == "fLastPedestalSample") { 
	  iss >> fLastPedestalSample; 
	}
      } // some match

    }		
  }

  in.close();
  return;
	
}

//_____________________________________________________________________
void AliCaloCalibPedestal::WriteParametersToFile(const char *parameterFile)
{
  static const string delimitor("::");
  ofstream out( parameterFile );
  out << "// " << parameterFile << endl;
  out << "fFirstPedestalSample" << "::" << fFirstPedestalSample << endl;
  out << "fLastPedestalSample" << "::" << fLastPedestalSample << endl;

  out.close();
  return;
}

//_____________________________________________________________________
Bool_t AliCaloCalibPedestal::AddInfo(const AliCaloCalibPedestal *ped)
{
  // just do this for the basic histograms/profiles that get filled in ProcessEvent
  // may not have data for all modules, but let's just Add everything..
  for (int i = 0; i < fModules; i++) {
    GetPedProfileLowGain(i)->Add( ped->GetPedProfileLowGain(i) );
    GetPedProfileHighGain(i)->Add( ped->GetPedProfileHighGain(i) );
    GetPeakProfileLowGain(i)->Add( ped->GetPeakProfileLowGain(i) );
    GetPeakProfileHighGain(i)->Add( ped->GetPeakProfileHighGain(i) );
  }//end for nModules 

  // DeadMap; Diff profiles etc would need to be redone after this operation

  return kTRUE;//We succesfully added info from the supplied object
}

//_____________________________________________________________________
Bool_t AliCaloCalibPedestal::ProcessEvent(AliRawReader *rawReader)
{ 
  // if fMapping is NULL the rawstream will crate its own mapping
  AliCaloRawStreamV3 rawStream(rawReader, fCaloString, (AliAltroMapping**)fMapping);
  return ProcessEvent(&rawStream);
}

//_____________________________________________________________________
Bool_t AliCaloCalibPedestal::ProcessEvent(AliCaloRawStreamV3 *in)
{ 
  // Method to process=analyze one event in the data stream
  if (!in) return kFALSE; //Return right away if there's a null pointer
  fNEvents++; // one more event
  
  // indices for the reading
  int sample = 0;
  int gain = 0;
  int time = 0;
  int i = 0; // sample counter
  int startBin = 0;

  // start loop over input stream 
  while (in->NextDDL()) {
    while (in->NextChannel()) {

      // counters
      int max = AliEMCALGeoParams::fgkSampleMin, min = AliEMCALGeoParams::fgkSampleMax; // min and max sample values
      
      // for the pedestal calculation
      int sampleSum = 0; // sum of samples
      int squaredSampleSum = 0; // sum of samples squared
      int nSum = 0; // number of samples in sum
      // calc. quantities
      double mean = 0, squaredMean = 0, rms = 0;
      
      while (in->NextBunch()) {
	const UShort_t *sig = in->GetSignals();
	startBin = in->GetStartTimeBin();
	for (i = 0; i < in->GetBunchLength(); i++) {
	  sample = sig[i];
	  time = startBin--;

	  // check if it's a min or max value
	  if (sample < min) min = sample;
	  if (sample > max) max = sample;
	  
	  // should we add it for the pedestal calculation?
	  if ( (fFirstPedestalSample<=time && time<=fLastPedestalSample) || // sample time in range
	       !fSelectPedestalSamples ) { // or we don't restrict the sample range.. - then we'll take all 
	    sampleSum += sample;
	    squaredSampleSum += sample*sample;
	    nSum++;
	  }
	  
	} // loop over samples in bunch
      } // loop over bunches

      // calculate pedesstal estimate: mean of possibly selected samples
      if (nSum > 0) {
	mean = sampleSum / (1.0 * nSum);
	squaredMean = squaredSampleSum / (1.0 * nSum);
	// The variance (rms squared) is equal to the mean of the squares minus the square of the mean..
	rms = sqrt(squaredMean - mean*mean); 
      }
      else {
	mean = 0;
	squaredMean = 0;
	rms  = 0;
      }
      
      // we're done with the calc. for this channel; let's prepare to fill histo
      gain = -1; // init to not valid value
      if ( in->IsLowGain() ) {
	gain = 0;
      }
      else if ( in->IsHighGain() ) {
	gain = 1;
      }
      
      // it should be enough to check the SuperModule info for each DDL really, but let's keep it here for now
      int arrayPos = in->GetModule(); //The modules are numbered starting from 0
      if (arrayPos >= fModules) {
	//TODO: return an error message, if appopriate (perhaps if debug>0?)
	return kFALSE;
      }     
      //Debug
      if (arrayPos < 0 || arrayPos >= fModules) {
	printf("Oh no: arrayPos = %i.\n", arrayPos); 
      }
      
      fNChanFills++; // one more channel found, and profile to be filled
      //NOTE: coordinates are (column, row) for the profiles
      if (gain == 0) {
	//fill the low gain histograms
	((TProfile2D*)fPeakMinusPedLowGain[arrayPos])->Fill(in->GetColumn(), fRowMultiplier*in->GetRow(), max - min);
	if (nSum>0) { // only fill pedestal info in case it could be calculated
	  ((TProfile2D*)fPedestalLowGain[arrayPos])->Fill(in->GetColumn(), fRowMultiplier*in->GetRow(), mean); 
	  ((TProfile2D*)fPedestalRMSLowGain[arrayPos])->Fill(in->GetColumn(), fRowMultiplier*in->GetRow(), rms);
	}
      } 
      else if (gain == 1) {
      	//fill the high gain ones
	((TProfile2D*)fPeakMinusPedHighGain[arrayPos])->Fill(in->GetColumn(), fRowMultiplier*in->GetRow(), max - min);
	if (nSum>0) { // only fill pedestal info in case it could be calculated
	  ((TProfile2D*)fPedestalHighGain[arrayPos])->Fill(in->GetColumn(), fRowMultiplier*in->GetRow(), mean); 
	  ((TProfile2D*)fPedestalRMSHighGain[arrayPos])->Fill(in->GetColumn(), fRowMultiplier*in->GetRow(), rms);
	}
      }//end if valid gain

    
    }// end while over channel   
  }//end while over DDL's, of input stream 

  in->Reset(); // just in case the next customer forgets to check if the stream was reset..
 
  return kTRUE;
}

//_____________________________________________________________________
Bool_t AliCaloCalibPedestal::SaveHistograms(TString fileName, Bool_t saveEmptyHistos)
{
  //Saves all the histograms (or profiles, to be accurate) to the designated file
  
  TFile destFile(fileName, "recreate");
  
  if (destFile.IsZombie()) {
    return kFALSE;
  }
  
  destFile.cd();
  
  for (int i = 0; i < fModules; i++) {
    if( ((TProfile2D *)fPeakMinusPedLowGain[i])->GetEntries() || saveEmptyHistos) {
      fPeakMinusPedLowGain[i]->Write();
    }
    if( ((TProfile2D *)fPeakMinusPedHighGain[i])->GetEntries() || saveEmptyHistos) { 
      fPeakMinusPedHighGain[i]->Write();
    }
    if( ((TProfile2D *)fPedestalLowGain[i])->GetEntries() || saveEmptyHistos) {
      fPedestalLowGain[i]->Write();
    }
    if( ((TProfile2D *)fPedestalHighGain[i])->GetEntries() || saveEmptyHistos) {
      fPedestalHighGain[i]->Write();
      Printf("save %d", i);
    }
    if( ((TProfile2D *)fPedestalRMSLowGain[i])->GetEntries() || saveEmptyHistos) {
      fPedestalRMSLowGain[i]->Write();
    }
    if( ((TProfile2D *)fPedestalRMSHighGain[i])->GetEntries() || saveEmptyHistos) {
      fPedestalRMSHighGain[i]->Write();
    }
  } 
  
  destFile.Close();
  
  return kTRUE;
}

//_____________________________________________________________________
Bool_t AliCaloCalibPedestal::LoadReferenceCalib(TString fileName, TString objectName)
{
  
  //Make sure that the histograms created when loading the object are not destroyed as the file object is destroyed
  TH1::AddDirectory(kFALSE);
  
  TFile *sourceFile = new TFile(fileName);
  if (sourceFile->IsZombie()) {
    return kFALSE;//We couldn't load the reference
  }

  if (fReference) delete fReference;//Delete the reference object, if it already exists
  fReference = 0;
  
  fReference = (AliCaloCalibPedestal*)sourceFile->Get(objectName);
 
  if (!fReference || !(fReference->InheritsFrom(AliCaloCalibPedestal::Class())) || (fReference->GetDetectorType() != fDetType)) {
    if (fReference) delete fReference;//Delete the object, in case we had an object of the wrong type
    fReference = 0;
    return kFALSE;
  }
	
  delete sourceFile;
 
  //Reset the histogram ownership behaviour. NOTE: a better workaround would be good, since this may accidentally set AddDirectory to true, even
  //if we are called by someone who has set it to false...
  TH1::AddDirectory(kTRUE);
 
  return kTRUE;//We succesfully loaded the object
}

//_____________________________________________________________________
void AliCaloCalibPedestal::ValidateComparisonProfiles()
{
  //Make sure the comparison histos exist
  if (!fPedestalLowGainDiff.IsEmpty()) return; //The profiles already exist. We just check one, because they're all created at
  //the same time
						
						
  //Then, loop for the requested number of modules
  TString title, name;
  for (int i = 0; i < fModules; i++) {
    //Pedestals, low gain
    name = "hPedlowgainDiff";
    name += i;
    title = "Pedestals difference, low gain, module ";
    title += i; 
    fPedestalLowGainDiff.Add(new TProfile2D(name, title,
					    fColumns, 0.0, fColumns, 
					    fRows, fRowMin, fRowMax,"s"));
  
    //Pedestals, high gain
    name = "hPedhighgainDiff";
    name += i;
    title = "Pedestals difference, high gain, module ";
    title += i; 
    fPedestalHighGainDiff.Add(new TProfile2D(name, title,
					     fColumns, 0.0, fColumns, 
					     fRows, fRowMin, fRowMax,"s"));

    //Peak-Pedestals, high gain
    name = "hPeakMinusPedhighgainDiff";
    name += i;
    title = "Peak-Pedestal difference, high gain, module ";
    title += i; 
    fPeakMinusPedHighGainDiff.Add(new TProfile2D(name, title,
						 fColumns, 0.0, fColumns, 
						 fRows, fRowMin, fRowMax,"s"));
  
    //Pedestals, low gain
    name = "hPedlowgainRatio";
    name += i;
    title = "Pedestals ratio, low gain, module ";
    title += i; 
    fPedestalLowGainRatio.Add(new TProfile2D(name, title,
					     fColumns, 0.0, fColumns, 
					     fRows, fRowMin, fRowMax,"s"));
  
    //Pedestals, high gain
    name = "hPedhighgainRatio";
    name += i;
    title = "Pedestals ratio, high gain, module ";
    title += i; 
    fPedestalHighGainRatio.Add(new TProfile2D(name, title,
					      fColumns, 0.0, fColumns, 
					      fRows, fRowMin, fRowMax,"s"));
  
    //Peak-Pedestals, low gain
    name = "hPeakMinusPedlowgainRatio";
    name += i;
    title = "Peak-Pedestal ratio, low gain, module ";
    title += i; 
    fPeakMinusPedLowGainRatio.Add(new TProfile2D(name, title,
						 fColumns, 0.0, fColumns, 
						 fRows, fRowMin, fRowMax,"s"));
  
    //Peak-Pedestals, high gain
    name = "hPeakMinusPedhighgainRatio";
    name += i;
    title = "Peak-Pedestal ratio, high gain, module ";
    title += i; 
    fPeakMinusPedHighGainRatio.Add(new TProfile2D(name, title,
						  fColumns, 0.0, fColumns, 
						  fRows, fRowMin, fRowMax,"s"));
    
  }//end for nModules create the histograms
}

//_____________________________________________________________________
void AliCaloCalibPedestal::ComputeDiffAndRatio()
{
  // calculate differences and ratios relative to a reference
  ValidateComparisonProfiles();//Make sure the comparison histos exist
 
  if (!fReference) {
    return;//Return if the reference object isn't loaded
  }

  for (int i = 0; i < fModules; i++) {
    //Compute the ratio of the histograms
    
    ((TProfile2D*)fPedestalLowGainRatio[i])->Divide(GetPedProfileLowGain(i), fReference->GetPedProfileLowGain(i), 1.0, 1.0);
    ((TProfile2D*)fPedestalHighGainRatio[i])->Divide(GetPedProfileHighGain(i), fReference->GetPedProfileHighGain(i), 1.0, 1.0);
    ((TProfile2D*)fPeakMinusPedLowGainRatio[i])->Divide(GetPeakProfileLowGain(i), fReference->GetPeakProfileLowGain(i), 1.0, 1.0);
    ((TProfile2D*)fPeakMinusPedHighGainRatio[i])->Divide(GetPeakProfileHighGain(i), fReference->GetPeakProfileHighGain(i), 1.0, 1.0);
  
    //For computing the difference, we cannot simply do TProfile2D->Add(), because that subtracts the sum of all entries,
    //which means that the mean of the new profile will not be the difference of the means. So do it by hand:
    for (int j = 0; j <= fColumns; j++) {
      for (int k = 0; k <= fRows; k++) {
	int bin = ((TProfile2D*)fPeakMinusPedHighGainDiff[i])->GetBin(j+1, k+1);//Note that we assume here that all histos have the same structure...
	double diff = fReference->GetPeakProfileHighGain(i)->GetBinContent(bin) - GetPeakProfileHighGain(i)->GetBinContent(bin);
	((TProfile2D*)fPeakMinusPedHighGainDiff[i])->SetBinContent(j+1, k+1, diff);
	((TProfile2D*)fPeakMinusPedHighGainDiff[i])->SetBinEntries(bin, 1);

	diff = fReference->GetPeakProfileLowGain(i)->GetBinContent(bin) - GetPeakProfileLowGain(i)->GetBinContent(bin);
	((TProfile2D*)fPeakMinusPedLowGainDiff[i])->SetBinContent(j+1, k+1, diff);
	((TProfile2D*)fPeakMinusPedLowGainDiff[i])->SetBinEntries(bin, 1);
    
	diff = fReference->GetPedProfileHighGain(i)->GetBinContent(bin) - GetPedProfileHighGain(i)->GetBinContent(bin);
	((TProfile2D*)fPedestalHighGainDiff[i])->SetBinContent(j+1, k+1, diff);
	((TProfile2D*)fPedestalHighGainDiff[i])->SetBinEntries(bin, 1);

	diff = fReference->GetPedProfileLowGain(i)->GetBinContent(bin) - GetPedProfileLowGain(i)->GetBinContent(bin);
	((TProfile2D*)fPedestalLowGainDiff[i])->SetBinContent(j+1, k+1, diff);
	((TProfile2D*)fPedestalLowGainDiff[i])->SetBinEntries(bin, 1);
       
      } // rows
    } // columns
    
  } // modules
 
}

//_____________________________________________________________________
void AliCaloCalibPedestal::ComputeDeadTowers(int threshold, const char * deadMapFile)
{
  //Computes the number of dead towers etc etc into memory, after this you can call the GetDead... -functions
  int countTot = 0;
  int countNew = 0;
  int countRes = 0;
  ofstream * fout = 0;
  ofstream * diff = 0;
  char name[512];//Quite a long temp buffer, just in case the filename includes a path
  
  if (deadMapFile) {
    snprintf(name, 512, "%s.txt", deadMapFile);
    fout = new ofstream(name);
    snprintf(name, 512, "%sdiff.txt", deadMapFile);
    diff = new ofstream(name);
    if (!fout->is_open()) {
      delete fout;
      fout = 0;//Set the pointer to empty if the file was not opened
    }
    if (!diff->is_open()) {
      delete diff;
      fout = 0;//Set the pointer to empty if the file was not opened
    }
  }
 
  for (int i = 0; i < fModules; i++) {
    if (GetPeakProfileHighGain(i)->GetEntries() > 0) { //don't care about empty histos
      for (int j = 1; j <= fColumns; j++) {
	for (int k = 1; k <= fRows; k++) {

	  if (GetPeakProfileHighGain(i)->GetBinContent(j, k) < threshold) {//It's dead
	    countTot++;//One more dead total
	    if (fout) {
	      (*fout) << i << " " 
		      << (fRows - k) << " " 
		      << (j-1) << " " 
		      << "1" << " " 
		      << "0" << endl;//Write the status to the deadmap file, if the file is open.
	    }
	    
	    if (fReference && fReference->GetPeakProfileHighGain(i)->GetBinContent(j, k) >= threshold) {
	      ((TH2D*)fDeadMap[i])->SetBinContent(j, k, kRecentlyDeceased); 
	      countNew++;//This tower wasn't dead before!
	      if (diff) {
		( *diff) << i << " " 
			 << (fRows - k) << " " 
			 << (j - 1) << " " 
			 << "1" << " " 
			 << "0" << endl;//Write the status to the deadmap difference file, if the file is open.
	      }
	    } 
	    else {
	      ((TH2D*)fDeadMap[i])->SetBinContent(j, k, kDead);//This has been dead before. Nothing new		
	    }
	  } 
	  else { //It's ALIVE!!
	    //Don't bother with writing the live ones.
	    //if (fout)
	    //  (*fout) << i << " " 
	    //     << (fRows - k) << " " 
	    //     << (j - 1) << " " 
	    //     << "1" << " " 
	    //     << "1" << endl;//Write the status to the deadmap file, if the file is open.
	    if (fReference && fReference->GetPeakProfileHighGain(i)->GetBinContent(j, k) < threshold) {
	      ((TH2D*)fDeadMap[i])->SetBinContent(j, k, kResurrected);
	      countRes++; //This tower was dead before => it's a miracle! :P
	      if (diff) {
		(*diff) << i << " " 
			<< (fRows - k) << " " 
			<< (j - 1) << " " 
			<< "1" << " " 
			<< "1" << endl;//Write the status to the deadmap difference file, if the file is open.
	      }
	    } 
	    else {
	      ((TH2D*)fDeadMap[i])->SetBinContent(j, k, kAlive);
	    }
	  }
	    
	}//end for k/rows
      }//end for j/columns
    }//end if GetEntries >= 0
  
  }//end for modules
 
 if (fout) {
   fout->close();
   delete fout;
 }
 
 fDeadTowers = countTot;
 fNewDeadTowers = countNew;
 fResurrectedTowers = countRes;
}

