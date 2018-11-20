
#ifndef CPUSEARCH_H
#define CPUSEARCH_H


int SearchEthernetCpus( struct stEthernetCpuInfo *ethernetCpuInfo , int maxEntries );
int SearchCpuViaSnmp(struct stEthernetCpuInfo *ethernetCpuInfo, int maxEntries);

#endif // CPUSEARCH_H



