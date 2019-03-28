#ifndef _TOOL_H_
#define _TOOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define UpAlign4(n) (((n) + 3) & ~3)  
#define UpAlign8(n) (((n) + 7) & ~7)  
  

extern void rgb_copy(const void * psrc, void * pdst, int sw, int sh, int dw, int dh, int bpp);  
extern void rbg565_copy(const void * psrc, void * pdst, int sw, int sh, int dw, int dh);  
extern void rbg888_copy(const void * psrc, void * pdst, int sw, int sh, int dw, int dh);  
extern void rbgx8888_copy(const void * psrc, void * pdst, int sw, int sh, int dw, int dh);  
  
 
extern void line_reversal(void * pdata, int w, int h, int bpp);  
extern void rgb565_line_reversal(void * p565, int w, int h);  
extern void rgb888_line_reversal(void * p888, int w, int h);  
extern void rgbx8888_line_reversal(void * p8888, int w, int h);  
  
 
typedef void * (* RGB_CONVERT_FUN)(const void * psrc, int w, int h);  
extern void * rgb565_to_rgb888_buffer(const void * psrc, int w, int h);  
extern void * rgb888_to_rgb565_buffer(const void * psrc, int w, int h);  
extern void * rgb565_to_rgbx8888_buffer(const void * psrc, int w, int h);  
extern void * rgbx8888_to_rgb565_buffer(const void * psrc, int w, int h);  
extern void * rgb888_to_rgbx8888_buffer(const void * psrc, int w, int h);  
extern void * rgbx8888_to_rgb888_buffer(const void * psrc, int w, int h);  
RGB_CONVERT_FUN get_convert_func(int frombpp, int tobpp);

typedef enum
{
	FMT_NV12 = 0,
	FMT_NV21,
	FMT_YV12,
}YUV_TYPE;
extern void RGB_T_NV21(unsigned int width, unsigned int height, unsigned char *rgb, unsigned char *nv21);
extern unsigned save_data(const unsigned char* buffer, int buffersize, const char* filename);
extern void save_png24_file(unsigned char *  in, int w, int h, const char * filename);
extern void load_png24_file(unsigned char * * out, unsigned * w, unsigned * h, const char * filename);
extern int arm_nv12torgb888widthfloat(int width, int height, unsigned char *src, unsigned char *dst, int dstbuf_w);
extern void YV12ToBGR24_Native(unsigned char* pYUV,unsigned char* pBGR24,int width,int height);
extern void yuv420p_to_rgb24(YUV_TYPE type, unsigned char* yuvbuffer,unsigned char* rgbbuffer, int width,int height);
extern void yuv420sp_to_rgb24(YUV_TYPE type, unsigned char* yuvbuffer,unsigned char* rgbbuffer, int width,int height); 
extern int read_png(int width, int height, const char *filename, unsigned char* ptr);
extern void yuv420sp_to_uyvy(YUV_TYPE type, unsigned char* yuvbuffer,unsigned char* uyvybuffer, int width,int height);
//extern png_texture_t * loadPNGTexture(const char *filename);
extern int get_file_size(char* filename);
extern void png_to_420sp(char *path, unsigned char *yuv);
extern unsigned int avm_save_file(const unsigned char* buffer, int buffersize, const char* filename);
extern void save_camera_data(void);
extern void bmp_to_420sp(char *path, unsigned char *yuv);

#ifdef __cplusplus
}
#endif
#endif //end of FILE_H_

