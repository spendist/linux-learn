// dev-cpp 与 LINUX 编译
#ifndef _SERVERSOCKET_H
	#define _SERVERSOCKET_H
	#include <sys/types.h>
	#ifdef WIN32
		#include <winsock2.h>
		#pragma comment(lib,"ws2_32.lib")
	#else
		##include <sys/socket.h>
	#endif

#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TRUE      1
#define FALSE     0

#define BUFSIZE      1024
#define PORT         2011

extern int errno;
int sock,flog,new_fd,sin_size;

void  signalfunction(int num);

void signalfunction(int num)
{
	fprintf(stderr,"get signal %d killed! exit\n",num);
	close(new_fd);
	close(sock); exit(0);
}

main()
{
  
	int len, clen,x,keepalive;
	struct sockaddr_in saddr;  // Server addr
	struct sockaddr_in caddr;   // Client addr
	struct tm *pt;
	long   timestamp;
	char   msgsend[BUFSIZE];  // send to clent data
	char   msgread[BUFSIZE],strDT[20];  // recv message;

	signal(SIGHUP, signalfunction);
	signal(SIGINT, signalfunction);
	if (fork()==-1) {
		fprintf(stderr,"fork(): %s\n",strerror(errno));
		exit(errno);
	}
	setsid();
    if ((flog=open("/var/log/rfc.log",O_CREAT|O_APPEND|O_WRONLY,0644))==-1) {
       perror("/var/log/rfc.log"); exit(-1);
    }
    close(1); dup(flog);
    close(2); dup(flog);
    close(0); close(flog);

	fprintf(stdout,"program start!!!\n");
    clen=sizeof(caddr);
	keepalive=1;
	// create socket
    if ((sock=socket(AF_INET,SOCK_STREAM,0))<0) { fprintf(stderr,"create: %s\n",strerror(errno)); exit(-1); }
	// init socket
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(PORT); // 服务端口 
	saddr.sin_addr.s_addr=INADDR_ANY;
	bzero(&(saddr.sin_zero),8);
	// 重用端口设置
	if (setsockopt(sock,SOL_SOCKET,SO_KEEPALIVE,(char *)&keepalive,sizeof(int))!=0) {
		fprintf(stderr,"Set socket keep alive error!\n");
		exit(errno);
	}
	// bind socket
    if (bind(sock,(struct sockaddr *)&saddr,sizeof(struct sockaddr))==-1) { fprintf(stderr,"bind: %s\n",strerror(errno)); exit(errno); }
	// create listen port
	if (listen(sock,20)==-1) { fprintf(stderr,"Listen: %s",strerror(errno)); exit(-1); }
	while(1) {
		signal(SIGHUP, signalfunction);
		signal(SIGINT, signalfunction);
		sin_size=sizeof(struct sockaddr_in);
		fprintf(stdout,"Server is running...\n");
		// if connected, create a new socket!
		if ((new_fd=accept(sock,(struct sockaddr *)&caddr,&sin_size))==-1) { 
			fprintf(stderr,"accept"); 
			exit(errno); 
		} else {
			fprintf(stdout,"accept success.\n");
		}
		// read data from client
		int numbytes;
		char msgread[512],msgsend[512];
		memset(msgread,0,512);memset(msgsend,0,512);
		if ((numbytes=recv(new_fd,msgread,sizeof(msgread),0))==-1) {
			fprintf(stderr,"recv: %s",strerror(errno)); exit(errno);
		}
		// get system localtime
		time(&timestamp);
		pt=localtime(&timestamp);
		sprintf(strDT,"%04d-%02d-%02d %02d:%02d:%02d",1900+pt->tm_year,pt->tm_mon+1,pt->tm_mday,pt->tm_hour,pt->tm_min,pt->tm_sec);
		fprintf(stdout,"%s|%s|\nGet:{%s}\n",strDT,inet_ntoa(caddr.sin_addr),msgread);
		// insert other process request!  丢给消息队列处理
		sprintf(msgsend,"<B>Yes I got! 3ks</B>\n");
		if (send(new_fd,msgsend,strlen(msgsend),0)==-1) fprintf(stderr,"send: %s",strerror(errno)); 
		fprintf(stdout,"Put:%s",msgsend);
		fflush(stdout);
		fflush(stderr);
		close(new_fd);
	} // end of while
	close(sock);
	exit(0);
}

