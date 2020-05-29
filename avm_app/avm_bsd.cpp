/*******************************************************************************
* File Name          : avm_ui.h
* Author             : Daich
* Revision           : 2.1
* Date               : 01/06/2018
* Description        : bsd logic code
*
* HISTORY***********************************************************************
* 01/06/2018  | sDas Basic 2.0                       | Zhangh
*
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#ifdef FSLBOARD
#include "GLES3/gl3.h"
#include "GLES3/gl3ext.h"
#endif
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include "avmlib_sdk.h"
#include "avm_config.h"
#include "avm_view_config.h"
#include "log.h"
#include <unistd.h>
#include "avm_common.h"
#include "stdlib.h"
#include "avm_bsd.h"
#include "avmlib_bsd.h"
//#include "avmlib_bsd.h"
#ifdef T3BOARD
#include "t3_fb.h"
#endif
/*
*************************************************************************
*************************************************************************
*                    LOCAL  STRUCT / E N U M E R A T I O N S
*************************************************************************
*************************************************************************
*/

bsd_usr_t g_bsd_usr;

#ifdef T3BOARD

void bsd_data_get(unsigned char *buffer)
{
	display_buffer_get(BSD_VIEW_X, BSD_VIEW_Y, BSD_VIEW_WIDTH, BSD_VIEW_HEIGHT, buffer);	

}

#endif
void *bsd_alg_thread(void *argv)
{
	BsdInput bsd_left_obj;
	BsdInput bsd_right_obj;
	OutblindRecs *bsd_left_output;
	OutblindRecs *bsd_right_output;
	
	g_bsd_usr.bsd_left_w 	= BSD_W;
	g_bsd_usr.bsd_left_h 	= BSD_H;
	
	g_bsd_usr.bsd_right_w	= BSD_W;
	g_bsd_usr.bsd_right_h	= BSD_H;
	
	g_bsd_usr.bsd_left_buf  = (unsigned char *)malloc( g_bsd_usr.bsd_left_w * g_bsd_usr.bsd_left_h);
	g_bsd_usr.bsd_right_buf = (unsigned char *)malloc(g_bsd_usr.bsd_right_w * g_bsd_usr.bsd_right_h);
//left	
	bsd_left_obj.img.w		= g_bsd_usr.bsd_left_w;
	bsd_left_obj.img.h		= g_bsd_usr.bsd_left_h;
	bsd_left_obj.img.pdata	= g_bsd_usr.bsd_left_buf;
	bsd_left_obj.pos		= LEFT;	

//right
	bsd_right_obj.img.w		= g_bsd_usr.bsd_right_w;
	bsd_right_obj.img.h		= g_bsd_usr.bsd_right_h;
	bsd_right_obj.img.pdata	= g_bsd_usr.bsd_right_buf;
	bsd_right_obj.pos		= RIGHT;
	
	 
	if(0 != sem_init(&g_bsd_usr.sem_require_ydata, 0, 0))
	{
		logg(ERR, "ldw_alg_thread sem_init error: %s, %d\n", __FUNCTION__, __LINE__);
	}
	if(0 != sem_init(&g_bsd_usr.sem_response_ydata, 0, 0))
	{
		logg(ERR, "ldw_alg_thread sem_init error: %s, %d\n", __FUNCTION__, __LINE__);
	} 
	
	DbgPrintf("bsd start post\n");
	
	if(0 != sem_post(&g_bsd_usr.sem_require_ydata))
	{
		logg(ERR, "ldw_alg_thread sem_post failed %s, %d\n",__FUNCTION__, __LINE__);
	}
	DbgPrintf("bsd start while\n");

	while(1)
	{
		//wait
		if(0 != sem_wait(&g_bsd_usr.sem_response_ydata))
        {
            logg(ERR, "ldw_alg_thread sem_wait failed %s, %d\n",__FUNCTION__, __LINE__);			
            continue;
        }

		mvBsdProcess(&bsd_left_obj);
		mvBsdProcess(&bsd_right_obj);
		mvGetBsdResult(&bsd_left_output,LEFT);
		mvGetBsdResult(&bsd_right_output,RIGHT);	
		if(bsd_left_output->nRecNum > 0)
		{
			g_bsd_usr.bsd_left_warning_status = BSD_WARNING_ON;
		}
		else
		{
			g_bsd_usr.bsd_left_warning_status = BSD_WARNING_OFF;
		}
		if(bsd_right_output->nRecNum > 0)
		{
			g_bsd_usr.bsd_right_warning_status = BSD_WARNING_ON;
		}
		else
		{
			g_bsd_usr.bsd_right_warning_status = BSD_WARNING_OFF;
	
		}
	//	DbgPrintf("left num = %d, right num = %d\n",bsd_left_output->nRecNum,bsd_right_output->nRecNum);
		
		//post
		if(0 != sem_post(&g_bsd_usr.sem_require_ydata))
        {
			logg(ERR, "ldw_alg_thread sem_post failed %s, %d\n",__FUNCTION__, __LINE__);
        }	
	}	
}
