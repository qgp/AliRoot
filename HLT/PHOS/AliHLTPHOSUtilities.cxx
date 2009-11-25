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
#include "AliHLTPHOSUtilities.h"
#include <cstdlib>
#include <cstdio>

AliHLTPHOSUtilities::AliHLTPHOSUtilities()
{

}


AliHLTPHOSUtilities::~AliHLTPHOSUtilities()
{

}


bool 
AliHLTPHOSUtilities::ScanSingleIntArgument(int argc, const char** argv, const char *name, int *value)
{
  //  cout << "AliHLTPHOSUtilities::ScanSingleIntArgument " << name <<" =  " << DoExistArgument(argc, argv, name) <<endl;  
  int tmpIndex =  DoExistArgument(argc, argv, name);

  if( tmpIndex  >= 0)
    {
      if(value == 0)
	{
	  return true;
	}
	    
      else
	{
	  if(tmpIndex +1 < argc)
	    {
	      *value =  atoi(argv[tmpIndex +1]);
	      return true;
	    }
	}
    }
 
  else
    {
      return false;
    }
  return false;
}
 
bool 
AliHLTPHOSUtilities::ScanSingleFloatArgument(int argc, const char** argv, const char *name, float *value)
{
  //  cout << "AliHLTPHOSUtilities::ScanSingleFloatArgument " << name <<" =  " << DoExistArgument(argc, argv, name) <<endl; 

  int tmpIndex =  DoExistArgument(argc, argv, name);

  if( tmpIndex  >= 0)
    {
      if(value == 0)
	{
	  return true;
	}
	    
      else
	{
	  if(tmpIndex +1 < argc)
	    {
	      *value =  atof(argv[tmpIndex +1]);
	      return true;
	    }
	}
    }
  else
    {
      return false;
    }
  return false;
}
 

 
bool 
AliHLTPHOSUtilities::ScanSingleNameArgument(int argc, const char** argv, const char *name, char *outname)
{
  //  cout << "AliHLTPHOSUtilities::ScanSingleNameArgument " << name <<" =  " << DoExistArgument(argc, argv, name) <<endl; 

  int tmpIndex =  DoExistArgument(argc, argv, name);

  if( tmpIndex  >= 0)
    {
      if(outname == 0)
	{
	  return true;
	}
	    
      else
	{
	  if(tmpIndex +1 < argc)
	    {
	      //    *value =  atoi(argv[tmpIndex +1]);
	      sprintf(outname, "%s", argv[tmpIndex +1] );

	      return true;
	    }
	}
    }
  else
    {
      return false;
    }
  return false;
}


bool 
AliHLTPHOSUtilities::ScanSingleArgument(int argc, const char** argv, const char *name)
{
  //  cout << "AliHLTPHOSUtilities::ScanSingleArgument " << name <<" =  " << DoExistArgument(argc, argv, name) <<endl; 

  if( DoExistArgument(argc, argv, name) >=0)
    {
      //     cout << "AliHLTPHOSUtilities::ScanSingleArgument " << name <<" > 0" <<endl;
      return true;
    }
  else
    {
      //    cout << "AliHLTPHOSUtilities::ScanSingleArgument " << name <<" > 0" <<endl;
      return false;
    }

}



bool
AliHLTPHOSUtilities::CheckFile(const char *fileName, const char *opt) const
{
  //returns true if the file specified by "fileName exists  and has acceees rights specified  by "opt", 
  //returns false if it doesnt exist, or it exists, but doesnt have the access right specified by "opt"
  FILE *fp = fopen(fileName, opt);

  if(fp == 0)
    {
      return false;
    }
  else
    {
      fclose(fp); 
      return true;
    }
}


// returns the index if the argument if it exists
// returns a negative value if it doesnt exist
int 
AliHLTPHOSUtilities::DoExistArgument(const int argc, const char** argv, const char *name) 
{
  string s1;

  for(int i= 0; i< argc; i++)
    {
      s1.assign(argv[i]);
      
      if(s1 == name)
	{
	  //	  cout << "AliHLTPHOSUtilities::DoExistArgumen , argument = " << name << "  Exists" <<endl;
	  return i;
	}
    }
  
  // cout << "AliHLTPHOSUtilities::DoExistArgumen , argument = " << name << "  does not exist " <<endl;
  return -1;


}
