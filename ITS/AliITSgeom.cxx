/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/*
$Log$
Revision 1.18.6.2  2002/06/28 16:46:31  hristov
More protections

Revision 1.18.6.1  2002/05/31 21:42:02  mariana
Fix memory leak

Revision 1.21  2002/10/22 14:45:38  alibrary
Introducing Riostream.h

Revision 1.20  2002/10/14 14:57:00  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.18.8.1  2002/07/24 09:27:50  alibrary
Updating on VirtualMC

Revision 1.19  2002/05/31 21:07:42  mariana
Fix memory leak

Revision 1.18  2001/08/24 21:06:37  nilsen
Added more documentation, fixed up some coding violations, and some
forward declorations.

Revision 1.17  2001/07/27 08:06:48  hristov
Use global gRandom generator (M.Ivanov)

Revision 1.16  2001/02/08 23:57:00  nilsen
Fixed up some informational printouts.

Revision 1.15  2001/02/07 20:23:21  nilsen
Fixed bug with HP and no unget in iostream.h. Now using putback instead.
Other changes and fixes also included.

Revision 1.14  2001/02/03 00:00:29  nilsen
New version of AliITSgeom and related files. Now uses automatic streamers,
set up for new formatted .det file which includes detector information.
Additional smaller modifications are still to come.

Revision 1.11  2000/10/02 16:32:35  barbera
Forward declaration added

Revision 1.4.4.15  2000/10/02 15:52:05  barbera
Forward declaration added

Revision 1.10  2000/09/05 14:25:50  nilsen
Made fixes for HP compiler. All function parameter default values placed
in .h file. Fixed the usual problem with HP compilers and the "for(Int_t i..."
business. Replaced casting (Double_t [3][3]) to (Double_t (*)[3]) for HP.
Lastly removed all "const" before function parameters which were 2 dim. arrays,
because on HP root generates some strange code (?). Thanks Peter for the
changes.

Revision 1.9  2000/08/29 20:19:03  nilsen
Removed dependency on structure AliITSeomS and replaced it with class
AliITSgeomMatrix. Added many new functions with many new arguments. Most
in the form of in line functions for speed.

Revision 1.4.4.6  2000/06/04 16:33:32  Nilsen
A restructured AliITSgeom class. Now used AliITSgeomMatrix.

Revision 1.4.4.5  2000/03/04 23:42:39  Nilsen
Updated the comments/documentations and improved the maintainability of the
code.

Revision 1.4.4.4  2000/03/02 21:27:07  Nilsen
Added two functions, SetByAngles and SetTrans.

Revision 1.4.4.3  2000/01/23 03:09:10  Nilsen
// fixed compiler warnings for new function LtLErrorMatrix(...)

Revision 1.4.4.2  2000/01/19 23:18:20  Nilsen
Added transformations of Error matrix to AliITSgeom and fixed some typos
in AliITS.h and AliITShitIndex.h

Revision 1.4.4.1  2000/01/12 19:03:32  Nilsen
This is the version of the files after the merging done in December 1999.
See the ReadMe110100.txt file for details

Revision 1.4  1999/10/15 07:03:20  fca
Fixed bug in GetModuleId(Int_t index,Int_t &lay,Int_t &lad, Int_t &det) and
a typo in the creator. aliroot need to be rerun to get a fixed geometry.

Revision 1.3  1999/10/04 15:20:12  fca
Correct syntax accepted by g++ but not standard for static members, remove minor warnings

Revision 1.2  1999/09/29 09:24:20  fca
Introduction of the Copyright and cvs Log

*/

///////////////////////////////////////////////////////////////////////
// ITS geometry manipulation routines.                               //
// Created April 15 1999.                                            //
// version: 0.0.0                                                    //
// By: Bjorn S. Nilsen                                               //
// version: 0.0.1                                                    //
// Updated May 27 1999.                                              //
// Added Cylindrical random and global based changes.               //
// Added  function PrintComparison.                                  //
///////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//     The local coordinate system by, default, is show in the following
// figures. Also shown are the ladder numbering scheme.
//Begin_Html
/*
<img src="picts/ITS/AliITSgeomMatrix_L1.gif">
</pre>
<br clear=left>
<font size=+2 color=blue>
<p>This shows the relative geometry differences between the ALICE Global
coordinate system and the local detector coordinate system.
</font>
<pre>

<pre>
<img src="picts/ITS/its1+2_convention_front_5.gif">
</pre>
<br clear=left>
<font size=+2 color=blue>
<p>This shows the front view of the SPDs and the orientation of the local
pixel coordinate system. Note that the inner pixel layer has its y coordinate
in the opposite direction from all of the other layers.
</font>
<pre>

<pre>
<img src="picts/ITS/its3+4_convention_front_5.gif">
</pre>
<br clear=left>
<font size=+2 color=blue>
<p>This shows the front view of the SDDs and the orientation of the local
pixel coordinate system.
</font>
<pre>

<pre>
<img src="picts/ITS/its5+6_convention_front_5.gif">
</pre>
<br clear=left>
<font size=+2 color=blue>
<p>This shows the front view of the SSDs and the orientation of the local
pixel coordinate system.
</font>
<pre>
*/
//End_Html
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// version: 0
// Written by Bjorn S. Nilsen
//
// Data Members:
//
// Int_t fNlayers
//     The number of ITS layers for this geometry. By default this
//  is 6, but can be modified by the creator function if there are
// more layers defined.
//
// Int_t *fNlad
//     A pointer to an array fNlayers long containing the number of 
// ladders for each layer. This array is typically created and filled 
// by the AliITSgeom creator function.
//
// Int_t *fNdet
//     A pointer to an array fNlayers long containing the number of
// active detector volumes for each ladder. This array is typically
// created and filled by the AliITSgeom creator function.
//
// AliITSgeomMatrix *fGm
//     A pointer to an array of AliITSgeomMatrix classes. One element 
// per module (detector) in the ITS. AliITSgeomMatrix basicly contains
// all of the necessary information about the detector and it's coordinate
// transformations.
//
// TObjArray *fShape
//     A pointer to an array of TObjects containing the detailed shape
// information for each type of detector used in the ITS. For example
// I have created AliITSgeomSPD, AliITSgeomSDD, and AliITSgeomSSD as
// example structures, derived from TObjects, to hold the detector
// information. I would recommend that one element in each of these
// structures, that which describes the shape of the active volume,
// be one of the ROOT classes derived from TShape. In this way it would
// be easy to have the display program display the correct active
// ITS volumes. See the example classes AliITSgeomSPD, AliITSgeomSDD,
// and AliITSgeomSSD for a more detailed example.
////////////////////////////////////////////////////////////////////////
#include <Riostream.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <TSystem.h>
#include <TRandom.h>

#include "AliITSgeom.h"
#include "AliITSgeomSPD.h"
#include "AliITSgeomSDD.h"
#include "AliITSgeomSSD.h"

ClassImp(AliITSgeom)

//______________________________________________________________________
AliITSgeom::AliITSgeom(){
    //     The default constructor for the AliITSgeom class. It, by default,
    // sets fNlayers to zero and zeros all pointers.
    // Do not allocate anything zero everything.

    fTrans   = 0; // standard GEANT global/local coordinate system.
    fNlayers = 0;
    fNlad    = 0;
    fNdet    = 0;
    fGm      = 0;
    fShape   = 0;
    strcpy(fVersion,"test");
    return;
}
//______________________________________________________________________
AliITSgeom::AliITSgeom(Int_t itype,Int_t nlayers,Int_t *nlads,Int_t *ndets,
		       Int_t mods){
    //     A simple constructor to set basic geometry class variables
    // Inputs:
    // Int_t itype   the type of transformation kept.
    //               bit 0 => Standard GEANT
    //               bit 1 => ITS tracking
    //               bit 2 => A change in the coordinate system has been made.
    //               others are still to be defined as needed.
    // Int_t nlayers The number of ITS layers also set the size of the arrays
    // Int_t *nlads  an array of the number of ladders for each layer. This
    //               array must be nlayers long.
    // Int_t *ndets  an array of the number of detectors per ladder for each
    //               layer. This array must be nlayers long.
    // Int_t mods    The number of modules. Typicaly the sum of all the 
    //               detectors on every layer and ladder.
    // Outputs:
    // none
    Int_t i;

    fTrans    = itype;
    fNlayers  = nlayers;
    fNlad     = new Int_t[nlayers];
    fNdet     = new Int_t[nlayers];
    for(i=0;i<nlayers;i++){fNlad[i] = nlads[i];fNdet[i] = ndets[i];}
    fNmodules = mods;
    fGm       = new TObjArray(mods,0);
    fShape    = new TObjArray(5); // default value
    for(i=0;i<5;i++) fShape->AddAt(0,i);
    strcpy(fVersion,"test");
    return;
}
//______________________________________________________________________
void AliITSgeom::CreatMatrix(Int_t mod,Int_t lay,Int_t lad,Int_t det,
			     AliITSDetector idet,const Double_t tran[3],
			     const Double_t rot[10]){
    // Given the translation vector tran[3] and the rotation matrix rot[1],
    // this function creates and adds to the TObject Array fGm the
    // AliITSgeomMatrix object.
    // Inputs are:
    // Int_t           mod     The module number. The location in TObjArray
    // Int_t           lay     The layer where this module is
    // Int_t           lad     On which ladder this module is
    // Int_t           det     Which detector on this ladder this module is
    // AliITSDetector idet     The type of detector see AliITSgeom.h
    // Double_t       tran[3]  The translation vector
    // Double_t       rot[10]  The rotation matrix.
    // Outputs are:
    //   none
    // The rot[10] matrix is set up like:
    /*   / rot[0]  rot[1]  rot[2] \
    //  |  rot[3]  rot[4]  rot[5]  |
    //   \ rot[6]  rot[7]  rot[8] /  if(rot[9]!=0) then the Identity matrix
    // is used regardless of the values in rot[0]-rot[8].
    */
    Int_t id[3];
    Double_t r[3][3] = {{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0}};

    if(fGm->At(mod)!=0) delete fGm->At(mod);
    id[0] = lay; id[1] = lad; id[2] = det;
    if(rot[9]!=0.0) { // null rotation
	r[0][0] = rot[0]; r[0][1] = rot[1]; r[0][2] = rot[2];
	r[1][0] = rot[3]; r[1][1] = rot[4]; r[1][2] = rot[5];
	r[2][0] = rot[6]; r[2][1] = rot[7]; r[2][2] = rot[8];
    } // end if
    fGm->AddAt(new AliITSgeomMatrix(idet,id,r,tran),mod);
}
//______________________________________________________________________
AliITSgeom::~AliITSgeom(){
    //     The destructor for the AliITSgeom class. If the arrays fNlad,
    // fNdet, or fGm have had memory allocated to them, there pointer values
    // are non zero, then this memory space is freed and they are set
    // to zero. In addition, fNlayers is set to zero. The destruction of
    // TObjArray fShape is, by default, handled by the TObjArray destructor.

   if(fGm!=0){
     //for(Int_t i=0;i<fNlayers;i++) delete fGm->At(i);
      fGm->Delete();
      delete fGm;
   } // end if fGm!=0
   if(fNlad!=0) delete[] fNlad;
   if(fNdet!=0) delete[] fNdet;
   fNlayers = 0;
   fNlad    = 0;
   fNdet    = 0;
   fGm      = 0;
   return;
}
//______________________________________________________________________
void AliITSgeom::ReadNewFile(const char *filename){
    // It is generaly preferred to define the geometry in AliITSgeom
    // directly from the GEANT geometry, see AliITSvPPRasymm.cxx for
    // and example. Under some circumstances this may not be possible.
    // This function will read in a formatted file for all of the
    // information needed to define the geometry in AliITSgeom.
    // Unlike the older file format, this file may contain comments
    // and the order of the data does not need to be completely
    // respected. A file can be created using the function WriteNewFile
    // defined below.
    // Inputs are:
    // const char *filename The file name of the file to be read in.
    // Outputs are:
    //  none
    Int_t ncmd=9;
    const char *cmda[]={"Version"        ,"fTrans"  ,"fNmodules",
	                "fNlayers"       ,"fNladers","fNdetectors",
	                "fNDetectorTypes","fShape"  ,"Matrix"};
    Int_t i,j,lNdetTypes,ldet;
    char cmd[20],c;
    AliITSgeomSPD *spd;
    AliITSgeomSDD *sdd;
    AliITSgeomSSD *ssd;
    AliITSgeomMatrix *m;
    ifstream *fp;
    char *filtmp;

    filtmp = gSystem->ExpandPathName(filename);
    cout << "AliITSgeom, Reading New .det file " << filtmp << endl;
    fp = new ifstream(filtmp,ios::in);  // open file to write
    while(fp->get(c)!=NULL){ // for ever loop
	if(c==' ') continue; // remove blanks
	if(c=='\n') continue;
	if(c=='#' || c=='!'){for(;fp->get(c)!=NULL,c!='\n';); continue;}
	if(c=='/'){
	    fp->get(c);{
		if(c=='/'){for(;fp->get(c)!=NULL,c!='\n';);continue;}
		if(c=='*'){
		NotYet:
		    for(;fp->get(c)!=NULL,c!='*';);
		    fp->get(c);{
			if(c=='/') continue;
			goto NotYet;
		    } //
		} // end if c=='*'
	    } // end if second /
	} // end if first /
	fp->putback(c);
	*fp >> cmd;
	for(i=0;i<ncmd;i++) if(strcmp(cmd,cmda[i])==0) break;
	switch (i){
	case 0:   // Version
	    *fp >> fVersion;
	    break;
	case 1:  // fTrans
	    *fp >> fTrans;
	    break;
	case 2:  // fNModules
	    *fp >> fNmodules;
	    if(fGm!=0){
		for(j=0;j<fGm->GetEntriesFast();j++) delete fGm->At(j);
		delete fGm;
	    } // end if
	    fGm = new TObjArray(fNmodules,0);
	    break;
	case 3:  // fNlayers
	    *fp >> fNlayers;
	    if(fNlad!=0) delete fNlad;
	    if(fNdet!=0) delete fNdet;
	    fNlad = new Int_t[fNlayers];
	    fNdet = new Int_t[fNlayers];
	    break;
	case 4:  // fNladers
	    for(j=0;j<fNlayers;j++) *fp >> fNlad[j];
	    break;
	case 5:  // fNdetectors
	    for(j=0;j<fNlayers;j++) *fp >> fNdet[j];
	    break;
	case 6:  // fNDetectorTypes
	    *fp >> lNdetTypes;
	    if(fShape!=0){
		for(j=0;j<fShape->GetEntriesFast();j++) delete fShape->At(j);
		delete fShape;
	    } // end if
	    fShape = new TObjArray(lNdetTypes,0);
	    break;
	case 7:  // fShape
	    *fp >> ldet;
	    if(fShape==0) fShape = new TObjArray(5,0);
	    switch (ldet){
	    case kSPD :
		ReSetShape(ldet,(TObject*) new AliITSgeomSPD());
		spd = (AliITSgeomSPD*) (fShape->At(ldet));
		*fp >> *spd;
		spd = 0;
		break;
	    case kSDD :
		ReSetShape(ldet,(TObject*) new AliITSgeomSDD());
		sdd = (AliITSgeomSDD*) (fShape->At(ldet));
		*fp >> *sdd;
		sdd = 0;
		break;
	    case kSSD : case kSSDp :
		ReSetShape(ldet,(TObject*) new AliITSgeomSSD());
		ssd = (AliITSgeomSSD*) (fShape->At(ldet));
		*fp >> *ssd;
		ssd = 0;
		break;
	    default:
		Error("ReadNewFile","Unknown fShape type number=%d c=%c",ldet,c);
		for(;fp->get(c)==NULL,c!='\n';); // skip to end of line.
		break;
	    } // end switch
	    break;
	case 8:  // Matrix
	    *fp >> ldet;
	    if(fGm==0) fGm = new TObjArray(2270,0);
	    if(fGm->At(ldet)!=0) delete (fGm->At(ldet));
	    fGm->AddAt((TObject*)new AliITSgeomMatrix(),ldet);
	    m = (AliITSgeomMatrix*) fGm->At(ldet);
	    *fp >> *m;
	    m = 0;
	    break;
	default:
	    Error("ReadNewFile","Data line i=%d c=%c",i,c);
	    for(;fp->get(c)==NULL,c!='\n';); // skip this line
	    break;
	} // end switch i
    } // end while
    delete fp;

    return;
}
//______________________________________________________________________
void AliITSgeom::WriteNewFile(const char *filename){
    // Writes AliITSgeom, AliITSgeomMatrix, and the defined AliITSgeomS*D
    // classes to a file in a format that is more readable and commendable.
    // Inputs are:
    // const char *filename The file name of the file to be write to.
    // Outputs are:
    //  none
    ofstream *fp;
    Int_t i;
    char *filtmp;

    filtmp = gSystem->ExpandPathName(filename);
    cout << "AliITSgeom, Writing New .det file " << filtmp << endl;
    fp = new ofstream(filtmp,ios::out);  // open file to write
    *fp << "//Comment lines begin with two //, one #, or one !" << endl;
    *fp << "#Blank lines are skipped including /* and */ sections." << endl;
    *fp << "!and, in principle the order of the lines is not important" <<endl;
    *fp << "/* In AliITSgeom.h are defined an enumerated type called" << endl;
    *fp << " AliITSDetectors These are kSPD=" << (Int_t) kSPD ;
    *fp << ", kSDD=" << (Int_t) kSDD << ", kSSD=" << (Int_t) kSSD;
    *fp << ", kSSDp=" << (Int_t) kSSDp << ", and kSDDp=" << (Int_t) kSDDp;
    *fp << "*/" << endl;
    *fp << "Version " << fVersion << endl;//This should be consistent with the
                                           // geometry version.
    *fp << "fTrans " << fTrans << endl;
    *fp << "fNmodules " << fNmodules << endl;
    *fp << "fNlayers " << fNlayers << endl;
    *fp << "fNladers ";
    for(i=0;i<fNlayers;i++) *fp << fNlad[i] << " ";
    *fp << endl;
    *fp << "fNdetectors ";
    for(i=0;i<fNlayers;i++) *fp << fNdet[i] << " ";
    *fp << endl;
    *fp << "fNDetectorTypes " << fShape->GetEntriesFast() << endl;
    for(i=0;i<fShape->GetEntriesFast();i++){
	if(!IsShapeDefined(i)) continue; // only print out used shapes.
	switch (i){
	case kSPD :
	    *fp << "fShape " << (Int_t) kSPD << " ";
	    *fp << *((AliITSgeomSPD*)(fShape->At(i)));
	    break;
	case kSDD :
	    *fp << "fShape " << (Int_t) kSDD << " ";
	    *fp << *((AliITSgeomSDD*)(fShape->At(i)));
	    break;
	case kSSD : case kSSDp :
	    *fp << "fShape " << i << " ";
	    *fp << *((AliITSgeomSSD*)(fShape->At(i)));
	    break;
	default:
	    Error("AliITSgeom::WriteNewFile","Unknown Shape value");
	} // end switch (i)
    } // end for i
    for(i=0;i<fNmodules;i++){
	*fp << "Matrix " << i << " ";
	*fp << *GetGeomMatrix(i);
    } // end for i
    *fp << "//End of File" << endl;;

    delete fp;
    return;
}
//______________________________________________________________________
AliITSgeom::AliITSgeom(const char *filename){
    //     The constructor for the AliITSgeom class. All of the data to fill
    // this structure is read in from the file given my the input filename.
    // Inputs are:
    // const char *filename The file name of the file to be read in.
    // Outputs are:
    //  none
    FILE     *pf=0;
    Int_t    i,lm=0,id[3];
    Int_t    l,a,d;
    Float_t  x,y,z,o,p,q,r,s,t;
    Double_t rot6[6],tran[3];
    char     buf[200],*buff=0; // input character buffer;
    char *filtmp;

    filtmp = gSystem->ExpandPathName(filename);
    cout << "AliITSgeom reading old .det file " << filtmp << endl;
    fShape = 0;
    strcpy(fVersion,"DefauleV5");
    pf = fopen(filtmp,"r");

    fNlayers = 6; // set default number of ladders
 TryAgain:
    fNlad    = new Int_t[fNlayers];
    fNdet    = new Int_t[fNlayers];
    fNmodules = 0;
    // find the number of ladders and detectors in this geometry.
    for(i=0;i<fNlayers;i++){fNlad[i]=fNdet[i]=0;} // zero out arrays
    while(fgets(buf,200,pf)!=NULL){ // for ever loop
	for(i=0;i<200;i++)if(buf[i]!=' '){ // remove blank spaces.
	    buff = &(buf[i]);
	    break;
	} // end for i
	// remove blank lines and comments.
	if(buff[0]=='\n'||buff[0]=='#'||buff[0]=='!'||
	   (buff[0]=='/'&&buff[1]=='/')) continue;
	if(isalpha(buff[0])) { // must be the new file formated file.
            fclose(pf);
            delete[] fNlad;delete[] fNdet;
            ReadNewFile(filename);
            return;
	} // end if isalpha(buff[0])
	sscanf(buff,"%d %d %d %f %f %f %f %f %f %f %f %f",
	       &l,&a,&d,&x,&y,&z,&o,&p,&q,&r,&s,&t);
	if(l>lm) lm = l;
	if(l<1 || l>fNlayers) {
	    printf("error in file %s layer=%d min. is 1 max is %d\n",
		   filename,l,fNlayers);
	    continue;
	}// end if l
	fNmodules++;
	if(l<=fNlayers&&fNlad[l-1]<a) fNlad[l-1] = a;
	if(l<=fNlayers&&fNdet[l-1]<d) fNdet[l-1] = d;
    } // end while ever loop
    if(lm>fNlayers){
	delete[] fNlad;
	delete[] fNdet;
	fNlayers = lm;
	goto TryAgain;
    } // end if lm>fNlayers
    // counted the number of ladders and detectors now allocate space.
    fGm = new TObjArray(fNmodules,0);

    // Set up Shapes for a default configuration of 6 layers.
    fTrans   = 0; // standard GEANT global/local coordinate system.
    // prepare to read in transforms
    lm = 0; // reuse lm as counter of modules.
    rewind(pf); // start over reading file
    while(fgets(buf,200,pf)!=NULL){ // for ever loop
	for(i=0;i<200;i++)if(buf[i]!=' '){ // remove blank spaces.
	    buff = &(buf[i]);
	    break;
	} // end for i
	// remove blank lines and comments.
	if(buff[0]=='\n'||buff[0]=='#'||buff[0]=='!'||
	   (buff[0]=='/'&&buff[1]=='/')) continue;
	x = y = z = o = p = q = r = s = t = 0.0;
	sscanf(buff,"%d %d %d %f %f %f %f %f %f %f %f %f",
	       &l,&a,&d,&x,&y,&z,&o,&p,&q,&r,&s,&t);
	if(l<1 || l>fNlayers) {
	    printf("error in file %s layer=%d min. is 1 max is %d/n",
		   filename,l,fNlayers);
	    continue;
	}// end if l
	id[0] = l;id[1] = a;id[2] = d;
	tran[0] = tran[1] = tran[2]  = 0.0;
	tran[0] = (Double_t)x;tran[1] = (Double_t)y;tran[2] = (Double_t)z;
	rot6[0] = rot6[1] = rot6[2] = rot6[3] = rot6[4] = rot6[5] =0.0;
	rot6[0] = (Double_t)o;rot6[1] = (Double_t)p;rot6[2] = (Double_t)q;
	rot6[3] = (Double_t)r;rot6[4] = (Double_t)s;rot6[5] = (Double_t)t;
	switch (l){
	case 1: case 2: // layer 1 or2 SPD
	    fGm->AddAt(new AliITSgeomMatrix(rot6,kSPD,id,tran),lm++);
	    break;
	case 3: case 4: // layer 3 or 4 SDD
	    fGm->AddAt(new AliITSgeomMatrix(rot6,kSDD,id,tran),lm++);
	    break;
	case 5: case 6: // layer 5 or 6 SSD
	    fGm->AddAt(new AliITSgeomMatrix(rot6,kSSD,id,tran),lm++);
	    break;
	} // end switch
    } // end while ever loop
    fclose(pf);
}
//______________________________________________________________________
AliITSgeom::AliITSgeom(AliITSgeom &source){
    //     The copy constructor for the AliITSgeom class. It calls the
    // = operator function. See the = operator function for more details.
    // Inputs are:
    // AliITSgeom &source  The AliITSgeom class with which to make this
    //                     a copy of.
    // Outputs are:
    // none.

    *this = source;  // Just use the = operator for now.
    return;
}
//______________________________________________________________________
AliITSgeom& AliITSgeom::operator=(AliITSgeom &source){
    //     The = operator function for the AliITSgeom class. It makes an
    // independent copy of the class in such a way that any changes made
    // to the copied class will not affect the source class in any way.
    // This is required for many ITS alignment studies where the copied
    // class is then modified by introducing some misalignment.
    // Inputs are:
    // AliITSgeom &source  The AliITSgeom class with which to make this
    //                     a copy of.
    // Outputs are:
    // return  *this       The a new copy of source.
   Int_t i;

   if(this == &source) return *this; // don't assign to ones self.

   // if there is an old structure allocated delete it first.
   if(this->fGm != 0){
      for(i=0;i<this->fNmodules;i++) delete this->fGm->At(i);
      delete this->fGm;
   } // end if fGm != 0 
   if(fNlad != 0) delete[] fNlad;
   if(fNdet != 0) delete[] fNdet;

   this->fTrans    = source.fTrans;
   this->fNmodules = source.fNmodules;
   this->fNlayers = source.fNlayers;
   this->fNlad = new Int_t[fNlayers];
   for(i=0;i<this->fNlayers;i++) this->fNlad[i] = source.fNlad[i];
   this->fNdet = new Int_t[fNlayers];
   for(i=0;i<this->fNlayers;i++) this->fNdet[i] = source.fNdet[i];
   this->fShape = new TObjArray(*(source.fShape));//This does not make a proper copy.
   this->fGm = new TObjArray(this->fNmodules,0);
   for(i=0;i<this->fNmodules;i++){
       this->fGm->AddAt(new AliITSgeomMatrix(*(
	   (AliITSgeomMatrix*)(source.fGm->At(i)))),i);
   } // end for i
   return *this;
}
//______________________________________________________________________
Int_t AliITSgeom::GetModuleIndex(Int_t lay,Int_t lad,Int_t det){
    //      This routine computes the module index number from the layer,
    // ladder, and detector numbers. The number of ladders and detectors
    // per layer is determined when this geometry package is constructed,
    // see AliITSgeom(const char *filename) for specifics.
    // Inputs are:
    // Int_t lay  The layer number. Starting from 1.
    // Int_t lad  The ladder number. Starting from 1.
    // Int_t det  The detector number. Starting from 1.
    // Outputs are:
    // return the module index number, starting from zero.
    Int_t i,j,k,id[3];

    i = fNdet[lay-1] * (lad-1) + det - 1;
    j = 0;
    for(k=0;k<lay-1;k++) j += fNdet[k]*fNlad[k];
    i = i+j;
    GetGeomMatrix(i)->GetIndex(id);
    if(id[0]==lay&&id[1]==lad&&id[2]==det) return i;
    // Array of modules fGm is not in expected order. Search for this index
    for(i=0;i<fNmodules;i++){
	GetGeomMatrix(i)->GetIndex(id);
	if(id[0]==lay&&id[1]==lad&&id[2]==det) return i;
    } // end for i
    // This layer ladder and detector combination does not exist return -1.
    return -1;
}
//______________________________________________________________________
void AliITSgeom::GetModuleId(Int_t index,Int_t &lay,Int_t &lad,Int_t &det){
    //      This routine computes the layer, ladder and detector number 
    // given the module index number. The number of ladders and detectors
    // per layer is determined when this geometry package is constructed,
    // see AliITSgeom(const char *filename) for specifics.
    // Inputs are:
    // Int_t index  The module index number, starting from zero.
    // Outputs are:
    // Int_t lay    The layer number. Starting from 1.
    // Int_t lad    The ladder number. Starting from 1.
    // Int_t det    The detector number. Starting from 1.
    Int_t id[3];
    AliITSgeomMatrix *g = GetGeomMatrix(index);
    if (g == 0x0)
     {
      Error("GetModuleId","Can not get GeoMatrix for index = %d",index);
      lay = -1; lad = -1; det = -1;
     }
    else
     {
      g->GetIndex(id);
      lay = id[0]; lad = id[1]; det = id[2];
     }
    return;

    // The old way kept for posterity.
/*
    Int_t i,j,k;
    j = 0;
    for(k=0;k<fNlayers;k++){
	j += fNdet[k]*fNlad[k];
	if(j>index)break;
    } // end for k
    lay = k+1;
    i = index -j + fNdet[k]*fNlad[k];
    j = 0;
    for(k=0;k<fNlad[lay-1];k++){
	j += fNdet[lay-1];
	if(j>i)break;
    } // end for k
    lad = k+1;
    det = 1+i-fNdet[lay-1]*k;
    return;
*/
}
//______________________________________________________________________
Int_t AliITSgeom::GetStartDet(Int_t dtype){
    // returns the starting module index value for a give type of detector id.
    // This assumes that the detector types are different on different layers
    // and that they are not mixed up.
    // Inputs are:
    // Int_t dtype A detector type number. 0 for SPD, 1 for SDD, and 2 for SSD.
    // outputs:
    // return the module index for the first occurance of that detector type.

    switch(dtype){
    case 0:
	return GetModuleIndex(1,1,1);
	break;
    case 1:
	return GetModuleIndex(3,1,1);
	break;
    case 2:
	return GetModuleIndex(5,1,1);
	break;
    default:
	Warning("GetStartDet","undefined detector type %d",dtype);
	return 0;
    } // end switch

    Warning("GetStartDet","undefined detector type %d",dtype);
    return 0;
}
//______________________________________________________________________
Int_t AliITSgeom::GetLastDet(Int_t dtype){
    // returns the last module index value for a give type of detector id.
    // This assumes that the detector types are different on different layers
    // and that they are not mixed up.
    // Inputs are:
    // Int_t dtype A detector type number. 0 for SPD, 1 for SDD, and 2 for SSD.
    // outputs are:
    // return the module index for the last occurance of that detector type.

    switch(dtype){
    case 0:
	return GetLastSPD();
	break;
    case 1:
	return GetLastSDD();
	break;
    case 2:
	return GetLastSSD();
	break;
    default:
	Warning("GetLastDet","undefined detector type %d",dtype);
	return 0;
    } // end switch

    Warning("GetLastDet","undefined detector type %d",dtype);
    return 0;
}
//______________________________________________________________________
void AliITSgeom::PrintComparison(FILE *fp,AliITSgeom *other){
    //     This function was primarily created for diagnostic reasons. It
    // print to a file pointed to by the file pointer fp the difference
    // between two AliITSgeom classes. The format of the file is basicly,
    // define d? to be the difference between the same element of the two
    // classes. For example dfrx = this->GetGeomMatrix(i)->frx 
    // - other->GetGeomMatrix(i)->frx.
    // if(at least one of dfx0, dfy0, dfz0,dfrx,dfry,dfrz are non zero) then
    // print layer ladder detector dfx0 dfy0 dfz0 dfrx dfry dfrz
    // if(at least one of the 9 elements of dfr[] are non zero) then print
    // layer ladder detector dfr[0] dfr[1] dfr[2]
    //                       dfr[3] dfr[4] dfr[5]
    //                       dfr[6] dfr[7] dfr[8]
    // Only non zero values are printed to save space. The differences are
    // typical written to a file because there are usually a lot of numbers
    // printed out and it is usually easier to read them in some nice editor
    // rather than zooming quickly past you on a screen. fprintf is used to
    // do the printing. The fShapeIndex difference is not printed at this time.
    // Inputs are:
    // FILE *fp           A file pointer to an opened file for writing in which
    //                    the results of the comparison will be written.
    // AliITSgeom *other  The other AliITSgeom class to which this one is
    //                    being compared.
    // outputs are:
    // none
    Int_t    i,j,idt[3],ido[3];
    Double_t tt[3],to[3];  // translation
    Double_t rt[3],ro[3];  // phi in radians
    Double_t mt[3][3],mo[3][3]; // matrixes
    AliITSgeomMatrix *gt,*go;
    Bool_t   t;

    for(i=0;i<this->fNmodules;i++){
	gt  =  this->GetGeomMatrix(i);
	go  = other->GetGeomMatrix(i);
	gt->GetIndex(idt);
	go->GetIndex(ido);
	t = kFALSE;
	for(i=0;i<3;i++) t = t&&idt[i]!=ido[i];
	if(t) fprintf(fp,"%4.4d %1.1d %2.2d %2.2d %1.1d %2.2d %2.2d\n",i,
		      idt[0],idt[1],idt[2],ido[0],ido[1],ido[2]);
	gt->GetTranslation(tt);
	go->GetTranslation(to);
	gt->GetAngles(rt);
	go->GetAngles(ro);
	t = kFALSE;
	for(i=0;i<3;i++) t = t&&tt[i]!=to[i];
	if(t) fprintf(fp,"%1.1d %2.2d %2.2d dTrans=%f %f %f drot=%f %f %f\n",
		      idt[0],idt[1],idt[2],
		      tt[0]-to[0],tt[1]-to[1],tt[2]-to[2],
		      rt[0]-ro[0],rt[1]-ro[1],rt[2]-ro[2]);
	t = kFALSE;
	gt->GetMatrix(mt);
	go->GetMatrix(mo);
	for(i=0;i<3;i++)for(j=0;j<3;j++)  t = mt[i][j] != mo[i][j];
	if(t){
	    fprintf(fp,"%1.1d %2.2d %2.2d dfr= %e %e %e\n",
		    idt[0],idt[1],idt[2],
		    mt[0][0]-mo[0][0],mt[0][1]-mo[0][1],mt[0][2]-mo[0][2]);
	    fprintf(fp,"        dfr= %e %e %e\n",
		    mt[1][0]-mo[1][0],mt[1][1]-mo[1][1],mt[1][2]-mo[1][2]);
	    fprintf(fp,"        dfr= %e %e %e\n",
		    mt[2][0]-mo[2][0],mt[2][1]-mo[2][1],mt[2][2]-mo[2][2]);
	} // end if t
    } // end for i
    return;
}
//______________________________________________________________________
void AliITSgeom::PrintData(FILE *fp,Int_t lay,Int_t lad,Int_t det){
    //     This function prints out the coordinate transformations for
    // the particular detector defined by layer, ladder, and detector
    // to the file pointed to by the File pointer fp. fprintf statements
    // are used to print out the numbers. The format is
    // layer ladder detector Trans= fx0 fy0 fz0 rot= frx fry frz 
    // Shape=fShapeIndex
    //                         dfr= fr[0] fr[1] fr[2]
    //                         dfr= fr[3] fr[4] fr[5]
    //                         dfr= fr[6] fr[7] fr[8]
    // By indicating which detector, some control over the information 
    // is given to the user. The output it written to the file pointed
    // to by the file pointer fp. This can be set to stdout if you want.
    // Inputs are:
    // FILE *fp           A file pointer to an opened file for writing in which
    //                    the results of the comparison will be written.
    // Int_t lay          The layer number. Starting from 1.
    // Int_t lad          The ladder number. Starting from 1.
    // Int_t det          The detector number. Starting from 1.
    // outputs are:
    // none
    AliITSgeomMatrix *gt;
    Double_t t[3],r[3],m[3][3];

    gt = this->GetGeomMatrix(GetModuleIndex(lay,lad,det));
    gt->GetTranslation(t);
    gt->GetAngles(r);
    fprintf(fp,"%1.1d %2.2d %2.2d Trans=%f %f %f rot=%f %f %f Shape=%d\n",
	    lay,lad,det,t[0],t[1],t[2],r[0],r[1],r[2],
	    gt->GetDetectorIndex());
    gt->GetMatrix(m);
    fprintf(fp,"        dfr= %e %e %e\n",m[0][0],m[0][1],m[0][2]);
    fprintf(fp,"        dfr= %e %e %e\n",m[1][0],m[1][1],m[1][2]);
    fprintf(fp,"        dfr= %e %e %e\n",m[2][0],m[2][1],m[2][2]);
    return;
}
//______________________________________________________________________
ofstream & AliITSgeom::PrintGeom(ofstream &rb){
    //     Stream out an object of class AliITSgeom to standard output.
    // Intputs are:
    // ofstream &rb    The output streaming buffer.
    // Outputs are:
    // ofstream &rb    The output streaming buffer.
    Int_t i;

    rb.setf(ios::scientific);
    rb << fTrans << " ";
    rb << fNmodules << " ";
    rb << fNlayers << " ";
    for(i=0;i<fNlayers;i++) rb << fNlad[i] << " ";
    for(i=0;i<fNlayers;i++) rb << fNdet[i] << "\n";
    for(i=0;i<fNmodules;i++) {
	rb <<setprecision(16) << *(GetGeomMatrix(i)) << "\n";
    } // end for i
    return rb;
}
//______________________________________________________________________
ifstream & AliITSgeom::ReadGeom(ifstream &rb){
    //     Stream in an object of class AliITSgeom from standard input.
    // Intputs are:
    // ifstream &rb    The input streaming buffer.
    // Outputs are:
    // ifstream &rb    The input streaming buffer.
    Int_t i;

    fNlad = new Int_t[fNlayers];
    fNdet = new Int_t[fNlayers];
    if(fGm!=0){
	for(i=0;i<fNmodules;i++) delete GetGeomMatrix(i);
	delete fGm;
    } // end if fGm!=0

    rb >> fTrans >> fNmodules >> fNlayers;
    fNlad = new Int_t[fNlayers];
    fNdet = new Int_t[fNlayers];
    for(i=0;i<fNlayers;i++) rb >> fNlad[i];
    for(i=0;i<fNlayers;i++) rb >> fNdet[i];
    fGm = new TObjArray(fNmodules,0);
    for(i=0;i<fNmodules;i++){
	fGm->AddAt(new AliITSgeomMatrix,i);
	rb >> *(GetGeomMatrix(i));
    } // end for i
    return rb;
}
//______________________________________________________________________
//     The following routines modify the transformation of "this"
// geometry transformations in a number of different ways.
//______________________________________________________________________
void AliITSgeom::GlobalChange(const Float_t *tran,const Float_t *rot){
    //     This function performs a Cartesian translation and rotation of
    // the full ITS from its default position by an amount determined by
    // the three element arrays tran and rot. If every element
    // of tran and rot are zero then there is no change made
    // the geometry. The change is global in that the exact same translation
    // and rotation is done to every detector element in the exact same way.
    // The units of the translation are those of the Monte Carlo, usually cm,
    // and those of the rotation are in radians. The elements of tran
    // are tran[0] = x, tran[1] = y, and tran[2] = z.
    // The elements of rot are rot[0] = rx, rot[1] = ry, and
    // rot[2] = rz. A change in x will move the hole ITS in the ALICE
    // global x direction, the same for a change in y. A change in z will
    // result in a translation of the ITS as a hole up or down the beam line.
    // A change in the angles will result in the inclination of the ITS with
    // respect to the beam line, except for an effective rotation about the
    // beam axis which will just rotate the ITS as a hole about the beam axis.
    // Intputs are:
    // Float_t *tran   A 3 element array representing the global translations.
    //                 the elements are x,y,z in cm.
    // Float_t *rot    A 3 element array representing the global rotation
    //                 angles about the three axis x,y,z in radians
    // Outputs are:
    // none.
    Int_t    i,j;
    Double_t t[3],r[3];
    AliITSgeomMatrix *g;

    fTrans = (fTrans && 0xfffd) + 2;  // set bit 1 true.
    for(i=0;i<fNmodules;i++){
	g = this->GetGeomMatrix(i);
	g->GetTranslation(t);
	g->GetAngles(r);
	for(j=0;j<3;j++){
	    t[j] += tran[j];
	    r[j] += rot[j];
	} // end for j
	g->SetTranslation(t);
	g->SetAngles(r);
    } // end for i
    return;
}
//______________________________________________________________________
void AliITSgeom::GlobalCylindericalChange(const Float_t *tran,
					  const Float_t *rot){
    //     This function performs a cylindrical translation and rotation of
    // each ITS element by a fixed about in radius, rphi, and z from its
    // default position by an amount determined by the three element arrays
    // tran and rot. If every element of tran and
    // rot are zero then there is no change made the geometry. The
    // change is global in that the exact same distance change in translation
    // and rotation is done to every detector element in the exact same way.
    // The units of the translation are those of the Monte Carlo, usually cm,
    // and those of the rotation are in radians. The elements of tran
    // are tran[0] = r, tran[1] = rphi, and tran[2] = z.
    // The elements of rot are rot[0] = rx, rot[1] = ry, and
    // rot[2] = rz. A change in r will results in the increase of the
    // radius of each layer by the same about. A change in rphi will results in
    // the rotation of each layer by a different angle but by the same
    // circumferential distance. A change in z will result in a translation
    // of the ITS as a hole up or down the beam line. A change in the angles
    // will result in the inclination of the ITS with respect to the beam
    // line, except for an effective rotation about the beam axis which will
    // just rotate the ITS as a hole about the beam axis.
    // Intputs are:
    // Float_t *tran   A 3 element array representing the global translations.
    //                 the elements are r,theta,z in cm/radians.
    // Float_t *rot    A 3 element array representing the global rotation
    //                 angles about the three axis x,y,z in radians
    // Outputs are:
    // none.
    Int_t    i,j;
    Double_t t[3],ro[3],r,r0,phi,rphi;
    AliITSgeomMatrix *g;

    fTrans = (fTrans && 0xfffd) + 2;  // set bit 1 true.
    for(i=0;i<fNmodules;i++){
	g = this->GetGeomMatrix(i);
	g->GetTranslation(t);
	g->GetAngles(ro);
	r = r0= TMath::Hypot(t[1],t[0]);
	phi   = TMath::ATan2(t[1],t[0]);
	rphi  = r0*phi;
	r    += tran[0];
	rphi += tran[1];
	phi   = rphi/r0;
	t[0]  = r*TMath::Cos(phi);
	t[1]  = r*TMath::Sin(phi);
	t[2] += tran[2];
	for(j=0;j<3;j++){
	    ro[j] += rot[j];
	} // end for j
	g->SetTranslation(t);
	g->SetAngles(ro);
    } // end for i
    return;
}
//______________________________________________________________________
void AliITSgeom::RandomChange(const Float_t *stran,const Float_t *srot){
    //     This function performs a Gaussian random displacement and/or
    // rotation about the present global position of each active
    // volume/detector of the ITS. The sigma of the random displacement
    // is determined by the three element array stran, for the
    // x y and z translations, and the three element array srot,
    // for the three rotation about the axis x y and z.
    // Intputs are:
    // Float_t *stran  A 3 element array representing the global translations
    //                 variances. The elements are x,y,z in cm.
    // Float_t *srot   A 3 element array representing the global rotation
    //                 angles variances about the three axis x,y,z in radians.
    // Outputs are:
    // none.
    Int_t    i,j;
    Double_t t[3],r[3];
    AliITSgeomMatrix *g;

    fTrans = (fTrans && 0xfffd) + 2;  // set bit 1 true.
    for(i=0;i<fNmodules;i++){
	g = this->GetGeomMatrix(i);
	g->GetTranslation(t);
	g->GetAngles(r);
	for(j=0;j<3;j++){
	    t[j] += gRandom->Gaus(0.0,stran[j]);
	    r[j] += gRandom->Gaus(0.0, srot[j]);
	} // end for j
	g->SetTranslation(t);
	g->SetAngles(r);
    } // end for i
    return;
}
//______________________________________________________________________
void AliITSgeom::RandomCylindericalChange(const Float_t *stran,
					  const Float_t *srot){
    //     This function performs a Gaussian random displacement and/or
    // rotation about the present global position of each active
    // volume/detector of the ITS. The sigma of the random displacement
    // is determined by the three element array stran, for the
    // r rphi and z translations, and the three element array srot,
    // for the three rotation about the axis x y and z. This random change
    // in detector position allow for the simulation of a random uncertainty
    // in the detector positions of the ITS.
    // Intputs are:
    // Float_t *stran  A 3 element array representing the global translations
    //                 variances. The elements are r,theta,z in cm/readians.
    // Float_t *srot   A 3 element array representing the global rotation
    //                 angles variances about the three axis x,y,z in radians.
    // Outputs are:
    // none.
    Int_t    i,j;
    Double_t t[3],ro[3],r,r0,phi,rphi;
    TRandom ran;
    AliITSgeomMatrix *g;

    fTrans = (fTrans && 0xfffd) + 2;  // set bit 1 true.
    for(i=0;i<fNmodules;i++){
	g = this->GetGeomMatrix(i);
	g->GetTranslation(t);
	g->GetAngles(ro);
	r = r0= TMath::Hypot(t[1],t[0]);
	phi   = TMath::ATan2(t[1],t[0]);
	rphi  = r0*phi;
	r    += ran.Gaus(0.0,stran[0]);
	rphi += ran.Gaus(0.0,stran[1]);
	phi   = rphi/r0;
	t[0]  = r*TMath::Cos(phi);
	t[1]  = r*TMath::Sin(phi);
	t[2] += ran.Gaus(0.0,stran[2]);
	for(j=0;j<3;j++){
	    ro[j] += ran.Gaus(0.0, srot[j]);
	} // end for j
	g->SetTranslation(t);
	g->SetAngles(ro);
    } // end for i
    return;
}
//______________________________________________________________________
void AliITSgeom::GeantToTracking(AliITSgeom &source){
    //     Copy the geometry data but change it to go between the ALICE
    // Global coordinate system to that used by the ITS tracking. A slightly
    // different coordinate system is used when tracking. This coordinate 
    // system is only relevant when the geometry represents the cylindrical
    // ALICE ITS geometry. For tracking the Z axis is left alone but X-> -Y
    // and Y-> X such that X always points out of the ITS cylinder for every
    // layer including layer 1 (where the detectors are mounted upside down).
    // Inputs are:
    // AliITSgeom &source  The AliITSgeom class with which to make this
    //                     a copy of.
    // Outputs are:
    // return  *this       The a new copy of source.
    //Begin_Html
    /*
      <img src="picts/ITS/AliITSgeomMatrix_T1.gif">
    */
    //End_Html
    Int_t    i,j,k,l,id[3];
    Double_t r0[3][3],r1[3][3];
    Double_t a0[3][3] = {{0.,+1.,0.},{-1.,0.,0.},{0.,0.,+1.}};
    Double_t a1[3][3] = {{0.,-1.,0.},{+1.,0.,0.},{0.,0.,+1.}};

    *this = source;  // copy everything
    for(i=0;i<GetIndexMax();i++){
	GetGeomMatrix(i)->GetIndex(id);
	GetGeomMatrix(i)->GetMatrix(r0);
	if(id[0]==1){ // Layer 1 is treated different from the others.
	    for(j=0;j<3;j++) for(k=0;k<3;k++){
		r1[j][k] = 0.;
		for(l=0;l<3;l++) r1[j][k] += a0[j][l]*r0[l][k];
	    } // end for j,k
	}else{
	    for(j=0;j<3;j++) for(k=0;k<3;k++){
		r1[j][k] = 0.;
		for(l=0;l<3;l++) r1[j][k] += a1[j][l]*r0[l][k];
	    } // end for j,k
	} // end if
	GetGeomMatrix(i)->SetMatrix(r1);
    } // end for i
    this->fTrans = (this->fTrans && 0xfffe) + 1;  // set bit 0 true.
    return;
}
//______________________________________________________________________
Int_t AliITSgeom::GetNearest(const Double_t g[3],Int_t lay){
    //      Finds the Detector (Module) that is nearest the point g [cm] in
    // ALICE Global coordinates. If layer !=0 then the search is restricted
    // to Detectors (Modules) in that particular layer.
    // Inputs are:
    // Double_t g[3]  The ALICE Cartesean global coordinate from which the
    //                distance is to be calculated with.
    // Int_t lay      The layer to restrict the search to. If layer=0 then
    //                all layers are searched. Default is lay=0.
    // Outputs are:
    // return         The module number representing the nearest module.
    Int_t    i,l,a,e,in=0;
    Double_t d,dn=1.0e10;
    Bool_t   t=lay!=0; // skip if lay = 0 default value check all layers.

    for(i=0;i<fNmodules;i++){
	if(t){GetModuleId(i,l,a,e);if(l!=lay) continue;}
	if((d=GetGeomMatrix(i)->Distance2(g))<dn){
	    dn = d;
	    in = i;
	} // end if
    } // end for i
    return in;
}
//______________________________________________________________________
void AliITSgeom::GetNearest27(const Double_t g[3],Int_t n[27],Int_t lay){
    //      Finds 27 Detectors (Modules) that are nearest the point g [cm] in
    // ALICE Global coordinates. If layer !=0 then the search is restricted
    // to Detectors (Modules) in that particular layer. The number 27 comes 
    // from including the nearest detector and all those around it (up, down,
    // left, right, forwards, backwards, and the corners).
    // Inputs are:
    // Double_t g[3]  The ALICE Cartesean global coordinate from which the
    //                distance is to be calculated with.
    // Int_t lay      The layer to restrict the search to. If layer=0 then
    //                all layers are searched. Default is lay=0.
    // Outputs are:
    // Int_t n[27]    The module number representing the nearest 27 modules
    //                in order.
    Int_t    i,l,a,e,in[27]={0,0,0,0,0,0,0,0,0,
			     0,0,0,0,0,0,0,0,0,
			     0,0,0,0,0,0,0,0,0,};
    Double_t d,dn[27]={1.0e10,1.0e10,1.0e10,1.0e10,1.0e10,1.0e10,
		       1.0e10,1.0e10,1.0e10,1.0e10,1.0e10,1.0e10,
		       1.0e10,1.0e10,1.0e10,1.0e10,1.0e10,1.0e10,
		       1.0e10,1.0e10,1.0e10,1.0e10,1.0e10,1.0e10,
		       1.0e10,1.0e10,1.0e10};
    Bool_t   t=(lay!=0); // skip if lay = 0 default value check all layers.

    for(i=0;i<fNmodules;i++){
	if(t){GetModuleId(i,l,a,e);if(l!=lay) continue;}
	for(a=0;a<27;a++){
	    d = GetGeomMatrix(i)->Distance2(g);
	    if(d<dn[a]){
		for(e=26;e>a;e--){dn[e] = dn[e-1];in[e] = in[e-1];}
		dn[a] = d; in[a] = i;
	    } // end if d<dn[i]
	} // end for a
    } // end for i
    for(i=0;i<27;i++) n[i] = in[i];
}
//----------------------------------------------------------------------
