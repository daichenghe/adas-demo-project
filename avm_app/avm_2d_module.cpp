/*******************************************************************************
* File Name          : avm_2d_module.cpp
* Author             : Daich
* Revision           : 2.1
* Date               : 15/05/2018
* Description        : avm_2d control and init code
*
* HISTORY***********************************************************************
* 15/05/2018  | sDas Basic 2.0                                          | Daich
*
*******************************************************************************/
#include "stdio.h"
#include "avmlib_sdk.h"
#include "avm_2d_module.h"
#include "avm_config.h"
#include "avm_view_config.h"
#include "avm_ctr.h"
#include "avmlib_loadpng.h"
#include <string.h>
#include "avmlib_2d.h"
#include "avm_bsd.h"
#include "avm_ldws.h"

#define PI		(3.14159265358)

static AVM_ERR avm_2d_view_init();
static AVM_ERR avm_single_view_init();
static AVM_ERR avm_periscope_center_view_init();
static AVM_ERR avm_periscope_side_view_init();
static AVM_ERR avm_periscope_view_init();
static AVM_ERR avm_svfs_view_init();
static AVM_ERR avm_ori_view_init();
static AVM_ERR avm_ori_split_view_init();
static AVM_ERR avm_ldws_view_init();
static AVM_ERR avm_bsd_view_init();
static AVM_ERR avm_backup_view_init();

static AVM_ERR avm_periscope_side_view_init();
static AVM_ERR avm_periscope_center_view_init();


static AVM_ERR view_gl_parameter_init(AVM_MODE mode, view_2d_parameter_t *view_gl_parameter_p[5]);
static AVM_ERR blend_init(AVM_MODE mode, blend_2d_Parameter_t 	*blend_Parameter_buf);
static AVM_ERR view_vbo_init(AVM_MODE mode, lut_parameter_t *view_vbo_parameter_buf);
static AVM_ERR avm_2d_view_video_init();
static AVM_ERR avm_2d_view_car_init();


static virtual_camera_avm_t 	virtual_camera_avm_param_buf[MODE_MAX];
static virtual_camera_sgv_t  	virtual_camera_sgv_param_buf[MODE_MAX];
static VirtualCameraParam_t VirtualCamera[MODE_MAX * 4];

avm2d_type_t 			avm_2d_view_handle;
static avm2d_type_t 			avm_single_view_handle;
static avm2d_type_t				avm_periscope_view_center_handle;
static avm2d_type_t				avm_periscope_view_side_handle;
static avm2d_type_t				avm_svfs_view_handle;
static avm2d_type_t				avm_backup_view_handle;
static avm2d_type_t				avm_ldws_view_handle;
static avm2d_type_t				avm_bsd_view_handle;
static avm2d_type_t				avm_ori_view_handle;
static avm2d_type_t				avm_ori_split_view_handle;
static avm2d_type_t 			avm_2d_view_car_handle;

static lut_parameter_t			view_vbo_parameter_buf;
static blend_2d_Parameter_t 	blend_Parameter_buf;
static view_2d_parameter_t  	view_gl_parameter_buf[MODE_MAX * 5];

static F32 avm_light_create  = 0.0;
static F32 avm_deinterlace   = 0.0;
static F32 avm_light_R       = 0.6;
static F32 avm_light_G       = 0.6;
static F32 avm_light_B       = 0.6;


AVM_ERR default_2d_init()
{
	avm_2d_view_init();	
	avm_single_view_init();

	return AVM_ERR_NONE;
}
AVM_ERR common_2d_init()
{
	avm_periscope_view_init();
	avm_svfs_view_init();
	avm_ori_view_init();
	avm_ori_split_view_init();
	avm_ldws_view_init();	
	avm_bsd_view_init();
	avm_backup_view_init();	
	return AVM_ERR_NONE;
}

/*
Function process:
	+  avm render
*/
AVM_ERR render_2d_avm()
{
	AVM_ERR ret;
    ret = avm_2d_view_handle.avm2d_shader_type->avmlib_Use(&avm_2d_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
//	DbgPrintf("%s %d\n", __FUNCTION__, __LINE__);
    ret = avm_2d_view_handle.avm2d_render_type->avmlib_Render_Topview(&avm_2d_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
//	DbgPrintf("%s %d\n", __FUNCTION__, __LINE__);
    ret = avm_2d_view_handle.avm2d_shader_type->avmlib_Unuse(&avm_2d_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
//	DbgPrintf("%s %d\n", __FUNCTION__, __LINE__);
	ret = avm_2d_view_car_handle.avm2d_shader_type->avmlib_Use(&avm_2d_view_car_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
//	DbgPrintf("%s %d\n", __FUNCTION__, __LINE__);
    ret = avm_2d_view_car_handle.avm2d_render_type->avmlib_Render_Car(&avm_2d_view_car_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
//	DbgPrintf("%s %d\n", __FUNCTION__, __LINE__);
    ret = avm_2d_view_car_handle.avm2d_shader_type->avmlib_Unuse(&avm_2d_view_car_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
//	DbgPrintf("%s %d\n", __FUNCTION__, __LINE__);
}

/*
Function process:
	+  ldws render
*/
AVM_ERR render_ldws_avm()
{
	AVM_ERR ret;
    ret = avm_ldws_view_handle.avm2d_shader_type->avmlib_Use(&avm_ldws_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_ldws_view_handle.avm2d_render_type->avmlib_Render_Topview(&avm_ldws_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_ldws_view_handle.avm2d_shader_type->avmlib_Unuse(&avm_ldws_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
}

/*
Function process:
	+  bsd render
*/
AVM_ERR render_bsd_avm()
{
	AVM_ERR ret;
    ret = avm_bsd_view_handle.avm2d_shader_type->avmlib_Use(&avm_bsd_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_bsd_view_handle.avm2d_render_type->avmlib_Render_Topview(&avm_bsd_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_bsd_view_handle.avm2d_shader_type->avmlib_Unuse(&avm_bsd_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
}


/*
Function process:
	+  single view render
*/
AVM_ERR render_single_view(int cam_index)
{
	AVM_ERR ret;
	int obj_index = cam_index;
    ret = avm_single_view_handle.avm2d_shader_type->avmlib_Use(&avm_single_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_single_view_handle.avm2d_render_type->avmlib_Render_Singleview(&avm_single_view_handle,obj_index,cam_index);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_single_view_handle.avm2d_shader_type->avmlib_Unuse(&avm_single_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
}
AVM_ERR render_backup_view(int cam_index)
{
	AVM_ERR ret;
	int obj_index = cam_index;
    ret = avm_backup_view_handle.avm2d_shader_type->avmlib_Use(&avm_backup_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_backup_view_handle.avm2d_render_type->avmlib_Render_Singleview(&avm_backup_view_handle,obj_index,cam_index);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_backup_view_handle.avm2d_shader_type->avmlib_Unuse(&avm_backup_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
}

/*
Function process:
	+  svfs render
*/
AVM_ERR render_svfs_view()
{
	AVM_ERR ret;
    ret = avm_svfs_view_handle.avm2d_shader_type->avmlib_Use(&avm_svfs_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_svfs_view_handle.avm2d_render_type->avmlib_Render_Singleview(&avm_svfs_view_handle,1,1);
    ret = avm_svfs_view_handle.avm2d_render_type->avmlib_Render_Singleview(&avm_svfs_view_handle,3,3);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_svfs_view_handle.avm2d_shader_type->avmlib_Unuse(&avm_svfs_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
}


/*
Function process:
	+  ori render
*/
AVM_ERR render_ori_view(int cam_index)
{
	AVM_ERR ret;
	int obj_index = cam_index;
    ret = avm_ori_view_handle.avm2d_shader_type->avmlib_Use(&avm_ori_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_ori_view_handle.avm2d_render_type->avmlib_Render_Singleview(&avm_ori_view_handle,obj_index,cam_index);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_ori_view_handle.avm2d_shader_type->avmlib_Unuse(&avm_ori_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
}
AVM_ERR render_ori_split_view()
{
	AVM_ERR ret;
    ret = avm_ori_split_view_handle.avm2d_shader_type->avmlib_Use(&avm_ori_split_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_ori_split_view_handle.avm2d_render_type->avmlib_Render_Singleview(&avm_ori_split_view_handle,0,0);
    ret = avm_ori_split_view_handle.avm2d_render_type->avmlib_Render_Singleview(&avm_ori_split_view_handle,1,1);
    ret = avm_ori_split_view_handle.avm2d_render_type->avmlib_Render_Singleview(&avm_ori_split_view_handle,2,2);
    ret = avm_ori_split_view_handle.avm2d_render_type->avmlib_Render_Singleview(&avm_ori_split_view_handle,3,3);
    if(ret != AVM_ERR_NONE) 
	{
	    ret = avm_ori_split_view_handle.avm2d_shader_type->avmlib_Unuse(&avm_ori_split_view_handle);
		return ret;
	}
    ret = avm_ori_split_view_handle.avm2d_shader_type->avmlib_Unuse(&avm_ori_split_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
}

/*
Function process:
	+  per render
*/
AVM_ERR render_periscope_view(int cam_index)
{
	AVM_ERR ret;
	int obj_index_screen_left;
	int obj_index_screen_right;
	int obj_index_screen_center = cam_index;
	if(cam_index == 0)
	{
		obj_index_screen_left = 0;
		obj_index_screen_right = 1;
	}
	else
	{
		obj_index_screen_left = 2;
		obj_index_screen_right = 3;
	}
    ret = avm_periscope_view_center_handle.avm2d_shader_type->avmlib_Use(&avm_periscope_view_center_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_periscope_view_center_handle.avm2d_render_type->avmlib_Render_Singleview(&avm_periscope_view_center_handle,obj_index_screen_center,cam_index);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_periscope_view_center_handle.avm2d_shader_type->avmlib_Unuse(&avm_periscope_view_center_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	ret = avm_periscope_view_side_handle.avm2d_shader_type->avmlib_Use(&avm_periscope_view_side_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_periscope_view_side_handle.avm2d_render_type->avmlib_Render_Singleview(&avm_periscope_view_side_handle,obj_index_screen_left,cam_index);
    ret = avm_periscope_view_side_handle.avm2d_render_type->avmlib_Render_Singleview(&avm_periscope_view_side_handle,obj_index_screen_right ,cam_index);

    ret = avm_periscope_view_side_handle.avm2d_shader_type->avmlib_Unuse(&avm_periscope_view_side_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
}
static AVM_ERR avm_2d_view_init()
{
	avm_2d_view_video_init();
	avm_2d_view_car_init();
}
static AVM_ERR avm_2d_view_video_init()
{
	AVM_ERR ret;
	view_2d_parameter_t *view_gl_parameter_p[5];
	avm_2d_view_handle.avm_type = avm2dview_type;
	
	/* 创建各种资源 */
	ret = avmlib_2d_Init(&avm_2d_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 确定此模式使用的shader*/
	avm_2d_view_handle.avm2d_shader_type->avmlib_Shader_Init(&avm_2d_view_handle, AVM_2D_VERT, AVM_2D_FRAG);
	if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	/* VBO查找表的尺寸初始化 */
	view_vbo_init(MODE_AVM_2D, &view_vbo_parameter_buf);
	
	ret = avm_2d_view_handle.avm2d_texture_type->avmlib_Gen_TextrueBuffer(&avm_2d_view_handle, view_vbo_parameter_buf.pavm->vc_width, view_vbo_parameter_buf.pavm->vc_height * 4, GL_ALPHA, AVM_NULL);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	/* 生成VBO */
    ret = avm_2d_view_handle.avm2d_object_type->avmlib_Gen_ObjBuffer(&avm_2d_view_handle, VBO_2D_TOPVIEW, 4,  &view_vbo_parameter_buf);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* 设置环视矫正效果 */
	blend_init(MODE_AVM_2D,&blend_Parameter_buf);	
	/* 设置blend参数 */
    ret = avm_2d_view_handle.avm2d_render_type->avmlib_Set_Blend(&avm_2d_view_handle, &blend_Parameter_buf);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 设置环视显示窗口数据(opengl) */
    view_gl_parameter_init(MODE_AVM_2D, view_gl_parameter_p);
	
	/* 设置OPENGL显示的窗口参数 */
    ret = avm_2d_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_2d_view_handle, view_gl_parameter_p[0], 0);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
}
static AVM_ERR avm_2d_view_car_init()
{
	AVM_ERR ret;
	view_2d_parameter_t *view_gl_parameter_p[5];

	png_texture_t *pdatancar = AVM_NULL;

	avm_2d_view_car_handle.avm_type = car2d_type;
	avm_2d_view_car_handle.pAttr = &avm_2d_view_handle;
	
	/* 创建各种资源 */
	 ret = avmlib_2d_Init(&avm_2d_view_car_handle);
     if(ret != AVM_ERR_NONE) 
	{
		 return ret;
	 }
	
	// /* 确定此模式使用的shader*/
	 avm_2d_view_car_handle.avm2d_shader_type->avmlib_Shader_Init(&avm_2d_view_car_handle, VEHICLESHADER2D_VERT, VEHICLESHADER2D_FRAG);
	 if(AVM_ERR_NONE != ret) 
	 {
		 return ret;
	 }
	/* 生成VBO */
    ret = avm_2d_view_car_handle.avm2d_object_type->avmlib_Gen_ObjBuffer(&avm_2d_view_car_handle, VBO_VEH_VIEW, 1,  AVM_NULL);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	pdatancar = loadPNGTexture(VEH_2D_NAME);
    if(pdatancar == NULL)
    {
        return AVM_ERR_SYS;
    }
	
    ret = avm_2d_view_car_handle.avm2d_texture_type->avmlib_Gen_TextrueBuffer(&avm_2d_view_car_handle, pdatancar->width, pdatancar->height, pdatancar->format, pdatancar->texels);
    if(ret != AVM_ERR_NONE)
    {
        return ret;
    }
		
	/* 设置环视矫正效果 */
    blend_Parameter_buf.ctl_blend[0] = 1.0;
    blend_Parameter_buf.blend_type = SRC_ONE_MINUS_SRC_ALPHA_2D;
	
	/* 设置blend参数 */
    ret = avm_2d_view_car_handle.avm2d_render_type->avmlib_Set_Blend(&avm_2d_view_car_handle, &blend_Parameter_buf);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 设置环视显示窗口数据(opengl) */
    view_gl_parameter_init(MODE_AVM_2D, view_gl_parameter_p);
	
	/* 设置OPENGL显示的窗口参数 */
    ret = avm_2d_view_car_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_2d_view_car_handle, view_gl_parameter_p[0 + 1], 0);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
}

static AVM_ERR avm_single_view_init()
{
	AVM_ERR ret;
	view_2d_parameter_t *view_gl_parameter_p[5];

	avm_single_view_handle.avm_type = singleview_type;
	
	/* 创建各种资源 */
	ret = avmlib_2d_Init(&avm_single_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 确定此模式使用的shader，目前shader用的是AVM 2D的shader程序*/
	avm_single_view_handle.avm2d_shader_type->avmlib_Shader_Init(&avm_single_view_handle, AVM_SV_VERT, AVM_SV_FRAG);
	if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* VBO查找表的尺寸初始化 */
	view_vbo_init(MODE_SV, &view_vbo_parameter_buf);
	
	/* 生成VBO */
    ret = avm_single_view_handle.avm2d_object_type->avmlib_Gen_ObjBuffer(&avm_single_view_handle, VBO_2D_LARGEVIEW, 4,  &view_vbo_parameter_buf);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* 设置环视矫正效果 */
	blend_init(MODE_SV,&blend_Parameter_buf);
	
	/* 设置blend参数 */
    ret = avm_single_view_handle.avm2d_render_type->avmlib_Set_Blend(&avm_single_view_handle, &blend_Parameter_buf);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 设置环视显示窗口数据(opengl) */
    view_gl_parameter_init(MODE_SV, view_gl_parameter_p);
	
	/* 设置OPENGL显示的窗口参数  ,分被设置前，右，后左 */
    ret = avm_single_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_single_view_handle, view_gl_parameter_p[0], 0);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_single_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_single_view_handle, view_gl_parameter_p[0 + 0], 1);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_single_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_single_view_handle, view_gl_parameter_p[0 + 0], 2);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_single_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_single_view_handle, view_gl_parameter_p[0 + 0], 3);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
}
static AVM_ERR avm_periscope_view_init()
{
	avm_periscope_center_view_init();
	avm_periscope_side_view_init();
}
static AVM_ERR avm_periscope_center_view_init()
{
	AVM_ERR ret;
	view_2d_parameter_t *view_gl_parameter_p[5];

	avm_periscope_view_center_handle.avm_type = periscopeview_type;
	
	/* 创建各种资源 */
	ret = avmlib_2d_Init(&avm_periscope_view_center_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 确定此模式使用的shader，目前shader用的是AVM 2D的shader程序*/
	avm_periscope_view_center_handle.avm2d_shader_type->avmlib_Shader_Init(&avm_periscope_view_center_handle, AVM_SV_VERT, AVM_SV_FRAG);
	if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* VBO查找表的尺寸初始化 */
	view_vbo_init(MODE_PER_CENTER, &view_vbo_parameter_buf);
	
	/* 生成VBO */
    ret = avm_periscope_view_center_handle.avm2d_object_type->avmlib_Gen_ObjBuffer(&avm_periscope_view_center_handle, VBO_2D_PERVIEW, 4,  &view_vbo_parameter_buf);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* 设置环视矫正效果 */
	blend_init(MODE_PER_CENTER,&blend_Parameter_buf);
	
	/* 设置blend参数 */
    ret = avm_periscope_view_center_handle.avm2d_render_type->avmlib_Set_Blend(&avm_periscope_view_center_handle, &blend_Parameter_buf);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 设置环视显示窗口数据(opengl) */
    view_gl_parameter_init(MODE_PER_CENTER, view_gl_parameter_p);
	
	/* 设置OPENGL显示的窗口参数  ,分被设置前，右，后左 */
    ret = avm_periscope_view_center_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_periscope_view_center_handle, view_gl_parameter_p[0], 0);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_periscope_view_center_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_periscope_view_center_handle, view_gl_parameter_p[0], 2);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
}
static AVM_ERR avm_periscope_side_view_init()
{
	AVM_ERR ret;
	view_2d_parameter_t *view_gl_parameter_p[5];
	avm_periscope_view_side_handle.avm_type = periscopeview_type;
	
	/* 创建各种资源 */
	ret = avmlib_2d_Init(&avm_periscope_view_side_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 确定此模式使用的shader，目前shader用的是AVM 2D的shader程序*/
	avm_periscope_view_side_handle.avm2d_shader_type->avmlib_Shader_Init(&avm_periscope_view_side_handle, AVM_SV_VERT, AVM_SV_FRAG);
	if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* VBO查找表的尺寸初始化 */
	view_vbo_init(MODE_PER, &view_vbo_parameter_buf);
	
	/* 生成VBO */
    ret = avm_periscope_view_side_handle.avm2d_object_type->avmlib_Gen_ObjBuffer(&avm_periscope_view_side_handle, VBO_2D_PERVIEW, 4,  &view_vbo_parameter_buf);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* 设置环视矫正效果 */
	blend_init(MODE_PER,&blend_Parameter_buf);
	
	/* 设置blend参数 */
    ret = avm_periscope_view_side_handle.avm2d_render_type->avmlib_Set_Blend(&avm_periscope_view_side_handle, &blend_Parameter_buf);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 设置环视显示窗口数据(opengl) */
    view_gl_parameter_init(MODE_PER, view_gl_parameter_p);
	
	/* 设置OPENGL显示的窗口参数  ,分被设置前，右，后左 */
    ret = avm_periscope_view_side_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_periscope_view_side_handle, view_gl_parameter_p[0], 0);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_periscope_view_side_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_periscope_view_side_handle, view_gl_parameter_p[0 +1], 1);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
		
    ret = avm_periscope_view_side_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_periscope_view_side_handle, view_gl_parameter_p[0], 3);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_periscope_view_side_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_periscope_view_side_handle, view_gl_parameter_p[0 + 1], 2);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
}

static AVM_ERR avm_svfs_view_init()
{
	AVM_ERR ret;
	view_2d_parameter_t *view_gl_parameter_p[5];

	avm_svfs_view_handle.avm_type = singleview_type;
	
	/* 创建各种资源 */
	ret = avmlib_2d_Init(&avm_svfs_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 确定此模式使用的shader，目前shader用的是AVM 2D的shader程序*/
	avm_svfs_view_handle.avm2d_shader_type->avmlib_Shader_Init(&avm_svfs_view_handle, AVM_SV_VERT, AVM_SV_FRAG);
	if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* VBO查找表的尺寸初始化 */
	view_vbo_init(MODE_SVFS, &view_vbo_parameter_buf);
	
	/* 生成VBO */
    ret = avm_svfs_view_handle.avm2d_object_type->avmlib_Gen_ObjBuffer(&avm_svfs_view_handle, VBO_2D_PERVIEW, 4,  &view_vbo_parameter_buf);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* 设置环视矫正效果 */
	blend_init(MODE_SVFS,&blend_Parameter_buf);
	
	/* 设置blend参数 */
    ret = avm_svfs_view_handle.avm2d_render_type->avmlib_Set_Blend(&avm_svfs_view_handle, &blend_Parameter_buf);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 设置环视显示窗口数据(opengl) */
    view_gl_parameter_init(MODE_SVFS, view_gl_parameter_p);
	
	/* 设置OPENGL显示的窗口参数  ,分被设置前，右，后左 */
    ret = avm_svfs_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_svfs_view_handle, view_gl_parameter_p[0], 1);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_svfs_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_svfs_view_handle, view_gl_parameter_p[0 + 1], 3);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
}
static AVM_ERR avm_backup_view_init()
{
	AVM_ERR ret;
	view_2d_parameter_t *view_gl_parameter_p[5];

	avm_backup_view_handle.avm_type = singleview_type;
	
	/* 创建各种资源 */
	ret = avmlib_2d_Init(&avm_backup_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 确定此模式使用的shader，目前shader用的是AVM 2D的shader程序*/
	avm_backup_view_handle.avm2d_shader_type->avmlib_Shader_Init(&avm_backup_view_handle, AVM_SV_VERT, AVM_SV_FRAG);
	if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* VBO查找表的尺寸初始化 */
	view_vbo_init(MODE_BACKUP, &view_vbo_parameter_buf);
	
	/* 生成VBO */
    ret = avm_backup_view_handle.avm2d_object_type->avmlib_Gen_ObjBuffer(&avm_backup_view_handle, VBO_2D_PERVIEW, 4,  &view_vbo_parameter_buf);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* 设置环视矫正效果 */
	blend_init(MODE_BACKUP,&blend_Parameter_buf);
	
	/* 设置blend参数 */
    ret = avm_backup_view_handle.avm2d_render_type->avmlib_Set_Blend(&avm_backup_view_handle, &blend_Parameter_buf);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 设置环视显示窗口数据(opengl) */
    view_gl_parameter_init(MODE_BACKUP, view_gl_parameter_p);
	
	/* 设置OPENGL显示的窗口参数  ,分被设置前，右，后左 */
    ret = avm_backup_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_backup_view_handle, view_gl_parameter_p[0], 0);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_backup_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_backup_view_handle, view_gl_parameter_p[0], 1);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_backup_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_backup_view_handle, view_gl_parameter_p[0], 2);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_backup_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_backup_view_handle, view_gl_parameter_p[0], 3);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
}
static AVM_ERR avm_ldws_view_init()
{
	AVM_ERR ret;
	view_2d_parameter_t *view_gl_parameter_p[5];

	avm_ldws_view_handle.avm_type = avm2dview_type;
	
	/* 创建各种资源 */
	ret = avmlib_2d_Init(&avm_ldws_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 确定此模式使用的shader*/
	avm_ldws_view_handle.avm2d_shader_type->avmlib_Shader_Init(&avm_ldws_view_handle, AVM_2D_VERT, AVM_2D_FRAG);
	if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* VBO查找表的尺寸初始化 */
	view_vbo_init(MODE_AVM_LDWS, &view_vbo_parameter_buf );
	
	ret = avm_ldws_view_handle.avm2d_texture_type->avmlib_Gen_TextrueBuffer(&avm_ldws_view_handle, view_vbo_parameter_buf.pavm->vc_width, view_vbo_parameter_buf.pavm->vc_height * 4, GL_ALPHA, AVM_NULL);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}

	/* 生成VBO */
    ret = avm_ldws_view_handle.avm2d_object_type->avmlib_Gen_ObjBuffer(&avm_ldws_view_handle, VBO_2D_TOPVIEW, 4,  &view_vbo_parameter_buf);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* 设置环视矫正效果 */
	blend_init(MODE_AVM_LDWS,&blend_Parameter_buf);
	
	/* 设置blend参数 */
    ret = avm_ldws_view_handle.avm2d_render_type->avmlib_Set_Blend(&avm_ldws_view_handle, &blend_Parameter_buf);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 设置环视显示窗口数据(opengl) */
    view_gl_parameter_init(MODE_AVM_LDWS, view_gl_parameter_p);
	
	/* 设置OPENGL显示的窗口参数 */
    ret = avm_ldws_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_ldws_view_handle, view_gl_parameter_p[0], 0);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}	
}

static AVM_ERR avm_bsd_view_init()
{
	AVM_ERR ret;
	view_2d_parameter_t *view_gl_parameter_p[5];

	avm_bsd_view_handle.avm_type = avm2dview_type;
	
	/* 创建各种资源 */
	ret = avmlib_2d_Init(&avm_bsd_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 确定此模式使用的shader*/
	avm_bsd_view_handle.avm2d_shader_type->avmlib_Shader_Init(&avm_bsd_view_handle, AVM_2D_VERT, AVM_2D_FRAG);
	if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* VBO查找表的尺寸初始化 */
	view_vbo_init(MODE_AVM_BSD, &view_vbo_parameter_buf);
	
	ret = avm_bsd_view_handle.avm2d_texture_type->avmlib_Gen_TextrueBuffer(&avm_bsd_view_handle, view_vbo_parameter_buf.pavm->vc_width, view_vbo_parameter_buf.pavm->vc_height * 4, GL_ALPHA, AVM_NULL);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}

	/* 生成VBO */
    ret = avm_bsd_view_handle.avm2d_object_type->avmlib_Gen_ObjBuffer(&avm_bsd_view_handle, VBO_2D_TOPVIEW, 4,  &view_vbo_parameter_buf);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* 设置环视矫正效果 */
	blend_init(MODE_AVM_BSD,&blend_Parameter_buf);
	
	/* 设置blend参数 */
    ret = avm_bsd_view_handle.avm2d_render_type->avmlib_Set_Blend(&avm_bsd_view_handle, &blend_Parameter_buf);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 设置环视显示窗口数据(opengl) */
    view_gl_parameter_init(MODE_AVM_BSD, view_gl_parameter_p);
	
	/* 设置OPENGL显示的窗口参数 */
    ret = avm_bsd_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_bsd_view_handle, view_gl_parameter_p[0], 0);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}	
}
static AVM_ERR avm_ori_view_init()
{
	AVM_ERR ret;
	int cam_index;
	view_2d_parameter_t *view_gl_parameter_p[5];

	avm_ori_view_handle.avm_type = singleview_type;
	
	/* 创建各种资源 */
	ret = avmlib_2d_Init(&avm_ori_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 确定此模式使用的shader，目前shader用的是AVM 2D的shader程序*/
	avm_ori_view_handle.avm2d_shader_type->avmlib_Shader_Init(&avm_ori_view_handle, AVM_SV_VERT, AVM_SV_FRAG);
	if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* VBO查找表的尺寸初始化 */
	view_vbo_init(MODE_ORI, &view_vbo_parameter_buf);
	
	/* 生成VBO */
    ret = avm_ori_view_handle.avm2d_object_type->avmlib_Gen_ObjBuffer(&avm_ori_view_handle, VBO_ORI_VIEW, 4,  AVM_NULL);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* 设置环视矫正效果 */
	blend_init(MODE_ORI,&blend_Parameter_buf);
	
	/* 设置blend参数 */
    ret = avm_ori_view_handle.avm2d_render_type->avmlib_Set_Blend(&avm_ori_view_handle, &blend_Parameter_buf);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	view_gl_parameter_init(MODE_ORI, view_gl_parameter_p);
	
	/* 设置OPENGL显示的窗口参数 */
	for(cam_index = 0; cam_index < 4; cam_index++)
	{
		ret = avm_ori_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_ori_view_handle, view_gl_parameter_p[0], cam_index);
		if(ret != AVM_ERR_NONE) 
		{
			return ret;
		}
	}	

}

static AVM_ERR avm_ori_split_view_init()
{
	AVM_ERR ret;
	int cam_index;
	view_2d_parameter_t *view_gl_parameter_p[5];

	avm_ori_split_view_handle.avm_type = singleview_type;
	
	/* 创建各种资源 */
	ret = avmlib_2d_Init(&avm_ori_split_view_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	/* 确定此模式使用的shader，目前shader用的是AVM 2D的shader程序*/
	avm_ori_split_view_handle.avm2d_shader_type->avmlib_Shader_Init(&avm_ori_split_view_handle, AVM_SV_VERT, AVM_SV_FRAG);
	if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* 生成VBO */
    ret = avm_ori_split_view_handle.avm2d_object_type->avmlib_Gen_ObjBuffer(&avm_ori_split_view_handle, VBO_ORI_VIEW, 4,  AVM_NULL);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	/* 设置环视矫正效果 */
	blend_init(MODE_ORI,&blend_Parameter_buf);
	
	/* 设置blend参数 */
    ret = avm_ori_split_view_handle.avm2d_render_type->avmlib_Set_Blend(&avm_ori_split_view_handle, &blend_Parameter_buf);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	
	/* 设置OPENGL显示的窗口参数 */
	view_gl_parameter_init(MODE_ORI, view_gl_parameter_p);
 	
	avm_ori_split_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_ori_split_view_handle, view_gl_parameter_p[0 + 1], 0);
	
	avm_ori_split_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_ori_split_view_handle, view_gl_parameter_p[0 + 2], 1);
	
	avm_ori_split_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_ori_split_view_handle, view_gl_parameter_p[0 + 3], 2);
	
	avm_ori_split_view_handle.avm2d_render_type->avmlib_Set_ViewParameter(&avm_ori_split_view_handle, view_gl_parameter_p[0 + 4], 3);

}

static AVM_ERR view_vbo_init(AVM_MODE mode, lut_parameter_t *view_vbo_parameter_buf )
{
	view_vbo_parameter_buf->pavm = &virtual_camera_avm_param_buf[mode];
	view_vbo_parameter_buf->psgv = &virtual_camera_sgv_param_buf[mode];
	
	 switch(mode)
    {
        case MODE_AVM_2D://环视       
            virtual_camera_avm_param_buf[mode].vc_height 		= 480;			//虚拟相机高度
            virtual_camera_avm_param_buf[mode].vc_width  		= 280; 			//虚拟相机宽度
            virtual_camera_avm_param_buf[mode].srcW 			= CAM_WIDTH;   	//原始图宽度
            virtual_camera_avm_param_buf[mode].srcH 			= CAM_HEIGHT;  	//原始图高度
            virtual_camera_avm_param_buf[mode].tri_x_div 		= 8;			/* opengl三角形x跨度 */
            virtual_camera_avm_param_buf[mode].tri_y_div 		= 8;			/* opengl三角形y跨度 */
            virtual_camera_avm_param_buf[mode].OFRONTY   		= -4; 			/* 前相机y方向的偏移 */
            virtual_camera_avm_param_buf[mode].OLEFTX    		= 0;  			/* 右相机x方向的偏移 */
            virtual_camera_avm_param_buf[mode].ORIGHTX   		= 0; 			/* 前相机y方向的偏移 */
            virtual_camera_avm_param_buf[mode].OBACKY    		= 8;  			/* 左相机x方向的偏移 */
            virtual_camera_avm_param_buf[mode].BLEND_ANGLE_H  = 90;			/* alpha结束角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].BLEND_ANGLE_M  = 75;			/* alpha过度角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].BLEND_ANGLE_L  = 60;			/* alpha起始角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].REAR_BLEND_ANGLE_H  = 90;			/* alpha结束角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].REAR_BLEND_ANGLE_M  = 75;			/* alpha过度角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].REAR_BLEND_ANGLE_L  = 60;			/* alpha起始角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].scale_pixel_mm = 23;			//缩放系数(一个像素代表实际的距离 eg. 22表示1个像素代表实际距离23mm)

            view_vbo_parameter_buf->lut_savepath = AVM_NULL;			//(C8 *)VBO_2D_TOPVIEW;
            break;
        case MODE_AVM_LDWS://环视       
            virtual_camera_avm_param_buf[mode].vc_height 		= 480;			//虚拟相机高度
            virtual_camera_avm_param_buf[mode].vc_width  		= 256; 			//虚拟相机宽度
            virtual_camera_avm_param_buf[mode].srcW 			= CAM_WIDTH;   	//原始图宽度
            virtual_camera_avm_param_buf[mode].srcH 			= CAM_HEIGHT;  	//原始图高度
            virtual_camera_avm_param_buf[mode].tri_x_div 		= 6;			/* opengl三角形x跨度 */
            virtual_camera_avm_param_buf[mode].tri_y_div 		= 6;			/* opengl三角形y跨度 */
            virtual_camera_avm_param_buf[mode].OFRONTY   		= 0; 			/* 前相机y方向的偏移 */
            virtual_camera_avm_param_buf[mode].OLEFTX    		= 0;  			/* 右相机x方向的偏移 */
            virtual_camera_avm_param_buf[mode].ORIGHTX   		= 0; 			/* 前相机y方向的偏移 */
            virtual_camera_avm_param_buf[mode].OBACKY    		= 0;  			/* 左相机x方向的偏移 */
            virtual_camera_avm_param_buf[mode].BLEND_ANGLE_H  = 90;			/* alpha结束角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].BLEND_ANGLE_M  = 75;			/* alpha过度角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].BLEND_ANGLE_L  = 0;			/* alpha起始角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].REAR_BLEND_ANGLE_H  = 90;			/* alpha结束角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].REAR_BLEND_ANGLE_M  = 75;			/* alpha过度角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].REAR_BLEND_ANGLE_L  = 0;			/* alpha起始角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].scale_pixel_mm = 23;			//缩放系数(一个像素代表实际的距离 eg. 22表示1个像素代表实际距离23mm)

            view_vbo_parameter_buf->lut_savepath = AVM_NULL;			//(C8 *)VBO_2D_TOPVIEW;
            break;
        case MODE_AVM_BSD://环视       
            virtual_camera_avm_param_buf[mode].vc_height 		= 384;			//虚拟相机高度
            virtual_camera_avm_param_buf[mode].vc_width  		= 320; 			//虚拟相机宽度
            virtual_camera_avm_param_buf[mode].srcW 			= CAM_WIDTH;   	//原始图宽度
            virtual_camera_avm_param_buf[mode].srcH 			= CAM_HEIGHT;  	//原始图高度
            virtual_camera_avm_param_buf[mode].tri_x_div 		= 6;			/* opengl三角形x跨度 */
            virtual_camera_avm_param_buf[mode].tri_y_div 		= 6;			/* opengl三角形y跨度 */
            virtual_camera_avm_param_buf[mode].OFRONTY   		= 0; 			/* 前相机y方向的偏移 */
            virtual_camera_avm_param_buf[mode].OLEFTX    		= 0;  			/* 右相机x方向的偏移 */
            virtual_camera_avm_param_buf[mode].ORIGHTX   		= 0; 			/* 前相机y方向的偏移 */
            virtual_camera_avm_param_buf[mode].OBACKY    		= 0;  			/* 左相机x方向的偏移 */
            virtual_camera_avm_param_buf[mode].BLEND_ANGLE_H  = 90;			/* alpha结束角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].BLEND_ANGLE_M  = 45;			/* alpha过度角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].BLEND_ANGLE_L  = 0;			/* alpha起始角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].REAR_BLEND_ANGLE_H  = 90;			/* alpha结束角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].REAR_BLEND_ANGLE_M  = 45;			/* alpha过度角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].REAR_BLEND_ANGLE_L  = 0;			/* alpha起始角度 (0-90)*/
            virtual_camera_avm_param_buf[mode].scale_pixel_mm = 49;			//缩放系数(一个像素代表实际的距离 eg. 22表示1个像素代表实际距离23mm)

            view_vbo_parameter_buf->lut_savepath = AVM_NULL;			//(C8 *)VBO_2D_TOPVIEW;
            break;
			
		case MODE_SV:
			virtual_camera_sgv_param_buf[mode].srcW 			= CAM_WIDTH;    //原始图宽度
            virtual_camera_sgv_param_buf[mode].srcH 			= CAM_HEIGHT;   //原始图高度
            virtual_camera_sgv_param_buf[mode].tri_x_div 		= 15;			/* opengl三角形x跨度 */
            virtual_camera_sgv_param_buf[mode].tri_y_div 		= 15;			/* opengl三角形y跨度 */
            //前
            virtual_camera_sgv_param_buf[mode].vc_height[0] 	= 434; 			//虚拟相机高度
            virtual_camera_sgv_param_buf[mode].vc_width[0] 	= 360;   		//虚拟相机宽度
            virtual_camera_sgv_param_buf[mode].mirror[0] 		= 0; 			/* 是否左右镜像 1:镜像 0:不镜像*/
            virtual_camera_sgv_param_buf[mode].vmode[0] 		= VCAMPARAM_ABS;/* 参数使用方式 */
            virtual_camera_sgv_param_buf[mode].fovx_minus[0] 	= -60;			/* 视点平面X负方向视野角度 */
            virtual_camera_sgv_param_buf[mode].fovx_plus[0] 	= 60;			/* 视点平面X正方向视野角度 */
            virtual_camera_sgv_param_buf[mode].fovy_minus[0] 	= -35;			/* 视点平面Y负方向视野角度 */
            virtual_camera_sgv_param_buf[mode].fovy_plus[0] 	= 50;			/* 视点平面Y正方向视野角度 */
            virtual_camera_sgv_param_buf[mode].pose[0][3]   	= 0;   			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].pose[0][4]   	= 2;			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].pose[0][5]   	= -1.0;			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].xi_ratio[0]  	= 0.1;			/* 鱼眼系数 */
            virtual_camera_sgv_param_buf[mode].stretch[0]   	= 1;   			/* 拉伸系数 */

            //右
            virtual_camera_sgv_param_buf[mode].vc_height[1] 	= 434; 
            virtual_camera_sgv_param_buf[mode].vc_width[1] 	= 360; 
            virtual_camera_sgv_param_buf[mode].mirror[1] 		= 0; 
            virtual_camera_sgv_param_buf[mode].vmode[1] 		= VCAMPARAM_ABS; 
            virtual_camera_sgv_param_buf[mode].fovx_minus[1] 	=-38;
            virtual_camera_sgv_param_buf[mode].fovx_plus[1] 	= 60;
            virtual_camera_sgv_param_buf[mode].fovy_minus[1] 	=-53;
            virtual_camera_sgv_param_buf[mode].fovy_plus[1] 	= 45;
            virtual_camera_sgv_param_buf[mode].pose[1][3]   	= 2;
            virtual_camera_sgv_param_buf[mode].pose[1][4]   	= 0;
            virtual_camera_sgv_param_buf[mode].pose[1][5]   	= -1.7;
            virtual_camera_sgv_param_buf[mode].xi_ratio[1]  	= 0.09;
            virtual_camera_sgv_param_buf[mode].stretch[1]   	= 1;    
            //后
            virtual_camera_sgv_param_buf[mode].vc_height[2] 	= 434; 
            virtual_camera_sgv_param_buf[mode].vc_width[2] 	= 360;   
            virtual_camera_sgv_param_buf[mode].mirror[2] 		= 1; 
            virtual_camera_sgv_param_buf[mode].vmode[2] 		= VCAMPARAM_ABS;  
            virtual_camera_sgv_param_buf[mode].fovx_minus[2] 	= -45; 
            virtual_camera_sgv_param_buf[mode].fovx_plus[2] 	= 45;   
            virtual_camera_sgv_param_buf[mode].fovy_minus[2] 	= -50;
            virtual_camera_sgv_param_buf[mode].fovy_plus[2] 	= 40; 
            virtual_camera_sgv_param_buf[mode].pose[2][3]   	= 0;     
            virtual_camera_sgv_param_buf[mode].pose[2][4]   	= -2;  
            virtual_camera_sgv_param_buf[mode].pose[2][5]   	= -1.5;   
            virtual_camera_sgv_param_buf[mode].xi_ratio[2]  	= 0.1;
            virtual_camera_sgv_param_buf[mode].stretch[2]   	= 1;     

            //左
            virtual_camera_sgv_param_buf[mode].vc_height[3] 	= 434; 
            virtual_camera_sgv_param_buf[mode].vc_width[3] 	= 360;   
            virtual_camera_sgv_param_buf[mode].mirror[3] 		= 0; 
            virtual_camera_sgv_param_buf[mode].vmode[3] 		= VCAMPARAM_ABS;  
            virtual_camera_sgv_param_buf[mode].fovx_minus[3] 	=-60;
            virtual_camera_sgv_param_buf[mode].fovx_plus[3] 	= 38;
            virtual_camera_sgv_param_buf[mode].fovy_minus[3] 	= -60;
            virtual_camera_sgv_param_buf[mode].fovy_plus[3] 	= 40;
            virtual_camera_sgv_param_buf[mode].pose[3][3]   	= -2;
            virtual_camera_sgv_param_buf[mode].pose[3][4]   	= 0;
            virtual_camera_sgv_param_buf[mode].pose[3][5]   	= -1.7;
            virtual_camera_sgv_param_buf[mode].xi_ratio[3]  	= 0.09;
            virtual_camera_sgv_param_buf[mode].stretch[3]   	= 1;     
            
			view_vbo_parameter_buf->lut_savepath = AVM_NULL;			//VBO_2D_LARGEVIEW;
            break;
		case MODE_PER:
			virtual_camera_sgv_param_buf[mode].srcW 			= CAM_WIDTH;    //原始图宽度
            virtual_camera_sgv_param_buf[mode].srcH 			= CAM_HEIGHT;   //原始图高度
            virtual_camera_sgv_param_buf[mode].tri_x_div 		= 15;			/* opengl三角形x跨度 */
            virtual_camera_sgv_param_buf[mode].tri_y_div 		= 15;			/* opengl三角形y跨度 */

            /* front screen left */
            virtual_camera_sgv_param_buf[mode].vc_height[0] 	= 298; 			//虚拟相机高度
            virtual_camera_sgv_param_buf[mode].vc_width[0] 	= 394;   		//虚拟相机宽度
            virtual_camera_sgv_param_buf[mode].mirror[0] 		= 0; 			/* 是否左右镜像 1:镜像 0:不镜像*/
            virtual_camera_sgv_param_buf[mode].vmode[0] 		= VCAMPARAM_ABS;/* 参数使用方式 */
            virtual_camera_sgv_param_buf[mode].fovx_minus[0] 	= -30;			/* 视点平面X负方向视野角度 */
            virtual_camera_sgv_param_buf[mode].fovx_plus[0] 	= 30;			/* 视点平面X正方向视野角度 */
            virtual_camera_sgv_param_buf[mode].fovy_minus[0] 	= -30;			/* 视点平面Y负方向视野角度 */
            virtual_camera_sgv_param_buf[mode].fovy_plus[0] 	= 60;			/* 视点平面Y正方向视野角度 */
            virtual_camera_sgv_param_buf[mode].pose[0][0]   	= 0;   			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].pose[0][1]   	= 0;			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].pose[0][3]   	= 0;			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].pose[0][3]   	= -1;   			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].pose[0][4]   	= tan(PI * 30.0/180.0);	/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].pose[0][5]   	= 0;			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].xi_ratio[0]  	= 0.05;			/* 鱼眼系数 */
            virtual_camera_sgv_param_buf[mode].stretch[0]   	= 1;   			/* 拉伸系数 */

            /* front screen right */
            virtual_camera_sgv_param_buf[mode].vc_height[1] 	= 298; 
            virtual_camera_sgv_param_buf[mode].vc_width[1] 	= 394; 
            virtual_camera_sgv_param_buf[mode].mirror[1] 		= 0; 
            virtual_camera_sgv_param_buf[mode].vmode[1] 		= VCAMPARAM_ABS; 
            virtual_camera_sgv_param_buf[mode].fovx_minus[1] 	=-30;
            virtual_camera_sgv_param_buf[mode].fovx_plus[1] 	= 30;
            virtual_camera_sgv_param_buf[mode].fovy_minus[1] 	=-30;
            virtual_camera_sgv_param_buf[mode].fovy_plus[1] 	= 60;
            virtual_camera_sgv_param_buf[mode].pose[1][0]   	= 0;
            virtual_camera_sgv_param_buf[mode].pose[1][1]   	= 0;
            virtual_camera_sgv_param_buf[mode].pose[1][2]   	= 0;
            virtual_camera_sgv_param_buf[mode].pose[1][3]   	= 1;
            virtual_camera_sgv_param_buf[mode].pose[1][4]   	= tan(PI * 30.0/180.0);
            virtual_camera_sgv_param_buf[mode].pose[1][5]   	= 0;
            virtual_camera_sgv_param_buf[mode].xi_ratio[1]  	= 0.05;
            virtual_camera_sgv_param_buf[mode].stretch[1]   	= 1;    
            
			/* back screen right */
            virtual_camera_sgv_param_buf[mode].vc_height[2] 	= 298; 
            virtual_camera_sgv_param_buf[mode].vc_width[2] 	= 394;   
            virtual_camera_sgv_param_buf[mode].mirror[2] 		= 0; 
            virtual_camera_sgv_param_buf[mode].vmode[2] 		= VCAMPARAM_ABS;  
            virtual_camera_sgv_param_buf[mode].fovx_minus[2] 	= -30; 
            virtual_camera_sgv_param_buf[mode].fovx_plus[2] 	= 5;   
            virtual_camera_sgv_param_buf[mode].fovy_minus[2] 	= -30;
            virtual_camera_sgv_param_buf[mode].fovy_plus[2] 	= 35; 
            virtual_camera_sgv_param_buf[mode].pose[2][0]   	= 0;     
            virtual_camera_sgv_param_buf[mode].pose[2][1]   	= 0;  
            virtual_camera_sgv_param_buf[mode].pose[2][2]   	= 0;   
            virtual_camera_sgv_param_buf[mode].pose[2][3]   	= -1;     
            virtual_camera_sgv_param_buf[mode].pose[2][4]   	= -tan(PI * 30.0/180.0);  
            virtual_camera_sgv_param_buf[mode].pose[2][5]   	= 0;   
            virtual_camera_sgv_param_buf[mode].xi_ratio[2]  	= 0.05;
            virtual_camera_sgv_param_buf[mode].stretch[2]   	= 1;     

            /* back screen left */
            virtual_camera_sgv_param_buf[mode].vc_height[3] 	= 298; 
            virtual_camera_sgv_param_buf[mode].vc_width[3] 	= 394;   
            virtual_camera_sgv_param_buf[mode].mirror[3] 		= 0; 
            virtual_camera_sgv_param_buf[mode].vmode[3] 		= VCAMPARAM_ABS;  
            virtual_camera_sgv_param_buf[mode].fovx_minus[3] 	=-0;
            virtual_camera_sgv_param_buf[mode].fovx_plus[3] 	= 30;
            virtual_camera_sgv_param_buf[mode].fovy_minus[3] 	= -30;
            virtual_camera_sgv_param_buf[mode].fovy_plus[3] 	= 35;
            virtual_camera_sgv_param_buf[mode].pose[3][0]   	= 0;
            virtual_camera_sgv_param_buf[mode].pose[3][1]   	= 0;
            virtual_camera_sgv_param_buf[mode].pose[3][2]   	= 0;
            virtual_camera_sgv_param_buf[mode].pose[3][3]   	= 1;
            virtual_camera_sgv_param_buf[mode].pose[3][4]   	= -tan(PI * 30.0/180.0);
            virtual_camera_sgv_param_buf[mode].pose[3][5]   	= 0;
            virtual_camera_sgv_param_buf[mode].xi_ratio[3]  	= 0.05;
            virtual_camera_sgv_param_buf[mode].stretch[3]   	= 1;     
            
			view_vbo_parameter_buf->lut_savepath = AVM_NULL;			//VBO_2D_LARGEVIEW;
            break;
		case MODE_PER_CENTER:
			virtual_camera_sgv_param_buf[mode].srcW 			= CAM_WIDTH;    //原始图宽度
            virtual_camera_sgv_param_buf[mode].srcH 			= CAM_HEIGHT;   //原始图高度
            virtual_camera_sgv_param_buf[mode].tri_x_div 		= 15;			/* opengl三角形x跨度 */
            virtual_camera_sgv_param_buf[mode].tri_y_div 		= 15;			/* opengl三角形y跨度 */

            //前
            virtual_camera_sgv_param_buf[mode].vc_height[0] 	= 298; 			//虚拟相机高度
            virtual_camera_sgv_param_buf[mode].vc_width[0] 	= 394;   		//虚拟相机宽度
            virtual_camera_sgv_param_buf[mode].mirror[0] 		= 0; 			/* 是否左右镜像 1:镜像 0:不镜像*/
            virtual_camera_sgv_param_buf[mode].vmode[0] 		= VCAMPARAM_ABS;/* 参数使用方式 */
            virtual_camera_sgv_param_buf[mode].fovx_minus[0] 	= -30;			/* 视点平面X负方向视野角度 */
            virtual_camera_sgv_param_buf[mode].fovx_plus[0] 	= 30;			/* 视点平面X正方向视野角度 */
            virtual_camera_sgv_param_buf[mode].fovy_minus[0] 	= -30;			/* 视点平面Y负方向视野角度 */
            virtual_camera_sgv_param_buf[mode].fovy_plus[0] 	= 60;			/* 视点平面Y正方向视野角度 */
            virtual_camera_sgv_param_buf[mode].pose[0][0]   	= 0;   			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].pose[0][1]   	= 0;			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].pose[0][2]   	= 0;			/* 虚拟相机视点参数，包括位置，方向 */
           
			virtual_camera_sgv_param_buf[mode].pose[0][3]   	= 0;   			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].pose[0][4]   	= 1;			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].pose[0][5]   	= 0;			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].xi_ratio[0]  	= 0.05;			/* 鱼眼系数 */
            virtual_camera_sgv_param_buf[mode].stretch[0]   	= 1;   			/* 拉伸系数 */

         
            //后
            virtual_camera_sgv_param_buf[mode].vc_height[2] 	= 298; 
            virtual_camera_sgv_param_buf[mode].vc_width[2] 	= 394;   
            virtual_camera_sgv_param_buf[mode].mirror[2] 		= 0; 
            virtual_camera_sgv_param_buf[mode].vmode[2] 		= VCAMPARAM_ABS;  
            virtual_camera_sgv_param_buf[mode].fovx_minus[2] 	= -30; 
            virtual_camera_sgv_param_buf[mode].fovx_plus[2] 	= 30;   
            virtual_camera_sgv_param_buf[mode].fovy_minus[2] 	= -30;
            virtual_camera_sgv_param_buf[mode].fovy_plus[2] 	= 35; 
            virtual_camera_sgv_param_buf[mode].pose[2][0]   	= 0;     
            virtual_camera_sgv_param_buf[mode].pose[2][1]   	= 0;  
            virtual_camera_sgv_param_buf[mode].pose[2][2]   	= 0;  
			
            virtual_camera_sgv_param_buf[mode].pose[2][3]   	= 0;     
            virtual_camera_sgv_param_buf[mode].pose[2][4]   	= -1;  
            virtual_camera_sgv_param_buf[mode].pose[2][5]   	= 0;   
            virtual_camera_sgv_param_buf[mode].xi_ratio[2]  	= 0.05;
            virtual_camera_sgv_param_buf[mode].stretch[2]   	= 1;     

         
            
			view_vbo_parameter_buf->lut_savepath = AVM_NULL;			//VBO_2D_LARGEVIEW;
            break;
		case MODE_SVFS:
			virtual_camera_sgv_param_buf[mode].srcW 			= CAM_WIDTH;    //原始图宽度
            virtual_camera_sgv_param_buf[mode].srcH 			= CAM_HEIGHT;   //原始图高度
            virtual_camera_sgv_param_buf[mode].tri_x_div 		= 15;			/* opengl三角形x跨度 */
            virtual_camera_sgv_param_buf[mode].tri_y_div 		= 15;			/* opengl三角形y跨度 */

            //右
            virtual_camera_sgv_param_buf[mode].vc_height[3] 	= 434; 
            virtual_camera_sgv_param_buf[mode].vc_width[3] 	= 360;   
            virtual_camera_sgv_param_buf[mode].mirror[3] 		= 0; 
            virtual_camera_sgv_param_buf[mode].vmode[3] 		= VCAMPARAM_ABS;  
            virtual_camera_sgv_param_buf[mode].fovx_minus[3] 	=-65;
            virtual_camera_sgv_param_buf[mode].fovx_plus[3] 	= 20;
            virtual_camera_sgv_param_buf[mode].fovy_minus[3] 	= -60;
            virtual_camera_sgv_param_buf[mode].fovy_plus[3] 	= 60;
            virtual_camera_sgv_param_buf[mode].pose[3][3]   	= -90;
            virtual_camera_sgv_param_buf[mode].pose[3][4]   	= 600;
            virtual_camera_sgv_param_buf[mode].pose[3][5]   	= -899;
            virtual_camera_sgv_param_buf[mode].xi_ratio[3]  	= 0.09;
            virtual_camera_sgv_param_buf[mode].stretch[3]   	= 1;     

         
            //左
            virtual_camera_sgv_param_buf[mode].vc_height[1] 	= 434; 
            virtual_camera_sgv_param_buf[mode].vc_width[1] 	= 360; 
            virtual_camera_sgv_param_buf[mode].mirror[1] 		= 0; 
            virtual_camera_sgv_param_buf[mode].vmode[1] 		= VCAMPARAM_ABS; 
            virtual_camera_sgv_param_buf[mode].fovx_minus[1] 	=-18;
            virtual_camera_sgv_param_buf[mode].fovx_plus[1] 	= 65;
            virtual_camera_sgv_param_buf[mode].fovy_minus[1] 	=-53;
            virtual_camera_sgv_param_buf[mode].fovy_plus[1] 	= 60;
            virtual_camera_sgv_param_buf[mode].pose[1][3]   	= 90;
            virtual_camera_sgv_param_buf[mode].pose[1][4]   	= 600;
            virtual_camera_sgv_param_buf[mode].pose[1][5]   	= -899;
            virtual_camera_sgv_param_buf[mode].xi_ratio[1]  	= 0.09;
            virtual_camera_sgv_param_buf[mode].stretch[1]   	= 1;    

         
            
			view_vbo_parameter_buf->lut_savepath = AVM_NULL;			//VBO_2D_LARGEVIEW;
            break;
		case MODE_BACKUP:
			virtual_camera_sgv_param_buf[mode].srcW 			= CAM_WIDTH;    //原始图宽度
            virtual_camera_sgv_param_buf[mode].srcH 			= CAM_HEIGHT;   //原始图高度
            virtual_camera_sgv_param_buf[mode].tri_x_div 		= 15;			/* opengl三角形x跨度 */
            virtual_camera_sgv_param_buf[mode].tri_y_div 		= 15;			/* opengl三角形y跨度 */

            //前
            virtual_camera_sgv_param_buf[mode].vc_height[0] 	= 434; 			//虚拟相机高度
            virtual_camera_sgv_param_buf[mode].vc_width[0] 	= 360;   		//虚拟相机宽度
            virtual_camera_sgv_param_buf[mode].mirror[0] 		= 0; 			/* 是否左右镜像 1:镜像 0:不镜像*/
            virtual_camera_sgv_param_buf[mode].vmode[0] 		= VCAMPARAM_ABS;/* 参数使用方式 */
            virtual_camera_sgv_param_buf[mode].fovx_minus[0] 	= -80;			/* 视点平面X负方向视野角度 */
            virtual_camera_sgv_param_buf[mode].fovx_plus[0] 	= 80;			/* 视点平面X正方向视野角度 */
            virtual_camera_sgv_param_buf[mode].fovy_minus[0] 	= -35;			/* 视点平面Y负方向视野角度 */
            virtual_camera_sgv_param_buf[mode].fovy_plus[0] 	= 87;			/* 视点平面Y正方向视野角度 */
            virtual_camera_sgv_param_buf[mode].pose[0][3]   	= 0;   			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].pose[0][4]   	= 1100;			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].pose[0][5]   	= -11;			/* 虚拟相机视点参数，包括位置，方向 */
            virtual_camera_sgv_param_buf[mode].xi_ratio[0]  	= 0.1;			/* 鱼眼系数 */
            virtual_camera_sgv_param_buf[mode].stretch[0]   	= 1;   			/* 拉伸系数 */

            //右
            virtual_camera_sgv_param_buf[mode].vc_height[1] 	= 434; 
            virtual_camera_sgv_param_buf[mode].vc_width[1] 	= 360; 
            virtual_camera_sgv_param_buf[mode].mirror[1] 		= 0; 
            virtual_camera_sgv_param_buf[mode].vmode[1] 		= VCAMPARAM_ABS; 
            virtual_camera_sgv_param_buf[mode].fovx_minus[1] 	=-18;
            virtual_camera_sgv_param_buf[mode].fovx_plus[1] 	= 65;
            virtual_camera_sgv_param_buf[mode].fovy_minus[1] 	=-53;
            virtual_camera_sgv_param_buf[mode].fovy_plus[1] 	= 60;
            virtual_camera_sgv_param_buf[mode].pose[1][3]   	= 90;
            virtual_camera_sgv_param_buf[mode].pose[1][4]   	= 600;
            virtual_camera_sgv_param_buf[mode].pose[1][5]   	= -899;
            virtual_camera_sgv_param_buf[mode].xi_ratio[1]  	= 0.09;
            virtual_camera_sgv_param_buf[mode].stretch[1]   	= 1;    
            //后
            virtual_camera_sgv_param_buf[mode].vc_height[2] 	= 434; 
            virtual_camera_sgv_param_buf[mode].vc_width[2] 	= 360;   
            virtual_camera_sgv_param_buf[mode].mirror[2] 		= 1; 
            virtual_camera_sgv_param_buf[mode].vmode[2] 		= VCAMPARAM_ABS;  
            virtual_camera_sgv_param_buf[mode].fovx_minus[2] 	= -80; 
            virtual_camera_sgv_param_buf[mode].fovx_plus[2] 	= 75;   
            virtual_camera_sgv_param_buf[mode].fovy_minus[2] 	= -50;
            virtual_camera_sgv_param_buf[mode].fovy_plus[2] 	= 70; 
            virtual_camera_sgv_param_buf[mode].pose[2][3]   	= -150;     
            virtual_camera_sgv_param_buf[mode].pose[2][4]   	= -1050;  
            virtual_camera_sgv_param_buf[mode].pose[2][5]   	= -298;   
            virtual_camera_sgv_param_buf[mode].xi_ratio[2]  	= 0.1;
            virtual_camera_sgv_param_buf[mode].stretch[2]   	= 1;     

            //左
            virtual_camera_sgv_param_buf[mode].vc_height[3] 	= 434; 
            virtual_camera_sgv_param_buf[mode].vc_width[3] 	= 360;   
            virtual_camera_sgv_param_buf[mode].mirror[3] 		= 0; 
            virtual_camera_sgv_param_buf[mode].vmode[3] 		= VCAMPARAM_ABS;  
            virtual_camera_sgv_param_buf[mode].fovx_minus[3] 	=-65;
            virtual_camera_sgv_param_buf[mode].fovx_plus[3] 	= 20;
            virtual_camera_sgv_param_buf[mode].fovy_minus[3] 	= -60;
            virtual_camera_sgv_param_buf[mode].fovy_plus[3] 	= 60;
            virtual_camera_sgv_param_buf[mode].pose[3][3]   	= -90;
            virtual_camera_sgv_param_buf[mode].pose[3][4]   	= 600;
            virtual_camera_sgv_param_buf[mode].pose[3][5]   	= -899;
            virtual_camera_sgv_param_buf[mode].xi_ratio[3]  	= 0.09;
            virtual_camera_sgv_param_buf[mode].stretch[3]   	= 1;     
            
			view_vbo_parameter_buf->lut_savepath = AVM_NULL;			//VBO_2D_LARGEVIEW;
            break;
	}
}
static AVM_ERR blend_init(AVM_MODE mode, blend_2d_Parameter_t 	*blend_Parameter_buf)
{
	switch(mode)
    {
        case MODE_AVM_2D://环视
		case MODE_AVM_LDWS:
		case MODE_AVM_BSD:
			blend_Parameter_buf->blend_type = SRC_DST_ALPHA_2D;
			blend_Parameter_buf->rgb_correct[0] = avm_light_R;
			blend_Parameter_buf->rgb_correct[1] = avm_light_G;
			blend_Parameter_buf->rgb_correct[2] = avm_light_B;
			blend_Parameter_buf->ctl_blend[0] = 1.0;
			blend_Parameter_buf->ctl_blend[1] = avm_light_create;
			blend_Parameter_buf->ctl_blend[2] = avm_deinterlace;
            break;
		case MODE_SV:
			blend_Parameter_buf->blend_type = ONE_ZERO_ALPHA_2D;
			blend_Parameter_buf->rgb_correct[0] = avm_light_R;
			blend_Parameter_buf->rgb_correct[1] = avm_light_G;
			blend_Parameter_buf->rgb_correct[2] = avm_light_B;
			blend_Parameter_buf->ctl_blend[0] = 1.0;
			blend_Parameter_buf->ctl_blend[1] = avm_light_create;
			blend_Parameter_buf->ctl_blend[2] = avm_deinterlace;
			break;
		default:
			blend_Parameter_buf->blend_type = ONE_ZERO_ALPHA_2D;
			blend_Parameter_buf->rgb_correct[0] = avm_light_R;
			blend_Parameter_buf->rgb_correct[1] = avm_light_G;
			blend_Parameter_buf->rgb_correct[2] = avm_light_B;
			blend_Parameter_buf->ctl_blend[0] = 1.0;
			blend_Parameter_buf->ctl_blend[1] = avm_light_create;
			blend_Parameter_buf->ctl_blend[2] = avm_deinterlace;
			break;
	}

}
static AVM_ERR view_gl_parameter_init(AVM_MODE mode, view_2d_parameter_t *view_gl_parameter_p[5])
{
	int mode_index_offset = mode * 5;
	view_gl_parameter_p[0] = &view_gl_parameter_buf[mode_index_offset + 0];
	view_gl_parameter_p[1] = &view_gl_parameter_buf[mode_index_offset + 1];
	view_gl_parameter_p[2] = &view_gl_parameter_buf[mode_index_offset + 2];
	view_gl_parameter_p[3] = &view_gl_parameter_buf[mode_index_offset + 3];
	view_gl_parameter_p[4] = &view_gl_parameter_buf[mode_index_offset + 4];
	switch(mode)
    {
        case MODE_AVM_2D://环视      
			view_gl_parameter_buf[mode_index_offset + 0].scale.x          = 1.0200;
			view_gl_parameter_buf[mode_index_offset + 0].scale.y          = 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.z          = 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.x            = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.y            = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.z            = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.x             = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.y             = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.z             = -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.x             = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.y             = 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.z             = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_x         = 0.000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_y         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_z         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_x         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_y         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_z         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].left             = -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].right            = 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].top              = 1.0;
			view_gl_parameter_buf[mode_index_offset + 0].bottom           = -1.0;
			view_gl_parameter_buf[mode_index_offset + 0].screen_x         = SCREEN_X_2D;
			view_gl_parameter_buf[mode_index_offset + 0].screen_y         = SCREEN_Y_2D;
			view_gl_parameter_buf[mode_index_offset + 0].screen_width     = SCREEN_WIDTH_2D;
			view_gl_parameter_buf[mode_index_offset + 0].screen_height    = SCREEN_HEIGHT_2D;
			view_gl_parameter_buf[mode_index_offset + 0].reserved         = 0.0000;      

			view_gl_parameter_buf[mode_index_offset + 1].scale.x          = 1.4000;
			view_gl_parameter_buf[mode_index_offset + 1].scale.y          = 1.200;
			view_gl_parameter_buf[mode_index_offset + 1].scale.z          = 1.0000;
			view_gl_parameter_buf[mode_index_offset + 1].offset_x         = 0.000;
			view_gl_parameter_buf[mode_index_offset + 1].offset_y         = 0.0;
			view_gl_parameter_buf[mode_index_offset + 1].offset_z         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].rotate_x         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].rotate_y         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].rotate_z         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].reserved         = 0.0000;       			
			break;
		case MODE_SV:
			view_gl_parameter_buf[mode_index_offset + 0].scale.x         	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.y        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.z        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.y       		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.z        		= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.y        		= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.z				= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_y        	= -0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].left           	= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].right          	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].top            	= 1.0 ;
			view_gl_parameter_buf[mode_index_offset + 0].bottom         	= -1.0;
			view_gl_parameter_buf[mode_index_offset + 0].screen_x         = SCREEN_X_SV;
			view_gl_parameter_buf[mode_index_offset + 0].screen_y         = SCREEN_Y_SV;
			view_gl_parameter_buf[mode_index_offset + 0].screen_width     = SCREEN_WIDTH_SV;
			view_gl_parameter_buf[mode_index_offset + 0].screen_height  	= SCREEN_HEIGHT_SV;

			break;		
		case MODE_PER:
			//左
			view_gl_parameter_buf[mode_index_offset + 0].scale.x         	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.y        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.z        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.y       		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.z        		= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.y        		= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.z				= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_y        	= -0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].left           	= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].right          	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].top            	= 1.0 ;
			view_gl_parameter_buf[mode_index_offset + 0].bottom         	= -1.0;
			view_gl_parameter_buf[mode_index_offset + 0].screen_x         = SCREEN_X_PER_L;
			view_gl_parameter_buf[mode_index_offset + 0].screen_y         = SCREEN_Y_PER_L;
			view_gl_parameter_buf[mode_index_offset + 0].screen_width     = SCREEN_WIDTH_PER_L;
			view_gl_parameter_buf[mode_index_offset + 0].screen_height  	= SCREEN_HEIGHT_PER_L;
			
			//右
			view_gl_parameter_buf[mode_index_offset + 1].scale.x         	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 1].scale.y        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 1].scale.z        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 1].eye.x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].eye.y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].eye.z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].at.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].at.y       		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].at.z        		= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 1].up.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].up.y        		= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 1].up.z				= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].offset_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].offset_y        	= -0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].offset_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].rotate_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].rotate_y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].rotate_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].left           	= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 1].right          	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 1].top            	= 1.0 ;
			view_gl_parameter_buf[mode_index_offset + 1].bottom         	= -1.0;
			view_gl_parameter_buf[mode_index_offset + 1].screen_x         = SCREEN_X_PER_R;
			view_gl_parameter_buf[mode_index_offset + 1].screen_y         = SCREEN_Y_PER_R;
			view_gl_parameter_buf[mode_index_offset + 1].screen_width     = SCREEN_WIDTH_PER_R;
			view_gl_parameter_buf[mode_index_offset + 1].screen_height  	= SCREEN_HEIGHT_PER_R;

			break;
		case MODE_PER_CENTER:
			view_gl_parameter_buf[mode_index_offset + 0].scale.x         	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.y        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.z        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.y       		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.z        		= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.y        		= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.z				= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_y        	= -0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].left           	= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].right          	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].top            	= 1.0 ;
			view_gl_parameter_buf[mode_index_offset + 0].bottom         	= -1.0;
			view_gl_parameter_buf[mode_index_offset + 0].screen_x         = SCREEN_X_PER_CENTER;
			view_gl_parameter_buf[mode_index_offset + 0].screen_y         = SCREEN_Y_PER_CENTER;
			view_gl_parameter_buf[mode_index_offset + 0].screen_width     = SCREEN_WIDTH_PER_CENTER;
			view_gl_parameter_buf[mode_index_offset + 0].screen_height  	= SCREEN_HEIGHT_PER_CENTER;

			break;
		case MODE_SVFS:
		
			//左
			view_gl_parameter_buf[mode_index_offset + 0].scale.x         	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.y        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.z        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.y       		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.z        		= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.y        		= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.z				= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_y        	= -0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].left           	= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].right          	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].top            	= 1.0 ;
			view_gl_parameter_buf[mode_index_offset + 0].bottom         	= -1.0;
			view_gl_parameter_buf[mode_index_offset + 0].screen_x         = SCREEN_X_SVFS_L;
			view_gl_parameter_buf[mode_index_offset + 0].screen_y         = SCREEN_Y_SVFS_L;
			view_gl_parameter_buf[mode_index_offset + 0].screen_width     = SCREEN_WIDTH_SVFS_L;
			view_gl_parameter_buf[mode_index_offset + 0].screen_height  	= SCREEN_HEIGHT_SVFS_L;
			//右
			view_gl_parameter_buf[mode_index_offset + 1].scale.x         	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 1].scale.y        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 1].scale.z        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 1].eye.x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].eye.y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].eye.z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].at.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].at.y       		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].at.z        		= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 1].up.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].up.y        		= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 1].up.z				= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].offset_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].offset_y        	= -0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].offset_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].rotate_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].rotate_y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].rotate_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 1].left           	= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 1].right          	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 1].top            	= 1.0 ;
			view_gl_parameter_buf[mode_index_offset + 1].bottom         	= -1.0;
			view_gl_parameter_buf[mode_index_offset + 1].screen_x         = SCREEN_X_SVFS_R;
			view_gl_parameter_buf[mode_index_offset + 1].screen_y         = SCREEN_Y_SVFS_R;
			view_gl_parameter_buf[mode_index_offset + 1].screen_width     = SCREEN_WIDTH_SVFS_R;
			view_gl_parameter_buf[mode_index_offset + 1].screen_height  	= SCREEN_HEIGHT_SVFS_R;

			break;
		case MODE_BACKUP:
			view_gl_parameter_buf[mode_index_offset + 0].scale.x         	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.y        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.z        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.y       		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.z        		= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.y        		= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.z				= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_y        	= -0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].left           	= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].right          	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].top            	= 1.0 ;
			view_gl_parameter_buf[mode_index_offset + 0].bottom         	= -1.0;
			view_gl_parameter_buf[mode_index_offset + 0].screen_x         = 0;
			view_gl_parameter_buf[mode_index_offset + 0].screen_y         = 0;
			view_gl_parameter_buf[mode_index_offset + 0].screen_width     = 720;
			view_gl_parameter_buf[mode_index_offset + 0].screen_height  	= 480;

			break;		
		case MODE_ORI:
			view_gl_parameter_buf[mode_index_offset + 0].scale.x         	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.y        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.z        	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.y       		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.z        		= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.x        		= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.y        		= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.z				= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_y        	= -0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_x        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_y        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_z        	= 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].left           	= -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].right          	= 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].top            	= 1.0 ;
			view_gl_parameter_buf[mode_index_offset + 0].bottom         	= -1.0;
			view_gl_parameter_buf[mode_index_offset + 0].screen_x         = SCREEN_X_ORI;
			view_gl_parameter_buf[mode_index_offset + 0].screen_y         = SCREEN_Y_ORI;
			view_gl_parameter_buf[mode_index_offset + 0].screen_width     = SCREEN_WIDTH_ORI;
			view_gl_parameter_buf[mode_index_offset + 0].screen_height  	= SCREEN_HEIGHT_ORI;
			
			memcpy(&view_gl_parameter_buf[mode_index_offset + 1],&view_gl_parameter_buf[mode_index_offset + 0],sizeof(view_2d_parameter_t));
			memcpy(&view_gl_parameter_buf[mode_index_offset + 2],&view_gl_parameter_buf[mode_index_offset + 0],sizeof(view_2d_parameter_t));
			memcpy(&view_gl_parameter_buf[mode_index_offset + 3],&view_gl_parameter_buf[mode_index_offset + 0],sizeof(view_2d_parameter_t));
			memcpy(&view_gl_parameter_buf[mode_index_offset + 4],&view_gl_parameter_buf[mode_index_offset + 0],sizeof(view_2d_parameter_t));
			
			view_gl_parameter_buf[mode_index_offset + 1].screen_x         = SCREEN_X_ORI_SPLIT_L;
			view_gl_parameter_buf[mode_index_offset + 1].screen_y         = SCREEN_Y_ORI_SPLIT_L;
			view_gl_parameter_buf[mode_index_offset + 1].screen_width     = SCREEN_WIDTH_ORI_SPLIT_L;
			view_gl_parameter_buf[mode_index_offset + 1].screen_height    = SCREEN_HEIGHT_ORI_SPLIT_L;
			
			view_gl_parameter_buf[mode_index_offset + 2].screen_x         = SCREEN_X_ORI_SPLIT_R;
			view_gl_parameter_buf[mode_index_offset + 2].screen_y         = SCREEN_Y_ORI_SPLIT_R;
			view_gl_parameter_buf[mode_index_offset + 2].screen_width     = SCREEN_WIDTH_ORI_SPLIT_R;
			view_gl_parameter_buf[mode_index_offset + 2].screen_height    = SCREEN_HEIGHT_ORI_SPLIT_R;
			
			view_gl_parameter_buf[mode_index_offset + 3].screen_x         = SCREEN_X_ORI_SPLIT_F;
			view_gl_parameter_buf[mode_index_offset + 3].screen_y         = SCREEN_Y_ORI_SPLIT_F;
			view_gl_parameter_buf[mode_index_offset + 3].screen_width     = SCREEN_WIDTH_ORI_SPLIT_F;
			view_gl_parameter_buf[mode_index_offset + 3].screen_height    = SCREEN_HEIGHT_ORI_SPLIT_F;
			
			view_gl_parameter_buf[mode_index_offset + 4].screen_x         = SCREEN_X_ORI_SPLIT_B;
			view_gl_parameter_buf[mode_index_offset + 4].screen_y         = SCREEN_Y_ORI_SPLIT_B;
			view_gl_parameter_buf[mode_index_offset + 4].screen_width     = SCREEN_WIDTH_ORI_SPLIT_B;
			view_gl_parameter_buf[mode_index_offset + 4].screen_height    = SCREEN_HEIGHT_ORI_SPLIT_B;

			break;
		case MODE_AVM_LDWS://环视      
			view_gl_parameter_buf[mode_index_offset + 0].scale.x          = 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.y          = 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.z          = 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.x            = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.y            = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.z            = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.x             = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.y             = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.z             = -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.x             = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.y             = 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.z             = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_x         = 0.000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_y         = -0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_z         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_x         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_y         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_z         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].left             = -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].right            = 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].top              = 1.0;
			view_gl_parameter_buf[mode_index_offset + 0].bottom           = -1.0;

			view_gl_parameter_buf[mode_index_offset + 0].screen_x         = LDWS_VIEW_X;
			view_gl_parameter_buf[mode_index_offset + 0].screen_y         = LDWS_VIEW_Y;
			view_gl_parameter_buf[mode_index_offset + 0].screen_width     = LDWS_VIEW_WIDTH;
			view_gl_parameter_buf[mode_index_offset + 0].screen_height    = LDWS_VIEW_HEIGHT;
			view_gl_parameter_buf[mode_index_offset + 0].reserved         = 0.0000;      
			break;
			
		case MODE_AVM_BSD://环视      
			view_gl_parameter_buf[mode_index_offset + 0].scale.x          = 1.0200;
			view_gl_parameter_buf[mode_index_offset + 0].scale.y          = 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].scale.z          = 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.x            = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.y            = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].eye.z            = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.x             = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.y             = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].at.z             = -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.x             = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.y             = 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].up.z             = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_x         = 0.000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_y         = -0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].offset_z         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_x         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_y         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].rotate_z         = 0.0000;
			view_gl_parameter_buf[mode_index_offset + 0].left             = -1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].right            = 1.0000;
			view_gl_parameter_buf[mode_index_offset + 0].top              = 1.0;
			view_gl_parameter_buf[mode_index_offset + 0].bottom           = -1.0;

			view_gl_parameter_buf[mode_index_offset + 0].screen_x         = BSD_VIEW_X;;
			view_gl_parameter_buf[mode_index_offset + 0].screen_y         = BSD_VIEW_Y;
			view_gl_parameter_buf[mode_index_offset + 0].screen_width     = BSD_VIEW_WIDTH;
			view_gl_parameter_buf[mode_index_offset + 0].screen_height    = BSD_VIEW_HEIGHT;
			view_gl_parameter_buf[mode_index_offset + 0].reserved         = 0.0000;      
			break;

	}
}

AVM_ERR avm_disp_info_get(AVM_MODE mode,virtual_camera_avm_t *virtual_camera_avm_param_p, virtual_camera_sgv_t *virtual_camera_sgv_param_p, view_2d_parameter_t view_2d_parameter_param_p[4])
{
	int mode_index = mode * 5;
	if(mode >= MODE_MAX)
	{
		return AVM_ERR_FALSE;
	}
	if(virtual_camera_avm_param_p != NULL)
	{
		memcpy(virtual_camera_avm_param_p, &virtual_camera_avm_param_buf[mode], sizeof(virtual_camera_avm_t));
	}
	if(virtual_camera_sgv_param_p != NULL)
	{
		memcpy(virtual_camera_sgv_param_p, &virtual_camera_sgv_param_buf[mode], sizeof(virtual_camera_sgv_t));
	}
	if(view_2d_parameter_param_p != NULL)
	{
		switch(mode)
		{
			case MODE_AVM_2D://环视      
				memcpy(&view_2d_parameter_param_p[0], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[1], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[2], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[3], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				break;
			case MODE_SV:
				memcpy(&view_2d_parameter_param_p[0], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[1], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[2], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[3], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				break;		
			case MODE_SVFS:		
				//左
				memcpy(&view_2d_parameter_param_p[0], &view_gl_parameter_buf[mode_index + 1], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[1], &view_gl_parameter_buf[mode_index + 1], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[2], &view_gl_parameter_buf[mode_index + 3], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[3], &view_gl_parameter_buf[mode_index + 3], sizeof(view_2d_parameter_t));
				break;
			case MODE_BACKUP:
				memcpy(&view_2d_parameter_param_p[0], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[1], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[2], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[3], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				break;		
			case MODE_ORI:
				memcpy(&view_2d_parameter_param_p[0], &view_gl_parameter_buf[mode_index + 0], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[1], &view_gl_parameter_buf[mode_index + 1], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[2], &view_gl_parameter_buf[mode_index + 2], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[3], &view_gl_parameter_buf[mode_index + 3], sizeof(view_2d_parameter_t));
				break;
			case MODE_AVM_LDWS://环视      
				memcpy(&view_2d_parameter_param_p[0], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[1], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[2], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[3], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				break;			
			case MODE_AVM_BSD://环视      
				memcpy(&view_2d_parameter_param_p[0], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[1], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[2], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				memcpy(&view_2d_parameter_param_p[3], &view_gl_parameter_buf[mode_index], sizeof(view_2d_parameter_t));
				break;

		}
	}

}
