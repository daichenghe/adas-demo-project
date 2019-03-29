/*******************************************************************************
* File Name          : avmlib_gpu_drawline.h
* Author             : Jerry
* Revision           : 2.1
* Date               : 15/07/2016
* Description        : Library Interface 
*******************************************************************************/

#ifndef __GPU_DRAWLINE_H__
#define __GPU_DRAWLINE_H__

#include <stdio.h>
#if !defined(EGL_EGLEXT_PROTOTYPES)
#define EGL_EGLEXT_PROTOTYPES
#endif

#if !defined(GL_GLEXT_PROTOTYPES)
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "avmlib_sdk.h"
#include "avmlib_vectors.h"
#include "avmlib_matrices.h"

/*
*************************************************************************
*************************************************************************
*                      D A T A   S T R U C T U R E S
*************************************************************************
*************************************************************************
*/

typedef struct draw_line_ //线段
{
    F32 sx;//起点x
    F32 sy;//起点y
    F32 ex;//终点x
    F32 ey;//终点y
    GLboolean r;//red(0~255)
    GLboolean g;//green(0~255)
    GLboolean b;//blue(0~255)
    GLboolean a;//alpha(保留未用)
} draw_line_t;

typedef struct point_t_ //线段点坐标
{
    F64 x;
    F64 y;
    U8 blend;
} point_t;

typedef struct color_t_ //线段颜色
{
    U8 red;
    U8 green;
    U8 blue;
} color_t;

typedef struct line_attr_ //线段属性
{
    U32 line_w;      // 线段宽度(单位像素)
    U32 win_x;       // 画线窗口x
    U32 win_y;       // 画线窗口y
    U32 win_width;   // 画线窗口宽度
    U32 win_height;  // 画线窗口高度
    U8 Blend;        // 线段alpha(0~255，0:不使用blending)
} line_attr_t;

typedef struct line_strip_attr_ //线段属性
{
    U32 line_w;     // 线段宽度(单位像素)
    U32 win_x;      // 画线窗口x
    U32 win_y;      // 画线窗口y
    U32 win_width;  // 画线窗口宽度
    U32 win_height; // 画线窗口高度
    U8 Blend;       // 线段alpha(0~255，0:不使用blending)
    U8 IsStrip;     // 1: glDrawArrays(GL_LINE_STRIP,.. 0: glDrawArrays(GL_LINES,..
    F32 scalex;     //坐标X缩放系数
    F32 scaley;     //坐标Y缩放系数
    color_t linecolor;
} line_strip_attr_t;

/*
*************************************************************************
*************************************************************************
*                      F U N C T I O N
*************************************************************************
*************************************************************************
*/

/*******************************************************************************************
* Function Name  : avmlib_Draw_Lines.
*
* Description    : 画线
*
* Arguments      : lines_buffer      所要画的线段指针
*                  -----
*
*                  line_num          线段数
*                  -----
*
*                  line_attr          线段属性
*
*                  up                 视点正方向
*                  -----
* Return         : AVM_ERR_NONE   successful.
*                  Other         Other AVM_ERR_xxx   fail.
*
* Note(s)        : 一次最多画50条线
*******************************************************************************************/
AVM_EXT AVM_ERR avmlib_Draw_Lines(const draw_line_t *const lines_buffer,U32 line_num,const line_attr_t *const line_attr,Vector3 *up);

/*******************************************************************************************
* Function Name  : avmlib_Draw_Lines_Strip.
*
* Description    : 画相连线参见GL_LINE_STRIP
*
* Arguments      : points_buffer      所要画的线段上的点数据指针
*                  -----
*
*                  point_num          点数
*                  -----
*
*                  line_attr          线段属性
*                  -----
*
*                  up                 视点正方向
*                  -----
* Return         : AVM_ERR_NONE   successful.
*                  Other         Other AVM_ERR_xxx   fail.
*
* Note(s)        : 一次最多画50条线(100个点)
*******************************************************************************************/
AVM_EXT AVM_ERR avmlib_Draw_Lines_Strip(const point_t *const points_buffer, U32 point_num, const line_strip_attr_t *const line_attr, Vector3 *up, GLfloat *ModelView, GLfloat *Proj);

/*******************************************************************************************
* Function Name  : avmlib_Drawline_Init.
*
* Description    : 画线初始化
*
* Arguments      : programName    顶点着色器代码
*                  -----
*
*                  programName1   片段着色器代码
*                  -----
*
* Return         : AVM_ERR_NONE   successful.
*                  Other         Other AVM_ERR_xxx   fail.
*
* Note(s)        : 在画线之前需要进行一次初始化.
******************************************************************************************/
AVM_EXT AVM_ERR avmlib_Drawline_Init(const C8 *programName, const C8 *programName1);

/*******************************************************************************************
* Function Name  : avmlib_DrawLine_Exit.
*
* Description    : 画线退出
*
* Arguments      : none
*
* Return         : AVM_ERR_NONE   successful.
*                  Other         Other AVM_ERR_xxx   fail.
*
* Note(s)        : none.
*******************************************************************************************/
AVM_EXT AVM_ERR avmlib_DrawLine_Exit(void);



#endif /* GPU_DRAWLINE_H_ */
