
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
	#include <windows.h>
	WSADATA wsa = {0};
	#define socklen_t int
	#define sleep Sleep
#else
	#include <sys/socket.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <fcntl.h>
	#include <string.h>
	#include <errno.h>
	#define SOCKET int
#endif



int main(int argc, char *argv[])
{
	#ifdef WIN32
		WSAStartup(MAKEWORD(2, 2), &wsa);
	#endif
	
	SOCKET sock_in = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	SOCKET sock_out = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	#ifdef WIN32
		u_long x = 1;
		ioctlsocket(sock_in, FIONBIO, &x);
		const char mode = 1;
		setsockopt(sock_out, SOL_SOCKET, SO_BROADCAST, &mode, sizeof (mode));
	#else
		int x = fcntl(sock_in, F_GETFL, 0);
		fcntl(sock_in, F_SETFL, x | O_NONBLOCK);
		int mode = 1;
		setsockopt(sock_out, SOL_SOCKET, SO_BROADCAST, &mode, sizeof (int));
	#endif
	
	

	sockaddr_in sa_in = {0};
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = htons(1337);
	sa_in.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sock_in, (sockaddr *) &sa_in, sizeof (sockaddr_in));
	
	sockaddr_in sa_out = {0};
	sa_out.sin_family = AF_INET;
	sa_out.sin_port = htons(1337);
	sa_out.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	
	char buffer[1024];
	for (;;)
	{
		printf("#> ");
		gets(buffer);
		if (*buffer)
			sendto(sock_out, buffer, strlen(buffer) + 1, 0, (sockaddr *) &sa_out, sizeof(sockaddr_in));
			//printf(">>>>>%d<<<<<<", errno);		
		int ret = 1;
		while (ret != -1)
		{
			sockaddr_in sa_rcv = {0};
			socklen_t sa_rcv_len = sizeof (sockaddr_in);
			ret = recvfrom(sock_in, buffer, sizeof (buffer), 0, (sockaddr *) &sa_rcv, &sa_rcv_len);
			printf("%s\n", buffer);
			//sleep(10);
		}
	}
	
	return (EXIT_SUCCESS);
}
