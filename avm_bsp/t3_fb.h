/*******************************************************************************
* File Name          : t3_fb.cpp
* Author             : Daich
* Revision           : 2.1
* Date               : 03/08/2018
* Description        : layer manage code
*
* HISTORY***********************************************************************
* 03/08/2018  | sDas Basic 2.0                       | Daich
*
*******************************************************************************/
#ifdef T3BOARD

#ifndef __T3_FB_H
#define __T3_FB_H

extern int screen_fb_init(void);
extern void display_buffer_get(int screen_x, int screen_y, int screen_width, int screen_height, unsigned char *buffer_tmp);

#endif

#endif