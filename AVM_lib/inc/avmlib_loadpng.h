/*******************************************************************************
* File Name          : avmlib_loadpng.h
* Author             : Jerry
* Revision           : 2.1
* Date               : 15/07/2016
* Description        : Library Interface 
*******************************************************************************/

#ifndef __AVMLIB_LOADPNG_H_
#define __AVMLIB_LOADPNG_H_

#include <stdio.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

typedef struct png_texture_
{
    GLsizei win_x; // 图片显示位置X
    GLsizei win_y; // 图片显示位置Y
    GLsizei width; // 图片宽度
    GLsizei height;// 图片高度
    GLenum format; //图片格式(固定RGBA)
    GLsizei size;  //图片大小
    GLint internalFormat;//内部格式
    GLubyte *texels; //图片数据
}  png_texture_t;

png_texture_t *ReadPNGFromFile(const char *filename);

png_texture_t *loadPNGTexture(const char *filename);

#endif
