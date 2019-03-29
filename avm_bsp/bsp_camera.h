#ifndef __BSP_CAMERA_H__
#define  __BSP_CAMERA_H__


//export

extern int nvp6124_init(void);
extern int nvp6124_brightness(int BRIGHTNESS_x,int brightness); 

extern int get_nvp6124_brightness(int BRIGHTNESS_x);

#endif