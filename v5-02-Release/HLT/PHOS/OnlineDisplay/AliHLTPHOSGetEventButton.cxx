// $Id$

/**************************************************************************
 * This file is property of and copyright by the Experimental Nuclear     *
 * Physics Group, Dep. of Physics                                         *
 * University of Oslo, Norway, 2007                                       *
 *                                                                        * 
 * Author: Per Thomas Hille <perthi@fys.uio.no> for the ALICE HLT Project.*
 * Contributors are mentioned in the code where appropriate.              *
 * Please report bugs to perthi@fys.uio.no                                * 
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

#include "AliHLTPHOSGetEventButton.h"
#include <iostream>
#include "AliHLTPHOSOnlineDisplay.h"

using std::cout;
using std::endl;


AliHLTPHOSGetEventButton::AliHLTPHOSGetEventButton()
{
  printf("\nYou cannot initalize the HetEventButton without parameters\n");
}


AliHLTPHOSGetEventButton::AliHLTPHOSGetEventButton(TGCompositeFrame *gfPtr, char *name, char opt):TGTextButton(gfPtr, name)
{
  fOption = opt;
  onlineDisplayPtr = (AliHLTPHOSOnlineDisplay *)gfPtr;
}


Bool_t
AliHLTPHOSGetEventButton::HandleButton(Event_t* event)
{
  if(event->fType == kButtonPress) 
    {
      switch (fOption)
	{
	case 'e':
	  onlineDisplayPtr->GetNextEvent();
	  cout << "updating Eventdisplay" << endl;
	  break;

	case 'h':
	  onlineDisplayPtr->GetHistogram();
	  cout << "updating Histograms" << endl;
	  break;
	  //	case 'r':
	  //	   onlineDisplayPtr-> GetNextEventRaw();
	   //   cout << "updating RawdataDisplay" << endl;
	}
   //     onlineDisplayPtr->GetNextEvent();
    }
}
