#ifdef T3BOARD

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include <type_camera.h>
#include <g2d_driver.h>
#include "ion.h"

#ifndef __G2D_HAL_H__
#define __G2D_HAL_H__

typedef struct roi_{
    int roi_x;
    int roi_y;
    int roi_width;
    int roi_height;
}roi_t;

typedef struct g2d_roi_{
    roi_t src;
    int src_width;
    int src_height;
    roi_t dst;
    int dst_width;
    int dst_height;
}g2d_roi_t;

extern void g2d_init(void);
extern int g2d_blit(int src_phy, int dst_phy, g2d_roi_t g2d_roi_para);
extern int g2d_rgba2yuv(int src_phy, int dst_phy, g2d_roi_t g2d_roi_para);
#endif

#endif
