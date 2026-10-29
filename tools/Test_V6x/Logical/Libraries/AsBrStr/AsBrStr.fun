                                                                      
{REDUND_CONTEXT} {REDUND_UNREPLICABLE} FUNCTION brsftoa : UINT		(*converts a REAL value into a character string*)
	VAR_INPUT
		value	:REAL;		(*REAL (FLOAT) value to be converted into a character string*)
		pString	:UDINT;		(*pointer to the destination character string*)
	END_VAR
END_FUNCTION
{REDUND_CONTEXT} {REDUND_UNREPLICABLE} FUNCTION brsatof : REAL		(*converts a character string into a REAL (FLOAT) value*)
	VAR_INPUT
		pString	:UDINT;		(*pointer to the character string to be converted*)
	END_VAR
END_FUNCTION
{REDUND_CONTEXT} {REDUND_UNREPLICABLE} FUNCTION brsatod : LREAL		(*converts a character string into a LREAL (DOUBLE) value*)
	VAR_INPUT
		pString	:UDINT;		(*pointer to the character string to be converted*)
	END_VAR
END_FUNCTION
{REDUND_CONTEXT} {REDUND_UNREPLICABLE} FUNCTION brsitoa : UINT		(*converts a DINT value into a character string*)
	VAR_INPUT
		value	:DINT;		(*numerical value to be converted into a string*)
		pString	:UDINT;		(*pointer to the destination character string*)
	END_VAR
END_FUNCTION
{REDUND_CONTEXT} {REDUND_UNREPLICABLE} FUNCTION brsatoi : DINT		(*converts a character string into a DINT value*)
	VAR_INPUT
		pString	:UDINT;		(*pointer to the character string to be converted*)
	END_VAR
END_FUNCTION
{REDUND_CONTEXT} {REDUND_UNREPLICABLE} FUNCTION brsmemset : UDINT	(*fills the memory area with specific values*)
	VAR_INPUT
		pDest	:UDINT;		(*pointer to the destination memory*)
		value	:USINT;		(*initialization value*)
		length	:UDINT;		(*number of bytes to be initialized*)
	END_VAR
END_FUNCTION
{REDUND_CONTEXT} {REDUND_UNREPLICABLE} FUNCTION brsmemcpy : UDINT	(*copies the memory area*)
	VAR_INPUT
		pDest	:UDINT;		(*pointer to the destination memory*)
		pSrc	:UDINT;		(*pointer to the data to be copied*)
		length	:UDINT;		(*size of bytes to be copied*)
	END_VAR
END_FUNCTION
{REDUND_CONTEXT} {REDUND_UNREPLICABLE} FUNCTION brsmemmove : UDINT	(*copies the memory area (even if the memory areas overlap)*)
	VAR_INPUT
		pDest	:UDINT;		(*pointer to the destination memory*)
		pSrc	:UDINT;		(*pointer to the data to be copied*)
		length	:UDINT;		(*size of bytes to be copied*)
	END_VAR
END_FUNCTION
{REDUND_CONTEXT} {REDUND_UNREPLICABLE} FUNCTION brsmemcmp : DINT	(*compares memory areas*)
	VAR_INPUT
		pMem1	:UDINT;		(*pointer to the memory area 1*)
		pMem2	:UDINT;		(*pointer to the memory area 2*)
		length	:UDINT;		(*size of bytes to be checked*)
	END_VAR
END_FUNCTION
{REDUND_CONTEXT} {REDUND_UNREPLICABLE} FUNCTION brsstrcat : UDINT	(*attaches character strings to one another*)
	VAR_INPUT
		pDest	:UDINT;		(*pointer to the destination character string*)
		pSrc	:UDINT;		(*pointer to the source character string*)
	END_VAR
END_FUNCTION
{REDUND_CONTEXT} {REDUND_UNREPLICABLE} FUNCTION brsstrlen : UDINT	(*provides the length of a character string*)
	VAR_INPUT
		pString	:UDINT;		(*pointer to the source character string*)
	END_VAR
END_FUNCTION
{REDUND_CONTEXT} {REDUND_UNREPLICABLE} FUNCTION brsstrcpy : UDINT	(*copies one character string into another*)
	VAR_INPUT
		pDest	:UDINT;		(*pointer to the destination character string*)
		pSrc	:UDINT;		(*pointer to the source character string*)
	END_VAR
END_FUNCTION
{REDUND_CONTEXT} {REDUND_UNREPLICABLE} FUNCTION brsstrcmp : DINT	(*compares two character strings*)
	VAR_INPUT
		pString1	:UDINT;	(*pointer to the first character string*)
		pString2	:UDINT;	(*pointer to the second character string*)
	END_VAR
END_FUNCTION

