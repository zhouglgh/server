#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#define RECV_SIZE 1024
#define PORT 8000
#define PROS_NUMBER 32

struct parameter
{
    int fd,acceptFD;
    struct sockaddr_in address;
};

void op(struct parameter* p);
int main()
{
    struct parameter *p =(struct parameter*)malloc(sizeof(struct parameter));

    p->fd = socket(AF_INET,SOCK_STREAM,0);
    p->address.sin_family = AF_INET;
    p->address.sin_port = htons(PORT);
    p->address.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(p->fd, (struct sockaddr*) &p->address, sizeof(p->address));
    listen(p->fd,5);
    printf("=======server is on.======\n");
    int pn=PROS_NUMBER;
    while(pn-- > 0)
    {
	    if(fork() == 0){
		    struct sockaddr_in client;
		    int client_length = sizeof(client);
		    while(1)
		    {
			    p->acceptFD = accept(p->fd,(struct sockaddr*)&client,(socklen_t *)(&client_length));

			    if(p->acceptFD == -1)
			    {
				    perror("acceptFD");
				    printf("%d",p->acceptFD);
			    }
			    op(p);
			    close(p->acceptFD);
		    }
	    }
    }
    close(p->fd);
    free(p);
    p=0;
    return 0;
}

void op(struct parameter* p)
{
	char *buf = malloc(RECV_SIZE);
	int rec_len= 1;
	while(1)
	{
		if((rec_len = recv(p->acceptFD, buf, RECV_SIZE, 0)) == -1) {
			return;
		}
		if(rec_len == 0) 
			break;
	}
	free(buf);
	buf=0;
}
