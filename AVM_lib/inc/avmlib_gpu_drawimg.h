/*******************************************************************************
* File Name          : avmlib_gpu_drawing.h
* Author             : Jerry
* Revision           : 2.1
* Date               : 15/07/2016
* Description        : Library Interface 
*******************************************************************************/
#ifndef __AVMLIB_GPU_DRAWIMG_H__
#define __AVMLIB_GPU_DRAWIMG_H__

#include <stdio.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "avmlib_sdk.h"
#include "avmlib_loadpng.h"
/*
*************************************************************************
*************************************************************************
*                      D A T A   S T R U C T U R E S
*************************************************************************
*************************************************************************
*/


/*
*************************************************************************
*************************************************************************
*                      F U N C T I O N
*************************************************************************
*************************************************************************
*/

AVM_ERR avmlib_create_texture(const GLvoid *pbuf,GLenum format,S32 texture_w, 
								S32 texture_h, S32 size,GLuint *tex_id_output);

/*******************************************************************************************
* Function Name  : avmlib_Draw_Png.
*
* Description    : 显示png图片
*
* Arguments      : png_texture    png_texture_t指针
*                  -----
*
*                  directdraw    是否直接显示图片纹理 AVM_NO:将png_texture更新后再显示
*                  -----                              AVM_YES:不更新png_texture直接显示
*
* Return         : AVM_ERR_NONE   successful.
*                  Other         Other AVM_ERR_xxx   fail.
*
* Note(s)        : none.
******************************************************************************************/
AVM_ERR avmlib_Draw_Png(const png_texture_t *const png_texture, GLuint tex_id_input);

/*******************************************************************************************
* Function Name  : avmlib_DrawPng_Init.
*
* Description    : 画图初始化
*
* Arguments      : programName    顶点着色器代码
*                  -----
*
*                  programName1   片段着色器代码
*                  -----
*
*                  format         图片格式
*                  -----
*
*                  size           图片尺寸
*                  -----
*
*                  pbuf           图片数据
*                  -----
*
*                  win_w          画图窗口宽度
*                  -----
*
*                  win_h          画图窗口高度
*                  -----
*
*
*                  dip_w          显示窗口宽度
*                  -----
*
*                  dip_h          显示窗口高度
*                  -----
* Return         : AVM_ERR_NONE   successful.
*                  Other         Other AVM_ERR_xxx   fail.
*
* Note(s)        :  在画图之前需要进行一次初始化.
******************************************************************************************/
AVM_EXT AVM_ERR avmlib_DrawPng_Init(const C8 *programName, const C8 *programName1);



/*******************************************************************************************
* Function Name  : avmlib_DrawPng_Exit.
*
* Description    : 画图退出
*
* Arguments      : none
*
* Return         : AVM_ERR_NONE   successful.
*                  Other         Other AVM_ERR_xxx   fail.
*
* Note(s)        : none.
******************************************************************************************/
AVM_EXT AVM_ERR avmlib_DrawPng_Exit(void);



#endif /* GPU_DRAWIMG_H_ */
