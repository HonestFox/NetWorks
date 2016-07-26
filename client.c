#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//client

static void usage(const char *proc )
{
	printf("Usage: %s [ip] [port]\n", proc);
}

int main(int argc, char *argv[])
{
	if(argc != 3 )
	{
		usage(argv[0]);
		exit(1);
	}
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror("socket");
		return 2;
	}
	struct sockaddr_in remote;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(atoi(argv[2]));
	remote.sin_addr.s_addr = inet_addr(argv[1]);

	if( connect(sock, (struct sockaddr*)&remote, sizeof(remote) ) < 0)
	{
		perror("connect");
		return 3;
	}

	char buf[1024];
	while(1)
	{
		printf("Please Enter: ");
		fflush(stdout);
		ssize_t _s = read(0, buf, sizeof(buf) - 1);
	
		if(_s > 0)
		{
			buf[_s - 1] = '\0';
			write(sock, buf, strlen(buf));
			_s = read(sock, buf, sizeof(buf));
			if( _s > 0)
			{
				buf[_s] = '\0';
				printf("%s\n", buf);
			}
		}
	}
	return 0;
}
