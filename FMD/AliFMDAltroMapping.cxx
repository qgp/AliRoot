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
/** @file    AliFMDAltroMapping.cxx
    @author  Christian Holm Christensen <cholm@nbi.dk>
    @date    Sun Mar 26 18:27:56 2006
    @brief   Map HW to detector 
*/
//____________________________________________________________________
//                                                                          
// Mapping of ALTRO hardware channel to detector coordinates 
//
//    The hardware address consist of a DDL number and 12bits of ALTRO
//    addresses.  The ALTRO address are formatted as follows. 
//
//    12              7         4            0
//    |---------------|---------|------------|
//    | Board #       | ALTRO # | Channel #  |
//    +---------------+---------+------------+
//
//
//
#include "AliFMDAltroMapping.h"		// ALIFMDALTROMAPPING_H
#include "AliFMDParameters.h"
#include "AliLog.h"

//____________________________________________________________________
ClassImp(AliFMDAltroMapping)
#if 0
  ; // This is here to keep Emacs for indenting the next line
#endif

//_____________________________________________________________________________
AliFMDAltroMapping::AliFMDAltroMapping()
{
  // Constructor 
}


//_____________________________________________________________________________
Bool_t
AliFMDAltroMapping::ReadMapping()
{
  // Read map from file - not used
  return kTRUE;
}

//_____________________________________________________________________________
void
AliFMDAltroMapping::DeleteMappingArrays()
{
  // Clear map in memory 
}

//____________________________________________________________________
Bool_t 
AliFMDAltroMapping::Hardware2Detector(UInt_t    ddl, UInt_t    addr, 
				      UShort_t& det, Char_t&   ring, 
				      UShort_t& sec, UShort_t& str) const
{
  // Translate a hardware address to detector coordinates. 
  // The detector is simply 
  // 
  //    ddl - kBaseDDL + 1
  // 
  // The ring number, sector, and strip number is given by the addr
  // argument.  The address argument, has the following format 
  // 
  //   12            7          4          0
  //   +-------------+----------+----------+
  //   | Board       | ALTRO    | Channel  |
  //   +-------------+----------+----------+
  // 
  // The board number identifier among other things the ring.  There's
  // up to 4 boards per DDL, and the two first (0 and 1) corresponds
  // to the inner rings, while the two last (2 and 3) corresponds to
  // the outer rings. 
  // 
  // The board number and ALTRO number together identifies the sensor,
  // and hence.  The lower board number (0 or 2) are the first N / 2
  // sensors (where N is the number of sensors in the ring).  
  // 
  // There are 3 ALTRO's per card, and each ALTRO serves up to 4
  // sensors.  Which of sensor is determined by the channel number.
  // For the inner rings, the map is
  // 
  //    ALTRO 0, Channel  0 to  7   -> Sensor 0 or 5
  //    ALTRO 0, Channel  8 to 15   -> Sensor 1 or 6
  //    ALTRO 1, Channel  0 to  7   -> Sensor 2 or 7 
  //    ALTRO 2, Channel  0 to  7   -> Sensor 3 or 8
  //    ALTRO 2, Channel  8 to 15   -> Sensor 4 or 9
  // 
  // For the outer rings, the map is 
  // 
  //    ALTRO 0, Channel  0 to  3   -> Sensor 0 or 10
  //    ALTRO 0, Channel  4 to  7   -> Sensor 1 or 11
  //    ALTRO 0, Channel  8 to 11   -> Sensor 2 or 12
  //    ALTRO 0, Channel 12 to 15   -> Sensor 3 or 13
  //    ALTRO 1, Channel  0 to  3   -> Sensor 4 or 14
  //    ALTRO 1, Channel  4 to  7   -> Sensor 5 or 15
  //    ALTRO 2, Channel  0 to  3   -> Sensor 6 or 16
  //    ALTRO 2, Channel  4 to  7   -> Sensor 7 or 17
  //    ALTRO 2, Channel  8 to 11   -> Sensor 8 or 18
  //    ALTRO 2, Channel 12 to 15   -> Sensor 9 or 19
  // 
  // Which divison of the sensor we're in, depends on the channel
  // number only.  For the inner rings, the map is 
  // 
  //    Channel 0                   -> Sector 0, strips   0-127
  //    Channel 1                   -> Sector 1, strips   0-127
  //    Channel 3                   -> Sector 0, strips 128-255
  //    Channel 4                   -> Sector 1, strips 128-255
  //    Channel 5                   -> Sector 0, strips 256-383
  //    Channel 6                   -> Sector 1, strips 256-383
  //    Channel 7                   -> Sector 0, strips 384-511
  //    Channel 8                   -> Sector 1, strips 384-511 
  // 
  // There are only half as many strips in the outer sensors, so there
  // only 4 channels are used for a full sensor.  The map is 
  // 
  //    Channel 0                   -> Sector 0, strips   0-127
  //    Channel 1                   -> Sector 1, strips   0-127
  //    Channel 3                   -> Sector 0, strips 128-255
  //    Channel 4                   -> Sector 1, strips 128-255
  // 
  // With this information, we can decode the hardware address to give
  // us detector coordinates, unique at least up a 128 strips.  We
  // return the first strip in the given range. 
  //
  det          =  (ddl - AliFMDParameters::kBaseDDL) + 1;
  UInt_t board =  (addr >> 7) & 0x1F;
  UInt_t altro =  (addr >> 4) & 0x7;
  UInt_t chan  =  (addr & 0xf);
  if (board > 3) {
    AliError(Form("Invalid board address %d for the FMD", board));
    return kFALSE;
  }
  if (altro > 2) {
    AliError(Form("Invalid ALTRO address %d for the FMD digitizer %d", 
		  altro, board));
    return kFALSE;
  }
  ring         =  (board > 1 ? 'O' : 'I');
  UInt_t nsen  =  (ring == 'I' ? 10 : 20);
  UInt_t nsa   =  (ring == 'I' ? 2 : 4);   // Sensors per ALTRO
  UInt_t ncs   =  (ring == 'I' ? 8 : 4);   // Channels per sensor 
  UInt_t sen   =  (board % 2) * nsen / 2;  // Base for half-ring
  sen          += chan / ncs + (altro == 0 ? 0   : 
				altro == 1 ? nsa : UInt_t(1.5 * nsa));
  sec          =  2 * sen + (chan % 2);
  str          =  (chan % ncs) / 2 * 128;
  return kTRUE;
}

//____________________________________________________________________
Bool_t 
AliFMDAltroMapping::Detector2Hardware(UShort_t  det, Char_t    ring, 
				      UShort_t  sec, UShort_t  str,
				      UInt_t&   ddl, UInt_t&   addr) const
{
  // Translate detector coordinates to a hardware address.
  // The ddl is simply 
  // 
  //    kBaseDDL + (det - 1)
  // 
  // The ring number, sector, and strip number must be encoded into a
  // hardware address.  The address argument, will have the following
  // format on output
  // 
  //   12            7          4          0
  //   +-------------+----------+----------+
  //   | Board       | ALTRO    | Channel  |
  //   +-------------+----------+----------+
  // 
  // The board number is given by the ring and sector.  The inner
  // rings board 0 and 1, while the outer are 2 and 3.  Which of these
  // depends on the sector.  The map is 
  // 
  //    Ring I, sector  0- 9       ->   board 0 
  //    Ring I, sector 10-19       ->   board 1
  //    Ring O, sector  0-19       ->   board 2 
  //    Ring O, sector 20-39       ->   board 3
  // 
  // There are 3 ALTRO's per board.  The ALTRO number is given by the
  // sector number.  For the inner rings, these are given by
  // 
  //    Sector  0- 3  or  10-13    ->   ALTRO 0 
  //    Sector  4- 5  or  14-15    ->   ALTRO 1 
  //    Sector  6- 9  or  16-19    ->   ALTRO 2 
  // 
  // For the outers, it's given by 
  // 
  //    Sector  0- 7  or  20-27    ->   ALTRO 0 
  //    Sector  8-11  or  28-31    ->   ALTRO 1 
  //    Sector 12-19  or  32-39    ->   ALTRO 2 
  //
  // The channel number is given by the sector and strip number.  For
  // the inners, the map is 
  // 
  //    Sector  0, strips   0-127  ->   Channel  0
  //    Sector  0, strips 128-255  ->   Channel  2
  //    Sector  0, strips 256-383  ->   Channel  4
  //    Sector  0, strips 384-511  ->   Channel  6
  //    Sector  1, strips   0-127  ->   Channel  1
  //    Sector  1, strips 128-255  ->   Channel  3
  //    Sector  1, strips 256-383  ->   Channel  5
  //    Sector  1, strips 384-511  ->   Channel  7
  //    Sector  2, strips   0-127  ->   Channel  8
  //    Sector  2, strips 128-255  ->   Channel 10
  //    Sector  2, strips 256-383  ->   Channel 12
  //    Sector  2, strips 384-511  ->   Channel 14
  //    Sector  3, strips   0-127  ->   Channel  9
  //    Sector  3, strips 128-255  ->   Channel 11
  //    Sector  3, strips 256-383  ->   Channel 13
  //    Sector  3, strips 384-511  ->   Channel 15
  // 
  // and so on, up to sector 19.  For the outer, the map is 
  // 
  //    Sector  0, strips   0-127  ->   Channel  0
  //    Sector  0, strips 128-255  ->   Channel  2
  //    Sector  1, strips   0-127  ->   Channel  1
  //    Sector  1, strips 128-255  ->   Channel  3
  //    Sector  2, strips   0-127  ->   Channel  4
  //    Sector  2, strips 128-255  ->   Channel  6
  //    Sector  3, strips   0-127  ->   Channel  5
  //    Sector  3, strips 128-255  ->   Channel  7
  //    Sector  4, strips   0-127  ->   Channel  8
  //    Sector  4, strips 128-255  ->   Channel 10
  //    Sector  5, strips   0-127  ->   Channel  9
  //    Sector  5, strips 128-255  ->   Channel 11
  //    Sector  6, strips   0-127  ->   Channel 12
  //    Sector  6, strips 128-255  ->   Channel 14
  //    Sector  7, strips   0-127  ->   Channel 13
  //    Sector  7, strips 128-255  ->   Channel 15
  // 
  // and so on upto sector 40. 
  // 
  // With this information, we can decode the detector coordinates to
  // give us a unique hardware address 
  //
  ddl          =  AliFMDParameters::kBaseDDL + (det - 1);
  UInt_t nsen  =  (ring == 'I' ? 10 : 20);
  UInt_t nsa   =  (ring == 'I' ? 2 : 4);   // Sensors per ALTRO
  UInt_t ncs   =  (ring == 'I' ? 8 : 4);   // Channels per sensor 
  UInt_t bbase =  (ring == 'I' ? 0 : 2);
  UInt_t board =  bbase + sec / nsen;
  UInt_t lsec  =  (sec - (board - bbase) * nsen); // Local sec in half-ring
  UInt_t altro =  (lsec < 2 * nsa ? 0 : (lsec < 3 * nsa ? 1       : 2));
  UInt_t sbase =  (altro == 0     ? 0 : altro == 1      ? 2 * nsa : 3 * nsa);
  UInt_t chan  =  (sec % 2) + (lsec-sbase) / 2 * ncs + 2 * (str / 128);
  AliDebug(40, Form("\n"
		    "  chan = (%d %% 2) + (%d-%d) / %d * %d + 2 * %d / 128\n"
		    "       = %d + %d + %d = %d", 
		    sec, lsec, sbase, 2, ncs, str, 
		    (sec % 2), (lsec - sbase) / 2 * ncs, 
		    2 * (str / 128), chan));
  addr         =  chan + (altro << 4) + (board << 7);
  
  return kTRUE;
}

//____________________________________________________________________
Int_t
AliFMDAltroMapping::GetHWAddress(Int_t sec, Int_t str, Int_t ring) const
{
  // Return hardware address corresponding to sector sec, strip str,
  // and ring ring.   Mapping from TPC to FMD coordinates are 
  // 
  //   TPC     | FMD
  //   --------+------
  //   padrow  | sector
  //   pad     | strip
  //   sector  | ring 
  //
  UInt_t ddl, hwaddr;
  Char_t r = Char_t(ring);
  if (!Detector2Hardware(1, r, sec, str, ddl, hwaddr)) 
    return -1;
  return hwaddr;
}

//____________________________________________________________________
Int_t
AliFMDAltroMapping::GetPadRow(Int_t hwaddr) const
{
  // Return sector corresponding to hardware address hwaddr.  Mapping
  // from TPC to FMD coordinates are  
  // 
  //   TPC     | FMD
  //   --------+------
  //   padrow  | sector
  //   pad     | strip
  //   sector  | ring 
  //
  UShort_t det;
  Char_t   ring;
  UShort_t sec;
  UShort_t str;
  Int_t    ddl = AliFMDParameters::kBaseDDL;
  if (!Hardware2Detector(ddl, hwaddr, det, ring, sec, str)) return -1;
  return Int_t(sec);
}

//____________________________________________________________________
Int_t
AliFMDAltroMapping::GetPad(Int_t hwaddr) const
{
  // Return strip corresponding to hardware address hwaddr.  Mapping
  // from TPC to FMD coordinates are  
  // 
  //   TPC     | FMD
  //   --------+------
  //   padrow  | sector
  //   pad     | strip
  //   sector  | ring 
  //
  UShort_t det;
  Char_t   ring;
  UShort_t sec;
  UShort_t str;
  Int_t    ddl = AliFMDParameters::kBaseDDL;
  if (!Hardware2Detector(ddl, hwaddr, det, ring, sec, str)) return -1;
  return Int_t(str);
}
  
//____________________________________________________________________
Int_t
AliFMDAltroMapping::GetSector(Int_t hwaddr) const
{
  // Return ring corresponding to hardware address hwaddr.  Mapping
  // from TPC to FMD coordinates are  
  // 
  //   TPC     | FMD
  //   --------+------
  //   padrow  | sector
  //   pad     | strip
  //   sector  | ring 
  //
  UShort_t det;
  Char_t   ring;
  UShort_t sec;
  UShort_t str;
  Int_t    ddl = AliFMDParameters::kBaseDDL;
  if (!Hardware2Detector(ddl, hwaddr, det, ring, sec, str)) return -1;
  return Int_t(ring);
}

//_____________________________________________________________________________
//
// EOF
//
