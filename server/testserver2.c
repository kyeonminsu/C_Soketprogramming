#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include"pkt.h"
void error_handling(char *message)
{
	fputs(message,stderr);
	fputc('\n', stderr);
	exit(1);
}
void Fileopen(int *file_fd)
{
	if((*file_fd=open("copy.jpg", O_RDWR |O_CREAT|O_APPEND, 0666))==-1)
	{
		error_handling("File open()error");
	}
}
int Readdata(int clnt_sock,int file_fd,int len,PKT_t*pkt1)
{
	if(read(clnt_sock, (void*)pkt1, sizeof(PKT_t))==-1)
	{
		error_handling("read()error");
	}

	len=pkt1->hdr.len;
	if(write(file_fd, pkt1->pload.data, len)==-1)
	{
			error_handling("low write()error");
	}
	
	return len;
}
void  Writedata(int clnt_sock, PKT_t*pkt)
{
	if(write(clnt_sock, (void*)pkt, sizeof(PKT_t))==-1)
	{
			error_handling("write ()error");
	}
	
}
int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;
	int file_fd=0;
	int i=0;
	int len=0;
	int sum=0;
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
	Fileopen(&file_fd);
	while(1)
	{
		len=Readdata(clnt_sock,file_fd,len,pkt1);
		pkt->hdr.seq=pkt1->hdr.seq;
		pkt->hdr.ack=pkt1->hdr.seq;
		if(pkt1->hdr.seq==pkt->hdr.ack) 
		{
			printf("Server %d:Rx seq:%d\n", i+1,pkt1->hdr.seq);
			printf("Server %d:Tx ack:%d\n", i+1,pkt->hdr.ack);		
		}
		else
		{
			printf("seq number error!\n");
		}
		sum=sum+len;
		len=pkt1->hdr.len;
		printf("Rx data:%dByte\n", sum);
		if(len==0)
		{
			break;
		}
		Writedata(clnt_sock,pkt);
		printf("----------------%d TxRx End---------------\n", i+1);//hamsu 
		printf("\n");
		i++;
    }
close(file_fd);	
close(clnt_sock);
close(serv_sock);
free(pkt1);
free(pkt);
return 0;
}
