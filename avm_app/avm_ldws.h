/*******************************************************************************
* File Name          : avm_ui.h
* Author             : Jerry
* Revision           : 2.1
* Date               : 03/07/2018
* Description        :
*
* HISTORY***********************************************************************
* 03/07/2018  | sDas Basic 2.0                       | Jerry
*
*******************************************************************************/
#ifndef _AVM_LDWS_H_
#define _AVM_LDWS_H_

#include <stdio.h>
#include <string.h>
#include "log.h"
#include <semaphore.h>
#include "avmlib_ldws.h"
/*
*************************************************************************
*************************************************************************
*                    LOCAL  STRUCT / E N U M E R A T I O N S
*************************************************************************
*************************************************************************
*/
typedef struct ldws_usr_type
{
	int ldws_w;
	int ldws_h;
	unsigned char *ldws_buf;
	
	
	ldws_warning_t ldws_left_warning_status;
	ldws_warning_t ldws_right_warning_status;
	
	sem_t sem_require_ydata;
	sem_t sem_response_ydata;
}ldws_usr_t;

/***********************LDWS************************/

/***********************END************************/

extern ldws_usr_t g_ldws_usr;

extern void ldws_data_get(void);
extern void ldws_data_update(void);
extern void *ldws_alg_thread(void *argv);

#endif