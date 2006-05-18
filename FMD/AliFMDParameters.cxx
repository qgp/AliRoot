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
/** @file    AliFMDParameters.cxx
    @author  Christian Holm Christensen <cholm@nbi.dk>
    @date    Mon Mar 27 12:44:26 2006
    @brief   Manager of FMD parameters     
*/
//____________________________________________________________________
//                                                                          
// Forward Multiplicity Detector based on Silicon wafers. 
//
// This class is a singleton that handles various parameters of
// the FMD detectors.  
// The manager normally serves the parameters from the Conditions
// Database (CDB).  These are retrivied by the member function
// `Init'.  Optionally, the class can serve hard-coded constants, if
// no CDB is available. 
//                                                       
#include "AliLog.h"		   // ALILOG_H
#include "AliFMDParameters.h"	   // ALIFMDPARAMETERS_H
#include "AliFMDGeometry.h"	   // ALIFMDGEOMETRY_H
#include "AliFMDRing.h"	           // ALIFMDRING_H
#include "AliFMDCalibGain.h"       // ALIFMDCALIBGAIN_H
#include "AliFMDCalibPedestal.h"   // ALIFMDCALIBPEDESTAL_H
#include "AliFMDCalibSampleRate.h" // ALIFMDCALIBSAMPLERATE_H
#include "AliFMDCalibStripRange.h" // ALIFMDCALIBSTRIPRANGE_H
#include "AliFMDAltroMapping.h"    // ALIFMDALTROMAPPING_H
#include <AliCDBManager.h>         // ALICDBMANAGER_H
#include <AliCDBEntry.h>           // ALICDBMANAGER_H
#include <Riostream.h>
#include <sstream>
#include <TArrayF.h>
#include <TH2D.h>

//====================================================================
ClassImp(AliFMDParameters)
#if 0
  ; // This is here to keep Emacs for indenting the next line
#endif

//____________________________________________________________________
AliFMDParameters* AliFMDParameters::fgInstance = 0;

//____________________________________________________________________
const char* AliFMDParameters::fgkPulseGain	 = "FMD/Calib/PulseGain";
const char* AliFMDParameters::fgkPedestal	 = "FMD/Calib/Pedestal";
const char* AliFMDParameters::fgkDead	         = "FMD/Calib/Dead";
const char* AliFMDParameters::fgkSampleRate	 = "FMD/Calib/SampleRate";
const char* AliFMDParameters::fgkAltroMap	 = "FMD/Calib/AltroMap";
const char* AliFMDParameters::fgkZeroSuppression = "FMD/Calib/ZeroSuppression";
const char* AliFMDParameters::fgkStripRange	 = "FMD/Calib/StripRange";


//____________________________________________________________________
AliFMDParameters* 
AliFMDParameters::Instance() 
{
  // Get static instance 
  if (!fgInstance) fgInstance = new AliFMDParameters;
  return fgInstance;
}

//____________________________________________________________________
AliFMDParameters::AliFMDParameters() 
  : fIsInit(kFALSE),
    fkSiDeDxMip(1.664), 
    fFixedPulseGain(0), 
    fEdepMip(0),
    fZeroSuppression(0), 
    fSampleRate(0), 
    fPedestal(0), 
    fPulseGain(0), 
    fDeadMap(0), 
    fAltroMap(0), 
    fStripRange(0)
{
  // Default constructor 
  SetVA1MipRange();
  SetAltroChannelSize();
  SetChannelsPerAltro();
  SetZeroSuppression();
  SetSampleRate();
  SetPedestal();
  SetPedestalWidth();
  SetPedestalFactor();
  SetThreshold();
  SetStripRange();
}

//__________________________________________________________________
void
AliFMDParameters::Init()
{
  // Initialize the parameters manager.  We need to get stuff from the
  // CDB here. 
  if (fIsInit) return;
  InitPulseGain();
  InitPedestal();
  InitDeadMap();
  InitSampleRate();
  InitZeroSuppression();
  InitAltroMap();
  fIsInit = kTRUE;
  
}

//__________________________________________________________________
#define DET2IDX(det,ring,sec,str) \
  (det * 10000 + (ring == 'I' ? 0 : 1000) + str)  
  
//__________________________________________________________________
void
AliFMDParameters::Draw(Option_t* option)
{
  TString opt(option);
  enum {
    kPulseGain,       // Path to PulseGain calib object
    kThreshold,       // Path to PulseGain calib object
    kPedestal,        // Path to Pedestal calib object
    kPedestalWidth,   // Path to Pedestal calib object
    kDead,            // Path to Dead calib object
    kSampleRate,      // Path to SampleRate calib object
    kAltroMap,        // Path to AltroMap calib object
    kZeroSuppression, // Path to ZeroSuppression cal object
    kMinStripRange,   // Path to strip range cal object
    kMaxStripRange    // Path to strip range cal object
  } what;
  
    
  if      (opt.Contains("dead", TString::kIgnoreCase)) 
    what = kDead;
  else if (opt.Contains("threshold",TString::kIgnoreCase)) 
    what = kThreshold;
  else if (opt.Contains("gain",TString::kIgnoreCase)) 
    what = kPulseGain;
  else if (opt.Contains("pedestal",TString::kIgnoreCase)) 
    what = kPedestal;
  else if (opt.Contains("noise",TString::kIgnoreCase)) 
    what = kPedestalWidth;
  else if (opt.Contains("zero",TString::kIgnoreCase)) 
    what = kZeroSuppression;
  else if (opt.Contains("rate",TString::kIgnoreCase)) 
    what = kSampleRate;
  else if (opt.Contains("min",TString::kIgnoreCase)) 
    what = kMinStripRange;
  else if (opt.Contains("max",TString::kIgnoreCase)) 
    what = kMaxStripRange;
  else if (opt.Contains("map",TString::kIgnoreCase)) 
    what = kAltroMap;
  else {
    Warning("Draw", "unknown parameter: %s\n\tShould be one of\n\t"
	    "dead, threshold, gain, pedestal, noise, zero, rate, "
	    "min, max, map",  
	    option); 
    return;
  }

  TArrayD xbins(3 * 512 + 2 * 256 + 5);
  Int_t i = 1;
  Bool_t skip = kTRUE;
  for (UShort_t det = 1; det <= 3; det++) {
    UShort_t nRings = (det == 1 ? 1 : 2);
    for (UShort_t iring = 0; iring < nRings; iring++) {
      UShort_t nStrip  = (iring == 0 ? 512 : 256);
      Char_t   ring    = (iring == 0 ? 'I' : 'O');
      for (UShort_t str = 0; str < nStrip; str++) {
	Int_t idx = DET2IDX(det, ring, 0, str);
	if (skip) {
	  xbins[i-1] = idx - .5;
	  skip  = kFALSE;
	}
	xbins[i] = idx + .5;
	i++;
      }
      skip = kTRUE;
      i++;
    }
  }
  TArrayD ybins(41);
  for (Int_t i = 0; i < 41; i++) ybins[i] = Float_t(i - .5);
  TH2D* hist = new TH2D("calib", Form("Calibration %s", option), 
			xbins.fN-1, xbins.fArray,  
			ybins.fN-1, ybins.fArray);

  // hist->Draw("Lego");
  // return;
  
  for (UShort_t det = 1; det <= 3; det++) {
    UShort_t nRings = (det == 1 ? 1 : 2);
    for (UShort_t iring = 0; iring < nRings; iring++) {
      UShort_t nSector = (iring == 0 ?  20 : 40);
      UShort_t nStrip  = (iring == 0 ? 512 : 256);
      Char_t   ring    = (iring == 0 ? 'I' : 'O');
      for (UShort_t sec = 0; sec < nSector; sec++) {
	for (UShort_t str = 0; str < nStrip; str++) {
	  Int_t idx = DET2IDX(det, ring, sec, str);
	  UInt_t ddl, addr;
	  Double_t val = 0;
	  switch (what) {
	  case kPulseGain:       // Path to PulseGain calib object
            val = GetPulseGain(det,ring,sec,str); break;
	  case kThreshold:       // Path to PulseGain calib object
            val = GetThreshold(); break;
	  case kPedestal:        // Path to Pedestal calib object
            val = GetPedestal(det,ring,sec,str); break;
	  case kPedestalWidth:   // Path to Pedestal calib object
            val = GetPedestalWidth(det,ring,sec,str); break;
	  case kDead:            // Path to Dead calib object
            val = IsDead(det,ring,sec,str); break;
	  case kSampleRate:      // Path to SampleRate calib object
            val = GetSampleRate(det,ring,sec,str); break;
	  case kAltroMap:        // Path to AltroMap calib object
	    Detector2Hardware(det,ring,sec,str, ddl, addr); 
            val = addr; break;
	  case kZeroSuppression: // Path to ZeroSuppression cal object
            val = GetZeroSuppression(det,ring,sec,str); break;
	  case kMinStripRange:   // Path to strip range cal object
            val = GetMinStrip(det,ring,sec,str); break;
	  case kMaxStripRange:    // Path to strip range cal object
            val = GetMaxStrip(det,ring,sec,str); break;
	  }
	  hist->Fill(idx,sec,val);
	}
      }
    }
  }
  hist->Draw("lego");
}

//__________________________________________________________________
void
AliFMDParameters::Print(Option_t* option) const
{
  // Print information. 
  // If option contains an 'A' then everything is printed. 
  TString opt(option);
  Bool_t showStrips = opt.Contains("a", TString::kIgnoreCase);
  if (opt.Contains("fmd",TString::kIgnoreCase)) {
    size_t   i   = opt.Index("fmd",TString::kIgnoreCase);
    size_t   j   = opt.Index("]",TString::kIgnoreCase);
    UShort_t det, sec, str;
    Char_t ring, lbrack, rbrack, comma;
    UInt_t ddl, addr;
    Detector2Hardware(det, ring, sec, str, ddl, addr);
    std::stringstream s(opt(i+4, j-i-3).Data());
    s >> det >> ring >> lbrack >> sec >> comma >> str >> rbrack;
    std::cout 
      << "     Strip    |     Pedestal      |    Gain    | ZS thr. | Address\n"
      << "--------------+-------------------+------------+---------+---------" 
      << "\nFMD" << det << ring << "[" << std::setw(2) << sec << "," 
      << std::setw(3) << str << "] | " 
      << std::setw(7) << GetPedestal(det, ring, sec, str) 
      << "+/-" << std::setw(7) 
      << GetPedestalWidth(det, ring, sec, str) 
      << " | " << std::setw(10) 
      << GetPulseGain(det, ring, sec, str) 
      << " | " << std::setw(7) 
      << GetZeroSuppression(det, ring, sec, str) 
      << " | 0x" << std::hex << std::setw(4) 
      << std::setfill('0') << ddl << ",0x" << std::setw(3) 
      << addr << std::dec << std::setfill(' ') << std::endl;
    return;
  }
  for (UShort_t det=1 ; det <= 3; det++) {
    std::cout << "FMD" << det << std::endl;
    Char_t rings[] = { 'I', (det == 1 ? '\0' : 'O'), '\0' };
    for (Char_t* ring = rings; *ring != '\0'; ring++) {
      std::cout << " Ring " << *ring << std::endl;
      UShort_t nSec = ( *ring == 'I' ? 20  :  40 );
      UShort_t nStr = ( *ring == 'I' ? 512 : 256 );
      for (UShort_t sec = 0; sec < nSec; sec++) {
	UShort_t min  = GetMinStrip(det, *ring, sec, 0);
	UShort_t max  = GetMaxStrip(det, *ring, sec, 0);
	UShort_t rate = GetSampleRate(det, *ring, sec, 0);
	std::cout << "  Sector " << std::setw(2) << sec 
		  << "  Strip range: " << std::setw(3) << min << "," 
		  << std::setw(3) << max << "  Rate: " << std::setw(2) 
		  << rate << std::endl;
	if (!showStrips) continue;
	std::cout 
	  << "  Strip |     Pedestal      |    Gain    | ZS thr. | Address\n" 
	  << "--------+-------------------+------------+---------+---------" 
	  << std::endl;
        for (UShort_t str = 0; str < nStr; str++) {
	  std::cout << "    " << std::setw(3) << str << " | ";
	  if (IsDead(det, *ring, sec, str)) {
	    std::cout << "dead" << std::endl;
	    continue;
	  }
	  UInt_t ddl, addr;
	  Detector2Hardware(det, *ring, sec, str, ddl, addr);
	  std::cout << std::setw(7) << GetPedestal(det, *ring, sec, str) 
		    << "+/-" << std::setw(7) 
		    << GetPedestalWidth(det, *ring, sec, str) 
		    << " | " << std::setw(10) 
		    << GetPulseGain(det, *ring, sec, str) 
		    << " | " << std::setw(5) 
		    << GetZeroSuppression(det, *ring, sec, str) 
		    << " | 0x" << std::hex << std::setw(4) 
		    << std::setfill('0') << ddl << ",0x" << std::setw(3) 
		    << addr << std::dec << std::setfill(' ') << std::endl;
        }
      }
    }
  }
}

//__________________________________________________________________
void
AliFMDParameters::SetStripRange(UShort_t min, UShort_t max) 
{
  // Set fixed strip range 
  fFixedMinStrip = min;
  fFixedMaxStrip = max;
}

//__________________________________________________________________
void
AliFMDParameters::InitPulseGain()
{
  // Get pulse gain from CDB or used fixed 
  AliCDBManager* cdb      = AliCDBManager::Instance();
  AliCDBEntry*   gain     = cdb->Get(fgkPulseGain);
  if (!gain) {
    AliWarning(Form("No %s found in CDB, perhaps you need to "
		    "use AliFMDCalibFaker?", fgkPulseGain));
    return;
  }
  
  AliDebug(1, Form("Got gain from CDB"));
  fPulseGain = dynamic_cast<AliFMDCalibGain*>(gain->GetObject());
  if (!fPulseGain) AliWarning("Invalid pulser gain object from CDB");
}
//__________________________________________________________________
void
AliFMDParameters::InitPedestal()
{
  // Initialize the pedestals from CDB 
  AliCDBManager* cdb      = AliCDBManager::Instance();
  AliCDBEntry*   pedestal = cdb->Get(fgkPedestal);
  if (!pedestal) {
    AliWarning(Form("No %s found in CDB, perhaps you need to "
		    "use AliFMDCalibFaker?", fgkPedestal));
    return;
  }
  AliDebug(1, Form("Got pedestal from CDB"));
  fPedestal = dynamic_cast<AliFMDCalibPedestal*>(pedestal->GetObject());
  if (!fPedestal) AliWarning("Invalid pedestal object from CDB");
}

//__________________________________________________________________
void
AliFMDParameters::InitDeadMap()
{
  // Get Dead-channel-map from CDB 
  AliCDBManager* cdb      = AliCDBManager::Instance();
  AliCDBEntry*   deadMap  = cdb->Get(fgkDead);
  if (!deadMap) {
    AliWarning(Form("No %s found in CDB, perhaps you need to "
		    "use AliFMDCalibFaker?", fgkDead));
    return;
  }
  AliDebug(1, Form("Got dead map from CDB"));
  fDeadMap = dynamic_cast<AliFMDCalibDeadMap*>(deadMap->GetObject());
  if (!fDeadMap) AliWarning("Invalid dead map object from CDB");
}

//__________________________________________________________________
void
AliFMDParameters::InitZeroSuppression()
{
  // Get 0-suppression from CDB 
  AliCDBManager* cdb      = AliCDBManager::Instance();
  AliCDBEntry*   zeroSup  = cdb->Get(fgkZeroSuppression);
  if (!zeroSup) {
    AliWarning(Form("No %s found in CDB, perhaps you need to "
		    "use AliFMDCalibFaker?", fgkZeroSuppression));
    return;
  }
  AliDebug(1, Form("Got zero suppression from CDB"));
  fZeroSuppression = 
    dynamic_cast<AliFMDCalibZeroSuppression*>(zeroSup->GetObject());
  if (!fZeroSuppression)AliWarning("Invalid zero suppression object from CDB");
}

//__________________________________________________________________
void
AliFMDParameters::InitSampleRate()
{
  // get Sample rate from CDB
  AliCDBManager* cdb      = AliCDBManager::Instance();
  AliCDBEntry*   sampRat  = cdb->Get(fgkSampleRate);
  if (!sampRat) {
    AliWarning(Form("No %s found in CDB, perhaps you need to "
		    "use AliFMDCalibFaker?", fgkSampleRate));
    return;
  }
  AliDebug(1, Form("Got zero suppression from CDB"));
  fSampleRate = dynamic_cast<AliFMDCalibSampleRate*>(sampRat->GetObject());
  if (!fSampleRate) AliWarning("Invalid zero suppression object from CDB");
}

//__________________________________________________________________
void
AliFMDParameters::InitAltroMap()
{
  // Get hardware mapping from CDB
  AliCDBManager* cdb      = AliCDBManager::Instance();
  AliCDBEntry*   hwMap    = cdb->Get(fgkAltroMap);       
  if (!hwMap) {
    AliWarning(Form("No %s found in CDB, perhaps you need to "
		    "use AliFMDCalibFaker?", fgkAltroMap));
    fAltroMap = new AliFMDAltroMapping;
    return;
  }
  AliDebug(1, Form("Got ALTRO map from CDB"));
  fAltroMap = dynamic_cast<AliFMDAltroMapping*>(hwMap->GetObject());
  if (!fAltroMap) {
    AliWarning("Invalid ALTRO map object from CDB");
    fAltroMap = new AliFMDAltroMapping;
  }
}

//__________________________________________________________________
void
AliFMDParameters::InitStripRange()
{
  // Get strips read-out from CDB
  AliCDBManager* cdb      = AliCDBManager::Instance();
  AliCDBEntry*   range    = cdb->Get(fgkStripRange);
  if (!range) {
    AliWarning(Form("No %s found in CDB, perhaps you need to "
		    "use AliFMDCalibFaker?", fgkStripRange));
    return;
  }
  AliDebug(1, Form("Got strip range from CDB"));
  fStripRange = dynamic_cast<AliFMDCalibStripRange*>(range->GetObject());
  if (!fStripRange) AliWarning("Invalid strip range object from CDB");
}


//__________________________________________________________________
Float_t
AliFMDParameters::GetThreshold() const
{
  // Get threshold from CDB
  if (!fPulseGain) return fFixedThreshold;
  return fPulseGain->Threshold();
}

//__________________________________________________________________
Float_t
AliFMDParameters::GetPulseGain(UShort_t detector, Char_t ring, 
			       UShort_t sector, UShort_t strip) const
{
  // Returns the pulser calibrated gain for strip # strip in sector #
  // sector or ring id ring of detector # detector. 
  // 
  // For simulation, this is normally set to 
  // 
  //       VA1_MIP_Range 
  //    ------------------ * MIP_Energy_Loss
  //    ALTRO_channel_size
  // 
  if (!fPulseGain) { 
    if (fFixedPulseGain <= 0)
      fFixedPulseGain = fVA1MipRange * GetEdepMip() / fAltroChannelSize;
    return fFixedPulseGain;
  }  
  AliDebug(50, Form("pulse gain for FMD%d%c[%2d,%3d]=%f",
		    detector, ring, sector, strip,
		    fPulseGain->Value(detector, ring, sector, strip)));
  return fPulseGain->Value(detector, ring, sector, strip);
}

//__________________________________________________________________
Bool_t
AliFMDParameters::IsDead(UShort_t detector, Char_t ring, 
			 UShort_t sector, UShort_t strip) const
{
  // Check if the channel is dead 
  if (!fDeadMap) return kFALSE;
  AliDebug(50, Form("Dead for FMD%d%c[%2d,%3d]=%s",
		    detector, ring, sector, strip,
		    fDeadMap->operator()(detector, ring, sector, strip) ? 
		    "no" : "yes"));
  return fDeadMap->operator()(detector, ring, sector, strip);
}

//__________________________________________________________________
UShort_t
AliFMDParameters::GetZeroSuppression(UShort_t detector, Char_t ring, 
				     UShort_t sector, UShort_t strip) const
{
  // Get zero suppression threshold 
  if (!fZeroSuppression) return fFixedZeroSuppression;
  // Need to map strip to ALTRO chip. 
  AliDebug(50, Form("zero sup. for FMD%d%c[%2d,%3d]=%f",
		    detector, ring, sector, strip,
		    fZeroSuppression->operator()(detector, ring, 
						 sector, strip)));
  return fZeroSuppression->operator()(detector, ring, sector, strip/128);
}

//__________________________________________________________________
UShort_t
AliFMDParameters::GetSampleRate(UShort_t det, Char_t ring, UShort_t sector, 
				UShort_t str) const
{
  // Get sampl rate 
  if (!fSampleRate) return fFixedSampleRate;
  // Need to map sector to digitizier card. 
  UInt_t ret = fSampleRate->Rate(det, ring, sector, str);
  AliDebug(50, Form("Sample rate for FMD%d%c[%2d,%3d]=%d", 
		    det, ring, sector, str, ret));
  return ret;
}

//__________________________________________________________________
UShort_t
AliFMDParameters::GetMinStrip(UShort_t det, Char_t ring, UShort_t sector, 
			      UShort_t str) const
{
  // Get strip range read out 
  if (!fStripRange) return fFixedMinStrip;
  // Need to map sector to digitizier card. 
  UInt_t ret = fStripRange->Min(det, ring, sector, str);
  AliDebug(50, Form("Min strip # for FMD%d%c[%2d,%3d]=%d", 
		    det, ring, sector, str, ret));
  return ret;
}

//__________________________________________________________________
UShort_t
AliFMDParameters::GetMaxStrip(UShort_t det, Char_t ring, UShort_t sector, 
			      UShort_t str) const
{
  // Get strip range read out 
  if (!fStripRange) return fFixedMaxStrip;
  // Need to map sector to digitizier card. 
  UInt_t ret = fStripRange->Max(det, ring, sector, str);
  AliDebug(50, Form("Max strip # for FMD%d%c[%2d,%3d]=%d", 
		    det, ring, sector, str, ret));
  return ret;
}

//__________________________________________________________________
Float_t
AliFMDParameters::GetPedestal(UShort_t detector, Char_t ring, 
			      UShort_t sector, UShort_t strip) const
{
  // Get the pedesal 
  if (!fPedestal) return fFixedPedestal;
  AliDebug(50, Form("pedestal for FMD%d%c[%2d,%3d]=%f",
		    detector, ring, sector, strip,
		    fPedestal->Value(detector, ring, sector, strip)));
  return fPedestal->Value(detector, ring, sector, strip);
}

//__________________________________________________________________
Float_t
AliFMDParameters::GetPedestalWidth(UShort_t detector, Char_t ring, 
				   UShort_t sector, UShort_t strip) const
{
  // Get the pedesal 
  if (!fPedestal) return fFixedPedestalWidth;
  AliDebug(50, Form("pedetal width for FMD%d%c[%2d,%3d]=%f",
		    detector, ring, sector, strip,
		    fPedestal->Width(detector, ring, sector, strip)));
  return fPedestal->Width(detector, ring, sector, strip);
}
  
//__________________________________________________________________
AliFMDAltroMapping*
AliFMDParameters::GetAltroMap() const
{
  // Get the hardware address to detector index map 
  return fAltroMap;
}


//__________________________________________________________________
Bool_t
AliFMDParameters::Hardware2Detector(UInt_t ddl, UInt_t addr, UShort_t& det,
				    Char_t& ring, UShort_t& sec, 
				    UShort_t& str) const
{
  // Map hardware address to detector index
  if (!fAltroMap) return kFALSE;
  return fAltroMap->Hardware2Detector(ddl, addr, det, ring, sec, str);
}

//__________________________________________________________________
Bool_t
AliFMDParameters::Detector2Hardware(UShort_t det, Char_t ring, UShort_t sec, 
				    UShort_t str, UInt_t& ddl, 
				    UInt_t& addr) const			      
{
  // Map detector index to hardware address
  if (!fAltroMap) return kFALSE;
  return fAltroMap->Detector2Hardware(det, ring, sec, str, ddl, addr);
}


//__________________________________________________________________
Float_t
AliFMDParameters::GetEdepMip() const 
{ 
  // Get energy deposited by a MIP in the silicon sensors
  if (fEdepMip <= 0){
    AliFMDGeometry* fmd = AliFMDGeometry::Instance();
    fEdepMip = (fkSiDeDxMip 
		* fmd->GetRing('I')->GetSiThickness() 
		* fmd->GetSiDensity());
  }
  return fEdepMip;
}


  
  
  
//____________________________________________________________________
//
// EOF
//
