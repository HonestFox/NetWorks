#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//server

static void usage(const char *proc )
{
	printf("Usage: %s [ip] [port]\n", proc);
}

void* thread_run(void *arg)
{
	printf("create a new thread\n");
	int fd = (int)arg;
	char buf[1024];
	while(1)
	{
		memset(buf, '\0', sizeof(buf));
		ssize_t _s = read(fd, buf, sizeof(buf) - 1 );
		if( _s > 0 )
		{
			buf[_s] = '\0';
			printf("client :# %s\n", buf);

			memset(buf, '\0', sizeof(buf));
			printf("please enter: ");
			fflush(stdout);
			_s = read(0, buf, sizeof(buf) - 1);
			if(_s > 0)
			{
				buf[_s - 1] = '\0';
				write(fd, buf, strlen(buf) );
			}
		}
		else if(_s == 0)
		{
			printf("client close...\n");
			break;
		}
		else
		{
			printf("read error...\n");
			break;
		}
	}
}

int main(int argc, char *argv[] )
{
	if(argc != 3 )
	{
		usage(argv[0]);
		exit(1);
	}
	

	//1 creat socket
	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_sock < 0)
	{
		perror("socket");
		return 2;
	}

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(atoi(argv[2]));  //argv[2]
	local.sin_addr.s_addr = inet_addr(argv[1]);
	//_addr = inet_addr(atoi(argv[1]));  //argv[1]


	//2 bind
	if( bind(listen_sock, (struct sockaddr*)&local, sizeof(local) ) < 0)
	{
		perror("bind");
		return 3;
	}

	//3 always listen
	listen(listen_sock, 5);

	//4 accept
	struct sockaddr_in peer;
	socklen_t len = sizeof(peer);
	while(1)
	{
		int fd = accept(listen_sock, (struct sockaddr*)&peer, &len);
		if(fd < 0)
		{
			perror("accept");
			return 4;
		}

		printf("get a new link... socket -> %s : %d\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
		
		pthread_t id;
		pthread_create(&id, NULL, thread_run, (void*)fd);
		pthread_detach(id);
		


	//	pid_t id = fork();
	//	if(id == 0) //child
	//	{
	//		char buf[1024];
	//		while(1)
	//		{
	//			memset(buf, '\0', sizeof(buf));
	//			ssize_t _s = read(fd, buf, sizeof(buf) - 1 );
	//			if( _s > 0 )
	//			{
	//				buf[_s] = '\0';
	//				printf("client :# %s\n", buf);
	//				write(fd, buf, strlen(buf) );
	//			}
	//			else
	//			{
	//				printf("read done...\n");
	//				break;
	//			}
	//		}
	//		exit(0);
	//	}
	//	else	//father
	//	{
	//		waitpid(-1, NULL, WNOHANG);	
	//	}
	}
	return 0;
}
