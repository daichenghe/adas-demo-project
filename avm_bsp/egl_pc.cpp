/*******************************************************************************
* File Name          : egl_pc.cpp
* Author             : xxy Daich
* Revision           : 2.1 
* Date               : 17/03/2018
* Description        : egl
*******************************************************************************/
#ifndef T3BOARD

#include <X11/X.h>
#include <X11/Xlib.h>
#include "avmlib_loadpng.h"
#include "avm_common.h"
#include "avm_config.h"
#include "avmlib_sdk.h"
#include <malloc.h>
#include "stdlib.h"
#include <memory.h>
#include <sys/time.h>
#include <time.h>

static GLuint g_video_texture_id[4];
EGLSurface           eglsurface;
EGLDisplay           egldisplay;

static EGLNativeWindowType  eglNativeWindow;
static EGLSurface           eglsurface_off;
static EGLNativeDisplayType eglNativeDisplayType;
static EGLConfig            eglconfig;
static EGLContext           eglcontext;
static char 				*egl_pixmap_addr;
static EGLNativePixmapType  egl_pixmap;

static int Textures_init(GLuint cam_width, GLuint cam_heigh);

EGLNativeDisplayType getNativeDisplay()
{
	EGLNativeDisplayType eglNativeDisplayType = NULL;
	eglNativeDisplayType = XOpenDisplay(NULL);
	assert(eglNativeDisplayType != NULL);
	return eglNativeDisplayType;
}

EGLNativeWindowType createwindow(EGLDisplay egldisplay, EGLNativeDisplayType eglNativeDisplayType)
{
	EGLNativeWindowType native_window = (EGLNativeWindowType)0;

	Window window, rootwindow;
	int screen = DefaultScreen(eglNativeDisplayType);
	rootwindow = RootWindow(eglNativeDisplayType,screen);
	window = XCreateSimpleWindow(eglNativeDisplayType, rootwindow, 0, 0, SCREEN_WINDOW_WIDTH, SCREEN_WINDOW_HEIGHT, 0, 0, WhitePixel (eglNativeDisplayType, screen));
	XMapWindow(eglNativeDisplayType, window);
	native_window = window;
	return native_window;

}


void destroywindow(EGLNativeWindowType eglNativeWindowType, EGLNativeDisplayType eglNativeDisplayType)
{
	(void) eglNativeWindowType;
	//close x display
	XCloseDisplay(eglNativeDisplayType);
}

int egl_init(void)
{
    static const EGLint s_configAttribs[] =
    {
    #if 1
        EGL_RED_SIZE,       5,
        EGL_GREEN_SIZE,      6,
        EGL_BLUE_SIZE,      5,
        EGL_ALPHA_SIZE,     0,
        EGL_SAMPLES,        0,
        EGL_DEPTH_SIZE,     24,
        EGL_NONE
       #else
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_BUFFER_SIZE, 32,

		EGL_STENCIL_SIZE, 0,
		EGL_DEPTH_SIZE, 0,

		EGL_SAMPLE_BUFFERS, 1,
		EGL_SAMPLES, 4,
	//	EGL_SAMPLES, 0,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,

		EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PIXMAP_BIT,


		EGL_NONE
	  #endif
    };

    EGLint iEGL_RED_SIZE, iEGL_GREEN_SIZE, iEGL_BLUE_SIZE, iEGL_ALPHA_SIZE, iEGL_BUFFER_SIZE, iEGL_SAMPLE_BUFFERS, iEGL_SAMPLES;
    EGLint ContextAttribList[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

    EGLint numconfigs ;
    EGLBoolean ret;
	
	int width;
	int height;
	int bits_perpixel;
	int stride;
	int attribListPbuffer[] =
    {
        EGL_WIDTH,  SCREEN_WINDOW_WIDTH,
        EGL_HEIGHT, SCREEN_WINDOW_HEIGHT,
        EGL_NONE
    };
    eglNativeDisplayType = getNativeDisplay();
    egldisplay = eglGetDisplay(eglNativeDisplayType);
    if((egldisplay == EGL_NO_DISPLAY))
    {
        return AVM_ERR_FALSE;
    }
    ret = eglInitialize(egldisplay, NULL, NULL);
    if(!ret)
    {
        return AVM_ERR_FALSE;
    }
    ret = eglBindAPI(EGL_OPENGL_ES_API);
    if(!ret)
    {
        return AVM_ERR_FALSE;
    }
    ret = eglChooseConfig(egldisplay, s_configAttribs, &eglconfig, 1, &numconfigs);
    if(!ret)
    {
        return AVM_ERR_FALSE;
    }
    eglNativeWindow = createwindow(egldisplay, eglNativeDisplayType);
    if(eglNativeWindow == 0)
    {
        return AVM_ERR_FALSE;
    }

    eglGetConfigAttrib(egldisplay, eglconfig, EGL_RED_SIZE, &iEGL_RED_SIZE);
    eglGetConfigAttrib(egldisplay, eglconfig, EGL_GREEN_SIZE, &iEGL_GREEN_SIZE);
    eglGetConfigAttrib(egldisplay, eglconfig, EGL_BLUE_SIZE, &iEGL_BLUE_SIZE);
    eglGetConfigAttrib(egldisplay, eglconfig, EGL_ALPHA_SIZE, &iEGL_ALPHA_SIZE);
    eglGetConfigAttrib(egldisplay, eglconfig, EGL_BUFFER_SIZE, &iEGL_BUFFER_SIZE);
    eglGetConfigAttrib(egldisplay, eglconfig,  EGL_SAMPLE_BUFFERS, &iEGL_SAMPLE_BUFFERS);
    eglGetConfigAttrib(egldisplay, eglconfig,  EGL_SAMPLES, &iEGL_SAMPLES);

    eglsurface  = eglCreateWindowSurface(egldisplay, eglconfig, eglNativeWindow, NULL);
    if((eglsurface == EGL_NO_SURFACE))
    {
        return AVM_ERR_FALSE;
    }
    eglcontext = eglCreateContext(egldisplay, eglconfig, EGL_NO_CONTEXT, ContextAttribList);
    if((eglcontext == EGL_NO_CONTEXT))
    {
        return AVM_ERR_FALSE;
    }

    ret = eglMakeCurrent(egldisplay, eglsurface, eglsurface, eglcontext);
    if(!ret)
    {
        return AVM_ERR_FALSE;
    }
	return AVM_ERR_NONE;
}
void avm_User_Stub_Textures(GLint TexturesID, GLint *OutTexturesID)
{
	GLint eGlTexture[4] = {GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3};

    glActiveTexture(eGlTexture[TexturesID]);//
    glBindTexture(GL_TEXTURE_2D, g_video_texture_id[TexturesID]);
	*OutTexturesID = TexturesID;
}
static int Textures_init(GLuint cam_width, GLuint cam_heigh)
{
    S32 i = 0;
	png_texture_t *png_tex = NULL;
	unsigned char *nv21 = NULL;
	char buft[128] = {'\0'};
	
	glGenTextures(4, g_video_texture_id);
    GL_CHECK_ERRORS;

	nv21 = (unsigned char*)calloc(cam_width*cam_heigh*4,sizeof(char));

	for(i = 0; i < 4; i++) 
    {
    	sprintf(buft, "media/test_%d.png",i);  
		png_tex = loadPNGTexture(buft);
		nv21 = png_tex->texels;
        glBindTexture(GL_TEXTURE_2D, g_video_texture_id[i]);
        GL_CHECK_ERRORS;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cam_width, cam_heigh, 0, GL_RGBA, GL_UNSIGNED_BYTE, nv21);
        GL_CHECK_ERRORS;
    }
	printf("texture init finish\r\n");

}

int texture_update(void)
{
	int i;
	GLint eGlTexture[4] = {GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3};
	char buft[128] = {'\0'};
	png_texture_t *png_tex = NULL;
	static unsigned char *texture_data[4] = {NULL};
	
	if (texture_data[0] = NULL)
	{
		for (i = 0; i < 4; i++)
		{
			sprintf(buft, "media/test_%d.png",i);  
   			png_tex = loadPNGTexture(buft);
			texture_data[i] = png_tex->texels;
		}
	}

	
//	texture_data[0] = (unsigned char*)calloc(CAM_WIDTH*CAM_HEIGHT*4,sizeof(char));
//	memset(texture_data[0], 224, CAM_WIDTH*CAM_HEIGHT*4);
	/* ¸üÐÂÎÆÀí */
	for(i = 0; i < 4; i++)
	{
		glActiveTexture(eGlTexture[i]);
		glBindTexture(GL_TEXTURE_2D, g_video_texture_id[i]);
		GL_CHECK_ERRORS;
		/* set texture parameters */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		GL_CHECK_ERRORS;
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CAM_WIDTH, CAM_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE,texture_data[i]);
		GL_CHECK_ERRORS;
	}

    glBindTexture(GL_TEXTURE_2D, 0);

}
void egl_swap(void)
{
	eglSwapBuffers(egldisplay, eglsurface);
}

void bsp_pc_init(void)
{
	egl_init();
	Textures_init(CAM_WIDTH, CAM_HEIGHT);

}
void fps_calc(void)
{
    static int fps;
    static struct timeval tv_start, tv_current;
    unsigned int total_timems;

    gettimeofday(&tv_current, 0);
    total_timems = (tv_current.tv_sec - tv_start.tv_sec) * 1000L;
    total_timems += (tv_current.tv_usec - tv_start.tv_usec) / 1000;
    if(total_timems >= 1000)
    {
        gettimeofday(&tv_start, 0);
        printf("fps:*********************************%d\n",fps);
        fps=0;
    }

    fps++;
}
#endif
