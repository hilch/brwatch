/******************************************************************************

Ping.cpp:
---------

Autor:             c-worker.ch

Quellen:           Winsock Doku, MSDN, rfc791, rfc792,
                   http://www.iig.uni-freiburg.de/~mhartman/tcpip/icmp.html,
                   ..

Beschreibung:      Benutzt das ICMP Protokoll um die Erreichbarkeit eines Hosts zu prüfen



Allgemeines zu ICMP:
--------------------

Echo Request, Echo Reply
------------------------

Diese Meldungen dienen zur Überprüfung ob ein Ziel erreichbar und
aktiv ist. Der Sender des Echo Request wartet auf eine Echo Reply
Message, die er nur erhält, wenn der Zielrechner aktiv ist. Als ICMP
Typ wurde die Nummer 8 für den Request und 0 für den Echo Reply
definiert. Der Code ist in beiden Fällen auf 0 gesetzt. Außerdem ist ein
ICMP Echo Identifikator definiert, welcher vom Sender des
Datagramms erzeugt wird und zur Identifikation des Prozesses dient.
Der Empfänger schickt den Reply an diesen Port. Eine Echo Sequenz
Nummer wird zur fortlaufenden Numerierung des Datagramms
genutzt. Der Empfänger nutzt die Nummer bei der Antwort und
ermöglicht dem Sender des Echo Request die Überprüfung der
Richtigkeit der Antwort. Die ICMP Echo Daten enthalten einen Echo
String, der vom Empfänger an den Sender zurückgeschickt wird.


Destination Unreachable
-----------------------

Diese Nachricht wird an den Sender des Datagramms geschickt,
wenn ein Subnetzwerk oder Router den Zielrechner nicht erreichen
kann oder das Paket nicht ausgeliefert werden kann weil das Don´t
Fragment Bit gesetzt ist und das Paket für ein Netzwerk zu groß ist.
Die Nachricht wird vom Router generiert sofern es sich um ein nicht
erreichbares Netzwerk oder einen unerreichbaren Zielrechner handelt.
Sollte es sich jedoch um einen unerreichbaren Port handeln so schickt
diese Meldung der Zielrechner.

ICMP Typ

Zur Unterscheidung der einzelnen ICMP Meldungen wurden ICMP
Nummern definiert. Die Destination Unreachable Meldung hat die
Nummer 3.

ICMP Code

Der ICMP Code teilt dem Sender mit, weshalb sein Datagramm nicht
übermittelt werden konnte. Es sind die folgenden Destination
Unreachable Codes definiert:

0= Netz nicht erreichbar
1= Rechner nicht erreichbar
2= Protokoll nicht erreichbar
3= Port nicht erreichbar
4= Fragmentierung notwendig, jedoch nicht möglich wegen gesetztem DF Bit
5= Source Route nicht erreichbar

******************************************************************************/
//#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include "ping.h"

#define ICMP_ECHOREPLY                 0
#define ICMP_UNREACHABLE               3
#define ICMP_ECHO                      8

// 0 = Netz nicht erreichbar
#define ICMP_CODE_NETWORK_UNREACHABLE  0
// 1 = Rechner nicht erreichbar
#define ICMP_CODE_HOST_UNREACHABLE     1

// Minimalgrösse 8 Byte
#define ICMP_MIN            8

#define STATUS_FAILED       0xFFFF
#define DEF_PACKET_SIZE     32
#define MAX_PACKET          65000

/*
Der IP Header:
--------------

|  Byte 0       |   Byte 1      |  Byte 2       |  Byte 3       |

0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Version|  IHL  |Type of Service|          Total Length         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Identification        |Flags|      Fragment Offset    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Time to Live |    Protocol   |         Header Checksum       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                       Source Address                          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Destination Address                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

typedef struct
{
    unsigned int   h_len:4;          // Länge des Headers
    unsigned int   version:4;        // IP Version
    unsigned char  tos;              // Type of service
    unsigned short total_len;        // Gesamt länge des Pakets
    unsigned short ident;            // unique identifier
    unsigned short frag_and_flags;   // flags
    unsigned char  ttl;              // TTL
    unsigned char  proto;            // Protokoll (TCP, UDP etc)
    unsigned short checksum;         // IP Checksumme
    unsigned int   sourceIP;         // Source IP
    unsigned int   destIP;           // Ziel IP
} IP_HEADER;


/*
Der ICMP Header:
----------------

|  Byte 0       |   Byte 1      |  Byte 2       |  Byte 3       |

0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| Type          | Code          |     ICMP Header Prüfsumme     |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Identifikatior |Sequenz Nummer |                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                      Timestamp                                |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

*/

typedef struct
{
    char           i_type;    // Type
    char           i_code;    // Code
    unsigned short i_cksum;   // Prüfsumme
    unsigned short i_id;      // Identifikatior
    unsigned short i_seq;     // Sequenz Nummer
    unsigned long  timestamp; // Um die benötigte Zeit zu messen
} ICMP_HEADER;



long WinsockStartup(void);
void FillIcmpData(char* icmp_data, int datasize);
unsigned short checksum(unsigned short *buffer, int size);
long DecodeResponse(char *buf, int bytes, SOCKADDR_IN *from);


long Ping(unsigned long broadcast, char *buffer, long buflen )
{
    SOCKET sockRaw;
    SOCKADDR_IN addrDest;
    SOCKADDR_IN addrFrom;
    int addrFromLen = sizeof(addrFrom);
    //HOSTENT *ptrHostent;
    unsigned long datasize;
    int RecvTimeout = 50;
    //char *dest_ip;
    char *icmp_data;
    char *recvbuf;
    unsigned short seq_no = 0;
    int BytesReceived;
    unsigned long BytesSent;
    long result;
    char errstring[256];
    time_t starttime;




    if (WinsockStartup())
        return -1;



    sockRaw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockRaw == INVALID_SOCKET)
    {
        sprintf( errstring, "Error: Cannot create Socket: %u", WSAGetLastError());
        MessageBox(NULL, errstring, "Winsock-Error", MB_ICONERROR);
        WSACleanup();
        return SOCKET_ERROR;
    }

    result = setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, (char *)&RecvTimeout, sizeof(RecvTimeout));
    if (result == SOCKET_ERROR)
    {
        sprintf(errstring, "Error: Cannot set Recv Timeout: %u", WSAGetLastError());
        MessageBox(NULL, errstring, "Winsock-Error", MB_ICONERROR);
        WSACleanup();
        return SOCKET_ERROR;
    }


    addrDest.sin_addr.s_addr = broadcast;
    addrDest.sin_family = AF_INET;


    // Konvertiert eine Netzwerk Adresse (SOCKADDR_IN) in einen String im Punkt Format (x.x.x.x)
//	dest_ip = inet_ntoa(addrDest.sin_addr);

    datasize = DEF_PACKET_SIZE;

    datasize += sizeof(ICMP_HEADER);

    icmp_data = (char *)malloc(MAX_PACKET);
    recvbuf = (char *)malloc(MAX_PACKET + sizeof(IP_HEADER) + sizeof(ICMP_HEADER));

    if (!icmp_data || !recvbuf)
    {
        //    cout << "Error: Not enough Memory: " << GetLastError() << endl;
        free( icmp_data );
        free( recvbuf );
        sprintf( errstring, "Not enough Memory" );
        MessageBox(NULL, errstring, "Winsock-Error", MB_ICONERROR);
        WSACleanup();
        return -1;
    }

    FillIcmpData(icmp_data, datasize);

    ((ICMP_HEADER *)icmp_data)->i_cksum = 0;
    ((ICMP_HEADER *)icmp_data)->timestamp = GetTickCount();

    ((ICMP_HEADER *)icmp_data)->i_seq = seq_no++;
    ((ICMP_HEADER *)icmp_data)->i_cksum = checksum((unsigned short *)icmp_data, datasize);

    BytesSent = sendto(sockRaw, icmp_data, datasize, 0, (SOCKADDR *) & addrDest, sizeof(addrDest));
    if (BytesSent == SOCKET_ERROR)
    {
        // if (WSAGetLastError() == WSAETIMEDOUT) {
        // sprintf(errstring, "timed out\n");
        // MessageBox(NULL, errstring, "Winsock-Error", MB_ICONERROR);
        // continue;
        // }
        // sprintf(errstring, "sendto failed: %u", WSAGetLastError());
        // MessageBox(NULL, errstring, "Winsock-Error", MB_ICONERROR);
        // return -1;
        result = -1;
    }

    if (BytesSent < datasize)
    {
        //cout <<"Wrote " << BytesSent << " bytes" << endl;
        result = -1;
    }


    strcpy( buffer, "" );
    starttime = time( NULL );
    result = 0;



    while(1)
    {
        FD_SET socket_set;
        char fromaddr[80];
        int rc;
        struct timeval timeout;

        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        FD_ZERO( &socket_set );
        FD_SET( sockRaw, &socket_set );

        rc = select( 0, &socket_set, NULL, NULL, &timeout );

        if( rc == SOCKET_ERROR )
        {
            MessageBox( NULL, "select", "", MB_OK );
            break;
        }

        if(	FD_ISSET(sockRaw, &socket_set) )
        {
            BytesReceived = recvfrom(sockRaw, recvbuf, MAX_PACKET + sizeof(IP_HEADER) + sizeof(ICMP_HEADER), 0, (SOCKADDR *) & addrFrom, &addrFromLen);

            if( BytesReceived > 0)
            {
                memset( fromaddr, 0, sizeof(fromaddr) );
                strcpy( fromaddr, inet_ntoa(addrFrom.sin_addr) );
                strcat( fromaddr, "\t" );
                if( buflen > (long) strlen(fromaddr) )  // noch Platz im Buffer ?
                {
                    strcat( buffer, fromaddr );
                    buflen -= (int) strlen(fromaddr);
                }
            }
            continue;
        }

        // Beep( 400, 2 );
        // Beep( 0, 50 );

        if( (time(NULL) - starttime) > 1 )
            break;

    }

    buffer[ strlen(buffer) ] = 0; // letzten Trenner entfernen

    if( strlen(buffer) == 0 )
        result = -1;

    //result = DecodeResponse(recvbuf, BytesReceived, &addrFrom);


    free( icmp_data );
    free( recvbuf );

    WSACleanup();
    return result ;

}



/*
Die Antwort die wir empfangen ist ein IP Paket. Wir müssen nun den IP
Header decodieren um die ICMP Daten lesen zu können
*/

long DecodeResponse(char *buf, int bytes, SOCKADDR_IN *from)
{
    IP_HEADER   *IpHeader;
    ICMP_HEADER *IcmpHeader;
    unsigned short IpHeaderLen;

    IpHeader = (IP_HEADER*)buf;
    IpHeaderLen = IpHeader->h_len * 4 ; // number of 32-bit words *4 = bytes





    if (bytes  < IpHeaderLen + ICMP_MIN)
    {
        //###cout << "Too few bytes from " << inet_ntoa(from->sin_addr) << endl;
    }
    IcmpHeader = (ICMP_HEADER*)(buf + IpHeaderLen);

    if (IcmpHeader->i_type != ICMP_ECHOREPLY)
    {
        if (IcmpHeader->i_type == ICMP_UNREACHABLE)
        {
            //###cout << "Reply from " << inet_ntoa(from->sin_addr);
            if(IcmpHeader->i_code == ICMP_CODE_HOST_UNREACHABLE)
            {
                //###cout << ": Destination Host unreachable !" << endl;
                return -1;
            }
            if(IcmpHeader->i_code == ICMP_CODE_NETWORK_UNREACHABLE)
            {
                //###cout << ": Destination Network unreachable !" << endl;
                return -1;
            }
        }
        else
        {
            //###cout << "non-echo type " << (int)IcmpHeader->i_type <<" received" << endl;
            return -1;
        }
    }

    if (IcmpHeader->i_id != (unsigned short)GetCurrentProcessId())
    {
        //###cout << "someone else's packet!" << endl;
        return -1;
    }

    return 0;

    //###cout << bytes << " bytes from " << inet_ntoa(from->sin_addr);
    //###cout << " icmp_seq = " << IcmpHeader->i_seq;
    //###cout << " time: " << GetTickCount()-IcmpHeader->timestamp << " ms " << endl;
}


unsigned short checksum(unsigned short *buffer, int size)
{
    unsigned long cksum=0;
    while(size >1)
    {
        cksum+=*buffer++;
        size -=sizeof(unsigned short);
    }

    if(size)
    {
        cksum += *(unsigned char*)buffer;
    }

    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >>16);
    return (unsigned short)(~cksum);
}

/*
Hilfs Funktion um unseren ICMP Header zu füllen
*/
void FillIcmpData(char * icmp_data, int datasize)
{

    ICMP_HEADER *icmp_hdr;
    char *datapart;

    icmp_hdr = (ICMP_HEADER*)icmp_data;

    icmp_hdr->i_type = ICMP_ECHO;
    icmp_hdr->i_code = 0;
    icmp_hdr->i_id = (unsigned short)GetCurrentProcessId();
    icmp_hdr->i_cksum = 0;
    icmp_hdr->i_seq = 0;

    datapart = icmp_data + sizeof(ICMP_HEADER);
    // Den Buffer mit etwas füllen
    memset(datapart,'C', datasize - sizeof(ICMP_HEADER));
}



long WinsockStartup(void)
{
    long result;
    char errstring[256];

    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 1);

    result = WSAStartup(wVersionRequested, &wsaData);

    if (result != 0)
    {

        strcpy(errstring, "WSAStartup()");
        switch (result)
        {
        case WSASYSNOTREADY:
            strcat(errstring, ": WSASYSNOTREADY");
            break;

        case WSAVERNOTSUPPORTED:
            strcat(errstring, ": WSAVERNOTSUPPORTED");
            break;

        case WSAEINPROGRESS:
            strcat(errstring, ": WSAEINPROGRESS");
            break;

        case WSAEPROCLIM:
            strcat(errstring, ": WSAEPROCLIM");
            break;

        default:
            break;
        }

        MessageBox(NULL, errstring, "Winsock-Error", MB_ICONERROR);
    }

    return result;
}


