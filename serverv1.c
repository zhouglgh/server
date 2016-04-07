#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#define THREADS 9
#define PORT 8000

struct parameter
{
    int fd;
    struct sockaddr_in address;
    pthread_t threads[THREADS];
};

void *threadFunction(struct parameter* p);
void itoa(unsigned n,char *msg);
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
    int i=0;
    int newT;
    while(i < THREADS)
    {
        newT = pthread_create(&p->threads[i],NULL,(void*)threadFunction,p);
        if(newT)
        {
            printf("main:pthread_create error!\n");
            break;
        }
        i++;
    }

    i--;
    while(i >= 0)
    {
        pthread_join(p->threads[i],NULL);
        i--;
    }

    close(p->fd);
    free(p);
    return 0;
}


void *threadFunction(struct parameter* p)
{
    struct sockaddr* clients;
    int acceptFD;int n;
    unsigned pid = syscall(SYS_gettid);
    char msg[10];
    char buf[64];
    itoa(pid,msg);
    while(1)
    {
        acceptFD = accept(p->fd,(struct sockaddr*)clients,NULL);
	int rec_len;
	if((rec_len = recv(p->fd, buf, 64,0)) == -1) {
		perror("recv error");
		exit(1);
	}
	printf("received %d bytes, and the content is:%s\n",rec_len,buf);

        //n = send(acceptFD,msg,strlen(msg),0);
	close(acceptFD);
    }
    pthread_exit(NULL);
}
void itoa(unsigned n,char *msg)
{
    int tt[10]={0};
    char rr[10]={0};
    int c=0;
    int i=0;
    while(n!=0)
    {
        c=n%10;
        n=n/10;
        tt[i] = c;
        i++;
    }
    int j=0;
    while(j < i)
    {
        rr[j] = ((tt[i-j-1] & 0xff) + 48);
        j++;
    }
    strcpy(msg,rr);
}
