

typedef {unsigned long} UL;




typedef struct header {
	unsigned long dst_addr;
	unsigned long src_addr;

	unsigned short cmd;
	unsigned short len;
	unsigned short seq;
	unsigned short ack;
} HEADER_t;


typedef struct payload1 {
	char filename[100];
	int file_size;
	time_t date;
} payload1_t;

typedef struct payload2 {
	...
		...
} payuload2_t;



typedef struct packet {
	HEADER_t hdr;
	payload1_t pload;
} PKT_1_t;



#define	MY_ADDR 100

int main (void)
{
	unsigned char *buf;

	unsigned short sent_seq;
	
	buf = malloc(sizeof(PKT_1_t));

	while(1)
	{

		last, I sent seq ---> store sent_seq;
		

		read(rx_fd, buf, sizeof(PKT_1_t));

		PKT_1_ *pkt = buf;
	
		if(pkt->hdr.dst_addr != MY_ADDR)
		{
			printf("Shit!!  It's not me\n");
			continue;
		}

		if(pkt->hdr.ack == sent_seq)
		{
			send the next packet...
		}
		else
		{
			send the last packet again.
		}

	
	}







	PKT_1_t *pkt = buf;

	pkt->hdr.dst_addr = 3;
	pkt->hdr.seq = 8;
	.....

	write(send_fd, buf, sizeof(PKT_1_t));
	



	PKT_1_t *pkt;
	pkt = (PKT_1_t *)malloc(sizeof(...));

	pkt->...

	write(send_fd, (unsgined char *)pkt, sizeof(PKT_1_t));
	or
	wirte(send_fd, (void *)pkt, sizeof(PKT_1_t));




}

