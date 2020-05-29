/*******************************************************************************
* File Name          : app.c
* Author             : Daich
* Revision           : 2.1
* Date               : 15/05/2018
* Description        : main
*
* HISTORY***********************************************************************
* 15/05/2018  | sDas Basic 2.0                                            | Daich
*
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#if !defined(EGL_EGLEXT_PROTOTYPES)
	# define EGL_EGLEXT_PROTOTYPES
#endif

#if !defined(GL_GLEXT_PROTOTYPES)
	#define GL_GLEXT_PROTOTYPES
#endif

#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "avmlib_sdk.h"
#include "app.h"
#include "avm_config.h"
#include "log.h"
#include "avm_common.h"
#include "avm_ui.h"
#include "avm_2d_module.h"
#include "avm_3d_module.h"
#include "avm.h"
#include "pthread.h"
#include "syscfg.h"
#include "avm_ctr.h"
#include <sys/time.h>
#include "avm_ldws.h"
#include "avm_bsd.h"
#include "avmlib_GenCaliData.h"
#include "avm_mcu_module.h"
#include "egl_img.h"
#include "avm_line.h"

#ifdef T3BOARD
#include "egl_t3.h"
#include "egl_dvr.h"
#include "DecoderHal.h"//player_lib/player/
#include "CameraHal.h"
#include "RecorderHal.h"
#include "t3_fb.h"
#include "g2d_hal.h"
#else
#include "egl_pc.h"
#endif


#ifdef T3BOARD
CameraHal *ptr_camera_obj;
static DecoderHal *g_player;
static dec_status_ g_plager_state = DEC_STOP;
#endif


/* Private variables */
S32 g_sysquit = AVM_STATE_ACTIVE;


/* Function declaration */
static AVM_ERR signal_catch_handler(void);
static void sighandler(S32 signal);
static AVM_ERR avm_lib_init();
static AVM_ERR Textures_init(GLuint cam_width, GLuint cam_heigh, GLenum Format, void *planes_Logical[], GLuint *planes_physical[]);
static AVM_ERR  RenderInit(void);
static void avm_User_Stub_Textures(GLint TexturesID);
static AVM_ERR default_mode_init(void);
static AVM_ERR thread_create();
static AVM_ERR camera_capture_thread_create(void);
static AVM_ERR common_mode_init(void);
static AVM_ERR avm_cali_init();
static AVM_ERR avm_input_monitor_thread_create(void);
static AVM_ERR avm_ldws_thread_create(void);
static AVM_ERR avm_bsd_thread_create(void);

#ifdef T3BOARD
static void *avm_common_mode_init_thread(void *argv);
static AVM_ERR avm_common_mode_init_thread_create(void);
static int event_callback(wis_event_t *args);
#endif

#ifdef T3BOARD
DecoderHal * get_player(void)
{
	return g_player;

}

/*****************************************************************************
 函 数 名  : get_player_state
 功能描述  : 获取播放器状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : dec_status_ 播放器状态枚举
*****************************************************************************/
dec_status_  get_player_state(void)
{
	return g_plager_state;

}

/*****************************************************************************
 函 数 名  : set_player_state
 功能描述  : 设置播放器状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : dec_status_ 播放器状态枚举
*****************************************************************************/
void set_player_state(dec_status_ state)
{
	g_plager_state = state;

}
static int event_callback(wis_event_t *args)
{
    dec_callback_t *dec_args;
    enc_callback_t *enc_args;
    cam_callback_t *cam_args;
    switch(args->e)
    {
        case DECODER_BUFFER:
            dec_args = (dec_callback_t *)args->buffer;
			g_plager_state = dec_args->dec_status;
            if(dec_args->err == NO_ERROR && dec_args->dec_status == DEC_PLAY)
            {
                update_mp4_buf(&dec_args->video_buf);
            }
            else
            {
            	post_dvr_sem();
	            DbgPrintf("\n********dvr status changed************\n");
	            DbgPrintf("status:%d,error:%d,seekset:%d\n",dec_args->dec_status,dec_args->err,dec_args->info_data.seekset);
            }
            break;
        case RECORDER_BUFFER:
            enc_args = (enc_callback_t *)args->buffer;
            DbgPrintf("file_status:%d,filename:%s\n",enc_args->info_data.new_cutfile.status,enc_args->info_data.new_cutfile.file);
            break;
        case CAM_BUFFER:
            cam_args = (cam_callback_t *)args->buffer;
            get_t3_video_buf(&cam_args->video_buf);
            break;
    }

}

#endif

/*
Function process:
	+ catch the system signal
*/
static AVM_ERR signal_catch_handler(void)
{	
	signal(SIGINT, sighandler);	
	signal(SIGTERM, sighandler);	
	//signal(SIGALRM, sigalrm_fn);
}

/*
Function process:
	+ system signal handler
*/
static void sighandler(S32 signal)
{    
	logg(MESSAGE, "Caught signal %d, setting flaq to quit.\n", signal);    
	g_sysquit = AVM_sTATE_INACTIVE;
}
/*
Function process:
	+ SDASlib init
	+ avm libcode init
	+ texture init
	+ set texture update method
*/

static AVM_ERR avm_lib_init()
{
	
    AVM_ERR libret;
	AVM_ERR ret;
    cali_attr_cfg_t cali_cfg;

	cali_cfg.grid_dx	 = USR_GRID_DX;
	cali_cfg.grid_dy	 = USR_GRID_DY;
    cali_cfg.grid_dist_v = USR_GRID_DIST_V ;
    cali_cfg.grid_dist_h = USR_GRID_DIST_H ;
    cali_cfg.isautocalib = USR_ISAUTOCALIB ;
    cali_cfg.dist_border = USR_DIST_BORDER ;
    libret = avmlib_init_SDASlib((const U8 * const)VBO_LUT_PATH, &cali_cfg);
	if(AVM_ERR_NONE != libret)
	{
		logg(ERR, "avmlib_init_SDASlib: %s, %d ret = %d\n", __FUNCTION__, __LINE__,libret);		
		return libret;
	}
	avmlib_Set_Stub_Textures(&avm_User_Stub_Textures);
    libret = avmlib_Init(CAM_WIDTH, CAM_HEIGHT, SCREEN_WINDOW_WIDTH, SCREEN_WINDOW_HEIGHT, AVM_ON);

	if(libret != AVM_ERR_NONE || ret != AVM_ERR_NONE)
	{
		logg(ERR, "avmlib_Init error: %s, %d\n", __FUNCTION__, __LINE__);
		return libret;
	}

	return AVM_ERR_NONE;
}
static AVM_ERR avm_cali_init()
{

} 

/*
Function process:
	+ default mode  param init
	+ set opengl param
*/

static AVM_ERR default_mode_init(void)
{
	AVM_ERR ret;
	
	ret = default_ui_init();
    ret = default_2d_init();
	ret = default_3d_init();
	ret = default_line_init();
	return AVM_ERR_NONE;
}

/*
Function process:
	+ common mode  param init
	+ set opengl param
*/
static AVM_ERR common_mode_init(void)
{
	AVM_ERR ret;
	ret = common_ui_init();
	ret = common_3d_init();
	ret = common_2d_init();
	ret = common_line_init();
	return AVM_ERR_NONE;
}

/*
Function process:
	+  create init thread
	+ param init
*/
static AVM_ERR thread_create()
{
#ifdef T3BOARD
	avm_common_mode_init_thread_create();
#endif
	avm_input_monitor_thread_create();
	avm_ldws_thread_create();
	avm_bsd_thread_create();
}

/*
Function process:
	+  create keyboard monitor thread
*/
static AVM_ERR avm_input_monitor_thread_create(void)
{
	AVM_ERR rec = AVM_ERR_NONE;
	pthread_t   tid_avm_key_input_monitor_thread;
	pthread_t   tid_avm_uart_input_monitor_thread;
    if(0 != pthread_create(&tid_avm_key_input_monitor_thread, NULL, avm_key_input_monitor_thread, NULL))
    {
        logg(ERR, "Create avm_input_monitor_thread_create  error: %s, %d\n", __FUNCTION__, __LINE__);
        rec = AVM_ERR_FALSE;
    }
#ifdef T3BOARD    
	mcu_thread_create();
#endif

}

/*
Function process:
	+  create ldws thread
*/
static AVM_ERR avm_ldws_thread_create(void)
{
	AVM_ERR rec = AVM_ERR_NONE;
	pthread_t   tid_avm_ldws_thread;
    if(0 != pthread_create(&tid_avm_ldws_thread, NULL, ldws_alg_thread, NULL))
    {
        logg(ERR, "Create avm_ldws_thread_create  error: %s, %d\n", __FUNCTION__, __LINE__);
        rec = AVM_ERR_FALSE;
    }

}

/*
Function process:
	+  create bsd thread
*/
static AVM_ERR avm_bsd_thread_create(void)
{
	AVM_ERR rec = AVM_ERR_NONE;
	pthread_t   tid_avm_bsd_thread;
    if(0 != pthread_create(&tid_avm_bsd_thread, NULL, bsd_alg_thread, NULL))
    {
        logg(ERR, "Create avm_ldws_thread_create  error: %s, %d\n", __FUNCTION__, __LINE__);
        rec = AVM_ERR_FALSE;
    }

}

/*
Function process:
	+  avm para init
*/
void avm_init(void) 
{
	int ret = -1;	
//	system("insmod ds28e15-gpio.ko");
//	system("insmod dma_interface.ko");
	ret = avm_lib_init(); 
	if(AVM_ERR_NONE != ret)    
	{		
		logg(ERR, "avm_lib_init error: %s, %d\n", __FUNCTION__, __LINE__);				
		log_close();		
	} 
	/* default mode init */	
	ret = default_mode_init();	  
	if(AVM_ERR_NONE != ret)	 
	{		 
		logg(ERR, "default_mode_init error: %s, %d\n", __FUNCTION__, __LINE__);		   
		log_close(); 	  
	//	return -1;	
	}
#ifndef T3BOARD
	common_mode_init();
#endif
	thread_create();
}
	



int main(int argc, char *argv[])
{

	AVM_ERR ret = AVM_ERR_NONE;
	static int fn = 0;
    int total_time;
    struct timeval tv_start;
    struct timeval tv_current;
	DbgPrintf("v1.1.0\n");
	/* start log function */
    log_open();
    logg(MESSAGE,"SDK Version :%s\n", "AVM_BASIC_V1.1.0");

	/* system signal processing */		
#ifdef T3BOARD

#ifndef IMAGE_DEBUG
	ptr_camera_obj = new CameraHal(); //CameraHal() RecorderHal() DecoderHal()
    ptr_camera_obj->setEventCallback(event_callback);
    g_player = new DecoderHal();
    g_player->setEventCallback(event_callback);
	
#endif
	screen_fb_init();
	g2d_init();
	dvr_es_init();
	es_init();
	avm_init();
	dmafd_init();  
    dvr_es_init();

#if 0	
    RecorderHal *recorder = new RecorderHal();
    recorder->setFileFolder("/mnt/sdcard/mmcblk1p1/mp4");
    recorder->setFileDurationSeconds(300);
    recorder->setOSD(1);
    recorder->setEventCallback(event_callback);
    recorder->startRecording();
#endif
	

#else

	bsp_pc_init();
	avm_init();
	
#endif
	while(1)
	{	
#ifndef T3BOARD
		texture_update();
#else
		update_dma_texture();		
#endif
		avm_render();
		egl_swap();
        fps_calc();

	}

}
#ifdef T3BOARD

/*
Function process:
	+  create common mode init thread
*/
static void *avm_common_mode_init_thread(void *argv)
{
	enable_new_context();
	common_mode_init();	
	disenable_new_context();
}

/*
Function process:
	+  common mode init thread
*/
static AVM_ERR avm_common_mode_init_thread_create(void)
{
	AVM_ERR rec = AVM_ERR_NONE;
	pthread_t   tid_avm_common_mode_thread;
    if(0 != pthread_create(&tid_avm_common_mode_thread, NULL, avm_common_mode_init_thread, NULL))
    {
        logg(ERR, "Create avm_common_mode_init_thread  error: %s, %d\n", __FUNCTION__, __LINE__);
        rec = AVM_ERR_FALSE;
		
    }

}
#endif


