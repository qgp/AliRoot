/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Authors: Oystein Djuvsland <oysteind@ift.uib.no>                       *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************
*/

#ifndef ALIHLTPHOSGEOMETRY_H
#define ALIHLTPHOSGEOMETRY_H

#include "AliHLTCaloGeometry.h"

class AliHLTPHOSGeometry : public AliHLTCaloGeometry
  {
     public:

      /** Default constructor */
      AliHLTPHOSGeometry ();   //COMMENT
    
      /** Destructor */
      virtual ~AliHLTPHOSGeometry ();       //COMMENT
      
      /** Get the ALICE global coordinates for a rec point */
      virtual void GetGlobalCoordinates ( AliHLTCaloRecPointDataStruct& recPoint,  AliHLTCaloGlobalCoordinate& globalCoord ); //COMMENT

      /** See base class for class documentation */
      virtual void GetCellAbsId(UInt_t /*module*/, UInt_t /*x*/, UInt_t /*z*/, Int_t& AbsId) const { AbsId = 0; }
    
  };

#endif // ALIHLTPHOSGEOMETRY_H
