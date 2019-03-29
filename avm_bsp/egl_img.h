/*************************************************************************
    > File Name: egl_img.h
    > Author: leniux
    > Created Time: 2017-03-23
 ************************************************************************/

#ifndef __EGL_IMG_H__
#define __EGL_IMG_H__

#define MAX_EGL_IMG 64
#define MAX_TEXTURE 2


typedef int uniform;
typedef int attribute;


typedef struct GlobalTexture_
{
	int *TextureID;
	int  TextureCnt;
	int TexVideoIDType;  
}GlobalTexture_t;

typedef struct _escontext
{
   /// Put your user data here...
   void*       userData;

   /// Window width
   void*       width;

   /// Window height
   void*       height;

   /// Window handle
  void*  hWnd;

   /// EGL display
   void*  eglDisplay;
      
   /// EGL context
   void*  eglContext;

   /// EGL surface
   void*  eglSurface;

   // EGL Config
   void*  eglConfig;

   GlobalTexture_t textureContext;

} ESContext;

#endif

