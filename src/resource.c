
#include <windows.h>
#include <commctrl.h>
#include <string.h>
#include "resource.h"
#include "main.h"
#include "pvi_interface.h"

static  WORD images[] = {
	IDR_ICO_DEVICE,
	IDR_ICO_CPU,
	IDR_ICO_TASK,
	IDR_ICO_VARIABLE,
	IDR_ICO_STRUCT,
	IDR_ICO_ARRAY,
	IDR_ICO_VARIABLE_U32,
	IDR_ICO_VARIABLE_I32,
	IDR_ICO_VARIABLE_U16,
	IDR_ICO_VARIABLE_I16,
	IDR_ICO_VARIABLE_U8,
	IDR_ICO_VARIABLE_I8,
	IDR_ICO_VARIABLE_BOOL,
	IDR_ICO_VARIABLE_REAL,
	IDR_ICO_VARIABLE_LREAL,	
	IDR_ICO_VARIABLE_STRING,
	IDR_ICO_VARIABLE_WSTRING,	
	IDR_ICO_VARIABLE_TIME,
	IDR_ICO_VARIABLE_DATE_AND_TIME,
	0
};




/* ============================================================================================ */

HIMAGELIST ResourcesCreateImageList(void) {
	HIMAGELIST himl;  // handle to image list
	HICON hicon;
	int i;

	// Imagelist erzeugen
	if ((himl = ImageList_Create( 16, 16, FALSE, sizeof(images)/sizeof(char*), 0)) == NULL)
		return NULL;

	// Images hinzufügen
	i = 0;
	while( images[i] != 0 ) {
		hicon = LoadImage( g_hInstance, MAKEINTRESOURCE(images[i]), IMAGE_ICON, 0,0, LR_DEFAULTCOLOR  | LR_LOADTRANSPARENT | LR_VGACOLOR  );
		ImageList_AddIcon( himl, hicon );
		DestroyIcon( hicon );
		++i;
	}

	return(himl);

}



int ResourcesGetImageIndex( WORD resource ) {
	int i = 0;

	while( images[i] != 0 ) {
		if( images[i] == resource )
			return i;
		++i;
	}
	return 0;
}


int ResourcesGetPviObjectImage( PVIOBJECT *object ) {
	int imageindex=0;

	if( object == NULL )
		return 0;

	switch( object->type ) {
		case POBJ_PVAR:
			if( object->ex.pv.dimension > 1 ) {
				imageindex = ResourcesGetImageIndex(IDR_ICO_ARRAY);
				break;
			}

			switch( object->ex.pv.type ) {
				case BR_STRUCT:
					imageindex = ResourcesGetImageIndex(IDR_ICO_STRUCT);
					break;
				case BR_UDINT:
					imageindex = ResourcesGetImageIndex(IDR_ICO_VARIABLE_U32);
					break;
				case BR_DINT:
					imageindex = ResourcesGetImageIndex(IDR_ICO_VARIABLE_I32);
					break;
				case BR_UINT:
					imageindex = ResourcesGetImageIndex(IDR_ICO_VARIABLE_U16);
					break;
				case BR_INT:
					imageindex = ResourcesGetImageIndex(IDR_ICO_VARIABLE_I16);
					break;
				case BR_USINT:
					imageindex = ResourcesGetImageIndex(IDR_ICO_VARIABLE_U8);
					break;
				case BR_SINT:
					imageindex = ResourcesGetImageIndex(IDR_ICO_VARIABLE_I8);
					break;
				case BR_BOOL:
					imageindex = ResourcesGetImageIndex(IDR_ICO_VARIABLE_BOOL);
					break;
				case BR_REAL:
					imageindex = ResourcesGetImageIndex(IDR_ICO_VARIABLE_REAL);
					break;				
				case BR_LREAL:
					imageindex = ResourcesGetImageIndex(IDR_ICO_VARIABLE_LREAL);
					break;									
				case BR_STRING:
					imageindex = ResourcesGetImageIndex(IDR_ICO_VARIABLE_STRING);
					break;
				case BR_WSTRING:
					imageindex = ResourcesGetImageIndex(IDR_ICO_VARIABLE_WSTRING);
					break;					
				case BR_TIME:
				case BR_TOD:
					imageindex = ResourcesGetImageIndex(IDR_ICO_VARIABLE_TIME);
					break;
				case BR_DATI:
				case BR_DATE:
					imageindex = ResourcesGetImageIndex(IDR_ICO_VARIABLE_DATE_AND_TIME);
					break;
				default:
					imageindex = ResourcesGetImageIndex(IDR_ICO_VARIABLE);
					break;
			}
			break;

		case POBJ_CPU:
			imageindex = ResourcesGetImageIndex(IDR_ICO_CPU);
			break;

		case POBJ_DEVICE:
			imageindex = ResourcesGetImageIndex(IDR_ICO_DEVICE);
			break;

		case POBJ_TASK:
			imageindex = ResourcesGetImageIndex(IDR_ICO_TASK);
			break;

		default:
			break;
	}
	return imageindex;
}

