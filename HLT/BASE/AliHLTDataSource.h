//-*- Mode: C++ -*-
// @(#) $Id$

#ifndef ALIHLTDATASOURCE_H
#define ALIHLTDATASOURCE_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/** @file   AliHLTDataSource.h
    @author Matthias Richter
    @date   
    @brief  Base class declaration for HLT data source components.
    @note   The class is used in Offline (AliRoot) context
*/

#include "AliHLTComponent.h"

/**
 * @class AliHLTDataSource
 * Base class of HLT data source components.
 * The class provides a common interface for the implementation of HLT data
 * source components. The child class must implement the functions:
 * - @ref DoInit (optional)
 * - @ref DoDeinit (optional)
 * - @ref GetEvent
 * - @ref GetComponentID
 * - @ref GetOutputDataType
 * - @ref GetOutputDataSize
 * - @ref Spawn
 *
 * @ingroup alihlt_component
 */
class AliHLTDataSource : public AliHLTComponent {
 public:
  /** standard constructor */
  AliHLTDataSource();
  /** standard destructor */
  virtual ~AliHLTDataSource();

  /**
   * Event processing function.
   * The method is called by the framework to process one event. After 
   * preparation of data structures. The call is redirected to GetEvent.
   * @return neg. error code if failed
   */
  int DoProcessing( const AliHLTComponentEventData& evtData,
		    const AliHLTComponentBlockData* blocks, 
		    AliHLTComponentTriggerData& trigData,
		    AliHLTUInt8_t* outputPtr, 
		    AliHLTUInt32_t& size,
		    vector<AliHLTComponentBlockData>& outputBlocks,
		    AliHLTComponentEventDoneData*& edd );

  // Information member functions for registration.

  /**
   * Return @ref AliHLTComponent::kSource type as component type.
   * @return component type id
   */
  TComponentType GetComponentType() { return AliHLTComponent::kSource;}

  /**
   * Default implementation for all data sources.
   * There are no input data types.
   */
  void GetInputDataTypes( vector<AliHLTComponentDataType>& list);

 private:

  /**
   * The low-level data processing method for the component.
   * This is the custom processing method and can be overloaded by 
   * the component.
   * @param evtData       event data structure
   * @param trigData	  trigger data structure
   * @param outputPtr	  pointer to target buffer
   * @param size	  <i>input</i>: size of target buffer
   *            	  <i>output</i>:size of produced data
   * @param outputBlocks  list to receive output block descriptors
   * @return neg. error code if failed
   */
  virtual int GetEvent( const AliHLTComponentEventData& evtData,
		AliHLTComponentTriggerData& trigData,
		AliHLTUInt8_t* outputPtr, 
		AliHLTUInt32_t& size,
		vector<AliHLTComponentBlockData>& outputBlocks );

  /**
   * The high-level data processing method.
   * This is the default processing method; the method is called
   * if no low level @ref GetEvent method is overloaded by the component.
   * @param evtData       event data structure
   * @param trigData	  trigger data structure
   * @return neg. error code if failed
   */
  virtual int GetEvent( const AliHLTComponentEventData& evtData, AliHLTComponentTriggerData& trigData);

  ClassDef(AliHLTDataSource, 1)
};
#endif
