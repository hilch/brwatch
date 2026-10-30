
#include "cpusearch.h"

#ifndef DLG_EDITCPU_H
#define DLG_EDIT_CPU_H

void DlgEditCpuShowDialog( PviObject *object );
PviObject *DlgAddCpuShowDialog(void);
int SetCPUIpParameters( struct stEthernetCpuInfo *info );
#endif // DLG_EDITCPU_H
