#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include"pkt.h"
#include<fcntl.h>
void error_handling(char *message)
{
	fputs(message,stderr);
	fputc('\n', stderr);
	exit(1);
}
void Fileopen(int  *file_fd)
{	
	if((*file_fd=open("copy.jpg", O_RDWR|O_APPEND))==-1)
	{
		error_handling("File open()error");
	}
	else
	{
		printf("File open Success\n");
	}
}
void Readdata(int sock, int num,PKT_t*pkt1)
{
	
	if(read(sock, (void*)pkt1, sizeof(PKT_t))==-1)
	{
		error_handling("read()error");
	}
	if(pkt1->hdr.ack==num)
	{
		printf("Client %d:Rx ack:%d\n", num,pkt1->hdr.ack);
	}
}
int Readfile(int file_fd,int len,PKT_t *pkt)
{
	len=read(file_fd, pkt->pload.data, 300);
	pkt->hdr.len=len;
	return len;
}
void  Writedata(int sock,int num,PKT_t*pkt)
{
	pkt->hdr.seq=num;
	printf("Client %d:Tx seq:%d\n", num,pkt->hdr.seq);
	if(write(sock, (void*)pkt, sizeof(PKT_t))==-1)
	{
			error_handling("write ()error");
	}
}
int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	int seqnum=0;
	int num=0;
	int i=0;
	int con=0;
	int file_fd=0;
	int len=0;
	int sum=0;
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
	
	con=connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if(con==-1)
	{
		error_handling("connect() error");
	}
	else 
	{
		printf("server connect\n");
	}
	
	printf("Input Seq num:");
	scanf("%d", &seqnum);
	Fileopen(&file_fd);

	while(1)
	{
		num=seqnum+i;
		len=Readfile(file_fd,len,pkt1);
		pkt->hdr.len=len;
		sum=sum+len;
		printf("Tx data:%dByte\n", sum);
		if(len==0)
		{	
				printf("Flie transmission End\n");
				break;
		}
		Writedata(sock,num,pkt);
		Readdata(sock,num,pkt1);
		printf("----------------%d TxRx End---------------\n", i+1); //hamsu
		printf("\n");
		i++;
	}
close(file_fd);
close(sock);
free(pkt);
free(pkt1);
return 0;
}
