/*******************************************************************************
* File Name          : avm_line_sv.cpp
* Author             : chenx 
* Revision           : 2.1
* Date               : 16/05/2018
* Description        : draw_line_sv
*******************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "avm_line_sv.h"
#include "avm_line.h"

static AVM_ERR avm_line_sv_front_init();
static AVM_ERR avm_line_sv_front_cfg();
static AVM_ERR avm_line_sv_front_calc_coord();
static AVM_ERR avm_line_sv_front_static_calc_coord();
static AVM_ERR avm_line_sv_front_dynamic_calc_coord();
static AVM_ERR avm_line_sv_back_init();
static AVM_ERR avm_line_sv_back_cfg();
static AVM_ERR avm_line_sv_back_calc_coord();
static AVM_ERR avm_line_sv_back_static_calc_coord();
static AVM_ERR avm_line_sv_back_dynamic_calc_coord();
static AVM_ERR avm_line_sv_left_init();
static AVM_ERR avm_line_sv_left_cfg();
static AVM_ERR avm_line_sv_left_calc_coord();
static AVM_ERR avm_line_sv_right_init();
static AVM_ERR avm_line_sv_right_cfg();
static AVM_ERR avm_line_sv_right_calc_coord();
static AVM_ERR avm_line_sv_front_static_draw();
static AVM_ERR avm_line_sv_front_dynamic_draw(double angle);
static AVM_ERR avm_line_sv_back_static_draw();
static AVM_ERR avm_line_sv_back_dynamic_draw(double angle);
static AVM_ERR avm_line_sv_left_draw();
static AVM_ERR avm_line_sv_right_draw();
static AVM_ERR avm_line_sv_vehicle_model_init();

/*******************************************************************************************
* Function Name  : avm_line_sv_init
* Description    : 单视模式初始化
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/
AVM_ERR avm_line_sv_init()
{
	avm_line_sv_vehicle_model_init();
	avm_line_sv_front_init();
	avm_line_sv_back_init();
	avm_line_sv_left_init();
	avm_line_sv_right_init();
	return AVM_ERR_NONE;
}

/*******************************************************************************************
* Function Name  : avm_line_vehicle_model_init
* Description    : 车辆信息初始化
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/
static AVM_ERR avm_line_sv_vehicle_model_init()
{
   avm_vehicle_info_t pvehicle_info;
   avmlib_Get_Vehicle_Info(&pvehicle_info);
	 
   vehicle_model_obj.length_front_camera_rear_wheel  = 4000;
   vehicle_model_obj.length_front_wheel_rear_wheel	 = 3000;
   vehicle_model_obj.length_between_rear_wheel		 = 1800;
   vehicle_model_obj.vehicle_width					 = pvehicle_info.Veh_width;
   vehicle_model_obj.vehicle_length 				 = pvehicle_info.Veh_length;
   return AVM_ERR_NONE;
}
/*******************************************************************************************
* Function Name  : avm_line_sv_draw
* Description    : 单视模式画线
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

AVM_ERR avm_line_sv_draw(int sub_mode,double angle)
{ 
  switch(sub_mode)
  {
	  case MODE_2D_FRONT:
		  avm_line_sv_front_static_draw();
		  avm_line_sv_front_dynamic_draw(angle);
		  break;
	  case MODE_2D_RIGHT:
		  avm_line_sv_right_draw();
		  break;
	  case MODE_2D_BACK:
		  avm_line_sv_back_static_draw();
		  avm_line_sv_back_dynamic_draw(angle);
		  break;
	  case MODE_2D_LEFT:
		  avm_line_sv_left_draw();
		  break;
	  default :
		  break;

  }    
  return AVM_ERR_NONE;

}
/*******************************************************************************************
* Function Name : avm_sv_calc_x_from_vehicle_model
* Description   : 求轨迹圆的x坐标
* Arguments	    : y_input	 轨迹圆的y轴值
*				  x_output	 轨迹圆求得的x轴值
*				  cfg		 车辆属性
* Return 	    : AVM_ERR_NONE successful.
*				  Other			 Other AVM_ERR_xxx	 fail.
* Note(s)	    : none
*******************************************************************************************/
static AVM_ERR avm_sv_calc_x_from_vehicle_model(double *y_input, double *x_output, vehicle_model_cfg_t *cfg)
{
	 point_t point_center_left;
	 point_t point_center_right;
	 F64 rr_left = 0;
	 F64 rr_right = 0;
	 int length_fcamera_rwheel = cfg->length_front_camera_rear_wheel;
	 int length_fwheel_rwheel  = cfg->length_front_wheel_rear_wheel;
	 int length_rwheel_rear  = cfg->vehicle_length - length_fcamera_rwheel;
	 int w = cfg->length_between_rear_wheel;
	 F64 radian = fabs(cfg->angle_front_wheel * PI / 180);
	 F64 y	= *y_input;
	 F64 x	= 0;
	 F64 temp = 0;
	 F64 ly_max = 0;
	 F64 ry_max = 0;
	 F64 rr = 0;

	 point_center_left.x = -(1.0 * w / 2 + 1.0 * length_fwheel_rwheel * cos(radian) / sin(radian)) ;
	 point_center_left.y = -length_fcamera_rwheel;

	 point_center_right.x = 1.0 * w / 2 + 1.0 * length_fwheel_rwheel * cos(radian) / sin(radian);
	 point_center_right.y = -length_fcamera_rwheel;


	 rr_left = (cfg->point_target.x - point_center_left.x) * (cfg->point_target.x - point_center_left.x)
	 + (cfg->point_target.y - point_center_left.y) * (cfg->point_target.y - point_center_left.y);

	 rr_right = (cfg->point_target.x - point_center_right.x) * (cfg->point_target.x - point_center_right.x)
	 + (cfg->point_target.y - point_center_right.y) * (cfg->point_target.y - point_center_right.y);
	 
	 if(cfg->cam_index == CAM_FRONT)
	{
		  if (rr_left < rr_right)//测试点在左侧
		  {
		  
			  ly_max = sqrt(rr_left / rr_right) * (vehicle_model_obj.length_line - point_center_left.y) + point_center_left.y;
			  ly_max = vehicle_model_obj.point_target.y + ly_max; 
			  
		  }
		  if (rr_left > rr_right)
		  {
			  ry_max = sqrt(rr_right / rr_left) * (vehicle_model_obj.length_line + length_fcamera_rwheel) - length_fcamera_rwheel;
			  ry_max = vehicle_model_obj.point_target.y + ry_max; 
			  
		  }
	}
	else if(cfg->cam_index == CAM_BACK)
	{
		  if (rr_left < rr_right)
		  {
			  ly_max = sqrt(rr_left / rr_right) * (vehicle_model_obj.length_line + length_rwheel_rear) - (length_rwheel_rear);
			  ly_max = vehicle_model_obj.point_target.y - ly_max; 
		  }
		  if (rr_left > rr_right)
		  {
			  ry_max = sqrt(rr_right / rr_left) * (vehicle_model_obj.length_line + length_rwheel_rear) - (length_rwheel_rear);
			  ry_max = vehicle_model_obj.point_target.y - ry_max; 
		  }
	}

	 switch(cfg->direction)
	 {
		 case TURN_LEFT:
			 point_center_left.x = -(1.0 * w / 2 + 1.0 * length_fwheel_rwheel * cos(radian) / sin(radian)) ;
			 point_center_left.y = -length_fcamera_rwheel;

			 rr = (cfg->point_target.x - point_center_left.x) * (cfg->point_target.x - point_center_left.x)
			 + (cfg->point_target.y - point_center_left.y) * (cfg->point_target.y - point_center_left.y);
#if 1
			 if(cfg->cam_index == CAM_FRONT)
			 {
				 if (rr < rr_right)
				 {
					 if (*y_input > ly_max)
					 {
						 *y_input = ly_max;
					 }
				 }
			 }
			 else if(cfg->cam_index == CAM_BACK)
			 {	 
				 if (rr < rr_right)
				 {
					 if (*y_input < ly_max)
					 {
						 *y_input = ly_max;
					 }
				 }		 
			 }

#endif
			 temp = rr - (*y_input - point_center_left.y) * (*y_input - point_center_left.y); 

			 if(temp < 0)
			 {
				 *y_input = point_center_left.y + int(sqrt(rr)/cfg->step_line) * cfg->step_line;	
				 return AVM_ERR_NONE;				 
			 }
			 x = point_center_left.x + sqrt(temp);
			 break;  
		 case TURN_RIGHT:
			 point_center_right.x = 1.0 * w / 2 + 1.0 * length_fwheel_rwheel * cos(radian) / sin(radian);
			 point_center_right.y = -length_fcamera_rwheel;

			 rr = (cfg->point_target.x - point_center_right.x) * (cfg->point_target.x - point_center_right.x)
			 + (cfg->point_target.y - point_center_right.y) * (cfg->point_target.y - point_center_right.y);  

#if 1			 
			 if(cfg->cam_index == CAM_FRONT)
			 {	 
				 if (rr < rr_left)
				 {
					 if (*y_input > ry_max)
					 {
						 *y_input = ry_max;
					 }
				 }
			 }
			 else if(cfg->cam_index == CAM_BACK)
			 {		 
				 if (rr < rr_left)	 
				 {
					 if (*y_input < ry_max)
					 {
						 *y_input = ry_max;
					 }
				 }
			 }
#endif			 
			 temp = rr - (*y_input - point_center_right.y) * (*y_input - point_center_right.y); 
			 if(temp < 0)
			 {
				 *y_input = point_center_right.y + int(sqrt(rr)/cfg->step_line) * cfg->step_line; 
				 return AVM_ERR_NONE;	 
			 }
			 x = point_center_right.x - sqrt(temp); 
			 break;
		 case STRAIGHT:
			 x = cfg->point_target.x + STRAIGHT_LINE;
			 break;
		 default :
			 return AVM_ERR_FALSE;
		 break;  
	 }
	 *x_output = x;  
	 return AVM_ERR_NONE;
}
/*******************************************************************************************
* Function Name : sv_cal_coord
* Description	: 从车辆坐标转换成虚拟相机图像坐标
* Arguments 	: points_buffer 	 所要画的线段上的点数据指针
*				  point_cnt 		 所要画的线段上的点数
*                 virtual_camera_sgv_param_p    单视参数
*				  camera_id 		 相机id
* Return		: AVM_ERR_NONE successful.
*				  Other 		 Other AVM_ERR_xxx	 fail.
* Note(s)		: none
*******************************************************************************************/


static AVM_ERR sv_cal_coord(point_t *point_buffer,int point_cnt,virtual_camera_sgv_t *virtual_camera_sgv_param_p,int camera_id)
{
	 F64 coord_v[3]  = {0};
	 S32 coord_vc[2] = {0};
	 F64 TrackingImagePointTemp[2] = {0};
	 F64 TrackingImagePointTemp_new[2] = {0};
	 
	 coord_v[0] = point_buffer[point_cnt].x;
	 coord_v[1] = point_buffer[point_cnt].y;
	 coord_v[2] = 0;
	 avmlib_getVCImageCoordFromVehicleCoord(&virtual_camera_sgv_param_p->puVC[camera_id],coord_v,coord_vc);
	 if(camera_id == CAM_FRONT)
	 {
		 point_buffer[point_cnt].x = coord_vc[0];
		 point_buffer[point_cnt].y = coord_vc[1];
	 }
	 else if(camera_id == CAM_BACK)
	 {
	 
		 point_buffer[point_cnt].x = virtual_camera_sgv_param_p->vc_width[0] - coord_vc[0];
		 point_buffer[point_cnt].y = coord_vc[1];
	 }

	return AVM_ERR_NONE;
}


/*******************************************************************************************
* Function Name  : avm_line_sv_front_init
* Description    : 单视前轮轨迹线坐标初始化
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_front_init()
{
  avm_line_sv_front_cfg();
  avm_line_sv_front_calc_coord();
  return AVM_ERR_NONE;
}
/*******************************************************************************************
* Function Name  : avm_line_sv_front_cfg
* Description    : 单视前轮轨迹线配置参数
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_front_cfg()
{
  int len_v1;
  int len_v2;
  int len_v3;
  int len_v4;
  int len_h1;
  int len_h2;
  int len_h3;
  int len_h4;
  int len_step;
  int len_v_step;
  int len_h_step;
  int line_width;
  int line_width_h;
  U8  line_isstrip;
	  
  len_v1 = 500;
  len_v2 = 500;
  len_v3 = 1000;
  len_v4 = 1000;
  len_h1 = 400;//vehicle_model_obj.vehicle_width / 2 + 100;//vehicle_model_obj.vehicle_width + OFFSET*2;
  len_h2 = 400;//vehicle_model_obj.vehicle_width / 2 + 100;//vehicle_model_obj.vehicle_width + OFFSET*2;
  len_h3 = 400;//vehicle_model_obj.vehicle_width / 2 + 100;//vehicle_model_obj.vehicle_width + OFFSET*2;
  len_h4 = 400;//vehicle_model_obj.vehicle_width / 2 + 100;
  len_step = 100;
  len_v_step = 100;
  len_h_step = 100;
  line_width = LINE_WIDTH;
  line_width_h = LINE_WIDTH_H;
  line_isstrip = 1;
  
  if(len_v1 < len_step)
  {
	  return AVM_ERR_NONE;
  }
  if(len_v2< len_step)
  {
	  return AVM_ERR_NONE;
  }
  if(len_v3 < len_step)
  {
	  return AVM_ERR_NONE;
  }
  if(len_v4 < len_step)
  {
	  return AVM_ERR_NONE;
  }
  
  //static point target
  vehicle_line_obj.line_sv_front.line_static.point_left_target.x  = -vehicle_model_obj.vehicle_width / 2 - OFFSET_X;
  vehicle_line_obj.line_sv_front.line_static.point_left_target.y  = 0 - OFFSET_Y;
  vehicle_line_obj.line_sv_front.line_static.point_right_target.x = vehicle_model_obj.vehicle_width / 2 + OFFSET_X;
  vehicle_line_obj.line_sv_front.line_static.point_right_target.y = 0 - OFFSET_Y;
  
  //static line length
  vehicle_line_obj.line_sv_front.line_static.line_segment_length_v1_mm	= len_v1;
  vehicle_line_obj.line_sv_front.line_static.line_segment_length_v2_mm	= len_v2;
  vehicle_line_obj.line_sv_front.line_static.line_segment_length_v3_mm	= len_v3;
  vehicle_line_obj.line_sv_front.line_static.line_segment_length_v4_mm	= len_v4;
  
  vehicle_line_obj.line_sv_front.line_static.line_segment_length_h1_mm	= len_h1;
  vehicle_line_obj.line_sv_front.line_static.line_segment_length_h2_mm	= len_h2;
  vehicle_line_obj.line_sv_front.line_static.line_segment_length_h3_mm	= len_h3;
  vehicle_line_obj.line_sv_front.line_static.line_segment_length_h4_mm	= len_h4;
  
  vehicle_line_obj.line_sv_front.line_static.line_point_step_mm = len_v_step;
  vehicle_line_obj.line_sv_front.line_static.line_point_h_step_mm = len_h_step;
  
  //static point num
  vehicle_line_obj.line_sv_front.line_static.line_point_num = 1.0 * (len_v1 + len_v2 + len_v3 + len_v4) / len_v_step + 1;
  vehicle_line_obj.line_sv_front.line_static.line_point_h1_num = len_h1 / len_h_step + 1; 
  vehicle_line_obj.line_sv_front.line_static.line_point_h2_num = len_h2 / len_h_step + 1;
  vehicle_line_obj.line_sv_front.line_static.line_point_h3_num = len_h3 / len_h_step + 1;
  vehicle_line_obj.line_sv_front.line_static.line_point_h4_num = len_h4 / len_h_step + 1;
#if 0
  if(vehicle_line_obj.line_sv_front.line_static.line_point_h1_num % 2 != 0)
  {
	  vehicle_line_obj.line_sv_front.line_static.line_point_h1_num += 2;
  }
  if(vehicle_line_obj.line_sv_front.line_static.line_point_h2_num % 2 != 0)
  {
	  vehicle_line_obj.line_sv_front.line_static.line_point_h2_num += 2;
  }
  if(vehicle_line_obj.line_sv_front.line_static.line_point_h3_num % 2 != 0)
  {
	  vehicle_line_obj.line_sv_front.line_static.line_point_h3_num += 2;
  }
  if(vehicle_line_obj.line_sv_front.line_static.line_point_h4_num % 2 != 0)
  {
	  vehicle_line_obj.line_sv_front.line_static.line_point_h4_num += 2;
  }
#endif

  //static left line attribute
  vehicle_line_obj.line_sv_front.line_static.line_v1_attr.r			= 255;
  vehicle_line_obj.line_sv_front.line_static.line_v1_attr.g			= 0;
  vehicle_line_obj.line_sv_front.line_static.line_v1_attr.b			= 0;		
  vehicle_line_obj.line_sv_front.line_static.line_v1_attr.width		= line_width;
  vehicle_line_obj.line_sv_front.line_static.line_v1_attr.width_h		= line_width_h;
  vehicle_line_obj.line_sv_front.line_static.line_v1_attr.isstrip		= line_isstrip;
  vehicle_line_obj.line_sv_front.line_static.line_v1_attr.index_start	= 0;
  vehicle_line_obj.line_sv_front.line_static.line_v1_attr.index_end	= 1.0 * len_v1 / len_v_step;
  
  vehicle_line_obj.line_sv_front.line_static.line_v2_attr.r			= 255;
  vehicle_line_obj.line_sv_front.line_static.line_v2_attr.g			= 255;
  vehicle_line_obj.line_sv_front.line_static.line_v2_attr.b			= 0;  
  vehicle_line_obj.line_sv_front.line_static.line_v2_attr.width		= line_width;
  vehicle_line_obj.line_sv_front.line_static.line_v2_attr.width_h		= line_width_h;
  vehicle_line_obj.line_sv_front.line_static.line_v2_attr.isstrip		= line_isstrip;
  vehicle_line_obj.line_sv_front.line_static.line_v2_attr.index_start	= vehicle_line_obj.line_sv_front.line_static.line_v1_attr.index_end;
  vehicle_line_obj.line_sv_front.line_static.line_v2_attr.index_end	= 1.0 * (len_v1 + len_v2) / len_v_step;
  
  vehicle_line_obj.line_sv_front.line_static.line_v3_attr.r			= 0;
  vehicle_line_obj.line_sv_front.line_static.line_v3_attr.g			= 0;
  vehicle_line_obj.line_sv_front.line_static.line_v3_attr.b			= 255;		
  vehicle_line_obj.line_sv_front.line_static.line_v3_attr.width		= line_width;
  vehicle_line_obj.line_sv_front.line_static.line_v3_attr.width_h		= line_width_h;
  vehicle_line_obj.line_sv_front.line_static.line_v3_attr.isstrip		= line_isstrip;
  vehicle_line_obj.line_sv_front.line_static.line_v3_attr.index_start	= vehicle_line_obj.line_sv_front.line_static.line_v2_attr.index_end;
  vehicle_line_obj.line_sv_front.line_static.line_v3_attr.index_end	= 1.0 * (len_v1 + len_v2 + len_v3) / len_v_step;
  
  vehicle_line_obj.line_sv_front.line_static.line_v4_attr.r			= 0;
  vehicle_line_obj.line_sv_front.line_static.line_v4_attr.g			= 255;
  vehicle_line_obj.line_sv_front.line_static.line_v4_attr.b			= 0;	
  vehicle_line_obj.line_sv_front.line_static.line_v4_attr.width		= line_width;
  vehicle_line_obj.line_sv_front.line_static.line_v4_attr.width_h		= line_width_h;
  vehicle_line_obj.line_sv_front.line_static.line_v4_attr.isstrip		= line_isstrip;
  vehicle_line_obj.line_sv_front.line_static.line_v4_attr.index_start	= vehicle_line_obj.line_sv_front.line_static.line_v3_attr.index_end;
  vehicle_line_obj.line_sv_front.line_static.line_v4_attr.index_end	= vehicle_line_obj.line_sv_front.line_static.line_point_num - 1;
	  
  //dynamic point target
  vehicle_line_obj.line_sv_front.line_dynamic.point_left_target.x  = -vehicle_model_obj.vehicle_width / 2 - OFFSET_X;
  vehicle_line_obj.line_sv_front.line_dynamic.point_left_target.y  = 0 - OFFSET_Y;
  vehicle_line_obj.line_sv_front.line_dynamic.point_right_target.x = vehicle_model_obj.vehicle_width / 2 + OFFSET_X;
  vehicle_line_obj.line_sv_front.line_dynamic.point_right_target.y = 0 - OFFSET_Y;
  
  //dynamic line length
  vehicle_line_obj.line_sv_front.line_dynamic.line_segment_length_mm = len_v1 + len_v2 + len_v3 + len_v4;
  vehicle_line_obj.line_sv_front.line_dynamic.line_point_step_mm  = len_step;
  
  //dynamic point num
  vehicle_line_obj.line_sv_front.line_dynamic.line_point_num = 1.0 * (len_v1 + len_v2 + len_v3 + len_v4) / len_step + 1;
  
  //dynamic line attribute
  vehicle_line_obj.line_sv_front.line_dynamic.line_attr.r			= 255;
  vehicle_line_obj.line_sv_front.line_dynamic.line_attr.g			= 215;
  vehicle_line_obj.line_sv_front.line_dynamic.line_attr.b			= 0;	
  vehicle_line_obj.line_sv_front.line_dynamic.line_attr.width		= line_width;
  vehicle_line_obj.line_sv_front.line_dynamic.line_attr.isstrip 	= line_isstrip;
  vehicle_line_obj.line_sv_front.line_dynamic.line_attr.index_start = 0;
  vehicle_line_obj.line_sv_front.line_dynamic.line_attr.index_end	= vehicle_line_obj.line_sv_front.line_dynamic.line_point_num - 1;
  return AVM_ERR_NONE;

}

/*******************************************************************************************
* Function Name  : avm_line_sv_front_calc_coord
* Description    : 单视前轮轨迹线坐标
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_front_calc_coord()
{
  avm_line_sv_front_static_calc_coord();
  avm_line_sv_front_dynamic_calc_coord();
  return AVM_ERR_NONE;
}
/*******************************************************************************************
* Function Name  : avm_line_sv_front_static_calc_coord
* Description    : 单视前轮静态轨迹线
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/
static AVM_ERR avm_line_sv_front_static_calc_coord()
{


	int point_cnt = 0;
	int point_num = 0;
	int point_step = 0;
	int point_h_step = 0;
	int position_h1 = 0;				
	int position_h2 = 0;
	int position_h3 = 0;
	int position_h4 = 0;
	int point_h1_num = 0;		
	int point_h2_num = 0;
	int point_h3_num = 0;
	int point_h4_num = 0;
	int position_h1_index = 0;
	int position_h2_index = 0;
	int position_h3_index = 0;
	int position_h4_index = 0;
	
	F64 y_input = 0;
	F64 coord_v[3]	= {0};
	S32 coord_vc[2] = {0};
	 
	point_t *point_left_buf;		
	point_t *point_right_buf;
	point_t *point_left_h1_buf; 	
	point_t *point_right_h1_buf;
	point_t *point_left_h2_buf;
	point_t *point_right_h2_buf;
	point_t *point_left_h3_buf;
	point_t *point_right_h3_buf;
	point_t *point_left_h4_buf;
	point_t *point_right_h4_buf;
  
	static virtual_camera_avm_t virtual_camera_avm_paraml = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static virtual_camera_sgv_t virtual_camera_sgv_paraml;
	static view_2d_parameter_t view_2d_parameter_param[4];

	if(virtual_camera_avm_paraml.scale_pixel_mm == 0)
	{
	  avm_disp_info_get(MODE_SV, &virtual_camera_avm_paraml,&virtual_camera_sgv_paraml, view_2d_parameter_param);
	}

	point_num  = vehicle_line_obj.line_sv_front.line_static.line_point_num;
	point_step = vehicle_line_obj.line_sv_front.line_static.line_point_step_mm;
	point_h_step = vehicle_line_obj.line_sv_front.line_static.line_point_h_step_mm;
		
	position_h1 = vehicle_line_obj.line_sv_front.line_static.line_segment_length_v1_mm;
	position_h2 = position_h1 + vehicle_line_obj.line_sv_front.line_static.line_segment_length_v2_mm;
	position_h3 = position_h2 + vehicle_line_obj.line_sv_front.line_static.line_segment_length_v3_mm;
	position_h4 = position_h3 + vehicle_line_obj.line_sv_front.line_static.line_segment_length_v4_mm;

	

	position_h1_index = position_h1 /point_step;
	position_h2_index = position_h2 /point_step;
	position_h3_index = position_h3 /point_step;
	position_h4_index = position_h4 /point_step;
	
	point_h1_num = vehicle_line_obj.line_sv_front.line_static.line_point_h1_num; 
	point_h2_num = vehicle_line_obj.line_sv_front.line_static.line_point_h2_num;
	point_h3_num = vehicle_line_obj.line_sv_front.line_static.line_point_h3_num;
	point_h4_num = vehicle_line_obj.line_sv_front.line_static.line_point_h4_num;
		
	point_left_buf	= (point_t *)malloc(point_num * sizeof(point_t));
	point_right_buf = (point_t *)malloc(point_num * sizeof(point_t));
 	point_left_h1_buf  = (point_t *)malloc(point_h1_num * sizeof(point_t));
	point_right_h1_buf = (point_t *)malloc(point_h1_num * sizeof(point_t));
	point_left_h2_buf  = (point_t *)malloc(point_h2_num * sizeof(point_t));
	point_right_h2_buf = (point_t *)malloc(point_h2_num * sizeof(point_t));
	point_left_h3_buf  = (point_t *)malloc(point_h3_num * sizeof(point_t));
	point_right_h3_buf = (point_t *)malloc(point_h3_num * sizeof(point_t));
	point_left_h4_buf  = (point_t *)malloc(point_h4_num * sizeof(point_t));
	point_right_h4_buf = (point_t *)malloc(point_h4_num * sizeof(point_t)); 

	vehicle_line_obj.line_sv_front.line_static.point_left_buf	   = point_left_buf;
	vehicle_line_obj.line_sv_front.line_static.point_right_buf	   = point_right_buf;
 	vehicle_line_obj.line_sv_front.line_static.point_left_h1_buf   = point_left_h1_buf;
	vehicle_line_obj.line_sv_front.line_static.point_right_h1_buf  = point_right_h1_buf;
	vehicle_line_obj.line_sv_front.line_static.point_left_h2_buf   = point_left_h2_buf;
	vehicle_line_obj.line_sv_front.line_static.point_right_h2_buf  = point_right_h2_buf;
	vehicle_line_obj.line_sv_front.line_static.point_left_h3_buf   = point_left_h3_buf;
	vehicle_line_obj.line_sv_front.line_static.point_right_h3_buf  = point_right_h3_buf;
	vehicle_line_obj.line_sv_front.line_static.point_left_h4_buf   = point_left_h4_buf;
	vehicle_line_obj.line_sv_front.line_static.point_right_h4_buf  = point_right_h4_buf;

	point_t *point_h1_buf;
	point_t *point_h2_buf;
	point_t *point_h3_buf;
	point_t *point_h4_buf;

	point_h1_buf  = (point_t *)malloc(2 * sizeof(point_t));
	point_h2_buf  = (point_t *)malloc(2 * sizeof(point_t));
	point_h3_buf  = (point_t *)malloc(2 * sizeof(point_t));
	point_h4_buf  = (point_t *)malloc(2 * sizeof(point_t));
	
	vehicle_line_obj.line_sv_front.line_static.point_h1_buf   = point_h1_buf;
	vehicle_line_obj.line_sv_front.line_static.point_h2_buf   = point_h2_buf;
	vehicle_line_obj.line_sv_front.line_static.point_h3_buf   = point_h3_buf;
	vehicle_line_obj.line_sv_front.line_static.point_h4_buf   = point_h4_buf;


  //v
  for(point_cnt = 0; point_cnt < point_num; point_cnt++)
  {
	  y_input = point_cnt * point_step;
	  
	  //left
	  point_left_buf[point_cnt].x = vehicle_line_obj.line_sv_front.line_static.point_left_target.x ;   
	  point_left_buf[point_cnt].y = vehicle_line_obj.line_sv_front.line_static.point_left_target.y + y_input;
	  sv_cal_coord(point_left_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_FRONT);

//	  DbgPrintf("point_left_buf(%f,%f)\n",point_left_buf[point_cnt].x,point_left_buf[point_cnt].y);
	  
	  //right
  	  point_right_buf[point_cnt].x = vehicle_line_obj.line_sv_front.line_static.point_right_target.x;
  	  point_right_buf[point_cnt].y = vehicle_line_obj.line_sv_front.line_static.point_right_target.y + y_input;
	  sv_cal_coord(point_right_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_FRONT);
  } 
#if 0
   //直接相连
   //h1
   point_h1_buf[0].x = point_left_buf[position_h1_index].x;
   point_h1_buf[0].y = point_left_buf[position_h1_index].y;
   point_h1_buf[1].x = point_right_buf[position_h1_index].x;
   point_h1_buf[1].y = point_right_buf[position_h1_index].y;
   //h2
   point_h2_buf[0].x = point_left_buf[position_h2_index].x;
   point_h2_buf[0].y = point_left_buf[position_h2_index].y;
   point_h2_buf[1].x = point_right_buf[position_h2_index].x;
   point_h2_buf[1].y = point_right_buf[position_h2_index].y;
   //h3
   point_h3_buf[0].x = point_left_buf[position_h3_index].x;
   point_h3_buf[0].y = point_left_buf[position_h3_index].y;
   point_h3_buf[1].x = point_right_buf[position_h3_index].x;
   point_h3_buf[1].y = point_right_buf[position_h3_index].y;

   //h4
   point_h4_buf[0].x = point_left_buf[position_h4_index].x;
   point_h4_buf[0].y = point_left_buf[position_h4_index].y;
   point_h4_buf[1].x = point_right_buf[position_h4_index].x;
   point_h4_buf[1].y = point_right_buf[position_h4_index].y;
   
	 
#endif   
#if 1
   //h1
  for(point_cnt = 0; point_cnt < point_h1_num; point_cnt++)
  {
	  y_input = point_cnt * point_h_step;
	  //left
	  point_left_h1_buf[point_cnt].x = vehicle_line_obj.line_sv_front.line_static.point_left_target.x + y_input;	
	  point_left_h1_buf[point_cnt].y = vehicle_line_obj.line_sv_front.line_static.point_left_target.y + position_h1;
	  sv_cal_coord(point_left_h1_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_FRONT);
	  point_left_h1_buf[point_cnt].y = point_left_h1_buf[0].y;
	  //DbgPrintf("point_left_h1_buf(%f,%f)\n",point_left_h1_buf[point_cnt].x,point_left_h1_buf[point_cnt].y);
	  //right
	  point_right_h1_buf[point_cnt].x = vehicle_line_obj.line_sv_front.line_static.point_right_target.x - y_input;	
	  point_right_h1_buf[point_cnt].y = vehicle_line_obj.line_sv_front.line_static.point_right_target.y + position_h1;
	  sv_cal_coord(point_right_h1_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_FRONT);
	  point_right_h1_buf[point_cnt].y = point_right_h1_buf[0].y;
	}
  //h2
  for(point_cnt = 0; point_cnt < point_h2_num; point_cnt++)
  {
	  y_input = point_cnt * point_h_step;
	  //left
	  point_left_h2_buf[point_cnt].x = vehicle_line_obj.line_sv_front.line_static.point_left_target.x + y_input;	
	  point_left_h2_buf[point_cnt].y = vehicle_line_obj.line_sv_front.line_static.point_left_target.y + position_h2;
	  sv_cal_coord(point_left_h2_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_FRONT);
	  point_left_h2_buf[point_cnt].y = point_left_h2_buf[0].y;
      //right
	  point_right_h2_buf[point_cnt].x = vehicle_line_obj.line_sv_front.line_static.point_right_target.x - y_input;		
	  point_right_h2_buf[point_cnt].y = vehicle_line_obj.line_sv_front.line_static.point_right_target.y + position_h2;	
	  sv_cal_coord(point_right_h2_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_FRONT);
	  point_right_h2_buf[point_cnt].y = point_right_h2_buf[0].y;
   }
  //h3
  for(point_cnt = 0; point_cnt < point_h3_num; point_cnt++)
  {
	  y_input = point_cnt * point_h_step;
	  //left
	  point_left_h3_buf[point_cnt].x = vehicle_line_obj.line_sv_front.line_static.point_left_target.x + y_input;	
	  point_left_h3_buf[point_cnt].y = vehicle_line_obj.line_sv_front.line_static.point_left_target.y + position_h3;
	  sv_cal_coord(point_left_h3_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_FRONT);
	  point_left_h3_buf[point_cnt].y = point_left_h3_buf[0].y;

	  //right
	  point_right_h3_buf[point_cnt].x = vehicle_line_obj.line_sv_front.line_static.point_right_target.x - y_input;		
	  point_right_h3_buf[point_cnt].y = vehicle_line_obj.line_sv_front.line_static.point_right_target.y + position_h3;
	  sv_cal_coord(point_right_h3_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_FRONT);
	  point_right_h3_buf[point_cnt].y = point_right_h3_buf[0].y;
  } 
  //h4
  for(point_cnt = 0; point_cnt < point_h4_num; point_cnt++)
  {
	  y_input = point_cnt * point_h_step;
	  //left
	  point_left_h4_buf[point_cnt].x = vehicle_line_obj.line_sv_front.line_static.point_left_target.x + y_input;	
	  point_left_h4_buf[point_cnt].y = vehicle_line_obj.line_sv_front.line_static.point_left_target.y + position_h4;
	  sv_cal_coord(point_left_h4_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_FRONT);
	  point_left_h4_buf[point_cnt].x = point_left_h4_buf[point_cnt].x - 1;
	  point_left_h4_buf[point_cnt].y = point_left_h4_buf[0].y;
	  //right
	  point_right_h4_buf[point_cnt].x = vehicle_line_obj.line_sv_front.line_static.point_right_target.x - y_input;		
	  point_right_h4_buf[point_cnt].y = vehicle_line_obj.line_sv_front.line_static.point_right_target.y + position_h4;
	  sv_cal_coord(point_right_h4_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_FRONT);
	  point_right_h4_buf[point_cnt].x = point_right_h4_buf[point_cnt].x + 1;
	  point_right_h4_buf[point_cnt].y = point_right_h4_buf[0].y;
  } 	

#endif
	return AVM_ERR_NONE;

}
/*******************************************************************************************
* Function Name  : avm_line_sv_front_dynamic_calc_coord
* Description    : 单视前轮动态轨迹线坐标
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_front_dynamic_calc_coord()
{
  int point_cnt = 0;
  int point_num = 0;
  int point_step = 0;
  int angle_cnt = 0;
  int angle_input = 0;
  point_t *point_left_buf;
  point_t *point_right_buf;
  point_t *point_left_angle0_buf;
  point_t *point_right_angle0_buf;
  F64 coord_v[3]  = {0};
  S32 coord_vc[2] = {0};
  F64 x_output = 0;
  F64 y_input = 0;

  static virtual_camera_avm_t virtual_camera_avm_paraml = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  static virtual_camera_sgv_t virtual_camera_sgv_paraml;
  static view_2d_parameter_t view_2d_parameter_param[4];

  if(virtual_camera_avm_paraml.scale_pixel_mm == 0)
  {
	  avm_disp_info_get(MODE_SV, &virtual_camera_avm_paraml,&virtual_camera_sgv_paraml, view_2d_parameter_param);
  }

  point_num = vehicle_line_obj.line_sv_front.line_dynamic.line_point_num;
  point_step = vehicle_line_obj.line_sv_front.line_dynamic.line_point_step_mm;

  for(angle_cnt = 0; angle_cnt < ANGLE_MAX_ABS * 2 + 1; angle_cnt++)
  {
	  point_left_buf  = (point_t *)malloc(point_num * sizeof(point_t));
	  point_right_buf = (point_t *)malloc(point_num * sizeof(point_t));
	  vehicle_line_obj.line_sv_front.line_dynamic.point_left_buf[angle_cnt]  = point_left_buf;
	  vehicle_line_obj.line_sv_front.line_dynamic.point_right_buf[angle_cnt] = point_right_buf;   
  }

  point_left_angle0_buf = (point_t *)malloc(point_num * sizeof(point_t));
  point_right_angle0_buf = (point_t *)malloc(point_num * sizeof(point_t));

  for(point_cnt = 0; point_cnt < point_num; point_cnt++)
  {
	  point_left_angle0_buf[point_cnt].x = vehicle_line_obj.line_sv_front.line_dynamic.point_left_target.x;   
	  point_left_angle0_buf[point_cnt].y = point_cnt * point_step;

	  point_right_angle0_buf[point_cnt].x = vehicle_line_obj.line_sv_front.line_dynamic.point_right_target.x;  
	  point_right_angle0_buf[point_cnt].y = point_cnt * point_step;
  }   

  for(angle_cnt = 0; angle_cnt < ANGLE_MAX_ABS * 2 + 1; angle_cnt++)
  {
	  point_left_buf = vehicle_line_obj.line_sv_front.line_dynamic.point_left_buf[angle_cnt];
	  point_right_buf = vehicle_line_obj.line_sv_front.line_dynamic.point_right_buf[angle_cnt];   

	  angle_input = angle_cnt - ANGLE_MAX_ABS;
//DbgPrintf("angle_input = %d\n",angle_input);
	  if(angle_input < 0)
	  {
		  vehicle_model_obj.direction = TURN_LEFT;
	  }
	  else if (angle_input > 0)
	  {
		  vehicle_model_obj.direction = TURN_RIGHT;
	  }
	  else
	  {
		  vehicle_model_obj.direction = STRAIGHT;

	  }
	  vehicle_model_obj.angle_front_wheel = angle_input;
	  vehicle_model_obj.cam_index = CAM_FRONT;
	  vehicle_model_obj.length_line = vehicle_line_obj.line_sv_front.line_dynamic.line_segment_length_mm;
	  vehicle_model_obj.step_line = point_step;
#if 1	  
	  for(point_cnt = 0; point_cnt < point_num; point_cnt++)
	  {
		  //left
		  vehicle_model_obj.point_target.x = vehicle_line_obj.line_sv_front.line_dynamic.point_left_target.x;
		  vehicle_model_obj.point_target.y = vehicle_line_obj.line_sv_front.line_dynamic.point_left_target.y;

		  y_input = vehicle_model_obj.point_target.y + point_left_angle0_buf[point_cnt].y;
		  
//angle_input == -40 ? DbgPrintf("y_input(%f)\n",y_input) : 0;
		  avm_sv_calc_x_from_vehicle_model(&y_input, &x_output, &vehicle_model_obj);
		  
//angle_input == -38 ? DbgPrintf("point_left_buf(%f,%f)\n",x_output,y_input) : 0;
		  point_left_buf[point_cnt].x = x_output;
		  point_left_buf[point_cnt].y = y_input;
		  sv_cal_coord(point_left_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_FRONT);
	  }
#endif	  
	  for(point_cnt = 0; point_cnt < point_num; point_cnt++)
	  {
		//right
		 vehicle_model_obj.point_target.x = vehicle_line_obj.line_sv_front.line_dynamic.point_right_target.x;
		 vehicle_model_obj.point_target.y = vehicle_line_obj.line_sv_front.line_dynamic.point_right_target.y;

		 y_input = vehicle_model_obj.point_target.y + point_left_angle0_buf[point_cnt].y;
		 avm_sv_calc_x_from_vehicle_model(&y_input, &x_output, &vehicle_model_obj);
//angle_input == -38 ? DbgPrintf("point_right_buf(%f,%f)\n",x_output,y_input) : 0;
		 point_right_buf[point_cnt].x = x_output;
		 point_right_buf[point_cnt].y = y_input;
		 sv_cal_coord(point_right_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_FRONT);
	  }

  }

  free(point_left_angle0_buf);
  free(point_right_angle0_buf);
  
  return AVM_ERR_NONE;
}
/*******************************************************************************************
* Function Name  : avm_line_sv_back_init
* Description    : 单视模式倒车轨迹线初始化
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_back_init()
{
  avm_line_sv_back_cfg();
  avm_line_sv_back_calc_coord();
  
  return AVM_ERR_NONE;

}
/*******************************************************************************************
* Function Name  : avm_line_sv_back_cfg
* Description    : 单视模式倒车轨迹线配置参数
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_back_cfg()
{
  int len_v1;
  int len_v2;
  int len_v3;
  int len_v4;
  int len_h1;
  int len_h2;
  int len_h3;
  int len_h4;
  int len_step;
  int len_v_step;
  int len_h_step;
  int line_width;
  int line_width_h;
  U8  line_isstrip;
	  
  len_v1 = 500;
  len_v2 = 500;
  len_v3 = 1000;
  len_v4 = 1000;
  len_h1 = 400;//vehicle_model_obj.vehicle_width / 2 + 100;//vehicle_model_obj.vehicle_width + OFFSET*2;
  len_h2 = 400;//vehicle_model_obj.vehicle_width / 2 + 100;//vehicle_model_obj.vehicle_width + OFFSET*2;
  len_h3 = 400;//vehicle_model_obj.vehicle_width / 2 + 100;//vehicle_model_obj.vehicle_width + OFFSET*2;
  len_h4 = 400;//vehicle_model_obj.vehicle_width + OFFSET*2;
  len_step = 100;
  len_v_step = 100;
  len_h_step = 100;
  line_width = LINE_WIDTH;
  line_width_h = LINE_WIDTH_H;
  line_isstrip = 1;
  
  if(len_v1 < len_step)
  {
	  return AVM_ERR_NONE;
  }
  if(len_v2< len_step)
  {
	  return AVM_ERR_NONE;
  }
  if(len_v3 < len_step)
  {
	  return AVM_ERR_NONE;
  }
  if(len_v4 < len_step)
  {
	  return AVM_ERR_NONE;
  }
  
  //static point target
  vehicle_line_obj.line_sv_back.line_static.point_left_target.x  = -vehicle_model_obj.vehicle_width / 2 - OFFSET_X;
  vehicle_line_obj.line_sv_back.line_static.point_left_target.y  = -vehicle_model_obj.vehicle_length - OFFSET_Y; 
  vehicle_line_obj.line_sv_back.line_static.point_right_target.x = vehicle_model_obj.vehicle_width / 2 + OFFSET_X;
  vehicle_line_obj.line_sv_back.line_static.point_right_target.y = -vehicle_model_obj.vehicle_length - OFFSET_Y;
  
  //static line length
  vehicle_line_obj.line_sv_back.line_static.line_segment_length_v1_mm	= len_v1;
  vehicle_line_obj.line_sv_back.line_static.line_segment_length_v2_mm	= len_v2;
  vehicle_line_obj.line_sv_back.line_static.line_segment_length_v3_mm	= len_v3;
  vehicle_line_obj.line_sv_back.line_static.line_segment_length_v4_mm	= len_v4;
  
  vehicle_line_obj.line_sv_back.line_static.line_segment_length_h1_mm	= len_h1;
  vehicle_line_obj.line_sv_back.line_static.line_segment_length_h2_mm	= len_h2;
  vehicle_line_obj.line_sv_back.line_static.line_segment_length_h3_mm	= len_h3;
  vehicle_line_obj.line_sv_back.line_static.line_segment_length_h4_mm	= len_h4;
  
  vehicle_line_obj.line_sv_back.line_static.line_point_step_mm = len_v_step;
  vehicle_line_obj.line_sv_back.line_static.line_point_h_step_mm = len_h_step;
  
  //static point num
  vehicle_line_obj.line_sv_back.line_static.line_point_num = 1.0 * (len_v1 + len_v2 + len_v3 + len_v4) / len_v_step + 1;
  vehicle_line_obj.line_sv_back.line_static.line_point_h1_num = len_h1 / len_h_step + 1; 
  vehicle_line_obj.line_sv_back.line_static.line_point_h2_num = len_h2 / len_h_step + 1;
  vehicle_line_obj.line_sv_back.line_static.line_point_h3_num = len_h3 / len_h_step + 1;
  vehicle_line_obj.line_sv_back.line_static.line_point_h4_num = len_h4 / len_h_step + 1;
#if 0
  if(vehicle_line_obj.line_sv_back.line_static.line_point_h1_num % 2 != 0)
  {
	  vehicle_line_obj.line_sv_back.line_static.line_point_h1_num += 2;
  }
  if(vehicle_line_obj.line_sv_back.line_static.line_point_h2_num % 2 != 0)
  {
	  vehicle_line_obj.line_sv_back.line_static.line_point_h2_num += 2;
  }
  if(vehicle_line_obj.line_sv_back.line_static.line_point_h3_num % 2 != 0)
  {
	  vehicle_line_obj.line_sv_back.line_static.line_point_h3_num += 2;
  }
  if(vehicle_line_obj.line_sv_back.line_static.line_point_h4_num % 2 != 0)
  {
	  vehicle_line_obj.line_sv_back.line_static.line_point_h4_num += 2;
  }
#endif
  //static left line attribute
  vehicle_line_obj.line_sv_back.line_static.line_v1_attr.r 			= 255;
  vehicle_line_obj.line_sv_back.line_static.line_v1_attr.g 			= 0;
  vehicle_line_obj.line_sv_back.line_static.line_v1_attr.b 			= 0;		
  vehicle_line_obj.line_sv_back.line_static.line_v1_attr.width 		= line_width;
  vehicle_line_obj.line_sv_back.line_static.line_v1_attr.width_h		= line_width_h;
  vehicle_line_obj.line_sv_back.line_static.line_v1_attr.isstrip		= line_isstrip;
  vehicle_line_obj.line_sv_back.line_static.line_v1_attr.index_start	= 0;
  vehicle_line_obj.line_sv_back.line_static.line_v1_attr.index_end 	= 1.0 * len_v1 / len_v_step + 0.5;
  
  vehicle_line_obj.line_sv_back.line_static.line_v2_attr.r 			= 255;
  vehicle_line_obj.line_sv_back.line_static.line_v2_attr.g 			= 255;
  vehicle_line_obj.line_sv_back.line_static.line_v2_attr.b 			= 0;  
  vehicle_line_obj.line_sv_back.line_static.line_v2_attr.width 		= line_width;
  vehicle_line_obj.line_sv_back.line_static.line_v2_attr.width_h		= line_width_h;
  vehicle_line_obj.line_sv_back.line_static.line_v2_attr.isstrip		= line_isstrip;
  vehicle_line_obj.line_sv_back.line_static.line_v2_attr.index_start	= vehicle_line_obj.line_sv_back.line_static.line_v1_attr.index_end;
  vehicle_line_obj.line_sv_back.line_static.line_v2_attr.index_end 	= 1.0 * (len_v1 + len_v2) / len_v_step + 0.5;
  
  vehicle_line_obj.line_sv_back.line_static.line_v3_attr.r 			= 0;
  vehicle_line_obj.line_sv_back.line_static.line_v3_attr.g 			= 0;
  vehicle_line_obj.line_sv_back.line_static.line_v3_attr.b 			= 255;		
  vehicle_line_obj.line_sv_back.line_static.line_v3_attr.width 		= line_width;
  vehicle_line_obj.line_sv_back.line_static.line_v3_attr.width_h		= line_width_h;
  vehicle_line_obj.line_sv_back.line_static.line_v3_attr.isstrip		= line_isstrip;
  vehicle_line_obj.line_sv_back.line_static.line_v3_attr.index_start	= vehicle_line_obj.line_sv_back.line_static.line_v2_attr.index_end;
  vehicle_line_obj.line_sv_back.line_static.line_v3_attr.index_end 	= 1.0 * (len_v1 + len_v2 + len_v3) / len_v_step + 0.5;
  
  vehicle_line_obj.line_sv_back.line_static.line_v4_attr.r 			= 0;
  vehicle_line_obj.line_sv_back.line_static.line_v4_attr.g 			= 255;
  vehicle_line_obj.line_sv_back.line_static.line_v4_attr.b 			= 0;	
  vehicle_line_obj.line_sv_back.line_static.line_v4_attr.width 		= line_width;
  vehicle_line_obj.line_sv_back.line_static.line_v4_attr.width_h		= line_width_h;
  vehicle_line_obj.line_sv_back.line_static.line_v4_attr.isstrip		= line_isstrip;
  vehicle_line_obj.line_sv_back.line_static.line_v4_attr.index_start	= vehicle_line_obj.line_sv_back.line_static.line_v3_attr.index_end;
  vehicle_line_obj.line_sv_back.line_static.line_v4_attr.index_end 	= vehicle_line_obj.line_sv_back.line_static.line_point_num - 1;
  
  
  
  
  //dynamic point target
  vehicle_line_obj.line_sv_back.line_dynamic.point_left_target.x  = -vehicle_model_obj.vehicle_width / 2 - OFFSET_X;
  vehicle_line_obj.line_sv_back.line_dynamic.point_left_target.y  = -vehicle_model_obj.vehicle_length - OFFSET_Y;
  vehicle_line_obj.line_sv_back.line_dynamic.point_right_target.x = vehicle_model_obj.vehicle_width / 2 + OFFSET_X;
  vehicle_line_obj.line_sv_back.line_dynamic.point_right_target.y = -vehicle_model_obj.vehicle_length - OFFSET_Y;
  
  //dynamic line length
  vehicle_line_obj.line_sv_back.line_dynamic.line_segment_length_mm = len_v1 + len_v2 + len_v3 + len_v4;
  vehicle_line_obj.line_sv_back.line_dynamic.line_point_step_mm  = len_step;
  
  //dynamic point num
  vehicle_line_obj.line_sv_back.line_dynamic.line_point_num = 1.0 * (len_v1 + len_v2 + len_v3 + len_v4) / len_step + 1;
  
  //dynamic line attribute
  vehicle_line_obj.line_sv_back.line_dynamic.line_attr.r			= 255;
  vehicle_line_obj.line_sv_back.line_dynamic.line_attr.g			= 215;
  vehicle_line_obj.line_sv_back.line_dynamic.line_attr.b			= 0;	
  vehicle_line_obj.line_sv_back.line_dynamic.line_attr.width		= line_width;
  vehicle_line_obj.line_sv_back.line_dynamic.line_attr.isstrip		= line_isstrip;
  vehicle_line_obj.line_sv_back.line_dynamic.line_attr.index_start	= 0;
  vehicle_line_obj.line_sv_back.line_dynamic.line_attr.index_end	= vehicle_line_obj.line_sv_back.line_dynamic.line_point_num - 1;

  return AVM_ERR_NONE;

}
/*******************************************************************************************
* Function Name  : avm_line_sv_back_calc_coord
* Description    : 单视模式倒车轨迹线坐标
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_back_calc_coord()
{
  avm_line_sv_back_static_calc_coord();
  avm_line_sv_back_dynamic_calc_coord();
  return AVM_ERR_NONE;
}
/*******************************************************************************************
* Function Name  : avm_line_sv_back_static_calc_coord
* Description    : 单视模式倒车静态轨迹线坐标
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_back_static_calc_coord()
{
	int len_h1 = 0;			  
	int len_h2 = 0;
	int len_h3 = 0;
	int len_h4 = 0;
	int point_cnt = 0;
	int point_num = 0;
	int point_step = 0;
	int point_h_step = 0;
	int position_h1 = 0;				  
	int position_h2 = 0;
	int position_h3 = 0;
	int position_h4 = 0;
	int position_h1_index = 0;
	int position_h2_index = 0;
	int position_h3_index = 0;
	int position_h4_index = 0;
	int point_h1_num = 0; 	  
	int point_h2_num = 0;
	int point_h3_num = 0;
	int point_h4_num = 0;
	F64 y_input = 0;

	point_t *point_left_buf;		 
	point_t *point_right_buf;
	point_t *point_left_h1_buf;	  
	point_t *point_right_h1_buf;
	point_t *point_left_h2_buf;
	point_t *point_right_h2_buf;
	point_t *point_left_h3_buf;
	point_t *point_right_h3_buf;
	point_t *point_left_h4_buf;
	point_t *point_right_h4_buf;

	static virtual_camera_avm_t virtual_camera_avm_paraml = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static virtual_camera_sgv_t virtual_camera_sgv_paraml;
	static view_2d_parameter_t view_2d_parameter_param[4];

	if(virtual_camera_avm_paraml.scale_pixel_mm == 0)
	{
	  avm_disp_info_get(MODE_SV, &virtual_camera_avm_paraml,&virtual_camera_sgv_paraml, view_2d_parameter_param);
	}

	point_num  = vehicle_line_obj.line_sv_back.line_static.line_point_num;
	point_step = vehicle_line_obj.line_sv_back.line_static.line_point_step_mm;
	point_h_step = vehicle_line_obj.line_sv_back.line_static.line_point_h_step_mm;
		
	position_h1 = vehicle_line_obj.line_sv_back.line_static.line_segment_length_v1_mm;
	position_h2 = position_h1 + vehicle_line_obj.line_sv_back.line_static.line_segment_length_v2_mm;
	position_h3 = position_h2 + vehicle_line_obj.line_sv_back.line_static.line_segment_length_v3_mm;
	position_h4 = position_h3 + vehicle_line_obj.line_sv_back.line_static.line_segment_length_v4_mm;



	position_h1_index = position_h1 /point_step;
	position_h2_index = position_h2 /point_step;
	position_h3_index = position_h3 /point_step;
	position_h4_index = position_h4 /point_step;
	

	point_h1_num = vehicle_line_obj.line_sv_back.line_static.line_point_h1_num; 
	point_h2_num = vehicle_line_obj.line_sv_back.line_static.line_point_h2_num;
	point_h3_num = vehicle_line_obj.line_sv_back.line_static.line_point_h3_num;
	point_h4_num = vehicle_line_obj.line_sv_back.line_static.line_point_h4_num;
		
	point_left_buf	= (point_t *)malloc(point_num * sizeof(point_t));
	point_right_buf = (point_t *)malloc(point_num * sizeof(point_t));
	point_left_h1_buf  = (point_t *)malloc(point_h1_num * sizeof(point_t));
	point_right_h1_buf = (point_t *)malloc(point_h1_num * sizeof(point_t));
	point_left_h2_buf  = (point_t *)malloc(point_h2_num * sizeof(point_t));
	point_right_h2_buf = (point_t *)malloc(point_h2_num * sizeof(point_t));
	point_left_h3_buf  = (point_t *)malloc(point_h3_num * sizeof(point_t));
	point_right_h3_buf = (point_t *)malloc(point_h3_num * sizeof(point_t)); 
	point_left_h4_buf  = (point_t *)malloc(point_h4_num * sizeof(point_t));
	point_right_h4_buf = (point_t *)malloc(point_h4_num * sizeof(point_t)); 

	vehicle_line_obj.line_sv_back.line_static.point_left_buf	  = point_left_buf;
	vehicle_line_obj.line_sv_back.line_static.point_right_buf	  = point_right_buf;
	vehicle_line_obj.line_sv_back.line_static.point_left_h1_buf   = point_left_h1_buf;
	vehicle_line_obj.line_sv_back.line_static.point_right_h1_buf  = point_right_h1_buf;
	vehicle_line_obj.line_sv_back.line_static.point_left_h2_buf   = point_left_h2_buf;
	vehicle_line_obj.line_sv_back.line_static.point_right_h2_buf  = point_right_h2_buf;
	vehicle_line_obj.line_sv_back.line_static.point_left_h3_buf   = point_left_h3_buf;
	vehicle_line_obj.line_sv_back.line_static.point_right_h3_buf  = point_right_h3_buf;
	vehicle_line_obj.line_sv_back.line_static.point_left_h4_buf   = point_left_h4_buf;
	vehicle_line_obj.line_sv_back.line_static.point_right_h4_buf  = point_right_h4_buf;

	point_t *point_h1_buf;
	point_t *point_h2_buf;
	point_t *point_h3_buf;
	point_t *point_h4_buf;

	point_h1_buf  = (point_t *)malloc(2 * sizeof(point_t));
	point_h2_buf  = (point_t *)malloc(2 * sizeof(point_t));
	point_h3_buf  = (point_t *)malloc(2 * sizeof(point_t));
	point_h4_buf  = (point_t *)malloc(2 * sizeof(point_t));
	
	vehicle_line_obj.line_sv_back.line_static.point_h1_buf   = point_h1_buf;
	vehicle_line_obj.line_sv_back.line_static.point_h2_buf   = point_h2_buf;
	vehicle_line_obj.line_sv_back.line_static.point_h3_buf   = point_h3_buf;
	vehicle_line_obj.line_sv_front.line_static.point_h4_buf	 = point_h4_buf;




	//v
	for(point_cnt = 0; point_cnt < point_num; point_cnt++)
	{
	  y_input = point_cnt * point_step;
	  //left
	  point_left_buf[point_cnt].x = vehicle_line_obj.line_sv_back.line_static.point_left_target.x; 
	  point_left_buf[point_cnt].y = vehicle_line_obj.line_sv_back.line_static.point_left_target.y - y_input;
	  sv_cal_coord(point_left_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_BACK);

	  //right
	  point_right_buf[point_cnt].x = vehicle_line_obj.line_sv_back.line_static.point_right_target.x;
	  point_right_buf[point_cnt].y = vehicle_line_obj.line_sv_back.line_static.point_right_target.y - y_input;
	  sv_cal_coord(point_right_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_BACK);
	}

#if 0
	//h1
	point_h1_buf[0].x = point_left_buf[position_h1_index].x;
	point_h1_buf[0].y = point_left_buf[position_h1_index].y;
	point_h1_buf[1].x = point_right_buf[position_h1_index].x;
	point_h1_buf[1].y = point_right_buf[position_h1_index].y;
	//h2
	point_h2_buf[0].x = point_left_buf[position_h2_index].x;
	point_h2_buf[0].y = point_left_buf[position_h2_index].y;
	point_h2_buf[1].x = point_right_buf[position_h2_index].x;
	point_h2_buf[1].y = point_right_buf[position_h2_index].y;
	//h3
	point_h3_buf[0].x = point_left_buf[position_h3_index].x;
	point_h3_buf[0].y = point_left_buf[position_h3_index].y;
	point_h3_buf[1].x = point_right_buf[position_h3_index].x;
	point_h3_buf[1].y = point_right_buf[position_h3_index].y;

	//h4
	point_h4_buf[0].x = point_left_buf[position_h4_index].x;
	point_h4_buf[0].y = point_left_buf[position_h4_index].y;
	point_h4_buf[1].x = point_right_buf[position_h4_index].x;
	point_h4_buf[1].y = point_right_buf[position_h4_index].y;
#endif

#if 1
	//h1
	for(point_cnt = 0; point_cnt < point_h1_num; point_cnt++)
	{
	  y_input = point_cnt * point_step;
	  //left
	  point_left_h1_buf[point_cnt].x = vehicle_line_obj.line_sv_back.line_static.point_left_target.x + y_input;   
	  point_left_h1_buf[point_cnt].y = vehicle_line_obj.line_sv_back.line_static.point_left_target.y - position_h1;
	  sv_cal_coord(point_left_h1_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_BACK);
	  point_left_h1_buf[point_cnt].y = point_left_h1_buf[0].y;
	  //right
	  point_right_h1_buf[point_cnt].x = vehicle_line_obj.line_sv_back.line_static.point_right_target.x - y_input;
	  point_right_h1_buf[point_cnt].y = vehicle_line_obj.line_sv_back.line_static.point_right_target.y - position_h1;
	  sv_cal_coord(point_right_h1_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_BACK);
	  point_right_h1_buf[point_cnt].y = point_right_h1_buf[0].y;
	}
	//h2
	for(point_cnt = 0; point_cnt < point_h2_num; point_cnt++)
	{
	   y_input = point_cnt * point_step;
	  //left
	  point_left_h2_buf[point_cnt].x = vehicle_line_obj.line_sv_back.line_static.point_left_target.x + y_input;   
	  point_left_h2_buf[point_cnt].y = vehicle_line_obj.line_sv_back.line_static.point_left_target.y - position_h2;
	  sv_cal_coord(point_left_h2_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_BACK);
	  point_left_h2_buf[point_cnt].y = point_left_h2_buf[0].y;
	  //right
	  point_right_h2_buf[point_cnt].x = vehicle_line_obj.line_sv_back.line_static.point_right_target.x - y_input;
	  point_right_h2_buf[point_cnt].y = vehicle_line_obj.line_sv_back.line_static.point_right_target.y - position_h2 ;
	  sv_cal_coord(point_right_h2_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_BACK);
	  point_left_h2_buf[point_cnt].y = point_left_h2_buf[0].y;
	}
	//h3
	for(point_cnt = 0; point_cnt < point_h3_num; point_cnt++)
	{
	   y_input = point_cnt * point_step;
	  //left
	  point_left_h3_buf[point_cnt].x = vehicle_line_obj.line_sv_back.line_static.point_left_target.x + y_input;   
	  point_left_h3_buf[point_cnt].y = vehicle_line_obj.line_sv_back.line_static.point_left_target.y - position_h3;
	  sv_cal_coord(point_left_h3_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_BACK);
	  point_left_h3_buf[point_cnt].y = point_left_h3_buf[0].y;
	  //right
	  point_right_h3_buf[point_cnt].x = vehicle_line_obj.line_sv_back.line_static.point_right_target.x - y_input;
	  point_right_h3_buf[point_cnt].y = vehicle_line_obj.line_sv_back.line_static.point_right_target.y - position_h3;
	  sv_cal_coord(point_right_h3_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_BACK);
	  point_right_h3_buf[point_cnt].y = point_right_h3_buf[0].y;
	}
	//h4
	for(point_cnt = 0; point_cnt < point_h4_num; point_cnt++)
	{
	   y_input = point_cnt * point_step;
	  //left
	  point_left_h4_buf[point_cnt].x = vehicle_line_obj.line_sv_back.line_static.point_left_target.x + y_input;   
	  point_left_h4_buf[point_cnt].y = vehicle_line_obj.line_sv_back.line_static.point_left_target.y - position_h4;
	  sv_cal_coord(point_left_h4_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_BACK);
	  point_left_h4_buf[point_cnt].x = point_left_h4_buf[point_cnt].x - 1; 
	  point_left_h4_buf[point_cnt].y = point_left_h4_buf[0].y;
	  //right
	  point_right_h4_buf[point_cnt].x = vehicle_line_obj.line_sv_back.line_static.point_right_target.x - y_input;
	  point_right_h4_buf[point_cnt].y = vehicle_line_obj.line_sv_back.line_static.point_right_target.y - position_h4 ;
	  sv_cal_coord(point_right_h4_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_BACK);
	  point_right_h4_buf[point_cnt].x = point_right_h4_buf[point_cnt].x + 1;
	  point_right_h4_buf[point_cnt].y = point_right_h4_buf[0].y;
	}
#endif
	return AVM_ERR_NONE;

}
/*******************************************************************************************
* Function Name  : avm_line_sv_back_dynamic_calc_coord
* Description    : 单视倒车动态轨迹线坐标
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_back_dynamic_calc_coord()
{
  int point_cnt = 0;
  int point_num = 0;
  int point_step = 0;
  int angle_cnt = 0;
  int angle_input = 0;
  point_t *point_left_buf;
  point_t *point_right_buf;
  point_t *point_left_angle0_buf;
  point_t *point_right_angle0_buf;
  F64 coord_v[3]  = {0};
  S32 coord_vc[2] = {0};
  F64 x_output = 0;
  F64 y_input = 0;

  static virtual_camera_avm_t virtual_camera_avm_paraml = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  static virtual_camera_sgv_t virtual_camera_sgv_paraml;
  static view_2d_parameter_t view_2d_parameter_param[4];

  if(virtual_camera_avm_paraml.scale_pixel_mm == 0)
  {
	  avm_disp_info_get(MODE_SV, &virtual_camera_avm_paraml,&virtual_camera_sgv_paraml, view_2d_parameter_param);
  }
  
  point_num  = vehicle_line_obj.line_sv_back.line_dynamic.line_point_num;
  point_step = vehicle_line_obj.line_sv_back.line_dynamic.line_point_step_mm;

  for(angle_cnt = 0; angle_cnt < ANGLE_MAX_ABS * 2 + 1; angle_cnt++)
  {
	  point_left_buf  = (point_t *)malloc(point_num * sizeof(point_t));
	  point_right_buf = (point_t *)malloc(point_num * sizeof(point_t));
	  vehicle_line_obj.line_sv_back.line_dynamic.point_left_buf[angle_cnt]	= point_left_buf;
	  vehicle_line_obj.line_sv_back.line_dynamic.point_right_buf[angle_cnt] = point_right_buf;		
  }

  point_left_angle0_buf = (point_t *)malloc(point_num * sizeof(point_t));
  point_right_angle0_buf = (point_t *)malloc(point_num * sizeof(point_t));

  for(point_cnt = 0; point_cnt < point_num; point_cnt++)
  {
	  point_left_angle0_buf[point_cnt].x = vehicle_line_obj.line_sv_back.line_dynamic.point_left_target.x;		
	  point_left_angle0_buf[point_cnt].y = point_cnt * point_step;

	  point_right_angle0_buf[point_cnt].x = vehicle_line_obj.line_sv_back.line_dynamic.point_right_target.x;  
	  point_right_angle0_buf[point_cnt].y = point_cnt * point_step;
  }   

  for(angle_cnt = 0; angle_cnt < ANGLE_MAX_ABS * 2 + 1; angle_cnt++)
  {
	  point_left_buf = vehicle_line_obj.line_sv_back.line_dynamic.point_left_buf[angle_cnt];
	  point_right_buf = vehicle_line_obj.line_sv_back.line_dynamic.point_right_buf[angle_cnt];	 

	  angle_input = angle_cnt - ANGLE_MAX_ABS;
	 if(angle_input < 0)
	  {
		  vehicle_model_obj.direction = TURN_LEFT;
	  }
	  else if (angle_input > 0)
	  {
		  vehicle_model_obj.direction = TURN_RIGHT;
	  }
	  else
	  {
		  vehicle_model_obj.direction = STRAIGHT;

	  }
	  vehicle_model_obj.angle_front_wheel = angle_input;
	  vehicle_model_obj.cam_index = CAM_BACK;
	  vehicle_model_obj.length_line = vehicle_line_obj.line_sv_back.line_dynamic.line_segment_length_mm;
	  vehicle_model_obj.step_line = point_step;
	  for(point_cnt = 0; point_cnt < point_num; point_cnt++)
	  {
		  //left
		  vehicle_model_obj.point_target.x = vehicle_line_obj.line_sv_back.line_dynamic.point_left_target.x;
		  vehicle_model_obj.point_target.y = vehicle_line_obj.line_sv_back.line_dynamic.point_left_target.y;

		  y_input = vehicle_model_obj.point_target.y - point_left_angle0_buf[point_cnt].y;		
		  avm_sv_calc_x_from_vehicle_model(&y_input, &x_output, &vehicle_model_obj); 
		  point_left_buf[point_cnt].x = x_output;
		  point_left_buf[point_cnt].y = y_input;
		  sv_cal_coord(point_left_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_BACK);
	  
		  //right
		  vehicle_model_obj.point_target.x = vehicle_line_obj.line_sv_back.line_dynamic.point_right_target.x;
		  vehicle_model_obj.point_target.y = vehicle_line_obj.line_sv_back.line_dynamic.point_right_target.y;

		  y_input = vehicle_model_obj.point_target.y - point_right_angle0_buf[point_cnt].y;
		  avm_sv_calc_x_from_vehicle_model(&y_input, &x_output, &vehicle_model_obj);	
		  point_right_buf[point_cnt].x = x_output;
		  point_right_buf[point_cnt].y = y_input;
		  sv_cal_coord(point_right_buf,point_cnt,&virtual_camera_sgv_paraml,CAM_BACK);
	  }
  }
  
  free(point_left_angle0_buf);
  free(point_right_angle0_buf);
  
  return AVM_ERR_NONE;
}
/*******************************************************************************************
* Function Name  : avm_line_sv_left_init
* Description    : 单视模式左侧辅助线初始化
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_left_init()
{
  avm_line_sv_left_cfg();
  avm_line_sv_left_calc_coord();
  return AVM_ERR_NONE;
}
/*******************************************************************************************
* Function Name  : avm_line_sv_left_cfg
* Description    : 单视模式左侧辅助线配置参数
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_left_cfg()
{
  int len_near;
  int len_far;
  int len_up;
  int len_down;
  int len_step;
  int line_width;

  len_near = 500;
  len_far  = 500;
  len_up   = 500;
  len_down = 1000;
  len_step = 200;
  line_width = LINE_WIDTH;
  //娴嬭瘯鐐?
  vehicle_line_obj.line_sv_left.line_side.point_left_target.x = -vehicle_model_obj.vehicle_width / 2;
  vehicle_line_obj.line_sv_left.line_side.point_left_target.y = 0;
  //绾块暱
  vehicle_line_obj.line_sv_left.line_side.line_segment_length_near_mm = len_near;
  vehicle_line_obj.line_sv_left.line_side.line_segment_length_far_mm  = len_far;
  vehicle_line_obj.line_sv_left.line_side.line_segment_length_up_mm   = len_up;
  vehicle_line_obj.line_sv_left.line_side.line_segment_length_down_mm = len_far;
  vehicle_line_obj.line_sv_left.line_side.line_point_step_mm		  = len_step;
  //鐐规暟
  vehicle_line_obj.line_sv_left.line_side.line_point_near_num = 1.0 * len_near / len_step + 1 + 0.5;
  vehicle_line_obj.line_sv_left.line_side.line_point_far_num  = 1.0 * len_far / len_step + 1 + 0.5;
  vehicle_line_obj.line_sv_left.line_side.line_point_up_num   = 1.0 * len_up / len_step + 1 + 0.5;
  vehicle_line_obj.line_sv_left.line_side.line_point_down_num = 1.0 * len_down / len_step + 1 + 0.5;

  //灞炴�?
  vehicle_line_obj.line_sv_left.line_side.line_h_near_attr.r			= 255;
  vehicle_line_obj.line_sv_left.line_side.line_h_near_attr.g			= 215;
  vehicle_line_obj.line_sv_left.line_side.line_h_near_attr.b			= 0;	 
  vehicle_line_obj.line_sv_left.line_side.line_h_near_attr.width		= line_width;
  vehicle_line_obj.line_sv_left.line_side.line_h_near_attr.isstrip		= 1;
  vehicle_line_obj.line_sv_left.line_side.line_h_near_attr.index_start	= 0;
  vehicle_line_obj.line_sv_left.line_side.line_h_near_attr.index_end	= 1.0 * len_near / len_step + 0.5;

  vehicle_line_obj.line_sv_left.line_side.line_h_far_attr.r 			= 255;
  vehicle_line_obj.line_sv_left.line_side.line_h_far_attr.g 			= 215;
  vehicle_line_obj.line_sv_left.line_side.line_h_far_attr.b 			= 0;	 
  vehicle_line_obj.line_sv_left.line_side.line_h_far_attr.width 		= line_width;
  vehicle_line_obj.line_sv_left.line_side.line_h_far_attr.isstrip		= 1;
  vehicle_line_obj.line_sv_left.line_side.line_h_far_attr.index_start	= 0;
  vehicle_line_obj.line_sv_left.line_side.line_h_far_attr.index_end 	= 1.0 * len_far / len_step + 0.5;

  vehicle_line_obj.line_sv_left.line_side.line_v_up_attr.r				= 255;
  vehicle_line_obj.line_sv_left.line_side.line_v_up_attr.g				= 215;
  vehicle_line_obj.line_sv_left.line_side.line_v_up_attr.b				= 0;   
  vehicle_line_obj.line_sv_left.line_side.line_v_up_attr.width			= line_width;
  vehicle_line_obj.line_sv_left.line_side.line_v_up_attr.isstrip		= 1;
  vehicle_line_obj.line_sv_left.line_side.line_v_up_attr.index_start	= 0;
  vehicle_line_obj.line_sv_left.line_side.line_v_up_attr.index_end		= 1.0 * len_up / len_step + 0.5;

  vehicle_line_obj.line_sv_left.line_side.line_v_down_attr.r			= 255;
  vehicle_line_obj.line_sv_left.line_side.line_v_down_attr.g			= 215;
  vehicle_line_obj.line_sv_left.line_side.line_v_down_attr.b			= 0;	 
  vehicle_line_obj.line_sv_left.line_side.line_v_down_attr.width		= line_width;
  vehicle_line_obj.line_sv_left.line_side.line_v_down_attr.isstrip		= 1;
  vehicle_line_obj.line_sv_left.line_side.line_v_down_attr.index_start	= 0;
  vehicle_line_obj.line_sv_left.line_side.line_v_down_attr.index_end	= 1.0 * len_down / len_step + 0.5;
  return AVM_ERR_NONE;
}
/*******************************************************************************************
* Function Name  : avm_line_sv_left_calc_coord
* Description    : 单视左侧辅助线坐标
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_left_calc_coord()
{
  int point_cnt = 0;
  int point_near_num = 0;
  int point_far_num = 0;
  int point_up_num = 0;
  int point_down_num = 0;
  int point_step = 0;
  F64 coord_v[3]  = {0};
  S32 coord_vc[2] = {0};
  point_t *point_near_buf;
  point_t *point_far_buf;
  point_t *point_up_buf;
  point_t *point_down_buf;

  static virtual_camera_avm_t virtual_camera_avm_paraml = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  static virtual_camera_sgv_t virtual_camera_sgv_paraml;
  static view_2d_parameter_t view_2d_parameter_param[4];

  if(virtual_camera_avm_paraml.scale_pixel_mm == 0)
  {
	  avm_disp_info_get(MODE_SV, &virtual_camera_avm_paraml,&virtual_camera_sgv_paraml, view_2d_parameter_param);
  }
  
  point_near_num = vehicle_line_obj.line_sv_left.line_side.line_point_near_num;
  point_far_num  = vehicle_line_obj.line_sv_left.line_side.line_point_far_num;
  point_up_num	 = vehicle_line_obj.line_sv_left.line_side.line_point_up_num;
  point_down_num = vehicle_line_obj.line_sv_left.line_side.line_point_down_num;

  point_step = vehicle_line_obj.line_sv_left.line_side.line_point_step_mm;

  point_near_buf = (point_t *)malloc(point_near_num * sizeof(point_t));
  point_far_buf  = (point_t *)malloc(point_far_num * sizeof(point_t));
  point_up_buf	 = (point_t *)malloc(point_up_num * sizeof(point_t));
  point_down_buf = (point_t *)malloc(point_down_num * sizeof(point_t));

  vehicle_line_obj.line_sv_left.line_side.point_left_near_buf = point_near_buf;
  vehicle_line_obj.line_sv_left.line_side.point_left_far_buf  = point_far_buf;
  vehicle_line_obj.line_sv_left.line_side.point_left_up_buf   = point_up_buf;
  vehicle_line_obj.line_sv_left.line_side.point_left_down_buf = point_down_buf;

  for(point_cnt = 0; point_cnt < point_near_num; point_cnt++)
  {
	  point_near_buf[point_cnt].x = vehicle_line_obj.line_sv_left.line_side.point_left_target.x + point_cnt * point_step ;	
	  point_near_buf[point_cnt].y = vehicle_line_obj.line_sv_left.line_side.point_left_target.y;
// DbgPrintf("(%f,%f)\n",point_near_buf[point_cnt].x,point_near_buf[point_cnt].y);
	  coord_v[0] = point_near_buf[point_cnt].x;
	  coord_v[1] = point_near_buf[point_cnt].y;
	  coord_v[2] = 0;
	  avmlib_getVCImageCoordFromVehicleCoord(&virtual_camera_sgv_paraml.puVC[3],coord_v,coord_vc);
	  point_near_buf[point_cnt].x = coord_vc[0];  
	  point_near_buf[point_cnt].y = coord_vc[1];
  }
  for(point_cnt = 0; point_cnt < point_far_num; point_cnt++)
  {
	  point_far_buf[point_cnt].x = vehicle_line_obj.line_sv_left.line_side.point_left_target.x - point_cnt * point_step ;  
	  point_far_buf[point_cnt].y = vehicle_line_obj.line_sv_left.line_side.point_left_target.y;

	  coord_v[0] = point_far_buf[point_cnt].x;
	  coord_v[1] = point_far_buf[point_cnt].y;
	  coord_v[2] = 0;
	  avmlib_getVCImageCoordFromVehicleCoord(&virtual_camera_sgv_paraml.puVC[3],coord_v,coord_vc);
	  point_far_buf[point_cnt].x = coord_vc[0];  
	  point_far_buf[point_cnt].y = coord_vc[1];
  }
  for(point_cnt = 0; point_cnt < point_up_num; point_cnt++)
  {
	  point_up_buf[point_cnt].x = vehicle_line_obj.line_sv_left.line_side.point_left_target.x;
	  point_up_buf[point_cnt].y = vehicle_line_obj.line_sv_left.line_side.point_left_target.y + point_cnt * point_step;

	  coord_v[0] = point_up_buf[point_cnt].x;
	  coord_v[1] = point_up_buf[point_cnt].y;
	  coord_v[2] = 0;
	  avmlib_getVCImageCoordFromVehicleCoord(&virtual_camera_sgv_paraml.puVC[3],coord_v,coord_vc);
	  point_up_buf[point_cnt].x = coord_vc[0];	
	  point_up_buf[point_cnt].y = coord_vc[1];
  }
  for(point_cnt = 0; point_cnt < point_down_num; point_cnt++)
  {
	  point_down_buf[point_cnt].x = vehicle_line_obj.line_sv_left.line_side.point_left_target.x;
	  point_down_buf[point_cnt].y = vehicle_line_obj.line_sv_left.line_side.point_left_target.y - point_cnt * point_step;

	  coord_v[0] = point_down_buf[point_cnt].x;
	  coord_v[1] = point_down_buf[point_cnt].y;
	  coord_v[2] = 0;
	  avmlib_getVCImageCoordFromVehicleCoord(&virtual_camera_sgv_paraml.puVC[3],coord_v,coord_vc);
	  point_down_buf[point_cnt].x = coord_vc[0];  
	  point_down_buf[point_cnt].y = coord_vc[1];
  }
  return AVM_ERR_NONE;
}
/*******************************************************************************************
* Function Name  : avm_line_sv_right_init
* Description    : 单视右侧辅助线初始化
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_right_init()
{
  avm_line_sv_right_cfg();
  avm_line_sv_right_calc_coord();
  return AVM_ERR_NONE;

}
/*******************************************************************************************
* Function Name  : avm_line_sv_right_cfg
* Description    : 单视右侧辅助线配置参数
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/
static AVM_ERR avm_line_sv_right_cfg()
{
  int len_near;
  int len_far;
  int len_up;
  int len_down;
  int len_step;
  int line_width;

  len_near = 500;
  len_far  = 500;
  len_up   = 500;
  len_down = 1000;
  len_step = 100;
  line_width = LINE_WIDTH;
  //娴嬭瘯鐐?
  vehicle_line_obj.line_sv_right.line_side.point_right_target.x = vehicle_model_obj.vehicle_width / 2;
  vehicle_line_obj.line_sv_right.line_side.point_right_target.y = 0;
  //绾块暱
  vehicle_line_obj.line_sv_right.line_side.line_segment_length_near_mm = len_near;
  vehicle_line_obj.line_sv_right.line_side.line_segment_length_far_mm  = len_far;
  vehicle_line_obj.line_sv_right.line_side.line_segment_length_up_mm   = len_up;
  vehicle_line_obj.line_sv_right.line_side.line_segment_length_down_mm = len_far;
  vehicle_line_obj.line_sv_right.line_side.line_point_step_mm		   = len_step;
  //鐐规暟
  vehicle_line_obj.line_sv_right.line_side.line_point_near_num	= 1.0 * len_near / len_step + 1 + 0.5;
  vehicle_line_obj.line_sv_right.line_side.line_point_far_num	= 1.0 * len_far / len_step + 1 + 0.5;
  vehicle_line_obj.line_sv_right.line_side.line_point_up_num	= 1.0 * len_up / len_step + 1 + 0.5;
  vehicle_line_obj.line_sv_right.line_side.line_point_down_num	= 1.0 * len_down / len_step + 1 + 0.5;
  //灞炴�?
  vehicle_line_obj.line_sv_right.line_side.line_h_near_attr.r			= 255;
  vehicle_line_obj.line_sv_right.line_side.line_h_near_attr.g			= 215;
  vehicle_line_obj.line_sv_right.line_side.line_h_near_attr.b			= 0;	  
  vehicle_line_obj.line_sv_right.line_side.line_h_near_attr.width		= line_width;
  vehicle_line_obj.line_sv_right.line_side.line_h_near_attr.isstrip 	= 1;
  vehicle_line_obj.line_sv_right.line_side.line_h_near_attr.index_start = 0;
  vehicle_line_obj.line_sv_right.line_side.line_h_near_attr.index_end	= 1.0 * len_near / len_step + 0.5;

  vehicle_line_obj.line_sv_right.line_side.line_h_far_attr.r			= 255;
  vehicle_line_obj.line_sv_right.line_side.line_h_far_attr.g			= 215;
  vehicle_line_obj.line_sv_right.line_side.line_h_far_attr.b			= 0;	  
  vehicle_line_obj.line_sv_right.line_side.line_h_far_attr.width		= line_width;
  vehicle_line_obj.line_sv_right.line_side.line_h_far_attr.isstrip		= 1;
  vehicle_line_obj.line_sv_right.line_side.line_h_far_attr.index_start	= 0;
  vehicle_line_obj.line_sv_right.line_side.line_h_far_attr.index_end	= 1.0 * len_far / len_step + 0.5;

  vehicle_line_obj.line_sv_right.line_side.line_v_up_attr.r 			= 255;
  vehicle_line_obj.line_sv_right.line_side.line_v_up_attr.g 			= 215;
  vehicle_line_obj.line_sv_right.line_side.line_v_up_attr.b 			= 0;   
  vehicle_line_obj.line_sv_right.line_side.line_v_up_attr.width 		= line_width;
  vehicle_line_obj.line_sv_right.line_side.line_v_up_attr.isstrip		= 1;
  vehicle_line_obj.line_sv_right.line_side.line_v_up_attr.index_start	= 0;
  vehicle_line_obj.line_sv_right.line_side.line_v_up_attr.index_end 	= 1.0 * len_up / len_step + 0.5;

  vehicle_line_obj.line_sv_right.line_side.line_v_down_attr.r			= 255;
  vehicle_line_obj.line_sv_right.line_side.line_v_down_attr.g			= 215;
  vehicle_line_obj.line_sv_right.line_side.line_v_down_attr.b			= 0;	  
  vehicle_line_obj.line_sv_right.line_side.line_v_down_attr.width		= line_width;
  vehicle_line_obj.line_sv_right.line_side.line_v_down_attr.isstrip 	= 1;
  vehicle_line_obj.line_sv_right.line_side.line_v_down_attr.index_start = 0;
  vehicle_line_obj.line_sv_right.line_side.line_v_down_attr.index_end	= 1.0 * len_down / len_step + 0.5;
  return AVM_ERR_NONE;

}
/*******************************************************************************************
* Function Name  : avm_line_sv_right_calc_coord
* Description    : 单视模式右侧辅助线坐标
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_right_calc_coord()
{
  int point_cnt = 0;
  int point_near_num = 0;
  int point_far_num = 0;
  int point_up_num = 0;
  int point_down_num = 0;
  int point_step = 0;
  F64 coord_v[3]  = {0};
  S32 coord_vc[2] = {0};
  point_t *point_near_buf;
  point_t *point_far_buf;
  point_t *point_up_buf;
  point_t *point_down_buf;
  
  static virtual_camera_avm_t virtual_camera_avm_paraml = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  static virtual_camera_sgv_t virtual_camera_sgv_paraml;
  static view_2d_parameter_t view_2d_parameter_param[4];

  if(virtual_camera_avm_paraml.scale_pixel_mm == 0)
  {
	  avm_disp_info_get(MODE_SV, &virtual_camera_avm_paraml,&virtual_camera_sgv_paraml, view_2d_parameter_param);
  }

  point_near_num = vehicle_line_obj.line_sv_right.line_side.line_point_near_num;
  point_far_num  = vehicle_line_obj.line_sv_right.line_side.line_point_far_num;
  point_up_num	 = vehicle_line_obj.line_sv_right.line_side.line_point_up_num;
  point_down_num = vehicle_line_obj.line_sv_right.line_side.line_point_down_num;

  point_step = vehicle_line_obj.line_sv_right.line_side.line_point_step_mm;

  point_near_buf = (point_t *)malloc(point_near_num * sizeof(point_t));
  point_far_buf  = (point_t *)malloc(point_far_num * sizeof(point_t));
  point_up_buf	 = (point_t *)malloc(point_up_num * sizeof(point_t));
  point_down_buf = (point_t *)malloc(point_down_num * sizeof(point_t));

  vehicle_line_obj.line_sv_right.line_side.point_right_near_buf = point_near_buf;
  vehicle_line_obj.line_sv_right.line_side.point_right_far_buf	= point_far_buf;
  vehicle_line_obj.line_sv_right.line_side.point_right_up_buf	= point_up_buf;
  vehicle_line_obj.line_sv_right.line_side.point_right_down_buf = point_down_buf;

  for(point_cnt = 0; point_cnt < point_near_num; point_cnt++)
  {
	  point_near_buf[point_cnt].x = vehicle_line_obj.line_sv_right.line_side.point_right_target.x - point_cnt * point_step;  
	  point_near_buf[point_cnt].y = vehicle_line_obj.line_sv_right.line_side.point_right_target.y;

	  coord_v[0] = point_near_buf[point_cnt].x;
	  coord_v[1] = point_near_buf[point_cnt].y;
	  coord_v[2] = 0;
	  avmlib_getVCImageCoordFromVehicleCoord(&virtual_camera_sgv_paraml.puVC[1],coord_v,coord_vc);
	  point_near_buf[point_cnt].x = coord_vc[0];  
	  point_near_buf[point_cnt].y = coord_vc[1];
  }
  for(point_cnt = 0; point_cnt < point_far_num; point_cnt++)
  {
	  point_far_buf[point_cnt].x = vehicle_line_obj.line_sv_right.line_side.point_right_target.x + point_cnt * point_step;	 
	  point_far_buf[point_cnt].y = vehicle_line_obj.line_sv_right.line_side.point_right_target.y;

	  coord_v[0] = point_far_buf[point_cnt].x;
	  coord_v[1] = point_far_buf[point_cnt].y;
	  coord_v[2] = 0;
	  avmlib_getVCImageCoordFromVehicleCoord(&virtual_camera_sgv_paraml.puVC[1],coord_v,coord_vc);
	  point_far_buf[point_cnt].x = coord_vc[0];  
	  point_far_buf[point_cnt].y = coord_vc[1];
  }

  for(point_cnt = 0; point_cnt < point_up_num; point_cnt++)
  {
	  point_up_buf[point_cnt].x = vehicle_line_obj.line_sv_right.line_side.point_right_target.x;	
	  point_up_buf[point_cnt].y = vehicle_line_obj.line_sv_right.line_side.point_right_target.y + point_cnt * point_step;

	  coord_v[0] = point_up_buf[point_cnt].x;
	  coord_v[1] = point_up_buf[point_cnt].y;
	  coord_v[2] = 0;
	  avmlib_getVCImageCoordFromVehicleCoord(&virtual_camera_sgv_paraml.puVC[1],coord_v,coord_vc);
	  point_up_buf[point_cnt].x = coord_vc[0];	
	  point_up_buf[point_cnt].y = coord_vc[1];
  }
  for(point_cnt = 0; point_cnt < point_down_num; point_cnt++)
  {
	  point_down_buf[point_cnt].x = vehicle_line_obj.line_sv_right.line_side.point_right_target.x;	 
	  point_down_buf[point_cnt].y = vehicle_line_obj.line_sv_right.line_side.point_right_target.y - point_cnt * point_step;

	  coord_v[0] = point_down_buf[point_cnt].x;
	  coord_v[1] = point_down_buf[point_cnt].y;
	  coord_v[2] = 0;
	  avmlib_getVCImageCoordFromVehicleCoord(&virtual_camera_sgv_paraml.puVC[1],coord_v,coord_vc);
	  point_down_buf[point_cnt].x = coord_vc[0];  
	  point_down_buf[point_cnt].y = coord_vc[1];
  }
  return AVM_ERR_NONE;
}


/*******************************************************************************************
* Function Name  : avm_line_sv_front_static_draw
* Description    : 单视前轮静态轨迹线绘制
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_front_static_draw()
{
	  int index_start = 0;
	  int index_end = 0;
	  int point_cnt = 0;

	  int point_h1_num = 0;
	  int point_h2_num = 0;
	  int point_h3_num = 0;
	  int point_h4_num = 0;
	
	  line_strip_attr_t line_strip_attr;
  
	  static virtual_camera_avm_t virtual_camera_avm_paraml = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	  static virtual_camera_sgv_t virtual_camera_sgv_paraml;
	  static view_2d_parameter_t view_2d_parameter_param[4];
  
	  if(virtual_camera_avm_paraml.scale_pixel_mm == 0)
	  {
			avm_disp_info_get(MODE_SV, &virtual_camera_avm_paraml,&virtual_camera_sgv_paraml, view_2d_parameter_param);
	  }

	  point_h1_num = vehicle_line_obj.line_sv_front.line_static.line_point_h1_num; 
	  point_h2_num = vehicle_line_obj.line_sv_front.line_static.line_point_h2_num; 
	  point_h3_num = vehicle_line_obj.line_sv_front.line_static.line_point_h3_num; 
	  point_h4_num = vehicle_line_obj.line_sv_front.line_static.line_point_h4_num; 

	  line_strip_attr.Blend   = 255;
	  line_strip_attr.IsStrip = 1;
	  line_strip_attr.win_x    = view_2d_parameter_param[0].screen_x;
	  line_strip_attr.win_y    = view_2d_parameter_param[0].screen_y;
	  line_strip_attr.win_width  = view_2d_parameter_param[0].screen_width;
	  line_strip_attr.win_height = view_2d_parameter_param[0].screen_height;
	  line_strip_attr.scalex = 1.0 * line_strip_attr.win_width / virtual_camera_sgv_paraml.vc_width[0];
	  line_strip_attr.scaley = 1.0 * line_strip_attr.win_height / virtual_camera_sgv_paraml.vc_height[0];
#if (MODE_DIFFERENT == 1)
//v1-v4属性一致，h不同属性
	  //h1
	  line_strip_attr.linecolor.red   = vehicle_line_obj.line_sv_front.line_static.line_v1_attr.r;
	  line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_front.line_static.line_v1_attr.g;
	  line_strip_attr.linecolor.blue  = vehicle_line_obj.line_sv_front.line_static.line_v1_attr.b;
	  line_strip_attr.line_w = vehicle_line_obj.line_sv_front.line_static.line_v1_attr.width;
	  //avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_h1_buf,2,&line_strip_attr,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_left_h1_buf,point_h1_num,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_right_h1_buf,point_h1_num,&line_strip_attr,NULL,NULL,NULL);

	  //h2
	  line_strip_attr.linecolor.red   = vehicle_line_obj.line_sv_front.line_static.line_v2_attr.r;
	  line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_front.line_static.line_v2_attr.g;
	  line_strip_attr.linecolor.blue  = vehicle_line_obj.line_sv_front.line_static.line_v2_attr.b;
	  line_strip_attr.line_w = vehicle_line_obj.line_sv_front.line_static.line_v2_attr.width;
	  //avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_h2_buf,2,&line_strip_attr,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_left_h2_buf,point_h2_num,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_right_h2_buf,point_h2_num,&line_strip_attr,NULL,NULL,NULL);


	  //h3
	  line_strip_attr.linecolor.red   = vehicle_line_obj.line_sv_front.line_static.line_v3_attr.r;
	  line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_front.line_static.line_v3_attr.g;
	  line_strip_attr.linecolor.blue  = vehicle_line_obj.line_sv_front.line_static.line_v3_attr.b;
	  line_strip_attr.line_w = vehicle_line_obj.line_sv_front.line_static.line_v3_attr.width;
	  //avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_h3_buf,2,&line_strip_attr,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_left_h3_buf,point_h3_num,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_right_h3_buf,point_h3_num,&line_strip_attr,NULL,NULL,NULL);

	  //h4
	  line_strip_attr.linecolor.red   = vehicle_line_obj.line_sv_front.line_static.line_v4_attr.r;
	  line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_front.line_static.line_v4_attr.g;
	  line_strip_attr.linecolor.blue  = vehicle_line_obj.line_sv_front.line_static.line_v4_attr.b;
	  line_strip_attr.line_w = vehicle_line_obj.line_sv_front.line_static.line_v4_attr.width;
	  //avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_h4_buf,2,&line_strip_attr,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_left_h4_buf,point_h4_num,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_right_h4_buf,point_h4_num,&line_strip_attr,NULL,NULL,NULL);

	  //v1-v4(left/right)
	  point_cnt = vehicle_line_obj.line_sv_front.line_static.line_point_num;
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_left_buf,point_cnt,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_right_buf,point_cnt,&line_strip_attr,NULL,NULL,NULL);
#endif

#if (MODE_SAME == 1)
	  //v1和h1属性一致
	  //h1,v1
	  line_strip_attr.linecolor.red   = vehicle_line_obj.line_sv_front.line_static.line_v1_attr.r;
	  line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_front.line_static.line_v1_attr.g;
	  line_strip_attr.linecolor.blue  = vehicle_line_obj.line_sv_front.line_static.line_v1_attr.b;
	  line_strip_attr.line_w = vehicle_line_obj.line_sv_front.line_static.line_v1_attr.width;
	  index_start = vehicle_line_obj.line_sv_front.line_static.line_v1_attr.index_start;
	  index_end   = vehicle_line_obj.line_sv_front.line_static.line_v1_attr.index_end;
	  point_cnt   = index_end - index_start + 1;
	  avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_front.line_static.point_left_buf[index_start],point_cnt,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_front.line_static.point_right_buf[index_start],point_cnt,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_left_h1_buf,point_h1_num,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_right_h1_buf,point_h1_num,&line_strip_attr,NULL,NULL,NULL);
	  //h2,v2
	  line_strip_attr.linecolor.red   = vehicle_line_obj.line_sv_front.line_static.line_v2_attr.r;
	  line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_front.line_static.line_v2_attr.g;
	  line_strip_attr.linecolor.blue  = vehicle_line_obj.line_sv_front.line_static.line_v2_attr.b;
	  line_strip_attr.line_w = vehicle_line_obj.line_sv_front.line_static.line_v2_attr.width;
	  index_start = vehicle_line_obj.line_sv_front.line_static.line_v2_attr.index_start;
	  index_end   = vehicle_line_obj.line_sv_front.line_static.line_v2_attr.index_end;
	  point_cnt   = index_end - index_start + 1;
	  avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_front.line_static.point_left_buf[index_start],point_cnt,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_front.line_static.point_right_buf[index_start],point_cnt,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_left_h2_buf,point_h2_num,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_right_h2_buf,point_h2_num,&line_strip_attr,NULL,NULL,NULL);
	  //h3,v3
	  line_strip_attr.linecolor.red   = vehicle_line_obj.line_sv_front.line_static.line_v3_attr.r;
	  line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_front.line_static.line_v3_attr.g;
	  line_strip_attr.linecolor.blue  = vehicle_line_obj.line_sv_front.line_static.line_v3_attr.b;
	  line_strip_attr.line_w = vehicle_line_obj.line_sv_front.line_static.line_v3_attr.width;
	  index_start = vehicle_line_obj.line_sv_front.line_static.line_v3_attr.index_start;
	  index_end   = vehicle_line_obj.line_sv_front.line_static.line_v3_attr.index_end;
	  point_cnt   = index_end - index_start + 1;
	  avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_front.line_static.point_left_buf[index_start],point_cnt,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_front.line_static.point_right_buf[index_start],point_cnt,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_left_h3_buf,point_h3_num,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_right_h3_buf,point_h3_num,&line_strip_attr,NULL,NULL,NULL);
	  //h4,v4
	  line_strip_attr.linecolor.red   = vehicle_line_obj.line_sv_front.line_static.line_v4_attr.r;
	  line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_front.line_static.line_v4_attr.g;
	  line_strip_attr.linecolor.blue  = vehicle_line_obj.line_sv_front.line_static.line_v4_attr.b;
	  line_strip_attr.line_w = vehicle_line_obj.line_sv_front.line_static.line_v4_attr.width;
	  index_start = vehicle_line_obj.line_sv_front.line_static.line_v4_attr.index_start;
	  index_end   = vehicle_line_obj.line_sv_front.line_static.line_v4_attr.index_end;
	  point_cnt   = index_end - index_start + 1;		  
	  avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_front.line_static.point_left_buf[index_start],point_cnt,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_front.line_static.point_right_buf[index_start],point_cnt,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_left_h4_buf,point_h4_num,&line_strip_attr,NULL,NULL,NULL);
	  avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_static.point_right_h4_buf,point_h4_num,&line_strip_attr,NULL,NULL,NULL);
#endif
	return AVM_ERR_NONE;
}
/*******************************************************************************************
* Function Name  : avm_line_sv_front_dynamic_draw
* Description    : 单视前轮动态轨迹线绘制
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_front_dynamic_draw(double angle)
{
	int index_start = 0;
	int index_end = 0;
	int point_cnt = 0;
	int angle_index = 0;

	line_strip_attr_t line_strip_attr;

	static virtual_camera_avm_t virtual_camera_avm_paraml = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static virtual_camera_sgv_t virtual_camera_sgv_paraml;
	static view_2d_parameter_t view_2d_parameter_param[4];

	if(virtual_camera_avm_paraml.scale_pixel_mm == 0)
	{
	  avm_disp_info_get(MODE_SV, &virtual_camera_avm_paraml,&virtual_camera_sgv_paraml, view_2d_parameter_param);
	}

	line_strip_attr.Blend	= 255;
	line_strip_attr.IsStrip = 1;
	line_strip_attr.win_x	   = view_2d_parameter_param[0].screen_x;
	line_strip_attr.win_y	   = view_2d_parameter_param[0].screen_y;
	line_strip_attr.win_width  = view_2d_parameter_param[0].screen_width;
	line_strip_attr.win_height = view_2d_parameter_param[0].screen_height;
	line_strip_attr.scalex = 1.0 * line_strip_attr.win_width / virtual_camera_sgv_paraml.vc_width[0];
	line_strip_attr.scaley = 1.0 * line_strip_attr.win_height / virtual_camera_sgv_paraml.vc_height[0];


	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_front.line_dynamic.line_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_front.line_dynamic.line_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_front.line_dynamic.line_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_front.line_dynamic.line_attr.width;

	point_cnt = vehicle_line_obj.line_sv_front.line_dynamic.line_point_num;

	angle_index = angle + ANGLE_MAX_ABS;
	if(angle_index > ANGLE_MAX_ABS * 2)
	{
	   angle_index = ANGLE_MAX_ABS * 2;
	}
	else if(angle_index < 0)
	{
	   angle_index = 0;
	}

	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_dynamic.point_left_buf[angle_index], point_cnt, &line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_dynamic.point_right_buf[angle_index], point_cnt, &line_strip_attr,NULL,NULL,NULL);	  

#if (FRONT_DYNAMIC_LINE_END_CONNECTION == 1)
	point_t point_h_buf[2];
	point_h_buf[0].x = vehicle_line_obj.line_sv_front.line_dynamic.point_left_buf[angle_index][point_cnt - 1].x - 2;
	point_h_buf[0].y = vehicle_line_obj.line_sv_front.line_dynamic.point_left_buf[angle_index][point_cnt - 1].y;
	point_h_buf[1].x = vehicle_line_obj.line_sv_front.line_dynamic.point_right_buf[angle_index][point_cnt - 1].x + 2;
	point_h_buf[1].y = vehicle_line_obj.line_sv_front.line_dynamic.point_right_buf[angle_index][point_cnt - 1].y;
	avmlib_Draw_Lines_Strip(point_h_buf, 2, &line_strip_attr,NULL,NULL,NULL); 
#endif
#if 0	
	int i = 0;
	line_strip_attr.line_w = 1;
	for(i = 0; i < ANGLE_MAX_ABS*2; i++)
	{
	
		avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_dynamic.point_left_buf[i], point_cnt, &line_strip_attr,NULL,NULL,NULL);
		avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_front.line_dynamic.point_right_buf[i], point_cnt, &line_strip_attr,NULL,NULL,NULL);	  

	}
#endif
	return AVM_ERR_NONE;
}
/*******************************************************************************************
* Function Name  : avm_line_sv_back_static_draw
* Description    : 单视模式静态倒车轨迹线绘制
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_back_static_draw()
{
	int index_start = 0;
	int index_end = 0;
	int point_cnt = 0;

	int point_h1_num = 0;	  //妯嚎鐨勭偣鏁?
	int point_h2_num = 0;
	int point_h3_num = 0;
	int point_h4_num = 0;
	int point_h_step = 0;

	line_strip_attr_t line_strip_attr;

	static virtual_camera_avm_t virtual_camera_avm_paraml = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static virtual_camera_sgv_t virtual_camera_sgv_paraml;
	static view_2d_parameter_t view_2d_parameter_param[4];

	if(virtual_camera_avm_paraml.scale_pixel_mm == 0)
	{
		  avm_disp_info_get(MODE_SV, &virtual_camera_avm_paraml,&virtual_camera_sgv_paraml, view_2d_parameter_param);
	}

	
	point_h1_num = vehicle_line_obj.line_sv_back.line_static.line_point_h1_num; 
	point_h2_num = vehicle_line_obj.line_sv_back.line_static.line_point_h2_num; 
	point_h3_num = vehicle_line_obj.line_sv_back.line_static.line_point_h3_num; 
	point_h4_num = vehicle_line_obj.line_sv_back.line_static.line_point_h4_num;
	
	line_strip_attr.Blend	= 255;
	line_strip_attr.IsStrip = 1;
	line_strip_attr.win_x	   = view_2d_parameter_param[0].screen_x;
	line_strip_attr.win_y	   = view_2d_parameter_param[0].screen_y;
	line_strip_attr.win_width  = view_2d_parameter_param[0].screen_width;
	line_strip_attr.win_height = view_2d_parameter_param[0].screen_height;
	line_strip_attr.scalex = 1.0 * line_strip_attr.win_width / virtual_camera_sgv_paraml.vc_width[0];
	line_strip_attr.scaley = 1.0 * line_strip_attr.win_height / virtual_camera_sgv_paraml.vc_height[0];

#if (MODE_DIFFERENT == 1)

	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_back.line_static.line_v1_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_back.line_static.line_v1_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_back.line_static.line_v1_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_back.line_static.line_v1_attr.width;
	//avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_h1_buf,2,&line_strip_attr,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_left_h1_buf,point_h1_num,&line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_right_h1_buf,point_h1_num,&line_strip_attr,NULL,NULL,NULL);

	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_back.line_static.line_v2_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_back.line_static.line_v2_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_back.line_static.line_v2_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_back.line_static.line_v2_attr.width;
	//avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_h2_buf,2,&line_strip_attr,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_left_h2_buf,point_h2_num,&line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_right_h2_buf,point_h2_num,&line_strip_attr,NULL,NULL,NULL);

	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_back.line_static.line_v3_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_back.line_static.line_v3_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_back.line_static.line_v3_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_back.line_static.line_v3_attr.width;
	//avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_h3_buf,2,&line_strip_attr,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_left_h3_buf,point_h3_num,&line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_right_h3_buf,point_h3_num,&line_strip_attr,NULL,NULL,NULL);

	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_back.line_static.line_v4_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_back.line_static.line_v4_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_back.line_static.line_v4_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_back.line_static.line_v4_attr.width;
	//avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_h3_buf,2,&line_strip_attr,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_left_h4_buf,point_h4_num,&line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_right_h4_buf,point_h4_num,&line_strip_attr,NULL,NULL,NULL);

	point_cnt = vehicle_line_obj.line_sv_back.line_static.line_point_num;
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_left_buf,point_cnt,&line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_right_buf,point_cnt,&line_strip_attr,NULL,NULL,NULL);
#endif
#if (MODE_SAME == 1)	 
	//left
	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_back.line_static.line_v1_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_back.line_static.line_v1_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_back.line_static.line_v1_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_back.line_static.line_v1_attr.width;
	index_start = vehicle_line_obj.line_sv_back.line_static.line_v1_attr.index_start;
	index_end	= vehicle_line_obj.line_sv_back.line_static.line_v1_attr.index_end;
	point_cnt	= index_end - index_start + 1;
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_back.line_static.point_left_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_back.line_static.point_right_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_left_h1_buf,point_h1_num,&line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_right_h1_buf,point_h1_num,&line_strip_attr,NULL,NULL,NULL);
	
	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_back.line_static.line_v2_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_back.line_static.line_v2_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_back.line_static.line_v2_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_back.line_static.line_v2_attr.width;
	index_start = vehicle_line_obj.line_sv_back.line_static.line_v2_attr.index_start;
	index_end	= vehicle_line_obj.line_sv_back.line_static.line_v2_attr.index_end;
	point_cnt	= index_end - index_start + 1;
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_back.line_static.point_left_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_back.line_static.point_right_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_left_h2_buf,point_h2_num,&line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_right_h2_buf,point_h2_num,&line_strip_attr,NULL,NULL,NULL);

	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_back.line_static.line_v3_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_back.line_static.line_v3_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_back.line_static.line_v3_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_back.line_static.line_v3_attr.width;
	index_start = vehicle_line_obj.line_sv_back.line_static.line_v3_attr.index_start;
	index_end	= vehicle_line_obj.line_sv_back.line_static.line_v3_attr.index_end;
	point_cnt	= index_end - index_start + 1;
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_back.line_static.point_left_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_back.line_static.point_right_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_left_h3_buf,point_h3_num,&line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_right_h3_buf,point_h3_num,&line_strip_attr,NULL,NULL,NULL);
	
	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_back.line_static.line_v4_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_back.line_static.line_v4_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_back.line_static.line_v4_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_back.line_static.line_v4_attr.width;
	index_start = vehicle_line_obj.line_sv_back.line_static.line_v4_attr.index_start;
	index_end	= vehicle_line_obj.line_sv_back.line_static.line_v4_attr.index_end;
	point_cnt	= index_end - index_start + 1;
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_back.line_static.point_left_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_back.line_static.point_right_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_left_h4_buf,point_h4_num,&line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_static.point_right_h4_buf,point_h4_num,&line_strip_attr,NULL,NULL,NULL);
#endif
	return AVM_ERR_NONE;
}
/*******************************************************************************************
* Function Name  : avm_line_sv_back_dynamic_draw
* Description    : 单视模式动态倒车轨迹线绘制
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_back_dynamic_draw(double angle)
{
	int index_start = 0;
	int index_end = 0;
	int point_cnt = 0;
	int angle_index = 0;

	line_strip_attr_t line_strip_attr;

	static virtual_camera_avm_t virtual_camera_avm_paraml = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static virtual_camera_sgv_t virtual_camera_sgv_paraml;
	static view_2d_parameter_t view_2d_parameter_param[4];

	if(virtual_camera_avm_paraml.scale_pixel_mm == 0)
	{
		avm_disp_info_get(MODE_SV, &virtual_camera_avm_paraml,&virtual_camera_sgv_paraml, view_2d_parameter_param);
	}

	line_strip_attr.Blend	= 255;
	line_strip_attr.IsStrip = 1;
	line_strip_attr.win_x	   = view_2d_parameter_param[0].screen_x;
	line_strip_attr.win_y	   = view_2d_parameter_param[0].screen_y;
	line_strip_attr.win_width  = view_2d_parameter_param[0].screen_width;
	line_strip_attr.win_height = view_2d_parameter_param[0].screen_height;
	line_strip_attr.scalex = 1.0 * line_strip_attr.win_width / virtual_camera_sgv_paraml.vc_width[0];
	line_strip_attr.scaley = 1.0 * line_strip_attr.win_height / virtual_camera_sgv_paraml.vc_height[0];
		 
	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_back.line_dynamic.line_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_back.line_dynamic.line_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_back.line_dynamic.line_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_back.line_dynamic.line_attr.width;

	point_cnt = vehicle_line_obj.line_sv_back.line_dynamic.line_point_num;

	angle_index = angle + ANGLE_MAX_ABS;
	if(angle_index > ANGLE_MAX_ABS * 2)
	{
	   angle_index = ANGLE_MAX_ABS * 2;
	}
	else if(angle_index < 0)
	{
	   angle_index = 0;
	}

	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_dynamic.point_left_buf[angle_index],point_cnt, &line_strip_attr,NULL,NULL,NULL);
	avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_dynamic.point_right_buf[angle_index], point_cnt,	&line_strip_attr,NULL,NULL,NULL);   

#if(BACK_DYNAMIC_LINE_END_CONNECTION == 1)
	point_t point_h_buf[2];
	point_h_buf[0].x = vehicle_line_obj.line_sv_back.line_dynamic.point_left_buf[angle_index][point_cnt - 1].x - 2;
	point_h_buf[0].y = vehicle_line_obj.line_sv_back.line_dynamic.point_left_buf[angle_index][point_cnt - 1].y;
	point_h_buf[1].x = vehicle_line_obj.line_sv_back.line_dynamic.point_right_buf[angle_index][point_cnt - 1].x + 2;
	point_h_buf[1].y = vehicle_line_obj.line_sv_back.line_dynamic.point_right_buf[angle_index][point_cnt - 1].y;
	avmlib_Draw_Lines_Strip(point_h_buf, 2, &line_strip_attr,NULL,NULL,NULL); 
#endif

#if 0
	int i = 0;
	line_strip_attr.line_w = 1;
	for(i = 0; i < ANGLE_MAX_ABS*2; i++)
	{
	  
		avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_dynamic.point_left_buf[i], point_cnt, &line_strip_attr,NULL,NULL,NULL);
		avmlib_Draw_Lines_Strip(vehicle_line_obj.line_sv_back.line_dynamic.point_right_buf[i], point_cnt,  &line_strip_attr,NULL,NULL,NULL);  
	 }
#endif
	  return AVM_ERR_NONE;
}
/*******************************************************************************************
* Function Name  : avm_line_sv_left_draw
* Description    : 单视模式左侧辅助线绘制
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_left_draw()
{
	int index_start = 0;
	int index_end = 0;
	int point_cnt = 0;

	line_strip_attr_t line_strip_attr;

	static virtual_camera_avm_t virtual_camera_avm_paraml = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static virtual_camera_sgv_t virtual_camera_sgv_paraml;
	static view_2d_parameter_t view_2d_parameter_param[4];

	if(virtual_camera_avm_paraml.scale_pixel_mm == 0)
	{
		avm_disp_info_get(MODE_SV, &virtual_camera_avm_paraml,&virtual_camera_sgv_paraml, view_2d_parameter_param);
	}

	line_strip_attr.Blend	= 255;
	line_strip_attr.IsStrip = 1;
	line_strip_attr.win_x	   = view_2d_parameter_param[0].screen_x;
	line_strip_attr.win_y	   = view_2d_parameter_param[0].screen_y;
	line_strip_attr.win_width  = view_2d_parameter_param[0].screen_width;
	line_strip_attr.win_height = view_2d_parameter_param[0].screen_height;
	line_strip_attr.scalex = 1.0 * line_strip_attr.win_width / virtual_camera_sgv_paraml.vc_width[0];
	line_strip_attr.scaley = 1.0 * line_strip_attr.win_height / virtual_camera_sgv_paraml.vc_height[0];

	//near 
	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_left.line_side.line_h_near_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_left.line_side.line_h_near_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_left.line_side.line_h_near_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_left.line_side.line_h_near_attr.width;
	index_start = vehicle_line_obj.line_sv_left.line_side.line_h_near_attr.index_start;
	index_end	= vehicle_line_obj.line_sv_left.line_side.line_h_near_attr.index_end;
	point_cnt	= index_end - index_start + 1;
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_left.line_side.point_left_near_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL);

	//far
	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_left.line_side.line_h_far_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_left.line_side.line_h_far_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_left.line_side.line_h_far_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_left.line_side.line_h_far_attr.width;
	index_start = vehicle_line_obj.line_sv_left.line_side.line_h_far_attr.index_start;
	index_end	= vehicle_line_obj.line_sv_left.line_side.line_h_far_attr.index_end;
	point_cnt	= index_end - index_start + 1;
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_left.line_side.point_left_far_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL);

	//up
	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_left.line_side.line_v_up_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_left.line_side.line_v_up_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_left.line_side.line_v_up_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_left.line_side.line_v_up_attr.width;
	index_start = vehicle_line_obj.line_sv_left.line_side.line_v_up_attr.index_start;
	index_end	= vehicle_line_obj.line_sv_left.line_side.line_v_up_attr.index_end;
	point_cnt	= index_end - index_start + 1;
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_left.line_side.point_left_up_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL);

	//down
	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_left.line_side.line_v_down_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_left.line_side.line_v_down_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_left.line_side.line_v_down_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_left.line_side.line_v_down_attr.width;
	index_start = vehicle_line_obj.line_sv_left.line_side.line_v_down_attr.index_start;
	index_end	= vehicle_line_obj.line_sv_left.line_side.line_v_down_attr.index_end;
	point_cnt	= index_end - index_start + 1;
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_left.line_side.point_left_down_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL);
	return AVM_ERR_NONE;
}
/*******************************************************************************************
* Function Name  : avm_line_sv_right_draw
* Description    : 单视模式右侧辅助线绘制
* Arguments	     : none								
* Return		 : AVM_ERR_NONE successful.
* 				   Other			 Other AVM_ERR_xxx	 fail.
* Note(s) 	     : none
*******************************************************************************************/

static AVM_ERR avm_line_sv_right_draw()
{
	int index_start = 0;
	int index_end = 0;
	int point_cnt = 0;

	line_strip_attr_t line_strip_attr;

	static virtual_camera_avm_t virtual_camera_avm_paraml = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static virtual_camera_sgv_t virtual_camera_sgv_paraml;
	static view_2d_parameter_t view_2d_parameter_param[4];

	if(virtual_camera_avm_paraml.scale_pixel_mm == 0)
	{
		avm_disp_info_get(MODE_SV, &virtual_camera_avm_paraml,&virtual_camera_sgv_paraml, view_2d_parameter_param);
	}

	line_strip_attr.Blend	= 255;
	line_strip_attr.IsStrip = 1;
	line_strip_attr.win_x	   = view_2d_parameter_param[0].screen_x;
	line_strip_attr.win_y	   = view_2d_parameter_param[0].screen_y;
	line_strip_attr.win_width  = view_2d_parameter_param[0].screen_width;
	line_strip_attr.win_height = view_2d_parameter_param[0].screen_height;
	line_strip_attr.scalex = 1.0 * line_strip_attr.win_width / virtual_camera_sgv_paraml.vc_width[0];
	line_strip_attr.scaley = 1.0 * line_strip_attr.win_height / virtual_camera_sgv_paraml.vc_height[0];

	//near
	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_right.line_side.line_h_near_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_right.line_side.line_h_near_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_right.line_side.line_h_near_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_right.line_side.line_h_near_attr.width;
	index_start = vehicle_line_obj.line_sv_right.line_side.line_h_near_attr.index_start;
	index_end	= vehicle_line_obj.line_sv_right.line_side.line_h_near_attr.index_end;
	point_cnt	= index_end - index_start + 1;
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_right.line_side.point_right_near_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL);

	//far
	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_right.line_side.line_h_far_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_right.line_side.line_h_far_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_right.line_side.line_h_far_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_right.line_side.line_h_far_attr.width;
	index_start = vehicle_line_obj.line_sv_right.line_side.line_h_far_attr.index_start;
	index_end	= vehicle_line_obj.line_sv_right.line_side.line_h_far_attr.index_end;
	point_cnt	= index_end - index_start + 1;
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_right.line_side.point_right_far_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL);

	//up
	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_right.line_side.line_v_up_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_right.line_side.line_v_up_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_right.line_side.line_v_up_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_right.line_side.line_v_up_attr.width;
	index_start = vehicle_line_obj.line_sv_right.line_side.line_v_up_attr.index_start;
	index_end	= vehicle_line_obj.line_sv_right.line_side.line_v_up_attr.index_end;
	point_cnt	= index_end - index_start + 1;
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_right.line_side.point_right_up_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL);

	//down
	line_strip_attr.linecolor.red	= vehicle_line_obj.line_sv_right.line_side.line_v_down_attr.r;
	line_strip_attr.linecolor.green = vehicle_line_obj.line_sv_right.line_side.line_v_down_attr.g;
	line_strip_attr.linecolor.blue	= vehicle_line_obj.line_sv_right.line_side.line_v_down_attr.b;
	line_strip_attr.line_w = vehicle_line_obj.line_sv_right.line_side.line_v_down_attr.width;
	index_start = vehicle_line_obj.line_sv_right.line_side.line_v_down_attr.index_start;
	index_end	= vehicle_line_obj.line_sv_right.line_side.line_v_down_attr.index_end;
	point_cnt	= index_end - index_start + 1;
	avmlib_Draw_Lines_Strip(&vehicle_line_obj.line_sv_right.line_side.point_right_down_buf[index_start], point_cnt, &line_strip_attr,NULL,NULL,NULL); 
	return AVM_ERR_NONE;
}

