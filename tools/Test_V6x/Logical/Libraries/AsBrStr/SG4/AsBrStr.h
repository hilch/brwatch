/* Automation Studio generated header file */
/* Do not edit ! */

#ifndef _ASBRSTR_
#define _ASBRSTR_
#ifdef __cplusplus
extern "C" 
{
#endif

#include <bur/plctypes.h>

#ifndef _BUR_PUBLIC
#define _BUR_PUBLIC
#endif



/* Prototyping of functions and function blocks */
_BUR_PUBLIC unsigned short brsftoa(float value, unsigned long pString);
_BUR_PUBLIC float brsatof(unsigned long pString);
_BUR_PUBLIC double brsatod(unsigned long pString);
_BUR_PUBLIC unsigned short brsitoa(signed long value, unsigned long pString);
_BUR_PUBLIC signed long brsatoi(unsigned long pString);
_BUR_PUBLIC unsigned long brsmemset(unsigned long pDest, unsigned char value, unsigned long length);
_BUR_PUBLIC unsigned long brsmemcpy(unsigned long pDest, unsigned long pSrc, unsigned long length);
_BUR_PUBLIC unsigned long brsmemmove(unsigned long pDest, unsigned long pSrc, unsigned long length);
_BUR_PUBLIC signed long brsmemcmp(unsigned long pMem1, unsigned long pMem2, unsigned long length);
_BUR_PUBLIC unsigned long brsstrcat(unsigned long pDest, unsigned long pSrc);
_BUR_PUBLIC unsigned long brsstrlen(unsigned long pString);
_BUR_PUBLIC unsigned long brsstrcpy(unsigned long pDest, unsigned long pSrc);
_BUR_PUBLIC signed long brsstrcmp(unsigned long pString1, unsigned long pString2);


#ifdef __cplusplus
};
#endif
#endif /* _ASBRSTR_ */

