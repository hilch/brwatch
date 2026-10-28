// cpusearch.cpp
//
#define WIN32_LEAN_AND_MEAN /* speed up compilations */
#include <winsock2.h>
#include <iphlpapi.h>
#include "main.h"
#include "cpusearch.h"
#include <time.h>

struct stAdapterInfo
{
	char ipAddress[20];
	char subnetMask[20];
	char description[40];
};

int SearchCpuViaSnmp(struct stEthernetCpuInfo *ethernetCpuInfo, int maxEntries);
int SearchCpuViaPortScan(struct stAdapterInfo *adapter, struct stEthernetCpuInfo *ethernetCpuInfo, int maxEntries);
int ListNetworkAdapters(struct stAdapterInfo *adapterInfo, int maxEntries);

/* -----------------------------------------------------------------------------------------
	start searching CPUs via TCP port 11169
	----------------------------------------------------------------------------------------
*/
int SearchCpuViaPortScan(struct stAdapterInfo *adapter, struct stEthernetCpuInfo *ethernetCpuInfo, int maxEntries)
{
	WSADATA wsa;
	int entries = 0;

	if (adapter == NULL || ethernetCpuInfo == NULL || maxEntries <= 0)
		return 0;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	unsigned long localIp = ntohl(inet_addr(adapter->ipAddress));
	unsigned long subnetMask = ntohl(inet_addr(adapter->subnetMask));
	unsigned long network = localIp & subnetMask;
	unsigned long broadcast = network | ~subnetMask;
	unsigned long scanNetwork = localIp & 0xFFFFFF00UL;

	for (unsigned host = 1; host < 255 && entries < maxEntries;)
	{
		SOCKET sockets[32];
		IN_ADDR addresses[32];
		int pending = 0;

		while (host < 255 && pending < 32 && entries < maxEntries)
		{
			unsigned long targetIp = scanNetwork | host++;
			if (targetIp == localIp || targetIp <= network || targetIp >= broadcast ||
				(targetIp & subnetMask) != network)
				continue;

			SOCKET connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (connection == INVALID_SOCKET)
				continue;

			u_long nonblocking = 1;
			SOCKADDR_IN localAddress = {};
			localAddress.sin_family = AF_INET;
			localAddress.sin_addr.s_addr = htonl(localIp);
			if (bind(connection, (SOCKADDR *)&localAddress, sizeof(localAddress)) == SOCKET_ERROR ||
				ioctlsocket(connection, FIONBIO, &nonblocking) == SOCKET_ERROR)
			{
				closesocket(connection);
				continue;
			}

			SOCKADDR_IN targetAddress = {};
			targetAddress.sin_family = AF_INET;
			targetAddress.sin_port = htons(11169);
			targetAddress.sin_addr.s_addr = htonl(targetIp);
			int result = connect(connection, (SOCKADDR *)&targetAddress, sizeof(targetAddress));
			if (result == 0)
			{
				memset(&ethernetCpuInfo[entries], 0, sizeof(ethernetCpuInfo[entries]));
				strcpy(ethernetCpuInfo[entries].ipAddress, inet_ntoa(targetAddress.sin_addr));
				strcpy(ethernetCpuInfo[entries].subnetMask, adapter->subnetMask);
				++entries;
				closesocket(connection);
			}
			else if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				sockets[pending] = connection;
				addresses[pending++] = targetAddress.sin_addr;
			}
			else
			{
				closesocket(connection);
			}
		}

		DWORD started = GetTickCount();
		while (pending > 0 && entries < maxEntries)
		{
			DWORD elapsed = GetTickCount() - started;
			if (elapsed >= 300)
				break;

			fd_set writable, failed;
			FD_ZERO(&writable);
			FD_ZERO(&failed);
			for (int index = 0; index < pending; ++index)
			{
				FD_SET(sockets[index], &writable);
				FD_SET(sockets[index], &failed);
			}
			struct timeval timeout = {0, static_cast<long>((300 - elapsed) * 1000)};
			if (select(0, NULL, &writable, &failed, &timeout) <= 0)
				break;

			for (int index = pending - 1; index >= 0 && entries < maxEntries; --index)
			{
				if (!FD_ISSET(sockets[index], &writable) && !FD_ISSET(sockets[index], &failed))
					continue;
				int error = 0;
				int errorSize = sizeof(error);
				if (getsockopt(sockets[index], SOL_SOCKET, SO_ERROR, (char *)&error, &errorSize) == 0 &&
					error == 0 && FD_ISSET(sockets[index], &writable))
				{
					memset(&ethernetCpuInfo[entries], 0, sizeof(ethernetCpuInfo[entries]));
					strcpy(ethernetCpuInfo[entries].ipAddress, inet_ntoa(addresses[index]));
					strcpy(ethernetCpuInfo[entries].subnetMask, adapter->subnetMask);
					++entries;
				}
				closesocket(sockets[index]);
				--pending;
				sockets[index] = sockets[pending];
				addresses[index] = addresses[pending];
			}
		}
		for (int index = 0; index < pending; ++index)
			closesocket(sockets[index]);
	}

	WSACleanup();
	return entries;
}

/* -----------------------------------------------------------------------------------------
	get a list of available ethernet adapters
	----------------------------------------------------------------------------------------
*/
int ListNetworkAdapters(struct stAdapterInfo *adapterInfo, int maxEntries)
{
	DWORD result;
	IP_ADAPTER_INFO *buffer;
	DWORD bufsize;
	IP_ADAPTER_INFO *next;
	IP_ADDR_STRING *ipaddr;

	int entry = 0;

	bufsize = 20 * sizeof(IP_ADAPTER_INFO);
	buffer = (IP_ADAPTER_INFO *)malloc(bufsize);
	next = (IP_ADAPTER_INFO *)buffer;
	if (buffer == NULL)
		return 0;

	result = GetAdaptersInfo(buffer, &bufsize);
	if (result == ERROR_SUCCESS)
	{
		//        if( maxEntries )
		//        {
		//            strcpy( adapterInfo->ipAddress, "127.0.0.1" );
		//            strcpy( adapterInfo->subnetMask, "255.255.255.0" );
		//            strcpy( adapterInfo->description, "Loopback-Adapter" );
		//            ++entry;
		//            ++adapterInfo;
		//        }

		while ((next != NULL) && (entry < maxEntries))
		{
			ipaddr = &next->IpAddressList;

			while (next->Type == MIB_IF_TYPE_ETHERNET && ipaddr != NULL)
			{
				if (strcmp(ipaddr->IpAddress.String, "0.0.0.0"))
				{
					strncpy(adapterInfo->ipAddress, ipaddr->IpAddress.String, sizeof(adapterInfo[0].ipAddress));
					strncpy(adapterInfo->subnetMask, ipaddr->IpMask.String, sizeof(adapterInfo[0].subnetMask));
					strncpy(adapterInfo->description, next->Description, sizeof(adapterInfo[0].description));
					++entry;
					++adapterInfo;
				}
				ipaddr = ipaddr->Next;
			}
			next = next->Next;
		}
	}
	free(buffer);
	return entry;
}

/* -----------------------------------------------------------------------------------------
	PVI Callback for SNMP line
	----------------------------------------------------------------------------------------
*/
static void WINAPI PviSnmpProc(WPARAM /* wParam */, LPARAM /* lParam */, LPVOID /* pData */, DWORD /* DataLen */, T_RESPONSE_INFO * /*pInfo*/) {

};

/* -----------------------------------------------------------------------------------------
	start searching CPUs via SNMP protocol
	----------------------------------------------------------------------------------------
*/
int SearchCpuViaSnmp(struct stEthernetCpuInfo *ethernetCpuInfo, int maxEntries)
{

	int entries = 0;
	DWORD result;
	DWORD linkIdSnmpLine;
	DWORD linkIdDevice;
	LPVOID pviSnmpCallback = reinterpret_cast<LPVOID>(PviSnmpProc);

	memset(ethernetCpuInfo, 0, sizeof(struct stEthernetCpuInfo) * maxEntries);

	result = PviCreate(&linkIdSnmpLine, "@Pvi/LnSNMP", POBJ_LINE, "CD=\"LNSNMP\"", pviSnmpCallback, SET_PVICALLBACK_DATA, 0, "Ev=eds");
	if (result == 0)
	{
		result = PviCreate(&linkIdDevice, "@Pvi/LnSNMP/Device", POBJ_DEVICE, "CD=\"/IF=snmp /RT=3000\"", pviSnmpCallback, SET_PVICALLBACK_DATA, 0, "Ev=eds");
		if (result == 0)
		{
			char *buffer = (char *)malloc(65536);
			if (buffer != NULL)
			{
				result = PviRead(linkIdDevice, POBJ_ACC_LIST_EXTERN, NULL, 0, buffer, 65536);
				if (result == 0)
				{ /* get list of MAC addresses */
					char delimiter[] = "\t";
					char *ptr = strtok(buffer, delimiter);
					while (ptr != NULL && (entries < maxEntries))
					{
						if (strstr(ptr, KWDESC_OBJTYPE "=" KWOBJTYPE_STATION))
						{
							DWORD linkIdStation;
							char description[50];

							strncpy(ethernetCpuInfo->macAddress, ptr, 17); /* MAC address */
							strcpy(description, "CD=\"/CN=");
							strcat(description, ethernetCpuInfo->macAddress);
							strcat(description, "\"");

							result = PviCreate(&linkIdStation, "@Pvi/LnSNMP/Device/Station", POBJ_STATION, description, pviSnmpCallback, SET_PVICALLBACK_DATA, 0, "Ev=eds");
							if (result == 0)
							{
								DWORD linkIdPvar;
								result = PviCreate(&linkIdPvar, "@Pvi/LnSNMP/Device/Station/Ip", POBJ_PVAR, "CD=ipAddress", pviSnmpCallback, SET_PVICALLBACK_DATA, 0, "Ev=eds");
								PviRead(linkIdPvar, POBJ_ACC_DATA, NULL, 0, ethernetCpuInfo->ipAddress, sizeof(ethernetCpuInfo->ipAddress));
								PviUnlink(linkIdPvar);
								result = PviCreate(&linkIdPvar, "@Pvi/LnSNMP/Device/Station/Subnet", POBJ_PVAR, "CD=subnetMask", pviSnmpCallback, SET_PVICALLBACK_DATA, 0, "Ev=eds");
								PviRead(linkIdPvar, POBJ_ACC_DATA, NULL, 0, ethernetCpuInfo->subnetMask, sizeof(ethernetCpuInfo->subnetMask));
								PviUnlink(linkIdPvar);
								result = PviCreate(&linkIdPvar, "@Pvi/LnSNMP/Device/Station/Gateway", POBJ_PVAR, "CD=defaultGateway", pviSnmpCallback, SET_PVICALLBACK_DATA, 0, "Ev=eds");
								PviRead(linkIdPvar, POBJ_ACC_DATA, NULL, 0, ethernetCpuInfo->gateway, sizeof(ethernetCpuInfo->gateway));
								PviUnlink(linkIdPvar);
								result = PviCreate(&linkIdPvar, "@Pvi/LnSNMP/Device/Station/snmpMode", POBJ_PVAR, "CD=snmpMode", pviSnmpCallback, SET_PVICALLBACK_DATA, 0, "Ev=eds");
								PviRead(linkIdPvar, POBJ_ACC_DATA, NULL, 0, (void *)&ethernetCpuInfo->SNMP_mode, sizeof(ethernetCpuInfo->SNMP_mode));
								PviUnlink(linkIdPvar);
								result = PviCreate(&linkIdPvar, "@Pvi/LnSNMP/Device/Station/inaActivated", POBJ_PVAR, "CD=inaActivated", pviSnmpCallback, SET_PVICALLBACK_DATA, 0, "Ev=eds");
								PviRead(linkIdPvar, POBJ_ACC_DATA, NULL, 0, (void *)&ethernetCpuInfo->INA_activated, sizeof(ethernetCpuInfo->INA_activated));
								PviUnlink(linkIdPvar);
								result = PviCreate(&linkIdPvar, "@Pvi/LnSNMP/Device/Station/inaNodeNumber", POBJ_PVAR, "CD=inaNodeNumber", pviSnmpCallback, SET_PVICALLBACK_DATA, 0, "Ev=eds");
								PviRead(linkIdPvar, POBJ_ACC_DATA, NULL, 0, (void *)&ethernetCpuInfo->INA_nodeNumber, sizeof(ethernetCpuInfo->INA_nodeNumber));
								PviUnlink(linkIdPvar);
								result = PviCreate(&linkIdPvar, "@Pvi/LnSNMP/Device/Station/inaPortNumber", POBJ_PVAR, "CD=inaPortNumber", pviSnmpCallback, SET_PVICALLBACK_DATA, 0, "Ev=eds");
								PviRead(linkIdPvar, POBJ_ACC_DATA, NULL, 0, (void *)&ethernetCpuInfo->INA_portNumber, sizeof(ethernetCpuInfo->INA_portNumber));
								PviUnlink(linkIdPvar);
								result = PviCreate(&linkIdPvar, "@Pvi/LnSNMP/Device/Station/ipMethod", POBJ_PVAR, "CD=ipMethod", pviSnmpCallback, SET_PVICALLBACK_DATA, 0, "Ev=eds");
								PviRead(linkIdPvar, POBJ_ACC_DATA, NULL, 0, (void *)&ethernetCpuInfo->ipMethod, sizeof(ethernetCpuInfo->ipMethod));
								PviUnlink(linkIdPvar);
								int arState;
								result = PviCreate(&linkIdPvar, "@Pvi/LnSNMP/Device/Station/arState", POBJ_PVAR, "CD=arState", pviSnmpCallback, SET_PVICALLBACK_DATA, 0, "Ev=eds");
								PviRead(linkIdPvar, POBJ_ACC_DATA, NULL, 0, (void *)&arState, sizeof(arState));
								PviUnlink(linkIdPvar);
								switch (arState)
								{
								case 1:
									strcpy(ethernetCpuInfo->arState, "BOOT");
									break;

								case 2:
									strcpy(ethernetCpuInfo->arState, "DIAG");
									break;

								case 3:
									strcpy(ethernetCpuInfo->arState, "SERVICE");
									break;

								case 4:
									strcpy(ethernetCpuInfo->arState, "RUN");
									break;

								default:
									strcpy(ethernetCpuInfo->arState, "(undefined)");
									break;
								}

								result = PviCreate(&linkIdPvar, "@Pvi/LnSNMP/Device/Station/targetTypeDescription", POBJ_PVAR, "CD=targetTypeDescription", pviSnmpCallback, SET_PVICALLBACK_DATA, 0, "Ev=eds");
								PviRead(linkIdPvar, POBJ_ACC_DATA, NULL, 0, (void *)&ethernetCpuInfo->targetTypeDescription, sizeof(ethernetCpuInfo->targetTypeDescription));
								PviUnlink(linkIdPvar);
								result = PviCreate(&linkIdPvar, "@Pvi/LnSNMP/Device/Station/arVersion", POBJ_PVAR, "CD=arVersion", pviSnmpCallback, SET_PVICALLBACK_DATA, 0, "Ev=eds");
								PviRead(linkIdPvar, POBJ_ACC_DATA, NULL, 0, (void *)&ethernetCpuInfo->arVersion, sizeof(ethernetCpuInfo->arVersion));
								PviUnlink(linkIdPvar);
								PviUnlink(linkIdStation);
							}
							++ethernetCpuInfo;
							++entries;
						}
						ptr = strtok(NULL, delimiter);
					}
				}
				free(buffer);
			}
			PviUnlink(linkIdDevice);
		}

		result = PviUnlink(linkIdSnmpLine);
	}

	return (entries);
}

/* -----------------------------------------------------------------------------------------
	start searching CPUs connected by Ethernet
	----------------------------------------------------------------------------------------
*/
int SearchEthernetCpus(struct stEthernetCpuInfo *ethernetCpuInfo, int maxEntries)
{
	int noOfCPUs = SearchCpuViaSnmp(ethernetCpuInfo, maxEntries);

	struct stAdapterInfo adapters[20];
	memset(adapters, 0, sizeof(adapters));

	int noOfEthernetAdapters = ListNetworkAdapters(adapters, 20);

	/* Also discover CPUs with SNMP disabled via TCP port 11169. */
	unsigned long buflen = 256 * sizeof(struct stEthernetCpuInfo);
	struct stEthernetCpuInfo *ethernetScanCpuInfo = (struct stEthernetCpuInfo *)malloc(buflen);
	if (ethernetScanCpuInfo != NULL)
	{
		for (int i = 0; i < noOfEthernetAdapters; ++i)
		{
			// printf("%s  %s   %s\n", adapters[i].ipAddress, adapters[i].subnetMask, adapters[i].description );
			memset(ethernetScanCpuInfo, 0, buflen);
			int result = SearchCpuViaPortScan(&adapters[i], ethernetScanCpuInfo, 256);
			if (result > 0)
			{
				for (int i = 0; i < result; ++i)
				{
#ifdef __DEBUG__
					printf("%s / %s \n", ethernetScanCpuInfo[i].ipAddress, ethernetScanCpuInfo[i].subnetMask);
#endif // __DEBUG__
					int found = 0;
					for (int n = 0; n < noOfCPUs; ++n)
					{ /* discard dublicates */
						if (strcmp(ethernetScanCpuInfo[i].ipAddress, ethernetCpuInfo[n].ipAddress) == 0)
						{
							found = 1;
							break;
						}
					}
					if (!found && (noOfCPUs < maxEntries))
					{
#ifdef __DEBUG__
						puts("SNMP disabled CPU found\n");
#endif
						memcpy(&ethernetCpuInfo[noOfCPUs], &ethernetScanCpuInfo[i], sizeof(struct stEthernetCpuInfo));
						++noOfCPUs;
					}
				}
			}
		}
		free(ethernetScanCpuInfo);
	}

	return (noOfCPUs);
}
