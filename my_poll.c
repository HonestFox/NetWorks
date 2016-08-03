/*************************************************************************
	> File Name: my_poll.c
	> Author: HonestFox
	> Mail: zhweizhi@foxmail.com 
	> Created Time: Thu 28 Jul 2016 11:52:28 AM CST
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<poll.h>
#include<stdlib.h>
#include<string.h>


void my_poll()
{
	struct pollfd polls[1];
	polls[0].fd = 0;
	polls[0].events = POLLIN;
	polls[0].revents = 0;
	
	int _timeout = 2000;	//ms
	
	int done = 0;
	while(!done)
	{
		switch(poll(polls, 1, _timeout))
		{
			case 0:
				printf("timeout\n");
				break;
			case -1:
				perror("poll");
				break;
			default:
				{
					char buf[1024];
					memset(buf, '\0', sizeof(buf));
					read(0, buf, sizeof(buf) - 1);
				}
				break;
		}
	}
}

int main()
{
	return 0;
}
