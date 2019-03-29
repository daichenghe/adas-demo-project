/*******************************************************************************
* File Name          : app_line.h
* Author             : Daich 
* Revision           : 2.1
* Date               : 16/05/2018
* Description        : draw_line interface
*******************************************************************************/

#ifndef __AVM_LINE_H__
#define __AVM_LINE_H__

#include <stdio.h>
#include "avmlib_sdk.h"
#include "avm_config.h"
#include "avmlib_gpu_drawline.h"
#include "avm_2d_module.h"
#include "avm_line_2d.h"
#include "avm_line_sv.h"
#include "avm_line_ori.h"
#include "avm_common.h"
#include "avm_ctr.h"
#define ANGLE_MAX_ABS (40) 			//角度范围
#define STRAIGHT_LINE (0.000001)	//直线
#define OFFSET_X 	  (20)			//偏移量(与车身两侧的距离)
#define OFFSET_Y 	  (0)			//偏移量(与车前后的距离)
#define LINE_WIDTH    (5)			//竖线线宽
#define LINE_WIDTH_H  (5)			//横线线宽
//1-开启；0-关闭
#define FRONT_DYNAMIC_LINE_END_CONNECTION (0) //前端相连
#define BACK_DYNAMIC_LINE_END_CONNECTION  (0)  //后端相连
#define MODE_SAME 		(1) //竖线与横线线段相同属性
#define MODE_DIFFERENT 	(0) //竖线与横线不同属性
//#define LINE_CONNECT	(1) //横线：直接画直线


/*
*************************************************************************
*************************************************************************
*                    LOCAL  DEFINE
*************************************************************************
*************************************************************************
*/
		  
//画线属性 
typedef struct draw_line_attr
{
  int r;			//颜色
  int g;
  int b;
  int width;		//动态轨迹线线宽
  int width_h;		//横线线宽
  U8  isstrip;		//0-虚线;1-实线
  int index_start;	
  int index_end;
}draw_line_attr_t;
//静态轨迹线 
typedef struct line_type_static
{
  point_t *point_left_buf;			//左侧轨迹线
  point_t *point_right_buf; 		//右侧轨迹线

  point_t *point_h1_buf;			//横线（2点）
  point_t *point_h2_buf;
  point_t *point_h3_buf;
  point_t *point_h4_buf;
  
  point_t *point_left_h1_buf;		//左侧横线
  point_t *point_left_h2_buf;
  point_t *point_left_h3_buf;
  point_t *point_left_h4_buf;
  point_t *point_right_h1_buf;		//右侧横线
  point_t *point_right_h2_buf;
  point_t *point_right_h3_buf;
  point_t *point_right_h4_buf;

  point_t point_left_target;			//左测试点
  point_t point_right_target;			//右测试点

  draw_line_attr_t line_v1_attr;	//轨迹线近处	(h和v都用这个)
  draw_line_attr_t line_v2_attr;	   
  draw_line_attr_t line_v3_attr;
  draw_line_attr_t line_v4_attr;

  int line_segment_length_v1_mm;		//竖线长
  int line_segment_length_v2_mm;	
  int line_segment_length_v3_mm;
  int line_segment_length_v4_mm;
  int line_segment_length_h1_mm;		 //横线长
  int line_segment_length_h2_mm;
  int line_segment_length_h3_mm;
  int line_segment_length_h4_mm;

  int line_point_step_mm;
  int line_point_h_step_mm;
  int line_point_num;

  int line_point_v1_num;				//竖线的点数
  int line_point_v2_num;
  int line_point_v3_num;
  int line_point_v4_num;
  int line_point_h1_num;				//横线的点数
  int line_point_h2_num;
  int line_point_h3_num;
  int line_point_h4_num;

}line_type_static_t;
//动态轨迹线
typedef struct line_type_dynamic
{
  point_t *point_left_buf[ANGLE_MAX_ABS*2+1];		//左侧轨迹线
  point_t *point_right_buf[ANGLE_MAX_ABS*2+1];		//右侧轨迹线
  point_t *point_front_buf[ANGLE_MAX_ABS*2+1];		//前轮轨迹线

  point_t point_left_target;   //左侧测试点
  point_t point_right_target;  //右侧测试点
  point_t point_front_left_target;	 //左侧前轮测试点
  point_t point_front_right_target;  //右侧前轮测试点
  
  draw_line_attr_t line_attr;//线属性  

  int line_segment_length_mm;
  int line_segment_length_front_mm;
  int line_point_step_mm;
  int line_point_num;
  int line_point_front_num;
}line_type_dynamic_t;
//左右辅助线（十字型）
typedef struct line_type_side
{
  point_t *point_left_near_buf;   //左侧辅助线
  point_t *point_left_far_buf;
  point_t *point_left_up_buf;
  point_t *point_left_down_buf;
  point_t *point_right_near_buf;  //右侧辅助线
  point_t *point_right_far_buf;
  point_t *point_right_up_buf;
  point_t *point_right_down_buf;
   

  point_t point_left_target;   //左侧测试点
  point_t point_right_target;  //右侧测试点

  draw_line_attr_t line_h_near_attr;	//十字型水平近处  
  draw_line_attr_t line_h_far_attr; 	 
  draw_line_attr_t line_v_up_attr;	   //十字型垂直近处 
  draw_line_attr_t line_v_down_attr;  

  int line_segment_length_near_mm;
  int line_segment_length_far_mm;
  int line_segment_length_up_mm;
  int line_segment_length_down_mm;
  int line_point_step_mm;
  int line_point_near_num;
  int line_point_far_num;
  int line_point_up_num;
  int line_point_down_num;
}line_type_side_t;

typedef struct line_type_front
{ 
  line_type_static_t  line_static;
  line_type_dynamic_t line_dynamic;

}line_type_front_t; 

typedef struct line_type_back
{ 
  line_type_static_t  line_static;
  line_type_dynamic_t line_dynamic;
 
}line_type_back_t;

typedef struct line_type_left
{ 
  line_type_side_t	line_side;

}line_type_left_t;

typedef struct line_type_right
{ 
  line_type_side_t	line_side;
	 
}line_type_right_t;

typedef struct line_type
{ 
  line_type_static_t  line_front_static;
  line_type_static_t  line_back_static;

  line_type_dynamic_t line_front_dynamic;
  line_type_dynamic_t line_back_dynamic;

  line_type_side_t	  line_left_side;
  line_type_side_t	  line_right_side;
}line_avm_t;

typedef struct vehicle_line
{
  //sv模式
  line_type_front_t line_sv_front;
  line_type_right_t line_sv_right;
  line_type_back_t	line_sv_back;
  line_type_left_t	line_sv_left;
  //2d模式
  line_avm_t  line_avm; 
  //ori模式
  //line_type_front_t line_sv_front;
  //line_type_right_t line_sv_right;
  line_type_back_t	line_ori_back;
  //line_type_left_t  line_sv_left;
}vehicle_line_type_t;

typedef enum
{
  TURN_LEFT,
  TURN_RIGHT,
  STRAIGHT
}vehicle_turn_t;

typedef struct vehicle_model_cfg
{
  int length_front_camera_rear_wheel;
  int length_front_wheel_rear_wheel;
  int length_between_rear_wheel;
  S32 vehicle_width;
  S32 vehicle_length;
  F64 angle_front_wheel;
  point_t point_target;
  vehicle_turn_t direction;
  camrea_chan_t cam_index;
  S32 length_line;
  S32 step_line;
}vehicle_model_cfg_t;

typedef enum
{
  UP,
  DOWN
}draw_line_direction;

extern vehicle_line_type_t vehicle_line_obj;
extern  vehicle_model_cfg_t vehicle_model_obj;
//vehicle_line_type_t vehicle_line_obj;
//vehicle_model_cfg_t vehicle_model_obj;
//static vehicle_line_type_t vehicle_line_obj;
//static vehicle_model_cfg_t vehicle_model_obj;



/*
*************************************************************************
*************************************************************************
*                      F U N C T I O N
*************************************************************************
*************************************************************************
*/

AVM_ERR default_line_init();
AVM_ERR common_line_init();
AVM_ERR line_handler(view_keep_t *view_keep);






#endif
