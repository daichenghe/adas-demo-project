/*******************************************************************************
* File Name          : egl_dvr.cpp
* Author             : provide by allwinner
* Revision           : 2.1 
* Date               : 22/10/2017
* Description        : dvr
*******************************************************************************/

#ifdef T3BOARD

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "video_hal.h"
#include "egl_img.h"
#include "avm_config.h"

#ifndef __EGL_T3_H__
#define __EGL_T3_H__

#define MAX_ION_DMAFD_ARRAY_NUM	2
#define MAX_ION_DMAFD_QUEUE_NUM 8
#define MAX_DMA_FD_NUM			(MAX_ION_DMAFD_ARRAY_NUM * MAX_ION_DMAFD_QUEUE_NUM)	




#ifdef __cplusplus
extern "C" {
#endif

extern int disenable_new_context(void);
extern int enable_new_context(void);
extern int get_t3_video_buf(video_buf_t *video_buf);
extern void *thread_egl_funtion(void *_args);
extern void dmafd_init(void);
extern void es_init(void);
extern void egl_swap(void);
extern void fps_calc(void);
extern void update_dma_texture(void);
extern int egl_esContext_get(ESContext **context);
extern void avm_User_Stub_Textures(GLint TexturesID, GLint *OutTexturesID);
extern void get_camera_yuv_data(unsigned char *video_0_buffer, unsigned char *video_1_buffer);

#ifdef IMAGE_DEBUG
extern void image_simulate(void);

#endif

#ifdef __cplusplus
}
#endif
#endif

#endif
