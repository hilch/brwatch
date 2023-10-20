// cpusearch.cpp
//
#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include <winsock2.h>
#include <iphlpapi.h>
#include "main.h"
#include "cpusearch.h"
#include <time.h>

struct stAdapterInfo {
	char ipAddress[20];
	char subnetMask[20];
	char description[40];
};

int SearchCpuViaSnmp(struct stEthernetCpuInfo *ethernetCpuInfo, int maxEntries);
int SearchCpuViaUDP( struct stAdapterInfo *adapter, struct stEthernetCpuInfo *ethernetCpuInfo, int maxEntries );
int ListNetworkAdapters( struct stAdapterInfo *adapterInfo, int maxEntries );



/* -----------------------------------------------------------------------------------------
    start searching CPUs via UDP datagrams
    ----------------------------------------------------------------------------------------
*/
int SearchCpuViaUDP( struct stAdapterInfo *adapter, struct stEthernetCpuInfo *ethernetCpuInfo, int maxEntries ) {
	WSADATA wsa;
	SOCKET s;
	SOCKADDR_IN addr;
	SOCKADDR_IN remoteAddr;
	fd_set socket_set;

	int result;
	unsigned char requestDatagram[10];
	char receivedDatagram[256];

	struct timeval timeout;
	int size;
	int entries = 0;



	// start winsock
	result = WSAStartup(MAKEWORD(2,0),&wsa);

	if( result == 0 ) {

		// UDP socket
		s=socket(AF_INET,SOCK_DGRAM,0);

		if( s!= INVALID_SOCKET ) {

			requestDatagram[0] = 1;
			/* calculate broadcast address */
			unsigned long ipAddress, subnetMask, broadcast;
			ipAddress = inet_addr(adapter->ipAddress);
			subnetMask = inet_addr(adapter->subnetMask);
			broadcast = (ipAddress & subnetMask) | ~subnetMask;

			// enable socket for UDP broadcast datagrams
			setsockopt( s, SOL_SOCKET, SO_BROADCAST, (char*) requestDatagram, 1 );
			addr.sin_family=AF_INET;
			addr.sin_port=htons(11159);
			addr.sin_addr.S_un.S_addr = broadcast;


			for( unsigned char node = 1; (node < 255) && (entries < maxEntries) ; ++node ) {
				memset( requestDatagram, 0, sizeof(requestDatagram) );
				requestDatagram[0] = 2; // INA- Request
				requestDatagram[1] = 99; // Source- node
				requestDatagram[6] = node; // Dest- node

				result = sendto( s, (char*) requestDatagram, 8, 0, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
				if( result == SOCKET_ERROR ) {
					result = WSAGetLastError();
					printf( "\nWSAGetLastError: %i", result );
					break;
				}

				FD_ZERO( &socket_set );
				FD_SET( s, &socket_set );
				timeout.tv_sec = 0;
				timeout.tv_usec = 500;

				memset( receivedDatagram, 0, sizeof(receivedDatagram) );
				size = sizeof(SOCKADDR_IN);

				select( 0, &socket_set, NULL, NULL, &timeout );

				if(	FD_ISSET(s, &socket_set) ) {
					recvfrom(s,receivedDatagram,sizeof(receivedDatagram),0,(SOCKADDR*) &remoteAddr, &size );
					if( receivedDatagram[0] == 0x03 ) { // INA response buf[1] contains node number
						if( strcmp(inet_ntoa( remoteAddr.sin_addr ), adapter->ipAddress) ) { /* ignore adapter itself */
							strcpy( ethernetCpuInfo->ipAddress, inet_ntoa( remoteAddr.sin_addr ) );
							strcpy( ethernetCpuInfo->subnetMask, adapter->subnetMask );
							++ethernetCpuInfo;
							++entries;
						}
					}
				}
			}

		} else {
			return -2;  // can not create socket
		}

	} else {
		return -1; // can not start winsock
	}

//    do
//    {
//        Sleep(100);
//    }
//    while( !threadParameter.done );
	closesocket(s);
	WSACleanup();
	return( entries );
}


/* -----------------------------------------------------------------------------------------
    get a list of available ethernet adapters
    ----------------------------------------------------------------------------------------
*/
int ListNetworkAdapters( struct stAdapterInfo *adapterInfo, int maxEntries ) {
	DWORD result;
	IP_ADAPTER_INFO *buffer;
	DWORD bufsize;
	IP_ADAPTER_INFO *next;
	IP_ADDR_STRING *ipaddr;
	int i;
	int entry = 0;

	bufsize = 20 * sizeof(IP_ADAPTER_INFO);
	buffer = malloc( bufsize );
	next = (IP_ADAPTER_INFO*) buffer;
	if( buffer == NULL )
		return 0;



	result = GetAdaptersInfo( buffer, &bufsize );
	if( result == ERROR_SUCCESS ) {
//        if( maxEntries )
//        {
//            strcpy( adapterInfo->ipAddress, "127.0.0.1" );
//            strcpy( adapterInfo->subnetMask, "255.255.255.0" );
//            strcpy( adapterInfo->description, "Loopback-Adapter" );
//            ++entry;
//            ++adapterInfo;
//        }

		while( (next != NULL) && (entry < maxEntries) ) {
			ipaddr = &next->IpAddressList;

			while( next->Type == MIB_IF_TYPE_ETHERNET && ipaddr != NULL ) {
				if( strcmp(ipaddr->IpAddress.String,"0.0.0.0") ) {
					strncpy( adapterInfo->ipAddress, ipaddr->IpAddress.String, sizeof(adapterInfo[0].ipAddress) );
					strncpy( adapterInfo->subnetMask, ipaddr->IpMask.String, sizeof(adapterInfo[0].subnetMask) );
					strncpy( adapterInfo->description, next->Description, sizeof(adapterInfo[0].description) );
					++entry;
					++adapterInfo;
				}
				ipaddr = ipaddr->Next;
				++i;
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
static void WINAPI PviSnmpProc (WPARAM wParam, LPARAM lParam, LPVOID pData, DWORD DataLen, T_RESPONSE_INFO* pInfo) {

}

/* -----------------------------------------------------------------------------------------
    start searching CPUs via SNMP protocol
    ----------------------------------------------------------------------------------------
*/
int SearchCpuViaSnmp(struct stEthernetCpuInfo *ethernetCpuInfo, int maxEntries) {

	int entries = 0;
	DWORD result;
	DWORD linkIdSnmpLine;
	DWORD linkIdDevice;

	memset(ethernetCpuInfo, 0, sizeof(struct stEthernetCpuInfo) * maxEntries );


	result = PviCreate( &linkIdSnmpLine, "@Pvi/LnSNMP", POBJ_LINE, "CD=\"LNSNMP\"", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
	if( result == 0 ) {
		result = PviCreate( &linkIdDevice, "@Pvi/LnSNMP/Device", POBJ_DEVICE, "CD=\"/IF=snmp /RT=3000\"", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
		if( result == 0 ) {
			char *buffer = (char* ) malloc(65536);
			if( buffer != NULL ) {
				result = PviRead( linkIdDevice, POBJ_ACC_LIST_EXTERN, NULL, 0, buffer, 65536);
				if( result == 0 ) { /* get list of MAC addresses */
					char delimiter[]="\t";
					char* ptr = strtok(buffer, delimiter);
					while( ptr != NULL && (entries < maxEntries) ) {
						if( strstr(ptr, KWDESC_OBJTYPE "=" KWOBJTYPE_STATION) ) {
							DWORD linkIdStation;
							char description[50];

							strncpy( ethernetCpuInfo->macAddress, ptr, 17 ); /* MAC address */
							strcpy( description, "CD=\"/CN=");
							strcat( description, ethernetCpuInfo->macAddress );
							strcat( description, "\"");

							result = PviCreate( &linkIdStation, "@Pvi/LnSNMP/Device/Station", POBJ_STATION, description, PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
							if( result == 0 ) {
								DWORD linkIdPvar;
								result = PviCreate( &linkIdPvar, "@Pvi/LnSNMP/Device/Station/Ip", POBJ_PVAR, "CD=ipAddress", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
								PviRead( linkIdPvar, POBJ_ACC_DATA, NULL, 0, ethernetCpuInfo->ipAddress, sizeof(ethernetCpuInfo->ipAddress));
								PviUnlink( linkIdPvar);
								result = PviCreate( &linkIdPvar, "@Pvi/LnSNMP/Device/Station/Subnet", POBJ_PVAR, "CD=subnetMask", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
								PviRead( linkIdPvar, POBJ_ACC_DATA, NULL, 0, ethernetCpuInfo->subnetMask, sizeof(ethernetCpuInfo->subnetMask));
								PviUnlink( linkIdPvar);
								result = PviCreate( &linkIdPvar, "@Pvi/LnSNMP/Device/Station/inaActivated", POBJ_PVAR, "CD=inaActivated", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
								PviRead( linkIdPvar, POBJ_ACC_DATA, NULL, 0, (void*) &ethernetCpuInfo->INA_activated, sizeof(ethernetCpuInfo->INA_activated));
								PviUnlink( linkIdPvar);
								result = PviCreate( &linkIdPvar, "@Pvi/LnSNMP/Device/Station/inaNodeNumber", POBJ_PVAR, "CD=inaNodeNumber", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
								PviRead( linkIdPvar, POBJ_ACC_DATA, NULL, 0, (void*) &ethernetCpuInfo->INA_nodeNumber, sizeof(ethernetCpuInfo->INA_nodeNumber));
								PviUnlink( linkIdPvar);
								result = PviCreate( &linkIdPvar, "@Pvi/LnSNMP/Device/Station/inaPortNumber", POBJ_PVAR, "CD=inaPortNumber", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
								PviRead( linkIdPvar, POBJ_ACC_DATA, NULL, 0, (void*) &ethernetCpuInfo->INA_portNumber, sizeof(ethernetCpuInfo->INA_portNumber));
								PviUnlink( linkIdPvar);
								result = PviCreate( &linkIdPvar, "@Pvi/LnSNMP/Device/Station/ipMethod", POBJ_PVAR, "CD=ipMethod", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
								PviRead( linkIdPvar, POBJ_ACC_DATA, NULL, 0, (void*) &ethernetCpuInfo->ipMethod, sizeof(ethernetCpuInfo->ipMethod));
								PviUnlink( linkIdPvar);
								int arState;
								result = PviCreate( &linkIdPvar, "@Pvi/LnSNMP/Device/Station/arState", POBJ_PVAR, "CD=arState", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
								PviRead( linkIdPvar, POBJ_ACC_DATA, NULL, 0, (void*) &arState, sizeof(arState));
								PviUnlink( linkIdPvar);
								switch( arState ) {
									case 1:
										strcpy( ethernetCpuInfo->arState, "BOOT");
										break;

									case 2:
										strcpy( ethernetCpuInfo->arState, "DIAG");
										break;

									case 3:
										strcpy( ethernetCpuInfo->arState, "SERVICE");
										break;

									case 4:
										strcpy( ethernetCpuInfo->arState, "RUN");
										break;

									default:
										strcpy( ethernetCpuInfo->arState, "(undefined)");
										break;
								}

								result = PviCreate( &linkIdPvar, "@Pvi/LnSNMP/Device/Station/targetTypeDescription", POBJ_PVAR, "CD=targetTypeDescription", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
								PviRead( linkIdPvar, POBJ_ACC_DATA, NULL, 0, (void*) &ethernetCpuInfo->targetTypeDescription, sizeof(ethernetCpuInfo->targetTypeDescription));
								PviUnlink( linkIdPvar);
								result = PviCreate( &linkIdPvar, "@Pvi/LnSNMP/Device/Station/arVersion", POBJ_PVAR, "CD=arVersion", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
								PviRead( linkIdPvar, POBJ_ACC_DATA, NULL, 0, (void*) &ethernetCpuInfo->arVersion, sizeof(ethernetCpuInfo->arVersion));
								PviUnlink( linkIdPvar);
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
			PviUnlink( linkIdDevice );
		}

		result = PviUnlink(linkIdSnmpLine);
	}

	return( entries);
}

/* -----------------------------------------------------------------------------------------
    start searching CPUs connected by Ethernet
    ----------------------------------------------------------------------------------------
*/
int SearchEthernetCpus( struct stEthernetCpuInfo *ethernetCpuInfo, int maxEntries ) {
	int noOfCPUs = SearchCpuViaSnmp( ethernetCpuInfo, maxEntries);

	struct stAdapterInfo adapters[20];
	memset( adapters, 0, sizeof(adapters));

	int noOfEthernetAdapters = ListNetworkAdapters( adapters, 20 );

	/* as PVI 2.x and AR 2.x doesnt work with SNMP or SNMP could be disabled we serach also via UDP broadcast */
	unsigned long buflen = 256*sizeof(struct stEthernetCpuInfo);
	struct stEthernetCpuInfo *ethernetUdpCpuInfo = malloc(buflen);
	if( ethernetUdpCpuInfo != NULL  ) {
		for( int i = 0; i < noOfEthernetAdapters; ++i ) {
			//printf("%s  %s   %s\n", adapters[i].ipAddress, adapters[i].subnetMask, adapters[i].description );
			memset( ethernetUdpCpuInfo, 0, buflen );
			int result = SearchCpuViaUDP( &adapters[i], ethernetUdpCpuInfo, 256 );
			if( result > 0 ) {
				for( int i = 0; i < result; ++i ) {
#ifdef __DEBUG__
					printf( "%s / %s \n", ethernetUdpCpuInfo[i].ipAddress, ethernetUdpCpuInfo[i].subnetMask );
#endif // __DEBUG__
					int found = 0;
					for( int n = 0; n < noOfCPUs; ++n ) { /* discard dublicates */
						if( strcmp( ethernetUdpCpuInfo[i].ipAddress, ethernetCpuInfo[n].ipAddress) == 0 ) {
							found = 1;
							break;
						}
					}
					if( !found && (noOfCPUs < maxEntries) ) {
#ifdef __DEBUG__
						puts("SNMP disabled CPU found\n");
#endif
						memcpy( &ethernetCpuInfo[noOfCPUs], &ethernetUdpCpuInfo[i], sizeof(struct stEthernetCpuInfo) );
						++noOfCPUs;
					}
				}
			}
		}
		free(ethernetUdpCpuInfo);
	}

	return( noOfCPUs );
}


