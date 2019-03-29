/*******************************************************************************
* File Name          : avm_line.cpp
* Author             : Daich 
* Revision           : 2.1
* Date               : 16/05/2018
* Description        : draw_line interface
*******************************************************************************/
#include <stdio.h>
#include "avm_line.h"
#include "avm_config.h"

AVM_ERR default_line_init()
{  
	AVM_ERR ret;
	ret = avmlib_Drawline_Init(AVM_LINE_VERT,AVM_LINE_FRAG);
	if (AVM_ERR_NONE != ret)
	{
		DbgPrintf("avmlib_Drawline_Init error!\n");
	}
	avm_line_sv_init();
	avm_line_2d_init();

  return AVM_ERR_NONE;
}

AVM_ERR common_line_init()
{
  avm_line_ori_init();
  
  return AVM_ERR_NONE;
}
AVM_ERR line_handler(view_keep_t *view_keep)
{
	switch(view_keep->mode)
	{
		case MODE_AVM_2D:
			avm_line_sv_draw(view_keep->sub_mode,30);
			avm_line_2d_draw(view_keep->sub_mode,30);
			break;
		case MODE_SV:
			break;
		case MODE_PER:
			break;
		case MODE_ORI:
			break;
		case MODE_SVFS:
			render_2d_avm();
			render_svfs_view();
			break;
		case MODE_BACKUP:
			break;
		case MODE_AVM_3D:
			break;
		case MODE_AVM_LDWS:
			break;
		case MODE_AVM_BSD:
			break;
		
	}
}

