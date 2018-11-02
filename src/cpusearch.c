// cpusearch.cpp
//
#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include <winsock2.h>
#include <iphlpapi.h>
#include "main.h"

#include <process.h>

static volatile BOOL abort_server;
SOCKET s;
SOCKADDR_IN addr;
SOCKADDR_IN remoteAddr;
char *pbuf;
long maxbuflen;


static void ReceiveMessage( void * arg )
{
    FD_SET socket_set;
    char buf[256];
    int len;
    struct timeval timeout;
    char tempstring[100];
    int size;


    // hören, was von draussen kommt...
    //printf( "\nServer gestartet" );
    abort_server = FALSE;

    while( 1 )
    {
        FD_ZERO( &socket_set );
        FD_SET( s, &socket_set );
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        memset( buf, 0, sizeof(buf) );
        size = sizeof(SOCKADDR_IN);

        select( 0, &socket_set, NULL, NULL, &timeout );

        if(	FD_ISSET(s, &socket_set) )
        {
            recvfrom(s,buf,sizeof(buf),0,(SOCKADDR*) &remoteAddr, &size );
            if( buf[0] == 0x03 )  // Ina- Response buf[1] enthält Knotennummer
            {
                sprintf( tempstring, "%3.3u%s\t", buf[1], inet_ntoa( remoteAddr.sin_addr ) );
                len = strlen(tempstring);
                // ARP- Request fuer MAC senden

                if( maxbuflen >= len  )
                {
                    strcat( pbuf, tempstring );
                    pbuf += len;
                    maxbuflen -= len;
                }
            }
        }

        if( abort_server )
        {
            // Winsock stoppen
            WSACleanup();
            //printf( "\nServer gestoppt" );
            abort_server = FALSE;
            break;
        }
    }
}



int SearchBRCPU( unsigned char sourcenode, unsigned long broadcast, char *buffer, long buflen )
{
    WSADATA wsa;
    int result;
    unsigned char request[10];
    int node;

    // Winsock starten
    result = WSAStartup(MAKEWORD(2,0),&wsa);

    if( result == 0 )
    {

        // UDP- Socket
        s=socket(AF_INET,SOCK_DGRAM,0);

        if( s!= INVALID_SOCKET )
        {

            request[0] = 1;
            // Socket für Broadcasts freigeben
            setsockopt( s, SOL_SOCKET, SO_BROADCAST, (char*) request, 1 );


            // Server starten
            pbuf = buffer;
            maxbuflen = buflen;
            memset( buffer, 0, buflen );
            _beginthread( ReceiveMessage, 10000, NULL );

            // addr vorbereiten
            addr.sin_family=AF_INET;
            addr.sin_port=htons(11159);
            addr.sin_addr.S_un.S_addr = broadcast;

            for( node = 1; node < 150; ++node )
            {
                //printf( "\nKnoten %u", node );
                // Knoten ansprechen
                memset( request, 0, sizeof(request) );
                request[0] = 2; // INA- Request
                request[1] = sourcenode; // Source- node
                request[6] = node; // Dest- node

                result = sendto( s, (char*) request, 8, 0, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
                if( result == SOCKET_ERROR )
                {
                    result = WSAGetLastError();
                    printf( "\nWSAGetLastError: %i", result );
                    break;
                }
                //Sleep(100);
            }

        }
        else
        {
            return -2;  // Socket konnte nicht angelegt werden
        }

    }
    else
    {
        return -1; // Winsock konnte nicht gestartet werden
    }


    Sleep( 500 );
    abort_server = TRUE;
    while( abort_server == TRUE );
    return 0;

}


