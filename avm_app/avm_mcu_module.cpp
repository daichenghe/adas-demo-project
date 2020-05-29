/******************************************************************************
* File Name          : avm_mcu_module.cpp
* Author             : Duqf
* Revision           : 2.1
* Date               : 03/07/2018
* Description        : Communication with mcu 
*
*******************************************************************************/

#include "avm_config.h"
#include "avm.h"
#include "avm_mcu_module.h"
#include "avm_2d_module.h"
#include "avm_3d_module.h"
#include "avm_ui.h"
#include "tool.h"
#include "avm_ctr.h"
#include "avmlib_sdk.h"
#include <pthread.h>
#include <semaphore.h> 
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include "log.h"
#ifdef T3BOARD

#include "Fat.h"

#endif
#define DEBUGPRINT printf

#define UPDATE_MAX_LENGTH 128

#define MSG_RC_CONTINUE_MISS_COUNT 6
#define MSG_RC_MISS_COUNT 20
#define MSG_RECEIVE_MAX  512
/*
*************************************************************************
*************************************************************************
*                      variable definition
*************************************************************************
*************************************************************************
*/
static pthread_mutex_t s_mcu_data_mutex;

static U8 s_send_msg_rc = 1;		// local send rolling code
static U8 s_recv_msg_rc = 1;		//  received msg rolling code
static U8 s_recv_msg_rc_old = 1;	//  previous received msg rolling code
static U8 s_send_data_buf[32];		// send buffer

static C8 s_write_buff[256];         		// writen to the fifo 
static volatile S32 s_comfd = -1;  // uart dev handle


/* struct*/
static Car_State_S s_car_state ;


/*

*************************************************************************
*************************************************************************
*                      local function prototypes 
*************************************************************************
*************************************************************************
*/

/*change submode for 2d */
static void set_2d_mode( U8 mode);
/*change submode for 3d */
static void set_3d_mode( U8 mode);
static void set_per_mode( U8 mode);
/* start calibration*/
static void start_calibration();
/* cancel calibration*/
static void stop_calibration();
/* change the mode to DVR mode*/
static void set_cal_mode(U8 mode);
/* change the mode to ldws mode*/
static void set_ldws_mode( U8 mode);
/*ldws switch control*/
static void set_ldws_switch(U8 date);
/*update car info and change view mode*/
static void update_car_info(U8 *data);
/*deal touch screen msg*/
static void dvr_touch_msg(U8 *data);
/*set version info*/
static void set_version(U8 *data);

/*query ldws and calibration state*/
static void query_ldws_cali_state();
/*response system error info*/
static void response_fault_info();

/**********************mcu update******************************/
/* check mcu update file*/
static U8 check_update_file(void);
/*notify mcu update */
static void T3_notify_mcu_update(); 
/*mcu update init*/
static void mcu_update_init(void);
/* check mcu update response result,and change mode in mcu update mode */
static void check_mcu_response_update_query(U8 *data);
/*response bootloder update query , and sleep 10ms send data*/
static void response_mcu_update_query(U8 flag);
/*mcu update*/
static void send_update_data();
/*fill update buf*/
static void fill_update_buf(U32 offet,U8 length);	
/*send update buf*/
static void send_update_buf(U8 length);
/*calculate update file check code*/
static void calculate_update_file_check_code(U8 length);
/* check mcu  response update data result*/
static void check_mcu_response_update_data(U8 *data);
/* notify mcu update over*/
static void send_mcu_update_over(U8 flag);
/* check mcu response update over */
static void check_mcu_response_update_over(U8 *data);
/* mcu update exit */
static void mcu_update_exit(void);
/* get file size*/
static U32 get_file_size_systemcall(const C8 * strFileName);   
/***********************mcu update end***********************************/

/* deal control msg ,*/
static void deal_control_msg(U8 *data);
/* deal notify msg*/
static void deal_notify_msg(U8 *data); 
/*deal the query message */
static void deal_query_msg();
/*deal receive data*/
static void deal_receive_msg(U8 *data,U8 length);
/*check data and  */
static void check_recv_data(U8*buf,U8 count);
/*check mcu msg rolling code*/
static void mcu_msg_rc_check(void);
/*read serial msg*/
static S32 get_mcu_data(U8 *buf);
/*free receive thread memery*/
static void  mcu_thread_free(void *arg);
/*set and open uart*/
static S32 com_open( void );
/*serial send msg*/
static void serial_send(U8 *buf,U32 data_length);
/*fill data and calculate the check digit */
static void fill_msg(U8 *data, U8 length,U8 ack_type);
/*ack msg*/
static void ack_message(void);
/*resend func*/
static void resend_func(void);
/*deal received response msg*/
static int deal_ack_msg(U8 msg_id);
/* init msg resend  mangager*/
static void init_resend_mangager(void);
/* add msg to resend buffer */
static void resendbuf_add_msg(U8 *data,U8 length,U32 msg_id);
/*check resend buffer and add to resend mangager*/
static void resendbuf_to_mangager(void);
/*add msg to resend mangager*/
static int mangager_add_msg(U8 *data,U8 length,U32 msg_id);
/*update  dvr info*/
static void send_dvr_state(void);
/*init car info */
static void init_car_info(void);

static void * avm_mcu_thread(void *arg);

/*send ldws alarm info*/
void send_ldws_alarm_msg(void)           
{    
    s_send_data_buf[0] = 0x02;
    s_send_data_buf[1] = 0x24;
    s_send_data_buf[2] = 0x05;
    s_send_data_buf[3] = 0x02;
    s_send_data_buf[4] = 0x00;
    s_send_data_buf[5] = 0x00;
   
    fill_msg(s_send_data_buf,6,0);
}

int  mcu_thread_create(void)
{
    pthread_attr_t threadattr;
    S32 rec = 0;
	pthread_t   tid_mcu; 
    // 绾跨▼
    pthread_attr_init(&threadattr);
    pthread_attr_setdetachstate(&threadattr, PTHREAD_CREATE_DETACHED);
//	test_thread_create();
    if(0 != pthread_create(&tid_mcu, &threadattr, avm_mcu_thread, NULL))
    {
        rec = -1;
    }
    else
    {
    
    }

    return rec;
}

/* serial receive thread loop*/
static void * avm_mcu_thread(void *arg)
{
    S32 count, i = 0;
    U8 buf[MSG_RECEIVE_MAX];	

    s_comfd = com_open();
    
	if (2 > s_comfd )
    {
    	logg(ERR,"Open uart error!\n", __FUNCTION__, __LINE__);     
        exit(0);
    }
	init_car_info();//init car info 
	pthread_mutex_init(&s_mcu_data_mutex, NULL);
	memset(s_send_data_buf,0,sizeof(s_send_data_buf));
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, AVM_NULL);       // enable receive cancel signal
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, AVM_NULL);		// if receive cancel signal  ,at next cancel-point out  

	while (AVM_TRUE)  
    {   
    	memset(buf,0,MSG_RECEIVE_MAX); 
    	count = get_mcu_data(buf);//receive mcu data

        if(count > 0)
		{	
			 DEBUGPRINT("mcu receive buf:");
	    	 for(i = 0;i<count;i++)
	    	 {
	    		DEBUGPRINT(" %02x ",buf[i]);

	    	 }
			 DEBUGPRINT("\r\n");
			check_recv_data(buf,count); //check mcu msg			
		 } 
		
		
	}  
	pthread_exit(AVM_NULL);
	
	return NULL;
}

  
U8 get_turn_light_state(void)
{
	U8 state = s_car_state.left_turn_light | s_car_state.right_turn_light;
	return state;
}

/*get car info */
Car_State_S get_car_info(void)
{
	pthread_mutex_lock(&s_mcu_data_mutex);
	Car_State_S car_info = s_car_state;
	
	pthread_mutex_unlock(&s_mcu_data_mutex);
	return car_info;
}

/*init car info */
static void init_car_info(void)
{
	
	pthread_mutex_lock(&s_mcu_data_mutex);
	s_car_state.left_turn_light = AVM_OFF;              
	s_car_state.right_turn_light = AVM_OFF;               
	s_car_state.steering_wheel =0;	
	s_car_state.steering_angle = 0;
	s_car_state.car_speed = 0;	
	
	pthread_mutex_unlock(&s_mcu_data_mutex);
}
static void set_cal_mode(U8 mode)
{
	
	int input_new_flag = AVM_INPUT_INVALID;
	view_keep_t view_keep_tmp;
	avm_view_mutex_lock();	
	memcpy(&view_keep_tmp,&g_view_keep,sizeof(view_keep_t));
	if(view_keep_tmp.mode != MODE_ORI)
	{
		view_keep_tmp.mode_last = view_keep_tmp.mode;
		view_keep_tmp.mode = MODE_ORI;
		view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
		view_keep_tmp.sub_mode = MODE_ORI_SPLIT;
		input_new_flag = AVM_INPUT_VALID;

	}
	if(input_new_flag == AVM_INPUT_VALID)
	{
		memcpy(&g_view_keep,&view_keep_tmp,sizeof(view_keep_t));
		g_view_keep.winclear = AVM_SCREEN_CLEAN_ON;
	}
	avm_view_mutex_unlock();	
}

/*change 2d submode */
static void set_2d_mode( U8 mode)
{	

	int input_new_flag = AVM_INPUT_INVALID;
	view_keep_t view_keep_tmp;
	int mode_change;
	avm_view_mutex_lock();	
	memcpy(&view_keep_tmp,&g_view_keep,sizeof(view_keep_t));

	if(view_keep_tmp.mode != MODE_AVM_2D)
	{
		view_keep_tmp.mode_last = view_keep_tmp.mode;
		view_keep_tmp.mode = MODE_AVM_2D;	
	}
	mode_change = mode - 1;
	switch (mode_change )
	{
		case MODE_2D_FRONT:
			
			view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
			view_keep_tmp.sub_mode = MODE_2D_FRONT;//MODE_3D_BACK;			
			view_keep_tmp.fly_flag = AVM_FLY_OFF;
			input_new_flag = AVM_INPUT_VALID;
			DEBUGPRINT("mcu change mode on 2d  front\r\n");
			break;
		
		case MODE_2D_RIGHT:
			
			view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
			view_keep_tmp.sub_mode = MODE_2D_RIGHT;//MODE_3D_BACK;			
			view_keep_tmp.fly_flag = AVM_FLY_OFF;
			input_new_flag = AVM_INPUT_VALID;
			DEBUGPRINT("mcu change mode on 2d right\r\n");
			
			break;
				
		case MODE_2D_BACK:
			
			view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
			view_keep_tmp.sub_mode = MODE_2D_BACK;//MODE_3D_BACK;			
			view_keep_tmp.fly_flag = AVM_FLY_OFF;
			input_new_flag = AVM_INPUT_VALID;
			DEBUGPRINT("mcu change mode on 2d back\r\n");
			
			break;

		case MODE_2D_LEFT:
			
			view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
			view_keep_tmp.sub_mode = MODE_2D_LEFT;//MODE_3D_BACK;			
			view_keep_tmp.fly_flag = AVM_FLY_OFF;
			input_new_flag = AVM_INPUT_VALID;
			
			DEBUGPRINT("mcu change mode on 2d left\r\n");
			break;

		default:
			
			view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
			view_keep_tmp.sub_mode = MODE_2D_FRONT;//MODE_3D_BACK;			
			view_keep_tmp.fly_flag = AVM_FLY_OFF;
			input_new_flag = AVM_INPUT_VALID;
			logg(ERR,"unknow 2d mode %d\n",mode);
			
			break;

	}
	if(input_new_flag == AVM_INPUT_VALID)
	{
		memcpy(&g_view_keep,&view_keep_tmp,sizeof(view_keep_t));
		g_view_keep.winclear = AVM_SCREEN_CLEAN_ON;
	}
	avm_view_mutex_unlock();	
}

/*change per submode */
static void set_per_mode( U8 mode)
{	
	
	int input_new_flag = AVM_INPUT_INVALID;
	view_keep_t view_keep_tmp;
	avm_view_mutex_lock();	
	memcpy(&view_keep_tmp,&g_view_keep,sizeof(view_keep_t));

	if(view_keep_tmp.mode != MODE_PER)
	{
		view_keep_tmp.mode_last = view_keep_tmp.mode;
		view_keep_tmp.mode = MODE_PER;
		
	}
	switch (mode)
	{
		case 0:
			
			if(view_keep_tmp.sub_mode != MODE_PER_FRONT)
			{
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_PER_FRONT;
				input_new_flag = AVM_INPUT_VALID;
			}
			DEBUGPRINT("mcu change mode per_mode  front\r\n");
			break;
		
		case 1:
			
			if(view_keep_tmp.sub_mode != MODE_PER_BACK)
			{
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_PER_BACK;
				input_new_flag = AVM_INPUT_VALID;

			}
			DEBUGPRINT("mcu change mode per_mode back\r\n");
			break;

		default:
			logg(ERR,"unknow MODE_PER %d\n",mode);
			break;

	}
	if(input_new_flag == AVM_INPUT_VALID)
	{
		memcpy(&g_view_keep,&view_keep_tmp,sizeof(view_keep_t));
		g_view_keep.winclear = AVM_SCREEN_CLEAN_ON;
	}
	avm_view_mutex_unlock();	
}

/*change 3d submode*/
static void set_3d_mode( U8 mode)
{
	
	int input_new_flag = AVM_INPUT_INVALID;
	view_keep_t view_keep_tmp;
	avm_view_mutex_lock();	
	memcpy(&view_keep_tmp,&g_view_keep,sizeof(view_keep_t));

	if(view_keep_tmp.mode != MODE_AVM_3D)
	{
		view_keep_tmp.mode_last = view_keep_tmp.mode;
		view_keep_tmp.mode = MODE_AVM_3D;
	}
	
	switch (mode)
	{
		case 0:
			
			view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
			view_keep_tmp.sub_mode = MODE_3D_ROTATE;//MODE_3D_BACK;			
			view_keep_tmp.fly_flag = AVM_FLY_OFF;
			input_new_flag = AVM_INPUT_VALID;	
			DEBUGPRINT("mcu change mode on MODE_3D_ROTATE\r\n");
			break;
			
		case 1:	
			
    		if(view_keep_tmp.sub_mode != MODE_3D_LEFTUP)
			{
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_3D_LEFTUP;
				view_keep_tmp.fly_flag = AVM_FLY_ON;
				input_new_flag = AVM_INPUT_VALID;

			}
			DEBUGPRINT("mcu change mode on MODE_3D_LEFTUP\r\n");
			break;
		
		case 2:	
			
    		if(view_keep_tmp.sub_mode != MODE_3D_BACK)
			{
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_3D_BACK;
				view_keep_tmp.fly_flag = AVM_FLY_ON;
				input_new_flag = AVM_INPUT_VALID;
			}
			DEBUGPRINT("mcu change mode on 3D MODE_3D_BACK\r\n");
			break;
			
		case 3:	
			
    		if(view_keep_tmp.sub_mode != MODE_3D_RIGHTUP)
			{
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_3D_RIGHTUP;
				view_keep_tmp.fly_flag = AVM_FLY_ON;
				input_new_flag = AVM_INPUT_VALID;
			}
			DEBUGPRINT("mcu change mode on 3D MODE_3D_RIGHTUP\r\n");
			break;

		case 4:		
			
    		if(view_keep_tmp.sub_mode != MODE_3D_RIGHT)
			{
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_3D_RIGHT;
				view_keep_tmp.fly_flag = AVM_FLY_ON;
				input_new_flag = AVM_INPUT_VALID;
			}
			DEBUGPRINT("mcu change mode on 3D MODE_3D_RIGHT\r\n");
			break;

		case 5:		
			
    		if(view_keep_tmp.sub_mode != MODE_3D_FRONTRUP)
			{
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_3D_FRONTRUP;
				view_keep_tmp.fly_flag = AVM_FLY_ON;
				input_new_flag = AVM_INPUT_VALID;
			}
			DEBUGPRINT("mcu change mode on 3D MODE_3D_FRONTRUP\r\n");
			break;
		
		case 6:		
			
    		if(view_keep_tmp.sub_mode != MODE_3D_FRONT)
			{
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_3D_FRONT;
				view_keep_tmp.fly_flag = AVM_FLY_ON;
				input_new_flag = AVM_INPUT_VALID;
			}
			DEBUGPRINT("mcu change mode on 3D MODE_3D_FRONT\r\n");
			break;
			
		case 7:	
			
    		if(view_keep_tmp.sub_mode != MODE_3D_FRONTLUP)
			{
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_3D_FRONTLUP;
				view_keep_tmp.fly_flag = AVM_FLY_ON;
				input_new_flag = AVM_INPUT_VALID;
			}
			DEBUGPRINT("mcu change mode on 3D MODE_3D_FRONTLUP\r\n");
			break;

		case 8:	
			
    		if(view_keep_tmp.sub_mode != MODE_3D_LEFT)
			{
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_3D_LEFT;
				view_keep_tmp.fly_flag = AVM_FLY_ON;
				input_new_flag = AVM_INPUT_VALID;
			}
			DEBUGPRINT("mcu change mode on 3D MODE_3D_LEFT\r\n");
			break;

			
		default:
		
			view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
			view_keep_tmp.sub_mode = MODE_3D_LEFTUP;
			view_keep_tmp.fly_flag = AVM_FLY_ON;
			input_new_flag = AVM_INPUT_VALID;
			logg(ERR,"unknow 3d mode %d\n",mode);    		
			break;

	}
	if(input_new_flag == AVM_INPUT_VALID)
	{
		memcpy(&g_view_keep,&view_keep_tmp,sizeof(view_keep_t));
		g_view_keep.winclear = AVM_SCREEN_CLEAN_ON;
	}
	avm_view_mutex_unlock();	
}



/*set car current info */
static void set_car_info(U8 *data)
{
	
	pthread_mutex_lock(&s_mcu_data_mutex);

	s_car_state.left_turn_light = data[5] & 0x01;              
	s_car_state.right_turn_light = (data[5] >> 1) & 0x01 ;               
	s_car_state.steering_wheel = (data[5] >> 2) & 0x01; 
	
	s_car_state.steering_angle = (data[6] << 8);       			
	s_car_state.steering_angle |= data[7]; 
	s_car_state.steering_angle /= 10;
	
	s_car_state.car_speed = (data[8] << 8);
	s_car_state.car_speed |= data[9]	;
	s_car_state.car_speed /= 10;	
	//DEBUGPRINT("mcu update car info\r\n");
	
	pthread_mutex_unlock(&s_mcu_data_mutex);
}


/*query ldws and calibration state*/
static void query_ldws_cali_state(void)
{
	s_send_data_buf[0] = RES_MSG;
    s_send_data_buf[1] = T3_RES_MCU;
    s_send_data_buf[2] = 0x06;
    s_send_data_buf[3] = 0x03;
    s_send_data_buf[4] = PRIORITY;
	s_send_data_buf[5] = 0;    //ldws switch
	s_send_data_buf[6] = 0; //cali state
	
    fill_msg(s_send_data_buf,7,1);	
}

/* deal control msg ,*/
static void deal_control_msg(U8 *data)
{
	U8 data_mode = 0;
	switch(data[2]) 
	{
		case MCU_CTL_MODE:	//set mode
			ack_message();
			data_mode = data[5]&0xf0;
			switch (data_mode)
			{
				case MCU_CTL_MODE_2D:
					set_2d_mode((data[5]&0x0f));
					break;
				case MCU_CTL_MODE_3D:
					set_3d_mode((data[5]&0x0f));
					break;
				case MCU_CTL_MODE_PER:
					set_per_mode((data[5]&0x0f));
					break;
				case MCU_CTL_MODE_DVR:
					set_cal_mode((data[5]&0x0f));
					break;
			}
			break;
						
#if (AVM_CFG_LDWS == AVM_ACTIVE)						
		case MCU_CTL_LDWS:				// ldws switch set
			ack_message();
//			set_ldws_switch(data[5]);
			break;
#endif	
					
		case MCU_CTL_CALI:				  // calibration  control
			ack_message();
			if((data[5]&0x0f) == MCU_CTL_CALI_START)
			{
				//start_calibration();    // start auto calibration
			}
			else if((data[5]&0x0f) == MCU_CTL_CALI_STOP)
			{
				//stop_calibration();    // stop calibration
			}
			else if((data[5]&0xf0) == MCU_CTL_CALI_SAVE)
			{
				DEBUGPRINT("start save camera_data\r\n");
				save_camera_data();    // stop calibration
			}
			
			break;		

		case MCU_CTL_UPDATE_RES:
//			response_mcu_update_query(AVM_ON); 
			DEBUGPRINT("mcu query update\r\n");
			break;
			
#if (AVM_CFG_DVR == AVM_ACTIVE)

		case MCU_CTL_DVR_MODE:			
//			set_dvr_mode(data[5]);
			ack_message();
			break;
			
		case MCU_CTL_DVR_RECORD:
//			dvr_record_control(data[5]);
			ack_message();
			break;
			
		case MCU_CTL_PLAYBACK:
//			enter_playback_interface(data[5]);
			ack_message();
			break;
			
		case MCU_CTL_FOLDER:
		//	enter_folder(data[5]);
			ack_message();
			break;
			
		case MCU_CTL_BACK:
		//	back_previous(data[5]);
			ack_message();
			break;
			
		case MCU_CTL_FILP_PAGES:
	//		flip_pages(data[5]);
			ack_message();
			break;

		case MCU_CTL_DVR_PLAY:
	//		play_control(data[5],data[6]);
			ack_message();
			break;
			
#endif

		default:
			set_2d_mode(MCU_CTL_MODE_2D);	
			break;
	}
}

/* deal notify msg*/
static void deal_notify_msg(U8 *data) 
{
	switch(data[2])
    {
       case MCU_NTFY_CAR_INFO:			// car info
			update_car_info(data);				                
			break;										                

		default :
			break;

    } 
}
static void update_car_info(U8 *data)
{
	
	if (data[3] == 0x06)                 //车状态信息6个字节
	{
		set_car_info(data);		
//		set_avm_state(CAR_INFO_UPDATE,AVM_ON);	
	}
}

/*deal the query message */
static void deal_query_msg(U8 *data)
{
	switch(data[2])
    {
    	case QUE_LDWS_CALI_STATE:
  //  		query_ldws_cali_state(); // query ldws and calibration state
			break;
			
		case QUE_FAULT_STATE:					// query error info
   // 		response_fault_info();
			break;
			
		default:
	//		ack_message();
			break;
    }
}
/* deal response msg*/
static void deal_response_msg(U8 *data) 
{
	switch(data[2])
    {
    	case MCU_RES_UPDATE_QUE:
			DEBUGPRINT("mcu response update query\r\n");
    //		check_mcu_response_update_query(data);				
			break;
			
		case MCU_RES_UPDATE_DATA:
			DEBUGPRINT("mcu response update data\r\n");
    	//	check_mcu_response_update_data(data);			
			break;
			
		case MCU_RES_UPDATE_OVER:
			DEBUGPRINT("mcu response update over\r\n");
    	//	check_mcu_response_update_over(data);			
			break;	
			
		default:
			//ack_message();
			break;
    }
}

/*deal receive data*/
static void deal_receive_msg(U8 *data,U8 length)
{	
	int i = 0;
	DEBUGPRINT("deal_receive_msg:");
	for(i = 0;i<length;i++)
	{
		DEBUGPRINT(" %02x ",data[i]);

	}
	DEBUGPRINT("\r\n");
    switch(data[0])                  //msg type
    {
        case CTL_MSG:                // control msg       
						
			if (data[1] == MCU_CTL_T3)    
			{	
				deal_control_msg(data);									 						
			}	
			else
			{				
				DEBUGPRINT("the control data is error\r\n");				
			}				
            break;
			
        case NTFY_MSG:         // notify msg 
        	 //ack_message();
			 if ((data[1] == MCU_NTF_T3))	 
           	 {	
           	 	deal_notify_msg(data);           		               
 	         }
			 else
			 {				
				DEBUGPRINT("the notify data is error\r\n");				
			 }
             break;
			
        case QUERY_MSG:               // query msg    
        	if ((data[1] == MCU_QUERY_T3))	 
           	 {
           	 	deal_query_msg(data);				
        	 }
			else
			 {				
				DEBUGPRINT("the query data is error\r\n");				
			 }
            
            break;
			
        case RES_MSG:       
			if ((data[1] == MCU_RES_T3))	 
           	 {	
           	 	deal_response_msg(data);           		               
 	         }
			 else
			 {				
				DEBUGPRINT("the response data is error\r\n");				
			 }
             break;

        case ACK_MSG:
//			deal_ack_msg(s_recv_msg_rc);				
            break;

		default:
			logg(MESSAGE,"deal_receive_msg:the data is wrong data[0]:%02x,data[1]:%02x\r\n",data[0],data[1]);
			break;

    }

//	mcu_msg_rc_check();//check msg is losed?
 }

/*check data and  */
static void check_recv_data(U8*buf,U8 count)
{
	S32 i=0;
	static U8 flag = 0;
	static U8 buf_deal[128] = {0};
	static U8 valid_data_length = 0;
	static U8 step = 0;
	static U8 cks = 0;
	static U8 num = 0;

	for (i=0; i< count; i++)
	{		 
		switch(step)
		{
			case MCU_MSG_HEADF:
				if(buf[i] == 0xFF) 
				{
					step = MCU_MSG_HEADS;
				}
				break;
			case MCU_MSG_HEADS:
				if(buf[i] == 0xA5)
				{
					step = MCU_MSG_HEADT;
				}
				else
				{
					step = MCU_MSG_HEADF;
				}
				break;
			case MCU_MSG_HEADT:
				if(buf[i] == 0x5A)
				{
					step = MCU_MSG_RC;
				}
				else
				{
					step = MCU_MSG_HEADF;
				}
				break;
			case MCU_MSG_RC:							// Rolling Code			
				s_recv_msg_rc = buf[i];
				cks = s_recv_msg_rc;
				step = MCU_MSG_LEN;				
				break;
			case MCU_MSG_LEN:							//get data length
				valid_data_length = buf[i];
				cks ^= valid_data_length;
				step = MCU_MSG_DATA;
				break;
			case MCU_MSG_DATA:							//get data				
				buf_deal[num] = buf[i];
				cks ^= buf_deal[num];
				num++;
				if(num == valid_data_length)				
				{
					step = MCU_MSG_CKS;
				}				
				break;
			case MCU_MSG_CKS:							//check cks
				if(cks == buf[i])
				{
					deal_receive_msg(buf_deal,valid_data_length);
				}
				num = 0; 
				step = MCU_MSG_HEADF;
				memset(buf_deal, 0, sizeof(buf_deal));
				break;
			
			default:
				break;
		}
		
	}
}

/*read serial msg*/
static S32 get_mcu_data(U8 *buf)
{
	S32 count = 0;
	S32 ret;
	struct timeval tv; //timeout timer
	fd_set oldfds;
	tv.tv_sec = 1;  //1s ,wait 1s
	tv.tv_usec = 0; //us
		
	FD_ZERO(&oldfds);
	FD_SET(s_comfd, &oldfds);
    ret = select(s_comfd + 1, &oldfds, NULL, NULL, &tv);   //non-block
    if (ret) //check change flag
	{  
        //为多串口的操作监听函数操作，这句判断则表明如果是当前这个串口的  
        //文件操作符有数据响应，并执行以下步骤，如果不是则退出本次循环！  
	    if(!FD_ISSET(s_comfd,&oldfds))  
		{
			return count;//continue;  
	    }    	
        count = read(s_comfd, buf, MSG_RECEIVE_MAX);  
    }
	else
	{		
	}
	return count;
}

/*set and open uart*/
static S32 com_open( void )
{
	S32 iFd;
	struct termios opt;
	iFd = open(SERIAL_COM, O_RDWR | O_NOCTTY);
    if(iFd < 0) 
	{
        logg(ERR,"Open uart error!\n", __FUNCTION__, __LINE__);  
        return -1;
    }

    tcgetattr(iFd, &opt);

     cfsetispeed(&opt, B115200);
     cfsetospeed(&opt, B115200);

    /* raw mode,N81*/
     
    opt.c_lflag   &=   ~(ECHO   |   ICANON   |   IEXTEN   |   ISIG);
    opt.c_iflag   &=   ~(BRKINT   |   ICRNL   |   INPCK   |   ISTRIP   |   IXON);
    opt.c_oflag   &=   ~(OPOST);
    opt.c_cflag   &=   ~(CSIZE   |   PARENB);
    opt.c_cflag   |=   CS8;

	fcntl(iFd,F_SETFL,0);
    /*
     * 'DATA_LEN' bytes can be read by serial
     */
    opt.c_cc[VMIN]   =   0;
    opt.c_cc[VTIME]  =   0;

    if (tcsetattr(iFd,   TCSANOW,   &opt)<0) {
        return   -1;
    }
    return iFd;
}

/*serial send msg*/
static void serial_send(U8 *buf,U32 data_length)                       
{
	S32 i = 0;
	S32 ret = 0;
	ret = write(s_comfd, buf, data_length );     //send
	if(ret < 0)
	{
		logg(ERR,"write uart error!\n", __FUNCTION__, __LINE__); 
	}

}

/*fill data and calculate the check digit */
static void fill_msg(U8 *data, U8 length,U8 ack_type)
{

    S32 i = 0;
    U8 cks = 0;
    U8 tmp_send_buffer[150] = {0};
    tmp_send_buffer[0] = (U8)0xFF;

    tmp_send_buffer[1] = (U8)0xA5;
    tmp_send_buffer[2] = (U8)0x5A;
	/*calculate the rolling code*/
	if (ack_type == 0)
	{
		 tmp_send_buffer[3] = (U8)s_send_msg_rc;			 
		  if(s_send_msg_rc == 0xFF)//max number is 0xFF(255)
		    {
		         s_send_msg_rc= 0;
		    }
		    else
		    {
		         s_send_msg_rc++;				 
		    }
	}
	else
	{      
		tmp_send_buffer[3] = (U8)s_recv_msg_rc;// reponse mcu msg  number		         
	}
	
    tmp_send_buffer[4] = (U8)length;   // data length

	/*calculate the check digit*/
    for (i=0; i < length; i++)
    {
        tmp_send_buffer[i+5] = data[i];
    }
    cks = tmp_send_buffer[3]^length;
    for (i=0; i<length; i++)
    {
        cks = cks^data[i];
    }
    tmp_send_buffer[length+5] = cks;
	
    serial_send(tmp_send_buffer,length+6);	
	
	
}


/*ack msg*/
static void ack_message(void)                  //回应消息
{
    s_send_data_buf[0] = 0xff;
    fill_msg(s_send_data_buf,1,1); 
}


/*update  dvr info*/
static void send_dvr_state(void)
{
    s_send_data_buf[0] = 0x02;
    s_send_data_buf[1] = 0x24;
    s_send_data_buf[2] = 0x02;
    s_send_data_buf[3] = 0x02;
    s_send_data_buf[4] = 0x00;
	s_send_data_buf[5] = 0x40;
    fill_msg(s_send_data_buf,6,0);
    
}



