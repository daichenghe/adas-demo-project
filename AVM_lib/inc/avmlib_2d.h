/*******************************************************************************
* File Name          : avmlib_2d.h
* Author             : Jerry
* Revision           : 2.1
* Date               : 15/07/2017
* Description        : Library Interface 
*******************************************************************************/

#ifndef __AVMLIB_2D_H__
#define __AVMLIB_2D_H__

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "avmlib_sdk.h"
#include "avmlib_vectors.h"
#include "avmlib_GenCaliData.h"

//lib使用，不可修改
#define MAX2DOBJECT       4
#define MAX2DTEXTURE      4
#define MAX2DUINFORM      8
#define MAX2DATTRIBUTE    5
#define MAX2DNAME         20

/*
*************************************************************************
*************************************************************************
*                    LOCAL  E N U M E R A T I O N S
*************************************************************************
*************************************************************************
*/
enum avm2d_type_enum/* avm 2d 类型 */
{
    invalid2d_type = 0,//无效
    avm2dview_type,    //环视
    avmldwview_type,   //ldw
    singleview_type,   //单视图
    periscopeview_type,
    car2d_type         //车模
};

enum blend2d_type_enum/*blending 类型 */
{
    SRC_DST_ALPHA_2D = 1,//源颜色的alpha值来作为因子，目标颜色的alpha值来作为因子
    SRC_ONE_MINUS_SRC_ALPHA_2D,//源颜色的alpha值来作为因子，1.0减去源颜色的alpha值来作为目标颜色因子
    ONE_ZERO_ALPHA_2D,//使用1.0作为源颜色的因子，0.0作为目标颜色的因子，
    //相当于完全的使用了源颜色参与混合而不使用目标颜色
};

/*
*************************************************************************
*************************************************************************
*                      D A T A   S T R U C T U R E S
*************************************************************************
*************************************************************************
*/
struct avm2d_type_;

typedef struct view_2d_parameter_//2d视点参数
{
    Vector3 scale;   //缩放因子(X,Y,Z;Z无效，固定为1.0)
    Vector3 eye;     //视点位置(X,Y,Z)
    Vector3 at;      //视点终点位置(X,Y,Z)
    Vector3 up;      //向上方向项量
    GLfloat offset_x;//X平移
    GLfloat offset_y;//Y平移
    GLfloat offset_z;//Z平移无效,固定为0.0
    GLfloat rotate_x;//绕X轴旋转
    GLfloat rotate_y;//绕Y轴旋转
    GLfloat rotate_z;//绕Z轴旋转
    GLfloat left;    //视景体left
    GLfloat right;   //视景体right
    GLfloat top;     //视景体top
    GLfloat bottom;  //视景体bottom
    S32 screen_x;    //视口位置x
    S32 screen_y;    //视口位置y
    S32 screen_width;//视口宽度
    S32 screen_height;//视口高度
    U32 reserved;     //保留
} view_2d_parameter_t;

typedef struct shader2d_member_//lib使用，不可修改
{
    C8 name[MAX2DNAME];
    GLuint memberID;
} shader2d_member_t;

typedef struct avm2d_shader_type_
{
    GLuint shaderProgram;//lib使用，不可修改
    GLuint uiniformListNum;//lib使用，不可修改
    GLuint attributeListNum;//lib使用，不可修改
    shader2d_member_t uniformList [MAX2DUINFORM];//lib使用，不可修改
    shader2d_member_t attributeList [MAX2DATTRIBUTE];//lib使用，不可修改
    AVM_ERR(*avmlib_Use)(struct avm2d_type_ *avm2d_type);//激活项目对象
    AVM_ERR(*avmlib_Unuse)(struct avm2d_type_ *avm2d_type);//去激活项目对象
    AVM_ERR(*avmlib_Shader_Init)(struct avm2d_type_ *avm2d_type, const C8 *programName, const C8 *programName1);//着色器初始化
    AVM_ERR(*avmlib_Delete_Shader)(struct avm2d_type_ *avm2d_type);//着色器退出
} avm2d_shader_type_t;
typedef struct objID_2d_//lib使用，不可修改
{
    GLuint ObjID;
    GLuint objIDPointNum;
} objID_2d_t;

typedef struct avm2d_object_type_
{
    objID_2d_t objID[MAX2DOBJECT];//lib使用，不可修改
    S32    objprivate[20];//lib使用，不可修改
    U8     usedObjNum;//lib使用，不可修改
    AVM_ERR(*avmlib_Gen_ObjBuffer)(struct avm2d_type_ *avm2d_type, const C8 *file_name, U8 num,lut_parameter_t* plutpar);//定义顶点缓存
    AVM_ERR(*avmlib_Delete_Object)(struct avm2d_type_ *avm2d_type);//删除顶点缓存
} avm2d_object_type_t;

typedef struct avm2d_texture_type_
{
    GLuint TextureID[2];//lib使用，不可修改
	GLuint TextureID_used;//lib使用，不可修改
    GLuint TextureW;//lib使用，不可修改
    GLuint TextureH;//lib使用，不可修改
    GLfloat sTextureCorrectParameter[3];//lib使用，不可修改
    AVM_ERR(*avmlib_Gen_TextrueBuffer)(struct avm2d_type_ *avm2d_type, GLuint texture_width, GLuint texture_heigh, GLenum format, void *pdata);//定义纹理
    AVM_ERR(*avmlib_Update_TextureBuffer)(struct avm2d_type_ *avm2d_type, GLint xoffset, GLint yoffset, GLuint texture_width, GLuint texture_heigh, GLenum format, void *pdata);
    AVM_ERR(*avmlib_Delete_Texture)(struct avm2d_type_ *avm2d_type);//删除纹理
} avm2d_texture_type_t;


typedef struct blend2dParameter_
{
    enum blend2d_type_enum blend_type;
    GLfloat   ctl_blend[4];  //[0] Blend [1]lightness correct [2]de-interlace
    GLfloat   rgb_correct[3];//[0]r [1]g [2]b
} blend_2d_Parameter_t;

typedef struct avm2dviewParameter_//lib使用，不可修改
{
    GLfloat matMVP[16];
    U32 screen_x;
    U32 screen_y;
    U32 screen_width;
    U32 screen_height;
    U32 changeflg;
} avm2d_view_Parameter_t;

typedef struct avm2d_render_type_
{
    blend_2d_Parameter_t blendParameter;//lib使用，不可修改
    avm2d_view_Parameter_t viewParameter[MAX2DOBJECT];//lib使用，不可修改
    AVM_ERR(*avmlib_Set_Blend)(struct avm2d_type_ *avm2d_type,blend_2d_Parameter_t *parameter);//Blending设置
    AVM_ERR(*avmlib_Set_ViewParameter)(struct avm2d_type_ *avm2d_type, view_2d_parameter_t *viewParameter, U8 Index);//视点参数设置
    AVM_ERR(*avmlib_Render_Singleview)(struct avm2d_type_ *avm2d_type, U8 obj_Index, U8 c_Index);//单视图显示
    AVM_ERR(*avmlib_Render_Topview)(struct avm2d_type_ *avm2d_type);//环视显示
    AVM_ERR(*avmlib_Render_Car)(struct avm2d_type_ *avm2d_type);//车模显示
} avm2d_render_type_t;

typedef struct avm2d_type_
{
    U8  IsUsed;//lib使用，不可修改
    enum avm2d_type_enum  avm_type; //2D 类型
    avm2d_shader_type_t  *avm2d_shader_type; //shader对象 lib使用，不可修改
    avm2d_object_type_t  *avm2d_object_type; //vbo对象  lib使用，不可修改
    avm2d_texture_type_t *avm2d_texture_type;//纹理对象 lib使用，不可修改
    avm2d_render_type_t  *avm2d_render_type; //渲染对象 lib使用，不可修改
    struct avm2d_type_ *pAttr;//车模所属的环视，其他单视野无效
}  avm2d_type_t;

/*
*************************************************************************
*************************************************************************
*                      F U N C T I O N
*************************************************************************
*************************************************************************
*/

/*******************************************************************************************
* Function Name  : avmlib_2d_Init.
*
* Description    : avm 2D初始化
*
* Arguments      : avm2d_type  avm2d_type_t类型指针
*                  -----
*
* Return         : AVM_ERR_NONE   successful.
*                  Other         Other AVM_ERR_xxx   fail.
*
* Note(s)        : avm2d_type在使用之前必须初始化
*******************************************************************************************/
AVM_EXT AVM_ERR avmlib_2d_Init(avm2d_type_t *avm2d_type);

/*******************************************************************************************
* Function Name  : avmlib_2d_Child_Init.
*
* Description    : avm 2d 子模式初始化 avm2d_child_type的avm2d_shader_type和avm2d_texture_type
*                  从avm2d_base_type中复制，对于环视和单视图的avm2d_object_type是
*                  从avm2d_base_type中复制,车模的avm2d_object_type是重新创建。
* Arguments      : avm2d_child_type  avm2d_type_t类型指针
*                  -----
*
*                  avm2d_base_type   avm2d_type_t类型指针
*                  -----
*
* Return         : AVM_ERR_NONE   successful.
*                  Other         Other AVM_ERR_xxx   fail.
*
* Note(s)        : avm2d_child_type在使用之前必须初始化
*******************************************************************************************/
AVM_EXT AVM_ERR avmlib_2d_Child_Init(avm2d_type_t *avm2d_child_type, avm2d_type_t *avm2d_base_type);

/*******************************************************************************************
* Function Name  : avmlib_2d_Exit.
*
* Description    : avm 2d 退出
*
* Arguments      : avm2d_type avm2d_type_t类型指针
*                  -----
*
* Return         : AVM_ERR_NONE   successful.
*                  Other         Other AVM_ERR_xxx   fail.
*
* Note(s)        : 在avmlib_Delete_Object、avmlib_Delete_Texture调用后再调用此函数
*******************************************************************************************/
AVM_EXT AVM_ERR avmlib_2d_Exit(avm2d_type_t *avm2d_type);




#endif /* AVM2D_H_ */
