// Author: Filimon Roukoutakis 02/08/2006

/******************************************************************************
  MOOD - Monitor Of On-line Data and Detector Debugger for ALICE Experiment
******************************************************************************/

#ifndef ROOT2DATE_H
#define ROOT2DATE_H

#include "AliRawData.h"
#include "AliRawEvent.h"
#include "AliRawEventHeaderBase.h"
#include "AliRawEquipment.h"
#include "AliRawEquipmentHeader.h"
#include "AliRawData.h"
#include "AliDAQ.h"

#include <Riostream.h>

int Root2Date(AliRawEvent *gdcRootEvent, unsigned char *gdcDateEvent, char *ddlDir);

#endif
