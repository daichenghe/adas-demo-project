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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "avm_config.h"

typedef struct FB {  
    unsigned char *bits;  
    unsigned size;  
    int fd;  
    struct fb_fix_screeninfo fi;
    struct fb_var_screeninfo vi;
}fb_s; 
	
static fb_s s_fb0;

static unsigned char * screen_data_get(fb_s *fb);

int screen_fb_init(void)
{
    s_fb0.fd = open("/dev/fb0", O_RDONLY);
    if (s_fb0.fd < 0) {
        printf("open(\"/dev/fb0\") failed!\n");  
        return -1;  
    }  
    s_fb0.bits = (unsigned char *)mmap(0, SCREEN_WINDOW_WIDTH*SCREEN_WINDOW_HEIGHT*4*2, PROT_READ, MAP_SHARED, s_fb0.fd, 0);
    if (s_fb0.bits == MAP_FAILED) {
        printf("mmap() failed!\n");  
        goto fail;  
    }  
  
    return 0;  
  
fail:  
    close(s_fb0.fd);
    return -1;  
}  

static unsigned char * screen_data_get(fb_s *fb)
{
    int offset = 0;

    if (fb)
    {
        struct fb_var_screeninfo var;
        struct fb_fix_screeninfo finfo;
        ioctl(fb->fd,FBIOGET_FSCREENINFO,&finfo);
        ioctl(fb->fd,FBIOGET_VSCREENINFO,&var);
   //     printf("var.yoffset %d\n",var.yoffset);
   //    printf("var.xres %d\n",var.xres);
   //     printf("var.yres %d\n",var.yres);
   //     printf("vinfo.bits_per_bits=%d\n",var.bits_per_pixel);
        if(var.yoffset == 0)
            offset = 0;
        else
            offset = var.xres *var.yres * 4;
    }
    return fb->bits + offset;
}
void display_buffer_get(int screen_x, int screen_y, int screen_width, int screen_height, unsigned char *buffer_tmp)
{

	int i,j;
	int screen_start_y = SCREEN_WINDOW_HEIGHT - screen_y;
	int screen_end_y = SCREEN_WINDOW_HEIGHT - screen_y- screen_height;
	int step_src = screen_width<<2;
	int step_des = SCREEN_WINDOW_WIDTH<<2;
	unsigned char * display_buffer_start;
	long off_set_tmp;
	long off_set;


	unsigned char *image_buffer = NULL;
	image_buffer = screen_data_get(&s_fb0);
	
	display_buffer_start = image_buffer + (screen_start_y*SCREEN_WINDOW_WIDTH + screen_x)*4 ;

	
		
	for (j = 0; j < screen_height; j++)
	{

		memcpy(buffer_tmp, display_buffer_start, step_src); // 6ms
		buffer_tmp += step_src;
		display_buffer_start -= step_des;
	}

}
#endif
