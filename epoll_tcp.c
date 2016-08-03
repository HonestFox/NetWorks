/*************************************************************************
> File Name: epoll_tcp.c
> Author: HonestFox
> Mail: zhweizhi@foxmail.com
> Created Time: Fri 29 Jul 2016 03:14:27 PM CST
************************************************************************/

#include<stdio.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h> 
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>

static void usage()
{
	printf("usage: ip: port\n");
}

static int set_nonblock(int sock)
{
	int fl = fcntl(sock, F_GETFL);
	return fcntl(sock, F_SETFL, fl | O_NONBLOCK);
}

static int startup(const char *_ip, const int _port)
{
	//Create Socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("socket");
		exit(2);
	}

	//Bind
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(_port);
	local.sin_addr.s_addr = inet_addr(_ip);

	if (bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
	{
		perror("bind");
		exit(3);
	}

	//Set Listen
	if (listen(sock, 5) < 0)
	{
		perror("listen");
		exit(5);
	}
	return sock;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		usage();
	}
	int listen_sock = startup(argv[1], atoi(argv[2]));

	int epfd = epoll_create(256);
	if (epfd < 0)
	{
		perror("epoll_create");
		exit(5);
	}

	struct epoll_event _ev;
	_ev.events = EPOLLIN;
	_ev.data.fd = listen_sock;

	epoll_ctl(epfd, EPOLL_CTL_ADD, listen_sock, &_ev);

	struct epoll_event _ready_ev[128];
	int _ready_evs = 128;
	int _timeout = 1000;
	int done = 0;
	int nums = 0;
	while (!done)
	{
		nums = epoll_wait(epfd, _ready_ev, _ready_evs, _timeout);
		switch (nums)
		{
		case -1:
			perror("epoll_wait");
			exit(6);
		case 0:
			printf("time out\n");
			break;
		default:
		{
			int i = 0;
			for (; i < nums; ++i)
			{
				int _fd = _ready_ev[i].data.fd;
				if (_fd == listen_sock && (_ready_ev[i].events & EPOLLIN))
				{
					printf("get a new client\n");
					struct sockaddr_in peer;
					socklen_t len = sizeof(peer);
					int new_sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
					//Get a New Link
					if (new_sock > 0)
					{
						printf("new sock > 0\n");
						printf("client info %s : %d\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
						_ev.events = EPOLLIN | EPOLLET; //ET
						_ev.data.fd = new_sock;
						set_nonblock(new_sock);
						epoll_ctl(epfd, EPOLL_CTL_ADD, new_sock, &_ev);
					}
				}
				else
				{
					if (_ready_ev[i].events & EPOLLIN)
					{
						char buf[1024];
						memset(buf, '\0', sizeof(buf));
						ssize_t _s = recv(_fd, buf, sizeof(buf) - 1, 0);
						//while()
						//{
						//	//read recv done
						//}
						if (_s > 0)
						{
							printf("client : %s\n", buf);
							//_ev.events = EPOLLOUT | EPOLLET; //ET
							//_ev.data.fd = _fd;
							epoll_ctl(epfd, EPOLL_CTL_MOD, _fd, &_ev);	//Mod
						}
						else if (_s == 0)	//Client Close
						{
							printf("client close...\n");
							close(_fd);
							epoll_ctl(epfd, EPOLL_CTL_DEL, _fd, NULL);
						}
						else
						{
							perror("recv");
							exit(5);
						}
					}
					else if (_ready_ev[i].events & EPOLLOUT)
					{
				//		//const char *msg = "HTTP/1.1 200 OK \r\n\r\n<h1>hello world <h1>";
						char buf[100] = "server : hello";
						send(_fd, buf, strlen(buf), 0);
				//		epoll_ctl(epfd, EPOLL_CTL_MOD, _fd, &_ev);	//Mod
				//		printf("flag1\n");
				//		close(_fd);
				//		epoll_ctl(epfd, EPOLL_CTL_DEL, _fd, NULL);
					}
				}
			}
		}
		break;
		}
	}
	return 0;
}
