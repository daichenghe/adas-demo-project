/*******************************************************************************
* File Name          : avm_3d_module.cpp
* Author             : Daich
* Revision           : 2.1
* Date               : 01/06/2018
* Description        : avm_2d control and init code
*
* HISTORY***********************************************************************
* 01/06/2018  | sDas Basic 2.0                                            | Daich
*
*******************************************************************************/
#include "stdio.h"
#include "avmlib_sdk.h"
#include "avm_3d_module.h"
#include "avm_config.h"
#include "avm_view_config.h"
#include "stdlib.h"
#include <string.h>
#include "avm.h"
#include "avmlib_3d.h"
#include "avmlib_matrices.h"

#define GL_GLEXT_PROTOTYPES 1

#define TIRE_COUNT  					(4)	/* 车轮个数 */
#define AVM_FLY_COUNTERCLOCKWISE		(0)	/* 逆时针 */
#define AVM_FLY_CLOCKWISE				(1)	/* 顺时针 */
#define AVM_FLY_CLOCK_THRESHOLD			(4)	/* 顺/逆时针阈值 */
#define AVM_FLY_MODE_POINT_CNT			(8)	/* 旋转路径上有多少个点 */
#define AVM_3D_FLY_STEP_COUNT			(10)/* 两个模式之间跳过去需要的步数 */
#define AVM_3D_VIEW_PARAM_UPDATE_ON		(1)
#define AVM_3D_VIEW_PARAM_UPDATE_OFF	(2)

typedef struct VECTOR3F
{
	float x;
	float y;
	float z;
}vector3f_t;

typedef struct tire_offset
{
	float x;
	float y;
	float z;
} tire_offset_t;



static avm3d_type_t avm_3D_handle[SUBMODECNT];

static avm3d_type_t avm_3D_car_default_handle;
static avm3d_type_t avm_3D_tire_default_handle[TIRE_COUNT];
static tire_offset_t tire_offset_parame[TIRE_COUNT] = {{-961, 1638, 453}, {961, 1638, 453}, {961, -1778, 453}, {-961, -1778, 453}};
static avm3d_type_t avm_3D_chassis_default_handle;

static avm3d_type_t  avm_3D_car_handle[SUBMODECNT];
static avm3d_type_t  avm_3D_tire_handle[SUBMODECNT][TIRE_COUNT];
static avm3d_type_t  avm_3D_chassis_handle[SUBMODECNT];

static virtual_camera_avm3d_t 	view_avm3d_model_param_buf;
static lut_parameter_t 			view_vbo_parameter_buf;

static view_3d_parameter_t view_gl_parameter_3d[SUBMODECNT];
static view_3d_parameter_t view_gl_parameter_3dveh[SUBMODECNT];
static view_3d_parameter_t view_gl_parameter_3dtire[TIRE_COUNT];
static view_3d_parameter_t view_gl_parameter_3dchassis[SUBMODECNT];

static AVM_ERR view_vbo_init(MODE3D mode, lut_parameter_t *view_vbo_parameter_buf, view_3d_parameter_t *view_gl_parameter_buf);
static AVM_ERR avm_3d_view_init(MODE3D mode_3d_index);
static AVM_ERR avm_3d_calc_current_view_parameter(view_keep_t *view_keep,view_3d_parameter_t *view_gl_parameter_output,int * update_flag);
static AVM_ERR view_gl_parameter_init(MODE3D mode);
static AVM_ERR avm_3d_view_video_init(MODE3D mode_3d_index);
static AVM_ERR avm_3d_view_car_init(MODE3D mode_3d_index);


AVM_ERR default_3d_init()
{
	avm_3d_view_init(MODE_3D_ROTATE);
	
	return AVM_ERR_NONE;
}

AVM_ERR common_3d_init()
{
	avm_3d_view_init(MODE_3D_LEFTUP);
	avm_3d_view_init(MODE_3D_BACK);
	avm_3d_view_init(MODE_3D_RIGHTUP);
	avm_3d_view_init(MODE_3D_RIGHT);
	avm_3d_view_init(MODE_3D_FRONTRUP);
	avm_3d_view_init(MODE_3D_FRONT);
	avm_3d_view_init(MODE_3D_FRONTLUP);
	avm_3d_view_init(MODE_3D_LEFT);
	return AVM_ERR_NONE;
}

static void tire_process(avm3d_type_t *pAttr,F32 turn_angle, F32 run_angle) //????
{
	Vector4 rotation_axis;
	Matrix4 matrixModel;
	Vector4 rotation_axis_ori;

	static view_3d_parameter_t *in3dpar_tire[4] = {NULL, NULL, NULL, NULL};
	if (in3dpar_tire[0] == NULL)
	{
		in3dpar_tire[0] = &view_gl_parameter_3dtire[0];
	}
	if (in3dpar_tire[1] == NULL)
	{
		in3dpar_tire[1] = &view_gl_parameter_3dtire[1];
	}
	if (in3dpar_tire[2] == NULL)
	{
		in3dpar_tire[2] = &view_gl_parameter_3dtire[2];
	}
	if (in3dpar_tire[3] == NULL)
	{
		in3dpar_tire[3] = &view_gl_parameter_3dtire[3];
	}

	in3dpar_tire[0]->rotate_x -= run_angle;
	in3dpar_tire[1]->rotate_x -= run_angle;
	in3dpar_tire[2]->rotate_x -= run_angle;
	in3dpar_tire[3]->rotate_x -= run_angle;

	if (in3dpar_tire[0]->rotate_x < -360)
	{
		in3dpar_tire[0]->rotate_x = 0;
	}
	if (in3dpar_tire[1]->rotate_x < -360)
	{
		in3dpar_tire[1]->rotate_x = 0;
	}
	if (in3dpar_tire[2]->rotate_x < -360)
	{
		in3dpar_tire[2]->rotate_x = 0;
	}
	if (in3dpar_tire[3]->rotate_x < -360)
	{
		in3dpar_tire[3]->rotate_x = 0;
	}

	matrixModel.identity();
	matrixModel.rotateX(0);
	matrixModel.rotateY(in3dpar_tire[0]->rotate_y);
	matrixModel.rotateZ(in3dpar_tire[0]->rotate_z);

	rotation_axis_ori.set(0, 0, 1, 1);

	rotation_axis = matrixModel * rotation_axis_ori;

	in3dpar_tire[0]->rotate_axis  = rotation_axis;
	in3dpar_tire[0]->rotate_angle = turn_angle;
	in3dpar_tire[1]->rotate_axis  = rotation_axis;
	in3dpar_tire[1]->rotate_angle = turn_angle;

    for (int j = 0; j < TIRE_COUNT; j++)
	{
    avm_3D_tire_default_handle[j].pAttr = pAttr;
	avm_3D_tire_default_handle[j].avm3d_render_type->avmlib_Set_ViewParameter(&avm_3D_tire_default_handle[j], in3dpar_tire[j]);
	}

}

AVM_ERR avm_3d_rotate_360_set_view_parameter(int *rotate_complete_flag)
{
	static int rotate_angle = 0;
	static view_3d_parameter_t view_gl_parameter_out;
	int rotate_step = 9;
	
	*rotate_complete_flag = AVM_3D_ROTATE_360_ONGING;
	if (rotate_angle == 0)
	{
		memcpy(&view_gl_parameter_out,&view_gl_parameter_3d[MODE_3D_ROTATE],sizeof(view_3d_parameter_t));
	}
	else if(rotate_angle + rotate_step >= 360)
	{
		memcpy(&view_gl_parameter_out,&view_gl_parameter_3d[MODE_3D_ROTATE],sizeof(view_3d_parameter_t));
		*rotate_complete_flag = AVM_3D_ROTATE_360_COMPLETE;
		rotate_angle = 0;
	}
	if ((view_gl_parameter_out.pitchhead.z > 0) && (view_gl_parameter_3d[MODE_3D_ROTATE].pitchhead.z > 0))
	{ //use pitch and head
		view_gl_parameter_out.pitchhead.y += rotate_step;
		rotate_angle += 9;
	}
	else
	{
		view_gl_parameter_out.rotate_z += rotate_step;
		rotate_angle +=9;
	}
	avm_3D_handle[MODE_3D_ROTATE].avm3d_render_type->avmlib_Set_ViewParameter(&avm_3D_handle[MODE_3D_ROTATE], &view_gl_parameter_out);
	avm_3D_car_default_handle.pAttr = &avm_3D_handle[MODE_3D_ROTATE];
	avm_3D_car_default_handle.avm3d_render_type->avmlib_Set_ViewParameter(&avm_3D_car_default_handle, &view_gl_parameter_out);

	avm_3D_chassis_default_handle.pAttr = &avm_3D_handle[MODE_3D_ROTATE];
	avm_3D_chassis_default_handle.avm3d_render_type->avmlib_Set_ViewParameter(&avm_3D_chassis_default_handle, &view_gl_parameter_out);
	for (int j = 0; j < TIRE_COUNT; j++)
	{
		memcpy(&view_gl_parameter_3dtire[j], &view_gl_parameter_out, sizeof(view_3d_parameter_t));
		view_gl_parameter_3dtire[j].offset_x = tire_offset_parame[j].x;
		view_gl_parameter_3dtire[j].offset_y = tire_offset_parame[j].y;
		view_gl_parameter_3dtire[j].offset_z = tire_offset_parame[j].z;
		avm_3D_tire_default_handle[j].pAttr = &avm_3D_handle[MODE_3D_ROTATE];
		avm_3D_tire_default_handle[j].avm3d_render_type->avmlib_Set_ViewParameter(&avm_3D_tire_default_handle[j], &view_gl_parameter_3dtire[j]);
	}
}
AVM_ERR avm_3d_set_view_parameter(view_keep_t *view_keep)
{
	int j;
	int update_flag;
	static view_3d_parameter_t view_gl_parameter_out;
	static int isfirstin3d = 1;
	if (MODE_3D_ROTATE == view_keep->sub_mode && isfirstin3d)
	{
		int rotate_complete_flag;
		avm_3d_rotate_360_set_view_parameter(&rotate_complete_flag);
		if (rotate_complete_flag == AVM_3D_ROTATE_360_COMPLETE)
		{
			isfirstin3d = 0;
			view_keep->sub_mode = MODE_3D_BACK;
		}
		else
		{
			return AVM_ERR_NONE;
		}
	}
	avm_3d_calc_current_view_parameter(view_keep, &view_gl_parameter_out, &update_flag);
	if (update_flag == AVM_3D_VIEW_PARAM_UPDATE_ON)
	{
		avm_3D_handle[view_keep->sub_mode].avm3d_render_type->avmlib_Set_ViewParameter(&avm_3D_handle[view_keep->sub_mode], &view_gl_parameter_out);
			
		avm_3D_car_default_handle.pAttr = &avm_3D_handle[view_keep->sub_mode];
	//	view_gl_parameter_out.scale.y = 1.5;
		avm_3D_car_default_handle.avm3d_render_type->avmlib_Set_ViewParameter(&avm_3D_car_default_handle, &view_gl_parameter_out);
		
		avm_3D_chassis_default_handle.pAttr = &avm_3D_handle[view_keep->sub_mode];
		avm_3D_chassis_default_handle.avm3d_render_type->avmlib_Set_ViewParameter(&avm_3D_chassis_default_handle, &view_gl_parameter_out);
		for(j = 0; j < TIRE_COUNT; j++)
		{
				Matrix4 matrixModel;
				Vector4 ori_point;
				Vector4 cur_point;
				matrixModel.identity();
 
				memcpy(&view_gl_parameter_3dtire[j], &view_gl_parameter_out, sizeof(view_3d_parameter_t));
				matrixModel.rotateX(view_gl_parameter_out.rotate_x);
				matrixModel.rotateY(view_gl_parameter_out.rotate_y);
				matrixModel.rotateZ(view_gl_parameter_out.rotate_z);
                ori_point.set(tire_offset_parame[j].x,tire_offset_parame[j].y,tire_offset_parame[j].z, 1.0);
                cur_point = matrixModel * ori_point;
				view_gl_parameter_3dtire[j].offset_x = tire_offset_parame[j].x +view_gl_parameter_out.offset_x+(cur_point.x - ori_point.x);
				view_gl_parameter_3dtire[j].offset_y = tire_offset_parame[j].y +view_gl_parameter_out.offset_y+(cur_point.y - ori_point.y);
				view_gl_parameter_3dtire[j].offset_z = tire_offset_parame[j].z +view_gl_parameter_out.offset_z+(cur_point.z - ori_point.z);
			avm_3D_tire_default_handle[j].pAttr = &avm_3D_handle[view_keep->sub_mode];
			avm_3D_tire_default_handle[j].avm3d_render_type->avmlib_Set_ViewParameter(&avm_3D_tire_default_handle[j], &view_gl_parameter_3dtire[j]);
		}
	//	view_gl_parameter_out.scale.y = 1.0;
	}
}

AVM_ERR render_3d_avm(MODE3D mode_3d_index)
{
	AVM_ERR ret ;
	int i;

	ret = avm_3D_handle[mode_3d_index].avm3d_shader_type->avmlib_Use(&avm_3D_handle[mode_3d_index]);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_3D_handle[mode_3d_index].avm3d_render_type->avmlib_Render_Topview(&avm_3D_handle[mode_3d_index]);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
    ret = avm_3D_handle[mode_3d_index].avm3d_shader_type->avmlib_Unuse(&avm_3D_handle[mode_3d_index]);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
#if 1
	 ret = avm_3D_chassis_default_handle.avm3d_shader_type->avmlib_Use(&avm_3D_chassis_default_handle);
	 if (ret != AVM_ERR_NONE)
	 {
	 	return ret;
	 }
	 ret = avm_3D_chassis_default_handle.avm3d_render_type->avmlib_Render_Car(&avm_3D_chassis_default_handle);
	 if (ret != AVM_ERR_NONE)
	 {
		return ret;
	 }
	ret = avm_3D_chassis_default_handle.avm3d_shader_type->avmlib_Unuse(&avm_3D_chassis_default_handle);
	if (ret != AVM_ERR_NONE)
	{
		return ret;
	}

	F32 run_angle = ((10) / 2.28) / 30.0 * 360;
	tire_process(&avm_3D_handle[mode_3d_index],0, run_angle); //????
	for (i = 0; i < TIRE_COUNT; i++)
	{
		ret = avm_3D_tire_default_handle[i].avm3d_shader_type->avmlib_Use(&avm_3D_tire_default_handle[i]);
		if (ret != AVM_ERR_NONE)
		{
			return ret;
		}
		ret = avm_3D_tire_default_handle[i].avm3d_render_type->avmlib_Render_Car(&avm_3D_tire_default_handle[i]);
		if (ret != AVM_ERR_NONE)
		{
			return ret;
		}
		ret = avm_3D_tire_default_handle[i].avm3d_shader_type->avmlib_Unuse(&avm_3D_tire_default_handle[i]);
		if (ret != AVM_ERR_NONE)
		{
			return ret;
		}
	}
#endif

	ret = avm_3D_car_default_handle.avm3d_shader_type->avmlib_Use(&avm_3D_car_default_handle);
	if (ret != AVM_ERR_NONE)
	{
		return ret;
	}
	ret = avm_3D_car_default_handle.avm3d_render_type->avmlib_Render_Car(&avm_3D_car_default_handle);
	if (ret != AVM_ERR_NONE)
	{
		return ret;
	}
	ret = avm_3D_car_default_handle.avm3d_shader_type->avmlib_Unuse(&avm_3D_car_default_handle);
	if (ret != AVM_ERR_NONE)
	{
		return ret;
	}

}

static AVM_ERR avm_3d_view_init(MODE3D mode_3d_index)
{
	avm_3d_view_video_init(mode_3d_index);
	avm_3d_view_car_init(mode_3d_index);
}
static AVM_ERR avm_3d_view_video_init(MODE3D mode_3d_index)
{
	int j;
	AVM_ERR ret;
	
	C8 *vboname[] = {(C8*)VBO_3D_VIEW, (C8*)VBO_3D_VIEW1, (C8*)VBO_3D_VIEW2, (C8*)VBO_3D_VIEW3, (C8*)VBO_3D_VIEW4, 
						(C8*)VBO_3D_VIEW5, (C8*)VBO_3D_VIEW6, (C8*)VBO_3D_VIEW7, (C8*)VBO_3D_VIEW8, (C8*)VBO_3D_VIEW9,
						(C8*)VBO_3D_VIEW10, (C8*)VBO_3D_VIEW11, (C8*)VBO_3D_VIEW12, (C8*)VBO_3D_VIEW13, (C8*)VBO_3D_VIEW14, 
						(C8*)VBO_3D_VIEW15, (C8*)VBO_3D_VIEW16, (C8*)VBO_3D_VIEW17, (C8*)VBO_3D_VIEW18};
    C8 *pname[] = {(C8*)TIRE1_3D_OBJ, (C8*)TIRE2_3D_OBJ, (C8*)TIRE3_3D_OBJ, (C8*)TIRE4_3D_OBJ};
    blend_3d_Parameter_t blendParameter;
	
	avm_3D_handle[mode_3d_index].avm_type = avm3dview_type;
//	DbgPrintf("%s %d\n", __FUNCTION__, __LINE__);	
    ret = avmlib_3d_Init(&avm_3D_handle[mode_3d_index]);
    if(ret != AVM_ERR_NONE) 
	{ 
		return ret;
	}

    ret = avm_3D_handle[mode_3d_index].avm3d_shader_type->avmlib_Shader_Init(&avm_3D_handle[mode_3d_index], AVM_3D_VERT, AVM_3D_FRAG);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
//	DbgPrintf("%s %d\n", __FUNCTION__, __LINE__);	
	view_gl_parameter_init(mode_3d_index);	

	view_vbo_init(mode_3d_index, &view_vbo_parameter_buf, view_gl_parameter_3d);
	
	ret = avm_3D_handle[mode_3d_index].avm3d_texture_type->avmlib_Gen_TextrueBuffer(&avm_3D_handle[mode_3d_index], view_vbo_parameter_buf.pavm3d->vc_width, view_vbo_parameter_buf.pavm3d->vc_height * 4, GL_ALPHA, AVM_NULL);
	if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
//	DbgPrintf("%s %d\n", __FUNCTION__, __LINE__);	
	ret = avm_3D_handle[mode_3d_index].avm3d_object_type->avmlib_Gen_ObjBuffer(&avm_3D_handle[mode_3d_index], vboname[mode_3d_index], 4, &view_vbo_parameter_buf);
	if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
//	DbgPrintf("%s %d\n", __FUNCTION__, __LINE__);	


	blendParameter.blend_type = SRC_DST_ALPHA_3D;
	blendParameter.rgb_correct[0] = 0.6;
	blendParameter.rgb_correct[1] = 0.6;
	blendParameter.rgb_correct[2] = 0.6;
	blendParameter.ctl_blend[0] = 1.0;
	blendParameter.ctl_blend[1] = 0;
	blendParameter.ctl_blend[2] = 0;
//	DbgPrintf("%s %d\n", __FUNCTION__, __LINE__);	
	avm_3D_handle[mode_3d_index].avm3d_render_type->avmlib_Set_Blend(&avm_3D_handle[mode_3d_index], &blendParameter);
}
static AVM_ERR avm_3d_view_car_init(MODE3D mode_3d_index)
{
	int j;
	AVM_ERR ret;
	static int car_init_complete_flag = 0;
	C8 *vboname[] = {(C8*)VBO_3D_VIEW, (C8*)VBO_3D_VIEW1, (C8*)VBO_3D_VIEW2, (C8*)VBO_3D_VIEW3, (C8*)VBO_3D_VIEW4, 
						(C8*)VBO_3D_VIEW5, (C8*)VBO_3D_VIEW6, (C8*)VBO_3D_VIEW7, (C8*)VBO_3D_VIEW8, (C8*)VBO_3D_VIEW9,
						(C8*)VBO_3D_VIEW10, (C8*)VBO_3D_VIEW11, (C8*)VBO_3D_VIEW12, (C8*)VBO_3D_VIEW13, (C8*)VBO_3D_VIEW14, 
						(C8*)VBO_3D_VIEW15, (C8*)VBO_3D_VIEW16, (C8*)VBO_3D_VIEW17, (C8*)VBO_3D_VIEW18};
    C8 *pname[] = {(C8*)TIRE1_3D_OBJ, (C8*)TIRE2_3D_OBJ, (C8*)TIRE3_3D_OBJ, (C8*)TIRE4_3D_OBJ};
    blend_3d_Parameter_t blendParameter;
	F32 tire_model_alpha      = 1.0;//0.9;
	F32 veh_model_alpha       = 1.0;//0.95;
	F32 chassis_model_alpha   = 1.0;
	
	if(car_init_complete_flag == 1)
	{
		return AVM_ERR_NONE;
	}
	
	car_init_complete_flag =1;
	
	avm_3D_car_default_handle.pAttr  = &avm_3D_handle[mode_3d_index];
	avm_3D_car_default_handle.avm_type  = car3d_type;

 	avm_3D_chassis_default_handle.avm_type  = chassis_type;

	for(S32 j = 0; j < TIRE_COUNT; j++)
	{
		avm_3D_tire_default_handle[j].avm_type   = tire_type;
	}
 
    ret = avmlib_3d_Init(&avm_3D_car_default_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}
	
	ret = avmlib_3d_Init(&avm_3D_chassis_default_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	} 

     for(S32 j = 0; j < TIRE_COUNT; j++)
    {
        ret = avmlib_3d_Init(&avm_3D_tire_default_handle[j]);
        if(ret != AVM_ERR_NONE) 
		{
			return ret;
		}
    } 

/*
    ret = avmlib_3d_Child_Init(&avm_3D_chassis_default_handle, &avm_3D_car_default_handle);
    if(ret != AVM_ERR_NONE) 
	{
		return ret;
	} 

     for(S32 j = 0; j < TIRE_COUNT; j++)
    {
        ret = avmlib_3d_Child_Init(&avm_3D_tire_default_handle[j], &avm_3D_car_default_handle);
        if(ret != AVM_ERR_NONE) 
		{
			return ret;
		}
    } 
*/
	ret = avm_3D_car_default_handle.avm3d_shader_type->avmlib_Shader_Init(&avm_3D_car_default_handle, VEHICLESHADER_VERT, VEHICLESHADER_FRAG);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
    ret = avm_3D_car_default_handle.avm3d_object_type->avmlib_Gen_ObjBuffer(&avm_3D_car_default_handle, VEH_3D_OBJ, 1, AVM_NULL);
    if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
    for(S32 i = 0; i < TIRE_COUNT; i++)
    {
		ret = avm_3D_tire_default_handle[i].avm3d_shader_type->avmlib_Shader_Init(&avm_3D_tire_default_handle[i], VEHICLESHADER_VERT, VEHICLESHADER_FRAG);
        ret = avm_3D_tire_default_handle[i].avm3d_object_type->avmlib_Gen_ObjBuffer(&avm_3D_tire_default_handle[i], pname[i], 1, AVM_NULL);
        if(AVM_ERR_NONE != ret) 
		{
			return ret;
		}
    }
	ret = avm_3D_chassis_default_handle.avm3d_shader_type->avmlib_Shader_Init(&avm_3D_chassis_default_handle, VEHICLESHADER_VERT, VEHICLESHADER_FRAG);
    ret = avm_3D_chassis_default_handle.avm3d_object_type->avmlib_Gen_ObjBuffer(&avm_3D_chassis_default_handle, CHASSIS_3D_OBJ, 1, AVM_NULL);   
	if(AVM_ERR_NONE != ret) 
	{
		return ret;
	}
	
	
	blendParameter.ctl_blend[0] = veh_model_alpha;
	blendParameter.blend_type = SRC_ONE_MINUS_SRC_ALPHA_3D;
	
	ret = avm_3D_car_default_handle.avm3d_render_type->avmlib_Set_Blend(&avm_3D_car_default_handle, &blendParameter);
	if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}

	blendParameter.ctl_blend[0] = chassis_model_alpha;
	blendParameter.blend_type = SRC_ONE_MINUS_SRC_ALPHA_3D;

	ret = avm_3D_chassis_default_handle.avm3d_render_type->avmlib_Set_Blend(&avm_3D_chassis_default_handle, &blendParameter);
	if(ret != AVM_ERR_NONE) 
	{
		return ret;
	}

	for(S32 j = 0; j < TIRE_COUNT; j++)
	{
		blendParameter.ctl_blend[0] = tire_model_alpha;
		blendParameter.blend_type = SRC_ONE_MINUS_SRC_ALPHA_3D;

		ret = avm_3D_tire_default_handle[j].avm3d_render_type->avmlib_Set_Blend(&avm_3D_tire_default_handle[j], &blendParameter);
		if(ret != AVM_ERR_NONE) 
		{
			return ret;
		}
	}


}

static AVM_ERR avm_3d_calc_current_view_parameter(view_keep_t *view_keep,view_3d_parameter_t *view_gl_parameter_output,int * update_flag)
{
	static view_3d_parameter_t view_gl_parameter_last;
	static int view_calc_complete_flag = -1;
	static int fly_step_cnt = 0;
	static int fly_step_index = 0;
	static int fly_mode_cnt = 0;
	static int fly_mode_index = 0;
	static int fly_mode_dst = 0;
	static int clock_order; 
	static vector3f_t eye_dist;
	static vector3f_t offset_dist;
	static vector3f_t rotate_dist;
	static double fov_dist;
	int i;
	
	*update_flag = AVM_3D_VIEW_PARAM_UPDATE_ON;
	if(view_keep->fly_flag == AVM_FLY_ON_COMPLETE)
	{
		*update_flag = AVM_3D_VIEW_PARAM_UPDATE_OFF;
	}
	if(view_keep->fly_flag == AVM_FLY_OFF)
	{

		view_keep->fly_flag = AVM_FLY_ON_COMPLETE;
		/* 第一次进入3D模式，不需要飞翔 */
		memcpy(view_gl_parameter_output,&view_gl_parameter_3d[view_keep->sub_mode],sizeof(view_3d_parameter_t));
	}
	
	if(view_keep->fly_flag == AVM_FLY_ON)
	{
		/* 3D 模式之间需要飞翔 */
		view_keep->fly_flag = AVM_FLY_ON_GOING;
		fly_step_cnt = 0;
		memcpy(&view_gl_parameter_last,&view_gl_parameter_3d[view_keep->sub_mode_last],sizeof(view_3d_parameter_t));
		
		/* 判断顺时针还是逆时针旋转 */
		if(view_keep->sub_mode > view_keep->sub_mode_last)
		{
			if(view_keep->sub_mode - view_keep->sub_mode_last > AVM_FLY_CLOCK_THRESHOLD)
			{
				clock_order = AVM_FLY_CLOCKWISE;
			}
			else
			{
				clock_order = AVM_FLY_COUNTERCLOCKWISE;
			}
			
		}
		else
		{
			if(view_keep->sub_mode_last - view_keep->sub_mode > AVM_FLY_CLOCK_THRESHOLD)
			{
				clock_order = AVM_FLY_COUNTERCLOCKWISE;
			}
			else
			{
				clock_order = AVM_FLY_CLOCKWISE;
			}
		}
		fly_step_cnt =0; 
		fly_mode_dst = view_keep->sub_mode;
		if(clock_order == AVM_FLY_COUNTERCLOCKWISE)
		{
			fly_mode_index = view_keep->sub_mode_last + 1;
			if(fly_mode_index > AVM_FLY_MODE_POINT_CNT)
			{
				fly_mode_index = 1;
				
			}
		}
		else
		{
			fly_mode_index = view_keep->sub_mode_last - 1;
			if(fly_mode_index < 1)
			{
				fly_mode_index = AVM_FLY_MODE_POINT_CNT;
				
			}
		}

	}
	
	if(view_keep->fly_flag == AVM_FLY_ON_GOING)
	{

		if(fly_step_cnt == 0)
		{
			if ((view_gl_parameter_last.pitchhead.z > 0) && (view_gl_parameter_3d[fly_mode_index].pitchhead.z > 0))
			{ //use pitch and head
				offset_dist.x = (view_gl_parameter_3d[fly_mode_index].offset_x - view_gl_parameter_last.offset_x) / AVM_3D_FLY_STEP_COUNT;
				offset_dist.y = (view_gl_parameter_3d[fly_mode_index].offset_y - view_gl_parameter_last.offset_y) / AVM_3D_FLY_STEP_COUNT;
				offset_dist.z = (view_gl_parameter_3d[fly_mode_index].offset_z - view_gl_parameter_last.offset_z) / AVM_3D_FLY_STEP_COUNT;

				if (fabs(view_gl_parameter_3d[fly_mode_index].pitchhead.y - view_gl_parameter_last.pitchhead.y) > 180)
				{
					if (view_gl_parameter_3d[fly_mode_index].pitchhead.y > view_gl_parameter_last.pitchhead.y)
					{
						view_gl_parameter_last.pitchhead.y += 360;
					}
					else
					{
						view_gl_parameter_last.pitchhead.y -= 360;
					}
				}

				if (fabs(view_gl_parameter_3d[fly_mode_index].rotate_x - view_gl_parameter_last.rotate_x) > 180)
				{
					if (view_gl_parameter_3d[fly_mode_index].rotate_x > 0)
					{
						view_gl_parameter_last.rotate_x += 360;
					}
					else if (view_gl_parameter_3d[fly_mode_index].rotate_x < 0)
					{
						view_gl_parameter_last.rotate_x -= 360;
					}
				}
				if (fabs(view_gl_parameter_3d[fly_mode_index].rotate_y - view_gl_parameter_last.rotate_y) > 180)
				{
					if (view_gl_parameter_3d[fly_mode_index].rotate_y > 0)
					{
						view_gl_parameter_last.rotate_y += 360;
					}
					else if (view_gl_parameter_3d[fly_mode_index].rotate_y < 0)
					{
						view_gl_parameter_last.rotate_y -= 360;
					}
				}
				if (fabs(view_gl_parameter_3d[fly_mode_index].rotate_z - view_gl_parameter_last.rotate_z) > 180)
				{
					if (view_gl_parameter_3d[fly_mode_index].rotate_z > 0)
					{
						view_gl_parameter_last.rotate_z += 360;
					}
					else if (view_gl_parameter_3d[fly_mode_index].rotate_z < 0)
					{
						view_gl_parameter_last.rotate_z -= 360;
					}
				}
				eye_dist.x = (view_gl_parameter_3d[fly_mode_index].pitchhead.x - view_gl_parameter_last.pitchhead.x) / AVM_3D_FLY_STEP_COUNT;
				eye_dist.y = (view_gl_parameter_3d[fly_mode_index].pitchhead.y - view_gl_parameter_last.pitchhead.y) / AVM_3D_FLY_STEP_COUNT;
				eye_dist.z = (view_gl_parameter_3d[fly_mode_index].pitchhead.z - view_gl_parameter_last.pitchhead.z) / AVM_3D_FLY_STEP_COUNT;

				rotate_dist.x = (view_gl_parameter_3d[fly_mode_index].rotate_x - view_gl_parameter_last.rotate_x) / AVM_3D_FLY_STEP_COUNT;
				rotate_dist.y = (view_gl_parameter_3d[fly_mode_index].rotate_y - view_gl_parameter_last.rotate_y) / AVM_3D_FLY_STEP_COUNT;
				rotate_dist.z = (view_gl_parameter_3d[fly_mode_index].rotate_z - view_gl_parameter_last.rotate_z) / AVM_3D_FLY_STEP_COUNT;

				fov_dist = (view_gl_parameter_3d[fly_mode_index].fov - view_gl_parameter_last.fov) / AVM_3D_FLY_STEP_COUNT;
			}
			else
			{
				eye_dist.x = (view_gl_parameter_3d[fly_mode_index].eye.x - view_gl_parameter_last.eye.x) / AVM_3D_FLY_STEP_COUNT;
				eye_dist.y = (view_gl_parameter_3d[fly_mode_index].eye.y - view_gl_parameter_last.eye.y) / AVM_3D_FLY_STEP_COUNT;
				eye_dist.z = (view_gl_parameter_3d[fly_mode_index].eye.z - view_gl_parameter_last.eye.z) / AVM_3D_FLY_STEP_COUNT;

				offset_dist.x = (view_gl_parameter_3d[fly_mode_index].offset_x - view_gl_parameter_last.offset_x) / AVM_3D_FLY_STEP_COUNT;
				offset_dist.y = (view_gl_parameter_3d[fly_mode_index].offset_y - view_gl_parameter_last.offset_y) / AVM_3D_FLY_STEP_COUNT;
				offset_dist.z = (view_gl_parameter_3d[fly_mode_index].offset_z - view_gl_parameter_last.offset_z) / AVM_3D_FLY_STEP_COUNT;

				if (fabs(view_gl_parameter_3d[fly_mode_index].rotate_x - view_gl_parameter_last.rotate_x) > 180)
				{
					if (view_gl_parameter_3d[fly_mode_index].rotate_x > 0)
					{
						view_gl_parameter_last.rotate_x += 360;
					}
					else if (view_gl_parameter_3d[fly_mode_index].rotate_x < 0)
					{
						view_gl_parameter_last.rotate_x -= 360;
					}
				}
				if (fabs(view_gl_parameter_3d[fly_mode_index].rotate_y - view_gl_parameter_last.rotate_y) > 180)
				{
					if (view_gl_parameter_3d[fly_mode_index].rotate_y > 0)
					{
						view_gl_parameter_last.rotate_y += 360;
					}
					else if (view_gl_parameter_3d[fly_mode_index].rotate_y < 0)
					{
						view_gl_parameter_last.rotate_y -= 360;
					}
				}
				if (fabs(view_gl_parameter_3d[fly_mode_index].rotate_z - view_gl_parameter_last.rotate_z) > 180)
				{
					if (view_gl_parameter_3d[fly_mode_index].rotate_z > 0)
					{
						view_gl_parameter_last.rotate_z += 360;
					}
					else if (view_gl_parameter_3d[fly_mode_index].rotate_z < 0)
					{
						view_gl_parameter_last.rotate_z -= 360;
					}
				}
				rotate_dist.x = (view_gl_parameter_3d[fly_mode_index].rotate_x - view_gl_parameter_last.rotate_x) / AVM_3D_FLY_STEP_COUNT;
				rotate_dist.y = (view_gl_parameter_3d[fly_mode_index].rotate_y - view_gl_parameter_last.rotate_y) / AVM_3D_FLY_STEP_COUNT;
				rotate_dist.z = (view_gl_parameter_3d[fly_mode_index].rotate_z - view_gl_parameter_last.rotate_z) / AVM_3D_FLY_STEP_COUNT;

				fov_dist = (view_gl_parameter_3d[fly_mode_index].fov - view_gl_parameter_last.fov) / AVM_3D_FLY_STEP_COUNT;
			}
		}
		if ((view_gl_parameter_last.pitchhead.z > 0) && (view_gl_parameter_3d[fly_mode_index].pitchhead.z > 0))
		{ //use pitch and head
			if (fabs(view_gl_parameter_last.pitchhead.x - view_gl_parameter_3d[fly_mode_index].pitchhead.x) > fabs(eye_dist.x) || fabs(view_gl_parameter_last.pitchhead.y - view_gl_parameter_3d[fly_mode_index].pitchhead.y) > fabs(eye_dist.y) || fabs(view_gl_parameter_last.pitchhead.z - view_gl_parameter_3d[fly_mode_index].pitchhead.z) > fabs(eye_dist.z) || fabs(view_gl_parameter_last.offset_x - view_gl_parameter_3d[fly_mode_index].offset_x) > fabs(offset_dist.x) || fabs(view_gl_parameter_last.offset_y - view_gl_parameter_3d[fly_mode_index].offset_y) > fabs(offset_dist.y) || fabs(view_gl_parameter_last.offset_z - view_gl_parameter_3d[fly_mode_index].offset_z) > fabs(offset_dist.z) || fabs(view_gl_parameter_last.rotate_x - view_gl_parameter_3d[fly_mode_index].rotate_x) > fabs(rotate_dist.x) || fabs(view_gl_parameter_last.rotate_y - view_gl_parameter_3d[fly_mode_index].rotate_y) > fabs(rotate_dist.y) || fabs(view_gl_parameter_last.rotate_z - view_gl_parameter_3d[fly_mode_index].rotate_z) > fabs(rotate_dist.z) || fabs(view_gl_parameter_last.fov - view_gl_parameter_3d[fly_mode_index].fov) > fabs(fov_dist))
			{
				fly_step_cnt++;
				view_gl_parameter_last.pitchhead.x += (fabs(view_gl_parameter_last.pitchhead.x - view_gl_parameter_3d[fly_mode_index].pitchhead.x) > fabs(eye_dist.x) ? eye_dist.x : 0);
				view_gl_parameter_last.pitchhead.y += (fabs(view_gl_parameter_last.pitchhead.y - view_gl_parameter_3d[fly_mode_index].pitchhead.y) > fabs(eye_dist.y) ? eye_dist.y : 0);
				view_gl_parameter_last.pitchhead.z += (fabs(view_gl_parameter_last.pitchhead.z - view_gl_parameter_3d[fly_mode_index].pitchhead.z) > fabs(eye_dist.z) ? eye_dist.z : 0);

				view_gl_parameter_last.offset_x += (fabs(view_gl_parameter_last.offset_x - view_gl_parameter_3d[fly_mode_index].offset_x) > fabs(offset_dist.x) ? offset_dist.x : 0);
				view_gl_parameter_last.offset_y += (fabs(view_gl_parameter_last.offset_y - view_gl_parameter_3d[fly_mode_index].offset_y) > fabs(offset_dist.y) ? offset_dist.y : 0);
				view_gl_parameter_last.offset_z += (fabs(view_gl_parameter_last.offset_z - view_gl_parameter_3d[fly_mode_index].offset_z) > fabs(offset_dist.z) ? offset_dist.z : 0);

				view_gl_parameter_last.rotate_x += (fabs(view_gl_parameter_last.rotate_x - view_gl_parameter_3d[fly_mode_index].rotate_x) > fabs(rotate_dist.x) ? rotate_dist.x : 0);
				view_gl_parameter_last.rotate_y += (fabs(view_gl_parameter_last.rotate_y - view_gl_parameter_3d[fly_mode_index].rotate_y) > fabs(rotate_dist.y) ? rotate_dist.y : 0);
				view_gl_parameter_last.rotate_z += (fabs(view_gl_parameter_last.rotate_z - view_gl_parameter_3d[fly_mode_index].rotate_z) > fabs(rotate_dist.z) ? rotate_dist.z : 0);

				view_gl_parameter_last.fov += (fabs(view_gl_parameter_last.fov - view_gl_parameter_3d[fly_mode_index].fov) > fabs(fov_dist) ? fov_dist : 0);

				memcpy(view_gl_parameter_output, &view_gl_parameter_last, sizeof(view_3d_parameter_t));
			}
			else
			{
				memcpy(view_gl_parameter_output, &view_gl_parameter_3d[fly_mode_index], sizeof(view_3d_parameter_t));
				fly_step_cnt = 0;
				if (fly_mode_index == fly_mode_dst)
				{
					view_keep->fly_flag = AVM_FLY_ON_COMPLETE;
				}
				else
				{
					memcpy(&view_gl_parameter_last, &view_gl_parameter_3d[fly_mode_index], sizeof(view_3d_parameter_t));
					if (clock_order == AVM_FLY_COUNTERCLOCKWISE)
					{
						fly_mode_index++;
						if (fly_mode_index > AVM_FLY_MODE_POINT_CNT)
						{
							fly_mode_index = 1;
						}
					}
					else
					{
						fly_mode_index--;
						if (fly_mode_index < 1)
						{
							fly_mode_index = AVM_FLY_MODE_POINT_CNT;
						}
					}
				}
			}
		}
		else
		{
			if (fabs(view_gl_parameter_last.eye.x - view_gl_parameter_3d[fly_mode_index].eye.x) > fabs(eye_dist.x) || fabs(view_gl_parameter_last.eye.y - view_gl_parameter_3d[fly_mode_index].eye.y) > fabs(eye_dist.y) || fabs(view_gl_parameter_last.eye.z - view_gl_parameter_3d[fly_mode_index].eye.z) > fabs(eye_dist.z) || fabs(view_gl_parameter_last.offset_x - view_gl_parameter_3d[fly_mode_index].offset_x) > fabs(offset_dist.x) || fabs(view_gl_parameter_last.offset_y - view_gl_parameter_3d[fly_mode_index].offset_y) > fabs(offset_dist.y) || fabs(view_gl_parameter_last.offset_z - view_gl_parameter_3d[fly_mode_index].offset_z) > fabs(offset_dist.z) || fabs(view_gl_parameter_last.rotate_x - view_gl_parameter_3d[fly_mode_index].rotate_x) > fabs(rotate_dist.x) || fabs(view_gl_parameter_last.rotate_y - view_gl_parameter_3d[fly_mode_index].rotate_y) > fabs(rotate_dist.y) || fabs(view_gl_parameter_last.rotate_z - view_gl_parameter_3d[fly_mode_index].rotate_z) > fabs(rotate_dist.z) || fabs(view_gl_parameter_last.fov - view_gl_parameter_3d[fly_mode_index].fov) > fabs(fov_dist))
			{

				fly_step_cnt++;

				view_gl_parameter_last.eye.x += (fabs(view_gl_parameter_last.eye.x - view_gl_parameter_3d[fly_mode_index].eye.x) > fabs(eye_dist.x) ? eye_dist.x : 0);
				view_gl_parameter_last.eye.y += (fabs(view_gl_parameter_last.eye.y - view_gl_parameter_3d[fly_mode_index].eye.y) > fabs(eye_dist.y) ? eye_dist.y : 0);
				view_gl_parameter_last.eye.z += (fabs(view_gl_parameter_last.eye.z - view_gl_parameter_3d[fly_mode_index].eye.z) > fabs(eye_dist.z) ? eye_dist.z : 0);

				view_gl_parameter_last.offset_x += (fabs(view_gl_parameter_last.offset_x - view_gl_parameter_3d[fly_mode_index].offset_x) > fabs(offset_dist.x) ? offset_dist.x : 0);
				view_gl_parameter_last.offset_y += (fabs(view_gl_parameter_last.offset_y - view_gl_parameter_3d[fly_mode_index].offset_y) > fabs(offset_dist.y) ? offset_dist.y : 0);
				view_gl_parameter_last.offset_z += (fabs(view_gl_parameter_last.offset_z - view_gl_parameter_3d[fly_mode_index].offset_z) > fabs(offset_dist.z) ? offset_dist.z : 0);

				view_gl_parameter_last.rotate_x += (fabs(view_gl_parameter_last.rotate_x - view_gl_parameter_3d[fly_mode_index].rotate_x) > fabs(rotate_dist.x) ? rotate_dist.x : 0);
				view_gl_parameter_last.rotate_y += (fabs(view_gl_parameter_last.rotate_y - view_gl_parameter_3d[fly_mode_index].rotate_y) > fabs(rotate_dist.y) ? rotate_dist.y : 0);
				view_gl_parameter_last.rotate_z += (fabs(view_gl_parameter_last.rotate_z - view_gl_parameter_3d[fly_mode_index].rotate_z) > fabs(rotate_dist.z) ? rotate_dist.z : 0);

				view_gl_parameter_last.fov += (fabs(view_gl_parameter_last.fov - view_gl_parameter_3d[fly_mode_index].fov) > fabs(fov_dist) ? fov_dist : 0);

				memcpy(view_gl_parameter_output, &view_gl_parameter_last, sizeof(view_3d_parameter_t));
			}
			else
			{
				memcpy(view_gl_parameter_output, &view_gl_parameter_3d[fly_mode_index], sizeof(view_3d_parameter_t));
				fly_step_cnt = 0;
				if (fly_mode_index == fly_mode_dst)
				{
					view_keep->fly_flag = AVM_FLY_ON_COMPLETE;
				}
				else
				{
					memcpy(&view_gl_parameter_last, &view_gl_parameter_3d[fly_mode_index], sizeof(view_3d_parameter_t));
					if (clock_order == AVM_FLY_COUNTERCLOCKWISE)
					{
						fly_mode_index++;
						if (fly_mode_index > AVM_FLY_MODE_POINT_CNT)
						{
							fly_mode_index = 1;
						}
					}
					else
					{
						fly_mode_index--;
						if (fly_mode_index < 1)
						{
							fly_mode_index = AVM_FLY_MODE_POINT_CNT;
						}
					}
				}
			}
		}
	}
}

static AVM_ERR view_vbo_init(MODE3D mode, lut_parameter_t *view_vbo_parameter_buf, view_3d_parameter_t *view_gl_parameter_buf)
{
	view_vbo_parameter_buf->pavm  = AVM_NULL;
	view_vbo_parameter_buf->psgv  = AVM_NULL;
	view_vbo_parameter_buf->pavm3d  = &view_avm3d_model_param_buf;
	
	view_avm3d_model_param_buf.srcW = CAM_WIDTH;
    view_avm3d_model_param_buf.srcH = CAM_HEIGHT;
	
	view_avm3d_model_param_buf.vc_width			= view_gl_parameter_buf[mode].screen_width;
	view_avm3d_model_param_buf.vc_height		= view_gl_parameter_buf[mode].screen_height;
	
    view_avm3d_model_param_buf.tri_div_x	= 3;      
    view_avm3d_model_param_buf.tri_div_y	= 3;        

	view_avm3d_model_param_buf.OFRONTY = -5;
	view_avm3d_model_param_buf.OLEFTX = 0;
	view_avm3d_model_param_buf.ORIGHTX = 0;
	view_avm3d_model_param_buf.OBACKY = 10;


	switch(mode)
	{
		case MODE_3D_ROTATE:
			view_avm3d_model_param_buf.front_angle0 	= 75;
            view_avm3d_model_param_buf.front_angle1 	= 25;
            view_avm3d_model_param_buf.front_angle2 	= 75;
            view_avm3d_model_param_buf.front_angle3 	= 25;
            view_avm3d_model_param_buf.rear_angle0 		= 75;
            view_avm3d_model_param_buf.rear_angle1 		= 25;
            view_avm3d_model_param_buf.rear_angle2 		= 75;
            view_avm3d_model_param_buf.rear_angle3 		= 25;
            view_avm3d_model_param_buf.r_begin 			= 0.8;
            view_avm3d_model_param_buf.r_bottom 		= 1.5;
            view_avm3d_model_param_buf.r_torus 			= 1.0;
            view_avm3d_model_param_buf.center_x 		= 0;
            view_avm3d_model_param_buf.center_y 		= -0.5;
			//view_avm3d_model_param_buf.res_angle 		= 1;
            //view_avm3d_model_param_buf.res_r 			= 0.18;
            view_avm3d_model_param_buf.res_angle 		= 3;
            view_avm3d_model_param_buf.res_r 			= 0.3;
            view_avm3d_model_param_buf.alpha_wall 		= 0;
            view_avm3d_model_param_buf.limit_wall 		= 0;
            view_avm3d_model_param_buf.dx_dy_ellipse 	= 0.8;
			
            view_vbo_parameter_buf->lut_savepath = AVM_NULL;
			break;
		case MODE_3D_LEFTUP:
			view_avm3d_model_param_buf.front_angle0 	= 75;
            view_avm3d_model_param_buf.front_angle1 	= 25;
            view_avm3d_model_param_buf.front_angle2 	= 75;
            view_avm3d_model_param_buf.front_angle3 	= 25;
            view_avm3d_model_param_buf.rear_angle0 		= 75;
            view_avm3d_model_param_buf.rear_angle1 		= 25;
            view_avm3d_model_param_buf.rear_angle2 		= 75;
            view_avm3d_model_param_buf.rear_angle3 		= 25;
            view_avm3d_model_param_buf.r_begin 			= 0.5;
            view_avm3d_model_param_buf.r_bottom 		= 1.5;
            view_avm3d_model_param_buf.r_torus 			= 2.0;
            view_avm3d_model_param_buf.center_x 		= 0;
            view_avm3d_model_param_buf.center_y 		= -0.5;
            view_avm3d_model_param_buf.res_angle 		= 3;
            view_avm3d_model_param_buf.res_r 			= 0.3;
            view_avm3d_model_param_buf.alpha_wall 		= 0;
            view_avm3d_model_param_buf.limit_wall 		= 0;
            view_avm3d_model_param_buf.dx_dy_ellipse 	= 0.5;

			
            view_vbo_parameter_buf->lut_savepath = AVM_NULL;
			break;
		case MODE_3D_BACK:
			view_avm3d_model_param_buf.front_angle0 	= 20;
            view_avm3d_model_param_buf.front_angle1 	= 10;
            view_avm3d_model_param_buf.front_angle2 	= 20;
            view_avm3d_model_param_buf.front_angle3 	= 10;
            view_avm3d_model_param_buf.rear_angle0 		= 20;
            view_avm3d_model_param_buf.rear_angle1 		= 10;
            view_avm3d_model_param_buf.rear_angle2 		= 20;
            view_avm3d_model_param_buf.rear_angle3 		= 10;
            view_avm3d_model_param_buf.r_begin 			= 0.5;
            view_avm3d_model_param_buf.r_bottom 		= 1.5;
            view_avm3d_model_param_buf.r_torus 			= 2.0;
            view_avm3d_model_param_buf.center_x 		= 0;
            view_avm3d_model_param_buf.center_y 		= -0.5;
            view_avm3d_model_param_buf.res_angle 		= 3;
            view_avm3d_model_param_buf.res_r 			= 0.3;
            view_avm3d_model_param_buf.alpha_wall 		= 0;
            view_avm3d_model_param_buf.limit_wall 		= 0;
            view_avm3d_model_param_buf.dx_dy_ellipse 	= 0.5;

			
            view_vbo_parameter_buf->lut_savepath = AVM_NULL;
			break;
		case MODE_3D_RIGHTUP:
			view_avm3d_model_param_buf.front_angle0 	= 75;
            view_avm3d_model_param_buf.front_angle1 	= 35;
            view_avm3d_model_param_buf.front_angle2 	= 75;
            view_avm3d_model_param_buf.front_angle3 	= 35;
            view_avm3d_model_param_buf.rear_angle0 		= 75;
            view_avm3d_model_param_buf.rear_angle1 		= 25;
            view_avm3d_model_param_buf.rear_angle2 		= 75;
            view_avm3d_model_param_buf.rear_angle3 		= 25;
           view_avm3d_model_param_buf.r_begin 			= 0.5;
            view_avm3d_model_param_buf.r_bottom 		= 1.5;
            view_avm3d_model_param_buf.r_torus 			= 2.0;
            view_avm3d_model_param_buf.center_x 		= 0;
            view_avm3d_model_param_buf.center_y 		= -0.5;
            view_avm3d_model_param_buf.res_angle 		= 3;
            view_avm3d_model_param_buf.res_r 			= 0.3;
            view_avm3d_model_param_buf.alpha_wall 		= 0;
            view_avm3d_model_param_buf.limit_wall 		= 0;
            view_avm3d_model_param_buf.dx_dy_ellipse 	= 0.5;
			
            view_vbo_parameter_buf->lut_savepath = AVM_NULL;
			break;
		case MODE_3D_RIGHT:
			view_avm3d_model_param_buf.front_angle0 	= 75;
            view_avm3d_model_param_buf.front_angle1 	= 35;
            view_avm3d_model_param_buf.front_angle2 	= 75;
            view_avm3d_model_param_buf.front_angle3 	= 35;
            view_avm3d_model_param_buf.rear_angle0 		= 75;
            view_avm3d_model_param_buf.rear_angle1 		= 25;
            view_avm3d_model_param_buf.rear_angle2 		= 75;
            view_avm3d_model_param_buf.rear_angle3 		= 25;
           view_avm3d_model_param_buf.r_begin 			= 0.5;
            view_avm3d_model_param_buf.r_bottom 		= 1.5;
            view_avm3d_model_param_buf.r_torus 			= 2.0;
            view_avm3d_model_param_buf.center_x 		= 0;
            view_avm3d_model_param_buf.center_y 		= -0.5;
            view_avm3d_model_param_buf.res_angle 		= 3;
            view_avm3d_model_param_buf.res_r 			= 0.3;
            view_avm3d_model_param_buf.alpha_wall 		= 0;
            view_avm3d_model_param_buf.limit_wall 		= 0;
            view_avm3d_model_param_buf.dx_dy_ellipse 	= 0.5;
			
            view_vbo_parameter_buf->lut_savepath = AVM_NULL;
			break;
		case MODE_3D_FRONTRUP:
			view_avm3d_model_param_buf.front_angle0 	= 75;
            view_avm3d_model_param_buf.front_angle1 	= 25;
            view_avm3d_model_param_buf.front_angle2 	= 75;
            view_avm3d_model_param_buf.front_angle3 	= 25;
            view_avm3d_model_param_buf.rear_angle0 		= 75;
            view_avm3d_model_param_buf.rear_angle1 		= 25;
            view_avm3d_model_param_buf.rear_angle2 		= 75;
            view_avm3d_model_param_buf.rear_angle3 		= 25;
            view_avm3d_model_param_buf.r_begin 			= 0.3;
            view_avm3d_model_param_buf.r_bottom 		= 0.7;
            view_avm3d_model_param_buf.r_torus 			= 2.0;
            view_avm3d_model_param_buf.center_x 		= 0;
            view_avm3d_model_param_buf.center_y 		= -0.5;
            view_avm3d_model_param_buf.res_angle 		= 1;
            view_avm3d_model_param_buf.res_r 			= 0.3;
            view_avm3d_model_param_buf.alpha_wall 		= 0;
            view_avm3d_model_param_buf.limit_wall 		= 0;
            view_avm3d_model_param_buf.dx_dy_ellipse 	= 1;
			
            view_vbo_parameter_buf->lut_savepath = AVM_NULL;
			break;
		case MODE_3D_FRONT:
			view_avm3d_model_param_buf.front_angle0 	= 75;
            view_avm3d_model_param_buf.front_angle1 	= 25;
            view_avm3d_model_param_buf.front_angle2 	= 75;
            view_avm3d_model_param_buf.front_angle3 	= 25;
            view_avm3d_model_param_buf.rear_angle0 		= 75;
            view_avm3d_model_param_buf.rear_angle1 		= 25;
            view_avm3d_model_param_buf.rear_angle2 		= 75;
            view_avm3d_model_param_buf.rear_angle3 		= 25;
            view_avm3d_model_param_buf.r_begin 			= 0.5;
            view_avm3d_model_param_buf.r_bottom 		= 0.7;
            view_avm3d_model_param_buf.r_torus 			= 2.0;
            view_avm3d_model_param_buf.center_x 		= 0;
            view_avm3d_model_param_buf.center_y 		= -0.5;
            view_avm3d_model_param_buf.res_angle 		= 1;
            view_avm3d_model_param_buf.res_r 			= 0.3;
            view_avm3d_model_param_buf.alpha_wall 		= 0;
            view_avm3d_model_param_buf.limit_wall 		= 0;
            view_avm3d_model_param_buf.dx_dy_ellipse 	= 1;
			
            view_vbo_parameter_buf->lut_savepath = AVM_NULL;
			break;
		case MODE_3D_FRONTLUP:
			view_avm3d_model_param_buf.front_angle0 	= 75;
            view_avm3d_model_param_buf.front_angle1 	= 25;
            view_avm3d_model_param_buf.front_angle2 	= 75;
            view_avm3d_model_param_buf.front_angle3 	= 25;
            view_avm3d_model_param_buf.rear_angle0 		= 75;
            view_avm3d_model_param_buf.rear_angle1 		= 25;
            view_avm3d_model_param_buf.rear_angle2 		= 75;
            view_avm3d_model_param_buf.rear_angle3 		= 25;
            view_avm3d_model_param_buf.r_begin 			= 0.5;
            view_avm3d_model_param_buf.r_bottom 		= 0.7;
            view_avm3d_model_param_buf.r_torus 			= 2.0;
            view_avm3d_model_param_buf.center_x 		= 0;
            view_avm3d_model_param_buf.center_y 		= -0.5;
            view_avm3d_model_param_buf.res_angle 		= 1;
            view_avm3d_model_param_buf.res_r 			= 0.3;
            view_avm3d_model_param_buf.alpha_wall 		= 0;
            view_avm3d_model_param_buf.limit_wall 		= 0;
            view_avm3d_model_param_buf.dx_dy_ellipse 	= 1;
			
            view_vbo_parameter_buf->lut_savepath = AVM_NULL;
			break;
		case MODE_3D_LEFT:
			view_avm3d_model_param_buf.front_angle0 	= 75;
            view_avm3d_model_param_buf.front_angle1 	= 25;
            view_avm3d_model_param_buf.front_angle2 	= 75;
            view_avm3d_model_param_buf.front_angle3 	= 25;
            view_avm3d_model_param_buf.rear_angle0 		= 75;
            view_avm3d_model_param_buf.rear_angle1 		= 25;
            view_avm3d_model_param_buf.rear_angle2 		= 75;
            view_avm3d_model_param_buf.rear_angle3 		= 25;
            view_avm3d_model_param_buf.r_begin 			= 0.5;
            view_avm3d_model_param_buf.r_bottom 		= 0.7;
            view_avm3d_model_param_buf.r_torus 			= 2.0;
            view_avm3d_model_param_buf.center_x 		= 0;
            view_avm3d_model_param_buf.center_y 		= -0.5;
            view_avm3d_model_param_buf.res_angle 		= 1;
            view_avm3d_model_param_buf.res_r 			= 0.3;
            view_avm3d_model_param_buf.alpha_wall 		= 0;
            view_avm3d_model_param_buf.limit_wall 		= 0;
            view_avm3d_model_param_buf.dx_dy_ellipse 	= 1;
			
            view_vbo_parameter_buf->lut_savepath = AVM_NULL;
		break;
	}
}
static AVM_ERR view_gl_parameter_init(MODE3D mode)
{
	switch(mode)
	{
		case MODE_3D_ROTATE:
			view_gl_parameter_3d[MODE_3D_ROTATE].scale.x		= 1.0000 ;
			view_gl_parameter_3d[MODE_3D_ROTATE].scale.y        = 1.0000 ;
			view_gl_parameter_3d[MODE_3D_ROTATE].scale.z		= 1.0000  ;
			view_gl_parameter_3d[MODE_3D_ROTATE].eye.x          = EYE_X_3D;
			view_gl_parameter_3d[MODE_3D_ROTATE].eye.y          = EYE_Y_3D;
			view_gl_parameter_3d[MODE_3D_ROTATE].eye.z          = EYE_Z_3D;
		    view_gl_parameter_3d[MODE_3D_ROTATE].pitchhead.x = 30;
	    	view_gl_parameter_3d[MODE_3D_ROTATE].pitchhead.y = 0;
		    view_gl_parameter_3d[MODE_3D_ROTATE].pitchhead.z = 8000;
			view_gl_parameter_3d[MODE_3D_ROTATE].at.x          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_ROTATE].at.y          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_ROTATE].at.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_ROTATE].up.x          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_ROTATE].up.y          	= 1.0000;
			view_gl_parameter_3d[MODE_3D_ROTATE].up.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_ROTATE].offset_x       = 0;
			view_gl_parameter_3d[MODE_3D_ROTATE].offset_y       = 0;
			view_gl_parameter_3d[MODE_3D_ROTATE].offset_z       = 0.0000;
			view_gl_parameter_3d[MODE_3D_ROTATE].rotate_x 		= 0;
			view_gl_parameter_3d[MODE_3D_ROTATE].rotate_y       = 0;
			view_gl_parameter_3d[MODE_3D_ROTATE].rotate_z       = 0;
			view_gl_parameter_3d[MODE_3D_ROTATE].fov            = 48;
			view_gl_parameter_3d[MODE_3D_ROTATE].screen_x       = SCREEN_X_3D;
			view_gl_parameter_3d[MODE_3D_ROTATE].screen_y       = SCREEN_Y_3D;
			view_gl_parameter_3d[MODE_3D_ROTATE].screen_width   = SCREEN_WIDTH_3D;
			view_gl_parameter_3d[MODE_3D_ROTATE].screen_height  = SCREEN_HEIGHT_3D;
			view_gl_parameter_3d[MODE_3D_ROTATE].reserved       = 1.0000;
			break;
		case MODE_3D_LEFTUP:
			view_gl_parameter_3d[MODE_3D_LEFTUP].scale.x		= 1.0000 ;
			view_gl_parameter_3d[MODE_3D_LEFTUP].scale.y        = 1.0000 ;
			view_gl_parameter_3d[MODE_3D_LEFTUP].scale.z		= 1.0000  ;
			view_gl_parameter_3d[MODE_3D_LEFTUP].eye.x          = EYE_X_3D;
			view_gl_parameter_3d[MODE_3D_LEFTUP].eye.y          = EYE_Y_3D;
			view_gl_parameter_3d[MODE_3D_LEFTUP].eye.z          = EYE_Z_3D;
	    	view_gl_parameter_3d[MODE_3D_LEFTUP].pitchhead.x = 30;
		    view_gl_parameter_3d[MODE_3D_LEFTUP].pitchhead.y = 315;
		    view_gl_parameter_3d[MODE_3D_LEFTUP].pitchhead.z = 8000;
		    view_gl_parameter_3d[MODE_3D_LEFTUP].at.x = 0.0000;
			view_gl_parameter_3d[MODE_3D_LEFTUP].at.y          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_LEFTUP].at.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_LEFTUP].up.x          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_LEFTUP].up.y          	= 1.0000;
			view_gl_parameter_3d[MODE_3D_LEFTUP].up.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_LEFTUP].offset_x       = 0;
			view_gl_parameter_3d[MODE_3D_LEFTUP].offset_y       = 0;
			view_gl_parameter_3d[MODE_3D_LEFTUP].offset_z       = 0.0000;
		    view_gl_parameter_3d[MODE_3D_LEFTUP].rotate_x = 0;
		    view_gl_parameter_3d[MODE_3D_LEFTUP].rotate_y = 0;
		    view_gl_parameter_3d[MODE_3D_LEFTUP].rotate_z = 0;
			view_gl_parameter_3d[MODE_3D_LEFTUP].fov            = 48;
			view_gl_parameter_3d[MODE_3D_LEFTUP].screen_x       = SCREEN_X_3D;
			view_gl_parameter_3d[MODE_3D_LEFTUP].screen_y       = SCREEN_Y_3D;
			view_gl_parameter_3d[MODE_3D_LEFTUP].screen_width   = SCREEN_WIDTH_3D;
			view_gl_parameter_3d[MODE_3D_LEFTUP].screen_height  = SCREEN_HEIGHT_3D;
			view_gl_parameter_3d[MODE_3D_LEFTUP].reserved       = 1.0000;
			break;
		case MODE_3D_BACK:
			view_gl_parameter_3d[MODE_3D_BACK].scale.x		= 1.0000 ;
			view_gl_parameter_3d[MODE_3D_BACK].scale.y        = 1.0000 ;
			view_gl_parameter_3d[MODE_3D_BACK].scale.z		= 1.0000  ;
			view_gl_parameter_3d[MODE_3D_BACK].eye.x          = EYE_X_3D;
			view_gl_parameter_3d[MODE_3D_BACK].eye.y          = EYE_Y_3D;
			view_gl_parameter_3d[MODE_3D_BACK].eye.z          = EYE_Z_3D;
		    view_gl_parameter_3d[MODE_3D_BACK].pitchhead.x = 30;
	    	view_gl_parameter_3d[MODE_3D_BACK].pitchhead.y = 0;
		    view_gl_parameter_3d[MODE_3D_BACK].pitchhead.z = 8000;
			view_gl_parameter_3d[MODE_3D_BACK].at.x          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_BACK].at.y          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_BACK].at.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_BACK].up.x          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_BACK].up.y          	= 1.0000;
			view_gl_parameter_3d[MODE_3D_BACK].up.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_BACK].offset_x       = 0;
			view_gl_parameter_3d[MODE_3D_BACK].offset_y       = 0;
			view_gl_parameter_3d[MODE_3D_BACK].offset_z = 0;
			view_gl_parameter_3d[MODE_3D_BACK].rotate_x = 0;
			view_gl_parameter_3d[MODE_3D_BACK].rotate_y       = 0;
			view_gl_parameter_3d[MODE_3D_BACK].rotate_z       = 0;
			view_gl_parameter_3d[MODE_3D_BACK].fov            = 48;
			view_gl_parameter_3d[MODE_3D_BACK].screen_x       = SCREEN_X_3D;
			view_gl_parameter_3d[MODE_3D_BACK].screen_y       = SCREEN_Y_3D;
			view_gl_parameter_3d[MODE_3D_BACK].screen_width   = SCREEN_WIDTH_3D;
			view_gl_parameter_3d[MODE_3D_BACK].screen_height  = SCREEN_HEIGHT_3D;
			view_gl_parameter_3d[MODE_3D_BACK].reserved       = 1.0000;
			break;
		case MODE_3D_RIGHTUP:
			view_gl_parameter_3d[MODE_3D_RIGHTUP].scale.x		= 1.0000 ;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].scale.y        = 1.0000 ;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].scale.z		= 1.0000  ;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].eye.x          = EYE_X_3D;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].eye.y          = EYE_Y_3D;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].eye.z          = EYE_Z_3D;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].pitchhead.x = 30;
		    view_gl_parameter_3d[MODE_3D_RIGHTUP].pitchhead.y = 45;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].pitchhead.z = 8000;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].at.x          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].at.y          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].at.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].up.x          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].up.y          	= 1.0000;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].up.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].offset_x = 0;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].offset_y = 0;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].offset_z = 0.0000;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].rotate_x = 0;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].rotate_y = 0;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].rotate_z       = 0;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].fov            = 48;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].screen_x       = SCREEN_X_3D;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].screen_y       = SCREEN_Y_3D;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].screen_width   = SCREEN_WIDTH_3D;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].screen_height  = SCREEN_HEIGHT_3D;
			view_gl_parameter_3d[MODE_3D_RIGHTUP].reserved       = 1.0000;
			break;
		case MODE_3D_RIGHT:
			view_gl_parameter_3d[MODE_3D_RIGHT].scale.x		= 1.0000 ;
			view_gl_parameter_3d[MODE_3D_RIGHT].scale.y        = 1.0000 ;
			view_gl_parameter_3d[MODE_3D_RIGHT].scale.z		= 1.0000  ;
			view_gl_parameter_3d[MODE_3D_RIGHT].eye.x          = EYE_X_3D;
			view_gl_parameter_3d[MODE_3D_RIGHT].eye.y          = EYE_Y_3D;
			view_gl_parameter_3d[MODE_3D_RIGHT].eye.z          = EYE_Z_3D;
			view_gl_parameter_3d[MODE_3D_RIGHT].pitchhead.x = 30;
			view_gl_parameter_3d[MODE_3D_RIGHT].pitchhead.y = 90;
			view_gl_parameter_3d[MODE_3D_RIGHT].pitchhead.z = 8000;
			view_gl_parameter_3d[MODE_3D_RIGHT].at.x = 0.0000;
			view_gl_parameter_3d[MODE_3D_RIGHT].at.y = 0.0000;
			view_gl_parameter_3d[MODE_3D_RIGHT].at.z = 0.0000;
			view_gl_parameter_3d[MODE_3D_RIGHT].up.x = 0.0000;
			view_gl_parameter_3d[MODE_3D_RIGHT].up.y = 1.0000;
			view_gl_parameter_3d[MODE_3D_RIGHT].up.z = 0.0000;
			view_gl_parameter_3d[MODE_3D_RIGHT].offset_x = 0;
			view_gl_parameter_3d[MODE_3D_RIGHT].offset_y = 0;
			view_gl_parameter_3d[MODE_3D_RIGHT].offset_z = 0.0000;
			view_gl_parameter_3d[MODE_3D_RIGHT].rotate_x = 0; //-90
			view_gl_parameter_3d[MODE_3D_RIGHT].rotate_y = 0; //90
			view_gl_parameter_3d[MODE_3D_RIGHT].rotate_z = 0;
			view_gl_parameter_3d[MODE_3D_RIGHT].fov            = 48;
			view_gl_parameter_3d[MODE_3D_RIGHT].screen_x       = SCREEN_X_3D;
			view_gl_parameter_3d[MODE_3D_RIGHT].screen_y       = SCREEN_Y_3D;
			view_gl_parameter_3d[MODE_3D_RIGHT].screen_width   = SCREEN_WIDTH_3D;
			view_gl_parameter_3d[MODE_3D_RIGHT].screen_height  = SCREEN_HEIGHT_3D;
			view_gl_parameter_3d[MODE_3D_RIGHT].reserved       = 1.0000;
			break;
		case MODE_3D_FRONTRUP:
			view_gl_parameter_3d[MODE_3D_FRONTRUP].scale.x		= 1.0000 ;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].scale.y        = 1.0000 ;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].scale.z		= 1.0000  ;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].eye.x          = EYE_X_3D;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].eye.y          = EYE_Y_3D;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].eye.z          = EYE_Z_3D;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].pitchhead.x = 30;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].pitchhead.y = 135;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].pitchhead.z = 8000;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].at.x          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].at.y          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].at.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].up.x          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].up.y          	= 1.0000;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].up.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].offset_x       = 0;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].offset_y       = 0;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].offset_z       = 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].rotate_x = 0;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].rotate_y = 0;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].rotate_z       = 0;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].fov            = 48;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].screen_x       = SCREEN_X_3D;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].screen_y       = SCREEN_Y_3D;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].screen_width   = SCREEN_WIDTH_3D;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].screen_height  = SCREEN_HEIGHT_3D;
			view_gl_parameter_3d[MODE_3D_FRONTRUP].reserved       = 1.0000;
			break;
		case MODE_3D_FRONT:
			view_gl_parameter_3d[MODE_3D_FRONT].scale.x		= 1.0000 ;
			view_gl_parameter_3d[MODE_3D_FRONT].scale.y        = 1.0000 ;
			view_gl_parameter_3d[MODE_3D_FRONT].scale.z		= 1.0000  ;
			view_gl_parameter_3d[MODE_3D_FRONT].eye.x          = EYE_X_3D;
			view_gl_parameter_3d[MODE_3D_FRONT].eye.y          = EYE_Y_3D;
			view_gl_parameter_3d[MODE_3D_FRONT].eye.z          = EYE_Z_3D;
			view_gl_parameter_3d[MODE_3D_FRONT].pitchhead.x = 30;
			view_gl_parameter_3d[MODE_3D_FRONT].pitchhead.y = 180;
			view_gl_parameter_3d[MODE_3D_FRONT].pitchhead.z = 8000;
			view_gl_parameter_3d[MODE_3D_FRONT].at.x          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONT].at.y          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONT].at.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONT].up.x          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONT].up.y          	= 1.0000;
			view_gl_parameter_3d[MODE_3D_FRONT].up.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONT].offset_x       = 0;
			view_gl_parameter_3d[MODE_3D_FRONT].offset_y       = 0;
			view_gl_parameter_3d[MODE_3D_FRONT].offset_z       = 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONT].rotate_x = 0;
			view_gl_parameter_3d[MODE_3D_FRONT].rotate_y = 0;
			view_gl_parameter_3d[MODE_3D_FRONT].rotate_z       = 0;
			view_gl_parameter_3d[MODE_3D_FRONT].fov            = 48;
			view_gl_parameter_3d[MODE_3D_FRONT].screen_x       = SCREEN_X_3D;
			view_gl_parameter_3d[MODE_3D_FRONT].screen_y       = SCREEN_Y_3D;
			view_gl_parameter_3d[MODE_3D_FRONT].screen_width   = SCREEN_WIDTH_3D;
			view_gl_parameter_3d[MODE_3D_FRONT].screen_height  = SCREEN_HEIGHT_3D;
			view_gl_parameter_3d[MODE_3D_FRONT].reserved       = 1.0000;
			break;
		case MODE_3D_FRONTLUP:
			view_gl_parameter_3d[MODE_3D_FRONTLUP].scale.x		= 1.0000 ;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].scale.y        = 1.0000 ;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].scale.z		= 1.0000  ;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].eye.x          = EYE_X_3D;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].eye.y          = EYE_Y_3D;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].eye.z          = EYE_Z_3D;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].pitchhead.x = 30;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].pitchhead.y = 225;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].pitchhead.z = 8000;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].at.x          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].at.y          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].at.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].up.x          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].up.y          	= 1.0000;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].up.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].offset_x       = 0;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].offset_y       = -0;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].offset_z       = 0.0000;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].rotate_x = 0;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].rotate_y = 0;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].rotate_z       = 0;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].fov            = 48;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].screen_x       = SCREEN_X_3D;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].screen_y       = SCREEN_Y_3D;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].screen_width   = SCREEN_WIDTH_3D;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].screen_height  = SCREEN_HEIGHT_3D;
			view_gl_parameter_3d[MODE_3D_FRONTLUP].reserved       = 1.0000;
			break;
		case MODE_3D_LEFT:
			view_gl_parameter_3d[MODE_3D_LEFT].scale.x		= 1.0000 ;
			view_gl_parameter_3d[MODE_3D_LEFT].scale.y        = 1.0000 ;
			view_gl_parameter_3d[MODE_3D_LEFT].scale.z		= 1.0000  ;
			view_gl_parameter_3d[MODE_3D_LEFT].eye.x          = EYE_X_3D;
			view_gl_parameter_3d[MODE_3D_LEFT].eye.y          = EYE_Y_3D;
			view_gl_parameter_3d[MODE_3D_LEFT].eye.z          = EYE_Z_3D;
			view_gl_parameter_3d[MODE_3D_LEFT].pitchhead.x = 30;
			view_gl_parameter_3d[MODE_3D_LEFT].pitchhead.y = 270;
			view_gl_parameter_3d[MODE_3D_LEFT].pitchhead.z = 8000;
			view_gl_parameter_3d[MODE_3D_LEFT].at.x          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_LEFT].at.y          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_LEFT].at.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_LEFT].up.x          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_LEFT].up.y          	= 1.0000;
			view_gl_parameter_3d[MODE_3D_LEFT].up.z          	= 0.0000;
			view_gl_parameter_3d[MODE_3D_LEFT].offset_x       = 0;
			view_gl_parameter_3d[MODE_3D_LEFT].offset_y       = 0;
			view_gl_parameter_3d[MODE_3D_LEFT].offset_z       = 0.0000;
			view_gl_parameter_3d[MODE_3D_LEFT].rotate_x = 0;
			view_gl_parameter_3d[MODE_3D_LEFT].rotate_y = 0;
			view_gl_parameter_3d[MODE_3D_LEFT].rotate_z   = 0;
			view_gl_parameter_3d[MODE_3D_LEFT].fov            = 48;
			view_gl_parameter_3d[MODE_3D_LEFT].screen_x       = SCREEN_X_3D;
			view_gl_parameter_3d[MODE_3D_LEFT].screen_y       = SCREEN_Y_3D;
			view_gl_parameter_3d[MODE_3D_LEFT].screen_width   = SCREEN_WIDTH_3D;
			view_gl_parameter_3d[MODE_3D_LEFT].screen_height  = SCREEN_HEIGHT_3D;
			view_gl_parameter_3d[MODE_3D_LEFT].reserved       = 1.0000;
			break;
	}
}
