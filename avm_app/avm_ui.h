/*******************************************************************************
* File Name          : avm_ui.h
* Author             : Duqf
* Revision           : 2.1
* Date               : 03/07/2018
* Description        :
*
* HISTORY***********************************************************************
* 03/07/2018  | sDas Basic 2.0                       | Duqf
*
*******************************************************************************/
#ifndef __AVM_UI_H__
#define __AVM_UI_H__

#include "avmlib_sdk.h"

/*
*************************************************************************
*************************************************************************
*                    LOCAL  DEFINE
*************************************************************************
*************************************************************************
*/

#define UI_LDWS_L	"media/ldws_l.png"
#define UI_LDWS_N	"media/ldws_n.png"
#define UI_LDWS_R	"media/ldws_r.png"

#define UI_BSD_L	"media/bsd_l.png"
#define UI_BSD_N	"media/bsd_n.png"
#define UI_BSD_R	"media/bsd_r.png"
#define UI_BSD_D	"media/bsd_d.png"

#define UI_2DBGD  "media/2dbgd.png"
#define UI_3DBGD  "media/3dbgd.png"
#define UI_PERBGD "media/prebdg.png"
#define UI_PERF   "media/pref.png"
#define UI_PERB   "media/preb.png"

#define UI_AUTOCBGD "media/autocbdg.png"
#define UI_FREEBGD  "media/bsbdg.png"
#define UI_BACKUPBGD "media/back.png"

#define UI_LVFRONT "media/lvf.png"
#define UI_LVRIGHT "media/lvr.png"
#define UI_LVBACK  "media/lvb.png"
#define UI_LVLEFT  "media/lvl.png"
#define UI_LVRIGHTB  "media/lvrb.png"
#define UI_LVLEFTB  "media/lvlb.png"
#define UI_LVLEFTBJ  "media/leftbj.png"
#define UI_LVRIGHTBJ  "media/rightbj.png"
#define UI_3DB   "media/3db.png"
#define UI_3DF   "media/3df.png"
#define UI_3DL   "media/3dl.png"
#define UI_3DR   "media/3dr.png"
#define UI_3DFR  "media/3dfr.png"
#define UI_3DFL  "media/3dfl.png"
#define UI_3DLB  "media/3dlb.png"
#define UI_3DRB  "media/3drb.png"


#define UI_AUTOCC   "media/autocc.png"
#define UI_AUTOCCS   "media/autoco.png"
#define UI_AUTOCCE   "media/autoce.png"

#define UI_AUTOCS    "media/floppy.png"
#define UI_AUTOCSO   "media/floppyo.png"
#define UI_AUTOCSE   "media/floppye.png"


#define UI_NUMC      "media/numc.png"
#define UI_NUM0     "media/num0.png"
#define UI_NUM1     "media/num1.png"
#define UI_NUM2     "media/num2.png"
#define UI_NUM3     "media/num3.png"
#define UI_NUM4     "media/num4.png"
#define UI_NUM5     "media/num5.png"
#define UI_NUM6     "media/num6.png"
#define UI_NUM7     "media/num7.png"
#define UI_NUM8     "media/num8.png"
#define UI_NUM9     "media/num9.png"

#define UI_COUNT   40

/*
*************************************************************************
*************************************************************************
*                      F U N C T I O N
*************************************************************************
*************************************************************************
*/
AVM_ERR default_ui_init();
AVM_ERR common_ui_init();
AVM_ERR draw_ui(const char *argfile);

#endif