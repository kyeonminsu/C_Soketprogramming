#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>

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
	int file_fd;
	int len;
	
	PKT_t *pkt;
	pkt = (PKT_t*)malloc(sizeof(PKT_t));
	pkt->hdr.dst_addr=1;
	pkt->hdr.src_addr=2;
	pkt->hdr.ack=3; //i+1
	//pkt->hdr.seq=4; //j+1
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
	
	if((file_fd=open("test.txt", O_RDWR | O_CREAT | O_APPEND))==-1)
	{
		error_handling("File open()error");
	}
	else
	{
		printf("File open Success\n");
	}
	pkt->pload.file_name[100]="test.txt";
	
	if((len=read(file_fd, pkt->pload.data, 100))==-1)
	{
		error_handling("Flie read()error");
	}
	else
	{
		printf("Flie read Success\n");
	}
	pkt->hdr.len=len;
	
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
	{
		error_handling("connect() error");
	}
	else 
	{
		printf("server connect\n");
	}
			
		//	PKT_t *pkt1;
			//pkt1=(PKT_t*)malloc(sizeof(PKT_t));
			w=write(sock,(void*)pkt,sizeof(PKT_t));
			if(w==-1)
			{
				error_handling("write() error");
			}	
	
			r=read(sock,(void*)pkt,sizeof(PKT_t));
			if(r==-1)
			{	
				error_handling("read() error");
			}
			else
			{
				printf("Wow good read\n");
			}
			if(pkt->hdr.seq==4)
			{
				printf("seq:%d\n", pkt->hdr.seq);
			}
			else
			{
				printf("seq:%d\n", pkt->hdr.seq);
				//printf("struct list error !\n");
			}
			
	close(file_fd);
    close(sock);
    //free(pkt1);
    free(pkt);
return 0;
}
