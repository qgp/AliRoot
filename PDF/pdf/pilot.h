#if 0
*CMZ :          16/03/95  22.26.48  by  H. Plothow-Besch
*Modified by IML
#endif
#if !defined(CERNLIB_SINGLE)
#ifndef CERNLIB_DOUBLE
#define CERNLIB_DOUBLE
#endif
#endif
#if defined __APPLE__ && !defined __INTEL_COMPILER
#define stop CALL EXIT !
#define STOP CALL EXIT !
#endif
