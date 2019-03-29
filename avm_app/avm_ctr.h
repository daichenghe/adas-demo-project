/*******************************************************************************
* File Name          : avm_ctr.h
* Author             : Daich
* Revision           : 2.1
* Date               : 03/07/2018
* Description        :
*
* HISTORY***********************************************************************
* 03/07/2018 | sDas Basic 2.0                       | Daich
*
*******************************************************************************/
#ifndef __AVM_CTR_H_
#define __AVM_CTR_H_

typedef enum
{
	CAMERA_FRONT = 0,
	CAMERA_RIGHT = 1,
	CAMERA_BACK  = 2,
	CAMERA_LEFT  = 3,
	CAMERA_MAX
} camera_t;
#define AVM_FLY_ON			(0)
#define AVM_FLY_ON_GOING	(1)
#define AVM_FLY_ON_COMPLETE	(2)
#define AVM_FLY_OFF			(3)

#define AVM_SCREEN_CLEAN_ON		(0)
#define AVM_SCREEN_CLEAN_OFF	(1)

typedef enum mode_2d_sub
{
    MODE_2D_FRONT   = CAMERA_FRONT,
    MODE_2D_RIGHT   = CAMERA_RIGHT,
    MODE_2D_BACK    = CAMERA_BACK,
    MODE_2D_LEFT    = CAMERA_LEFT,
	
    MODE_SV_FRONT    = CAMERA_FRONT,
    MODE_SV_RIGHT    = CAMERA_RIGHT,
    MODE_SV_BACK     = CAMERA_BACK,
    MODE_SV_LEFT     = CAMERA_LEFT,
	
    MODE_BACKUP_FRONT    = CAMERA_FRONT,
    MODE_BACKUP_RIGHT    = CAMERA_RIGHT,
    MODE_BACKUP_BACK     = CAMERA_BACK,
    MODE_BACKUP_LEFT     = CAMERA_LEFT,
	
    MODE_PER_FRONT  = CAMERA_FRONT,
    MODE_PER_BACK   = CAMERA_BACK,
	
    MODE_ORI_FRONT   = CAMERA_FRONT,
    MODE_ORI_RIGHT   = CAMERA_RIGHT,
    MODE_ORI_BACK    = CAMERA_BACK,
    MODE_ORI_LEFT    = CAMERA_LEFT,
    
	MODE_ORI_SPLIT    		= 5,
	MODE_ORI_SAVE_IMAGE    	= 6,
	MODE_ORI_CALI    		= 7,
	
    MODE_NONE,
} MODE2D;

typedef enum
{
    MODE_3D_ROTATE  = 0,
    MODE_3D_LEFTUP   = 1,
    MODE_3D_BACK     = 2,
    MODE_3D_RIGHTUP  = 3,
    MODE_3D_RIGHT    = 4,
    MODE_3D_FRONTRUP = 5,
    MODE_3D_FRONT    = 6,
    MODE_3D_FRONTLUP = 7,
    MODE_3D_LEFT     = 8,

    MODE_NONE_3D,
} MODE3D;

typedef enum avm_mode
{
	MODE_AVM_3D	= 1,   /* 3D环视 */
    MODE_AVM_2D, 	   /* 2D环视 */
    MODE_SV,           /* 单视野 */
    MODE_PER,          /* 潜望镜 */
    MODE_ORI,          /* 标定   */
    MODE_SVFS,         /* 前侧视 */
    MODE_SVBOTHS,      /* 双侧顶视 */
    MODE_SVBS,         /* 倒车镜 */
    MODE_SF,           /* 3D辅助视角 */
    MODE_BACKUP,       /* 倒车 */
    MODE_PER_CENTER,   /* 潜望镜中间 */
    MODE_AVM_LDWS, 	   /* 2D环视 */
    MODE_AVM_BSD, 	   /* 2D环视 */
    MODE_AVM_DVR,
	MODE_MAX,
} AVM_MODE;

typedef struct VIEWKEEP
{
	int mode;
    int mode_last;
    
	int sub_mode;
    int sub_mode_last;
	
    int winclear;
	int fly_flag;
} view_keep_t;

extern view_keep_t g_view_keep;


void *avm_key_input_monitor_thread(void *arg);
void *avm_uart_input_monitor_thread(void *arg);

#endif