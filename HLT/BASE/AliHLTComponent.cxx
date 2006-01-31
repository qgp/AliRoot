// $Id$

/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Authors: Matthias Richter <Matthias.Richter@ift.uib.no>                *
 *          Timm Steinbeck <timm@kip.uni-heidelberg.de>                   *
 *          Artur Szostak <artursz@iafrica.com>                           *
 *          for The ALICE Off-line Project.                               *
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
//                                                                           //
// base class for HLT components                                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#if __GNUC__== 3
using namespace std;
#endif

#include "AliHLTComponent.h"
#include "AliHLTComponentHandler.h"
#include <string.h>
#include "AliHLTSystem.h"

ClassImp(AliHLTComponent)

AliHLTComponentHandler* AliHLTComponent::fpComponentHandler=NULL;

AliHLTComponent::AliHLTComponent()
{ 
  memset(&fEnvironment, 0, sizeof(AliHLTComponentEnvironment));
  if (fpComponentHandler)
    fpComponentHandler->ScheduleRegister(this);
}

AliHLTComponent::~AliHLTComponent()
{
}

int AliHLTComponent::Init( AliHLTComponentEnvironment* environ, void* environ_param, int argc, const char** argv )
{
  int iResult=0;
  if (environ) {
    memcpy(&fEnvironment, environ, sizeof(AliHLTComponentEnvironment));
    fEnvironment.fParam=environ_param;
  }
  iResult=DoInit(argc, argv);
  return iResult;
}

int AliHLTComponent::Deinit()
{
  int iResult=0;
  iResult=DoDeinit();
  return iResult;
}

void AliHLTComponent::DataType2Text( const AliHLTComponent_DataType& type, char output[14] ) {
memset( output, 0, 14 );
strncat( output, type.fOrigin, 4 );
strcat( output, ":" );
strncat( output, type.fID, 8 );
}

int AliHLTComponent::MakeOutputDataBlockList( const vector<AliHLTComponent_BlockData>& blocks, AliHLTUInt32_t* blockCount,
					      AliHLTComponent_BlockData** outputBlocks ) {
    if ( !blockCount || !outputBlocks )
	return EFAULT;
    AliHLTUInt32_t count = blocks.size();
    if ( !count )
	{
	*blockCount = 0;
	*outputBlocks = NULL;
	return 0;
	}
    *outputBlocks = reinterpret_cast<AliHLTComponent_BlockData*>( AllocMemory( sizeof(AliHLTComponent_BlockData)*count ) );
    if ( !*outputBlocks )
	return ENOMEM;
    for ( unsigned long i = 0; i < count; i++ )
	(*outputBlocks)[i] = blocks[i];
    *blockCount = count;
    return 0;

}
