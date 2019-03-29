/*
    Jonathan Dummer
    2007-07-26-10.36

    Simple OpenGL Image Library

    Public Domain
    using Sean Barret's stb_image as a base

    Thanks to:
    * Sean Barret - for the awesome stb_image
    * Dan Venkitachalam - for finding some non-compliant DDS files, and patching some explicit casts
    * everybody at gamedev.net
*/

#define SOIL_CHECK_FOR_GL_ERRORS 0

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wingdi.h>
#include <GL/gl.h>
#elif defined(__APPLE__) || defined(__APPLE_CC__)
/*  I can't test this Apple stuff!  */
#include <OpenGL/gl.h>
#include <Carbon/Carbon.h>
#define APIENTRY
#else
#if !defined(EGL_EGLEXT_PROTOTYPES)
# define EGL_EGLEXT_PROTOTYPES
#endif

#if !defined(GL_GLEXT_PROTOTYPES)
#define GL_GLEXT_PROTOTYPES
#endif

#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#endif

#include "SOIL.h"
#include "stb_image_aug.h"
#include "image_helper.h"
#include "image_DXT.h"

#include <stdlib.h>
#include <string.h>

/*  error reporting */
char *result_string_pointer;// = "SOIL initialized";

/*  for loading chassis maps   */
enum
{
    SOIL_CAPABILITY_UNKNOWN = -1,
    SOIL_CAPABILITY_NONE = 0,
    SOIL_CAPABILITY_PRESENT = 1
};
static int has_chassismap_capability = SOIL_CAPABILITY_UNKNOWN;
int query_chassismap_capability(void);
#define SOIL_TEXTURE_WRAP_R                 0x8072
#define SOIL_CLAMP_TO_EDGE                  0x812F
#define SOIL_NORMAL_MAP                     0x8511
#define SOIL_REFLECTION_MAP                 0x8512
#define SOIL_TEXTURE_CUBE_MAP               0x8513
#define SOIL_TEXTURE_BINDING_CUBE_MAP       0x8514
#define SOIL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define SOIL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define SOIL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define SOIL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
#define SOIL_PROXY_TEXTURE_CUBE_MAP         0x851B
#define SOIL_MAX_CUBE_MAP_TEXTURE_SIZE      0x851C
/*  for non-power-of-two texture    */
static int has_NPOT_capability = SOIL_CAPABILITY_UNKNOWN;
int query_NPOT_capability(void);
/*  for texture rectangles  */
static int has_tex_rectangle_capability = SOIL_CAPABILITY_UNKNOWN;
int query_tex_rectangle_capability(void);
#define SOIL_TEXTURE_RECTANGLE_ARB              0x84F5
#define SOIL_MAX_RECTANGLE_TEXTURE_SIZE_ARB     0x84F8
/*  for using DXT compression   */
static int has_DXT_capability = SOIL_CAPABILITY_UNKNOWN;
int query_DXT_capability(void);
#define SOIL_RGB_S3TC_DXT1      0x83F0
#define SOIL_RGBA_S3TC_DXT1     0x83F1
#define SOIL_RGBA_S3TC_DXT3     0x83F2
#define SOIL_RGBA_S3TC_DXT5     0x83F3
typedef void (GL_APIENTRY *P_SOIL_GLCOMPRESSEDTEXIMAGE2DPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
P_SOIL_GLCOMPRESSEDTEXIMAGE2DPROC soilGlCompressedTexImage2D = NULL;
unsigned int SOIL_direct_load_DDS(
    const char *filename,
    unsigned int reuse_texture_ID,
    int flags,
    int loading_as_chassismap);
unsigned int SOIL_direct_load_DDS_from_memory(
    const unsigned char *const buffer,
    int buffer_length,
    unsigned int reuse_texture_ID,
    int flags,
    int loading_as_chassismap);
/*  other functions */
unsigned int
SOIL_internal_create_OGL_texture
(
    const unsigned char *const data,
    int width, int height, int channels,
    unsigned int reuse_texture_ID,
    unsigned int flags,
    unsigned int opengl_texture_type,
    unsigned int opengl_texture_target,
    unsigned int texture_check_size_enum
);
//extern void G2D_yuv2rgb(const unsigned char *dsrc, unsigned char *ddst, int w, int h);
void yuv2rgb(const unsigned char *dsrc, unsigned char *ddst, int w, int h);
/*  and the code magic begins here [8^) */
unsigned int
SOIL_load_OGL_texture
(
    const char *filename,
    int force_channels,
    unsigned int reuse_texture_ID,
    unsigned int flags
)
{
    /*  variables   */
    unsigned char *img;
    int width, height, channels;
    unsigned int tex_id;
    /*  does the user want direct uploading of the image as a DDS file? */
    if(flags & SOIL_FLAG_DDS_LOAD_DIRECT)
    {
        /*  1st try direct loading of the image as a DDS file
            note: direct uploading will only load what is in the
            DDS file, no MIPmaps will be generated, the image will
            not be flipped, etc.    */
        tex_id = SOIL_direct_load_DDS(filename, reuse_texture_ID, flags, 0);
        if(tex_id)
        {
            /*  hey, it worked!!    */
            return tex_id;
        }
    }
    /*  try to load the image   */
    img = SOIL_load_image(filename, &width, &height, &channels, force_channels);
    /*  channels holds the original number of channels, which may have been forced  */
    if((force_channels >= 1) && (force_channels <= 4))
    {
        channels = force_channels;
    }
    if(NULL == img)
    {
        /*  image loading failed    */
        result_string_pointer = stbi_failure_reason();
        return 0;
    }
    /*  OK, make it a texture!  */
    tex_id = SOIL_internal_create_OGL_texture(
                 img, width, height, channels,
                 reuse_texture_ID, flags,
                 GL_TEXTURE_2D, GL_TEXTURE_2D,
                 GL_MAX_TEXTURE_SIZE);
    /*  and nuke the image data */
    SOIL_free_image_data(img);
    /*  and return the handle, such as it is    */
    return tex_id;
}

unsigned int
SOIL_load_OGL_HDR_texture
(
    const char *filename,
    int fake_HDR_format,
    int rescale_to_max,
    unsigned int reuse_texture_ID,
    unsigned int flags
)
{
    /*  variables   */
    unsigned char *img;
    int width, height, channels;
    unsigned int tex_id;
    /*  no direct uploading of the image as a DDS file  */
    /* error check */
    if((fake_HDR_format != SOIL_HDR_RGBE) &&
       (fake_HDR_format != SOIL_HDR_RGBdivA) &&
       (fake_HDR_format != SOIL_HDR_RGBdivA2))
    {
        result_string_pointer = (char *)"Invalid fake HDR format specified";
        return 0;
    }
    /*  try to load the image (only the HDR type) */
    img = stbi_hdr_load_rgbe(filename, &width, &height, &channels, 4);
    /*  channels holds the original number of channels, which may have been forced  */
    if(NULL == img)
    {
        /*  image loading failed    */
        result_string_pointer = stbi_failure_reason();
        return 0;
    }
    /* the load worked, do I need to convert it? */
    if(fake_HDR_format == SOIL_HDR_RGBdivA)
    {
        RGBE_to_RGBdivA(img, width, height, rescale_to_max);
    }
    else if(fake_HDR_format == SOIL_HDR_RGBdivA2)
    {
        RGBE_to_RGBdivA2(img, width, height, rescale_to_max);
    }
    /*  OK, make it a texture!  */
    tex_id = SOIL_internal_create_OGL_texture(
                 img, width, height, channels,
                 reuse_texture_ID, flags,
                 GL_TEXTURE_2D, GL_TEXTURE_2D,
                 GL_MAX_TEXTURE_SIZE);
    /*  and nuke the image data */
    SOIL_free_image_data(img);
    /*  and return the handle, such as it is    */
    return tex_id;
}

unsigned int
SOIL_load_OGL_texture_from_memory
(
    const unsigned char *const buffer,
    int buffer_length,
    int force_channels,
    unsigned int reuse_texture_ID,
    unsigned int flags
)
{
    /*  variables   */
    unsigned char *img;
    int width, height, channels;
    unsigned int tex_id;
    /*  does the user want direct uploading of the image as a DDS file? */
    if(flags & SOIL_FLAG_DDS_LOAD_DIRECT)
    {
        /*  1st try direct loading of the image as a DDS file
            note: direct uploading will only load what is in the
            DDS file, no MIPmaps will be generated, the image will
            not be flipped, etc.    */
        tex_id = SOIL_direct_load_DDS_from_memory(
                     buffer, buffer_length,
                     reuse_texture_ID, flags, 0);
        if(tex_id)
        {
            /*  hey, it worked!!    */
            return tex_id;
        }
    }
    /*  try to load the image   */
    img = SOIL_load_image_from_memory(
              buffer, buffer_length,
              &width, &height, &channels,
              force_channels);
    /*  channels holds the original number of channels, which may have been forced  */
    if((force_channels >= 1) && (force_channels <= 4))
    {
        channels = force_channels;
    }
    if(NULL == img)
    {
        /*  image loading failed    */
        result_string_pointer = stbi_failure_reason();
        return 0;
    }
    /*  OK, make it a texture!  */
    tex_id = SOIL_internal_create_OGL_texture(
                 img, width, height, channels,
                 reuse_texture_ID, flags,
                 GL_TEXTURE_2D, GL_TEXTURE_2D,
                 GL_MAX_TEXTURE_SIZE);
    /*  and nuke the image data */
    SOIL_free_image_data(img);
    /*  and return the handle, such as it is    */
    return tex_id;
}

unsigned int
SOIL_load_OGL_chassismap
(
    const char *x_pos_file,
    const char *x_neg_file,
    const char *y_pos_file,
    const char *y_neg_file,
    const char *z_pos_file,
    const char *z_neg_file,
    int force_channels,
    unsigned int reuse_texture_ID,
    unsigned int flags
)
{
    /*  variables   */
    unsigned char *img;
    int width, height, channels;
    unsigned int tex_id;
    /*  error checking  */
    if((x_pos_file == NULL) ||
       (x_neg_file == NULL) ||
       (y_pos_file == NULL) ||
       (y_neg_file == NULL) ||
       (z_pos_file == NULL) ||
       (z_neg_file == NULL))
    {
        result_string_pointer = (char *)"Invalid chassis map files list";
        return 0;
    }
    /*  capability checking */
    if(query_chassismap_capability() != SOIL_CAPABILITY_PRESENT)
    {
        result_string_pointer = (char *)"No chassis map capability present";
        return 0;
    }
    /*  1st face: try to load the image */
    img = SOIL_load_image(x_pos_file, &width, &height, &channels, force_channels);
    /*  channels holds the original number of channels, which may have been forced  */
    if((force_channels >= 1) && (force_channels <= 4))
    {
        channels = force_channels;
    }
    if(NULL == img)
    {
        /*  image loading failed    */
        result_string_pointer = stbi_failure_reason();
        return 0;
    }
    /*  upload the texture, and create a texture ID if necessary    */
    tex_id = SOIL_internal_create_OGL_texture(
                 img, width, height, channels,
                 reuse_texture_ID, flags,
                 SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_POSITIVE_X,
                 SOIL_MAX_CUBE_MAP_TEXTURE_SIZE);
    /*  and nuke the image data */
    SOIL_free_image_data(img);
    /*  continue?   */
    if(tex_id != 0)
    {
        /*  1st face: try to load the image */
        img = SOIL_load_image(x_neg_file, &width, &height, &channels, force_channels);
        /*  channels holds the original number of channels, which may have been forced  */
        if((force_channels >= 1) && (force_channels <= 4))
        {
            channels = force_channels;
        }
        if(NULL == img)
        {
            /*  image loading failed    */
            result_string_pointer = stbi_failure_reason();
            return 0;
        }
        /*  upload the texture, but reuse the assigned texture ID   */
        tex_id = SOIL_internal_create_OGL_texture(
                     img, width, height, channels,
                     tex_id, flags,
                     SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                     SOIL_MAX_CUBE_MAP_TEXTURE_SIZE);
        /*  and nuke the image data */
        SOIL_free_image_data(img);
    }
    /*  continue?   */
    if(tex_id != 0)
    {
        /*  1st face: try to load the image */
        img = SOIL_load_image(y_pos_file, &width, &height, &channels, force_channels);
        /*  channels holds the original number of channels, which may have been forced  */
        if((force_channels >= 1) && (force_channels <= 4))
        {
            channels = force_channels;
        }
        if(NULL == img)
        {
            /*  image loading failed    */
            result_string_pointer = stbi_failure_reason();
            return 0;
        }
        /*  upload the texture, but reuse the assigned texture ID   */
        tex_id = SOIL_internal_create_OGL_texture(
                     img, width, height, channels,
                     tex_id, flags,
                     SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                     SOIL_MAX_CUBE_MAP_TEXTURE_SIZE);
        /*  and nuke the image data */
        SOIL_free_image_data(img);
    }
    /*  continue?   */
    if(tex_id != 0)
    {
        /*  1st face: try to load the image */
        img = SOIL_load_image(y_neg_file, &width, &height, &channels, force_channels);
        /*  channels holds the original number of channels, which may have been forced  */
        if((force_channels >= 1) && (force_channels <= 4))
        {
            channels = force_channels;
        }
        if(NULL == img)
        {
            /*  image loading failed    */
            result_string_pointer = stbi_failure_reason();
            return 0;
        }
        /*  upload the texture, but reuse the assigned texture ID   */
        tex_id = SOIL_internal_create_OGL_texture(
                     img, width, height, channels,
                     tex_id, flags,
                     SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                     SOIL_MAX_CUBE_MAP_TEXTURE_SIZE);
        /*  and nuke the image data */
        SOIL_free_image_data(img);
    }
    /*  continue?   */
    if(tex_id != 0)
    {
        /*  1st face: try to load the image */
        img = SOIL_load_image(z_pos_file, &width, &height, &channels, force_channels);
        /*  channels holds the original number of channels, which may have been forced  */
        if((force_channels >= 1) && (force_channels <= 4))
        {
            channels = force_channels;
        }
        if(NULL == img)
        {
            /*  image loading failed    */
            result_string_pointer = stbi_failure_reason();
            return 0;
        }
        /*  upload the texture, but reuse the assigned texture ID   */
        tex_id = SOIL_internal_create_OGL_texture(
                     img, width, height, channels,
                     tex_id, flags,
                     SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                     SOIL_MAX_CUBE_MAP_TEXTURE_SIZE);
        /*  and nuke the image data */
        SOIL_free_image_data(img);
    }
    /*  continue?   */
    if(tex_id != 0)
    {
        /*  1st face: try to load the image */
        img = SOIL_load_image(z_neg_file, &width, &height, &channels, force_channels);
        /*  channels holds the original number of channels, which may have been forced  */
        if((force_channels >= 1) && (force_channels <= 4))
        {
            channels = force_channels;
        }
        if(NULL == img)
        {
            /*  image loading failed    */
            result_string_pointer = stbi_failure_reason();
            return 0;
        }
        /*  upload the texture, but reuse the assigned texture ID   */
        tex_id = SOIL_internal_create_OGL_texture(
                     img, width, height, channels,
                     tex_id, flags,
                     SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
                     SOIL_MAX_CUBE_MAP_TEXTURE_SIZE);
        /*  and nuke the image data */
        SOIL_free_image_data(img);
    }
    /*  and return the handle, such as it is    */
    return tex_id;
}

unsigned int
SOIL_load_OGL_chassismap_from_memory
(
    const unsigned char *const x_pos_buffer,
    int x_pos_buffer_length,
    const unsigned char *const x_neg_buffer,
    int x_neg_buffer_length,
    const unsigned char *const y_pos_buffer,
    int y_pos_buffer_length,
    const unsigned char *const y_neg_buffer,
    int y_neg_buffer_length,
    const unsigned char *const z_pos_buffer,
    int z_pos_buffer_length,
    const unsigned char *const z_neg_buffer,
    int z_neg_buffer_length,
    int force_channels,
    unsigned int reuse_texture_ID,
    unsigned int flags
)
{
    /*  variables   */
    unsigned char *img;
    int width, height, channels;
    unsigned int tex_id;
    /*  error checking  */
    if((x_pos_buffer == NULL) ||
       (x_neg_buffer == NULL) ||
       (y_pos_buffer == NULL) ||
       (y_neg_buffer == NULL) ||
       (z_pos_buffer == NULL) ||
       (z_neg_buffer == NULL))
    {
        result_string_pointer = (char *)"Invalid chassis map buffers list";
        return 0;
    }
    /*  capability checking */
    if(query_chassismap_capability() != SOIL_CAPABILITY_PRESENT)
    {
        result_string_pointer = (char *)"No chassis map capability present";
        return 0;
    }
    /*  1st face: try to load the image */
    img = SOIL_load_image_from_memory(
              x_pos_buffer, x_pos_buffer_length,
              &width, &height, &channels, force_channels);
    /*  channels holds the original number of channels, which may have been forced  */
    if((force_channels >= 1) && (force_channels <= 4))
    {
        channels = force_channels;
    }
    if(NULL == img)
    {
        /*  image loading failed    */
        result_string_pointer = stbi_failure_reason();
        return 0;
    }
    /*  upload the texture, and create a texture ID if necessary    */
    tex_id = SOIL_internal_create_OGL_texture(
                 img, width, height, channels,
                 reuse_texture_ID, flags,
                 SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_POSITIVE_X,
                 SOIL_MAX_CUBE_MAP_TEXTURE_SIZE);
    /*  and nuke the image data */
    SOIL_free_image_data(img);
    /*  continue?   */
    if(tex_id != 0)
    {
        /*  1st face: try to load the image */
        img = SOIL_load_image_from_memory(
                  x_neg_buffer, x_neg_buffer_length,
                  &width, &height, &channels, force_channels);
        /*  channels holds the original number of channels, which may have been forced  */
        if((force_channels >= 1) && (force_channels <= 4))
        {
            channels = force_channels;
        }
        if(NULL == img)
        {
            /*  image loading failed    */
            result_string_pointer = stbi_failure_reason();
            return 0;
        }
        /*  upload the texture, but reuse the assigned texture ID   */
        tex_id = SOIL_internal_create_OGL_texture(
                     img, width, height, channels,
                     tex_id, flags,
                     SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                     SOIL_MAX_CUBE_MAP_TEXTURE_SIZE);
        /*  and nuke the image data */
        SOIL_free_image_data(img);
    }
    /*  continue?   */
    if(tex_id != 0)
    {
        /*  1st face: try to load the image */
        img = SOIL_load_image_from_memory(
                  y_pos_buffer, y_pos_buffer_length,
                  &width, &height, &channels, force_channels);
        /*  channels holds the original number of channels, which may have been forced  */
        if((force_channels >= 1) && (force_channels <= 4))
        {
            channels = force_channels;
        }
        if(NULL == img)
        {
            /*  image loading failed    */
            result_string_pointer = stbi_failure_reason();
            return 0;
        }
        /*  upload the texture, but reuse the assigned texture ID   */
        tex_id = SOIL_internal_create_OGL_texture(
                     img, width, height, channels,
                     tex_id, flags,
                     SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                     SOIL_MAX_CUBE_MAP_TEXTURE_SIZE);
        /*  and nuke the image data */
        SOIL_free_image_data(img);
    }
    /*  continue?   */
    if(tex_id != 0)
    {
        /*  1st face: try to load the image */
        img = SOIL_load_image_from_memory(
                  y_neg_buffer, y_neg_buffer_length,
                  &width, &height, &channels, force_channels);
        /*  channels holds the original number of channels, which may have been forced  */
        if((force_channels >= 1) && (force_channels <= 4))
        {
            channels = force_channels;
        }
        if(NULL == img)
        {
            /*  image loading failed    */
            result_string_pointer = stbi_failure_reason();
            return 0;
        }
        /*  upload the texture, but reuse the assigned texture ID   */
        tex_id = SOIL_internal_create_OGL_texture(
                     img, width, height, channels,
                     tex_id, flags,
                     SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                     SOIL_MAX_CUBE_MAP_TEXTURE_SIZE);
        /*  and nuke the image data */
        SOIL_free_image_data(img);
    }
    /*  continue?   */
    if(tex_id != 0)
    {
        /*  1st face: try to load the image */
        img = SOIL_load_image_from_memory(
                  z_pos_buffer, z_pos_buffer_length,
                  &width, &height, &channels, force_channels);
        /*  channels holds the original number of channels, which may have been forced  */
        if((force_channels >= 1) && (force_channels <= 4))
        {
            channels = force_channels;
        }
        if(NULL == img)
        {
            /*  image loading failed    */
            result_string_pointer = stbi_failure_reason();
            return 0;
        }
        /*  upload the texture, but reuse the assigned texture ID   */
        tex_id = SOIL_internal_create_OGL_texture(
                     img, width, height, channels,
                     tex_id, flags,
                     SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                     SOIL_MAX_CUBE_MAP_TEXTURE_SIZE);
        /*  and nuke the image data */
        SOIL_free_image_data(img);
    }
    /*  continue?   */
    if(tex_id != 0)
    {
        /*  1st face: try to load the image */
        img = SOIL_load_image_from_memory(
                  z_neg_buffer, z_neg_buffer_length,
                  &width, &height, &channels, force_channels);
        /*  channels holds the original number of channels, which may have been forced  */
        if((force_channels >= 1) && (force_channels <= 4))
        {
            channels = force_channels;
        }
        if(NULL == img)
        {
            /*  image loading failed    */
            result_string_pointer = stbi_failure_reason();
            return 0;
        }
        /*  upload the texture, but reuse the assigned texture ID   */
        tex_id = SOIL_internal_create_OGL_texture(
                     img, width, height, channels,
                     tex_id, flags,
                     SOIL_TEXTURE_CUBE_MAP, SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
                     SOIL_MAX_CUBE_MAP_TEXTURE_SIZE);
        /*  and nuke the image data */
        SOIL_free_image_data(img);
    }
    /*  and return the handle, such as it is    */
    return tex_id;
}

unsigned int
SOIL_load_OGL_single_chassismap
(
    const char *filename,
    const char face_order[6],
    int force_channels,
    unsigned int reuse_texture_ID,
    unsigned int flags
)
{
    /*  variables   */
    unsigned char *img;
    int width, height, channels, i;
    unsigned int tex_id = 0;
    /*  error checking  */
    if(filename == NULL)
    {
        result_string_pointer = (char *)"Invalid single chassis map file name";
        return 0;
    }
    /*  does the user want direct uploading of the image as a DDS file? */
    if(flags & SOIL_FLAG_DDS_LOAD_DIRECT)
    {
        /*  1st try direct loading of the image as a DDS file
            note: direct uploading will only load what is in the
            DDS file, no MIPmaps will be generated, the image will
            not be flipped, etc.    */
        tex_id = SOIL_direct_load_DDS(filename, reuse_texture_ID, flags, 1);
        if(tex_id)
        {
            /*  hey, it worked!!    */
            return tex_id;
        }
    }
    /*  face order checking */
    for(i = 0; i < 6; ++i)
    {
        if((face_order[i] != 'N') &&
           (face_order[i] != 'S') &&
           (face_order[i] != 'W') &&
           (face_order[i] != 'E') &&
           (face_order[i] != 'U') &&
           (face_order[i] != 'D'))
        {
            result_string_pointer = (char *)"Invalid single chassis map face order";
            return 0;
        };
    }
    /*  capability checking */
    if(query_chassismap_capability() != SOIL_CAPABILITY_PRESENT)
    {
        result_string_pointer = (char *)"No chassis map capability present";
        return 0;
    }
    /*  1st off, try to load the full image */
    img = SOIL_load_image(filename, &width, &height, &channels, force_channels);
    /*  channels holds the original number of channels, which may have been forced  */
    if((force_channels >= 1) && (force_channels <= 4))
    {
        channels = force_channels;
    }
    if(NULL == img)
    {
        /*  image loading failed    */
        result_string_pointer = stbi_failure_reason();
        return 0;
    }
    /*  now, does this image have the right dimensions? */
    if((width != 6 * height) &&
       (6 * width != height))
    {
        SOIL_free_image_data(img);
        result_string_pointer = (char *)"Single chassismap image must have a 6:1 ratio";
        return 0;
    }
    /*  try the image split and create  */
    tex_id = SOIL_create_OGL_single_chassismap(
                 img, width, height, channels,
                 face_order, reuse_texture_ID, flags
             );
    /*  nuke the temporary image data and return the texture handle */
    SOIL_free_image_data(img);
    return tex_id;
}

unsigned int
SOIL_load_OGL_single_chassismap_from_memory
(
    const unsigned char *const buffer,
    int buffer_length,
    const char face_order[6],
    int force_channels,
    unsigned int reuse_texture_ID,
    unsigned int flags
)
{
    /*  variables   */
    unsigned char *img;
    int width, height, channels, i;
    unsigned int tex_id = 0;
    /*  error checking  */
    if(buffer == NULL)
    {
        result_string_pointer = (char *)"Invalid single chassis map buffer";
        return 0;
    }
    /*  does the user want direct uploading of the image as a DDS file? */
    if(flags & SOIL_FLAG_DDS_LOAD_DIRECT)
    {
        /*  1st try direct loading of the image as a DDS file
            note: direct uploading will only load what is in the
            DDS file, no MIPmaps will be generated, the image will
            not be flipped, etc.    */
        tex_id = SOIL_direct_load_DDS_from_memory(
                     buffer, buffer_length,
                     reuse_texture_ID, flags, 1);
        if(tex_id)
        {
            /*  hey, it worked!!    */
            return tex_id;
        }
    }
    /*  face order checking */
    for(i = 0; i < 6; ++i)
    {
        if((face_order[i] != 'N') &&
           (face_order[i] != 'S') &&
           (face_order[i] != 'W') &&
           (face_order[i] != 'E') &&
           (face_order[i] != 'U') &&
           (face_order[i] != 'D'))
        {
            result_string_pointer = (char *)"Invalid single chassis map face order";
            return 0;
        };
    }
    /*  capability checking */
    if(query_chassismap_capability() != SOIL_CAPABILITY_PRESENT)
    {
        result_string_pointer = (char *)"No chassis map capability present";
        return 0;
    }
    /*  1st off, try to load the full image */
    img = SOIL_load_image_from_memory(
              buffer, buffer_length,
              &width, &height, &channels,
              force_channels);
    /*  channels holds the original number of channels, which may have been forced  */
    if((force_channels >= 1) && (force_channels <= 4))
    {
        channels = force_channels;
    }
    if(NULL == img)
    {
        /*  image loading failed    */
        result_string_pointer = stbi_failure_reason();
        return 0;
    }
    /*  now, does this image have the right dimensions? */
    if((width != 6 * height) &&
       (6 * width != height))
    {
        SOIL_free_image_data(img);
        result_string_pointer = (char *)"Single chassismap image must have a 6:1 ratio";
        return 0;
    }
    /*  try the image split and create  */
    tex_id = SOIL_create_OGL_single_chassismap(
                 img, width, height, channels,
                 face_order, reuse_texture_ID, flags
             );
    /*  nuke the temporary image data and return the texture handle */
    SOIL_free_image_data(img);
    return tex_id;
}

unsigned int
SOIL_create_OGL_single_chassismap
(
    const unsigned char *const data,
    int width, int height, int channels,
    const char face_order[6],
    unsigned int reuse_texture_ID,
    unsigned int flags
)
{
    /*  variables   */
    unsigned char *sub_img;
    int dw, dh, sz, i;
    unsigned int tex_id;
    /*  error checking  */
    if(data == NULL)
    {
        result_string_pointer = (char *)"Invalid single chassis map image data";
        return 0;
    }
    /*  face order checking */
    for(i = 0; i < 6; ++i)
    {
        if((face_order[i] != 'N') &&
           (face_order[i] != 'S') &&
           (face_order[i] != 'W') &&
           (face_order[i] != 'E') &&
           (face_order[i] != 'U') &&
           (face_order[i] != 'D'))
        {
            result_string_pointer = (char *)"Invalid single chassis map face order";
            return 0;
        };
    }
    /*  capability checking */
    if(query_chassismap_capability() != SOIL_CAPABILITY_PRESENT)
    {
        result_string_pointer = (char *)"No chassis map capability present";
        return 0;
    }
    /*  now, does this image have the right dimensions? */
    if((width != 6 * height) &&
       (6 * width != height))
    {
        result_string_pointer = (char *)"Single chassismap image must have a 6:1 ratio";
        return 0;
    }
    /*  which way am I stepping?    */
    if(width > height)
    {
        dw = height;
        dh = 0;
    }
    else
    {
        dw = 0;
        dh = width;
    }
    sz = dw + dh;
    sub_img = (unsigned char *)malloc(sz * sz * channels);
    /*  do the splitting and uploading  */
    tex_id = reuse_texture_ID;
    for(i = 0; i < 6; ++i)
    {
        int x, y, idx = 0;
        unsigned int chassismap_target = 0;
        /*  copy in the sub-image   */
        for(y = i * dh; y < i * dh + sz; ++y)
        {
            for(x = i * dw * channels; x < (i * dw + sz)*channels; ++x)
            {
                sub_img[idx++] = data[y * width * channels + x];
            }
        }
        /*  what is my texture target?
            remember, this coordinate system is
            LHS if viewed from inside the chassis! */
        switch(face_order[i])
        {
            case 'N':
                chassismap_target = SOIL_TEXTURE_CUBE_MAP_POSITIVE_Z;
                break;
            case 'S':
                chassismap_target = SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
                break;
            case 'W':
                chassismap_target = SOIL_TEXTURE_CUBE_MAP_NEGATIVE_X;
                break;
            case 'E':
                chassismap_target = SOIL_TEXTURE_CUBE_MAP_POSITIVE_X;
                break;
            case 'U':
                chassismap_target = SOIL_TEXTURE_CUBE_MAP_POSITIVE_Y;
                break;
            case 'D':
                chassismap_target = SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
                break;
        }
        /*  upload it as a texture  */
        tex_id = SOIL_internal_create_OGL_texture(
                     sub_img, sz, sz, channels,
                     tex_id, flags,
                     SOIL_TEXTURE_CUBE_MAP,
                     chassismap_target,
                     SOIL_MAX_CUBE_MAP_TEXTURE_SIZE);
    }
    /*  and nuke the image and sub-image data   */
    SOIL_free_image_data(sub_img);
    /*  and return the handle, such as it is    */
    return tex_id;
}

unsigned int
SOIL_create_OGL_texture
(
    const unsigned char *const data,
    int width, int height, int channels,
    unsigned int reuse_texture_ID,
    unsigned int flags
)
{
    /*  wrapper function for 2D textures    */
    return SOIL_internal_create_OGL_texture(
               data, width, height, channels,
               reuse_texture_ID, flags,
               GL_TEXTURE_2D, GL_TEXTURE_2D,
               GL_MAX_TEXTURE_SIZE);
}

#if SOIL_CHECK_FOR_GL_ERRORS
void check_for_GL_errors(const char *calling_location)
{
    /*  check for errors    */
    GLenum err_code = glGetError();
    while(GL_NO_ERROR != err_code)
    {
       // printf("OpenGL Error @ %s: %i", calling_location, err_code);
        err_code = glGetError();
    }
}
#else
void check_for_GL_errors(const char *calling_location)
{
    /*  no check for errors */
}
#endif
typedef unsigned char  BYTE;
typedef unsigned short WORD;
// BMP图像各部分说明如下
/***********
    第一部分    位图文件头
该结构的长度是固定的，为14个字节，各个域的依次如下：
    2byte   ：文件类型，必须是0x4d42，即字符串"BM"。
    4byte   ：整个文件大小
    4byte   ：保留字，为0
    4byte   ：从文件头到实际的位图图像数据的偏移字节数。
*************/
typedef struct
{
    long imageSize;
    long blank;
    long startPosition;
} BmpHead;
/*********************
/*********************
第二部分    位图信息头
该结构的长度也是固定的，为40个字节，各个域的依次说明如下：
    4byte   ：本结构的长度，值为40
    4byte   ：图像的宽度是多少象素。
    4byte   ：图像的高度是多少象素。
    2Byte   ：必须是1。
    2Byte   ：表示颜色时用到的位数，常用的值为1(黑白二色图)、4(16色图)、8(256色图)、24(真彩色图)。
    4byte   ：指定位图是否压缩，有效值为BI_RGB，BI_RLE8，BI_RLE4，BI_BITFIELDS。Windows位图可采用RLE4和RLE8的压缩格式，BI_RGB表示不压缩。
    4byte   ：指定实际的位图图像数据占用的字节数，可用以下的公式计算出来：
     图像数据 = Width' * Height * 表示每个象素颜色占用的byte数(即颜色位数/8,24bit图为3，256色为1）
     要注意的是：上述公式中的biWidth'必须是4的整数倍(不是biWidth，而是大于或等于biWidth的最小4的整数倍)。
     如果biCompression为BI_RGB，则该项可能为0。
    4byte   ：目标设备的水平分辨率。
    4byte   ：目标设备的垂直分辨率。
    4byte   ：本图像实际用到的颜色数，如果该值为0，则用到的颜色数为2的(颜色位数)次幂,如颜色位数为8，2^8=256,即256色的位图
    4byte   ：指定本图像中重要的颜色数，如果该值为0，则认为所有的颜色都是重要的。
***********************************/
typedef struct

{
    long    Length;
    long    width;
    long    height;
    WORD    colorPlane;
    WORD    bitColor;
    long    zipFormat;
    long    realSize;
    long    xPels;
    long    yPels;
    long    colorUse;
    long    colorImportant;
    /*  void show()

      {
          printf("infoHead Length:%dn",Length);
          printf("width&height:%d*%dn",width,height);
          printf("colorPlane:%dn",colorPlane);
          printf("bitColor:%dn",bitColor);
          printf("Compression Format:%dn",zipFormat);
          printf("Image Real Size:%dn",realSize);
          printf("Pels(X,Y):(%d,%d)n",xPels,yPels);
          printf("colorUse:%dn",colorUse);
          printf("Important Color:%dn",colorImportant);
      }*/
} InfoHead;
/***************************
/***************************
    第三部分    调色盘结构  颜色表
    对于256色BMP位图，颜色位数为8，需要2^8 = 256个调色盘；
    对于24bitBMP位图，各象素RGB值直接保存在图像数据区，不需要调色盘，不存在调色盘区
    rgbBlue：   该颜色的蓝色分量。
    rgbGreen：  该颜色的绿色分量。
    rgbRed：    该颜色的红色分量。
    rgbReserved：保留值。
************************/
typedef struct
{
    BYTE   rgbBlue;
    BYTE   rgbGreen;
    BYTE   rgbRed;
    BYTE   rgbReserved;
    /*   void show(void)
       {
          printf("Mix Plate B,G,R:%d %d %dn",rgbBlue,rgbGreen,rgbRed);
       }*/
} RGBMixPlate;
/****************************
      RGB加上头部信息转换成BMP
      参数說明：
      rgb_buffer        :RGB数据文件中的信息
      nData             :RGB数据的长度
      nWidth            :图像宽度的像素数
      nHeight           :图像高度的像素数
      fp1               :所存放的文件
*****************************/
int RGB2BMP(unsigned char *rgb_buffer, int nWidth, int nHeight, const char *filename)
{
    char bfType[2] = {'B', 'M'};
    long total = 0;
    unsigned char *pVisit = NULL;
    long i = 0;
    unsigned char R, G, B;
    unsigned char *tmp ;
    long nData = 0;
    unsigned short *rgb_buffer_s;

    if(rgb_buffer == NULL)
    {
        return -1;
    }

    total = nWidth * nHeight * 3;
    nData = nWidth * nHeight;

    //pVisit = (unsigned char *)malloc(total * sizeof(char));
    //if(pVisit == NULL)
    //{
      //  return -1;
   // }
    tmp = rgb_buffer;//pVisit;
   /* rgb_buffer_s = (unsigned short *)rgb_buffer;

    while(i < nData)
    {
        R = (*rgb_buffer_s >> 11) & 0x1f;
        G = (*rgb_buffer_s >> 5) & 0x3f;
        B = *rgb_buffer_s & 0x1f;
        B = B << 3;
        G = G << 2;
        R = R << 3;
        *pVisit = R;
        pVisit++;
        *pVisit = G;
        pVisit++;
        *pVisit = B;
        pVisit++;
        rgb_buffer_s++;
        i++;
    }*/
    SOIL_save_image(filename, SOIL_SAVE_TYPE_BMP, nWidth, nHeight, 3,  tmp);
    //free(tmp);
    return 0;
}

int bmp565_write(unsigned char *image, int width, int height, const char *filename)
{
    long file_size;
    long data_size;
    int widthAlignBytes;
    FILE *fp;

    // 文件头
    unsigned char header[66] =
    {
        // BITMAPFILEINFO
        'B', 'M',               // [0-1] bfType:必须是BM字符
        0, 0, 0, 0,             // [2-5] bfSize:总文件大小
        0, 0, 0, 0,             // [6-9] brReserved1,bfReserved2:保留
        sizeof(header), 0, 0, 0,// [10-13] bfOffBits:到图像数据的偏移
        // BITMAPFILEHEADER
        0x28, 0, 0, 0,          // [14-17] biSize:BITMAPINFOHEADER大小40字节
        0, 0, 0, 0,             // [18-21] biWidth:图片宽度
        0, 0, 0, 0,             // [22-25] biHeight:图片高度
        0x01, 0,                // [26-27] biPlanes:必须为1
        0x10, 0,                // [28-29] biBitCount:16位
        0x03, 0, 0, 0,          // [30-33] biCompression:BI_BITFIELDS=3
        0, 0, 0, 0,             // [34-37] biSizeImage:图片大小
        0x12, 0x0B, 0, 0,       // [38-41] biXPelsPerMeter:单位长度内的像素数
        0x12, 0x0B, 0, 0,       // [42-45] biYPelsPerMeter:单位长度内的像素数
        0, 0, 0, 0,             // [46-49] biClrUsed:可用像素数，设为0即可
        0, 0, 0, 0,             // [50-53] biClrImportant:重要颜色数，设为0即可
        // RGBQUAD MASK
        0x0, 0xF8, 0, 0,        // [54-57] 红色掩码
        0xE0, 0x07, 0, 0,       // [58-61] 绿色掩码
        0x1F, 0, 0, 0           // [62-65] 蓝色掩码
    };

    widthAlignBytes = ((width * 16 + 31) & ~31) / 8; // 每行需要的合适字节个数
    data_size = widthAlignBytes * height;      // 图像数据大小
    file_size = data_size + sizeof(header);    // 整个文件的大小

    *((long *)(header + 2)) = file_size;
    *((long *)(header + 18)) = width;
    *((long *)(header + 22)) = height;
    *((long *)(header + 34)) = data_size;

    if(!(fp = fopen(filename, "wb")))
    { return -1; }

    fwrite(header, sizeof(unsigned char), sizeof(header), fp);

    if(widthAlignBytes == width * 2)
    {
        fwrite(image, sizeof(unsigned char), (size_t)data_size, fp);
    }
    else
    {
        // 每一行单独写入
        int i;
        const static int DWZERO = 0;
        for(i = 0; i < height; i++)
        {
            fwrite(image + i * width * 2, sizeof(unsigned char),
                   (size_t) width * 2, fp);
            fwrite(&DWZERO, sizeof(unsigned char),
                   widthAlignBytes - width * 2, fp);
        }
    }

    fclose(fp);
    return 0;
}

unsigned int
SOIL_internal_create_OGL_texture
(
    const unsigned char *const data,
    int width, int height, int channels,
    unsigned int reuse_texture_ID,
    unsigned int flags,
    unsigned int opengl_texture_type,
    unsigned int opengl_texture_target,
    unsigned int texture_check_size_enum
)
{
    /*  variables   */
    unsigned char *img;
    unsigned int tex_id;
    unsigned int internal_texture_format = 0, original_texture_format = 0;
    int DXT_mode = SOIL_CAPABILITY_UNKNOWN;
    int max_supported_size;
    /*  If the user wants to use the texture rectangle I kill a few flags   */
    if(flags & SOIL_FLAG_TEXTURE_RECTANGLE)
    {
        /*  well, the user asked for it, can we do that?    */
        if(query_tex_rectangle_capability() == SOIL_CAPABILITY_PRESENT)
        {
            /*  only allow this if the user in _NOT_ trying to do a chassismap!    */
            if(opengl_texture_type == GL_TEXTURE_2D)
            {
                /*  clean out the flags that cannot be used with texture rectangles */
                flags &= ~(
                             SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS |
                             SOIL_FLAG_TEXTURE_REPEATS
                         );
                /*  and change my target    */
                opengl_texture_target = SOIL_TEXTURE_RECTANGLE_ARB;
                opengl_texture_type = SOIL_TEXTURE_RECTANGLE_ARB;
            }
            else
            {
                /*  not allowed for any other uses (yes, I'm looking at you, chassismaps!) */
                flags &= ~SOIL_FLAG_TEXTURE_RECTANGLE;
            }

        }
        else
        {
            /*  can't do it, and that is a breakable offense (uv coords use pixels instead of [0,1]!)   */
            result_string_pointer = (char *)"Texture Rectangle extension unsupported";
            return 0;
        }
    }
    /*  create a copy the image data    */
    img = (unsigned char *)malloc(width * height * channels);
    memcpy(img, data, width * height * channels);
    /*  does the user want me to invert the image?  */
    if(flags & SOIL_FLAG_INVERT_Y)
    {
        int i, j;
        for(j = 0; j * 2 < height; ++j)
        {
            int index1 = j * width * channels;
            int index2 = (height - 1 - j) * width * channels;
            for(i = width * channels; i > 0; --i)
            {
                unsigned char temp = img[index1];
                img[index1] = img[index2];
                img[index2] = temp;
                ++index1;
                ++index2;
            }
        }
    }
    /*  does the user want me to scale the colors into the NTSC safe RGB range? */
    if(flags & SOIL_FLAG_NTSC_SAFE_RGB)
    {
        scale_image_RGB_to_NTSC_safe(img, width, height, channels);
    }
    /*  does the user want me to convert from straight to pre-multiplied alpha?
        (and do we even _have_ alpha?)  */
    if(flags & SOIL_FLAG_MULTIPLY_ALPHA)
    {
        int i;
        switch(channels)
        {
            case 2:
                for(i = 0; i < 2 * width * height; i += 2)
                {
                    img[i] = (img[i] * img[i + 1] + 128) >> 8;
                }
                break;
            case 4:
                for(i = 0; i < 4 * width * height; i += 4)
                {
                    img[i + 0] = (img[i + 0] * img[i + 3] + 128) >> 8;
                    img[i + 1] = (img[i + 1] * img[i + 3] + 128) >> 8;
                    img[i + 2] = (img[i + 2] * img[i + 3] + 128) >> 8;
                }
                break;
            default:
                /*  no other number of channels contains alpha data */
                break;
        }
    }
    /*  if the user can't support NPOT textures, make sure we force the POT option  */
    if((query_NPOT_capability() == SOIL_CAPABILITY_NONE) &&
       !(flags & SOIL_FLAG_TEXTURE_RECTANGLE))
    {
        /*  add in the POT flag */
        flags |= SOIL_FLAG_POWER_OF_TWO;
    }
    /*  how large of a texture can this OpenGL implementation handle?   */
    /*  texture_check_size_enum will be GL_MAX_TEXTURE_SIZE or SOIL_MAX_CUBE_MAP_TEXTURE_SIZE   */
    glGetIntegerv(texture_check_size_enum, &max_supported_size);
    /*  do I need to make it a power of 2?  */
    if(
        (flags & SOIL_FLAG_POWER_OF_TWO) || /*  user asked for it   */
        (flags & SOIL_FLAG_MIPMAPS) ||      /*  need it for the MIP-maps    */
        (width > max_supported_size) ||     /*  it's too big, (make sure it's   */
        (height > max_supported_size))      /*  2^n for later down-sampling)    */
    {
        int new_width = 1;
        int new_height = 1;
        while(new_width < width)
        {
            new_width *= 2;
        }
        while(new_height < height)
        {
            new_height *= 2;
        }
        /*  still?  */
        if((new_width != width) || (new_height != height))
        {
            /*  yep, resize */
            unsigned char *resampled = (unsigned char *)malloc(channels * new_width * new_height);
            up_scale_image(
                img, width, height, channels,
                resampled, new_width, new_height);
            /*  OJO this is for debug only! */
            /*
            SOIL_save_image( "\\showme.bmp", SOIL_SAVE_TYPE_BMP,
                            new_width, new_height, channels,
                            resampled );
            */
            /*  nuke the old guy, then point it at the new guy  */
            SOIL_free_image_data(img);
            img = resampled;
            width = new_width;
            height = new_height;
        }
    }
    /*  now, if it is too large...  */
    if((width > max_supported_size) || (height > max_supported_size))
    {
        /*  I've already made it a power of two, so simply use the MIPmapping
            code to reduce its size to the allowable maximum.   */
        unsigned char *resampled;
        int reduce_block_x = 1, reduce_block_y = 1;
        int new_width, new_height;
        if(width > max_supported_size)
        {
            reduce_block_x = width / max_supported_size;
        }
        if(height > max_supported_size)
        {
            reduce_block_y = height / max_supported_size;
        }
        new_width = width / reduce_block_x;
        new_height = height / reduce_block_y;
        resampled = (unsigned char *)malloc(channels * new_width * new_height);
        /*  perform the actual reduction    */
        mipmap_image(img, width, height, channels,
                     resampled, reduce_block_x, reduce_block_y);
        /*  nuke the old guy, then point it at the new guy  */
        SOIL_free_image_data(img);
        img = resampled;
        width = new_width;
        height = new_height;
    }
    /*  does the user want us to use YCoCg color space? */
    if(flags & SOIL_FLAG_CoCg_Y)
    {
        /*  this will only work with RGB and RGBA images */
        convert_RGB_to_YCoCg(img, width, height, channels);
        /*
        save_image_as_DDS( "CoCg_Y.dds", width, height, channels, img );
        */
    }
    /*  create the OpenGL texture ID handle
        (note: allowing a forced texture ID lets me reload a texture)   */
    tex_id = reuse_texture_ID;
    if(tex_id == 0)
    {
        glGenTextures(1, &tex_id);
    }
    check_for_GL_errors("glGenTextures");
    /* Note: sometimes glGenTextures fails (usually no OpenGL context)  */
    if(tex_id)
    {
        /*  and what type am I using as the internal texture format?    */
        switch(channels)
        {
            case 1:
                original_texture_format = GL_LUMINANCE;
                break;
            case 2:
                original_texture_format = GL_LUMINANCE_ALPHA;
                break;
            case 3:
                original_texture_format = GL_RGB;
                break;
            case 4:
                original_texture_format = GL_RGBA;
                break;
        }
        internal_texture_format = original_texture_format;
        /*  does the user want me to, and can I, save as DXT?   */
        if(flags & SOIL_FLAG_COMPRESS_TO_DXT)
        {
            DXT_mode = query_DXT_capability();
            if(DXT_mode == SOIL_CAPABILITY_PRESENT)
            {
                /*  I can use DXT, whether I compress it or OpenGL does */
                if((channels & 1) == 1)
                {
                    /*  1 or 3 channels = DXT1  */
                    internal_texture_format = SOIL_RGB_S3TC_DXT1;
                }
                else
                {
                    /*  2 or 4 channels = DXT5  */
                    internal_texture_format = SOIL_RGBA_S3TC_DXT5;
                }
            }
        }
        /*  bind an OpenGL texture ID   */
        glBindTexture(opengl_texture_type, tex_id);
        check_for_GL_errors("glBindTexture");
        /*  upload the main image   */
        if(DXT_mode == SOIL_CAPABILITY_PRESENT)
        {
            /*  user wants me to do the DXT conversion! */
            int DDS_size;
            unsigned char *DDS_data = NULL;
            if((channels & 1) == 1)
            {
                /*  RGB, use DXT1   */
                DDS_data = convert_image_to_DXT1(img, width, height, channels, &DDS_size);
            }
            else
            {
                /*  RGBA, use DXT5  */
                DDS_data = convert_image_to_DXT5(img, width, height, channels, &DDS_size);
            }
            if(DDS_data)
            {
                soilGlCompressedTexImage2D(
                    opengl_texture_target, 0,
                    internal_texture_format, width, height, 0,
                    DDS_size, DDS_data);
                check_for_GL_errors("glCompressedTexImage2D");
                SOIL_free_image_data(DDS_data);
                /*  printf( "Internal DXT compressor\n" );  */
            }
            else
            {
                /*  my compression failed, try the OpenGL driver's version  */
                glTexImage2D(
                    opengl_texture_target, 0,
                    internal_texture_format, width, height, 0,
                    original_texture_format, GL_UNSIGNED_BYTE, img);
                check_for_GL_errors("glTexImage2D");
                /*  printf( "OpenGL DXT compressor\n" );    */
            }
        }
        else
        {
            /*  user want OpenGL to do all the work!    */
            glTexImage2D(
                opengl_texture_target, 0,
                internal_texture_format, width, height, 0,
                original_texture_format, GL_UNSIGNED_BYTE, img);
                check_for_GL_errors("glTexImage2D");
            /*printf( "OpenGL DXT compressor\n" );  */
        }
        /*  are any MIPmaps desired?    */
        if(flags & SOIL_FLAG_MIPMAPS)
        {
            int MIPlevel = 1;
            int MIPwidth = (width + 1) / 2;
            int MIPheight = (height + 1) / 2;
            unsigned char *resampled = (unsigned char *)malloc(channels * MIPwidth * MIPheight);
            while(((1 << MIPlevel) <= width) || ((1 << MIPlevel) <= height))
            {
                /*  do this MIPmap level    */
                mipmap_image(
                    img, width, height, channels,
                    resampled,
                    (1 << MIPlevel), (1 << MIPlevel));
                /*  upload the MIPmaps  */
                if(DXT_mode == SOIL_CAPABILITY_PRESENT)
                {
                    /*  user wants me to do the DXT conversion! */
                    int DDS_size;
                    unsigned char *DDS_data = NULL;
                    if((channels & 1) == 1)
                    {
                        /*  RGB, use DXT1   */
                        DDS_data = convert_image_to_DXT1(
                                       resampled, MIPwidth, MIPheight, channels, &DDS_size);
                    }
                    else
                    {
                        /*  RGBA, use DXT5  */
                        DDS_data = convert_image_to_DXT5(
                                       resampled, MIPwidth, MIPheight, channels, &DDS_size);
                    }
                    if(DDS_data)
                    {
                        soilGlCompressedTexImage2D(
                            opengl_texture_target, MIPlevel,
                            internal_texture_format, MIPwidth, MIPheight, 0,
                            DDS_size, DDS_data);
                        check_for_GL_errors("glCompressedTexImage2D");
                        SOIL_free_image_data(DDS_data);
                    }
                    else
                    {
                        /*  my compression failed, try the OpenGL driver's version  */
                        glTexImage2D(
                            opengl_texture_target, MIPlevel,
                            internal_texture_format, MIPwidth, MIPheight, 0,
                            original_texture_format, GL_UNSIGNED_BYTE, resampled);
                        check_for_GL_errors("glTexImage2D");
                    }
                }
                else
                {
                    /*  user want OpenGL to do all the work!    */
                    glTexImage2D(
                        opengl_texture_target, MIPlevel,
                        internal_texture_format, MIPwidth, MIPheight, 0,
                        original_texture_format, GL_UNSIGNED_BYTE, resampled);
                    check_for_GL_errors("glTexImage2D");
                }
                /*  prep for the next level */
                ++MIPlevel;
                MIPwidth = (MIPwidth + 1) / 2;
                MIPheight = (MIPheight + 1) / 2;
            }
            SOIL_free_image_data(resampled);
            /*  instruct OpenGL to use the MIPmaps  */
            glTexParameteri(opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            check_for_GL_errors("GL_TEXTURE_MIN/MAG_FILTER");
        }
        else
        {
            /*  instruct OpenGL _NOT_ to use the MIPmaps    */
            glTexParameteri(opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            check_for_GL_errors("GL_TEXTURE_MIN/MAG_FILTER");
        }
        /*  does the user want clamping, or wrapping?   */
        if(flags & SOIL_FLAG_TEXTURE_REPEATS)
        {
            glTexParameteri(opengl_texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(opengl_texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
            if(opengl_texture_type == SOIL_TEXTURE_CUBE_MAP)
            {
                /*  SOIL_TEXTURE_WRAP_R is invalid if chassismaps aren't supported */
                glTexParameteri(opengl_texture_type, SOIL_TEXTURE_WRAP_R, GL_REPEAT);
            }
            check_for_GL_errors("GL_TEXTURE_WRAP_*");
        }
        else
        {
            /*  unsigned int clamp_mode = SOIL_CLAMP_TO_EDGE;   */
            unsigned int clamp_mode = GL_CLAMP_TO_EDGE;
            glTexParameteri(opengl_texture_type, GL_TEXTURE_WRAP_S, clamp_mode);
            glTexParameteri(opengl_texture_type, GL_TEXTURE_WRAP_T, clamp_mode);
            if(opengl_texture_type == SOIL_TEXTURE_CUBE_MAP)
            {
                /*  SOIL_TEXTURE_WRAP_R is invalid if chassismaps aren't supported */
                glTexParameteri(opengl_texture_type, SOIL_TEXTURE_WRAP_R, clamp_mode);
            }
            check_for_GL_errors("GL_TEXTURE_WRAP_*");
        }
        /*  done    */
        result_string_pointer = (char *)"Image loaded as an OpenGL texture";
    }
    else
    {
        /*  failed  */
        result_string_pointer = (char *)"Failed to generate an OpenGL texture name; missing OpenGL context?";
    }
    SOIL_free_image_data(img);
    return tex_id;
}

int
SOIL_save_screenshot
(
    const char *filename,
    int image_type,
    int x, int y,
    int width, int height, unsigned char *pixel_data
)
{

    unsigned char *pixel_datat;
    int i, j;
    int save_result;

    /*  error checks    */
    if((width < 1) || (height < 1))
    {
        result_string_pointer = (char *)"Invalid screenshot dimensions";
        return 0;
    }
    if((x < 0) || (y < 0))
    {
        result_string_pointer = (char *)"Invalid screenshot location";
        return 0;
    }
    if(filename == NULL)
    {
        result_string_pointer = (char *)"Invalid screenshot filename";
        return 0;
    }

    /*  Get the data from OpenGL    */
   // pixel_data = (unsigned char *)malloc(4 * width * height);
    pixel_datat = (unsigned char *)malloc(3 * width * height);
   // glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data);

    /*  invert the image    */
    /*for(j = 0; j * 2 < height; ++j)
    {
       int index1 = j * width * 4;
       int index2 = (height - 1 - j) * width * 4;
       for(i = width * 4; i > 0; --i)
       {
           unsigned char temp = pixel_data[index1];
           pixel_data[index1] = pixel_data[index2];
           pixel_data[index2] = temp;
           ++index1;
           ++index2;
       }
    }*/
    for(j = 0; j < height; ++j)
    {
        for(i = 0; i < width; ++i)
        {

            pixel_datat[(j * width + i) * 3] = pixel_data[(j * width + i) * 4];
            pixel_datat[(j * width + i) * 3 + 1]  = pixel_data[(j * width + i) * 4 + 1];
            pixel_datat[(j * width + i) * 3 + 2]  = pixel_data[(j * width + i) * 4 + 2];
        }
    }

    /*  save the image  */
    save_result = SOIL_save_image(filename, image_type, width, height, 3, pixel_datat);

    /*  And free the memory */
    //SOIL_free_image_data(pixel_data);
    SOIL_free_image_data(pixel_datat);
    return save_result;
}

unsigned char *
SOIL_load_image
(
    const char *filename,
    int *width, int *height, int *channels,
    int force_channels
)
{
    unsigned char *result = stbi_load(filename,width, height, channels, force_channels);
    if(result == NULL)
    {
        //result_string_pointer = stbi_failure_reason();
    }
    else
    {
       // result_string_pointer = "Image loaded";
    }
    return result;
}

unsigned char *
SOIL_load_image_from_memory
(
    const unsigned char *const buffer,
    int buffer_length,
    int *width, int *height, int *channels,
    int force_channels
)
{
    unsigned char *result = stbi_load_from_memory(
                                buffer, buffer_length,
                                width, height, channels,
                                force_channels);
    if(result == NULL)
    {
        result_string_pointer = stbi_failure_reason();
    }
    else
    {
        result_string_pointer = (char *)"Image loaded from memory";
    }
    return result;
}

int
SOIL_save_image
(
    const char *filename,
    int image_type,
    int width, int height, int channels,
    const unsigned char *const data
)
{
    int save_result;
///////////
#if 0
    /*  Get the data from OpenGL    */
    unsigned char *pixel_data;

    pixel_data = (unsigned char *)malloc(3 * width * height);

    /*  invert the image    */
    for(j = 0; j * 2 < height; ++j)
    {
        int index1 = j * width * 3;
        int index2 = (height - 1 - j) * width * 3;
        for(i = width * 3; i > 0; --i)
        {
            unsigned char temp = pixel_data[index1];
            pixel_data[index1] = pixel_data[index2];
            pixel_data[index2] = temp;
            ++index1;
            ++index2;
        }
    }

    /*  save the image  */
    save_result = SOIL_save_image(filename, image_type, width, height, 3, pixel_data);

    /*  And free the memory */
    SOIL_free_image_data(pixel_data);
#endif
/////////////
    /*  error check */
    if((width < 1) || (height < 1) ||
       (channels < 1) || (channels > 4) ||
       (data == NULL) ||
       (filename == NULL))
    {
        return 0;
    }
    if(image_type == SOIL_SAVE_TYPE_BMP)
    {
        save_result = stbi_write_bmp(filename,
                                     width, height, channels, (void *)data);
    }
    else if(image_type == SOIL_SAVE_TYPE_TGA)
    {
        save_result = stbi_write_tga(filename,
                                     width, height, channels, (void *)data);
    }
    else if(image_type == SOIL_SAVE_TYPE_DDS)
    {
        save_result = save_image_as_DDS(filename,
                                        width, height, channels, (const unsigned char *const)data);
    }
    else
    {
        save_result = 0;
    }
    if(save_result == 0)
    {
        result_string_pointer = (char *)"Saving the image failed";
    }
    else
    {
        result_string_pointer = (char *)"Image saved";
    }
    return save_result;
}

void
SOIL_free_image_data
(
    unsigned char *img_data
)
{
    free((void *)img_data);
}

const char *
SOIL_last_result
(
    void
)
{
    return result_string_pointer;
}

unsigned int SOIL_direct_load_DDS_from_memory(
    const unsigned char *const buffer,
    int buffer_length,
    unsigned int reuse_texture_ID,
    int flags,
    int loading_as_chassismap)
{
    /*  variables   */
    DDS_header header;
    unsigned int buffer_index = 0;
    unsigned int tex_ID = 0;
    /*  file reading variables  */
    unsigned int S3TC_type = 0;
    unsigned char *DDS_data;
    unsigned int DDS_main_size;
    unsigned int DDS_full_size;
    unsigned int width, height;
    int mipmaps, chassismap, uncompressed, block_size = 16;
    unsigned int flag;
    unsigned int cf_target, ogl_target_start, ogl_target_end;
    unsigned int opengl_texture_type;
    int i;
    /*  1st off, does the filename even exist?  */
    if(NULL == buffer)
    {
        /*  we can't do it! */
        result_string_pointer = (char *)"NULL buffer";
        return 0;
    }
    if((unsigned int)buffer_length < sizeof(DDS_header))
    {
        /*  we can't do it! */
        result_string_pointer = (char *)"DDS file was too small to contain the DDS header";
        return 0;
    }
    /*  try reading in the header   */
    memcpy((void *)(&header), (const void *)buffer, sizeof(DDS_header));
    buffer_index = sizeof(DDS_header);
    /*  guilty until proven innocent    */
    result_string_pointer = (char *)"Failed to read a known DDS header";
    /*  validate the header (warning, "goto"'s ahead, shield your eyes!!)   */
    flag = ('D' << 0) | ('D' << 8) | ('S' << 16) | (' ' << 24);
    if(header.dwMagic != flag) {goto quick_exit;}
    if(header.dwSize != 124) {goto quick_exit;}
    /*  I need all of these */
    flag = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    if((header.dwFlags & flag) != flag) {goto quick_exit;}
    /*  According to the MSDN spec, the dwFlags should contain
        DDSD_LINEARSIZE if it's compressed, or DDSD_PITCH if
        uncompressed.  Some DDS writers do not conform to the
        spec, so I need to make my reader more tolerant */
    /*  I need one of these */
    flag = DDPF_FOURCC | DDPF_RGB;
    if((header.sPixelFormat.dwFlags & flag) == 0) {goto quick_exit;}
    if(header.sPixelFormat.dwSize != 32) {goto quick_exit;}
    if((header.sCaps.dwCaps1 & DDSCAPS_TEXTURE) == 0) {goto quick_exit;}
    /*  make sure it is a type we can upload    */
    if((header.sPixelFormat.dwFlags & DDPF_FOURCC) &&
       !(
           (header.sPixelFormat.dwFourCC == (('D' << 0) | ('X' << 8) | ('T' << 16) | ('1' << 24))) ||
           (header.sPixelFormat.dwFourCC == (('D' << 0) | ('X' << 8) | ('T' << 16) | ('3' << 24))) ||
           (header.sPixelFormat.dwFourCC == (('D' << 0) | ('X' << 8) | ('T' << 16) | ('5' << 24)))
       ))
    {
        goto quick_exit;
    }
    /*  OK, validated the header, let's load the image data */
    result_string_pointer = (char *)"DDS header loaded and validated";
    width = header.dwWidth;
    height = header.dwHeight;
    uncompressed = 1 - (header.sPixelFormat.dwFlags & DDPF_FOURCC) / DDPF_FOURCC;
    chassismap = (header.sCaps.dwCaps2 & DDSCAPS2_CUBEMAP) / DDSCAPS2_CUBEMAP;
    if(uncompressed)
    {
        S3TC_type = GL_RGB;
        block_size = 3;
        if(header.sPixelFormat.dwFlags & DDPF_ALPHAPIXELS)
        {
            S3TC_type = GL_RGBA;
            block_size = 4;
        }
        DDS_main_size = width * height * block_size;
    }
    else
    {
        /*  can we even handle direct uploading to OpenGL DXT compressed images?    */
        if(query_DXT_capability() != SOIL_CAPABILITY_PRESENT)
        {
            /*  we can't do it! */
            result_string_pointer = (char *)"Direct upload of S3TC images not supported by the OpenGL driver";
            return 0;
        }
        /*  well, we know it is DXT1/3/5, because we checked above  */
        switch((header.sPixelFormat.dwFourCC >> 24) - '0')
        {
            case 1:
                S3TC_type = SOIL_RGBA_S3TC_DXT1;
                block_size = 8;
                break;
            case 3:
                S3TC_type = SOIL_RGBA_S3TC_DXT3;
                block_size = 16;
                break;
            case 5:
                S3TC_type = SOIL_RGBA_S3TC_DXT5;
                block_size = 16;
                break;
        }
        DDS_main_size = ((width + 3) >> 2) * ((height + 3) >> 2) * block_size;
    }
    if(chassismap)
    {
        /* does the user want a chassismap?    */
        if(!loading_as_chassismap)
        {
            /*  we can't do it! */
            result_string_pointer = (char *)"DDS image was a chassismap";
            return 0;
        }
        /*  can we even handle chassismaps with the OpenGL driver? */
        if(query_chassismap_capability() != SOIL_CAPABILITY_PRESENT)
        {
            /*  we can't do it! */
            result_string_pointer = (char *)"Direct upload of chassismap images not supported by the OpenGL driver";
            return 0;
        }
        ogl_target_start = SOIL_TEXTURE_CUBE_MAP_POSITIVE_X;
        ogl_target_end =   SOIL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        opengl_texture_type = SOIL_TEXTURE_CUBE_MAP;
    }
    else
    {
        /* does the user want a non-chassismap?    */
        if(loading_as_chassismap)
        {
            /*  we can't do it! */
            result_string_pointer = (char *)"DDS image was not a chassismap";
            return 0;
        }
        ogl_target_start = GL_TEXTURE_2D;
        ogl_target_end =   GL_TEXTURE_2D;
        opengl_texture_type = GL_TEXTURE_2D;
    }
    if((header.sCaps.dwCaps1 & DDSCAPS_MIPMAP) && (header.dwMipMapCount > 1))
    {
        int shift_offset;
        mipmaps = header.dwMipMapCount - 1;
        DDS_full_size = DDS_main_size;
        if(uncompressed)
        {
            /*  uncompressed DDS, simple MIPmap size calculation    */
            shift_offset = 0;
        }
        else
        {
            /*  compressed DDS, MIPmap size calculation is block based  */
            shift_offset = 2;
        }
        for(i = 1; i <= mipmaps; ++ i)
        {
            int w, h;
            w = width >> (shift_offset + i);
            h = height >> (shift_offset + i);
            if(w < 1)
            {
                w = 1;
            }
            if(h < 1)
            {
                h = 1;
            }
            DDS_full_size += w * h * block_size;
        }
    }
    else
    {
        mipmaps = 0;
        DDS_full_size = DDS_main_size;
    }
    DDS_data = (unsigned char *)malloc(DDS_full_size);
    /*  got the image data RAM, create or use an existing OpenGL texture handle */
    tex_ID = reuse_texture_ID;
    if(tex_ID == 0)
    {
        glGenTextures(1, &tex_ID);
    }
    /*  bind an OpenGL texture ID   */
    glBindTexture(opengl_texture_type, tex_ID);
    /*  do this for each face of the chassismap!   */
    for(cf_target = ogl_target_start; cf_target <= ogl_target_end; ++cf_target)
    {
        if(buffer_index + DDS_full_size <= (unsigned int)buffer_length)
        {
            unsigned int byte_offset = DDS_main_size;
            memcpy((void *)DDS_data, (const void *)(&buffer[buffer_index]), DDS_full_size);
            buffer_index += DDS_full_size;
            /*  upload the main chunk   */
            if(uncompressed)
            {
                /*  and remember, DXT uncompressed uses BGR(A),
                    so swap to RGB(A) for ALL MIPmap levels */
                for(i = 0; (unsigned int)i < DDS_full_size; i += block_size)
                {
                    unsigned char temp = DDS_data[i];
                    DDS_data[i] = DDS_data[i + 2];
                    DDS_data[i + 2] = temp;
                }
                glTexImage2D(
                    cf_target, 0,
                    S3TC_type, width, height, 0,
                    S3TC_type, GL_UNSIGNED_BYTE, DDS_data);
            }
            else
            {
                soilGlCompressedTexImage2D(
                    cf_target, 0,
                    S3TC_type, width, height, 0,
                    DDS_main_size, DDS_data);
            }
            /*  upload the mipmaps, if we have them */
            for(i = 1; i <= mipmaps; ++i)
            {
                int w, h, mip_size;
                w = width >> i;
                h = height >> i;
                if(w < 1)
                {
                    w = 1;
                }
                if(h < 1)
                {
                    h = 1;
                }
                /*  upload this mipmap  */
                if(uncompressed)
                {
                    mip_size = w * h * block_size;
                    glTexImage2D(
                        cf_target, i,
                        S3TC_type, w, h, 0,
                        S3TC_type, GL_UNSIGNED_BYTE, &DDS_data[byte_offset]);
                }
                else
                {
                    mip_size = ((w + 3) / 4) * ((h + 3) / 4) * block_size;
                    soilGlCompressedTexImage2D(
                        cf_target, i,
                        S3TC_type, w, h, 0,
                        mip_size, &DDS_data[byte_offset]);
                }
                /*  and move to the next mipmap */
                byte_offset += mip_size;
            }
            /*  it worked!  */
            result_string_pointer = (char *)"DDS file loaded";
        }
        else
        {
            glDeleteTextures(1, & tex_ID);
            tex_ID = 0;
            cf_target = ogl_target_end + 1;
            result_string_pointer = (char *)"DDS file was too small for expected image data";
        }
    }/* end reading each face */
    SOIL_free_image_data(DDS_data);
    if(tex_ID)
    {
        /*  did I have MIPmaps? */
        if(mipmaps > 0)
        {
            /*  instruct OpenGL to use the MIPmaps  */
            glTexParameteri(opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
        else
        {
            /*  instruct OpenGL _NOT_ to use the MIPmaps    */
            glTexParameteri(opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        /*  does the user want clamping, or wrapping?   */
        if(flags & SOIL_FLAG_TEXTURE_REPEATS)
        {
            glTexParameteri(opengl_texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(opengl_texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(opengl_texture_type, SOIL_TEXTURE_WRAP_R, GL_REPEAT);
        }
        else
        {
            /*  unsigned int clamp_mode = SOIL_CLAMP_TO_EDGE;   */
            unsigned int clamp_mode = GL_CLAMP_TO_EDGE;
            glTexParameteri(opengl_texture_type, GL_TEXTURE_WRAP_S, clamp_mode);
            glTexParameteri(opengl_texture_type, GL_TEXTURE_WRAP_T, clamp_mode);
            glTexParameteri(opengl_texture_type, SOIL_TEXTURE_WRAP_R, clamp_mode);
        }
    }

quick_exit:
    /*  report success or failure   */
    return tex_ID;
}

unsigned int SOIL_direct_load_DDS(
    const char *filename,
    unsigned int reuse_texture_ID,
    int flags,
    int loading_as_chassismap)
{
    FILE *f;
    unsigned char *buffer;
    size_t buffer_length, bytes_read;
    unsigned int tex_ID = 0;
    /*  error checks    */
    if(NULL == filename)
    {
        result_string_pointer = (char *)"NULL filename";
        return 0;
    }
    f = fopen(filename, "rb");
    if(NULL == f)
    {
        /*  the file doesn't seem to exist (or be open-able)    */
        result_string_pointer = (char *)"Can not find DDS file";
        return 0;
    }
    fseek(f, 0, SEEK_END);
    buffer_length = ftell(f);
    fseek(f, 0, SEEK_SET);
    buffer = (unsigned char *) malloc(buffer_length);
    if(NULL == buffer)
    {
        result_string_pointer = (char *)"malloc failed";
        fclose(f);
        return 0;
    }
    bytes_read = fread((void *)buffer, 1, buffer_length, f);
    fclose(f);
    if(bytes_read < buffer_length)
    {
        /*  huh?    */
        buffer_length = bytes_read;
    }
    /*  now try to do the loading   */
    tex_ID = SOIL_direct_load_DDS_from_memory(
                 (const unsigned char *const)buffer, buffer_length,
                 reuse_texture_ID, flags, loading_as_chassismap);
    SOIL_free_image_data(buffer);
    return tex_ID;
}

int query_NPOT_capability(void)
{
    /*  check for the capability    */
    if(has_NPOT_capability == SOIL_CAPABILITY_UNKNOWN)
    {
        /*  we haven't yet checked for the capability, do so    */
        if(
            (NULL == strstr((char const *)glGetString(GL_EXTENSIONS),
                            "GL_ARB_texture_non_power_of_two"))
        )
        {
            /*  not there, flag the failure */
            has_NPOT_capability = SOIL_CAPABILITY_NONE;
        }
        else
        {
            /*  it's there! */
            has_NPOT_capability = SOIL_CAPABILITY_PRESENT;
        }
    }
    /*  let the user know if we can do non-power-of-two textures or not */
    return has_NPOT_capability;
}

int query_tex_rectangle_capability(void)
{
    /*  check for the capability    */
    if(has_tex_rectangle_capability == SOIL_CAPABILITY_UNKNOWN)
    {
        /*  we haven't yet checked for the capability, do so    */
        if(
            (NULL == strstr((char const *)glGetString(GL_EXTENSIONS),
                            "GL_ARB_texture_rectangle"))
            &&
            (NULL == strstr((char const *)glGetString(GL_EXTENSIONS),
                            "GL_EXT_texture_rectangle"))
            &&
            (NULL == strstr((char const *)glGetString(GL_EXTENSIONS),
                            "GL_NV_texture_rectangle"))
        )
        {
            /*  not there, flag the failure */
            has_tex_rectangle_capability = SOIL_CAPABILITY_NONE;
        }
        else
        {
            /*  it's there! */
            has_tex_rectangle_capability = SOIL_CAPABILITY_PRESENT;
        }
    }
    /*  let the user know if we can do texture rectangles or not    */
    return has_tex_rectangle_capability;
}

int query_chassismap_capability(void)
{
    /*  check for the capability    */
    if(has_chassismap_capability == SOIL_CAPABILITY_UNKNOWN)
    {
        /*  we haven't yet checked for the capability, do so    */
        if(
            (NULL == strstr((char const *)glGetString(GL_EXTENSIONS),
                            "GL_ARB_texture_chassis_map"))
            &&
            (NULL == strstr((char const *)glGetString(GL_EXTENSIONS),
                            "GL_EXT_texture_chassis_map"))
        )
        {
            /*  not there, flag the failure */
            has_chassismap_capability = SOIL_CAPABILITY_NONE;
        }
        else
        {
            /*  it's there! */
            has_chassismap_capability = SOIL_CAPABILITY_PRESENT;
        }
    }
    /*  let the user know if we can do chassismaps or not  */
    return has_chassismap_capability;
}



int query_DXT_capability(void)
{
    /*  check for the capability    */
    if(has_DXT_capability == SOIL_CAPABILITY_UNKNOWN)
    {
        /*  we haven't yet checked for the capability, do so    */
        if(NULL == strstr(
               (char const *)glGetString(GL_EXTENSIONS),
               "GL_EXT_texture_compression_s3tc"))
        {
            /*  not there, flag the failure */
            has_DXT_capability = SOIL_CAPABILITY_NONE;
        }
        else
        {
            /*  and find the address of the extension function  */
            P_SOIL_GLCOMPRESSEDTEXIMAGE2DPROC ext_addr = NULL;
#ifdef WIN32
            ext_addr = (P_SOIL_GLCOMPRESSEDTEXIMAGE2DPROC)
                       wglGetProcAddress
                       (
                           "glCompressedTexImage2DARB"
                       );
#elif defined(__APPLE__) || defined(__APPLE_CC__)
            /*  I can't test this Apple stuff!  */
            CFBundleRef bundle;
            CFURLRef bundleURL =
                CFURLCreateWithFileSystemPath(
                    kCFAllocatorDefault,
                    CFSTR("/System/Library/Frameworks/OpenGL.framework"),
                    kCFURLPOSIXPathStyle,
                    true);
            CFStringRef extensionName =
                CFStringCreateWithCString(
                    kCFAllocatorDefault,
                    "glCompressedTexImage2DARB",
                    kCFStringEncodingASCII);
            bundle = CFBundleCreate(kCFAllocatorDefault, bundleURL);
            assert(bundle != NULL);
            ext_addr = (P_SOIL_GLCOMPRESSEDTEXIMAGE2DPROC)
                       CFBundleGetFunctionPointerForName
                       (
                           bundle, extensionName
                       );
            CFRelease(bundleURL);
            CFRelease(extensionName);
            CFRelease(bundle);
#else
            //ext_addr = (P_SOIL_GLCOMPRESSEDTEXIMAGE2DPROC)
            //glXGetProcAddressARB
            //(
            //  (const GLubyte *)"glCompressedTexImage2DARB"
            //);
#endif
            /*  Flag it so no checks needed later   */
            if(NULL == ext_addr)
            {
                /*  hmm, not good!!  This should not happen, but does on my
                    laptop's VIA chipset.  The GL_EXT_texture_compression_s3tc
                    spec requires that ARB_texture_compression be present too.
                    this means I can upload and have the OpenGL drive do the
                    conversion, but I can't use my own routines or load DDS files
                    from disk and upload them directly [8^( */
                has_DXT_capability = SOIL_CAPABILITY_NONE;
            }
            else
            {
                /*  all's well! */
                soilGlCompressedTexImage2D = ext_addr;
                has_DXT_capability = SOIL_CAPABILITY_PRESENT;
            }
        }
    }
    /*  let the user know if we can do DXT or not   */
    return has_DXT_capability;
}


/*******************************************************************************
* Function Name  : SaveYuv2BMP
* Description    : 将YUV422已BMP格式图片保存到SD卡
* Input          : pyuv422: pointer to YUV444 data
*                  save_name : the save422 name
*                  width :  the picture width
*                  height:  the picture height
* Output         : none
*
* Return         : 成功:1    失败:-1
*******************************************************************************/
int SaveYuv2BMP(const unsigned char *pyuv422,  int width, int height, const char  *save_name)
{
#if defined(FSLBOARD)&& !defined(DEBUG_BMP)
    unsigned char *Rgbbuf = NULL;
    int save_result = -1;

    Rgbbuf = (unsigned char *)malloc(width * height * 3);
    yuv2rgb(pyuv422, Rgbbuf, width, height);

    /*  save the image  */
    save_result = RGB2BMP((unsigned char *)Rgbbuf, width, height, save_name);

    /*  And free the memory */
    SOIL_free_image_data(Rgbbuf);
    if(save_result == -1)
    {
        printf("RGB2BMP err\n");
        return -1;
    }
#endif
    return 1;
}
void yuv2rgb(const unsigned char *dsrc, unsigned char *ddst, int w, int h)
{
	int  y  = 0;
    int  x  = 0;
    unsigned char  CY, CYn, Cb, Cr;
    int R, G, B;
    if(NULL == dsrc || NULL == ddst)
    {
        return ;
    }


    for(y = 0; y < h; y++)
    {
        for(x = 0; x < (w - 4); x += 2)
        {
            Cb  = dsrc[y * w * 2 + x * 2 + 0];
            CY  = dsrc[y * w * 2 + x * 2 + 1];
            Cr  = dsrc[y * w * 2 + x * 2 + 2];
            CYn = dsrc[y * w * 2 + x * 2 + 3];

            B = (int)((float)CY + (Cb - 128.0) * 1.375);
            G = (int)((float)CY - (Cb - 128.0) * 0.34375 - (Cr - 128.0) * 0.703125);
            R = (int)((float)CY + (Cr - 128.0) * 1.734375);

            ddst[y * w * 3 + x * 3 + 0] = (unsigned char)R;
            ddst[y * w * 3 + x * 3 + 1] = (unsigned char)G;
            ddst[y * w * 3 + x * 3 + 2] = (unsigned char)B;
			
            R = (int)((float)CYn + (Cb - 128.0) * 1.375);
            G = (int)((float)CYn - (Cb - 128.0) * 0.34375 - (Cr - 128.0) * 0.703125);
            B = (int)((float)CYn + (Cr - 128.0) * 1.734375);

            ddst[y * w * 3 + (x + 1) * 3 + 0] = (unsigned char)R;
            ddst[y * w * 3 + (x + 1) * 3 + 1] = (unsigned char)G;
            ddst[y * w * 3 + (x + 1) * 3 + 2] = (unsigned char)B;
        }
    }
}
