// @(#) $Id$

/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 *                                                                        *
 * Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *
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

/** @file   AliHLTAgentUtil.cxx
    @author Matthias Richter
    @date   
    @brief  Agent of the libAliHLTUtil library
*/

#include "AliHLTAgentUtil.h"
#include "AliHLTConfiguration.h"

/** global instance for agent registration */
AliHLTAgentUtil gAliHLTAgentUtil;

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTAgentUtil)

AliHLTAgentUtil::AliHLTAgentUtil()
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTAgentUtil::~AliHLTAgentUtil()
{
  // see header file for class documentation
}

int AliHLTAgentUtil::CreateConfigurations(AliHLTConfigurationHandler* handler,
					  AliRunLoader* /*runloader*/) const
{
  // see header file for class documentation
  if (handler) {
  }
  return 0;
}

const char* AliHLTAgentUtil::GetLocalRecConfigurations(AliRunLoader* /*runloader*/) const
{
  // see header file for class documentation
  return NULL;
}

const char* AliHLTAgentUtil::GetRequiredComponentLibraries() const
{
  // see header file for class documentation
  return "libAliHLTUtil.so";
}
