/*******************************************************************************
* File Name          : egl_dvr.cpp
* Author             : provide by allwinner
* Revision           : 2.1 
* Date               : 22/10/2017
* Description        : dvr
*******************************************************************************/

#ifdef T3BOARD
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "tool.h"

#include <sys/time.h>
#include <time.h>
#include "drm_fourcc.h"
#include <pthread.h>
#include <semaphore.h>
#include "egl_t3.h"
#include "egl_img.h"
#include "ion.h"
#include "drm_fourcc.h"


#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>


#define DMAFD_ARRAY_NUM   16


static EGLint const attribList[] = {
	EGL_RED_SIZE, 8,
	EGL_GREEN_SIZE, 8,
	EGL_BLUE_SIZE, 8,
	EGL_ALPHA_SIZE, 8,
	EGL_BUFFER_SIZE, 32,
	EGL_STENCIL_SIZE, 0,
	EGL_DEPTH_SIZE, 0,
	EGL_SAMPLE_BUFFERS, 1,
	EGL_SAMPLES, 4,
	EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
	EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PIXMAP_BIT,
	EGL_NONE
};


static int ion_fd = -1; 
static int dmafd_array_flag = 0;
static sem_t sem_update;
static sem_t sem_update2;
static EGLSurface  egl_surface_new;
static EGLContext  egl_context_new;
static ESContext esContext;
static video_buf_t video_buf_array[2];
static EGLImageKHR eglImgArray[MAX_DMA_FD_NUM];
static int dmaFdArray[MAX_DMA_FD_NUM];
static int video01DmafdIndex[MAX_ION_DMAFD_ARRAY_NUM];
static GLuint textureID[MAX_ION_DMAFD_ARRAY_NUM];

#ifdef IMAGE_DEBUG
	static int dmafd_video0 = -1;
	static int dmafd_video1 = -1;
	static unsigned char *video0_data = NULL;
	static unsigned char *video1_data = NULL;
#endif
static int setupNV21ImageKHR(EGLDisplay dpy, EGLImageKHR *egl_img_array,  int *dma_fd_array, int dma_fd_array_cnt);
static int init_eglimg_dmafd(void);
static void *create_es_thread( void *param);
static int set_dmafd(int dmafd, int channel);

static int egl_init(void);
static int get_fb0_size(int *w, int *h);
static int get_dmafd_index(int dmafd, int *p_index);
static int update_dmafd01_texture(int *dmafd01_index, GLuint *texture01_id, int texture_cnt, EGLImageKHR *img );
static int init_dma_buf_png(char *path, int size,unsigned char *yuv);
static int init_dma_buf_bmp(char *path, int size,unsigned char *yuv);
static int init_eglimg_dmafd(void)
{
    unsigned long i;
    for (i = 0; i < DMAFD_ARRAY_NUM; i++)
    {
        dmaFdArray[i] = -1;
    }
	
    return 0;
}

static int set_dmafd(int dmafd, int channel)
{
    for (int i = 0; i < 8; i++)
    {
        if(channel == 3)
        {
            if (dmaFdArray[i] == dmafd)
            {
                break;
            }
            if (dmaFdArray[i] == -1)
            {
                dmaFdArray[i] = dmafd;
                break;
            }
        }
        if(channel == 12)
        {
            if (dmaFdArray[i+8] == dmafd)
            {
                break;
            }
            if (dmaFdArray[i+8] == -1)
            {
                dmaFdArray[i+8] = dmafd;
                break;
            }
        }
    }
}
static int get_dmafd_index(int dmafd, int *p_index)
{
    int ret=-1;
    int i;

    for (i = 0; i < DMAFD_ARRAY_NUM; i++)
    {
        if (dmaFdArray[i] == dmafd)
        {
            ret = 0;
            break;
        }
        if (dmaFdArray[i] == -1)
        {
            break;
        }
    }

    if (p_index == NULL)
      printf("p index NULL exit -1\n");
    else
      *p_index = i;

    return ret;
}

static int update_dmafd01_texture(int *dmafd01_index, GLuint *texture01_id, int texture_cnt, EGLImageKHR *img )
{
	if(dmafd01_index == 0 || texture01_id == 0 || img == 0)
	{
		printf("%s,line:%d,parameter err\n",__FILE__,__LINE__);
		return -1;
	}
	for(int i=0; i<texture_cnt; i++) 
	{
		GLint Gerr;
		GLuint TextureOES_index;
		TextureOES_index = dmafd01_index[i];
		switch(i)
		{
			case 0:
				glActiveTexture(GL_TEXTURE0);
				break;
			case 1:
				glActiveTexture(GL_TEXTURE1);
				break;
		}
		glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture01_id[i]);
		glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, (GLeglImageOES)img[TextureOES_index]);
		Gerr = glGetError();
		if( Gerr )
		{
			fprintf(stderr,"%s,line:%d,Gerr%d, TextureOES_index:%d, texture01_id[%d]:%d, img[TextureOES_index]:0x%x\n",__FILE__,__LINE__,Gerr,TextureOES_index,i,texture01_id[i],img[TextureOES_index]);
		}
	}

	return 0;
}

static int setupNV21ImageKHR(EGLDisplay dpy, EGLImageKHR *egl_img_array,  int *dma_fd_array, int dma_fd_array_cnt) 
{
	int dma_fd;
	int atti = 0;
	EGLint attribs[30];

	if(egl_img_array == 0 || dma_fd_array == 0 || dma_fd_array_cnt > MAX_EGL_IMG)
	{
	//	printf("%d %d %d \n", egl_img_array, dma_fd_array, dma_fd_array_cnt);
		fprintf(stderr, "%s,line:%d parameter null err\n",__FILE__,__LINE__);
		return -1;
	}

	for(int i=0; i<dma_fd_array_cnt; i++)
	{
		atti = 0;

		dma_fd = dma_fd_array[i];
		if(dma_fd < 0)
		{
			fprintf(stderr, "dma_fd_array err.\n");
			return -1;
		}

		attribs[atti++] = EGL_WIDTH;
		attribs[atti++] = DMAFD_W;
		attribs[atti++] = EGL_HEIGHT;
		attribs[atti++] = DMAFD_H;
		attribs[atti++] = EGL_LINUX_DRM_FOURCC_EXT;
		attribs[atti++] = DRM_FORMAT_NV21;
		attribs[atti++] = EGL_DMA_BUF_PLANE0_FD_EXT;
		attribs[atti++] = dma_fd;
		attribs[atti++] = EGL_DMA_BUF_PLANE0_OFFSET_EXT;
		attribs[atti++] = 0;
		attribs[atti++] = EGL_DMA_BUF_PLANE0_PITCH_EXT;
		attribs[atti++] = DMAFD_W;
		attribs[atti++] = EGL_DMA_BUF_PLANE1_FD_EXT;
		attribs[atti++] = dma_fd;
		attribs[atti++] = EGL_DMA_BUF_PLANE1_OFFSET_EXT;
		attribs[atti++] = DMAFD_W*DMAFD_H;
		attribs[atti++] = EGL_DMA_BUF_PLANE1_PITCH_EXT;
		attribs[atti++] = DMAFD_W;
		attribs[atti++] = EGL_YUV_COLOR_SPACE_HINT_EXT;
		attribs[atti++] = EGL_ITU_REC709_EXT;
		attribs[atti++] = EGL_SAMPLE_RANGE_HINT_EXT;
		attribs[atti++] = EGL_YUV_FULL_RANGE_EXT;
		attribs[atti++] = EGL_NONE;
		egl_img_array[i] = eglCreateImageKHR(dpy, EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, 0, attribs);


		if (egl_img_array[i] == EGL_NO_IMAGE_KHR) {
			fprintf(stderr, "Error:0000 failed: 0x%08X\n",glGetError());
			return -1;
		}
	}

	return 0;
}

int egl_esContext_get(ESContext **context) 
{

	*context = &esContext;
	return 0;

}

static int get_fb0_size( int *w, int *h)
{
    int ret = 0;
    int fd;
    struct fb_var_screeninfo var;

    if((fd = open("/dev/fb0",O_RDWR)) == -1)
    {
        printf("Error:open file /dev/fb0 fail. \n");
        return -1;
    }

    if(ioctl(fd,FBIOGET_VSCREENINFO,&var)==-1)
    {

        printf("get screen information failure\n");
        return -2;
    }

    close(fd);
    *w = var.xres;
    *h = var.yres;

    printf("fb0 xres=%d,yres=%d\n",*w,*h);
    printf("fb0 width=%d,height=%d\n",var.width,var.height);

    return 0;
}

static void esInitContext_t3 ( ESContext *esContext )
{
   if ( esContext != NULL )
   {
      memset( esContext, 0, sizeof( ESContext) );
   }
}
static int egl_init(void)
{
	int width = SCREEN_WINDOW_WIDTH;
	int height= SCREEN_WINDOW_HEIGHT;
	struct fbdev_window native_window;
	EGLint numConfigs;
	EGLint majorVersion;
	EGLint minorVersion;
	EGLDisplay display;
	EGLContext context;
	EGLSurface surface;
	EGLConfig config;
	EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
	static EGLint window_attribute_list[] = {
		EGL_NONE
	};

    get_fb0_size( &width, &height);
	esInitContext_t3( &esContext );
	esContext.width = width;
	esContext.height = height;

	// Get Display
	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if ( display == EGL_NO_DISPLAY )
	{
		fprintf(stderr, "Error: eglGetDisplay(EGL_DEFAULT_DISPLAY) failed: 0x%08X\n", eglGetError());
		return EGL_FALSE;
	}

	// Initialize EGL
	if ( !eglInitialize(display, &majorVersion, &minorVersion) )
	{
		fprintf(stderr, "Error: eglInitialize failed: 0x%08X\n", eglGetError());
		return EGL_FALSE;
	}


	// Choose config
	if ( !eglChooseConfig(display, attribList, &config, 1, &numConfigs) )
	{
		fprintf(stderr, "Error: eglChooseConfig failed: 0x%08X\n", eglGetError());
		return EGL_FALSE;
	}

	native_window.width = width;
	native_window.height = height;

	// Create a surface
	surface = eglCreateWindowSurface(display, config, &native_window, window_attribute_list);
	if ( surface == EGL_NO_SURFACE )
	{
		fprintf(stderr, " -1 surface:%p\n",surface);
		return EGL_FALSE;
	}

	// Create a initial-first GL context
	context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs );
	if ( context == EGL_NO_CONTEXT )
	{
		fprintf(stderr, " -1 context:%p\n",context);
		return EGL_FALSE;
	}   

	// Create a GL context
	if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
			!eglQuerySurface(display, surface, EGL_HEIGHT, &height)) {
		fprintf(stderr, "Error: eglQuerySurface failed: 0x%08X\n", eglGetError());
		return -1;
	}

	// Make the context current
	if ( !eglMakeCurrent(display, surface, surface, context) )
	{
		fprintf(stderr, " eglMakeCurrent err!!!\n");
		return EGL_FALSE;
	}

	esContext.eglDisplay = display;
	esContext.eglSurface = surface;
	esContext.eglContext = context;
	esContext.eglConfig  = config;

	egl_surface_new = eglCreatePbufferSurface(display, config,NULL);
	if (egl_surface_new == EGL_NO_SURFACE) 
	{
		printf("Error: eglCreatePbufferSurface new failed: 0x%08X , %s, %d \n", eglGetError(), __FUNCTION__, __LINE__);
		return EGL_FALSE;
	}

	egl_context_new = eglCreateContext(display, config, context , contextAttribs);
    if((egl_context_new == EGL_NO_CONTEXT))
    {
		printf("Error: eglCreateContext new failed: 0x%08X, %s, %d \n",eglGetError(), __FUNCTION__, __LINE__);
		return EGL_FALSE;
	}

	return 0;
}


void egl_swap(void)
{
#if 1
	EGLSyncKHR sync;
	eglSwapBuffers(esContext.eglDisplay, esContext.eglSurface);
	sync = eglCreateSyncKHR(esContext.eglDisplay, EGL_SYNC_FENCE_KHR, NULL);
	eglClientWaitSyncKHR(esContext.eglDisplay, sync, EGL_SYNC_FLUSH_COMMANDS_BIT_KHR, 3000000000);
	eglDestroySyncKHR(esContext.eglDisplay, sync);
	if(glGetError())
		printf("Error:44 failed: %s\n",__FUNCTION__); 
#else
	glFinish();
	eglSwapBuffers(esContext.eglDisplay, esContext.eglSurface);
#endif
}

int enable_new_context(void)
{
	if (!eglMakeCurrent(esContext.eglDisplay, egl_surface_new, egl_surface_new, egl_context_new)) 
	{
		printf("Error: eglMakeCurrent() failed: 0x%08X  , %s, %d\n",eglGetError(), __FUNCTION__, __LINE__);
		return -1;
	}
	return 0;
}

/*disenable_new_context*/
int disenable_new_context(void)
{
	eglMakeCurrent(0, 0, 0, 0);
    eglDestroySurface(esContext.eglDisplay, egl_surface_new);
	return 0;
}

static void camera_texture_init(void)
{
	glGenTextures(MAX_ION_DMAFD_ARRAY_NUM, textureID);
    printf("mTextureID0:%d,mTextureID1:%d\n",textureID[0], textureID[1]);

    esContext.textureContext.TextureCnt = 2;
    esContext.textureContext.TexVideoIDType = GL_TEXTURE_EXTERNAL_OES;
    esContext.textureContext.TextureID = textureID;
    printf("esthread:: texture:%d,%d; texcnt:%d; type:0x%x\n",esContext.textureContext.TextureID[0],esContext.textureContext.TextureID[1],esContext.textureContext.TextureCnt,esContext.textureContext.TexVideoIDType);
    
}
#ifndef IMAGE_DEBUG
void update_dma_texture(void)
{
	int img_index;
	static int i = 0;
	int flag = 0x0f;
	int channel_flag;
	do{
	    sem_wait(&sem_update);
	    get_dmafd_index(video_buf_array[i%2].dma_fd,&img_index);
	    channel_flag = video_buf_array[i%2].channel;
	    i++;
	    flag &= ~(channel_flag);//11111100

		video01DmafdIndex[1] = img_index;
	    if(!flag)
	      break;
	    video01DmafdIndex[0] = img_index;

	}while(1);

	update_dmafd01_texture(video01DmafdIndex, textureID, MAX_ION_DMAFD_ARRAY_NUM, eglImgArray);
 
}
#else
void update_dma_texture(void)
{
	
	for(int i=0; i<2; i++) 
	{
		GLint Gerr;
		GLuint TextureOES_index;
		switch(i)
		{
			case 0:
				glActiveTexture(GL_TEXTURE0);
				break;
			case 1:
				glActiveTexture(GL_TEXTURE1);
				break;
		}
		glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureID[i]);
		glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, (GLeglImageOES)eglImgArray[i]);
		Gerr = glGetError();
		if( Gerr )
		{
			fprintf(stderr,"%s,line:%d,Gerr%d\n",__FILE__,__LINE__,Gerr);
		}
	}

}
#endif

#ifdef IMAGE_DEBUG
void image_simulate(void)
{

	printf("init image simulate start\n");
	dmafd_video0 = init_dma_buf_png("./video0.png", DMAFD_W*DMAFD_H*3/2,video0_data);
	dmafd_video1 = init_dma_buf_png("./video1.png", DMAFD_W*DMAFD_H*3/2, video1_data);
	
}

void dmafd_init(void)
{
	dmaFdArray[0] = dmafd_video0;
	dmaFdArray[1] = dmafd_video1;
	setupNV21ImageKHR(esContext.eglDisplay, eglImgArray, dmaFdArray, 2);

}

#else
void dmafd_init(void)
{
	sem_wait(&sem_update2);
	setupNV21ImageKHR(esContext.eglDisplay, eglImgArray, dmaFdArray, MAX_DMA_FD_NUM);
	

}
#endif
void es_init(void)
{
	int ret = 0;
#ifdef IMAGE_DEBUG
	image_simulate();
#endif
	init_eglimg_dmafd();
    egl_init();
	camera_texture_init();

	ret = sem_init(&sem_update, 0, 0);
    ret = sem_init(&sem_update2, 0, 0);

}


int get_t3_video_buf(video_buf_t *video_buf)
{
    int ret;
    if(dmafd_array_flag == 0)
    {
        set_dmafd(video_buf->dma_fd,video_buf->channel);
        if(dmaFdArray[7]!=-1 && dmaFdArray[15]!=-1)
        {
            dmafd_array_flag = 1;
            sem_post(&sem_update2);
        }
        printf("func:%s,index:%d,frame_cnt:%d,dmafd:%d\n",__func__,video_buf->format, video_buf->frame_cnt, video_buf->dma_fd);
    }else{
        //int flag = 0x11;
        //flag &= ~(video_buf->channel);
     
        if(video_buf->channel == 3)
        {
            video_buf_array[0].channel = video_buf->channel;
            video_buf_array[0].dma_fd = video_buf->dma_fd;
			video_buf_array[0].vir_addr = video_buf->vir_addr;
        }
        else
		{
            video_buf_array[1].channel = video_buf->channel;
            video_buf_array[1].dma_fd = video_buf->dma_fd;
			video_buf_array[1].vir_addr = video_buf->vir_addr;
        }

        sem_post(&sem_update);
    }

    return 0;
}

void get_camera_yuv_data(unsigned char *video_0_buffer, unsigned char *video_1_buffer)
{
	printf("memcpy start\n");
	memcpy(video_0_buffer, video_buf_array[0].vir_addr, DMAFD_W*DMAFD_H*3/2);
	memcpy(video_1_buffer, video_buf_array[1].vir_addr, DMAFD_W*DMAFD_H*3/2);

}

void fps_calc(void)
{
    static int fps;
    static struct timeval tv_start, tv_current;
    int total_timems;

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

void avm_User_Stub_Textures(GLint TexturesID, GLint *OutTexturesID)
{
	int c_index  = 0;
	if ((TexturesID > 3)||(TexturesID < 0))
	{
		printf("the camera_number is wrong\n");
	}
	
	switch(TexturesID)
	{
		case 0:				
		case 1:	
			*OutTexturesID = 1;
			c_index = 1;
			glActiveTexture(GL_TEXTURE1);
			break;
		case 2:		
		case 3:
			c_index = 0;
			*OutTexturesID = 0;
			glActiveTexture(GL_TEXTURE0);
			break;
	}
     glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureID[c_index]);

}


static int init_dma_buf_png(char *path, int size,unsigned char *yuv)
{
    int file_fd;
    int ret;
    void* addr_file,*addr_dma;

    if(ion_fd == -1)
        ion_fd = open("/dev/ion", O_RDWR);
    if(ion_fd < 0)
    {
        printf("open /dev/ion err.\n");
        return -1;
    }
    //alloc
    struct ion_allocation_data sAllocInfo =
	{
		.len		= size,
		.align		= 4096,
		.heap_id_mask	= ION_HEAP_CARVEOUT_MASK,//ION_HEAP_TYPE_DMA_MASK,
		.flags		= 0,
		.handle     = 0,
	};
	ret = ioctl(ion_fd, ION_IOC_ALLOC, &sAllocInfo);
	if(ret < 0)
	{
	    printf("alloc ion err.\n");
		close(ion_fd);
	    return  -1;
	}
    //share
    struct ion_fd_data data = {
        .handle = sAllocInfo.handle,
    };

	ret = ioctl(ion_fd, ION_IOC_MAP, &data);
    if (ret < 0)
        return ret;
    if (data.fd < 0) {
        printf("share ioctl returned negative fd\n");
        return -1;
    }
    addr_dma = (void *)mmap((void *)0, size, PROT_WRITE, MAP_SHARED, data.fd, 0);


	if (yuv == NULL)
	{
		yuv = (unsigned char*)calloc(size,sizeof(char));
	}
	png_to_420sp(path, yuv);

	memcpy(addr_dma, yuv, size);
	system("sync");
    struct ion_fd_data data2 = {
        .fd = data.fd,
    };
    
    ioctl(ion_fd, ION_IOC_SYNC, &data2);
    munmap(addr_dma ,size);
    printf("end######## %s #########.\n",path);


    return data.fd;
}

static int init_dma_buf_bmp(char *path, int size, unsigned char *yuv)
{
    int file_fd;
    int ret;
    void* addr_file,*addr_dma;

    if(ion_fd == -1)
        ion_fd = open("/dev/ion", O_RDWR);
    if(ion_fd < 0)
    {
        printf("open /dev/ion err.\n");
        return -1;
    }
    //alloc
    struct ion_allocation_data sAllocInfo =
	{
		.len		= size,
		.align		= 4096,
		.heap_id_mask	= ION_HEAP_CARVEOUT_MASK,//ION_HEAP_TYPE_DMA_MASK,
		.flags		= 0,
		.handle     = 0,
	};
	ret = ioctl(ion_fd, ION_IOC_ALLOC, &sAllocInfo);
	if(ret < 0)
	{
	    printf("alloc ion err.\n");
		close(ion_fd);
	    return  -1;
	}
    //share
    struct ion_fd_data data = {
        .handle = sAllocInfo.handle,
    };

	ret = ioctl(ion_fd, ION_IOC_MAP, &data);
    if (ret < 0)
        return ret;
    if (data.fd < 0) {
        printf("share ioctl returned negative fd\n");
        return -1;
    }
    addr_dma = (void *)mmap((void *)0, size, PROT_WRITE, MAP_SHARED, data.fd, 0);


	if (yuv == NULL)
	{
		yuv = (unsigned char*)calloc(size,sizeof(char));
	}
	
	bmp_to_420sp(path, yuv);

	memcpy(addr_dma, yuv, size);
	system("sync");
    struct ion_fd_data data2 = {
        .fd = data.fd,
    };
    
    ioctl(ion_fd, ION_IOC_SYNC, &data2);
    munmap(addr_dma ,size);
    printf("end######## %s #########.\n",path);


    return data.fd;
}
#endif
