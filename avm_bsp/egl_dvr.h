/*******************************************************************************
* File Name          : egl_dvr.cpp
* Author             : provide by allwinner
* Revision           : 2.1 
* Date               : 22/10/2017
* Description        : dvr
*******************************************************************************/
#ifdef T3BOARD

#ifndef __EGL_DVR_H__
#define __EGL_DVR_H__

#include "video_hal.h"

#ifdef __cplusplus
extern "C" {
#endif
extern void dvr_test(void);
extern void update_mp4_buf(video_buf_t *video_buf);
extern void dvr_es_init( void);
extern void dvr_es_render(int player_state);
extern void post_dvr_sem(void);
#ifdef __cplusplus
}
#endif
#endif

#endif