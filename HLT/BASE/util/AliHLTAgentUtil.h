// @(#) $Id$

#ifndef ALIHLTAGENTUTIL_H
#define ALIHLTAGENTUTIL_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/** @file   AliHLTAgentUtil.h
    @author Matthias Richter
    @date   
    @brief  Agent of the libAliHLTUtil library
*/

#include "AliHLTModuleAgent.h"
#include "AliHLTOUTHandler.h"

class AliHLTOUTHandler;

/**
 * @class AliHLTAgentUtil
 * This is the agent for the AliHLTUtil library.
 *
 * @ingroup alihlt_system
 */
class AliHLTAgentUtil : public AliHLTModuleAgent {
 public:
  /**
   * standard constructor. The agent is automatically registered in the
   * global agent manager
   */
  AliHLTAgentUtil();
  /** destructor */
  virtual ~AliHLTAgentUtil();

  /**
   * Register all configurations belonging to this module with the
   * AliHLTConfigurationHandler. The agent can adapt the configurations
   * to be registered to the current AliRoot setup by checking the
   * runloader.
   * @param handler   [in] the configuration handler
   * @param rawReader [in] AliRoot RawReader instance 
   * @param runloader [in] AliRoot runloader
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
   * @param rawReader [in] AliRoot RawReader instance 
   * @param runloader [in] AliRoot runloader
   * @return string containing the top configurations separated by blanks
   */
  const char* GetReconstructionChains(AliRawReader* rawReader=NULL,
				      AliRunLoader* runloader=NULL) const;
  /**
   * Component libraries which the configurations of this agent depend on.
   * @return list of component libraries as a blank-separated string.
   */
  const char* GetRequiredComponentLibraries() const;

  /**
   * Register components for the AliHLTUtil library.
   * @param pHandler  [in] instance of the component handler          
   */
  int RegisterComponents(AliHLTComponentHandler* pHandler) const;

  int GetHandlerDescription(AliHLTComponentDataType dt,
			    AliHLTUInt32_t spec,
			    AliHLTOUTHandlerDesc& desc) const;

  AliHLTOUTHandler* GetOutputHandler(AliHLTComponentDataType dt, AliHLTUInt32_t spec);

  int DeleteOutputHandler(AliHLTOUTHandler* pInstance);

  /**
   * The handler for trigger decision blocks in the HLTOUT stream.
   */
  class AliHLTStreamerInfoHandler : public AliHLTOUTHandler {
  public:
    /** constructor */
    AliHLTStreamerInfoHandler();
    /** destructor */
    ~AliHLTStreamerInfoHandler();

    /// inherited from AliHLTOUTHandler
    /// do nothing for the moment, the streamer info is handled
    /// in AliHLTReconstructor
    int ProcessData(AliHLTOUT* /*pData*/) {return 0;}

  private:
    /** copy constructor forbidden */
    AliHLTStreamerInfoHandler(const AliHLTStreamerInfoHandler&);
    /** assignment operator forbidden */
    AliHLTStreamerInfoHandler& operator=(const AliHLTStreamerInfoHandler&);
  };
 protected:

 private:
  /** copy constructor prohibited */
  AliHLTAgentUtil(const AliHLTAgentUtil&);
  /** assignment operator prohibited */
  AliHLTAgentUtil& operator=(const AliHLTAgentUtil&);

  /** the one and only handler for compstat blocks */
  AliHLTOUTHandler* fCompStatDataHandler; //!transient

  /** dummy handler for streamer info blocks */
  AliHLTOUTHandler* fStreamerInfoDataHandler; //!transient
  
  ClassDef(AliHLTAgentUtil, 1);
};

#endif
