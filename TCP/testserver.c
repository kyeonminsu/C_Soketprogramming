#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include"pkt.h"
#define MAXLINE 1024
#if 0
typedef struct header{
	unsigned long dst_addr;
	unsigned long src_addr;
	unsigned short cmd;
	unsigned short len;
	unsigned short seq;
	unsigned short ack;
}HEADER_t;
typedef struct payload{
	char file_name[100];
	int file_size;
	time_t date;
}payload_t;
typedef struct packet{
	HEADER_t hdr;
	payload_t pload;
}PKT_t;
#endif

void error_handling(char *message)
{
	fputs(message,stderr);
	fputc('\n', stderr);
	exit(1);
}
int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;
	int r=0;
	int w=0;
	int acknum=0;
	//int time=0;
	int i=0;
	char buf[MAXLINE];
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	PKT_t *pkt;
	pkt = (PKT_t*)malloc(sizeof(PKT_t));

	PKT_t *pkt1;
	pkt1=(PKT_t*)malloc(sizeof(PKT_t));
	
	pkt->hdr.dst_addr=1;
	pkt->hdr.src_addr=2;

	printf("Socket program Start!!\n");
	if(argc!=2)
	{
		printf("Usage : %s<port>\n", argv[0]);
		exit(1);
	}

	serv_sock=socket(PF_INET,SOCK_STREAM, 0);

	if(serv_sock==-1)
	{
		error_handling("socket() error");
	}
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
	{
		error_handling("accepr()error");
	}
	if(listen(serv_sock, 5)==-1)
	{
		error_handling("listen()error");
	}

	clnt_addr_size=sizeof(clnt_addr);
	

		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
		if(clnt_sock==-1)
		{
			error_handling("accept() error");
		}
		else 
		{
			printf("Connected client\n");
		}
	
	while(1)
	{	
		r=read(clnt_sock,(void*)pkt1,sizeof(PKT_t));
		if(r==-1)
		{
			error_handling("read() error");
		}
		
		if(pkt->hdr.src_addr==pkt1->hdr.dst_addr)
		{
			printf("addr Match!!\n");
		}
		else 
		{
			error_handling("addr error");
		}
		
		pkt->hdr.seq=pkt1->hdr.seq;
		pkt->hdr.ack=pkt1->hdr.seq;
		if(pkt1->hdr.seq==pkt->hdr.ack) // 1st
		{
			printf("Rx seq:%d\n", pkt1->hdr.seq);
			printf("Tx ack:%d\n", pkt->hdr.ack);		
		}
		else
		{
			printf("seq number error!\n");
		}
		memset(buf, 0x00, MAXLINE);
		fputs("Input message(S or s == Start, Q or q == quit, A or a == ack):", stdout);
		fgets(buf, MAXLINE, stdin);
		fflush(stdout);
                if(!strcmp(buf,"S\n")||!strcmp(buf,"s\n"))	
          	{
			w=write(clnt_sock,(void*)pkt,sizeof(PKT_t));
			if(w==-1)
			{
				error_handling("write() error");
			}
		}
		else if(!strcmp(buf, "Q\n")||!strcmp(buf, "q\n"))
		{
			pkt->hdr.cmd=1;
			write(clnt_sock, (void*)pkt, sizeof(PKT_t));
			printf("End\n");
			close(clnt_sock);
			//break;
			exit(1);
		}
		else if(!strcmp(buf, "A\n")||!strcmp(buf, "a\n"))
		{
			fputs("Input ack num: ", stdout);
			fgets(buf, MAXLINE, stdin);
			acknum=atoi(&buf[0]);
			pkt->hdr.ack=acknum;
			if(write(clnt_sock, (void*)pkt, sizeof(PKT_t))<0)
			{
				error_handling("write() error");
			}
		}
		
	}	
	  close(clnt_sock);
	
free(pkt);
free(pkt1);
close(serv_sock);
return 0;
}
