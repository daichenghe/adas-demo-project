#ifdef T3BOARD

#include "g2d_hal.h"
#include "log.h"
#include "ion_lib.h"

static int mG2DHandle;

void g2d_init(void)
{
	ion_open();
    // open MP driver
    mG2DHandle = open("/dev/g2d", O_RDWR, 0);
    if (mG2DHandle < 0)
    {
	    logg(ERR,"open /dev/g2d failed");
        return ;
    }
    logg(MESSAGE,"open /dev/g2d OK \n");
}

int g2d_blit(int src_phy, int dst_phy, g2d_roi_t g2d_roi_para)
{
    g2d_stretchblt scale;

    scale.flag = G2D_BLT_NONE;//G2D_BLT_NONE;//G2D_BLT_PIXEL_ALPHA|G2D_BLT_ROTATE90;
    scale.src_image.addr[0] = (int)src_phy;
    scale.src_image.addr[1] = (int)src_phy + g2d_roi_para.src_width * g2d_roi_para.src_height;
    scale.src_image.w = g2d_roi_para.src_width;
    scale.src_image.h = g2d_roi_para.src_height;
    scale.src_image.format = G2D_FMT_PYUV420UVC;
    scale.src_image.pixel_seq = G2D_SEQ_NORMAL;
    scale.src_rect.x = g2d_roi_para.src.roi_x;
    scale.src_rect.y = g2d_roi_para.src.roi_y;
    scale.src_rect.w = g2d_roi_para.src.roi_width;
    scale.src_rect.h = g2d_roi_para.src.roi_height;

    scale.dst_image.addr[0] = (int)dst_phy;
    scale.dst_image.addr[1] = (int)dst_phy + g2d_roi_para.dst_width * g2d_roi_para.dst_height;
    //scale.dst_image.addr[2] = (int)dst + dst_width * dst_height * 5 / 4;
    scale.dst_image.w = g2d_roi_para.dst_width;
    scale.dst_image.h = g2d_roi_para.dst_height;
    scale.dst_image.format =G2D_FMT_PYUV420UVC;// G2D_FMT_PYUV420UVC;
    scale.dst_image.pixel_seq = G2D_SEQ_NORMAL;
    scale.dst_rect.x = g2d_roi_para.dst.roi_x;
    scale.dst_rect.y = g2d_roi_para.dst.roi_y;
    scale.dst_rect.w = g2d_roi_para.dst.roi_width;
    scale.dst_rect.h = g2d_roi_para.dst.roi_height;
    scale.color = 0xff;
    scale.alpha = 0xff;

    if(ioctl(mG2DHandle, G2D_CMD_STRETCHBLT, &scale)<0){
        return -1;
    }
    return 0;

}


int g2d_rgba2yuv(int src_phy, int dst_phy, g2d_roi_t g2d_roi_para)
{
    g2d_stretchblt scale;

    //scale.flag = G2D_BLT_NONE;//G2D_BLT_NONE;//G2D_BLT_PIXEL_ALPHA|G2D_BLT_ROTATE90;
    scale.flag = G2D_BLT_FLIP_VERTICAL;                      //y方向翻转
    scale.src_image.addr[0] = (int)src_phy;
    scale.src_image.addr[1] = (int)src_phy + g2d_roi_para.src_width * g2d_roi_para.src_height;
    scale.src_image.w = g2d_roi_para.src_width;
    scale.src_image.h = g2d_roi_para.src_height;
    scale.src_image.format = G2D_FMT_BGRX8888;
    scale.src_image.pixel_seq = G2D_SEQ_NORMAL;
    scale.src_rect.x = g2d_roi_para.src.roi_x;
    scale.src_rect.y = g2d_roi_para.src.roi_y;
    scale.src_rect.w = g2d_roi_para.src.roi_width;
    scale.src_rect.h = g2d_roi_para.src.roi_height;

    scale.dst_image.addr[0] = (int)dst_phy;
    scale.dst_image.addr[1] = (int)dst_phy + g2d_roi_para.dst_width * g2d_roi_para.dst_height;
    //scale.dst_image.addr[2] = (int)dst + dst_width * dst_height * 5 / 4;
    scale.dst_image.w = g2d_roi_para.dst_width;
    scale.dst_image.h = g2d_roi_para.dst_height;
    scale.dst_image.format = G2D_FMT_PYUV420UVC;// G2D_FMT_PYUV420UVC;
    scale.dst_image.pixel_seq = G2D_SEQ_NORMAL;
    scale.dst_rect.x = g2d_roi_para.dst.roi_x;
    scale.dst_rect.y = g2d_roi_para.dst.roi_y;
    scale.dst_rect.w = g2d_roi_para.dst.roi_width;
    scale.dst_rect.h = g2d_roi_para.dst.roi_height;
    scale.color = 0xff;
    scale.alpha = 0xff;

    if(ioctl(mG2DHandle, G2D_CMD_STRETCHBLT, &scale)<0){
        return -1;
    }
    return 0;

}
#endif


