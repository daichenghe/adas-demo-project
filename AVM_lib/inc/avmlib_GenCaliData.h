/*******************************************************************************
* File Name          : avmlib_gpu_drawing.h
* Author             : Jerry
* Revision           : 2.1
* Date               : 15/07/2016
* Description        : Library Interface 
*******************************************************************************/


#ifndef __AVM_GENCALIDATA_H__
#define __AVM_GENCALIDATA_H__
#include "avmlib_sdk.h"

/* 环视LUT参数 */
typedef struct virtual_camera_avm_
{
    S32 vc_height;//虚拟相机高度
    S32 vc_width; //虚拟相机宽度
    S32 srcW;     //原始图宽度
    S32 srcH;     //原始图高度
    S32 tri_x_div;/* opengl三角形x跨度 */
    S32 tri_y_div;/* opengl三角形y跨度 */
    S32 OFRONTY;  /* 前相机y方向的偏移 */
    S32 OLEFTX;   /* 右相机x方向的偏移 */
    S32 ORIGHTX;  /* 前相机y方向的偏移 */
    S32 OBACKY;   /* 左相机x方向的偏移 */
    S32 BLEND_ANGLE_H;/* alpha结束角度 (0-90)*/
    S32 BLEND_ANGLE_M;/* alpha过度角度 (0-90)*/
    S32 BLEND_ANGLE_L;/* alpha起始角度 (0-90)*/
    S32 REAR_BLEND_ANGLE_H;/* alpha结束角度 (0-90)*/
    S32 REAR_BLEND_ANGLE_M;/* alpha过度角度 (0-90)*/
    S32 REAR_BLEND_ANGLE_L;/* alpha起始角度 (0-90)*/
    F32 scale_pixel_mm;//缩放系数(一个像素代表实际的距离 eg. 23表示1个像素代表实际距离23mm)
} virtual_camera_avm_t;//环视LUT参数

/* 虚拟相机参数使用标志 */
typedef enum VCAMPARAM_FLAG
{
    VCAMPARAM_ABS  = 0,  /* 直接赋值 */
    VCAMPARAM_DEF ,      /* 默认值 */
    VCAMPARAM_OFFSET,    /* 偏移值 */
} vcamparam_flag_t;

/* 单视野LUT参数 */
typedef struct VirtualCameraParam_{
		double alpha_c;//skew coefficient
		double gammax;//general focus oin x direction.
		double gammay;//general focus oin y direction.
		double cx;//principle point in x direction
		double cy;//principle point in y direction
		double kc[5];//distortion[5]
		double xi;//
		double stretch;
		double R[3][3];////R,T are transformation matrix from virtual camera's CS to vehicle's CS: Xvt=R*Xcam+T
		double T[3];////R,T are transformation matrix from virtual camera's CS to vehicle's CS: Xvt=R*Xcam+T
		int IMG_WIDTH;
		int IMG_HEIGHT;

}VirtualCameraParam_t;

typedef struct virtual_camera_sv_
{
    S32 vc_height[4];   //虚拟相机高度
    S32 vc_width[4];    //虚拟相机宽度
    S32 srcW;           //原始图宽度
    S32 srcH;           //原始图高度
    S32 tri_x_div; /* opengl三角形x跨度 */
    S32 tri_y_div; /* opengl三角形y跨度 */
    S32 mirror[4]; /* 是否左右镜像 1:镜像 0:不镜像*/
    vcamparam_flag_t vmode[4];/* 参数使用方式 */
    F64 fovx_minus[4];/* 视点平面X负方向视野角度 */
    F64 fovx_plus[4]; /* 视点平面X正方向视野角度 */
    F64 fovy_minus[4];/* 视点平面Y负方向视野角度 */
    F64 fovy_plus[4]; /* 视点平面Y正方向视野角度 */
    F64 pose[4][6];   /* 虚拟相机视点参数，包括位置[x][0-2]，方向[x][3-5],其中位置不可修改*/
    F64 xi_ratio[4];  /* 鱼眼系数 */
    F64 stretch[4];   /* 拉伸系数 */
    VirtualCameraParam_t puVC[4];/*puVC:返回用户虚拟相机，puVC为元素个数为4的数组首元素指针
                              eg. VirtualCameraParam VirtualCamera[4];*/
} virtual_camera_sgv_t;//单个摄像头LUT参数
//3D


typedef struct virtual_camera_avm3d_
{
	S32 vc_width;
	S32 vc_height;
    S32 srcW;           //原始图宽度
    S32 srcH;           //原始图高度
    S32 tri_div_x;        /* 三角形纵向跨度 */
    S32 tri_div_y;        /* 三角形纵向跨度 */
    S32 front_angle0; //前右融合起始角度
    S32 front_angle1; //前右融合角度
    S32 front_angle2; //前左融合起始角度
    S32 front_angle3; //前左融合角度
	S32 rear_angle0;  //后右融合起始角度
    S32 rear_angle1;  //后右融合角度
    S32 rear_angle2;  //后左融合起始角度
    S32 rear_angle3;  //后左融合角度
    S32 OFRONTY;  /* 前相机y方向的偏移 */
    S32 OLEFTX;   /* 右相机x方向的偏移 */
    S32 ORIGHTX;  /* 前相机y方向的偏移 */
    S32 OBACKY;   /* 左相机x方向的偏移 */
    F32 r_begin;      //模型起始半径 x * V_width
    F32 r_bottom;     //模型结束半径 x * V_length
    F32 r_torus;      //倒角圆半径
    F32 center_x;     //模型中心x  设为0
    F32 center_y;     //模型中心y  x * V_length
    F32 res_angle;    //角度分辨率
    F32 res_r;        //径向分辨率 x * V_width
    F32 alpha_wall;   //倒角
    F32 limit_wall;   //壁高 x * V_length
    F32 dx_dy_ellipse;//模型的短半径/长半径
	F32 alpha_scale_mm;//alpha表的分辨率
    
} virtual_camera_avm3d_t;//环视LUT参数

//cali
typedef struct virtual_camera_
{
    virtual_camera_avm3d_t *pavm3d;//3D环视lut参数
    virtual_camera_avm_t *pavm;//环视lut参数
    virtual_camera_sgv_t *psgv;//单视野lut参数
    C8 *lut_savepath;//lut_savepath为vbo数据保存路径
                     //如果lut_savepath为NULL,则vbo数据不保存
} lut_parameter_t;//LUT参数

typedef struct
{
    /* camera 前 */
    S32 cam0_left_sx; //左边色块的位置区域
    S32 cam0_left_sy;
    S32 cam0_left_ex;
    S32 cam0_left_ey;

    S32 cam0_right_sx;//右边色块的位置区域
    S32 cam0_right_sy;
    S32 cam0_right_ex;
    S32 cam0_right_ey;

    S32 cam0_left_area_min;//色块最大最小面积，暂未使用
    S32 cam0_left_area_max;
    S32 cam0_right_area_min;
    S32 cam0_right_area_max;

    /* camera 右 */
    S32 cam1_left_sx;
    S32 cam1_left_sy;
    S32 cam1_left_ex;
    S32 cam1_left_ey;

    S32 cam1_right_sx;
    S32 cam1_right_sy;
    S32 cam1_right_ex;
    S32 cam1_right_ey;

    S32 cam1_left_area_min;
    S32 cam1_left_area_max;
    S32 cam1_right_area_min;
    S32 cam1_right_area_max;

    /* camera 后 */
    S32 cam2_left_sx;
    S32 cam2_left_sy;
    S32 cam2_left_ex;
    S32 cam2_left_ey;

    S32 cam2_right_sx;
    S32 cam2_right_sy;
    S32 cam2_right_ex;
    S32 cam2_right_ey;

    S32 cam2_left_area_min;
    S32 cam2_left_area_max;
    S32 cam2_right_area_min;
    S32 cam2_right_area_max;

    /* camera 左 */
    S32 cam3_left_sx;
    S32 cam3_left_sy;
    S32 cam3_left_ex;
    S32 cam3_left_ey;

    S32 cam3_right_sx;
    S32 cam3_right_sy;
    S32 cam3_right_ex;
    S32 cam3_right_ey;

    S32 cam3_left_area_min;
    S32 cam3_left_area_max;
    S32 cam3_right_area_min;
    S32 cam3_right_area_max;

    S32 cam0_left_rot_angle;   //相机中左侧色版旋转角度 逆时针为正
    S32 cam0_right_rot_angle;  //相机中右侧色版旋转角度 顺时针为负

    S32 cam1_left_rot_angle;
    S32 cam1_right_rot_angle;

    S32 cam2_left_rot_angle;
    S32 cam2_right_rot_angle;

    S32 cam3_left_rot_angle;
    S32 cam3_right_rot_angle;

} range_info_t;

typedef struct
{
    F32 flrotanglescale;//前摄像头左侧色块旋转角度系数
    F32 frrotanglescale;//前摄像头右侧色块旋转角度系数
    F32 rlrotanglescale;//右摄像头左侧色块旋转角度系数
    F32 rrrotanglescale;//右摄像头右侧色块旋转角度系数
    F32 blrotanglescale;//后摄像头左侧色块旋转角度系数
    F32 brrotanglescale;//后摄像头右侧色块旋转角度系数
    F32 llrotanglescale;//左摄像头左侧色块旋转角度系数
    F32 lrrotanglescale;//左摄像头右侧色块旋转角度系数
} rotangle_t;

typedef struct
{
    F32  l_r_luminancemin ;    //左摄像头右侧色块最低亮度
    F32  l_l_luminancemin ;    //左摄像头左侧色块最低亮度
    F32  r_r_luminancemin ;    //右摄像头右侧色块最低亮度
    F32  r_l_luminancemin ;    //右摄像头左侧色块最低亮度
    F32  l_r_luminancescale;   //左摄像头右侧色块平均亮度系数
    F32  l_l_luminancescale;   //左摄像头左侧色块平均亮度系数
    F32  r_r_luminancescale;   //右摄像头右侧色块平均亮度系数
    F32  r_l_luminancescale;   //右摄像头左侧色块平均亮度系数
    F32  l_r_luminancemin_s;   //左摄像头右侧色块最低亮度（第二次精选）
    F32  l_l_luminancemin_s;   //左摄像头左侧色块最低亮度（第二次精选）
    F32  r_r_luminancemin_s;   //右摄像头右侧色块最低亮度（第二次精选）
    F32  r_l_luminancemin_s;   //右摄像头左侧色块最低亮度（第二次精选）
    F32  l_r_luminancescale_s; //左摄像头右侧色块平均亮度系数（第二次精选）
    F32  l_l_luminancescale_s; //左摄像头左侧色块平均亮度系数（第二次精选）
    F32  r_r_luminancescale_s; //右摄像头右侧色块平均亮度系数（第二次精选）
    F32  r_l_luminancescale_s; //右摄像头左侧色块平均亮度系数（第二次精选）

    F32  f_l_luminancemin ;   //前摄像头左侧色块最低亮度
    F32  f_r_luminancemin ;   //前摄像头右侧色块最低亮度
    F32  b_l_luminancemin ;   //后摄像头左侧色块最低亮度
    F32  b_r_luminancemin ;   //后摄像头右侧色块最低亮度
    F32  f_l_luminancescale;   //前摄像头左侧色块平均亮度系数
    F32  f_r_luminancescale;   //前摄像头右侧色块平均亮度系数
    F32  b_l_luminancescale;   //后摄像头左侧色块平均亮度系数
    F32  b_r_luminancescale;   //后摄像头右侧色块平均亮度系数
    F32  f_l_luminancemin_s;   //前摄像头左侧色块最低亮度（第二次精选）
    F32  f_r_luminancemin_s;   //前摄像头右侧色块最低亮度（第二次精选）
    F32  b_l_luminancemin_s;   //后摄像头左侧色块最低亮度（第二次精选）
    F32  b_r_luminancemin_s;   //后摄像头右侧色块最低亮度（第二次精选）
    F32  f_l_luminancescale_s; //前摄像头左侧色块平均亮度系数（第二次精选）
    F32  f_r_luminancescale_s; //前摄像头右侧色块平均亮度系数（第二次精选）
    F32  b_l_luminancescale_s; //后摄像头左侧色块平均亮度系数（第二次精选）
    F32  b_r_luminancescale_s; //后摄像头右侧色块平均亮度系数（第二次精选）
} luminance_t;

typedef struct calibration_attr_cfg_
{
 int   grid_dx;        //色块宽度的一半mm
 int   grid_dy;        //色块高度的一半mm
 int   grid_dist_v;    //车辆左侧或右侧两标定板之间距离mm
 int   grid_dist_h;    //车辆前侧或右侧两标定板之间距离mm
 int   isautocalib;    //标定模式(0:2D 4:3D)
 int   dist_border;    //色块到标定板边沿的距离mm
} cali_attr_cfg_t; //标定配置

typedef struct
{
    S32 video_w;//图像宽度
    S32 video_h;//图像高度
    luminance_t  luminance;  //亮度参数
    rotangle_t   rotangle;   //角度参数
    range_info_t range_info; //区域参数
    cali_attr_cfg_t cali_cfg ;//标定配置
} cali_parame_t; //自动标定参数

typedef struct cali_cameraParam_
{
    F64 Xw;//camera's x coordinates in the big world CS
    F64 Yw;//camera's y coordinates in the big world CS
    F64 Zw;//camera's z coordinates in the big world CS
} avm_cameraParam_t;

typedef struct vehicle_information_
{
    S32   Veh_width;//车宽
    S32   Veh_length;//车长
    avm_cameraParam_t paramLeft;//左相机信息
    avm_cameraParam_t paramRight;//右相机信息
    avm_cameraParam_t paramFront;//前相机信息
    avm_cameraParam_t paramBack;//后相机信息
    F64 Tgw[3];//vehicle translation vector (Tgw) in world CS
} avm_vehicle_info_t;

AVM_ERR avmlib_Get_Vehicle_Info(avm_vehicle_info_t *pvehicle_info);
AVM_ERR  avmlib_init_SDASlib(const U8 *const lut_file_path, const cali_attr_cfg_t *const cali_cfg);
AVM_ERR  avmlib_getVCImageCoordFromVehicleCoord(const VirtualCameraParam_t *const pVC, const F64 *const Coord_v, S32 *Coord_vc);
AVM_ERR avmlib_getImageCoordFromVehicleCoord(const int camera_id, const F64 *const Coord_v, F64 *camera_x,F64 *camera_y);

#endif /* AUTOCALI_H_ */
