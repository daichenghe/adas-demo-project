/**********************************************************************************
* File Name          : avm_config.h
* Author             : Daich
* Revision           : 2.1
* Date               : 03/07/2018
* Description        : system configuration file
*
* HISTORY***********************************************************************
* 03/07/2018 | sDas Basic 2.0                                             | Daich
*
*******************************************************************************/

#ifndef __AVM_CONFIG__H__
#define __AVM_CONFIG__H__


 /******************HW_CFG*********************/

//#define DbgPrintf printf
#define DbgPrintf //

 
#define CAM_WIDTH               (1280)
#define CAM_HEIGHT              (720)
#define CAM_COUNT               (4)
#define SCREEN_WINDOW_WIDTH	    (1024)
#define SCREEN_WINDOW_HEIGHT	(600)

#define DMAFD_W 2560
#define DMAFD_H 720

#define AVM_INPUT_INVALID	(0)
#define AVM_INPUT_VALID		(1)

#define WHEELTREAD              (2120  ) //轮距(轴长)
#define WHEELBASE               (2738 ) //轴距
#define V_D_REAR				(0)
#define V_D_FRONT				(500)

#define SERIAL_COM 			 "/dev/ttyS4" //����1

#define IMAGE_DEBUG

#define MASS_ENABLE 
 /******************END HW*****************/
#define CALI_MODE  0   //(3D:4 2D:0)
 /******************APP_CFG*********************/
#define AVM_CFG_2D              1u   /* Enable (1) or Disable (0) */
#define AVM_CFG_3D              1u   /* Enable (1) or Disable (0) */
#define AVM_CFG_PRE             1u
#define AVM_CFG_BOTHSV          0u
#define AVM_CFG_SF              0u
#define AVM_CFG_SVBS            0u
#define AVM_CFG_FREE            1u

#define DEFAULT_MODE            1

 /******************MEDIA_PATH_CFG*******************/
#define  VEH_2D_NAME         "media/car.png"

#define  VEHL1_2D_NAME        "media/car.png"
#define  VEHR1_2D_NAME        "media/car.png"
#define  VEHB1_2D_NAME        "media/car.png"

#define  VEHL2_2D_NAME        "media/car.png"
#define  VEHR2_2D_NAME        "media/car.png"
#define  VEHB2_2D_NAME        "media/car.png"

#define  VEHL3_2D_NAME        "media/car.png"
#define  VEHR3_2D_NAME        "media/car.png"
#define  VEHB3_2D_NAME        "media/car.png"

#define  AVM_IMG_CORRECT      "media/y_balance.bmp"

#define  VEH_3D_NAME          "media/1.AnalysisFile"
#define  CUBE_3D_NAME         "media/1_1.AnalysisFile"

#define  VEH_3D_OBJ           "media/car12.obj"
#define  CHASSIS_3D_OBJ       "media/cube.obj"

#define  TIRE1_3D_OBJ         "media/tire1.obj"
#define  TIRE2_3D_OBJ         "media/tire2.obj"
#define  TIRE3_3D_OBJ         "media/tire3.obj"
#define  TIRE4_3D_OBJ         "media/tire4.obj"


 /******************END MEDIA*******************/

 /******************LUT_PATH_CFG*******************/
#define  VBO_LUT_PATH        "lut/"
#define  VBO_LUT             "lut/LUT.bin"

#define  VBO_3D_SF           "lut/vbo_sf.bin"

#define  VBO_2D_TOPVIEW      "lut/vbo_topview.bin"


#define  VBO_ORI_VIEW        "lut/vbo_ori.bin"//名字vbo_ori.bin不可以修�?

#define  VBO_VEH_VIEW        "lut/vbo_veh.bin"

#define  VBO_2D_LARGEVIEW    "lut/vbo_largeview.bin"

#define  VBO_2D_LARGEVIEWBS  "lut/vbo_bsblargeview.bin"

#define  VBO_2D_FSIDEVIEW    "lut/vbo_fsideview.bin"

#define  VBO_2D_BOTHTOPVIEW    "lut/vbo_bothtview.bin"

#define  VBO_2D_PERVIEW      "lut/vbo_periscope.bin"

#define  VBO_2D_BACKUP       "lut/vbo_backup.bin"

#define  VBO_3D_VIEW         "lut/vbo_point.bin"
#define  VBO_3D_VIEW1        "lut/vbo_point1.bin"
#define  VBO_3D_VIEW2        "lut/vbo_point2.bin"
#define  VBO_3D_VIEW3        "lut/vbo_point3.bin"
#define  VBO_3D_VIEW4        "lut/vbo_point4.bin"
#define  VBO_3D_VIEW5        "lut/vbo_point5.bin"
#define  VBO_3D_VIEW6        "lut/vbo_point6.bin"
#define  VBO_3D_VIEW7        "lut/vbo_point7.bin"
#define  VBO_3D_VIEW8        "lut/vbo_point8.bin"
#define  VBO_3D_VIEW9        "lut/vbo_point9.bin"
#define  VBO_3D_VIEW10       "lut/vbo_point10.bin"
#define  VBO_3D_VIEW11       "lut/vbo_point11.bin"
#define  VBO_3D_VIEW12       "lut/vbo_point12.bin"
#define  VBO_3D_VIEW13       "lut/vbo_point13.bin"
#define  VBO_3D_VIEW14       "lut/vbo_point14.bin"
#define  VBO_3D_VIEW15       "lut/vbo_point15.bin"
#define  VBO_3D_VIEW16       "lut/vbo_point15.bin"
#define  VBO_3D_VIEW17       "lut/vbo_point16.bin"
#define  VBO_3D_VIEW18       "lut/vbo_point16.bin"

 /******************END LUT*******************/

 /******************SHADER_PATH_CFG*******************/
#ifndef T3BOARD
#define CUBESHADER_VERT       "shaders_pc/chassisshader.vert"
#define CUBESHADER_FRAG       "shaders_pc/chassisshader.frag"

#define VEHICLESHADER_VERT    "shaders_pc/vehicleshader.vert"
#define VEHICLESHADER_FRAG    "shaders_pc/vehicleshader.frag"

#define VEHICLESHADER2D_VERT   "shaders_pc/vehicleshader-2d.vert"
#define VEHICLESHADER2D_FRAG   "shaders_pc/vehicleshader-2d.frag"

#define AVM_3D_VERT           "shaders_pc/avm_3d.vert"
#define AVM_3D_FRAG           "shaders_pc/avm_3d.frag"

#define AVM_2D_VERT           "shaders_pc/avm_2d.vert"
#define AVM_2D_FRAG           "shaders_pc/avm_2d.frag"

#define AVM_SV_VERT           "shaders_pc/avm_sv.vert"
#define AVM_SV_FRAG           "shaders_pc/avm_sv.frag"

#define AVM_CAL_VERT          "shaders_pc/avm_cal.vert"
#define AVM_CAL_FRAG          "shaders_pc/avm_cal.frag"

#define AVM_LINE_VERT         "shaders_pc/avm_line.vert"
#define AVM_LINE_FRAG         "shaders_pc/avm_line.frag"

#define AVM_UI_VERT           "shaders_pc/avm_ui.vert"
#define AVM_UI_FRAG           "shaders_pc/avm_ui.frag"

#else
	
#define CUBESHADER_VERT       "shaders/chassisshader.vert"
#define CUBESHADER_FRAG       "shaders/chassisshader.frag"

#define VEHICLESHADER_VERT    "shaders/vehicleshader.vert"
#define VEHICLESHADER_FRAG    "shaders/vehicleshader.frag"

#define VEHICLESHADER2D_VERT   "shaders/vehicleshader-2d.vert"
#define VEHICLESHADER2D_FRAG   "shaders/vehicleshader-2d.frag"

#define AVM_3D_VERT           "shaders/avm_3d.vert"
#define AVM_3D_FRAG           "shaders/avm_3d.frag"

#define AVM_2D_VERT           "shaders/avm_2d.vert"
#define AVM_2D_FRAG           "shaders/avm_2d.frag"

#define AVM_SV_VERT           "shaders/avm_sv.vert"
#define AVM_SV_FRAG           "shaders/avm_sv.frag"

#define AVM_CAL_VERT          "shaders/avm_cal.vert"
#define AVM_CAL_FRAG          "shaders/avm_cal.frag"

#define AVM_LINE_VERT         "shaders/avm_line.vert"
#define AVM_LINE_FRAG         "shaders/avm_line.frag"

#define AVM_UI_VERT           "shaders/avm_ui.vert"
#define AVM_UI_FRAG           "shaders/avm_ui.frag"
/******************END SHADER*******************/

#endif

/******************LINE_CFG*******************/
/* 辅助线开�?*/
#if (AVM_CFG_2D > 0)
//#define _SV_REAR_TRACE_LINE_ENABLED_    /* 环视后轨迹线 */
//#define SV_TRACKLINE_POINT_COUNT         (40)   /* 倒车轨迹线一侧所画点的个�?*/
//#define _SV_REAR_AID_LINE_ENABLED_      /* 环视后车幅线 */

#define _RV_REAR_BACK_LINE_ENABLED_     /* 右视图的倒车�?*/
#define _RV_REAR_TRACE_LINE_ENABLED_      /* 右视图的轨迹�?*/
#define _RV_SIDE_AID_LINE_ENABLED_      /* 右视侧边车幅�?  */
#define _RV_FRONT_LINE_ENABLED_
#define _SVFS_SIDE_AID_LINE_ENABLED_      /* 右视侧边车幅�?  */

#ifdef _RV_REAR_BACK_LINE_ENABLED_
#define  RV_BACKLINE_POINT_COUNT         (40)   /* 倒车轨迹线一侧所画点的个�?*/
#define  RV_BACKLINE_POINT_START         (250)  /* 倒车轨迹线起始位�?mm(距离后相机多少mm出开�? */
#define  RV_BACKLINE_POINT_DIST          (100)  /* 倒车轨迹线所画点之间距离 mm
                                                   eg.倒车线长�?125*(40-1)=4.875m */
#define  RV_BACKLINE_MARK_LEN            (500)  /* 横向标度线间�?*/
#define  RV_BACKLINE_MARK_DIST           (0.13)  /* 横向标度线长度比�?*/
#define  RV_BACKLINE_MARK_CNT            ((RV_BACKLINE_POINT_COUNT-1) * RV_BACKLINE_POINT_DIST/RV_BACKLINE_MARK_LEN-2)  /* 横向标度线个�?*/
#endif
#ifdef _RV_REAR_TRACE_LINE_ENABLED_
#define  RV_TRACKLINE_ANGLE              (40)   /* 随动轨迹线一侧角�?*/
#define  RV_BACKLINE_POINT_TRACK_COUNT   (40)   /* 倒车轨迹线一侧所画点的个�?*/
#define  RV_BACKLINE_POINT_TRACK_START   (250)  /* 倒车轨迹线起始位�?mm(距离后相机多少mm出开�? */
#define  RV_BACKLINE_POINT_TRACK_DIST    (100)  /* 倒车轨迹线所画点之间距离 mm
                                                    eg.倒车线长�?125*(40-1)=4.875m */
#endif
#ifdef _RV_FRONT_LINE_ENABLED_
#define RV_FRONT_LINE_POINT_COUNT		(40)
#define RV_FRONT_LINE_POINT_START		(0)
#define RV_FRONT_LINE_POINT_DIST		(100)
#endif

#endif

#define BUV_REAR_BACK_LINE_ENABLED_      /* 倒车视野倒车�?*/
//#define BUV_REAR_TRACE_LINE_ENABLED_     /* 倒车视野轨迹�?*/

#define AVM_2D_LINE_ENABLED_
#define AVM_2D_REAR_TRACE_LINE_ENABLED_
#define AVM_2D_FRONT_LINE_ENABLED_
#define AVM_2D_FRONT_BACKUP_LINE_ENABLED_

#ifdef BUV_REAR_BACK_LINE_ENABLED_
#define  BUV_BACKLINE_POINT_COUNT         (40)   /* 倒车轨迹线一侧所画点的个�?*/
#define  BUV_BACKLINE_POINT_START         (100)  /* 倒车轨迹线起始位�?mm(距离后相机多少mm出开�? */
#define  BUV_BACKLINE_POINT_DIST          (100)  /* 倒车轨迹线所画点之间距离 mm
                                                   eg.倒车线长�?125*(40-1)=4.875m */
#define  BUV_BACKLINE_MARK_LEN            (500)  /* 横向标度线间�?*/
#define  BUV_BACKLINE_MARK_DIST           (0.13)  /* 横向标度线长度比�?*/
#define  BUV_BACKLINE_MARK_CNT            ((BUV_BACKLINE_POINT_COUNT-1) * BUV_BACKLINE_POINT_DIST/BUV_BACKLINE_MARK_LEN-2)  /* 横向标度线个�?*/
#endif

#ifdef BUV_REAR_TRACE_LINE_ENABLED_
#define  BUV_TRACKLINE_ANGLE              (30)   /* 随动轨迹线一侧角�?*/
#define  BUV_BACKLINE_POINT_TRACK_COUNT   (40)   /* 倒车轨迹线一侧所画点的个�?*/
#define  BUV_BACKLINE_POINT_TRACK_START   (100)  /* 倒车轨迹线起始位�?mm(距离后相机多少mm出开�? */
#define  BUV_BACKLINE_POINT_TRACK_DIST    (100)  /* 倒车轨迹线所画点之间距离 mm
                                                    eg.倒车线长�?125*(40-1)=4.875m */
#endif
#ifdef AVM_2D_LINE_ENABLED_
#define AVM_2D_LINE_ANGLE	(40)
#ifdef AVM_2D_REAR_TRACE_LINE_ENABLED_
#define AVM_2D_TRACKLINE_ANGLE	AVM_2D_LINE_ANGLE
#define AVM_2D_TRACE_LINE_POINT_COUNT	(40)

#endif
#ifdef AVM_2D_FRONT_LINE_ENABLED_
#define AVM_2D_FRONT_LINE_POINT_COUNT	(40)

#endif
#ifdef AVM_2D_FRONT_BACKUP_LINE_ENABLED_
#define AVM_2D_FRONT_BACKUP_LINE_POINT_COUNT	(40)
#define AVM_2D_FRONT_BACKUP_LINE_ANGLE		AVM_2D_LINE_ANGLE
#endif
#endif

/******************END_LINE_CFG*******************/

/******************calibration CFG*********************/
#define CAM0_LEFT_SX        50
#define CAM0_LEFT_EX        500
#define CAM0_LEFT_SY        140
#define CAM0_LEFT_EY        500

#define CAM0_RIGHT_SX       CAM_WIDTH-CAM_WIDTH/2+200
#define CAM0_RIGHT_EX       CAM_WIDTH-11
#define CAM0_RIGHT_SY       140
#define CAM0_RIGHT_EY       500

/* camera 1 */
#define CAM1_LEFT_SX        20
#define CAM1_LEFT_EX        500
#define CAM1_LEFT_SY        140
#define CAM1_LEFT_EY        500

#define CAM1_RIGHT_SX       CAM_WIDTH-CAM_WIDTH/2+200
#define CAM1_RIGHT_EX       CAM_WIDTH-11
#define CAM1_RIGHT_SY       200
#define CAM1_RIGHT_EY       600

/* camera 2 */
#define CAM2_LEFT_SX        80
#define CAM2_LEFT_EX        500
#define CAM2_LEFT_SY        140
#define CAM2_LEFT_EY        500

#define CAM2_RIGHT_SX       CAM_WIDTH-CAM_WIDTH/2+200
#define CAM2_RIGHT_EX       CAM_WIDTH-11
#define CAM2_RIGHT_SY       140
#define CAM2_RIGHT_EY       500

/* camera 3 */
#define CAM3_LEFT_SX        20
#define CAM3_LEFT_EX        500
#define CAM3_LEFT_SY        200
#define CAM3_LEFT_EY        600

#define CAM3_RIGHT_SX       CAM_WIDTH-CAM_WIDTH/2+200
#define CAM3_RIGHT_EX       CAM_WIDTH-11
#define CAM3_RIGHT_SY       140
#define CAM3_RIGHT_EY       500


#if   (CALI_MODE == 4) //3D lut.bin
#define USR_GRID_DX       250
#define USR_GRID_DY       250
#define USR_GRID_DIST_V   6650
#define USR_GRID_DIST_H   2650
#define USR_ISAUTOCALIB   4
#define USR_DIST_BORDER   150
#elif  (CALI_MODE == 0)  //2D lut.bin

#define USR_GRID_DX       400
#define USR_GRID_DY       400
#define USR_GRID_DIST_V   5800
#define USR_GRID_DIST_H   2900
#define USR_ISAUTOCALIB   0
#define USR_DIST_BORDER   200

#endif

 /******************END calibration CFG*****************/

#endif
