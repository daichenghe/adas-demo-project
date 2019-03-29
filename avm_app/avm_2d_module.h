/*******************************************************************************
* File Name          : avm_2d_module.h
* Author             : Daich
* Revision           : 2.1
* Date               : 15/05/2018
* Description        :
*
* HISTORY***********************************************************************
* 15/05/2018  | sDas Basic 2.0                       | Daich
*
*******************************************************************************/
#ifndef __AVM_2D_MODULE_H__
#define __AVM_2D_MODULE_H__

#include "avmlib_sdk.h"
#include "avmlib_2d.h"
#include "avmlib_GenCaliData.h"
#include "avm_ctr.h"
/*
*************************************************************************
*************************************************************************
*                      F U N C T I O N
*************************************************************************
*************************************************************************
*/

AVM_ERR default_2d_init();
AVM_ERR common_2d_init();

AVM_ERR render_2d_avm();
AVM_ERR render_single_view(int cam_index);
AVM_ERR render_ori_view(int cam_index);
AVM_ERR render_ori_split_view();
AVM_ERR render_periscope_view(int cam_index);
AVM_ERR render_ldws_avm();
AVM_ERR render_bsd_avm();
AVM_ERR render_svfs_view();
AVM_ERR render_backup_view(int cam_index);
AVM_ERR avm_disp_info_get(AVM_MODE mode,virtual_camera_avm_t *virtual_camera_avm_param_p, virtual_camera_sgv_t *virtual_camera_sgv_param_p, view_2d_parameter_t view_2d_parameter_param_p[4]);

#endif