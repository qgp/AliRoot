// $Id$
   
// Author: Constantin Loizides <loizides@ikf.uni-frankfurt.de>
//*-- Copyright &copy ALICE HLT Group


#include "AliL3StandardIncludes.h"

#include "AliL3RootTypes.h"
#include "AliL3Logging.h"
#include "AliL3Logger.h"
#include "AliL3MemHandler.h"
#include "AliL3Transform.h"
#include "AliL3VHDLClusterFinder.h"
#include "AliL3AltroMemHandler.h"


#if __GNUC__ == 3
using namespace std;
#else
#include <stream.h>
#include <libgen.h>
#include <stdlib.h>
#endif


/**
Example program how to run the vhdl clusterfinder.
*/

int main(Int_t argc,Char_t **argv)
{
  Int_t slice=0;
  Int_t patch=0;
  Int_t fm=4;
  Int_t th=10;
  Bool_t de=kFALSE;

  AliL3Logger l;
  l.Set(AliL3Logger::kAll);
  l.UseStderr();
  //l.UseStdout();
  //l.UseStream();

  if(argc<2){
    cout<<"Usage: runvhdlcf altrodatafile [slice] [patch] [matchwidth] [threshold] [deconv]"<<endl;
    return -1;
  }
  if (argc>2) {
    slice=atoi(argv[2]);
  }
  if (argc>3) {
    patch=atoi(argv[3]);
  }
  if (argc>4) {
    fm=atoi(argv[4]);
  }
  if (argc>5) {
    th=atoi(argv[5]);
  }
  if (argc>6) {
    de=kTRUE;
  }

  /*
  //reading transformer config file
  Char_t fname[1024];
  strcpy(fname,argv[1]);
  AliL3Transform::Init(dirname(fname)); 
  strcpy(fname,argv[1]);
  */

  FILE *afile=fopen(argv[1],"r");
  if(!afile) {
    cerr << "Can't open file " << argv[1] << endl;
    exit(1);
  }

  AliL3VHDLClusterFinder cf;
  cf.SetASCIIInput(afile);

  //set cluster finder parameters
  cf.SetMatchWidth(fm);
  cf.SetThreshold(th);
  //cf.SetXYError(0.2);
  //cf.SetZError(0.3);
  cf.SetSTDOutput(kTRUE);
  cf.SetCalcErr(kTRUE);
  cf.SetDeconv(de);
  
  //start processing data
  cf.ProcessDigits();

  if(afile) fclose(afile);
  exit(1);
}





