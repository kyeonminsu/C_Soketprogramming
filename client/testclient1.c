#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include"pkt.h"
#include<fcntl.h>
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
	unsigned char data[100];
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
	int sock;
	struct sockaddr_in serv_addr;
	int r=0;
	int w=0;
	char buf[MAXLINE];
	int seqnum=0;
	int i=0;
	int con=0;
	int file_fd;
	int len=0;
	char *fname="copy.jpg";
	
	PKT_t *pkt;
	pkt = (PKT_t*)malloc(sizeof(PKT_t));
	
	PKT_t *pkt1;
	pkt1=(PKT_t*)malloc(sizeof(PKT_t));

	pkt->hdr.dst_addr=2;
	pkt->hdr.src_addr=1;
	
	printf("Socket program Start!!\n");
	
	if(argc!=2)
	{
		printf("Usage : %s <IP><port>\n", argv[0]);
		exit(1);
	}
	sock=socket(PF_INET,SOCK_STREAM, 0);
	
	if(sock==-1)
	{
		error_handling("socket() error");
	}
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	serv_addr.sin_port=htons(atoi(argv[1]));
	
	if((file_fd=open(fname, O_RDWR |O_APPEND))==-1)
	{
		error_handling("File open()error");
	}
	else
	{
		printf("File open Success\n");
	}
	pkt->pload.file_name[0]=*fname;
	
	con=connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if(con==-1)
	{
		error_handling("connect() error");
	}
	else 
	{
		printf("server connect\n");
	}
	fputs("Input seq num: ", stdout);
	fgets(buf, MAXLINE, stdin);
	seqnum = atoi(&buf[0]);
	while(1)
	{	
		len=read(file_fd, pkt->pload.data, 100);
	    pkt->hdr.len=len;
		if(len==0)
		{
				printf("Flie transmission End\n");
				break;
		}
		pkt->hdr.seq=seqnum+i;
		w=write(sock,(void*)pkt,sizeof(PKT_t));
		
		printf("Tx seq:%d\n", pkt->hdr.seq);
		if(w==-1)
		{
			error_handling("write() error");
		}	
  		//sleep(1);
			r=read(sock,(void*)pkt1,sizeof(PKT_t));
			if(r==-1)
			{	
				error_handling("read() error");
			}
			//if(pkt1->hdr.cmd==1)
			//
				//printf("End\n");
				//break;
			//
			if(pkt->hdr.src_addr==pkt1->hdr.dst_addr)
			{
				printf("addr Match!\n");
			}
			else
			{
				error_handling("addr error\n");
			}

			if(pkt1->hdr.ack==pkt->hdr.seq)
			{
				printf("Rx ack:%d\n", pkt1->hdr.ack);
			}
			else
			{
				printf("ack error! retransmission\n");
				continue;
			}
			i++;
		}
		
	close(file_fd);
    close(sock);
    free(pkt1);
    free(pkt);
return 0;
}
