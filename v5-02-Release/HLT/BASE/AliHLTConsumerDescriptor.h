//-*- Mode: C++ -*-
// $Id$

#ifndef ALIHLTCONSUMERDESCRIPTOR_H
#define ALIHLTCONSUMERDESCRIPTOR_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/// @file   AliHLTConsumerDescriptor.h
/// @author Matthias Richter
/// @date   
/// @brief  Helper class to describe a consumer component.
/// @note   The class is used in Offline (AliRoot) context

#include "AliHLTDataBuffer.h"

/**
 * @class AliHLTConsumerDescriptor
 * @brief Helper class to describe a consumer component.
 *
 * There is unfortunately no unique determination of the data type from the
 * component itself possible, thats why both component and data type have to
 * be initialized and are stored in a compound. The class is intended to make
 * bookkeeping easier.
 *
 * @note This class is only used for the @ref alihlt_system.
 *
 * @ingroup alihlt_system
 */
class AliHLTConsumerDescriptor : public TObject, public AliHLTLogging {
 public:
  /** standard constructur */
  AliHLTConsumerDescriptor();
  /** constructur 
   * @param pConsumer pointer to the consumer component
   */
  AliHLTConsumerDescriptor(AliHLTComponent* pConsumer);
  /** destructor */
  ~AliHLTConsumerDescriptor();

  /**
   * Get the component of this descriptor.
   * @return pointer to the component
   */
  AliHLTComponent* GetComponent() {return fpConsumer;}

  /**
   * Set an active data segment.
   * the pointer will be handled in a container, no allocation, copy or
   * cleanup.
   * @param segment segment descriptor
   * @return >=0 if succeeded
   */
  int SetActiveDataSegment(AliHLTDataBuffer::AliHLTDataSegment segment);

  /**
   * Check whether there is an active data segment of certain size with
   * certain offset.
   * @param segment segment descriptor
   * @return > if existend, 0 if not
   */
  int CheckActiveDataSegment(AliHLTDataBuffer::AliHLTDataSegment segment);

  /** find an active data segment of certain size with certain offset
   * will see if this is necessary
   * @param offset  offset of the data segment in the data buffer
   * @param size    size of the data segment in the data buffer
   * @return offset of the data segment
   */
  //AliHLTUInt32_t FindActiveDataSegment(AliHLTUInt32_t offset, AliHLTUInt32_t size);

  /** get the number of active segments for this consumer
   * @return number of active segments
   */
  int GetNofActiveSegments() {return fSegments.size();};

  /**
   * @param segment segment descriptor
   */
  int ReleaseActiveDataSegment(AliHLTDataBuffer::AliHLTDataSegment segment);

  virtual void Print(const char* option) const;

 private:
  /** copy constructor prohibited */
  AliHLTConsumerDescriptor(const AliHLTConsumerDescriptor&);
  /** assignment operator prohibited */
  AliHLTConsumerDescriptor& operator=(const AliHLTConsumerDescriptor&);

  /** consumer object */
  AliHLTComponent* fpConsumer;                                     //! transient

  /** list of data segments */
  vector<AliHLTDataBuffer::AliHLTDataSegment> fSegments;           // see above

  ClassDef(AliHLTConsumerDescriptor, 0)
};

#endif // ALIHLTCONSUMERDESCRIPTOR_H
