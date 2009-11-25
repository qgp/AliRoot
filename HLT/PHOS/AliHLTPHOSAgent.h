
#ifndef ALIHLTPHOSAGENT_H
#define ALIHLTPHOSAGENT_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/** @file   AliHLTPHOSAgent.h
    @author Oystein Djuvsland
    @date   
    @brief  Agent of the libAliHLTPHOS library
*/

#include "AliHLTModuleAgent.h"
#include "AliHLTOUTHandlerEquId.h"

class AliHLTOUTHandlerChain;

/**
 * @class AliHLTPHOSAgent
 * This is the agent for the AliHLTPHOS library.
 *
 * @ingroup alihlt_system
 */
class AliHLTPHOSAgent : public AliHLTModuleAgent {
 public:
  /**
   * standard constructor. The agent is automatically registered in the
   * global agent manager
   */
  AliHLTPHOSAgent();
  /** destructor */
  virtual ~AliHLTPHOSAgent();

  /**
   * Register all configurations belonging to this module with the
   * AliHLTConfigurationHandler. The agent can adapt the configurations
   * to be registered to the current AliRoot setup by checking the
   * runloader.
   * @param handler      the configuration handler
   * @param rawReader    AliRawReader instance
   * @param runloader    AliRoot runloader
   * @return neg. error code if failed
   */
  int CreateConfigurations(AliHLTConfigurationHandler* handler,
			   AliRawReader* rawReader=NULL,
			   AliRunLoader* runloader=NULL) const;

  /**
   * Get the top configurations belonging to this module.
   * A top configuration describes a processing chain. It can simply be
   * described by the last configuration(s) in the chain. 
   * The agent can adapt the configurations to be registered to the current
   * AliRoot setup by checking the runloader.
   * @param rawReader    AliRawReader instance
   * @param runloader    AliRoot runloader
   * @return number of configurations, neg. error code if failed
   */
  const char* GetReconstructionChains(AliRawReader* rawReader=NULL,
				      AliRunLoader* runloader=NULL) const;

  /**
   * Component libraries which the configurations of this agent depend on.
   * @return list of component libraries as a blank-separated string.
   */
  const char* GetRequiredComponentLibraries() const;

  /**
   * Register components for the AliHLTPHOS library.
   * @param pHandler  [in] instance of the component handler          
   */
  int RegisterComponents(AliHLTComponentHandler* pHandler) const;

  /**
   * Get handler decription for PHOS data in the HLTOUT data stream.
   * @param dt        [in] data type of the block
   * @param spec      [in] specification of the block
   * @param desc      [out] handler description
   * @return 1 if the agent can provide a handler, 0 if not
   */
  int GetHandlerDescription(AliHLTComponentDataType dt,
			    AliHLTUInt32_t spec,
			    AliHLTOUTHandlerDesc& desc) const;

  /**
   * Get specific handler for PHOS data in the HLTOUT data stream.
   * @param dt        [in] data type of the block
   * @param spec      [in] specification of the block
   * @return pointer to handler
   */
  AliHLTOUTHandler* GetOutputHandler(AliHLTComponentDataType dt,
				     AliHLTUInt32_t spec);

  /**
   * Delete an HLTOUT handler.
   * @param pInstance      pointer to handler
   */
  int DeleteOutputHandler(AliHLTOUTHandler* pInstance);

  /**
   * The handler for PHOS RAW data in the HLTOUT stream.
   */
  class AliHLTPHOSRawDataHandler : public AliHLTOUTHandlerEquId {
  public:
    /** constructor */
    AliHLTPHOSRawDataHandler();
    /** destructor */
    ~AliHLTPHOSRawDataHandler();

    /**
     * Process a data block.
     * Decode specification and return equipment id of the data block.
     * The data itsself i untouched.
     * @return equipment id the block should be used for.
     */
    int ProcessData(AliHLTOUT* pData);

  private:

  };

 protected:

 private:
  /** copy constructor prohibited */
  AliHLTPHOSAgent(const AliHLTPHOSAgent&);
  /** assignment operator prohibited */
  AliHLTPHOSAgent& operator=(const AliHLTPHOSAgent&);

  /** handler for PHOS raw data in the HLTOUT stream */
  AliHLTPHOSRawDataHandler* fRawDataHandler; //!transient

  ClassDef(AliHLTPHOSAgent, 1);
};

#endif
