/*******************************************************************************
* File Name          : avm.cpp
* Author             : Daich
* Revision           : 2.1 
* Date               : 15/05/2018
* Description        : avm control code
*
* HISTORY***********************************************************************
* 15/05/2018  | sDas Basic 2.0                                            | Daich
*
*******************************************************************************/
#include "stdio.h"
#include "avm.h"
#include "avm_ui.h" 
#include "log.h"
#include "avm_2d_module.h"
#include "avm_3d_module.h"
#include "avm_ctr.h"
#include "SOIL.h"
#include "avm_ldws.h"
#include "avm_bsd.h"
#include <pthread.h>
#include <semaphore.h> 
#include "app.h"
#include "avm_line.h"
#include "avm_ldws.h"
#include "avm_config.h"
#include "avm_view_config.h"

#ifdef T3BOARD
#include "egl_dvr.h"
#include "DecoderHal.h"//player_lib/player/
#endif



static pthread_mutex_t avm_view_mutex;
static unsigned char disp_addr[SCREEN_WINDOW_WIDTH*SCREEN_WINDOW_HEIGHT*4];


static void avm_screen_clean();
static void save_bmp(const char * filename, unsigned char * src, int width, int height);
static void rgba2y_ldws_egl(unsigned char * src, unsigned char * dst);
static void save_bmp_y(const char * filename, unsigned char * src, int width, int height);
static void rgba2y_bsd_l_egl(unsigned char * src, unsigned char * dst);
static void rgba2y_bsd_r_egl(unsigned char * src, unsigned char * dst);

static AVM_ERR avm_bsp_set_screen_off(void)
{
	
}
static AVM_ERR avm_bsp_set_screen_on(void)
{
	
}

/*
Function process:
	+  avm_view mutex init lock unlock
*/
AVM_ERR avm_view_mutex_init(void)
{	
	pthread_mutex_init(&avm_view_mutex, NULL);
	return  AVM_ERR_NONE ;
}

void avm_view_mutex_lock(void)
{
	pthread_mutex_lock(&avm_view_mutex);
}

void avm_view_mutex_unlock(void)
{
	pthread_mutex_unlock(&avm_view_mutex);
}


/*
Function process:
	+  avm render by mode
*/
AVM_ERR avm_render(void)
{
	AVM_ERR ret;
	static int ui_flag = -1;
	unsigned char *egl_disp_addr;
	static int bsd_disp_cnt = 0;
	
#ifdef T3BOARD

	dec_status_ player_state = get_player_state();

#endif
	if(g_view_keep.winclear == AVM_SCREEN_CLEAN_ON)
	{
		g_view_keep.winclear = AVM_SCREEN_CLEAN_OFF;
		avm_screen_clean();
		
	}
	avm_view_mutex_lock();
//	DbgPrintf("mode  %d subemode %d\n",g_view_keep.mode, g_view_keep.sub_mode);
	switch(g_view_keep.mode)
	{
		case MODE_AVM_2D:
			ret = render_2d_avm();
			render_single_view(g_view_keep.sub_mode);
			ui_flag = -1;
			break;
		case MODE_SV:
			render_single_view(g_view_keep.sub_mode);
			ui_flag = -1;
			break;
		case MODE_PER:
			render_periscope_view(g_view_keep.sub_mode);
			ui_flag = -1;
			break;
		case MODE_ORI:
			switch(g_view_keep.sub_mode)
			{
				case MODE_ORI_FRONT:
				case MODE_ORI_RIGHT:
				case MODE_ORI_BACK:
				case MODE_ORI_LEFT:
					render_ori_view(g_view_keep.sub_mode);
					break;
				case MODE_ORI_SPLIT:
					render_ori_split_view();
					break;
				case MODE_ORI_SAVE_IMAGE:
					break;
				case MODE_ORI_CALI:
					break;
			}
			ui_flag = -1;
			break;
		case MODE_SVFS:
			render_2d_avm();
			render_svfs_view();
			ui_flag = -1;
			break;
		case MODE_BACKUP:
			render_backup_view(g_view_keep.sub_mode);
			ui_flag = -1;
			break;
		case MODE_AVM_3D:
			render_2d_avm();
			avm_3d_set_view_parameter(&g_view_keep);
			render_3d_avm((MODE3D)g_view_keep.sub_mode);
			ui_flag = -1;
			break;
		case MODE_AVM_DVR:
#ifdef T3BOARD
		
			dvr_es_render(player_state);
			
#endif
			break;
		case MODE_AVM_LDWS:
#ifdef T3BOARD

			render_ldws_avm();
			ldws_data_get();
			if(0 != sem_wait(&g_ldws_usr.sem_require_ydata))
			{
					logg(ERR, "ldw_alg_thread sem_wait failed %s, %d\n",__FUNCTION__, __LINE__);
		
			}
			ldws_data_update();
			
			//post
			if(0 != sem_post(&g_ldws_usr.sem_response_ydata))
			{
				logg(ERR, "ldw_alg_thread sem_post failed %s, %d\n",__FUNCTION__, __LINE__);
			}
#else
			render_ldws_avm();
			glReadPixels(0, 0, SCREEN_WINDOW_WIDTH, SCREEN_WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE,  disp_addr);
			//wait
			if(0 != sem_wait(&g_ldws_usr.sem_require_ydata))
			{
					logg(ERR, "ldw_alg_thread sem_wait failed %s, %d\n",__FUNCTION__, __LINE__);
		
			}
			rgba2y_ldws_egl(disp_addr,g_ldws_usr.ldws_buf);
			
			//post
			if(0 != sem_post(&g_ldws_usr.sem_response_ydata))
			{
				logg(ERR, "ldw_alg_thread sem_post failed %s, %d\n",__FUNCTION__, __LINE__);
			}
#endif	

/*
			if(g_ldws_usr.ldws_left_warning_status == WARNING_ON)
			{
				draw_ui(UI_LDWS_L);
			}
			else if(g_ldws_usr.ldws_right_warning_status == WARNING_ON)
			{
				draw_ui(UI_LDWS_R);

			}
			else
			{
				draw_ui(UI_LDWS_N);
			}
		*/
			break;
		case MODE_AVM_BSD:

			render_bsd_avm();
			
#ifdef T3BOARD

			bsd_data_get(disp_addr);
#else	
			glReadPixels(0, 0, SCREEN_WINDOW_WIDTH, SCREEN_WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE,  disp_addr);
#endif			//wait
			if(0 != sem_wait(&g_bsd_usr.sem_require_ydata))
			{
					logg(ERR, "ldw_alg_thread sem_wait failed %s, %d\n",__FUNCTION__, __LINE__);
		
			}
			rgba2y_bsd_l_egl(disp_addr,g_bsd_usr.bsd_left_buf);
			rgba2y_bsd_r_egl(disp_addr,g_bsd_usr.bsd_right_buf);
			//post
			if(0 != sem_post(&g_bsd_usr.sem_response_ydata))
			{
				logg(ERR, "ldw_alg_thread sem_post failed %s, %d\n",__FUNCTION__, __LINE__);
			}
#if 0
	
			if(g_bsd_usr.bsd_left_warning_status == BSD_WARNING_ON && g_bsd_usr.bsd_right_warning_status == BSD_WARNING_ON)
			{
				bsd_disp_cnt = 20;
				draw_ui(UI_BSD_D);
			}
			else if(g_bsd_usr.bsd_left_warning_status == BSD_WARNING_OFF && g_bsd_usr.bsd_right_warning_status == BSD_WARNING_OFF)
			{
				bsd_disp_cnt--;
				if(bsd_disp_cnt <= 0)
				{
					bsd_disp_cnt = 0;
				}
				if(bsd_disp_cnt <=0)
				{	
					draw_ui(UI_BSD_N);
				}
	
			}
			else if(g_bsd_usr.bsd_left_warning_status == BSD_WARNING_ON)
			{
				bsd_disp_cnt = 20;
				
				draw_ui(UI_BSD_L);
			}
			else if(g_bsd_usr.bsd_right_warning_status == BSD_WARNING_ON)
			{
				bsd_disp_cnt = 20;
				draw_ui(UI_BSD_R);
			}
#endif
			break;
		
	}
	line_handler(&g_view_keep);
	avm_view_mutex_unlock();
	return ret;
}

/*
Function process:
	+  clean the screen
*/
static void avm_screen_clean()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepthf(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


/*
Function process:
	+  ldws rgba to y
*/
static void rgba2y_ldws_egl(unsigned char * src, unsigned char * dst)
{
	int x,y;
	int pix_addr;
	unsigned char src_temp[SCREEN_WINDOW_WIDTH*SCREEN_WINDOW_HEIGHT*4];
	memcpy(src_temp,src,SCREEN_WINDOW_WIDTH*SCREEN_WINDOW_HEIGHT*4);
	
	for(y = 0; y < 480; y++)
	{
		for(x = 0; x < 256; x++)
		{
			pix_addr = ((SCREEN_WINDOW_HEIGHT- 1 - y) * SCREEN_WINDOW_WIDTH + x) * 4;
			dst[y * 256 + x] = (76 * src_temp[pix_addr] + 150 * src_temp[pix_addr + 1] + 29 * src_temp[pix_addr + 2]) >> 8;
		}
	}
	
}
#ifndef T3BOARD
static void rgba2y_bsd_l_egl(unsigned char * src, unsigned char * dst)
{
	int x,y;
	int pix_addr;
	int avm_sx = BSD_VIEW_X;
	int avm_sy = SCREEN_WINDOW_HEIGHT - BSD_VIEW_HEIGHT ;
	int avm_ex = avm_sx + BSD_VIEW_WIDTH;
	int avm_ey = SCREEN_WINDOW_HEIGHT;
	
	int src_x;
	int src_y;
	
	int bsd_w = g_bsd_usr.bsd_left_w;
	int bsd_h = g_bsd_usr.bsd_left_h;
	unsigned char src_temp[SCREEN_WINDOW_WIDTH*SCREEN_WINDOW_HEIGHT*4];
	
	memcpy(src_temp,src,SCREEN_WINDOW_WIDTH*SCREEN_WINDOW_HEIGHT*4);
	
	for(y = 0; y < bsd_h; y++)
	{
		for(x = 0; x < bsd_w; x++)
		{
			src_x = (avm_sx +avm_ex ) / 2 - 2000 / 49 / 2 - 500 / 49 - 104 + x;
			src_y = avm_ey - bsd_h + y;
			pix_addr = ((SCREEN_WINDOW_HEIGHT - 1 - src_y) * SCREEN_WINDOW_WIDTH+ src_x) * 4;
			dst[y * bsd_w + x] = (76 * src_temp[pix_addr] + 150 * src_temp[pix_addr + 1] + 29 * src_temp[pix_addr + 2]) >> 8;
		}
	}
	
}

/*
Function process:
	+  bsd rgba to y
*/
static void rgba2y_bsd_r_egl(unsigned char * src, unsigned char * dst)
{
	int x,y;
	int pix_addr;
	int avm_sx = BSD_VIEW_X;
	int avm_sy = SCREEN_WINDOW_HEIGHT - BSD_VIEW_HEIGHT ;
	int avm_ex = avm_sx + BSD_VIEW_WIDTH;
	int avm_ey = SCREEN_WINDOW_HEIGHT;
	
	int src_x;
	int src_y;
	
	int bsd_w = g_bsd_usr.bsd_right_w;
	int bsd_h = g_bsd_usr.bsd_right_h;

	unsigned char src_temp[SCREEN_WINDOW_WIDTH*SCREEN_WINDOW_HEIGHT*4];
	memcpy(src_temp,src,SCREEN_WINDOW_WIDTH*SCREEN_WINDOW_HEIGHT*4);
	
	for(y = 0; y < bsd_h; y++)
	{
		for(x = 0; x < bsd_w; x++)
		{
			src_x = (avm_sx +avm_ex ) / 2 + 2000 / 49 / 2 + 500 / 49 + x;
			src_y = avm_ey - bsd_h + y;
			pix_addr = ((SCREEN_WINDOW_HEIGHT - 1 - src_y) * SCREEN_WINDOW_WIDTH+ src_x)*4;
			dst[y * bsd_w + x] = (76 * src_temp[pix_addr] + 150 * src_temp[pix_addr + 1] + 29 * src_temp[pix_addr + 2]) >> 8;
		}
	}
	
}
#else
static void rgba2y_bsd_l_egl(unsigned char * src, unsigned char * dst)    //RGB565
{
	int x,y;
	int pix_addr;
	unsigned short pix_addr565;
	int avm_sx = 0;
	int avm_sy = g_bsd_usr.bsd_left_w ;
	int avm_ex = avm_sx + BSD_VIEW_WIDTH;
	int avm_ey = BSD_VIEW_HEIGHT;
	
	int src_x;
	int src_y;
	
	int bsd_w = g_bsd_usr.bsd_left_w;                         //104
	int bsd_h = g_bsd_usr.bsd_left_h;                         //152

	for(y = 0; y < bsd_h; y++)
	{
		for(x = 0; x < bsd_w; x++)
		{
			src_x = (avm_sx + avm_ex) / 2 - 2000 / 49 / 2 - 500 / 49 - 104 + x;
			src_y = avm_ey - bsd_h + y;
			pix_addr = ((BSD_VIEW_HEIGHT - 1 - src_y) * BSD_VIEW_WIDTH + src_x) * 4;
			dst[y * bsd_w + x] = (76 * src[pix_addr] + 150 * src[pix_addr + 1] + 29 * src[pix_addr]) >> 8;
			
		}
	}
	
}

static void rgba2y_bsd_r_egl(unsigned char * src, unsigned char * dst)              ////RGB565
{
	int x,y;
	int pix_addr;
	unsigned short pix_addr565;
	int avm_sx = 0;
	int avm_sy = g_bsd_usr.bsd_left_w ;
	int avm_ex = avm_sx + BSD_VIEW_WIDTH;
	int avm_ey = BSD_VIEW_HEIGHT;	
	int src_x;
	int src_y;
	
	int bsd_w = g_bsd_usr.bsd_right_w;            //100
	int bsd_h = g_bsd_usr.bsd_right_h;            //146

	
	for(y = 0; y < bsd_h; y++)
	{
		for(x = 0; x < bsd_w; x++)
		{
			src_x = (avm_sx + avm_ex ) / 2 + 2000 / 49 / 2 + 500 / 49 + x;
			src_y = avm_ey - bsd_h + y;
			pix_addr = ((BSD_VIEW_HEIGHT - 1 - src_y) * BSD_VIEW_WIDTH + src_x) * 4;
			dst[y * bsd_w + x] = (76 * src[pix_addr+2] + 150 * src[pix_addr + 1] + 29 * src[pix_addr]) >> 8;;
		}
	}
	
}
#endif
void save_bmp(const char * filename, unsigned char * src, int width, int height)
{
	int x,y;
	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{
			src[(y*width + x) * 4 + 3] = 255;
		}
	}

	SOIL_save_image(filename, SOIL_SAVE_TYPE_BMP, width, height, 4, src);
}

/*
Function process:
	+  save bmp by y
*/
static void save_bmp_y(const char * filename, unsigned char * src, int width, int height)
{
	int x,y;

	SOIL_save_image(filename, SOIL_SAVE_TYPE_BMP, width, height, 1, src);
}
