// $Id$

/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 *                                                                        *
 * Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *
 *                  Timm Steinbeck <timm@kip.uni-heidelberg.de>           *
 *                  for The ALICE HLT Project.                            *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/** @file   AliHLT_C_Component_WrapperInterface.cxx
    @author Matthias Richter, Timm Steinbeck
    @date   
    @brief  Pure C interface to the AliRoot HLT component handler
*/

#if __GNUC__>= 3
using namespace std;
#endif

#include "AliHLT_C_Component_WrapperInterface.h"
#include "AliHLTComponentHandler.h"
#include "AliHLTComponent.h"
#include <errno.h>

static AliHLTComponentHandler *gComponentHandler_C = NULL;

int AliHLT_C_Component_InitSystem( AliHLTComponentEnvironment* comenv )
{
  if ( gComponentHandler_C )
    {
      return EINPROGRESS;
    }
  gComponentHandler_C = new AliHLTComponentHandler(comenv);
  if ( !gComponentHandler_C )
    return EFAULT;
  gComponentHandler_C->InitAliLogTrap(gComponentHandler_C);
  gComponentHandler_C->AnnounceVersion();
  return 0;
}

int AliHLT_C_Component_DeinitSystem()
{
  if ( gComponentHandler_C )
    {
      delete gComponentHandler_C;
      gComponentHandler_C = NULL;
    }
  return 0;
}

int AliHLT_C_Component_LoadLibrary( const char* libraryPath )
{
  if ( !gComponentHandler_C )
    return ENXIO;
  return gComponentHandler_C->LoadLibrary( libraryPath );
}

int AliHLT_C_Component_UnloadLibrary( const char* /*libraryPath*/ )
{
  if ( !gComponentHandler_C )
    return ENXIO;
  // Matthias 26.10.2007
  // Unloading of libraries has to be re-worked. It has been commented out here
  // since the libraries will be unloaded at the destruction of the component
  // handler instance anyway. So it has no effect to the operation in PubSub.
  // With the introduction of the dynamic component registration via module
  // agents we run into trouble when cleaning up the samples managed by the
  // component handler. Destruction of the sample objects is done AFTER
  // unloading of the library and thus the destructor is not present any 
  // more.
  //return gComponentHandler_C->UnloadLibrary( libraryPath );
  return 0;
}

int AliHLT_C_CreateComponent( const char* componentType, void* environParam, int argc, const char** argv, AliHLTComponentHandle* handle )
{
  if ( !gComponentHandler_C )
    return ENXIO;
  if ( !handle ) return EINVAL;
  AliHLTComponent* comp=NULL;
  const char* cdbPath = getenv("ALIHLT_HCDBDIR");
  if (!cdbPath) cdbPath = getenv("ALICE_ROOT");
  int ret = gComponentHandler_C->CreateComponent( componentType, environParam, argc, argv, comp, cdbPath);
  *handle = reinterpret_cast<AliHLTComponentHandle>( comp );

  return ret;
}

void AliHLT_C_DestroyComponent( AliHLTComponentHandle handle )
{
  if ( !handle )
    return;
  
  AliHLTComponent* pComp=reinterpret_cast<AliHLTComponent*>( handle );
  pComp->Deinit();
  delete pComp;
}

int AliHLT_C_SetRunDescription(const AliHLTRunDesc* desc, const char* runType)
{
  if (!desc) return -EINVAL;
  if (desc->fStructSize<sizeof(AliHLTUInt32_t)) return -EINVAL;
  if (!gComponentHandler_C) return ENXIO;

  AliHLTRunDesc internalDesc=kAliHLTVoidRunDesc;
  memcpy(&internalDesc, desc, desc->fStructSize<sizeof(internalDesc)?desc->fStructSize:sizeof(internalDesc));
  return gComponentHandler_C->SetRunDescription(&internalDesc, runType);
}

int AliHLT_C_ProcessEvent( AliHLTComponentHandle handle, const AliHLTComponentEventData* evtData, const AliHLTComponentBlockData* blocks, 
                           AliHLTComponentTriggerData* trigData, AliHLTUInt8_t* outputPtr,
                           AliHLTUInt32_t* size, AliHLTUInt32_t* outputBlockCnt, 
                           AliHLTComponentBlockData** outputBlocks,
                           AliHLTComponentEventDoneData** edd )
{
  if ( !handle )
    return ENXIO;
  AliHLTComponent* comp = reinterpret_cast<AliHLTComponent*>( handle );
  return comp->ProcessEvent( *evtData, blocks, *trigData, outputPtr, *size, *outputBlockCnt, *outputBlocks, *edd );
}

int AliHLT_C_GetOutputDataType( AliHLTComponentHandle handle, AliHLTComponentDataType* dataType )
{
  if ( !handle )
    return ENXIO;
  AliHLTComponent* comp = reinterpret_cast<AliHLTComponent*>( handle );
  *dataType = comp->GetOutputDataType();
  return 0;
}

int AliHLT_C_GetOutputSize( AliHLTComponentHandle handle, unsigned long* constBase, double* inputMultiplier )
{
  if ( !handle )
    return ENXIO;
  AliHLTComponent* comp = reinterpret_cast<AliHLTComponent*>( handle );
  comp->GetOutputDataSize( *constBase, *inputMultiplier );
  return 0;
}
