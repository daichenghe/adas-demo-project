/*******************************************************************************
* File Name          : bsp_camera.cpp
* Author             : fengkui
* Revision           : 2.1 
* Date               : 15/05/2018
* Description        : adjust camera parameter
*
* HISTORY***********************************************************************
* 15/05/2018  | sDas Basic 2.0                                            | fengkui
*
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define IOC_NVP6124_REG_WRITE                   0xA0
#define IOC_NVP6124_REG_READ                    0xA1

static int nvp614_fd;

#ifdef T3BOARD

typedef struct _nvp6124_reg_direct_rw
{	
	unsigned char chip;	
	unsigned char reg_addr;
	unsigned char reg_value;	
	unsigned char rw;//0:read, 1:write}nvp6124_reg_direct_rw;
}nvp6124_reg_direct_rw;

int nvp6124_init(void)
{
	nvp614_fd = open("/dev/nvp_cam", O_RDWR | O_NONBLOCK, 0);
	
	if(nvp614_fd < 0)
	{
		printf("open err\n");
		return -1;
	}
	return 0;
}


/**************************************************
0-254  ´Ó°µµ½ÁÁ
 ***************************************************/
int nvp6124_brightness(int BRIGHTNESS_x,int brightness)      
{
	int error = 0;                            
	int ret;

	nvp6124_reg_direct_rw debug_para;
	if((BRIGHTNESS_x > 0x4)|(BRIGHTNESS_x < 0x1))
	{
		printf("%s, Unsupport BRIGHTNESS_x %x\n", __func__, BRIGHTNESS_x);
		error = -1;
	}
	else{
		
		switch (BRIGHTNESS_x) {
			case 0x1:
				debug_para.reg_addr = 0x0c;
				debug_para.chip = 0x00;
				break;
			case 0x2:
				debug_para.reg_addr = 0x0d;
				debug_para.chip = 0x00;
				break;
			case 0x3:
				debug_para.reg_addr = 0x0e;
				debug_para.chip = 0x00;
				break;
			case 0x4:
				debug_para.reg_addr = 0x0f;
				debug_para.chip = 0x00;
				break;
			default:
				error=-1;
				break;
		}
		debug_para.reg_value = 0;
		ret = ioctl (nvp614_fd, IOC_NVP6124_REG_READ, &debug_para);
		if (ret < 0)
		{
			printf("Error opening device: unable to device.\n");
			error=-1;
		}
	//	printf("IOC_NVP6124_REG_READ reg_addr=%x,reg_value=%x\n",debug_para.reg_addr,debug_para.reg_value);
		 
		if((brightness>0xff)|(brightness<0x00))
		{
			printf("Error  brightness input error\n");
			error = -1;
		}
		else{
			if ((brightness >= 0)&&(brightness <= 0x7f))
			{
				brightness += 0x80;
			}
			else if((brightness >= 0x80)&&(brightness <= 0xFE))
			{
				brightness -= 0x7f;	
			}
//			printf("write bright_data is %x\r\n",brightness);
			debug_para.reg_value=debug_para.reg_value & (~(0xff))|(brightness); 
			ret = ioctl (nvp614_fd, IOC_NVP6124_REG_WRITE, &debug_para);
			if (ret < 0)
			{
				printf("Error opening device: unable to device.\n");
				error = -1;
			}
	//		printf("IOC_NVP6124_REG_WRITE reg_addr=%x,reg_value=%x\n",debug_para.reg_addr,debug_para.reg_value);
		}

	}
	return error;
}

int get_nvp6124_brightness(int BRIGHTNESS_x)
{
	int error = 0;                            
	int ret;
	int bright_data;
	nvp6124_reg_direct_rw debug_para;
	if((BRIGHTNESS_x > 0x4)|(BRIGHTNESS_x < 0x1))
	{
		printf("%s, Unsupport BRIGHTNESS_x %x\n", __func__, BRIGHTNESS_x);
		error = -1;
	}
	else{
		
		switch (BRIGHTNESS_x) {
			case 0x1:
				debug_para.reg_addr = 0x0c;
				debug_para.chip = 0x00;
				break;
			case 0x2:
				debug_para.reg_addr = 0x0d;
				debug_para.chip = 0x00;
				break;
			case 0x3:
				debug_para.reg_addr = 0x0e;
				debug_para.chip = 0x00;
				break;
			case 0x4:
				debug_para.reg_addr = 0x0f;
				debug_para.chip = 0x00;
				break;
			default:
				error = -1;
				break;
		}
		debug_para.reg_value = 0;
		ret = ioctl (nvp614_fd, IOC_NVP6124_REG_READ, &debug_para);
		if (ret < 0)
		{
			printf("Error opening device: unable to device.\n");
			error = -1;
		}	
		bright_data = debug_para.reg_value;
//		printf("read bright_data is %d\r\n",bright_data);
		if ((bright_data >= 0x80)&&(bright_data <= 0xff))
			{
				bright_data -= 0x80;
			}
			else if((bright_data >= 0x01)&&(bright_data <= 0x7f))
			{
				bright_data += 0x7f;	
			}
		return bright_data;
//		return debug_para.reg_value;
	}
}
#endif