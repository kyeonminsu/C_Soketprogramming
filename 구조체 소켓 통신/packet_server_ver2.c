#include <stdio.h>              // 표준 입출력
#include <stdlib.h>             // 표준 라이브러리
#include <string.h>             // 문자열 처리 함수
#include <unistd.h>             // unix 표준
#include <arpa/inet.h>          // 인터넷 프로토콜
#include <sys/socket.h>         // 소켓 통신 함수
#include <sys/types.h>		// 데이터 유형
#include <sys/stat.h>		// stat() 함수에 의해 반환 된 데이터

#include "IPS.h"


void error_handling(char *message);     // 에러 처리 함수(계속 재사용)

int main(int argc, char *argv[])        // shell에서 입력되는 인자 갯수와 배열
{
	/*서버 소켓 관련*/
        int server_sockfd,client_sockfd;        // 서버 소켓과 클라이언트 소켓 2개 필요   
        //서버 소켓은 받아들이는 일만 하고 , 실제 통신은 클라이언트 소켓이 한다.
        char message[BUF_SIZE];         // 메시지 수신용 버퍼 1024 바이트
        int str_len, i;                 // 수신한 데이터의 길이와 제어변수 i (index,관례)
        
        struct sockaddr_in server_addr; // 서버용 소켓(accept 용)
        struct sockaddr_in client_addr; // 클라이언트 소켓(서버 소켓이 accept 해주면 생성)
        socklen_t client_addr_size;     // 클라이언트 주소 구조체의 바이트 사이즈(크기)
       
	/*Protocol 관련*/ 
		packet_t *pkt;  		// packet  
//	packet_t Packet;

	// packet generation
	packet_t Packet;
	int pkt_count = 0;
	short user_command = 0;
	short payload_id = 0;

        rvt_t *RSSI_tx;  			// number : 1
        psv_t *sensing; 		// number : 2
        pasd_t *stat; 			// number : 3
        pfn_t *notify; 			// number : 4
        pup_t *position; 		// number : 5
        resc_t *congestion; 		// number : 6
        fop_t  *optimal; 		// number : 7
	
	RSSI_tx =((rvt_t*)pkt->payload);
	sensing =((psv_t*)pkt->payload);
	stat =((pasd_t*)pkt->payload);
	notify =((pfn_t*)pkt->payload);
	position =((pup_t*)pkt->payload);
	congestion = ((resc_t*)pkt->payload);
	optimal =((fop_t*)pkt->payload);

	//pkt = &Packet;
	pkt = malloc(sizeof(packet_t));

//	free(pkt);
	

	if(argc != 2)                   // 아규먼트가 2개가 아니면 , shell 에서 포트 번호를 지정하지 않았으면
        {
                printf("Usage : %s <port>\n",argv[0]);  // 사용자에게 사용법 안내 할 것
                exit(1);                // 비정상 종료처리
        }
	
	/* 01. socket : 클라이언트로 부터 요청을 받기 위한 소켓 생성 */
        server_sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);      // TCP 통신용 서버 소켓 생성
        /* PF_INET vs AF_INET
           둘다 출력을 해보면 2라는 상수가 나오게 된다. 
           정의하는 것은 IPv4 에서의 인터넷 프로토콜 로 같다.
           PF_INET(프로토콜 체계를 표현 할때), AF_INET(주소 체계를 표현할 떄)
           주소 체계를 표현 하는 것은 struct sockaddr_in 의 구조체에 sin_family 멤버에 있다. */

	if(server_sockfd == -1)         				// 생성에 실패 했을 떄 성공 : file descriptor , 실패시 : -1 반환
                error_handling("Server : Can't open stream socket\n"); 	//error 표시
	
	memset(&server_addr,0x00,sizeof(server_addr));  		// 서버 구조체를 0으로 채운다 즉, 0으로 초기화
	server_addr.sin_family = AF_INET;       			// 서버 주소 체계를 인터넷 계열로 설정한다.
        server_addr.sin_addr.s_addr  = htonl(INADDR_ANY);       	// 서버는 주소가 필요없음 (제자리 동작하기 떄문)
        server_addr.sin_port = htons(atoi(argv[1]));    		// 서버가 사용할 포트 번호
	// 프로그램당 포트1개
	
	/* 02. bind : 소켓과 주소를 엮어준다.  즉, 서로 연결해준다. */
	if(bind(server_sockfd,(struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
                error_handling("Server : Can't bind local address\n");
	
	/* 03. listen : 소켓을 서버용으로 사용 할 수 있게 한다. ( 클라이언트로부터 요청이 있을 때 까지 수신 대기열 생성) */
	if(listen(server_sockfd, 5) == -1)
	        error_handling("Server : Can't listenig connect\n");

	
	client_addr_size = sizeof(client_addr);         	// 클라이언트 주소 구조체의 크기
	pkt = (packet_t *)message;
	printf("Waiting for request from client.....\n");		// 클라이언트로 부터 요청을 기다리는중 .. 이라는 메시지 출력
	

	for(i=0; i<5; i++)      // 5번만 클라이언트의 접속을 받아 들인다.
        {                       // 서버 프로그램은 여기서 Blcok 된다. 클라이언트가 접속할 때 까지 듣고 있는다.
		/*04. accept : */
                client_sockfd = accept(server_sockfd,(struct sockaddr*)&client_addr,&client_addr_size);
                if(client_sockfd == -1)
                        error_handling("Server accept failed\n");
                else
                        printf("Connected client %d\n %d\n,",i+1);
			printf("New Client IP: %s\n ",inet_ntoa(client_addr.sin_addr));

                // accept가 성공하면 클라이언트 소켓 client_sockfd 가 만들어진다.
               	// while 문 언제 끝날 까?
                // 클라이언트가 소켓을 닫아 버리면 0(NULL)이 돌아 온다.
        	while((str_len = read(client_sockfd,pkt,sizeof(pkt->payload))!= 0))
		{
                      /*  if(pkt->payload_id ==1)
                        {
                        	RSSI_tx->B_id;
				RSSI_tx->tx_power;
				RSSI_tx->adv_int;
				pkt->len =sizeof(packet_t) - sizeof(pkt->payload) + sizeof(rvt_t);
				printf("1's len >>>>> %d\n",pkt->len);		
			}
												











                        else if(pkt->payload_id ==2)
                        {
                                printf("%s\n",sensing);
                        }
                        else if(pkt->payload_id ==3)
                        {
                                printf("%s\n",stat);
                        }
                        else if(pkt->payload_id ==4)
                        {
                                printf("%s\n",notify);
                        }
                        else if(pkt->payload_id ==5)
                        {
                                printf("%s\n",position);
                        }
                        else if(pkt->payload_id ==6)
                        {
                                printf("%s\n",congestion);
                        }
                        else(pkt->payload_id ==7)
                        {
                                printf("%s\n",optimal);
                        } 
		} */
        		
            		write(client_sockfd, pkt, sizeof(pkt->len));
                	{
				if(pkt->payload_id == 1)
				{
					RSSI_tx->B_id;
                                	RSSI_tx->tx_power;
                                	RSSI_tx->adv_int;
                                	pkt->len =sizeof(packet_t) - sizeof(pkt->payload) + sizeof(rvt_t);
                                	printf("1's len >>>>> %d\n",pkt->len);          
				}
			}
            	close(client_sockfd);   // 클라이언트와 접속이 끊겼으면 클라이언트 소켓을 닫는다.
		}
        }
        close(server_sockfd);           // 5번 클라이언트를 받아주고 나면 서버 소켓을 닫고 끝낸다.
	printf("Server: %s client closed\n",inet_ntoa(client_addr.sin_addr));
        return 0;
}
                      
void error_handling(char *message)
{
        fputs(message,stderr);
        fputc('\n',stderr);
        exit(1);
}

