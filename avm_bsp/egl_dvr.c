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

#include <sys/time.h>
#include <time.h>
#include "drm_fourcc.h"
#include <pthread.h>
#include <semaphore.h>
#include "egl_t3.h"
#include "egl_img.h"
#include "avm_config.h"





#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>

#define MAX_ION_DMAFD_ARRAY_NUMS	1
#define MAX_ION_DMAFD_QUEUE_NUMS    4
#define MAX_DMA_FD_NUMS			(MAX_ION_DMAFD_ARRAY_NUMS * MAX_ION_DMAFD_QUEUE_NUMS)	

#define DVR_WIDTH 2560 
#define DVR_HEIGHT 1440


static GLuint vertexShader;
static GLuint fragmentShader;
static GLuint programObject;
static GLuint YuvTexSamplerHandle;

static sem_t sem_dvr_update;
static int dvr_dmafds = -1;
static int dmafd_init_flag = 0;
static ESContext *dvr_EsContex = NULL;
static GLuint dvr_texture_id[MAX_ION_DMAFD_ARRAY_NUMS];
static EGLImageKHR dvr_EglImgArray[MAX_DMA_FD_NUMS];
static int dvr_Video01DmafdIndex[MAX_ION_DMAFD_ARRAY_NUMS];
static int dvr_DmaFdArray[MAX_DMA_FD_NUMS] = {-1, -1, -1, -1};

static GLuint dvr_vbo;



static GLfloat vertex_uv0[] = {
    -1.0, 1.0,
    0.0f, 0.0f,

    -1.0, -1.0,
    0.0f, 1.0f,

    1.0, -1.0,
    1.0f, 1.0f,

    1.0,  1.0,
    1.0f, 0.0f
};
static int update_dvr_dmafd(int *dmafd01_index, GLuint *texture01_id, int texture_cnt, EGLImageKHR *img );
static int init_dvr_egl_image(EGLDisplay dpy, EGLImageKHR *egl_img_array,  int *dma_fd_array, int dma_fd_array_cnt);
static void init_dvr_dmafds(int dmafd);
static int get_dvr_dmafd_index(int dmafd, int *p_index);
static int dvr_shaderInit ( ESContext *esContext );
static void dvr_use_vbo(GLuint vbo);
static void dvr_gen_vbo(void);
static void dvr_Draw ( ESContext *esContext );
static GLuint load_shader ( GLenum type, const char *shaderSrc );


void dvr_es_init( void)
{
    int ret = -1;
	
    ret = sem_init(&sem_dvr_update, 0, 0);
    egl_esContext_get(&dvr_EsContex) ;
    dvr_shaderInit(dvr_EsContex);
    dvr_gen_vbo();
    glGenTextures(MAX_ION_DMAFD_ARRAY_NUMS, dvr_texture_id);

}

void dvr_es_render(int player_state)
{

    int img_index;
	
	if (player_state == 1)
	{
		sem_wait(&sem_dvr_update);
	}
	
	get_dvr_dmafd_index(dvr_dmafds, &img_index);

	dvr_Video01DmafdIndex[0] = img_index;

	update_dvr_dmafd(dvr_Video01DmafdIndex, dvr_texture_id, MAX_ION_DMAFD_ARRAY_NUMS, dvr_EglImgArray);

	dvr_Draw(dvr_EsContex);

}

void update_mp4_buf(video_buf_t *video_buf)
{
    if(dmafd_init_flag == 0)
    {
        init_dvr_dmafds(video_buf->dma_fd);
        if(dvr_DmaFdArray[3]!=-1 )
        {
            dmafd_init_flag = 1;
			init_dvr_egl_image(dvr_EsContex->eglDisplay, dvr_EglImgArray, dvr_DmaFdArray, MAX_DMA_FD_NUMS);
        }
    }
	else
	{
        dvr_dmafds = video_buf->dma_fd;
        sem_post(&sem_dvr_update);
    }
}

void post_dvr_sem(void)
{
     sem_post(&sem_dvr_update);
}
static void init_dvr_dmafds(int dmafd)
{
    for (int i = 0; i < 4; i++)
    {
        if (dvr_DmaFdArray[i] == dmafd)
        {
            break;
        }
        if (dvr_DmaFdArray[i] == -1)
        {
            dvr_DmaFdArray[i] = dmafd;
            break;
        }
    }
}

static int get_dvr_dmafd_index(int dmafd, int *p_index)
{
    int ret=-1;
    int i;

    for (i = 0; i < 4; i++)
    {
        if (dvr_DmaFdArray[i] == dmafd)
        {
            ret = 0;
            break;
        }
        if (dvr_DmaFdArray[i] == -1)
        {
            break;
        }
    }

    if (p_index == NULL)
        printf("p index NULL exit -1¥n");
    else
        *p_index = i;

    return ret;
}

int DesetupNV21ImageKHR(EGLDisplay dpy, EGLImageKHR *egl_img_array,  int *dma_fd_array, int dma_fd_array_cnt, int width, int height) 
{
    if(egl_img_array == 0)
    {
        fprintf(stderr, "%s,line:%d parameter null err\n",__FILE__,__LINE__);
        return -1;
    }

    for(int i=0; i<dma_fd_array_cnt; i++)
    {
        eglDestroyImageKHR(dpy, egl_img_array[i]);
    }

    return 0;
}

static GLuint load_shader ( GLenum type, const char *shaderSrc )
{
	GLuint shader;
	GLint compiled;

	// Create the shader object
	shader = glCreateShader ( type );

	if ( shader == 0 )
		return 0;

	// Load the shader source
	glShaderSource ( shader, 1, &shaderSrc, NULL );

	// Compile the shader
	glCompileShader ( shader );

	// Check the compile status
	glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

	if ( !compiled ) 
	{
		GLint infoLen = 0;

		glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

		if ( infoLen > 1 )
		{
			char* infoLog = malloc (sizeof(char) * infoLen );

			glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
			printf( "Error compiling shader:\n%s\n", infoLog );            

			free ( infoLog );
		}

		glDeleteShader ( shader );
		return 0;
	}
	return shader;
}
static int dvr_shaderInit ( ESContext *esContext )
{

    const char vShaderStr[] = 
        "attribute vec2 aPosition;    \n"
        "attribute vec2 aUv;    \n"
        "varying vec2 yuvTexCoords;\n"
        "void main()                  \n"
        "{                            \n"
        "    yuvTexCoords = aUv.xy;  \n"
        "    gl_Position = vec4(aPosition.xy, 0.0, 1.0); \n"
        "}                            \n";

    const char fShaderStr[] =  
        "#extension GL_OES_EGL_image_external : require\n"
        "precision mediump float;\n"
        "uniform samplerExternalOES yuvTexSampler;\n"
        "varying vec2 yuvTexCoords;\n"
        "void main() {\n"
        "vec4 pixel = texture2D(yuvTexSampler, yuvTexCoords); \n"
        "vec3 minor_color = vec3(1.0, 1.0, 1.0) - pixel.xyz; \n"
        "gl_FragColor = vec4(pixel.xyz * 0.9 + minor_color * 0.1, 1.0);\n"
        "}\n";
/*
	const char fShaderStr[] =  
        "#extension GL_OES_EGL_image_external : require\n"
        "precision mediump float;\n"
        "uniform samplerExternalOES yuvTexSampler;\n"
        "varying vec2 yuvTexCoords;\n"
        "void main() {\n"
        "gl_FragColor = texture2D(yuvTexSampler, yuvTexCoords); \n"
        "}\n";
       */
    GLint linked;

    // Load the vertex/fragment shaders
    vertexShader = load_shader( GL_VERTEX_SHADER, vShaderStr );
    fragmentShader = load_shader( GL_FRAGMENT_SHADER, fShaderStr );

    // Create the program object
    programObject = glCreateProgram ( );

    if ( programObject == 0 )
      return 0;

    glAttachShader ( programObject, vertexShader );
    glAttachShader ( programObject, fragmentShader );
    glBindAttribLocation(programObject, 0, "aPosition");
    glBindAttribLocation(programObject, 1, "aUv");

    // Link the program
    glLinkProgram ( programObject );

    // Check the link status
    glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

    if ( !linked ) 
    {
        GLint infoLen = 0;

        glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

        if ( infoLen > 1 )
        {
            char* infoLog = (char *)malloc (sizeof(char) * infoLen );

            glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
            printf ( "Error linking program:\n%s\n", infoLog );            

            free ( infoLog );
        }

        glDeleteProgram ( programObject );
        return GL_FALSE;
    }

    glUseProgram(programObject);
    // Bind vPosition to attribute 0   
    YuvTexSamplerHandle = glGetUniformLocation(programObject, "yuvTexSampler");

    GLuint erro_status;
    if((erro_status = glGetError()))
      printf("Error: 88 failed: 0x%08X\n",erro_status); 
   
    return GL_TRUE;
}

static void dvr_gen_vbo(void)
{
    glGenBuffers(1, &dvr_vbo);

    glBindBuffer(GL_ARRAY_BUFFER, dvr_vbo);
    glBufferData(GL_ARRAY_BUFFER, 8*2*sizeof(GLfloat), vertex_uv0, GL_STATIC_DRAW);

}

static void dvr_use_vbo(GLuint vbo)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLubyte *)(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLubyte *)(2 * sizeof(GLfloat)));   
    glEnableVertexAttribArray(0);        
    glEnableVertexAttribArray(1);        
}

///
// Draw a triangle using the shader pair created in Init()
//
static void dvr_Draw ( ESContext *esContext )
{
    static int a0;

    // Set the viewport
    glViewport ( 0, 0, esContext->width, esContext->height);
    // Use the program object 
    glUseProgram (programObject);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, dvr_texture_id[0]);
    glUniform1i(YuvTexSamplerHandle, 0);

    if(glGetError())
    {
        printf("glUniform1 eirror: failed line: %d \n",__LINE__);
    }
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
	//glBlendFunc(GL_ONE, GL_ZERO);

    dvr_use_vbo(dvr_vbo);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  //  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
 
}

static int init_dvr_egl_image(EGLDisplay dpy, EGLImageKHR *egl_img_array,  int *dma_fd_array, int dma_fd_array_cnt) 
{
    int dma_fd;
    int atti = 0;
    EGLint attribs[30];

	if(egl_img_array == 0 || dma_fd_array == 0 || dma_fd_array_cnt > MAX_EGL_IMG)
	{
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
        attribs[atti++] = DVR_WIDTH;
        attribs[atti++] = EGL_HEIGHT;
        attribs[atti++] = DVR_HEIGHT;
        attribs[atti++] = EGL_LINUX_DRM_FOURCC_EXT;
        attribs[atti++] = DRM_FORMAT_NV21;
        attribs[atti++] = EGL_DMA_BUF_PLANE0_FD_EXT;
        attribs[atti++] = dma_fd;
        attribs[atti++] = EGL_DMA_BUF_PLANE0_OFFSET_EXT;
        attribs[atti++] = 0;
        attribs[atti++] = EGL_DMA_BUF_PLANE0_PITCH_EXT;
        attribs[atti++] = DVR_WIDTH;
        attribs[atti++] = EGL_DMA_BUF_PLANE1_FD_EXT;
        attribs[atti++] = dma_fd;
        attribs[atti++] = EGL_DMA_BUF_PLANE1_OFFSET_EXT;
        attribs[atti++] = DVR_WIDTH*DVR_HEIGHT;
        attribs[atti++] = EGL_DMA_BUF_PLANE1_PITCH_EXT;
        attribs[atti++] = DVR_WIDTH;
        attribs[atti++] = EGL_YUV_COLOR_SPACE_HINT_EXT;
        attribs[atti++] = EGL_ITU_REC709_EXT;
        attribs[atti++] = EGL_SAMPLE_RANGE_HINT_EXT;
        attribs[atti++] = EGL_YUV_FULL_RANGE_EXT;
        attribs[atti++] = EGL_NONE;
        egl_img_array[i] = eglCreateImageKHR(dpy, EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, 0, attribs);

        printf("dma_fd:%d, eglCreateImageKHR::EGLImageKHR:egl_img_array:%p\n",dma_fd, egl_img_array[i]);

        if (egl_img_array[i] == EGL_NO_IMAGE_KHR) {
            fprintf(stderr, "Error:0000 failed: 0x%08X\n",glGetError());
            return -1;
        }
    }

    return 0;
}



static int update_dvr_dmafd(int *dmafd01_index, GLuint *texture01_id, int texture_cnt, EGLImageKHR *img )
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
		if(i%2)
		{
			glActiveTexture(GL_TEXTURE1);
		}
		else
		{
			glActiveTexture(GL_TEXTURE0);
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
#endif
