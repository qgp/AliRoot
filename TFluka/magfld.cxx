#include "TVirtualMCApplication.h"
#include "Fdblprc.h"  //(DBLPRC) fluka common
//
// #include "TCallf77.h"

#ifndef WIN32
#define magfld magfld_
#define type_of_call
#else
#define magfld MAGFLD
#define type_of_call  _stdcall
#endif

extern "C" void type_of_call magfld(double& x,   double& y,   double& z, 
				    double& btx, double& bty, double& btz, double& b, 
				    int&    nreg,int& idisc)
{

/*
*----------------------------------------------------------------------*
*                                                                      *
*                                                                      *
*     Input variables:                                                 *
*            x,y,z = current position                                  *
*            nreg  = current region                                    *
*     Output variables:                                                *
*            btx,bty,btz = cosines of the magn. field vector           *
*            B = magnetic field intensity (Tesla)                      *
*            idisc = set to 1 if the particle has to be discarded      *
*                                                                      *
*----------------------------------------------------------------------*
*/
    
    idisc = 0;
    
    Double_t bc[3];
    Double_t xc[3];
    
    xc[1] = x;
    xc[0] = y;
    xc[2] = z;
    
    
	
    (TVirtualMCApplication::Instance())->Field(xc, bc);
    
    b = sqrt(bc[0] * bc[0] + bc[1] * bc[1] + bc[2] * bc[2]);
    if (b) {
	btx = bc[1]/b;
	bty = bc[0]/b;
	Double_t btt = btx * btx + bty * bty;
	if (btt >= (Double_t) 1.) {
	    btx /= TMath::Sqrt(btt);
	    bty /= TMath::Sqrt(btt);
	    b   /= TMath::Sqrt(btt);
	    btz =  (Double_t) 0.;
	} else {
	    btz = TMath::Sqrt((Double_t) 1. -  btt);
	}
    } else {
	btx = 0.;
	bty = 0.;
	btz = 1.;
    }
    
    // from kG to T
    b /= (Double_t) 10.;
} 
