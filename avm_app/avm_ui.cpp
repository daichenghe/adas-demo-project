/*******************************************************************************
* File Name          : avm_ui.h
* Author             : Duqf
* Revision           : 2.1
* Date               : 03/07/2018
* Description        : draw ui
*
* HISTORY***********************************************************************
* 03/07/2018  | sDas Basic 2.0                       | Duqf
*
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "avm_ui.h"
#include "avmlib_sdk.h"
#include "avm_config.h"
#include "avmlib_loadpng.h"
#include "log.h"
#include "avmlib_gpu_drawimg.h"
/*
*************************************************************************
*************************************************************************
*                    LOCAL  STRUCT / E N U M E R A T I O N S
*************************************************************************
*************************************************************************
*/
typedef struct
{
    const char *filename;
    GLuint winx;
    GLuint winy;
    GLuint winw;
    GLuint winh;
} image_info_t;

typedef struct
{
    png_texture_t *png_tex[UI_COUNT];
	GLuint tex_id[UI_COUNT];
    image_info_t *imageinfo;
} ui_obj_t;

/*
*************************************************************************
*************************************************************************
*                     LOCAL  F U N T I O N S
*************************************************************************
*************************************************************************
*/

static int find_tex_id(const char *argfile);

image_info_t ui_image[UI_COUNT] =
{
    {UI_LDWS_L, 0, 0, 720, 480},
    {UI_LDWS_N, 0, 0, 720, 480},
    {UI_LDWS_R, 0, 0, 720, 480},
	
    {UI_BSD_L, 0, 0, 720, 480},
    {UI_BSD_N, 0, 0, 720, 480},
    {UI_BSD_R, 0, 0, 720, 480},
    {UI_BSD_D, 0, 0, 720, 480},

    {UI_2DBGD, 0, 0, SCREEN_WINDOW_WIDTH, SCREEN_WINDOW_HEIGHT},
    {UI_LVFRONT, 258, 0, 100, 101},
    {UI_LVRIGHT, 0, 0, 100, 101},
    {UI_LVBACK,  200, 0, 100, 101},
    {UI_LVLEFT,  0, 0, 100, 101},
    {UI_AUTOCBGD, 0, 0, SCREEN_WINDOW_WIDTH, SCREEN_WINDOW_HEIGHT},
    {UI_AUTOCC,  0, 294, 64, 131},
    {UI_AUTOCCS, 0, 294, 64, 131},
    {UI_AUTOCCE, 0, 294, 64, 131},
    {UI_AUTOCS,  12, 23, 44, 44},
    {UI_AUTOCSO, 12, 23, 44, 44},
    {UI_AUTOCSE, 12, 23, 44, 44},

    {UI_BACKUPBGD, 0, 0, SCREEN_WINDOW_WIDTH, SCREEN_WINDOW_HEIGHT},
    {UI_3DBGD, 0, 0, SCREEN_WINDOW_WIDTH, SCREEN_WINDOW_HEIGHT},
    {UI_PERBGD, 0, 0, SCREEN_WINDOW_WIDTH, SCREEN_WINDOW_HEIGHT},
    {UI_FREEBGD, 0, 0, SCREEN_WINDOW_WIDTH, SCREEN_WINDOW_HEIGHT},

    {UI_LVLEFTB,  335, 618, 100, 101},
    {UI_LVRIGHTB, 335, 618, 100, 101},
    {UI_LVLEFTBJ, 377, 38, 90, 100},
    {UI_LVRIGHTBJ, 16, 38, 90, 100},

    {UI_PERF,  589, 25, 100, 101},
    {UI_PERB,  589, 25, 100, 101},

    {UI_NUMC ,  0, 0, 120, 24},
    {UI_NUM0 ,  0, 0, 12, 24},
    {UI_NUM1 ,  0, 0, 12, 24},
    {UI_NUM2 ,  0, 0, 12, 24},
    {UI_NUM3 ,  0, 0, 12, 24},
    {UI_NUM4 ,  0, 0, 12, 24},
    {UI_NUM5 ,  0, 0, 12, 24},
    {UI_NUM6 ,  0, 0, 12, 24},
    {UI_NUM7 ,  0, 0, 12, 24},
    {UI_NUM8 ,  0, 0, 12, 24},
    {UI_NUM9 ,  0, 0, 12, 24},

};

static ui_obj_t ui_obj;
static int ui_texture_init_flag = 0;
AVM_ERR default_ui_init()
{
	AVM_ERR ret;
	int i;
	ui_obj.imageinfo = ui_image;
	for(i = 0; i < 7; i++)
    {
        ui_obj.png_tex[i]   = loadPNGTexture(ui_obj.imageinfo[i].filename);
        if(ui_obj.png_tex[i] == NULL)
        {
            logg(ERR, "loadPNGTexture err:%s\n", ui_obj.imageinfo[i].filename);
        }
		avmlib_create_texture(ui_obj.png_tex[i]->texels,
								ui_obj.png_tex[i]->format,
								ui_obj.png_tex[i]->width, 
								ui_obj.png_tex[i]->height, 
								0, &ui_obj.tex_id[i]);
    }
	return AVM_ERR_NONE;
}
AVM_ERR common_ui_init()
{
	AVM_ERR ret;
	int i;
	ui_obj.imageinfo = ui_image;
	for(i = 7; i < UI_COUNT; i++)
    {
        ui_obj.png_tex[i]   = loadPNGTexture(ui_obj.imageinfo[i].filename);
        if(ui_obj.png_tex[i] == NULL)
        {
            logg(ERR, "loadPNGTexture err:%s\n", ui_obj.imageinfo[i].filename);
        }
		avmlib_create_texture(ui_obj.png_tex[i]->texels,
						ui_obj.png_tex[i]->format,
						ui_obj.png_tex[i]->width, 
						ui_obj.png_tex[i]->height, 
						0, &ui_obj.tex_id[i]);

    }

	return AVM_ERR_NONE;
}
AVM_ERR draw_ui(const char *argfile)
{
    int arg;
    AVM_ERR ret = AVM_ERR_NONE;
    if(AVM_NULL == argfile)
    {
        return AVM_ERR_FALSE;
    }
	
	if(ui_texture_init_flag == 0)
	{
		ui_texture_init_flag = 1;
		ret = avmlib_DrawPng_Init(AVM_UI_VERT, AVM_UI_FRAG);
		if(ret != AVM_ERR_NONE)
		{
			return AVM_ERR_FALSE;
		}
	}
    arg = find_tex_id(argfile);

    if(arg == -1 || ui_obj.png_tex[arg] == NULL)
    {
        return AVM_ERR_FALSE;
    }

    ui_obj.png_tex[arg]->win_x = ui_obj.imageinfo[arg].winx;
    ui_obj.png_tex[arg]->win_y = ui_obj.imageinfo[arg].winy;
	
	ret = avmlib_Draw_Png(ui_obj.png_tex[arg], ui_obj.tex_id[arg]);
    if(AVM_ERR_NONE != ret)
    {
        return AVM_ERR_FALSE;
    }
    return AVM_ERR_NONE;
}

static int find_tex_id(const char *argfile)
{
    int i;
    ui_obj.imageinfo = ui_image;

    for(i = 0; i < UI_COUNT; i++)
    {
        if(0 == strcmp(ui_obj.imageinfo[i].filename, argfile))
        {
			return i;
            break;
        }
    }

    return -1;
}