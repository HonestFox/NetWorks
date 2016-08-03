/*************************************************************************
	> File Name: TCP_select.c
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Thu 28 Jul 2016 04:49:50 PM CST
 ************************************************************************/
#include<stdio.h>
#include<unistd.h>
#include<sys/select.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<netinet/in.h>

#define _MAX_SIZE 100

int fd_arr[_MAX_SIZE];
int max_fd = 0;

void usage(char *str)
{
	printf("Usage: %s [IP]:[port]\n]", str);
	exit(1);
}

static void init_fd_arr()
{
	int i = 0;
	for(i = 0; i < _MAX_SIZE; ++i)
	{
		fd_arr[i] = -1;
	}
}

static int add_fd_arr(int fd)
{
	int i = 0;
	for(; i < _MAX_SIZE; ++i)
	{
		if(fd_arr[i] == -1)
		{
			fd_arr[i] = fd;
			return 0;
		}
	}
	return -1;
}

static int remove_fd_arr(int fd)
{
	printf("want to remove : %d\n", fd);
	int i = 0;
	for(; i < _MAX_SIZE; ++i)
	{
		if(fd_arr[i] == fd)
		{
			printf("remove : %d\n", fd);
			fd_arr[i] = -1;
			break;
		}
	}
	return 0;
}

static int reload_fd_set(fd_set *fd_set)
{
	int i = 0;
	for(; i < _MAX_SIZE; ++i)
	{
		if(fd_arr[i] != -1)
		{
			FD_SET(fd_arr[i], fd_set);
			if(fd_arr[i] > max_fd)
			{
				max_fd = fd_arr[i];
			}
		}
	}
	return 0;
}

static void print_msg(int i, char buf[])
{
	printf("fd : %d, msg : %s\n", i, buf);
}

int select_server(char *_ip, char *_port)
{
	struct sockaddr_in ser;
	struct sockaddr_in cli;
	fd_set fds;
	
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd == 0)
	{
		perror("create socket error");
		exit(2);
	}

	int tmp_val = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &tmp_val, sizeof(int));

	memset(&ser, '\0', sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(atoi(_port));
	ser.sin_addr.s_addr = inet_addr(_ip);

	if(bind(fd, (struct sockaddr*)&ser, sizeof(ser)) != 0)
	{
		perror("bind error");
		exit(3);
	}

	init_fd_arr();
	add_fd_arr(fd);
	FD_ZERO(&fds);

	if(listen(fd, 5) != 0)
	{
		perror("listen error");
		exit(4);
	}

	int done = 0;
	while(!done)
	{
		max_fd = 0;
		reload_fd_set(&fds);
		printf("max_fd : %d\n", max_fd);
		struct timeval timeout = {1, 1};  //no use yet
		switch(select(max_fd + 1, &fds, NULL, NULL, &timeout))
		{
			reload_fd_set(&fds);
			case -1:
			{
				perror("select error");
				exit(5);
			}
			case 0:
			{
				printf("timeout  .. .. ..\n");
				break;
			}
			default:
			{
				int index = 0;
				for(index = 0; index < _MAX_SIZE; ++index)
				{
					if(index == 0 && fd_arr[index] != -1 && FD_ISSET(fd_arr[index], &fds))
					{
						socklen_t len = sizeof(cli);
						memset(&cli, '\0', sizeof(cli));
						int new_fd = accept(fd, (struct sockaddr*)&cli, &len);
						printf("new : %d\n", new_fd);  //
						if(new_fd != -1)
						{
							printf("get a new client!\n");
							if(add_fd_arr(new_fd) == -1)
							{
								perror("fd arr is full, close new fd:\n");
							}
						}
						continue;
					}
					if(fd_arr[index] != -1 && FD_ISSET(fd_arr[index], &fds ))
					{
						char buf[1042];
						memset(buf, '\0', sizeof(buf));
						printf("flag3\n");
						ssize_t _size = recv(fd_arr[index], buf, sizeof(buf)-1, 0); //read
						if(_size == 0 || _size == -1)
						{
							printf("client close\n");
							remove_fd_arr(fd_arr[index]);
							close(fd_arr[index]);
							FD_CLR(fd_arr[index], &fds);
						}
						else
						{
							print_msg(index, buf);
						}
						FD_ZERO(&fds);
					}
				}
				printf("out for()\n");
			}
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		usage(argv[0]);
		exit(1);
	}
	select_server(argv[1], argv[2]);
	return 0;
}




//
//void GetServerSocket(char *_ip, char *_port)
//{
//	int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
//	if(listnen_sock < 0)
//	{
//		perror("socket");
//		exit(2);
//	}
//	struct sockaddr_in local;
//	local.sin_family = AF_INET;
//	local.sin_port = htons(atoi(_port));
//	local.sin_addr.s_addr = inet_addr(_ip);
//
//	//bind
//	if(bind(listen_sock), (struct sockaddr*)&local, sizeof(local))
//	{
//		perror("bind");
//		exit(3);
//	}
//
//	//always listen
//	if( listen(listen_sock, 5) < 0)
//	{
//		perror("listen");
//		eixt(4);
//	}
//
//	return listen_sock;
//}
//
//
//int main(int argc, char*argv[])
//{
//	if(argc != 3)
//	{
//		usage(argv[0]);
//		exit(1);
//	}
//
//	int listen_sock = GetServerSocket(argv[1], argv[2]);
//	
//	
//
//
//	int done = 0;
//	while(!done)
//	{
//		
//	}
//	
//	return 0;
//}
