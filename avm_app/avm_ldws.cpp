/*******************************************************************************
* File Name          : avm_ldws.cpp
* Author             : Daich
* Revision           : 2.1
* Date               : 03/07/2018
* Description        : ldws logic code
*
* HISTORY***********************************************************************
* 03/07/2018  | sDas Basic 2.0                       | Zhangh
*
*******************************************************************************/
#include <stdio.h>
#include <string.h>

/* OPEN TexDirectVIV funtion */
#if !defined(EGL_EGLEXT_PROTOTYPES)
	# define EGL_EGLEXT_PROTOTYPES
#endif

#if !defined(GL_GLEXT_PROTOTYPES)
	#define GL_GLEXT_PROTOTYPES
#endif

#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"

#include "avmlib_sdk.h"
#include "avm_config.h"
#include "avm_view_config.h"

#include "log.h"
#include <unistd.h>
#include "avm_common.h"
#include "avm_ldws.h"
#include "stdlib.h"
#include "avmlib_ldws.h"

#ifdef T3BOARD

#include "ion_lib.h"
#include "g2d_hal.h"
#include "t3_fb.h"

#endif
/*
*************************************************************************
*************************************************************************
*                    LOCAL  STRUCT / E N U M E R A T I O N S
*************************************************************************
*************************************************************************
*/
ldws_usr_t g_ldws_usr;

#ifdef T3BOARD

ion_buf_t *ldws_ion_buffer_before = NULL;
ion_buf_t *ldws_ion_buffer_after = NULL;

void ldws_data_get(void)
{
	if((ldws_ion_buffer_before == NULL)||(ldws_ion_buffer_after == NULL))
	{
		ldws_ion_buffer_before = (ion_buf_t*)malloc(sizeof(ion_buf_t));
		ldws_ion_buffer_after = (ion_buf_t*)malloc(sizeof(ion_buf_t));

		ion_alloc( (int)(LDWS_VIEW_WIDTH * LDWS_VIEW_HEIGHT * 4 * sizeof(GLubyte)), ldws_ion_buffer_before);
		ion_alloc( (int)(LDWS_VIEW_WIDTH * LDWS_VIEW_HEIGHT * 4 *sizeof(GLubyte)), ldws_ion_buffer_after);
		if((ldws_ion_buffer_before == NULL)||(ldws_ion_buffer_after == NULL))
		{
			DbgPrintf("ldw_buffer calloc error \n");
		}
	}	
	display_buffer_get(LDWS_VIEW_X, LDWS_VIEW_Y, LDWS_VIEW_WIDTH, LDWS_VIEW_HEIGHT, (unsigned char*)ldws_ion_buffer_before->ion_vir_addr);

	g2d_roi_t g2d_para;
	g2d_para.src.roi_width	= LDWS_VIEW_WIDTH;
	g2d_para.src.roi_height = LDWS_VIEW_HEIGHT;
	g2d_para.src.roi_x = 0;
	g2d_para.src.roi_y = 0;
	g2d_para.src_width		= LDWS_VIEW_WIDTH;
	g2d_para.src_height 	= LDWS_VIEW_HEIGHT;
	g2d_para.dst.roi_width	= LDWS_VIEW_WIDTH;
	g2d_para.dst.roi_height = LDWS_VIEW_HEIGHT;
	g2d_para.dst.roi_x = 0;
	g2d_para.dst.roi_y = 0;
	g2d_para.dst_width = LDWS_VIEW_WIDTH;
	g2d_para.dst_height = LDWS_VIEW_HEIGHT;

	g2d_rgba2yuv(ldws_ion_buffer_before->ion_phy_addr, ldws_ion_buffer_after ->ion_phy_addr, g2d_para);


}

void ldws_data_update(void)
{
	memcpy(g_ldws_usr.ldws_buf,ldws_ion_buffer_after->ion_vir_addr,LDWS_VIEW_WIDTH * LDWS_VIEW_HEIGHT);				
}

#endif

void *ldws_alg_thread(void *argv)
{
	ldws_type_t ldws_obj;

	g_ldws_usr.ldws_w = 256;
	g_ldws_usr.ldws_h = 480;
	g_ldws_usr.ldws_left_warning_status = WARNING_OFF;
	g_ldws_usr.ldws_right_warning_status = WARNING_OFF;
	g_ldws_usr.ldws_buf = (unsigned char *)malloc(g_ldws_usr.ldws_w * g_ldws_usr.ldws_h);
	
	ldws_obj.ldws_input_buf 							= g_ldws_usr.ldws_buf;
	ldws_obj.ldws_input_info.vehicle_width 				= 1800;
	ldws_obj.ldws_input_info.vehicle_length 			= 5000;
	ldws_obj.ldws_input_info.vehicle_speed 				= 70;
	ldws_obj.ldws_input_info.vehicle_signal 			= TURN_SIGNAL_NONE;
	ldws_obj.ldws_input_info.vehicle_steering_angle 	= 0;
	ldws_obj.ldws_input_info.ldws_sensitivity 			= SENSITIVITY_HIGH;
	ldws_obj.ldws_input_info.avm_scale 					= 23;
	ldws_obj.ldws_input_info.avm_mask_widht				= 80;
	ldws_obj.ldws_input_info.avm_mask_length			= 100;
	
	ldws_obj.ldws_input_info.ldws_strategy				= STRATEGY_SINGLE_LINE;

	avmlib_ldws_init(&ldws_obj); 
	 
	if(0 != sem_init(&g_ldws_usr.sem_require_ydata, 0, 0))
	{
		logg(ERR, "ldw_alg_thread sem_init error: %s, %d\n", __FUNCTION__, __LINE__);
	}
	if(0 != sem_init(&g_ldws_usr.sem_response_ydata, 0, 0))
	{
		logg(ERR, "ldw_alg_thread sem_init error: %s, %d\n", __FUNCTION__, __LINE__);
	} 
	
	
	if(0 != sem_post(&g_ldws_usr.sem_require_ydata))
	{
		logg(ERR, "ldw_alg_thread sem_post failed %s, %d\n",__FUNCTION__, __LINE__);
	}

	while(1)
	{
		//wait
		if(0 != sem_wait(&g_ldws_usr.sem_response_ydata))
        {
                logg(ERR, "ldw_alg_thread sem_wait failed %s, %d\n",__FUNCTION__, __LINE__);
			
            continue;
        }

		avmlib_ldws_process(&ldws_obj);
		
		g_ldws_usr.ldws_left_warning_status = ldws_obj.ldws_output_info.ldws_left_warning_status;
		g_ldws_usr.ldws_right_warning_status = ldws_obj.ldws_output_info.ldws_right_warning_status;

		  /* DbgPrintf("left = %d, dist = %d, warn = %d, 	right = %d, dist = %d, warn = %d\n",
													ldws_obj.ldws_output_info.ldws_left_detect_status,
													ldws_obj.ldws_output_info.ldws_left_dist,
													ldws_obj.ldws_output_info.ldws_left_warning_status,
													ldws_obj.ldws_output_info.ldws_right_detect_status,
													ldws_obj.ldws_output_info.ldws_right_dist,
													ldws_obj.ldws_output_info.ldws_right_warning_status); */
		 
		//post
		if(0 != sem_post(&g_ldws_usr.sem_require_ydata))
        {
			logg(ERR, "ldw_alg_thread sem_post failed %s, %d\n",__FUNCTION__, __LINE__);
        }

		
	}	
}
