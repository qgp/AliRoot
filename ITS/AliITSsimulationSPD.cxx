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
Revision 1.14.2.1  2002/06/06 14:23:57  hristov
Merged with v3-08-02

Revision 1.23  2002/10/25 18:54:22  barbera
Various improvements and updates from B.S.Nilsen and T. Virgili

Revision 1.22  2002/10/22 14:45:44  alibrary
Introducing Riostream.h

Revision 1.21  2002/10/14 14:57:08  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.15.4.3  2002/10/14 13:14:08  hristov
Updating VirtualMC to v3-09-02

Revision 1.20  2002/09/11 10:32:41  hristov
Use new for arrays with variable size

Revision 1.19  2002/09/09 17:23:28  nilsen
Minor changes in support of changes to AliITSdigitS?D class'.

Revision 1.18  2002/08/21 22:11:13  nilsen
Debug output now settable via a DEBUG flag.

Revision 1.17  2002/07/16 17:00:17  barbera
Fixes added to make the slow simulation running with the current HEAD (from M. Masera)

Revision 1.16  2002/06/19 16:02:22  hristov
Division by zero corrected

Revision 1.15  2002/03/15 17:32:14  nilsen
Reintroduced SDigitization, and Digitization from SDigits, along with
functions InitSimulationModule, and FinishSDigitizModule.

Revision 1.14  2001/11/23 13:04:07  barbera
Some protection added in case of high multiplicity

Revision 1.13  2001/11/13 11:13:24  barbera
A protection against tracks with the same entrance and exit has been made more strict

Revision 1.12  2001/10/04 22:44:31  nilsen
Major changes in supppor of PreDigits (SDigits). Changes made with will make
it easier to suppor expected changes in AliITSHit class. Added use of new
class AliITSpList. Both SPD and SDD have added effects of Dead Channels. Both
of these will require addtional work as data bases of detectors and the like
are developed.

*/
#include <Riostream.h>
#include <TRandom.h>
#include <TH1.h>
#include <TMath.h>
#include <TString.h>
#include <TParticle.h>

#include "AliRun.h"
#include "AliITS.h"
#include "AliITShit.h"
#include "AliITSdigit.h"
#include "AliITSmodule.h"
#include "AliITSMapA2.h"
#include "AliITSpList.h"
#include "AliITSsimulationSPD.h"
#include "AliITSsegmentation.h"
#include "AliITSresponse.h"
#include "AliITSsegmentationSPD.h"
#include "AliITSresponseSPD.h"

//#define DEBUG

ClassImp(AliITSsimulationSPD)
////////////////////////////////////////////////////////////////////////
// Version: 0
// Written by Rocco Caliandro
// from a model developed with T. Virgili and R.A. Fini
// June 15 2000
//
// AliITSsimulationSPD is the simulation of SPDs
//
//______________________________________________________________________
AliITSsimulationSPD::AliITSsimulationSPD(){
    // Default constructor

    fResponse     = 0;
    fSegmentation = 0;
    fHis          = 0;
    fMapA2        = 0;

/*
    fThresh       = 0.;
    fSigma        = 0.;
    fCouplCol     = 0.;
    fCouplRow     = 0.; */
}
//______________________________________________________________________
AliITSsimulationSPD::AliITSsimulationSPD(AliITSsegmentation *seg,
					 AliITSresponse *resp) {
    // Standard constructor

    fResponse     = 0;
    fSegmentation = 0;
    fHis          = 0;
    fMapA2        = 0;

/*
    fThresh       = 0.;
    fSigma        = 0.;
    fCouplCol     = 0.;
    fCouplRow     = 0.*/
    Init((AliITSsegmentationSPD*)seg,(AliITSresponseSPD*)resp);
}
//______________________________________________________________________
void AliITSsimulationSPD::Init(AliITSsegmentationSPD *seg,
			       AliITSresponseSPD *resp) {
    // Initilizes the variables of AliITSsimulation SPD.

    fHis = 0;
    fResponse     = resp;
    fSegmentation = seg;
    fMapA2  = new AliITSMapA2(fSegmentation);
    fpList  = new AliITSpList(GetNPixelsZ()+1,GetNPixelsX()+1);
/*
    fResponse->Thresholds(fThresh,fSigma);
    fResponse->GetNoiseParam(fCouplCol,fCouplRow);
    fNPixelsZ = fSegmentation->Npz();
    fNPixelsX = fSegmentation->Npx();
*/
}
//______________________________________________________________________
AliITSsimulationSPD::~AliITSsimulationSPD() { 
    // destructor

    delete fMapA2;
//    delete fpList;

    if (fHis) {
	fHis->Delete(); 
	delete fHis;     
    } // end if
}
//______________________________________________________________________
AliITSsimulationSPD::AliITSsimulationSPD(const AliITSsimulationSPD &source){
    // Copy Constructor

    if(&source == this) return;

    this->fMapA2    = source.fMapA2;
    this->fHis      = source.fHis;
/*
    this->fThresh   = source.fThresh;
    this->fSigma    = source.fSigma;
    this->fCouplCol = source.fCouplCol;
    this->fCouplRow = source.fCouplRow;
    this->fNPixelsX = source.fNPixelsX;
    this->fNPixelsZ = source.fNPixelsZ;
*/
    return;
}
//______________________________________________________________________
AliITSsimulationSPD& AliITSsimulationSPD::operator=(const AliITSsimulationSPD 
						    &source) {
    //    Assignment operator

    if(&source == this) return *this;

    this->fMapA2    = source.fMapA2;
    this->fHis      = source.fHis;
/*
    this->fThresh   = source.fThresh;
    this->fSigma    = source.fSigma;
    this->fCouplCol = source.fCouplCol;
    this->fCouplRow = source.fCouplRow;
    this->fNPixelsX = source.fNPixelsX;
    this->fNPixelsZ = source.fNPixelsZ;
*/
    return *this;
} 
//______________________________________________________________________
void AliITSsimulationSPD::InitSimulationModule(Int_t module,Int_t event){
    // Creates maps to build the list of tracks for each sumable digit
    // Inputs:
    //   Int_t module    // Module number to be simulated
    //   Int_t event     // Event number to be simulated
    // Outputs:
    //   none.
    // Return
    //    none.
 
    fModule = module;
    fEvent  = event;
    fMapA2->ClearMap();
    fpList->ClearMap();
}
//______________________________________________________________________
void AliITSsimulationSPD::FinishSDigitiseModule(){
    // Does the Sdigits to Digits work
    // Inputs:
    //   none.
    // Outputs:
    //   none.
    // Return:
    //   none.

    SDigitsToDigits(fModule,fpList);
}
//______________________________________________________________________
void AliITSsimulationSPD::SDigitiseModule(AliITSmodule *mod, Int_t dummy0,
                                             Int_t dummy1) {
    // Sum digitize module
    if (!(mod->GetNhits())) return; // if module has no hits then no Sdigits.
    Int_t    number     = 10000;
    Int_t    *frowpixel = new Int_t[number];
    Int_t    *fcolpixel = new Int_t[number];
    Double_t *fenepixel = new Double_t[number];

    fModule = mod->GetIndex();

    // Array of pointers to store the track index of the digits
    // leave +1, otherwise pList crashes when col=256, row=192

    HitsToAnalogDigits(mod,frowpixel,fcolpixel,fenepixel,fpList);

    WriteSDigits(fpList);

    // clean memory
    delete[] frowpixel;
    delete[] fcolpixel;
    delete[] fenepixel;
    fMapA2->ClearMap();
    fpList->ClearMap();
}
//______________________________________________________________________
void AliITSsimulationSPD::DigitiseModule(AliITSmodule *mod, Int_t dummy0,
                                             Int_t dummy1) {
    // digitize module. Also need to digitize modules with only noise.

    Int_t    number     = 10000;
    Int_t    *frowpixel = new Int_t[number];
    Int_t    *fcolpixel = new Int_t[number];
    Double_t *fenepixel = new Double_t[number];

    // Array of pointers to store the track index of the digits
    // leave +1, otherwise pList crashes when col=256, row=192
    fModule = mod->GetIndex();
    // noise setting
    SetFluctuations(fpList,fModule);

    HitsToAnalogDigits(mod,frowpixel,fcolpixel,fenepixel,fpList);

    // apply mask to SPD module
    SetMask();

    CreateDigit(fModule,fpList);

    // clean memory
    delete[] frowpixel;
    delete[] fcolpixel;
    delete[] fenepixel;
    fMapA2->ClearMap();
    fpList->ClearMap();
}
//______________________________________________________________________
void AliITSsimulationSPD::SDigitsToDigits(Int_t module,AliITSpList *pList) {
    // sum digits to Digits.

#ifdef DEBUG
    cout << "Entering AliITSsimulatinSPD::SDigitsToDigits for module=";
    cout << module << endl;
#endif
    fModule = module;

    // noise setting
    SetFluctuations(pList,module);

    fMapA2->ClearMap(); // since noise is in pList aready. Zero Map so that
    // noise is not doubled when calling FillMapFrompList.

    FillMapFrompList(pList);

    // apply mask to SPD module
    SetMask();

    CreateDigit(module,pList);

    fMapA2->ClearMap();
    pList->ClearMap();
}
//______________________________________________________________________
void AliITSsimulationSPD::UpdateMapSignal(Int_t row,Int_t col,Int_t trk,
					  Int_t hit,Int_t mod,Double_t ene,
					  AliITSpList *pList) {
    // updates the Map of signal, adding the energy  (ene) released by
    // the current track

    fMapA2->AddSignal(row,col,ene);
    pList->AddSignal(row,col,trk,hit,mod,ene);
}
//______________________________________________________________________
void AliITSsimulationSPD::UpdateMapNoise(Int_t row,Int_t col,Int_t mod,
					 Double_t ene,AliITSpList *pList) {
    // updates the Map of noise, adding the energy  (ene) give my noise

    fMapA2->AddSignal(row,col,ene);
    pList->AddNoise(row,col,mod,ene);
}
//______________________________________________________________________
void AliITSsimulationSPD::HitsToAnalogDigits(AliITSmodule *mod,
					     Int_t *frowpixel,Int_t *fcolpixel,
					     Double_t *fenepixel,
					     AliITSpList *pList) {
    // Loops over all hits to produce Analog/floting point digits. This
    // is also the first task in producing standard digits.
    
    // loop over hits in the module
    Int_t hitpos,nhits = mod->GetNhits();
    for (hitpos=0;hitpos<nhits;hitpos++) {
	HitToDigit(mod,hitpos,frowpixel,fcolpixel,fenepixel,pList);
    }// end loop over digits
}
//______________________________________________________________________
void AliITSsimulationSPD::HitToDigit(AliITSmodule *mod,Int_t hitpos,
				     Int_t *frowpixel,Int_t *fcolpixel,
				     Double_t *fenepixel,AliITSpList *pList) {
    //  Steering function to determine the digits associated to a given
    // hit (hitpos)
    // The digits are created by charge sharing (ChargeSharing) and by
    // capacitive coupling (SetCoupling). At all the created digits is
    // associated the track number of the hit (ntrack)
    Double_t x1l=0.0,y1l=0.0,z1l=0.0,x2l=0.0,y2l=0.0,z2l=0.0;
    Int_t r1,r2,c1,c2,row,col,npixel = 0;
    Int_t ntrack;
    Double_t ene=0.0,etot=0.0;
    const Float_t kconv = 10000.;     // cm -> microns
    const Float_t kconv1= 0.277e9;    // GeV -> electrons equivalent

    if(!(mod->LineSegmentL(hitpos,x1l,x2l,y1l,y2l,z1l,z2l,etot,ntrack)))return;

    x2l += x1l; y2l += y1l; z2l += z1l; // Convert to ending coordinate.
    // positions shifted and converted in microns
    x1l   = x1l*kconv + fSegmentation->Dx()/2.;
    z1l   = z1l*kconv + fSegmentation->Dz()/2.;
    // positions  shifted and converted in microns
    x2l   = x2l*kconv + fSegmentation->Dx()/2.;
    z2l   = z2l*kconv + fSegmentation->Dz()/2.;
    etot *= kconv1; // convert from GeV to electrons equivalent.
    Int_t module = mod->GetIndex();

    // to account for the effective sensitive area
    // introduced in geometry 
    if (z1l<0 || z1l>fSegmentation->Dz()) return;
    if (z2l<0 || z2l>fSegmentation->Dz()) return;
    if (x1l<0 || x1l>fSegmentation->Dx()) return;
    if (x2l<0 || x2l>fSegmentation->Dx()) return;

    //Get the col and row number starting from 1
    // the x direction is not inverted for the second layer!!!
    fSegmentation->GetPadIxz(x1l, z1l, c1, r1); 
    fSegmentation->GetPadIxz(x2l, z2l, c2, r2);

    // to account for unexpected equal entrance and 
    // exit coordinates
    if (x1l==x2l) x2l=x2l+x2l*0.1;
    if (z1l==z2l) z2l=z2l+z2l*0.1;

    if ((r1==r2) && (c1==c2)){
	// no charge sharing
	npixel = 1;		 
	frowpixel[npixel-1] = r1;
	fcolpixel[npixel-1] = c1;
	fenepixel[npixel-1] = etot;
    } else {
	// charge sharing
	ChargeSharing(x1l,z1l,x2l,z2l,c1,r1,c2,r2,etot,
		      npixel,frowpixel,fcolpixel,fenepixel);
    } // end if r1==r2 && c1==c2.

    for (Int_t npix=0;npix<npixel;npix++){
	row = frowpixel[npix];
	col = fcolpixel[npix];
	ene = fenepixel[npix];
	UpdateMapSignal(row,col,ntrack,hitpos,module,ene,pList); 
	// Starting capacitive coupling effect
	SetCoupling(row,col,ntrack,hitpos,module,pList); 
    } // end for npix
}
//______________________________________________________________________
void AliITSsimulationSPD::ChargeSharing(Float_t x1l,Float_t z1l,Float_t x2l,
					Float_t z2l,Int_t c1,Int_t r1,Int_t c2,
					Int_t r2,Float_t etot,
					Int_t &npixel,Int_t *frowpixel,
					Int_t *fcolpixel,Double_t *fenepixel){
    //  Take into account the geometrical charge sharing when the track
    //  crosses more than one pixel.
    //
    //Begin_Html
    /*
      <img src="picts/ITS/barimodel_2.gif">
      </pre>
      <br clear=left>
      <font size=+2 color=red>
      <a href="mailto:Rocco.Caliandro@ba.infn.it"></a>.
      </font>
      <pre>
    */
    //End_Html
    //Float_t dm;
    Float_t xa,za,xb,zb,dx,dz,dtot,refr,refm,refc;
    Float_t refn=0.;
    Float_t arefm, arefr, arefn, arefc, azb, az2l, axb, ax2l;
    Int_t   dirx,dirz,rb,cb;
    Int_t flag,flagrow,flagcol;
    Double_t epar;

    npixel = 0;
    xa     = x1l;
    za     = z1l;
//    dx     = x1l-x2l;
//    dz     = z1l-z2l;
    dx     = x2l-x1l;
    dz     = z2l-z1l;
    dtot   = TMath::Sqrt((dx*dx)+(dz*dz));   
    if (dtot==0.0) dtot = 0.01;
    dirx   = (Int_t) TMath::Sign((Float_t)1,dx);
    dirz   = (Int_t) TMath::Sign((Float_t)1,dz);

    // calculate the x coordinate of  the pixel in the next column    
    // and the z coordinate of  the pixel in the next row
    Float_t xpos, zpos;

    fSegmentation->GetPadCxz(c1, r1-1, xpos, zpos); 

    Float_t xsize = fSegmentation->Dpx(0);
    Float_t zsize = fSegmentation->Dpz(r1-1);
    
    if (dirx == 1) refr = xpos+xsize/2.;
    else refr = xpos-xsize/2.;

    if (dirz == 1) refn = zpos+zsize/2.;
    else refn = zpos-zsize/2.;

    flag = 0;
    flagrow = 0;
    flagcol = 0;
    do{
	// calculate the x coordinate of the intersection with the pixel
	// in the next cell in row  direction
      if(dz!=0)
        refm = dx*((refn - z1l)/dz) + x1l;
      else
        refm = refr+dirx*xsize;
   
	// calculate the z coordinate of the intersection with the pixel
	// in the next cell in column direction
      if (dx!=0)
        refc = dz*((refr - x1l)/dx) + z1l;
      else
        refc = refn+dirz*zsize;

	arefm = refm * dirx;
	arefr = refr * dirx;
	arefn = refn * dirz;
	arefc = refc * dirz;

	if ((arefm < arefr) && (arefn < arefc)){
	    // the track goes in the pixel in the next cell in row direction
	    xb = refm;
	    zb = refn;
	    cb = c1;
	    rb = r1 + dirz;
	    azb = zb * dirz;
	    az2l = z2l * dirz;
	    if (rb == r2) flagrow=1;
	    if (azb > az2l) {
	        zb = z2l;
	        xb = x2l;
	    } // end if
	    // shift to the pixel in the next cell in row direction
	    Float_t zsizeNext = fSegmentation->Dpz(rb-1);
	    //to account for cell at the borders of the detector
	    if(zsizeNext==0) zsizeNext = zsize;
	    refn += zsizeNext*dirz;
	}else {
	    // the track goes in the pixel in the next cell in column direction
	    xb = refr;
	    zb = refc;
	    cb = c1 + dirx;
	    rb = r1;
	    axb = xb * dirx;
	    ax2l = x2l * dirx;
	    if (cb == c2) flagcol=1;
	    if (axb > ax2l) {
	        zb = z2l;
	        xb = x2l;
	    } // end ifaxb > ax2l

	    // shift to the pixel in the next cell in column direction
	    Float_t xsizeNext = fSegmentation->Dpx(cb-1);
	    //to account for cell at the borders of the detector
	    if(xsizeNext==0) xsizeNext = xsize;
	    refr += xsizeNext*dirx;
	} // end if (arefm < arefr) && (arefn < arefc)

	//calculate the energy lost in the crossed pixel      
	epar = TMath::Sqrt((xb-xa)*(xb-xa)+(zb-za)*(zb-za)); 
	epar = etot*(epar/dtot);

	//store row, column and energy lost in the crossed pixel
	frowpixel[npixel] = r1;
	fcolpixel[npixel] = c1;
	fenepixel[npixel] = epar;
	npixel++;
 
	// the exit point of the track is reached
	if (epar == 0) flag = 1;
	if ((r1 == r2) && (c1 == c2)) flag = 1;
	if (flag!=1) {
	    r1 = rb;
	    c1 = cb;
	    xa = xb;
	    za = zb;
	} // end if flag!=1
    } while (flag==0);
}
//______________________________________________________________________
void AliITSsimulationSPD::SetCoupling(Int_t row, Int_t col, Int_t ntrack,
				      Int_t idhit,Int_t module,
				      AliITSpList *pList) {
    //  Take into account the coupling between adiacent pixels.
    //  The parameters probcol and probrow are the probability of the
    //  signal in one pixel shared in the two adjacent pixels along
    //  the column and row direction, respectively.
    //
    //Begin_Html
    /*
      <img src="picts/ITS/barimodel_3.gif">
      </pre>
      <br clear=left>
      <font size=+2 color=red>
      <a href="mailto:tiziano.virgili@cern.ch"></a>.
      </font>
      <pre>
    */
    //End_Html
    Int_t j1,j2,flag=0;
    Double_t pulse1,pulse2;
    Float_t couplR=0.0,couplC=0.0;
    Double_t xr=0.;

    GetCouplings(couplR,couplC);
    j1 = row;
    j2 = col;
    pulse1 = fMapA2->GetSignal(row,col);
    pulse2 = pulse1;
    for (Int_t isign=-1;isign<=1;isign+=2){// loop in row direction
	do{
	    j1 += isign;
	    //   pulse1 *= couplR; 
          xr = gRandom->Rndm();
	  //   if ((j1<0) || (j1>GetNPixelsZ()-1) || (pulse1<GetThreshold())){
	    if ((j1<0) || (j1>GetNPixelsZ()-1) || (xr>couplR)){
		j1 = row;
	       	flag = 1;
	    }else{
		UpdateMapSignal(j1,col,ntrack,idhit,module,pulse1,pList);
	      //  flag = 0;
	      flag = 1; // only first next!!
	    } // end if
	} while(flag == 0);
	// loop in column direction
      do{
	  j2 += isign;
	  // pulse2 *= couplC; 
          xr = gRandom->Rndm();
	  //  if ((j2<0) || (j2>(GetNPixelsX()-1)) || (pulse2<GetThreshold())){
	    if ((j2<0) || (j2>GetNPixelsX()-1) || (xr>couplC)){
	      j2 = col;
	      flag = 1;
	  }else{
	      UpdateMapSignal(row,j2,ntrack,idhit,module,pulse2,pList);
	      //  flag = 0;
	      flag = 1; // only first next!!
	  } // end if
      } while(flag == 0);
    } // for isign
}
//______________________________________________________________________
void AliITSsimulationSPD::SetCouplingOld(Int_t row, Int_t col, Int_t ntrack,
					 Int_t idhit,Int_t module,
					 AliITSpList *pList) {
    //  Take into account the coupling between adiacent pixels.
    //  The parameters probcol and probrow are the fractions of the
    //  signal in one pixel shared in the two adjacent pixels along
    //  the column and row direction, respectively.
    //
    //Begin_Html
    /*
      <img src="picts/ITS/barimodel_3.gif">
      </pre>
      <br clear=left>
      <font size=+2 color=red>
      <a href="mailto:Rocco.Caliandro@ba.infn.it"></a>.
      </font>
      <pre>
    */
    //End_Html
    Int_t j1,j2,flag=0;
    Double_t pulse1,pulse2;
    Float_t couplR=0.0,couplC=0.0;

    GetCouplings(couplR,couplC);
    j1 = row;
    j2 = col;
    pulse1 = fMapA2->GetSignal(row,col);
    pulse2 = pulse1;
    for (Int_t isign=-1;isign<=1;isign+=2){// loop in row direction
	do{
	    j1 += isign;
	    pulse1 *= couplR;
	    if ((j1<0) || (j1>GetNPixelsZ()-1) || (pulse1<GetThreshold())){
		pulse1 = fMapA2->GetSignal(row,col);
		j1 = row;
		flag = 1;
	    }else{
		UpdateMapSignal(j1,col,ntrack,idhit,module,pulse1,pList);
		flag = 0;
	    } // end if
	} while(flag == 0);
	// loop in column direction
      do{
	  j2 += isign;
	  pulse2 *= couplC;
	  if ((j2<0) || (j2>(GetNPixelsX()-1)) || (pulse2<GetThreshold())){
	      pulse2 = fMapA2->GetSignal(row,col);
	      j2 = col;
	      flag = 1;
	  }else{
	      UpdateMapSignal(row,j2,ntrack,idhit,module,pulse2,pList);
	      flag = 0;
	  } // end if
      } while(flag == 0);
    } // for isign
}
//______________________________________________________________________
void AliITSsimulationSPD::CreateDigit(Int_t module,AliITSpList *pList) {
    // The pixels are fired if the energy deposited inside them is above
    // the threshold parameter ethr. Fired pixed are interpreted as digits
    // and stored in the file digitfilename. One also needs to write out
    // cases when there is only noise (nhits==0).

    static AliITS *aliITS  = (AliITS*)gAlice->GetModule("ITS");

    Int_t size = AliITSdigitSPD::GetNTracks();
    Int_t * digits = new Int_t[size];
    Int_t * tracks = new Int_t[size];
    Int_t * hits = new Int_t[size];
    Float_t * charges = new Float_t[size]; 
    Int_t j1;

    for(j1=0;j1<size;j1++){tracks[j1]=-3;hits[j1]=-1;charges[j1]=0.0;}
    for (Int_t r=1;r<=GetNPixelsZ();r++) {
	for (Int_t c=1;c<=GetNPixelsX();c++) {
	    // check if the deposited energy in a pixel is above the
	    // threshold 
	    Float_t signal = (Float_t) fMapA2->GetSignal(r,c);
	    if ( signal > GetThreshold()) {
		digits[0] = r-1;  // digits starts from 0
		digits[1] = c-1;  // digits starts from 0
		//digits[2] = 1;  
		digits[2] =  (Int_t) signal;  // the signal is stored in
                                              //  electrons
		for(j1=0;j1<size;j1++){
		    if(j1<pList->GetNEnteries()){
			tracks[j1] = pList->GetTrack(r,c,j1);
			hits[j1]   = pList->GetHit(r,c,j1);
			//}else{
			//tracks[j1] = -3;
			//hits[j1]   = -1;
		    } // end if
		    //charges[j1] = 0;
		} // end for j1
		Float_t phys = 0;
		aliITS->AddSimDigit(0,phys,digits,tracks,hits,charges);
#ifdef DEBUG
		cout << " CreateSPDDigit mod=" << fModule << " r,c=" << r;
		cout <<","<<c<< " sig=" << fpList->GetSignalOnly(r,c);
		cout << " noise=" << fpList->GetNoise(r,c);
		cout << " Msig="<< signal << " Thres=" << GetThreshold()<<endl;
#endif
	    } // end if of threshold condition
	} // for c
    }// end do on pixels
    delete [] digits;
    delete [] tracks;
    delete [] hits;
    delete [] charges;

}
//______________________________________________________________________
void AliITSsimulationSPD::SetFluctuations(AliITSpList *pList,Int_t module) {
    //  Set the electronic noise and threshold non-uniformities to all the
    //  pixels in a detector.
    //  The parameter fSigma is the squared sum of the sigma due to noise
    //  and the sigma of the threshold distribution among pixels.
    //
    //Begin_Html
    /*
      <img src="picts/ITS/barimodel_1.gif">
      </pre>
      <br clear=left>
      <font size=+2 color=red>
      <a href="mailto:Rocco.Caliandro@ba.infn.it"></a>.
      </font>
      <pre>
    */
    //End_Html
    Float_t  thr=0.0,sigm=0.0;
    Double_t signal,sigma;
    Int_t iz,ix;

    GetThresholds(thr,sigm);
    sigma = (Double_t) sigm;
    for(iz=1;iz<=GetNPixelsZ();iz++){
	for(ix=1;ix<=GetNPixelsX();ix++){
	    signal = sigma*gRandom->Gaus(); 
	    fMapA2->SetHit(iz,ix,signal);
	    // insert in the label-signal-hit list the pixels fired
	    // only by noise
	    pList->AddNoise(iz,ix,module,signal);
	} // end of loop on pixels
    } // end of loop on pixels
}
//______________________________________________________________________
void AliITSsimulationSPD::SetMask() {
    //  Apply a mask to the SPD module. 1% of the pixel channels are
    //  masked. When the database will be ready, the masked pixels
    //  should be read from it.
    Double_t signal;
    Int_t iz,ix,im;
    Float_t totMask;
    Float_t perc = ((AliITSresponseSPD*)fResponse)->GetFractionDead();
    // in this way we get the same set of random numbers for all runs.
    // This is a cluge for now.
    static TRandom *rnd = new TRandom();

    totMask= perc*GetNPixelsZ()*GetNPixelsX();
    for(im=1;im<totMask;im++){
	do{
	    ix=(Int_t)(rnd->Rndm()*(GetNPixelsX()-1.) + 1.);
	} while(ix<=0 || ix>GetNPixelsX());
	do{
	    iz=(Int_t)(rnd->Rndm()*(GetNPixelsZ()-1.) + 1.);
	} while(iz<=0 || iz>GetNPixelsZ());
	signal = -1.;
	fMapA2->SetHit(iz,ix,signal);
    } // end loop on masked pixels
}
//______________________________________________________________________
void AliITSsimulationSPD::CreateHistograms() {
    // Create Histograms
    Int_t i;

    fHis=new TObjArray(GetNPixelsZ());
    for(i=0;i<GetNPixelsZ();i++) {
	TString spdname("spd_");
	Char_t candnum[4];
	sprintf(candnum,"%d",i+1);
	spdname.Append(candnum);
	(*fHis)[i] = new TH1F(spdname.Data(),"SPD maps",
			      GetNPixelsX(),0.,(Float_t) GetNPixelsX());
    } // end for i
}
//______________________________________________________________________
void AliITSsimulationSPD::ResetHistograms() {
    // Reset histograms for this detector
    Int_t i;

    for(i=0;i<GetNPixelsZ();i++ ) {
	if ((*fHis)[i])    ((TH1F*)(*fHis)[i])->Reset();
    } // end for i
}
//______________________________________________________________________
void AliITSsimulationSPD::WriteSDigits(AliITSpList *pList){
    // Fills the Summable digits Tree
    Int_t i,ni,j,nj;
    static AliITS *aliITS = (AliITS*)gAlice->GetModule("ITS");

    pList->GetMaxMapIndex(ni,nj);
    for(i=0;i<ni;i++)for(j=0;j<nj;j++){
	if(pList->GetSignalOnly(i,j)>0.0){
	    aliITS->AddSumDigit(*(pList->GetpListItem(i,j)));
#ifdef DEBUG
	    cout << "pListSPD: " << *(pList->GetpListItem(i,j)) << endl;
	    cout << " CreateSPDSDigit mod=" << fModule << " r,c=";
	    cout << i  <<","<< j << " sig=" << fpList->GetSignalOnly(i,j);
	    cout << " noise=" << fpList->GetNoise(i,j) <<endl;
#endif
	} // end if
    } // end for i,j
    return;
}
//______________________________________________________________________
void AliITSsimulationSPD::FillMapFrompList(AliITSpList *pList){
    // Fills fMap2A from the pList of Summable digits
    Int_t ix,iz;

    for(iz=0;iz<GetNPixelsZ();iz++)for(ix=0;ix<GetNPixelsX();ix++) 
        fMapA2->AddSignal(iz,ix,pList->GetSignal(iz,ix));
    return;
}
