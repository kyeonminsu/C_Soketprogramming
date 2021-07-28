#include <stdio.h>                      // 표준 입출력
#include <stdlib.h>                     // 표준 라이브러리
#include <string.h>                     // 문자열 처리 함수
#include <unistd.h>                     // Unix 표준
#include <arpa/inet.h>                  // 인터넷 프로토콜
#include <sys/socket.h>                 // 소켓 통신 함수
#include <sys/types.h>                  // 데이터 유형
#include <sys/stat.h>                   // stat() 함수에 의해 반환 된 데이터

#include "IPS.h"

#define CLIENT_ID			0x0002

void error_handling(char *message);     // error 처리 함수

int main(int argc, char *argv[])        //shell 에서 IP와 port번호를 받는다.
{
	/*소켓 관련 */
        int server_sockfd;              // 서버에 접속할 소켓
        char message[BUF_SIZE];         // 서버에 보낼 메시지를 저장할 문자열 버퍼
        int str_len;                    // 송수신 메시지의 문자열 길이
        struct sockaddr_in server_addr; // 접속할 서버의 주소 (사전에 주소를 알아야됨)

		// packet generation
		packet_t Packet;
		int pkt_count = 0;
		short user_command = 0;
		short user_payload_id = 0;

	 /*Protocol 관련*/
        packet_t *pkt, *rx_pkt;                  // packet  
        rvt_t *RSSI_tx;                      // number : 1
        psv_t *sensing;                 // number : 2
        pasd_t *stat;                   // number : 3
        pfn_t *notify;                  // number : 4
        pup_t *position;                // number : 5
        resc_t *congestion;             // number : 6
        fop_t  *optimal;                // number : 7

	
	/*IP와 PORT를 지정하지 않았을 경우 사용법을 알려준다.*/
	if(argc!=3)
        {
                printf("Usage : %s <IP> <PORT>\n",argv[0]);      // 사용법 표시해둘것.
                exit(1);                // 비정상 종료
        }

	/*01. Socket : 서버에 연결하기 위한 소켓을 생성한다 .*/
        server_sockfd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);        // TCP(Stream ) 프로토콜 생성
        if(server_sockfd == -1)         //만들어 지지 않을 경우
                error_handling("Client :  Can't create socket\n");      //에러 표시후 종료

        memset(&server_addr,0,sizeof(server_addr));                  // 서버 쪽 주소 구조체를 0으로 초기화
        server_addr.sin_family = AF_INET;                               // 인터넷 주소체계 사용
        server_addr.sin_addr.s_addr = inet_addr(argv[1]);               // 서버 IP를 구조체에 저장한다.
        server_addr.sin_port = htons(atoi(argv[2]));                    // 서버 PORT를 구조체에 저장한다.

	/*02. Connect : 클라이언트 함수 이고 , 서버는 accept 상태여야 한다.*/
	if(connect(server_sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr)) == -1)
                error_handling("Client : Can't connect\n");             // 연결 실패
        else
                puts("Connected.!!!\n");        //연결 성공


	
	
	while(1)                                // 연결이 되면 무한 루프
        {
//           	fputs("Input message(Q to quit): ",stdout);             // 메시지 입력 안내문
//           	fgets(message, BUF_SIZE, stdin);                        // 사용자로부터 메시지를 입력받는다.

			pkt = &Packet;
			pkt->Destination_address = SERVER_ID;
			pkt->source_address = 0x0002;
			pkt->sequnence_no = pkt_count++;
			pkt->ack = 0;



			printf("Select CMD Num  (Req:1, Resp:2, Push:3) :  ");
			fgets(message, sizeof(message), stdin);

			if(!strcpy(message,"q \n") || !strcmp(message,"Q \n"))
			   break;										   // 사용자가 q나 Q를 입력하면 무한 루프 탈출


			user_command = atoi(message);

			if(user_command == 1)
			{
				printf("CMD : Request\n");
				pkt->CMD = 0b00;
			}
			else if(user_command == 2)
			{
				printf("CMD : Reply\n");
				pkt->CMD = 0b01;
			}
			else if(user_command == 3)
			{
				printf("CMD : Push\n");
				pkt->CMD = 0b10;
			}
			else 
			{
				printf("CMD : TBD\n");
				pkt->CMD = 0b11;
			}


			// Payload ID : User Input Code
			printf("Select Payload ID :  ");
			fgets(message, sizeof(message), stdin);

			
			user_payload_id = atoi(message);

			if(user_payload_id == 1)
			{
				printf("RSSI Value Tx\n");
				
				RSSI_tx = pkt->payload;
				RSSI_tx->B_id = 1;
				RSSI_tx->tx_power = 10;
				RSSI_tx->adv_int = 100;

				pkt->len = sizeof(packet_t) - sizeof(pkt->payload) + sizeof(rvt_t);
				printf("len=? %d\n",pkt->len);
			}
			else if(user_payload_id == 2)
			{
				printf("Sensing Value\n");

				sensing = pkt->payload;
				sensing->Mac_address[6]=(char)120;
				sensing->F_sensor_status = 2;
				sensing->F_sensor_id = (char)100;
				sensing->sensing_value = 20;
				sensing->F_sensor_position = 2.3;

				pkt->len = sizeof(packet_t) - sizeof(pkt->payload) + sizeof(psv_t);
				printf("len=? %d\n",pkt->len);
			}
			else if
		//	{
				
			// continued...
			


			
			



			write(server_sockfd, pkt, sizeof(pkt->len));           // 소켓을 이요해 메시지 전송

            str_len =read(server_sockfd, message, BUF_SIZE-1);       // 에코되어 돌아오는 메시지 수신
            message[str_len] =0;                                   // 보낼 때 NULL 문자 안보내서, 받았을 때 넣어줘야 한다.
//           	printf("Message from server : %s",message);             // 서버로 부터 받은 메시지 표시

			rx_pkt = (packet_t *)message;

			if(rx_pkt->Destination_address != CLIENT_ID)
			{
				printf("Destination is not me\n");
				continue;
			}

			if(rx_pkt->source_address != SERVER_ID)
			{
				printf("Source is not Server\n");
				continue;
			}

			if(rx_pkt->ack == 1)
			{
				printf("Ack OK\n");
			}

			// if Ack = 0, check CMD
			// .....
			// pthread�� ���� ����
			

			


        }
	close(server_sockfd);						// 종료하기 전에 시스템에서 얻어온 소켓을 돌려준다.
	return 0;
}
void error_handling(char *message)                                      // 예외 처리 함수
{
        fputs(message, stderr);                                          // 예외 표시하고
        fputc('\n', stderr);                                             // 줄 바꿔주고
        exit(1);                                                        // 비정상 종료 처리
}

