
struct stEthernetCpuInfo {
    char macAddress[20];
    char ipAddress[20];
    char subnetMask[20];
};
int SearchEthernetCpus( struct stEthernetCpuInfo *ethernetCpuInfo , int maxEntries );





