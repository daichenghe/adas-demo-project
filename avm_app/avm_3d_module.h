/*******************************************************************************
* File Name          : avm_3d_module.h
* Author             : Daich
* Revision           : 2.1
* Date               : 01/06/2018
* Description        :
*
* HISTORY***********************************************************************
* 01/06/2018  | sDas Basic 2.0                       | Daich
*
*******************************************************************************/
#ifndef __AVM_3D_MODULE_H__
#define __AVM_3D_MODULE_H__

#include "avmlib_sdk.h"
#include "avm_ctr.h"

#define SUBMODECNT  (19) /* 3D模式预留个数 */



#define AVM_3D_ROTATE_360_ONGING	(0)
#define AVM_3D_ROTATE_360_COMPLETE 	(1)
/*
*************************************************************************
*************************************************************************
*                      F U N C T I O N
*************************************************************************
*************************************************************************
*/
AVM_ERR default_3d_init();
AVM_ERR common_3d_init();
AVM_ERR avm_3d_set_view_parameter(view_keep_t *view_keep);
AVM_ERR render_3d_avm(MODE3D mode_3d_index);

#endif