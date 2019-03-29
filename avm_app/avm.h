/*******************************************************************************
* File Name          : avm.h
* Author             : Daich
* Revision           : 2.1
* Date               : 15/05/2018
* Description        :
*
* HISTORY***********************************************************************
* 15/05/2018  | sDas Basic 2.0                                             | Daich
*
*******************************************************************************/
#ifndef __AVM__H__
#define __AVM__H__

#include "avmlib_sdk.h"      //avm lib system type redefine


extern AVM_ERR avm_render(void);
extern void avm_view_mutex_lock();
extern void avm_view_mutex_unlock();


#endif
