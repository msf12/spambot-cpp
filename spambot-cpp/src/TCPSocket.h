#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

using std::string;

class TCPSocket
{
private:
	SOCKET Socket;
	addrinfo *SockInfo;
#define recvbuflen 1024
	//one extra char for '\0' in the case of a full recv buffer
	char recvbuf[recvbuflen+1];
public:
	TCPSocket();
	TCPSocket(string url, string port);
	~TCPSocket();

	void timeout(long int millis);
	int connect(string url, string port);
	int send(string message);
	string receive();
	string receiveUntil(char *terminator, size_t length);
};
