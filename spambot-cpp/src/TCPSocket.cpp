#include "TCPSocket.h"

TCPSocket::TCPSocket()
{
	WSADATA wsaData;

	WSAStartup(MAKEWORD(2,2), &wsaData);

	addrinfo hints{};
	SockInfo = &hints;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	getaddrinfo("irc.chat.twitch.tv", "6667", &hints, &SockInfo);

	Socket = INVALID_SOCKET;
	Socket = socket(SockInfo->ai_family, SockInfo->ai_socktype, SockInfo->ai_protocol);
}

TCPSocket::TCPSocket(string url, string port): TCPSocket()
{
	connect(url, port);
}

TCPSocket::~TCPSocket()
{
	freeaddrinfo(SockInfo);
	WSACleanup();
}

void TCPSocket::timeout(long int millis)
{
	setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char *) &millis, sizeof(millis));
}

int TCPSocket::connect(string url, string port)
{
	return ::connect(Socket, SockInfo->ai_addr, (int) SockInfo->ai_addrlen);
}

int TCPSocket::send(string message)
{
	return ::send(Socket, message.c_str(), (int) message.length(), 0);
}

string TCPSocket::receive()
{
	string received;

	do
	{
		auto byteCountReceived = recv(Socket, recvbuf, recvbuflen, 0);

		if(byteCountReceived < 1)
		{
			return "";
		}
		else if(byteCountReceived == recvbuflen)
		{
			recvbuf[recvbuflen-1] = '\0';
		}
		else
		{
			recvbuf[byteCountReceived] = '\0';
		}
		
		received.append(recvbuf);
	} while(WSAGetLastError() == WSAEMSGSIZE);

	return received;
}
