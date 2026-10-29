/* Automation Studio generated header file */
/* Do not edit ! */
/* MpBase 6.8.0 */

#ifndef _MPBASE_
#define _MPBASE_
#ifdef __cplusplus
extern "C" 
{
#endif
#ifndef _MpBase_VERSION
#define _MpBase_VERSION 6.8.0
#endif

#include <bur/plctypes.h>

#ifndef _BUR_PUBLIC
#define _BUR_PUBLIC
#endif
/* Datatypes and datatypes of function blocks */
typedef enum MpComFacilitiesEnum
{	mpCOM_FAC_UNDEFINED = -1,
	mpCOM_FAC_ARCORE = 0,
	mpCOM_FAC_SAFETY1 = 1,
	mpCOM_FAC_SAFETY2 = 2,
	mpCOM_FAC_GMC1 = 96,
	mpCOM_FAC_GMC2 = 97,
	mpCOM_FAC_GMCAXIS = 98,
	mpCOM_FAC_GMCAXESGROUP = 99,
	mpCOM_FAC_GMCARNCGROUP = 103,
	mpCOM_FAC_TRF = 105,
	mpCOM_FAC_MAPP_INTERNAL = 144,
	mpCOM_FAC_MAPP_CORE = 145,
	mpCOM_FAC_MAPP_INFRASTRUCTURE = 146,
	mpCOM_FAC_MAPP_MECHATRONIC = 147,
	mpCOM_FAC_MAPP_INDUSTRY = 148
} MpComFacilitiesEnum;

typedef enum MpComSeveritiesEnum
{	mpCOM_SEV_SUCCESS = 0,
	mpCOM_SEV_INFORMATIONAL = 1,
	mpCOM_SEV_WARNING = 2,
	mpCOM_SEV_ERROR = 3
} MpComSeveritiesEnum;

typedef struct MpComIdentType
{	unsigned long Internal[2];
} MpComIdentType;

typedef struct MpComInternalDataType
{	unsigned long pObject;
	unsigned long State;
} MpComInternalDataType;

typedef struct MpComConfigInternalType
{	unsigned long pObject;
	signed long State;
} MpComConfigInternalType;

typedef struct MpComInternalIDType
{	signed long ID;
	enum MpComSeveritiesEnum Severity;
	enum MpComFacilitiesEnum Facility;
	unsigned short Code;
} MpComInternalIDType;

typedef struct MpBaseCfgArrayType
{	unsigned long DataAddress;
	unsigned long Capacity;
	unsigned long NumberOfElements;
} MpBaseCfgArrayType;

typedef struct MpBaseCfgPasswordType
{	plcbit Valid;
	plcstring Data[101];
} MpBaseCfgPasswordType;






/* Constants */
#ifdef _REPLACE_CONST
/*Constant mpCOM_STANDALONE not declared. Initialized constants from structured datatypes cannot be used in ANSI C programs / libraries. See Automation Help for details.*/

/*Constant mpCOM_MAIN not declared. Initialized constants from structured datatypes cannot be used in ANSI C programs / libraries. See Automation Help for details.*/

 #define mpCOM_STANDALONE_INT 2U
 #define mpCOM_MAIN_INT 1U
#else


 _GLOBAL_CONST unsigned long mpCOM_STANDALONE_INT;
 _GLOBAL_CONST unsigned long mpCOM_MAIN_INT;
#endif




#ifdef __cplusplus
};
#endif
#endif /* _MPBASE_ */

