

#define BUF_SIZE 1024                   // 소켓 통신 문자열 버퍼의 크기는 1024바이트
#define SERVER_ID	0x0001

typedef struct packet
        {
                short Destination_address;
		short source_address;
		short sequnence_no;
		short len;
                char ack;
                short CMD;		// 1: xxxx    2: xxx
                short payload_id;
                char payload[60];
        } packet_t;

typedef struct rssi_value_tx	// 1
        {
                int B_id;
                short tx_power;
                short adv_int;
        }rvt_t;


typedef struct push_sensing_value
        {
                char Mac_address[6];
                short F_sensor_status;
                char F_sensor_id;
                short sensing_value;
                float F_sensor_position;
        } psv_t;
         
        
typedef struct push_a_status_dec
        {    
                short congestion_calc;
                char A_status;
                short A_id;
                short A_position; 
        }pasd_t;
 
    
typedef struct push_fire_notify
        {
                short value[2]; 

        }pfn_t;


typedef struct push_user_position
        {
                char user_id;
                int user_ip;
                char user_position;
        }pup_t;


typedef struct res_congestion
        {
               short A_id;
                char A_status;
                short congestion_calc;
                short A_tx_power;
        }resc_t;


typedef struct find_optimal_path
        {
                int fire_area_cell_num[10];
                short A_id;
                short A_position;
                char user_position;
	}fop_t;

