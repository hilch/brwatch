//	PviCom_supp.h : supplement header file for PviCom.dll
//	Author: DLL to Lib version 1.42
//	Date: Saturday, February 11, 2006
//	Description: The declaration of the PviCom.dll's entry-point function.
//	Prototype: BOOL WINAPI xxx_DllMain(HINSTANCE hinstance, DWORD fdwReason, LPVOID lpvReserved);
//	Parameters: 
//		hinstance
//		  Handle to current instance of the application. Use AfxGetInstanceHandle()
//		  to get the instance handle if your project has MFC support.
//		fdwReason
//		  Specifies a flag indicating why the entry-point function is being called.
//		lpvReserved 
//		  Specifies further aspects of DLL initialization and cleanup. Should always
//		  be set to NULL;
//	Comment: Please see the help document for detail information about the entry-point 
//		 function
//	Homepage: http://www.binary-soft.com
//	Technical Support: support@binary-soft.com
/////////////////////////////////////////////////////////////////////

#if !defined(D2L_PVICOM_SUPP_H__63BF2E5D_58A5_2FB3_53A3_68D23D536C48__INCLUDED_)
#define D2L_PVICOM_SUPP_H__63BF2E5D_58A5_2FB3_53A3_68D23D536C48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __cplusplus
extern "C" {
#endif


#include <windows.h>

/* This is PviCom.dll's entry-point function. You should call it to do necessary
 initialization and finalization. */

BOOL WINAPI PVICOM_DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);


#ifdef __cplusplus
}
#endif

#endif 
// !defined(D2L_PVICOM_SUPP_H__63BF2E5D_58A5_2FB3_53A3_68D23D536C48__INCLUDED_)

