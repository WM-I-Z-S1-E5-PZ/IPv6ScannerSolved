// WinSock2 define section
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

// standart libraries
// here i have to include both Windows.h and WinSock2.h in order to use:
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>
#include <ctime>
//#include <fstream>
#include "IPv6Address.h"

// own classess
#include "IPv6Address.h"

// instead of make changes in the project settings
// add WinSock library here
#pragma comment(lib, "ws2_32.lib")

// all around socket definitions
#define PORT "80"
#define BUFLEN 1024

// To get IP addresses of exist pages:
// nslookup www.google.com 8.8.8.8
const char* szHost[] = { "::1", "2a00:1450:401b:808::2004", "2a03:2880:f116:83:face:b00c:0:25de", "2a03:2880:f216:e5:face:b00c:0:4420" };

// just namespace using
using std::cout;
using std::endl;

int myTestSolve(int iTmpResult, int myCnt);
int mySolve(int iTmpResult, char* tmpAddr, WSADATA &tmpWsaData);
void wrRprt(int bfSize, char* tmpAddr);
void wrTimeRprt(int tmpTime);

int main()
{
    srand(time_t(NULL));
    // calculate the execution time of program run
    clock_t tStart = clock();
	// Initializing WinSock2
	WSADATA wsaData;
	WORD mVersionRequested = MAKEWORD(2, 2);
	// The WSAStartup function is calles to initiate use of WS2_32.dll
	int iResult = WSAStartup(mVersionRequested, &wsaData);
	if (iResult != NO_ERROR)
	{
		cout << "WSAStartup failed: " << iResult << endl;
		return 1;
	}
	else
	{
		cout << "========================= _WinSock status_ ======================\n";
		cout << "|\t\t\tThe WinSock dll found!\t\t\t|\n" << "|\t\t\tThe status: " << wsaData.szSystemStatus << "\t\t\t|" << endl;
		cout << "=================================================================\n\n";
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		/* Tell the user that we could not find a usable WinSock DLL.*/
		printf("The dll do not support the Winsock version %u.%u!\n", LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
		WSACleanup();
		return 2;
	}
	else
	{
		cout << "========================= _System data_ =========================\n";
		cout << "|          The dll supports the Winsock version: " << (int)LOBYTE(wsaData.wVersion) << "." << (int)HIBYTE(wsaData.wVersion) << "            |" << endl;
		cout << "|          The highest version this dll can support: " << (int)LOBYTE(wsaData.wHighVersion) << "." << (int)HIBYTE(wsaData.wHighVersion) << "        |" << endl;
		cout << "=================================================================\n\n";
	}

    // TEST
    //for (size_t i = 0; i < 4; ++i)
    //{
    //    cout << szHost[i] << endl;
    //}

    //myTestSolve(iResult, 4);
    std::vector<string> myHosts;
    myHosts.push_back("::1");
    myHosts.push_back("2a00:1450:401b:808::2004");
    myHosts.push_back("2a03:2880:f116:83:face:b00c:0:25de");
    myHosts.push_back("2a03:2880:f216:e5:face:b00c:0:4420");

    cout << "Please input how many addresses you want to check: ";
    int tmpCnt = 0;
    std::cin >> tmpCnt;
    cout << endl;
    IPv6Address myIPv6Gen;
    for (int i = 0; i < tmpCnt; i++)
    {
        myHosts.push_back(myIPv6Gen.randomAddress());
    }

    // added existed addresses to check loop
    // www.youtube.com
    myHosts.push_back("2a00:1450:401b:805::200e");
    // www.bbc.com
    myHosts.push_back("2a04:4e42::81");
    // gmail.com
    myHosts.push_back("2a00:1450:400d:802::2005");


    for (auto tmp : myHosts)
    {
        char* myTmpAddr = new char[tmp.size()];
        tmp.copy(myTmpAddr, tmp.size() + 1);
        myTmpAddr[tmp.size()] = '\0';
        mySolve(iResult, myTmpAddr, wsaData);
    }
    // get the time of the program end
    int tEnd = (int)((clock() - tStart) / CLOCKS_PER_SEC);
    wrTimeRprt(tEnd);
    /*int rDay = 0, rHours = 0, rMin = 0, rSec = 0;
    rDay = tEnd / 86400;
    rHours = (tEnd - 86400 * rDay) / 3600;
    rMin = (tEnd - 86400 * rDay - 3600 * rHours) / 60;
    rSec = tEnd - 86400 * rDay - 3600 * rHours - 60 * rMin;
    cout << "Program execution time is: " << rDay << " days.\n" << rHours << " hours.\n" << rMin << " min.\n" << rSec << " sec.\n\n"; */
	return 0;
}

int myTestSolve(int iTmpResult, int myCnt)
{
    // Creating a socket (client)
    struct addrinfo* result = nullptr,
        * ptr = nullptr,
        hints;
    ZeroMemory(&hints, sizeof(hints));
    /*
        The address family was left unspecified (AF_UNSPEC),
        so the returned IP address could be either an IPv6 or IPv4 address for the server.

        If you want to connect using only IPv6 or IPv4,
        then the address family needs to be set to AF_INET6 for IPv6
        or AF_INET for IPv4 in the hints parameter.
        https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-client
    */
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

	for (int i = 0; i < myCnt; i++)
	{
        // Resolve the server address and port
        iTmpResult = getaddrinfo(szHost[i], PORT, &hints, &result);
        if (iTmpResult != 0)
        {
            cout << "getaddrinfo failed: " << iTmpResult << endl;
            WSACleanup();
            return 1;
        }
        SOCKET mySock = INVALID_SOCKET;
        // Attempt to connect to the first address returned by
        // the call to getaddrinfo
        ptr = result;
        // Create a SOCKET to connect to server
        mySock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        // Check for errors to ensure that the socket is a valid socket.
        if (mySock == INVALID_SOCKET)
        {
            cout << "Error at socket(): " << WSAGetLastError() << endl;
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }
        // Connect to server
        iTmpResult = connect(mySock, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iTmpResult == SOCKET_ERROR)
        {
            closesocket(mySock);
            mySock = INVALID_SOCKET;
        }
        freeaddrinfo(result);
        if (mySock == INVALID_SOCKET)
        {
            cout << "Unable to connect to server.\n";
            WSACleanup();
            return 1;
        }

        // Sending and Receiving Data
        int recvBufLen = BUFLEN;

        const char* sendBuf = "HEAD / HTTP/1.0\r\n\r\n";
        char recvBuf[BUFLEN];

        // Send an initial buffer
        iTmpResult = send(mySock, sendBuf, (int)strlen(sendBuf), 0);
        if (iTmpResult == SOCKET_ERROR)
        {
            cout << "Send failed: " << WSAGetLastError();
            closesocket(mySock);
            WSACleanup();
            return 1;
        }

        cout << "Testing \"" << szHost[i] << "\":\n";
        cout << "Bytes send: " << iTmpResult << endl;

        // Shutdown the connection for sending since no more data will be sent
        // the client can still use the ConnectSocket for receiving data
        iTmpResult = shutdown(mySock, SD_SEND);
        if (iTmpResult == SOCKET_ERROR)
        {
            cout << "Shutdown failed: " << WSAGetLastError() << endl;
            closesocket(mySock);
            WSACleanup();
            return 1;
        }

        // Receive the data until the server closes the connection
        do
        {
            iTmpResult = recv(mySock, recvBuf, recvBufLen, 0);
            if (iTmpResult > 0)
            {
                cout << "Bytes received: " << iTmpResult << endl;
            }
            else if (iTmpResult == 0)
            {
                cout << "Connection close.\n";
            }
            else
            {
                cout << "Receive failed: " << WSAGetLastError();
            }
        } while (iTmpResult > 0);

        // Disconnecting the Client
        // shutdown the send half of the connection since no more data will be sent
        iTmpResult = shutdown(mySock, SD_SEND);
        if (iTmpResult == SOCKET_ERROR) {
            cout << "Shutdown failed: " << WSAGetLastError();
            closesocket(mySock);
            WSACleanup();
            return 1;
        }
        closesocket(mySock);
        cout << endl;
	}
    // cleanup
    WSACleanup();
    return 0;
}

int mySolve(int iTmpResult, char* tmpAddr, WSADATA& tmpWsaData)
{
    iTmpResult = WSAStartup(MAKEWORD(2, 2), &tmpWsaData);
    if (iTmpResult != NO_ERROR)
    {
        cout << "WSAStartup failed: " << iTmpResult << endl;
        return 1;
    }
    // Creating a socket (client)
    struct addrinfo* result = nullptr,
        * ptr = nullptr,
        hints;
    ZeroMemory(&hints, sizeof(hints));
    /*
        The address family was left unspecified (AF_UNSPEC),
        so the returned IP address could be either an IPv6 or IPv4 address for the server.

        If you want to connect using only IPv6 or IPv4,
        then the address family needs to be set to AF_INET6 for IPv6
        or AF_INET for IPv4 in the hints parameter.
        https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-client
    */
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    cout << "Testing \"" << tmpAddr << "\":\n";

    // Resolve the server address and port
    iTmpResult = getaddrinfo(tmpAddr, PORT, &hints, &result);
    if (iTmpResult != 0)
    {
        cout << "getaddrinfo failed: " << iTmpResult << endl;
        WSACleanup();
        return 1;
    }
    SOCKET mySock = INVALID_SOCKET;
    // Attempt to connect to the first address returned by
    // the call to getaddrinfo
    ptr = result;
    // Create a SOCKET to connect to server
    mySock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    // Check for errors to ensure that the socket is a valid socket.
    if (mySock == INVALID_SOCKET)
    {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    // Connect to server
    iTmpResult = connect(mySock, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iTmpResult == SOCKET_ERROR)
    {
        closesocket(mySock);
        mySock = INVALID_SOCKET;
    }
    freeaddrinfo(result);
    if (mySock == INVALID_SOCKET)
    {
        cout << "Unable to connect to server.\n\n";
        WSACleanup();
        return 1;
    }

    // Sending and Receiving Data
    int recvBufLen = BUFLEN;

    const char* sendBuf = "HEAD / HTTP/1.0\r\n\r\n";
    char recvBuf[BUFLEN];

    // Send an initial buffer
    iTmpResult = send(mySock, sendBuf, (int)strlen(sendBuf), 0);
    if (iTmpResult == SOCKET_ERROR)
    {
        cout << "Send failed: " << WSAGetLastError();
        closesocket(mySock);
        WSACleanup();
        return 1;
    }

    //cout << "Testing \"" << tmpAddr << "\":\n";
    cout << "Bytes send: " << iTmpResult << endl;

    // Shutdown the connection for sending since no more data will be sent
    // the client can still use the ConnectSocket for receiving data
    iTmpResult = shutdown(mySock, SD_SEND);
    if (iTmpResult == SOCKET_ERROR)
    {
        cout << "Shutdown failed: " << WSAGetLastError() << endl;
        closesocket(mySock);
        WSACleanup();
        return 1;
    }

    // Receive the data until the server closes the connection
    do
    {
        iTmpResult = recv(mySock, recvBuf, recvBufLen, 0);
        if (iTmpResult > 250) wrRprt(iTmpResult, tmpAddr);
        if (iTmpResult > 0)
        {
            cout << "Bytes received: " << iTmpResult << endl;
        }
        else if (iTmpResult == 0)
        {
            cout << "Connection close.\n";
        }
        else
        {
            cout << "Receive failed: " << WSAGetLastError();
        }
    } while (iTmpResult > 0);

    // Disconnecting the Client
    // shutdown the send half of the connection since no more data will be sent
    iTmpResult = shutdown(mySock, SD_SEND);
    if (iTmpResult == SOCKET_ERROR) {
        cout << "Shutdown failed: " << WSAGetLastError();
        closesocket(mySock);
        WSACleanup();
        return 1;
    }
    closesocket(mySock);
    cout << endl;
    // cleanup
    //WSACleanup();
    return 0;
}

// simple method to write data to the file
void wrRprt(int bfSize, char* tmpAddr)
{
    // create file strem object
    std::fstream myRprt;
    // open file at the disk with arguments to write, read and edit
    myRprt.open("../Report/report.txt", std::fstream::in | std::fstream::out | std::fstream::app);
    // check is file open
    if (myRprt.is_open())
    {
        // parse data to file
        myRprt << "Can be used to DDoS attacks:\t" << tmpAddr << endl;
    }
    // close file
    myRprt.close();
}

void wrTimeRprt(int tmpTime)
{
    int rDay = 0, rHours = 0, rMin = 0, rSec = 0;
    rDay = tmpTime / 86400;
    rHours = (tmpTime - 86400 * rDay) / 3600;
    rMin = (tmpTime - 86400 * rDay - 3600 * rHours) / 60;
    rSec = tmpTime - 86400 * rDay - 3600 * rHours - 60 * rMin;
    cout << "Program execution time is: " << rDay << " days.\n" << rHours << " hours.\n" << rMin << " min.\n" << rSec << " sec.\n\n";
    // create file strem object
    std::fstream myRprt;
    // open file at the disk with arguments to write, read and edit
    myRprt.open("../Report/time.txt", std::fstream::in | std::fstream::out | std::fstream::app );
    // check is file open
    if (myRprt.is_open())
    {
        // parse data to file
        myRprt << "Program execution time is(dd:hh:mm:ss):\n"
            << rDay << ":"
            << rHours << ":"
            << rMin << ":"
            << rSec << endl;
    }
    // close file
    myRprt.close();
}