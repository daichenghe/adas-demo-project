/*******************************************************************************
* File Name          : avm_ui.h
* Author             : Daich
* Revision           : 2.1
* Date               : 01/06/2018
* Description        :
*
* HISTORY***********************************************************************
* 01/06/2018  | sDas Basic 2.0                       | Zhangh
*
*******************************************************************************/
#ifndef _AVM_BSD_H_
#define _AVM_BSD_H_

#include <stdio.h>
#include <string.h>
#include "log.h"
#include <semaphore.h>
/*
*************************************************************************
*************************************************************************
*                    LOCAL  STRUCT / E N U M E R A T I O N S
*************************************************************************
*************************************************************************
*/
typedef enum
{
	BSD_WARNING_OFF,
	BSD_WARNING_ON,
}bsd_warning_t;
typedef struct bsd_usr_type
{
	unsigned char *bsd_left_buf;
	unsigned char *bsd_right_buf;
	
	unsigned char bsd_left_w;
	unsigned char bsd_left_h;
	
	unsigned char bsd_right_w;
	unsigned char bsd_right_h;
	
	bsd_warning_t bsd_left_warning_status;
	bsd_warning_t bsd_right_warning_status;
	
	sem_t sem_require_ydata;
	sem_t sem_response_ydata;
}bsd_usr_t;


/***********************BSD************************/


#define BSD_W 100
#define BSD_H 146

#define BSD_X_L 22
#define BSD_Y_L 237

#define BSD_X_R 201
#define BSD_Y_R 237
/***********************END************************/

extern bsd_usr_t g_bsd_usr;
#ifdef T3BOARD

extern void bsd_data_get(unsigned char *buffer);

#endif
extern void *bsd_alg_thread(void *argv);

#endif