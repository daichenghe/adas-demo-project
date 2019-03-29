/*******************************************************************************
* File Name          : avmlib_3d.h
* Author             : Jerry
* Revision           : 2.1
* Date               : 15/07/2016
* Description        : Library Interface 
*******************************************************************************/
#ifndef __AVMLIB_3D_H__
#define __AVMLIB_3D_H__

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "avmlib_sdk.h"
#include "avmlib_vectors.h"
#include "avmlib_GenCaliData.h"



//lib使用，不可修改
#define MAX3DOBJECT       4
#define MAX3DTEXTURE      4
#define MAX3DUINFORM      9
#define MAX3DATTRIBUTE    5
#define MAX3DNAME         20

/*
*************************************************************************
*************************************************************************
*                    LOCAL  E N U M E R A T I O N S
*************************************************************************
*************************************************************************
*/
enum avm3d_type_enum/* avm 3d 类型 */
{
    invalid3d_type = 0,//无效
    avm3dview_type,    //3D环视
    car3d_type,        //3D车模，
    tire_type,         //3D车轮
    chassis_type       //3D底盘
};

enum blend3d_type_enum/*blending 类型 */
{
    SRC_DST_ALPHA_3D = 1, //源颜色的alpha值来作为因子，目标颜色的alpha值来作为因子
    SRC_ONE_MINUS_SRC_ALPHA_3D,//源颜色的alpha值来作为因子，1.0减去源颜色的alpha值来作为目标颜色因子
};

/*
*************************************************************************
*************************************************************************
*                      D A T A   S T R U C T U R E S
*************************************************************************
*************************************************************************
*/
struct avm3d_type_;

typedef struct view_3d_parameter_
{
    Vector3 scale;    //缩放因子(X,Y,Z)
    Vector3 eye;      //视点位置(X,Y,Z)
    Vector3 pitchhead;      //pitch head radius
    Vector3 at;       //视点终点位置(X,Y,Z)
    Vector3 up;       //向上方向项量
    Vector4 rotate_axis;//轮子转动轴
    GLfloat rotate_angle;//轮子转角度
    GLfloat offset_x; //X平移
    GLfloat offset_y; //Y平移
    GLfloat offset_z; //Z平移
    GLfloat rotate_x; //绕X轴旋转
    GLfloat rotate_y; //绕Y轴旋转
    GLfloat rotate_z; //绕Z轴旋转
    GLfloat fov ;     //视角范围
    S32 screen_x;     //视口位置x
    S32 screen_y;     //视口位置y
    S32 screen_width; //视口宽度
    S32 screen_height;//视口高度
    U32 reserved;     //保留
} view_3d_parameter_t;

typedef struct shader3d_member_//lib使用，不可修改
{
    C8 name[MAX3DNAME];
    GLuint memberID;
} shader3d_member_t;

typedef struct avm3d_shader_type_
{
    GLuint shaderProgram;//lib使用，不可修改
    GLuint uiniformListNum;//lib使用，不可修改
    GLuint attributeListNum;//lib使用，不可修改
    shader3d_member_t uniformList [MAX3DUINFORM];//lib使用，不可修改
    shader3d_member_t attributeList [MAX3DATTRIBUTE];//lib使用，不可修改
    AVM_ERR(*avmlib_Use)(struct avm3d_type_ *avm3d_type);//激活项目对象
    AVM_ERR(*avmlib_Unuse)(struct avm3d_type_ *avm3d_type);//去激活项目对象
    AVM_ERR(*avmlib_Shader_Init)(struct avm3d_type_ *avm3d_type, const C8 *programName, const C8 *programName1);//着色器初始化
    AVM_ERR(*avmlib_Delete_Shader)(struct avm3d_type_ *avm3d_type);//着色器退出

}  avm3d_shader_type_t;

typedef struct objID_3d_//lib使用，不可修改
{
    GLuint ObjID;
    GLuint objIDPointNum;
} objID_3d_t;

typedef struct avm3d_object_type_
{
    objID_3d_t objID[MAX3DOBJECT];//lib使用，不可修改
    S32    objprivate[16];//lib使用，不可修改
    GLuint usedObjNum;//lib使用，不可修改
    AVM_ERR(*avmlib_Gen_ObjBuffer)(struct avm3d_type_ *avm3d_type, const C8 *file_name, U8 num,lut_parameter_t *plutpar);//定义顶点缓存
    AVM_ERR(*avmlib_Delete_Object)(struct avm3d_type_ *avm3d_type);//删除顶点缓存
} avm3d_object_type_t;

typedef struct avm3d_texture_type_
{
    GLuint TextureID[2];//lib使用，不可修改
	GLuint TextureID_used;//lib使用，不可修改
    GLuint TextureW;//lib使用，不可修改
    GLuint TextureH;//lib使用，不可修改
    GLfloat sTextureCorrectParameter[3];//lib使用，不可修改
    AVM_ERR(*avmlib_Gen_TextrueBuffer)(struct avm3d_type_ *avm3d_type, GLuint texture_width, GLuint texture_heigh, GLenum format, void *pdata);//定义纹理
    AVM_ERR(*avmlib_Update_TextureBuffer)(struct avm3d_type_ *avm3d_type, GLint xoffset, GLint yoffset, GLuint texture_width, GLuint texture_heigh, GLenum format, void *pdata);
    AVM_ERR(*avmlib_Delete_Texture)(struct avm3d_type_ *avm3d_type);//删除纹理
} avm3d_texture_type_t;

typedef struct avm3dviewParameter_//lib使用，不可修改
{
    GLfloat matMVP[16];
    GLfloat matProj[16];
    GLfloat matMview[16];
    U32 screen_x;
    U32 screen_y;
    U32 screen_width;
    U32 screen_height;
    U32 changeflg;
	F32 alpha_scale_mm;
} avm3d_view_Parameter_t;


typedef struct blend3dParameter_
{
    enum blend3d_type_enum blend_type;
    GLfloat   ctl_blend[4];//[0] Blend [1]lightness correct [2]de-interlace(暂不支持)
    GLfloat   rgb_correct[3];//[0]r [1]g [2]b
}  blend_3d_Parameter_t;

typedef struct avm3d_render_type_
{
    blend_3d_Parameter_t blendParameter;//lib使用，不可修改
    avm3d_view_Parameter_t viewParameter;//lib使用，不可修改
    AVM_ERR(*avmlib_Set_Blend)(struct avm3d_type_ *avm3d_type,blend_3d_Parameter_t *parameter);//Blending设置
    AVM_ERR(*avmlib_Set_ViewParameter)(struct avm3d_type_ *avm3d_type, view_3d_parameter_t *viewParameter);//视点参数设置
    AVM_ERR(*avmlib_Render_Topview)(struct avm3d_type_ *avm3d_type);//环视显示
    AVM_ERR(*avmlib_Render_Car)(struct avm3d_type_ *avm3d_type);//车模显示
} avm3d_render_type_t;

typedef struct avm3d_type_
{
    U8     IsUsed;//lib使用，不可修改
    enum   avm3d_type_enum  avm_type;
    avm3d_shader_type_t  *avm3d_shader_type;
    avm3d_object_type_t  *avm3d_object_type;
    avm3d_texture_type_t *avm3d_texture_type;
    avm3d_render_type_t  *avm3d_render_type;
    struct avm3d_type_ *pAttr;  //对于车模所属3D环视
}  avm3d_type_t;

/*
*************************************************************************
*************************************************************************
*                      F U N C T I O N
*************************************************************************
*************************************************************************
*/
/*******************************************************************************************
* Function Name  : avmlib_3d_Init.
*
* Description    : avm 3D初始化
*
* Arguments      : avm3d_type  avm3d_type_t类型指针
*                  -----
*
* Return         : AVM_ERR_NONE   successful.
*                  Other         Other AVM_ERR_xxx   fail.
*
* Note(s)        : avm3d_type在使用之前必须初始化
*******************************************************************************************/
AVM_EXT AVM_ERR avmlib_3d_Init(avm3d_type_t *avm3d_type);

/*******************************************************************************************
* Function Name  : avmlib_3d_Child_Init.
*
* Description    : avm 3d 初始化 avm3d_child_type的avm3d_shader_type
*                  从avm3d_base_type中复制，对于环视的avm3d_object_type是
*                  从avm3d_base_type中复制,车模的avm3d_object_type是重新创建。
* Arguments      : avm3d_child_type  avm3d_type_t类型指针
*                  -----
*
*                  avm3d_base_type   avm3d_type_t类型指针
*                  -----
*
* Return         : AVM_ERR_NONE   successful.
*                  Other         Other AVM_ERR_xxx   fail.
*
* Note(s)        : avm3d_child_type在使用之前必须初始化
*******************************************************************************************/
AVM_EXT AVM_ERR avmlib_3d_Child_Init(avm3d_type_t *avm3d_child_type, avm3d_type_t *avm3d_base_type);

/*******************************************************************************************
* Function Name  : avmlib_3d_Exit.
*
* Description    : avm 3d 退出
*
* Arguments      : avm3d_type avm3d_type_t类型指针
*                  -----
*
* Return         : AVM_ERR_NONE   successful.
*                  Other         Other AVM_ERR_xxx   fail.
*
* Note(s)        : 在avmlib_Delete_Object、avmlib_Delete_Texture调用后再调用此函数
*******************************************************************************************/
AVM_EXT AVM_ERR avmlib_3d_Exit(avm3d_type_t *avm3d_type);





#endif /* AVM3D_H_ */
