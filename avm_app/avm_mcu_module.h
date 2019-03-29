/******************************************************************************
* File Name          : avm_mcu_module.h
* Author             : DUUF
* Revision           : 2.1
* Date               : 03/07/2018
* Description        :
*
*******************************************************************************/
#ifndef __AVM_SERIAL_H__
#define	__AVM_SERIAL_H__

#include "avmlib_sdk.h"
//*****************************************/
#define MCU_CTL_T3 		(0x13)    
#define MCU_NTF_T3 		(0x23)
#define T3_NTF_MCU 		(0x24)
#define MCU_QUERY_T3 	(0x33)
#define MCU_RES_T3 		(0x43)
#define T3_RES_MCU 		(0x44)

#define PRIORITY			(0x00)

//********************MSG Define**********************/
#define CTL_MSG  			(0x01)
#define NTFY_MSG  			(0x02)
#define QUERY_MSG  			(0x03)
#define RES_MSG  			(0x04)		
#define ACK_MSG  			(0xFF)

//*****************CTL_MSG Define*************************/
#define MCU_CTL_MODE 		(0x01)
#define MCU_CTL_LDWS 		(0x02)
#define MCU_CTL_CALI 		(0x03)
#define MCU_CTL_UPDATE_RES	(0x08)
#define MCU_CTL_DVR_MODE 	(0x09)
#define MCU_CTL_DVR_RECORD	(0x0A)
#define MCU_CTL_PLAYBACK 	(0x0B)
#define MCU_CTL_FOLDER 		(0x0C)
#define MCU_CTL_BACK		(0x0D)
#define MCU_CTL_FILP_PAGES 	(0x0E)
#define MCU_CTL_DVR_PLAY 	(0x0F)

//******************CTL_MODE Define***********************/
#define MCU_CTL_MODE_2D 		(0x00)
#define MCU_CTL_MODE_3D 		(0x10)
#define MCU_CTL_MODE_PER 		(0x20)
#define MCU_CTL_MODE_DVR 		(0x30)

#define MCU_CTL_MODE_LDWS 		(0x21)

#define MCU_CTL_CALI_START		(0x01)
#define MCU_CTL_CALI_STOP		(0x02)
#define MCU_CTL_CALI_SAVE		(0x10)

//******************NTFY_MSG Define***********************/
#define T3_NTFY_ALARM	 		(0x01)
#define MCU_NTFY_CAR_INFO 		(0x04)

//******************QUE_MSG Define***********************/
#define QUE_LDWS_CALI_STATE	 	(0x06)
#define QUE_FAULT_STATE	 		(0x0B)

//******************RES_MSG Define***********************/
#define MCU_RES_UPDATE_QUE	 	(0x07)
#define T3_RES_UPDATE_QUE	 	(0x08)
#define MCU_RES_UPDATE_DATA	 	(0x09)
#define MCU_RES_UPDATE_OVER	 	(0x0A)

//******************PLAYER_CTL_MSG Define***********************/
#define PLAYER_PLAY_VEDIO 		0x01
#define PLAYER_PAUSE 			0x02
#define PLAYER_PLAY 			0x03
#define PLAYER_REPLAY 			0x04
#define PLAYER_PLAY_PERVIOUS 	0x10
#define PLAYER_PLAY_NEXT 		0x20

//********************************************************/
#define PATH_SDCARD  "/mnt"
#define PATH_SD_DEV  "/dev/mmcblk1"
#define REPEAT_MESSAGE_NUMBER (16)
#define REPEAT_NUM  (2)
#define REPEAT_OUTTIME (100000) 

typedef struct _Coordinate_State
{
    U32            touch_x; // x зјБъ
    U32            touch_y; // y зјБъ
} Coordinate_State_S;

typedef struct _Date_Time
{
  U8 year;
  U8 mouth;
  U8 date;
  U8 hour;
  U8 minute;
  U8 second;
}Date_Time_S;

typedef enum 
{
    MCU_UPGRATE_START   = 0, 
    MCU_UPGRATE_ING,
    MCU_UPGRATE_FINISH,
} MODE_MCU_UPGRATE_STATE;


enum
{
 	MCU_MSG_HEADF = 0,//check msg head 1
	MCU_MSG_HEADS ,		//check msg head 2
	MCU_MSG_HEADT ,		//check msg head 3
	MCU_MSG_RC ,		//get msg rolling code
	MCU_MSG_LEN ,		//get data len
	MCU_MSG_DATA,		//get data
	MCU_MSG_CKS,		// check
};

typedef struct _Car_State
{
  U8 left_turn_light;		//left turn light state
  U8 right_turn_light;	 	//right turn light state
  U8 steering_wheel;   	//direction of the steering wheel
  U16 steering_angle;  	//angle of the steering wheel
  U16 car_speed;			//speed of the car
  
}Car_State_S;

/*resend msg function*/
extern void deal_resend(void);
/*get car info */
extern Car_State_S get_car_info(void);
extern U8 get_turn_light_state(void);

/* serial receive thread loop*/
extern int  mcu_thread_create(void);
extern void send_upgrate_file_info(U8 state);
extern S32 get_touchscreen_press_falg(void);
extern S32 get_touchscreen_press_x(void);
//ldws alarm
extern void send_ldws_alarm_msg(void);           
extern U32 get_mcu_update_size(void);
extern U32 get_mcu_update_progress(void);

/*find update file and notify mcu update*/
extern void check_mcu_update_file(void);  
/*open 3d filter*/
extern S32 open_3d_filter(S32 num);

#endif /* __AVM_SERIAL_H__ */
