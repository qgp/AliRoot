#ifndef ROOT_THerwig
#define ROOT_THerwig

// declaration of c++ Class THerwig6 to be used in ROOT
// this is a c++ interface to the F77 Herwig6 program
// author: j. g. contreras jgcn@moni.mda.cinvestav.mx
// date: december 22, 2000

/*

 Class THerwig6 is an interface to the Herwig program

C-----------------------------------------------------------------------
C                           H E R W I G
C
C            a Monte Carlo event generator for simulating
C        +---------------------------------------------------+
C        | Hadron Emission Reactions With Interfering Gluons |
C        +---------------------------------------------------+
C I.G. Knowles(*), G. Marchesini(+), M.H. Seymour($) and B.R. Webber(#)
C-----------------------------------------------------------------------
C with Minimal Supersymmetric Standard Model Matrix Elements by
C                  S. Moretti($) and K. Odagiri($)
C-----------------------------------------------------------------------
C R parity violating Supersymmetric Decays and Matrix Elements by
C                          P. Richardson(&)
C-----------------------------------------------------------------------
C matrix element corrections to top decay and Drell-Yan type processes
C                         by G. Corcella(+)
C-----------------------------------------------------------------------
C Deep Inelastic Scattering and Heavy Flavour Electroproduction by
C                  G. Abbiendi(@) and L. Stanco(%)
C-----------------------------------------------------------------------
C and Jet Photoproduction in Lepton-Hadron Collisions by J. Chyla(~)
C-----------------------------------------------------------------------
C(*)  Department of Physics & Astronomy, University of Edinburgh
C(+)  Dipartimento di Fisica, Universita di Milano
C($)  Rutherford Appleton Laboratory
C(#)  Cavendish Laboratory, Cambridge
C(&)  Department of Physics, University of Oxford
C(@)  Dipartimento di Fisica, Universita di Bologna
C(%)  Dipartimento di Fisica, Universita di Padova
C(~)  Institute of Physics, Prague
C-----------------------------------------------------------------------
C                  Version 6.100 - 16th December 1999
C-----------------------------------------------------------------------
C Main reference:
C    G.Marchesini,  B.R.Webber,  G.Abbiendi,  I.G.Knowles,  M.H.Seymour,
C    and L.Stanco, Computer Physics Communications 67 (1992) 465.
C-----------------------------------------------------------------------
C Please send e-mail about  this program  to one of the  authors at the
C following Internet addresses:
C    I.Knowles@ed.ac.uk        Giuseppe.Marchesini@mi.infn.it
C    M.Seymour@rl.ac.uk        webber@hep.phy.cam.ac.uk
C-----------------------------------------------------------------------
*/

/* declarations from ROOT */
#include "TGenerator.h"
#include "TObjArray.h"

// Translation of Fortran commons from the Herwig6
// f77 program into c++ structures to be used in ROOT
// and declaration of Fortran functions as extern
// C functions to be called from the class Herwig6
// author: j. g. contreras jgcn@moni.mda.cinvestav.mx
// date: december 22, 2000

int const NMXHEP = 2000;

struct Hepevt_t {
  int NEVHEP;
  int NHEP;
  int ISTHEP[NMXHEP];
  int IDHEP[NMXHEP];
  int JMOHEP[NMXHEP][2];
  int JDAHEP[NMXHEP][2];
  double PHEP[NMXHEP][5];
  double VHEP[NMXHEP][4];
};

struct Hwbeam_t {
  int IPART1;
  int IPART2;
};

struct Hwbmch_t {
  char PART1[8];
  char PART2[8];
};

struct Hwproc_t {
  double EBEAM1;
  double EBEAM2;
  double PBEAM1;
  double PBEAM2;
  int    IPROC;
  int    MAXEV;
};

struct Hwpram_t {
  double AFCH[2][16];
  double ALPHEM;
  double B1LIM;
  double BETAF;
  double BTCLM;
  double CAFAC;
  double CFFAC;
  double CLMAX;
  double CLPOW;
  double CLSMR[2];
  double CSPEED;
  double ENSOF;
  double ETAMIX;
  double F0MIX;
  double F1MIX;
  double F2MIX;
  double GAMH;
  double GAMW;
  double GAMZ;
  double GAMZP;
  double GEV2NB;
  double H1MIX;
  double PDIQK;
  double PGSMX;
  double PGSPL[4];
  double PHIMIX;
  double PIFAC;
  double PRSOF;
  double PSPLT[2];
  double PTRMS;
  double PXRMS;
  double QCDL3;
  double QCDL5;
  double QCDLAM;
  double QDIQK;
  double QFCH[16];
  double QG;
  double QSPAC;
  double QV;
  double SCABI;
  double SWEIN;
  double TMTOP;
  double VFCH[2][16];
  double VCKM[3][3];
  double VGCUT;
  double VQCUT;   
  double VPCUT;
  double ZBINM;
  double EFFMIN;
  double OMHMIX;
  double ET2MIX;
  double PH3MIX;
  double GCUTME;
  int    IOPREM;
  int    IPRINT;
  int    ISPAC;
  int    LRSUD;
  int    LWSUD;
  int    MODPDF[2];
  int    NBTRY;
  int    NCOLO;
  int    NCTRY;
  int    NDTRY;
  int    NETRY;
  int    NFLAV;
  int    NGSPL;
  int    NSTRU;
  int    NSTRY;
  int    NZBIN;
  int    IOP4JT[2];
  int    NPRFMT;
  int AZSOFT;
  int AZSPIN;
  int CLDIR[2];
  int HARDME;
  int NOSPAC;
  int PRNDEC;
  int PRVTX;
  int SOFTME;
  int ZPRIME;
  int PRNDEF;
  int PRNTEX;
  int PRNWEB;
};

struct Hwprch_t {
  char AUTPDF[2][20];
  char BDECAY[4];
};

int const NMXPAR = 500;

struct Hwpart_t {
  int  NEVPAR;
  int  NPAR;
  int  ISTPAR[NMXPAR];
  int  IDPAR[NMXPAR];
  int  JMOPAR[NMXPAR][2];
  int  JDAPAR[NMXPAR][2];
  double  PPAR[NMXPAR][5];
  double  VPAR[NMXPAR][4];
};

struct Hwparp_t {
  double DECPAR[NMXPAR][2];
  double PHIPAR[NMXPAR][2];
  double RHOPAR[NMXPAR][2];
  int TMPAR[NMXPAR];
};

int const MODMAX = 5;

struct Hwbosc_t {
  double  ALPFAC;
  double  BRHIG[12];
  double  ENHANC[12];
  double  GAMMAX;
  double  RHOHEP[NMXHEP][3];
  int     IOPHIG;
  int     MODBOS[MODMAX];
};

struct Hwparc_t {
  int     JCOPAR[NMXPAR][4];
};

struct Hwbrch_t {
  double ANOMSC[2][2];
  double HARDST;
  double PTINT[2][3];
  double XFACT;
  int    INHAD;
  int    JNHAD;
  int    NSPAC[7];
  int    ISLENT;
  int    BREIT;
  int    FROST;
  int    USECMF;
};

struct Hwevnt_t {
  double AVWGT;
  double EVWGT;
  double GAMWT;
  double TLOUT;
  double WBIGST;
  double WGTMAX;
  double WGTSUM;
  double WSQSUM;
  int    IDHW[NMXHEP];
  int    IERROR;
  int    ISTAT;
  int    LWEVT;
  int    MAXER;
  int    MAXPR;
  int    NOWGT;
  int    NRN[2];
  int    NUMER;
  int    NUMERU;
  int    NWGTS;
  int    GENSOF;
};

struct Hwhard_t {
  double ASFIXD;
  double CLQ[6][7];
  double COSS;
  double COSTH;
  double CTMAX;
  double DISF[2][13];
  double EMLST;
  double EMMAX;
  double EMMIN;
  double EMPOW;
  double EMSCA;
  double EPOLN[3];
  double GCOEF[7];
  double GPOLN;
  double OMEGA0;
  double PHOMAS;
  double PPOLN[3];
  double PTMAX;
  double PTMIN;
  double PTPOW;
  double Q2MAX;
  double Q2MIN;
  double Q2POW;
  double Q2WWMN;
  double Q2WWMX;
  double QLIM;
  double SINS;
  double THMAX;
  double Y4JT;
  double TMNISR;
  double TQWT;
  double XX[2];
  double XLMIN;
  double XXMIN;
  double YBMAX;
  double YBMIN;
  double YJMAX;
  double YJMIN;
  double YWWMAX;
  double YWWMIN;
  double WHMIN;
  double ZJMAX;
  double ZMXISR;
  int    IAPHIG;
  int    IBRN[2];
  int    IBSH;
  int    ICO[10];
  int    IDCMF;
  int    IDN[10];
  int    IFLMAX;
  int    IFLMIN;
  int    IHPRO;
  int    IPRO;
  int    MAPQ[10];
  int    MAXFL;
  int    BGSHAT;
  int    COLISR;
  int    FSTEVT;
  int    FSTWGT;
  int    GENEV;
  int    HVFCEN;
  int    TPOL;
  int     DURHAM;
};

int const NMXRES = 500;

struct Hwprop_t {
  double RLTIM[NMXRES+1];
  double RMASS[NMXRES+1];
  double RSPIN[NMXRES+1];
  int    ICHRG[NMXRES+1];
  int    IDPDG[NMXRES+1];
  int    IFLAV[NMXRES+1];
  int    NRES;
  int    VTOCDK[NMXRES+1];
  int    VTORDK[NMXRES+1];
  int    QORQQB[NMXRES+1];
  int    QBORQQ[NMXRES+1];
};

struct Hwunam_t {
  char RNAME[NMXRES+1][8];
  char TXNAME[NMXRES+1][2][37];
};

int const NMXDKS = 4000;
int const NMXMOD = 200;

struct Hwupdt_t {
  double BRFRAC[NMXDKS];
  double CMMOM[NMXDKS];
  double DKLTM[NMXRES];
  int    IDK[NMXDKS];
  int    IDKPRD[NMXDKS][5];
  int    LNEXT[NMXDKS];
  int    LSTRT[NMXRES];
  int    NDKYS;
  int    NME[NMXDKS];
  int    NMODES[NMXRES];
  int    NPRODS[NMXDKS];
  int    DKPSET;
  int    RSTAB[NMXRES+1];
};

struct Hwuwts_t {
  double REPWT[5][4][4];
  double SNGWT;
  double DECWT;
  double QWT[3];
  double PWT[12];
  double SWTEF[NMXRES];
};

int const NMXCDK = 4000;

struct Hwuclu_t {
  double CLDKWT[NMXCDK];
  double CTHRPW[12][12];
  double PRECO;
  double RESN[12][12];
  double RMIN[12][12];
  int    LOCN[12][12];
  int    NCLDK[NMXCDK];
  int    NRECO;
  int    CLRECO;
};

struct Hwdist_t {
  double EXAG;
  double GEV2MM;
  double HBAR;
  double PLTCUT;
  double VMIN2;
  double VTXPIP[4];
  double XMIX[2];
  double XMRCT[2];
  double YMIX[2];
  double YMRCT[2];
  int    IOPDKL;
  int    MAXDKL;
  int    MIXING;
  int    PIPSMR;
};

int const NMXQDK=20;

struct Hwqdks_t {
  double VTXQDK[NMXQDK][4];
  int    IMQDK[NMXQDK];
  int    LOCQ[NMXQDK];
  int    NQDK;
};

int const NMXSUD = 1024;

struct Hwusud_t {
  double ACCUR;
  double QEV[6][NMXSUD];
  double SUD[6][NMXSUD];
  int    INTER;
  int    NQEV;
  int    NSUD;
  int    SUDORD;
};

struct Hwsusy_t {
  double TANB;
  double ALPHAH;
  double COSBPA;
  double SINBPA;
  double COSBMA;
  double SINBMA;
  double COSA;
  double SINA;
  double COSB;
  double SINB;
  double COTB;
  double ZMIXSS[4][4];
  double ZMXNSS[4][4];
  double ZSGNSS[4]; 
  double LFCH[16];
  double RFCH[16];
  double SLFCH[4][16];
  double SRFCH[4][16]; 
  double WMXUSS[2][2];
  double WMXVSS[2][2]; 
  double WSGNSS[2];
  double QMIXSS[2][2][6];
  double LMIXSS[2][2][6];
  double THETAT;
  double THETAB;
  double THETAL;
  double ATSS;
  double ABSS;
  double ALSS;
  double MUSS;
  double FACTSS;
  double GHWWSS[3];
  double GHZZSS[3];
  double GHDDSS[4];
  double GHUUSS[4];
  double GHWHSS[3];
  double GHSQSS[2][2][6][4];
  double XLMNSS;
  double RMMNSS;
  double IMSSM;
  double SENHNC[24];
  double SSPARITY;
  int    SUSYIN;
};

struct Hwrpar_t {
  double LAMDA1[3][3][3];
  double LAMDA2[3][3][3];
  double LAMDA3[3][3][3];
  int    HRDCOL[5][2];
  int    RPARTY;
  int    COLUPD;
};

struct Hwminb_t {
  double PMBN1;
  double PMBN2;
  double PMBN3;
  double PMBK1;
  double PMBK2;
  double PMBM1;
  double PMBM2;
  double PMBP1;
  double PMBP2;
  double PMBP3;
};

int const NMXCL = 500;

struct Hwclus_t {
  double PPCL[NMXCL][5];
  int    IDCL[NMXCL];
  int    NCL;
};


extern "C" {
  void  hwigin_();
  void  hwuinc_();
  void  hwusta_(char * name, int);
  void  hweini_();
  void  hwuine_();
  void  hwepro_();
  void  hwbgen_();
  void  hwdhob_();
  void  hwcfor_();
  void  hwcdec_();
  void  hwdhad_();
  void  hwdhvy_();
  void  hwmevt_();
  void  hwufne_();
  void  hwefin_();
}






/* THerwig6 class declaration */
class THerwig6 : public TGenerator {
protected:
  // Standard hep common block
  Hepevt_t* fHepevt;
  // Herwig6 common-blocks
  Hwbeam_t* fHwbeam;
  Hwbmch_t* fHwbmch;
  Hwproc_t* fHwproc;
  Hwpram_t* fHwpram;
  Hwprch_t* fHwprch;
  Hwpart_t* fHwpart;
  Hwparp_t* fHwparp;
  Hwbosc_t* fHwbosc;
  Hwparc_t* fHwparc;
  Hwbrch_t* fHwbrch;
  Hwevnt_t* fHwevnt;
  Hwhard_t* fHwhard;
  Hwprop_t* fHwprop;
  Hwunam_t* fHwunam;
  Hwupdt_t* fHwupdt;
  Hwuwts_t* fHwuwts;
  Hwuclu_t* fHwuclu;
  Hwdist_t* fHwdist;
  Hwqdks_t* fHwqdks;
  Hwusud_t* fHwusud;
  Hwsusy_t* fHwsusy;
  Hwrpar_t* fHwrpar;
  Hwminb_t* fHwminb;
  Hwclus_t* fHwclus;
//----------------------------------------------------------------------------
//  functions:
//----------------------------------------------------------------------------
public:
				// ****** constructors and destructor
  THerwig6();
  virtual ~THerwig6();

  // acces to hep common block
  Hepevt_t*   GetHepevt        ()           { return fHepevt; }
  int         GetNevhep        ()           { return fHepevt->NEVHEP; }
  int         GetNhep          ()           { return fHepevt->NHEP; }
  int         GetISTHEP    (int i)          { return fHepevt->ISTHEP[i-1]; }
  int         GetIDHEP     (int i)          { return fHepevt->IDHEP[i-1]; }
  int         GetJMOHEP (int i, int j) { return fHepevt->JMOHEP[i-1][j-1]; }
  int         GetJDAHEP (int i, int j) { return fHepevt->JDAHEP[i-1][j-1]; }
  double      GetPHEP   (int i, int j) { return fHepevt->PHEP[i-1][j-1]; }
  double      GetVHEP   (int i, int j) { return fHepevt->VHEP[i-1][j-1]; }

  // access to Herwig6 common-blocks
  // WARNING: Some arrays start in 1, others in 0. Look up the manual!

  // /HWBEAM/

  Hwbeam_t*   GetHwbeam        ()           { return fHwbeam; }
  int         GetIPART1        ()           { return fHwbeam->IPART1; }
  int         GetIPART2        ()           { return fHwbeam->IPART2; }

  // /HWBMCH/
  Hwbmch_t*   GetHwbmch        ()           { return fHwbmch; }
  char*       GetPART1         ()           { return fHwbmch->PART1; }
  char*       GetPART2         ()           { return fHwbmch->PART2; }
  
  
  // /HWPROC/
  Hwproc_t*   GetHwproc        ()           { return fHwproc; }
  double      GetEBEAM1        ()           { return fHwproc->EBEAM1; }
  double      GetEBEAM2        ()           { return fHwproc->EBEAM2; }
  double      GetPBEAM1        ()           { return fHwproc->PBEAM1; }
  double      GetPBEAM2        ()           { return fHwproc->PBEAM2; }
  int         GetIPROC         ()           { return fHwproc->IPROC; }
  int         GetMAXEV         ()           { return fHwproc->MAXEV; }

  // /HWPRAM/
  Hwpram_t*   GetHwpram        ()           { return fHwpram; }
  double      GetQCDLAM        ()           { return fHwpram->QCDLAM; }
  void        SetQCDLAM   (double q)        { fHwpram->QCDLAM = q; }
  double      GetVQCUT         ()           { return fHwpram->VQCUT; }
  void        SetVQCUT    (double v)        { fHwpram->VQCUT = v; }
  double      GetVGCUT         ()           { return fHwpram->VGCUT; }
  void        SetVGCUT    (double v)        { fHwpram->VGCUT = v; }
  double      GetVPCUT         ()           { return fHwpram->VPCUT; }
  void        SetVPCUT    (double v)        { fHwpram->VPCUT = v; }
  double      GetCLMAX         ()           { return fHwpram->CLMAX; }
  void        SetCLMAX    (double c)        { fHwpram->CLMAX = c; }
  double      GetCLPOW         ()           { return fHwpram->CLPOW; }
  void        SetCLPOW    (double c)        { fHwpram->CLPOW = c; }
  double      GetPSPLT    (int i)           { return fHwpram->PSPLT[i-1];}
  void        SetPSPLT    (int i, double p) { fHwpram->PSPLT[i-1] = p;}
  double      GetQDIQK         ()           { return fHwpram->QDIQK; }
  void        SetQDIQK    (double q)        { fHwpram->QDIQK = q; }
  double      GetPDIQK         ()           { return fHwpram->PDIQK; }
  void        SetPDIQK    (double p)        { fHwpram->PDIQK = p; }
  double      GetQSPAC         ()           { return fHwpram->QSPAC; }
  void        SetQSPAC    (double q)        { fHwpram->QSPAC = q; }
  double      GetPTRMS         ()           { return fHwpram->PTRMS; }
  void        SetPTRMS    (double p)        { fHwpram->PTRMS = p; }
  double      GetENSOF         ()           { return fHwpram->ENSOF; }
  void        SetENSOF    (double e)        { fHwpram->ENSOF = e; } 
  int         GetIPRINT         ()          { return fHwpram->IPRINT; }
  void        SetIPRINT    (int i)          { fHwpram->IPRINT = i; }
  int         GetMODPDF     (int i)         { return fHwpram->MODPDF[i-1];}
  void        SetMODPDF     (int i, int j)  { fHwpram->MODPDF[i-1] = j; }
  int         GetNSTRU         ()           { return fHwpram->NSTRU; }
  void        SetNSTRU     (int i)          { fHwpram->NSTRU = i; }

  // /HWPRCH/
  Hwprch_t*   GetHwprch        ()           { return fHwprch; }
  char*       GetAUTPDF     (int i)         { return fHwprch->AUTPDF[i-1]; }
  void        SetAUTPDF(int i,const char* s){ strncpy(fHwprch->AUTPDF[i-1],s,20);}
  char*       GetBDECAY        ()           { return fHwprch->BDECAY; }

  // /HWPART/
  Hwpart_t*   GetHwpart        ()           { return fHwpart; }
  
  // /HWPARP/
  Hwparp_t*   GetHwparp        ()           { return fHwparp; }

  // /HWBOSC/
  Hwbosc_t*   GetHwbosc        ()           { return fHwbosc; }

  // /HWPARC/
  Hwparc_t*   GetHwparc        ()           { return fHwparc; }

  // /HWBRCH/
  Hwbrch_t*   GetHwbrch        ()           { return fHwbrch; }

  // /HWEVNT/
  Hwevnt_t*   GetHwevnt        ()           { return fHwevnt; }
  double      GetAVWGT         ()           { return fHwevnt->AVWGT; }
  int         GetMAXPR         ()           { return fHwevnt->MAXPR; }
  void        SetMAXPR    (int i)           { fHwevnt->MAXPR = i; }
  int         GetMAXER         ()           { return fHwevnt->MAXER; }
  void        SetMAXER    (int i)           { fHwevnt->MAXER = i; }
  int         GetNRN      (int i)           { return fHwevnt->NRN[i-1]; }
  void        SetNRN    (int i, int j)      { fHwevnt->NRN[i-1] = j; }

  // /HWHARD/
  Hwhard_t*   GetHwhard        ()           { return fHwhard; }
  double      GetPTMIN         ()           { return fHwhard->PTMIN; }
  void        SetPTMIN    (double d)        { fHwhard->PTMIN = d; }
  double      GetPTPOW         ()           { return fHwhard->PTPOW; }
  void        SetPTPOW    (double d)        { fHwhard->PTPOW = d; }
  double      GetYJMIN         ()           { return fHwhard->YJMIN; }
  void        SetYJMIN    (double d)        { fHwhard->YJMIN = d; }
  double      GetYJMAX         ()           { return fHwhard->YJMAX; }
  void        SetYJMAX    (double d)        { fHwhard->YJMAX = d; }
  double      GetQ2MIN         ()           { return fHwhard->Q2MIN; }
  void        SetQ2MIN    (double d)        { fHwhard->Q2MIN = d; }
  double      GetQ2MAX         ()           { return fHwhard->Q2MAX; }
  void        SetQ2MAX    (double d)        { fHwhard->Q2MAX = d; }
  double      GetYBMIN         ()           { return fHwhard->YBMIN; }
  void        SetYBMIN    (double d)        { fHwhard->YBMIN = d; }
  double      GetYBMAX         ()           { return fHwhard->YBMAX; }
  void        SetYBMAX    (double d)        { fHwhard->YBMAX = d; }
  double      GetZJMAX        ()            { return fHwhard->ZJMAX; }
  void        SetZJMAX    (double d)        { fHwhard->ZJMAX = d; }

  // /HWPROP/
  Hwprop_t*   GetHwprop        ()           { return fHwprop; }
  double      GetRMASS      (int i)         { return fHwprop->RMASS[i]; }
  void        SetRMASS    (int i, double r) { fHwprop->RMASS[i] = r; }

  // /HWUNAM/
  Hwunam_t*   GetHwunam        ()           { return fHwunam; }

  // /HWUPDT/
  Hwupdt_t*   GetHwupdt        ()           { return fHwupdt; }

  // /HWUWTS/
  Hwuwts_t*   GetHwuwts        ()           { return fHwuwts; }

  // /HWUCLU/
  Hwuclu_t*   GetHwuclu        ()           { return fHwuclu; }

  // /HWDIST/
  Hwdist_t*   GetHwdist        ()           { return fHwdist; }

  // /HWQDKT/
  Hwqdks_t*   GetHwqdkt        ()           { return fHwqdks; }

  // /HWUSUD/
  Hwusud_t*   GetHwusud        ()           { return fHwusud; }

  // /HWSUSY/
  Hwsusy_t*   GetHwsusy        ()           { return fHwsusy; }

  // /HWRPAR/
  Hwrpar_t*   GetHwrpar        ()           { return fHwrpar; }

  // /HWMINB/
  Hwminb_t*   GetHwminb        ()           { return fHwminb; }

  // /HWCLUS/
  Hwclus_t*   GetHwclus        ()           { return fHwclus; }

  // Herwig6 routines  
  // the user would call 
  //   Initialize
  //   change by himself the parameters s/he wants
  //   Hwusta to make stable the particles s/he wants
  //   PrepareRun
  //   GenerateEvent as many times as wished
  // An example is given in SetupTest

  void             GenerateEvent();
  void             Initialize(const char *beam, const char *target, double pbeam1, double pbeam2, int iproc);
  void             PrepareRun();
  void             OpenFortranFile(int lun, char* name);
  void             CloseFortranFile(int lun);
  Int_t            ImportParticles(TClonesArray *particles, Option_t *option="");
  TObjArray       *ImportParticles(Option_t *option="");
  TObjArray       *Particles() { return fParticles; }
  void             Hwigin();
  void             Hwuinc();
  void             Hwusta(char * name);
  void             Hweini();
  void             Hwuine();
  void             Hwepro();
  void             Hwbgen();
  void             Hwdhob();
  void             Hwcfor();
  void             Hwcdec();
  void             Hwdhad();
  void             Hwdhvy();
  void             Hwmevt();
  void             Hwufne();
  void             Hwefin();
  void             SetupTest();

  ClassDef(THerwig6,0)  //Interface to Herwig6.1 Event Generator
};

#endif
