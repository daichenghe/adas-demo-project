/******************************************************************************
  文 件 名   : tool.cpp
  版 本 号   : 1.0
  作    者   : Beartan
  生成日期   : 2017年2月9日
  功能描述   : 工具处理接口
  函数列表   :
              arm_nv12torgb888widthfloat
              init_yuv420p_table
              read_png
              yuv420p_to_rgb24
              yuv420sp_to_rgb24
              YV12ToBGR24_Native

  修改历史   :
  1.日    期   : 2017年2月9日
    作    者   : Beartan
    修改内容   : 创建文件

******************************************************************************/
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "lodepng.h"
#include "tool.h"
#include "SOIL.h"
#ifdef T3BOARD

#include "egl_t3.h"

#endif

static long int crv_tab[256];   
static long int cbu_tab[256];   
static long int cgu_tab[256];   
static long int cgv_tab[256];   
static long int tab_76309[256]; 
static unsigned char clp[1024];   //for clip in CCIR601   

#ifdef T3BOARD

//extern camera_hal camera_obj;

#endif
static void init_yuv420p_table(void);
/*****************************************************************************
 函 数 名  : read_png
 功能描述  : 读取png
 输入参数  : int width             
             int height            
             const char *filename  
             unsigned char* ptr    
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
int read_png(int width, int height, const char *filename, unsigned char* ptr)
{
	  unsigned error;
	  unsigned char* image;
	  unsigned int width_1 = 0; 
	  unsigned int height_1 = 0;;
	  unsigned char* png = 0;
	  size_t pngsize;
	  LodePNGState state;

	  lodepng_state_init(&state);
	  /*optionally customize the state*/
//	  printf("filename is %s\r\n",filename);
	  error = lodepng_load_file(&png, &pngsize, filename);
	  if(!error) error = lodepng_decode(&image, &width_1, &height_1, &state, png, pngsize);
	  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
//	  printf(" width_1 is %d,height_1 is %d\r\n",width_1,height_1);
	  free(png);
	  
	  /*use image here*/
	  /*state contains extra information about the PNG such as text chunks, ...*/
	   memcpy((unsigned char*)ptr, (unsigned char*)image, width*height*4);
	  lodepng_state_cleanup(&state);
	  free(image);
	  /*use image here*/
	

	  return 0;
}
/*****************************************************************************
 函 数 名  : init_yuv420p_table
 功能描述  : 初始化yuv转换表
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
static void init_yuv420p_table() 
{   
    long int crv,cbu,cgu,cgv;   
    int i,ind;      
    static int init = 0;

    if (init == 1) return;

    crv = 104597; cbu = 132201;  /* fra matrise i global.h */   
    cgu = 25675;  cgv = 53279;   
   
    for (i = 0; i < 256; i++)    
    {   
        crv_tab[i] = (i-128) * crv;   
        cbu_tab[i] = (i-128) * cbu;   
        cgu_tab[i] = (i-128) * cgu;   
        cgv_tab[i] = (i-128) * cgv;   
        tab_76309[i] = 76309*(i-16);   
    }   
   
    for (i = 0; i < 384; i++)   
        clp[i] = 0;   
    ind = 384;   
    for (i = 0;i < 256; i++)   
        clp[ind++] = i;   
    ind = 640;   
    for (i = 0;i < 384; i++)   
        clp[ind++] = 255;

    init = 1;
}
/*****************************************************************************
 函 数 名  : yuv420p_to_rgb24
 功能描述  : yuv420p格式转换为rgb24
 输入参数  : YUV_TYPE type             
             unsigned char* yuvbuffer  
             unsigned char* rgbbuffer  
             int width                 
             int height                
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void yuv420p_to_rgb24(YUV_TYPE type, unsigned char* yuvbuffer,unsigned char* rgbbuffer, int width,int height)   
{
    int y1, y2, u, v;    
    unsigned char *py1, *py2;   
    int i, j, c1, c2, c3, c4;   
    unsigned char *d1, *d2;   
    unsigned char *src_u, *src_v;
    static int init_yuv420p = 0;
    
    src_u = yuvbuffer + width * height;   // u
    src_v = src_u + width * height / 4;  //  v

    if (type == FMT_YV12)
    {
        src_v = yuvbuffer + width * height;   // v
        src_u = src_u + width * height / 4;  //  u
    }
    py1 = yuvbuffer;   // y
    py2 = py1 + width;   
    d1 = rgbbuffer;   
    d2 = d1 + 3 * width;   

    init_yuv420p_table();

    for (j = 0; j < height; j += 2)    
    {    
        for (i = 0; i < width; i += 2)    
        {
            u = *src_u++;   
            v = *src_v++;   
   
            c1 = crv_tab[v];   
            c2 = cgu_tab[u];   
            c3 = cgv_tab[v];   
            c4 = cbu_tab[u];   
   
            //up-left   
            y1 = tab_76309[*py1++];    
            *d1++ = clp[384+((y1 + c1)>>16)];     
            *d1++ = clp[384+((y1 - c2 - c3)>>16)];   
            *d1++ = clp[384+((y1 + c4)>>16)];   
   
            //down-left   
            y2 = tab_76309[*py2++];   
            *d2++ = clp[384+((y2 + c1)>>16)];     
            *d2++ = clp[384+((y2 - c2 - c3)>>16)];   
            *d2++ = clp[384+((y2 + c4)>>16)];   
   
            //up-right   
            y1 = tab_76309[*py1++];   
            *d1++ = clp[384+((y1 + c1)>>16)];     
            *d1++ = clp[384+((y1 - c2 - c3)>>16)];   
            *d1++ = clp[384+((y1 + c4)>>16)];   
   
            //down-right   
            y2 = tab_76309[*py2++];   
            *d2++ = clp[384+((y2 + c1)>>16)];     
            *d2++ = clp[384+((y2 - c2 - c3)>>16)];   
            *d2++ = clp[384+((y2 + c4)>>16)];   
        }
        d1  += 3*width;
        d2  += 3*width;
        py1 += width;
        py2 += width;
    }
}
/*****************************************************************************
 函 数 名  : yuv420sp_to_rgb24
 功能描述  : yuv420sp转换为rgb24
 输入参数  : YUV_TYPE type             
             unsigned char* yuvbuffer  
             unsigned char* rgbbuffer  
             int width                 
             int height                
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void yuv420sp_to_rgb24(YUV_TYPE type, unsigned char* yuvbuffer,unsigned char* rgbbuffer, int width,int height)   
{
    int y1, y2, u, v;    
    unsigned char *py1, *py2;   
    int i, j, c1, c2, c3, c4;   
    unsigned char *d1, *d2;   
    unsigned char *src_u;
    static int init_yuv420p = 0;

    src_u = yuvbuffer + width * height;   // u

    py1 = yuvbuffer;   // y
    py2 = py1 + width;   
    d1 = rgbbuffer;   
    d2 = d1 + 3 * width;   

    init_yuv420p_table();

    for (j = 0; j < height; j += 2)    
    {    
        for (i = 0; i < width; i += 2)    
        {
            if (type ==  FMT_NV12)
            {
                u = *src_u++;   
                v = *src_u++;      // v紧跟u，在u的下一个位置
            }
            if (type == FMT_NV21)
            {
                v = *src_u++;   
                u = *src_u++;      // u紧跟v，在v的下一个位置
            }

            c1 = crv_tab[v];   
            c2 = cgu_tab[u];   
            c3 = cgv_tab[v];   
            c4 = cbu_tab[u];   

            //up-left   
            y1 = tab_76309[*py1++];    
            *d1++ = clp[384+((y1 + c1)>>16)];     
            *d1++ = clp[384+((y1 - c2 - c3)>>16)];   
            *d1++ = clp[384+((y1 + c4)>>16)];   

            //down-left   
            y2 = tab_76309[*py2++];   
            *d2++ = clp[384+((y2 + c1)>>16)];     
            *d2++ = clp[384+((y2 - c2 - c3)>>16)];   
            *d2++ = clp[384+((y2 + c4)>>16)];   

            //up-right   
            y1 = tab_76309[*py1++];   
            *d1++ = clp[384+((y1 + c1)>>16)];     
            *d1++ = clp[384+((y1 - c2 - c3)>>16)];   
            *d1++ = clp[384+((y1 + c4)>>16)];   

            //down-right   
            y2 = tab_76309[*py2++];   
            *d2++ = clp[384+((y2 + c1)>>16)];     
            *d2++ = clp[384+((y2 - c2 - c3)>>16)];   
            *d2++ = clp[384+((y2 + c4)>>16)];   
        }
        d1  += 3*width;
        d2  += 3*width;
        py1 += width;
        py2 += width;
    }
}
/*****************************************************************************
 函 数 名  : arm_nv12torgb888widthfloat
 功能描述  : nv12转换888
 输入参数  : int width           
             int height          
             unsigned char *src  
             unsigned char *dst  
             int dstbuf_w        
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
int arm_nv12torgb888widthfloat(int width, int height, unsigned char *src, unsigned char *dst, int dstbuf_w)
{
    unsigned char *py, *pu, *pv;
    int line, col;
    float y, u, v, yy, vr, ug, vg, ub;
    float r, g, b;
    float ug_coeff = 0.3451;// (88 / 255.0);
    float ub_coeff = 1.78039;// (454 / 255.0);
    float vg_coeff = 0.71765;// (183 / 255.0);
    float vr_coeff = 1.4078;// (359 / 255.0);

    py = src;
    pu = py + (width * height);
    pv = pu + 1;
    y = *py++;

    //yy
    yy = y;

    u = *pu - 128;
    ug = ug_coeff * u;
    ub = ub_coeff * u;


    v = *pv - 128;
    vg = vg_coeff * v;
    vr = vr_coeff * v;


    for (line = 0; line < height; line++)
    {
        for (col = 0; col < width; col++)
        {
            r = yy + vr;
            g = yy - ug - vg;
            b = yy + ub;
            if (r < 0)   r = 0;
            if (r > 255) r = 255;
            if (g < 0)   g = 0;
            if (g > 255) g = 255;
            if (b < 0)   b = 0;
            if (b > 255) b = 255;

            *dst++ = b;
            *dst++ = g;
            *dst++ = r;

            y = *py++;
            yy = y;
            if (col & 1)
            {
                pu += 2;
                pv = pu + 1;
                u = *pu - 128;
                ug = ug_coeff * u;
                ub = ub_coeff * u;
                v = *pv - 128;
                vg = vg_coeff * v;
                vr = vr_coeff * v;
            }
        }
        dst += dstbuf_w - width;
        if ((line & 1) == 0)
        {
            //even line: rewind
            pu -= width;
            pv = pu + 1;
        }
    }
    return 0;
}

/*****************************************************************************
 函 数 名  : YV12ToBGR24_Native
 功能描述  : yv12格式转换bgr24
 输入参数  : unsigned char* pYUV    
             unsigned char* pBGR24  
             int width              
             int height             
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void YV12ToBGR24_Native(unsigned char* pYUV,unsigned char* pBGR24,int width,int height)
{
    if (width < 1 || height < 1 || pYUV == NULL || pBGR24 == NULL)
        return ;
    const long len = width * height;
    unsigned char* yData = pYUV;
    unsigned char* vData = &yData[len];
    unsigned char* uData = &vData[len >> 2];

    int bgr[3];
    int yIdx,uIdx,vIdx,idx;
    for (int i = 0;i < height;i++){
        for (int j = 0;j < width;j++){
            yIdx = i * width + j;
            vIdx = (i/2) * (width/2) + (j/2);
            uIdx = vIdx;
        
            bgr[2] = (int)(yData[yIdx] + 1.732446 * (uData[vIdx] - 128));                                    // b分量
            bgr[1] = (int)(yData[yIdx] - 0.698001 * (uData[uIdx] - 128) - 0.703125 * (vData[vIdx] - 128));    // g分量
            bgr[0] = (int)(yData[yIdx] + 1.370705 * (vData[uIdx] - 128));                                    // r分量

            for (int k = 0;k < 3;k++){
                idx = (i * width + j) * 3 + k;
                if(bgr[k] >= 0 && bgr[k] <= 255)
                    pBGR24[idx] = bgr[k];
                else
                    pBGR24[idx] = (bgr[k] < 0)?0:255;
            }
        }
    }
}
/*****************************************************************************
 函 数 名  : rgb_copy
 功能描述  : rgb复制
 输入参数  : const void * psrc  
             void * pdst        
             int sw             
             int sh             
             int dw             
             int dh             
             int bpp            
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void rgb_copy(const void * psrc, void * pdst, int sw, int sh, int dw, int dh, int bpp)  
{  
    int bytes = UpAlign8(bpp) >> 3; // bpp / 8  
    int srclinesize = UpAlign4(sw * bytes);  
    int dstlinesize = UpAlign4(dw * bytes);  
    int copylinesize = srclinesize < dstlinesize ? srclinesize : dstlinesize;  
    int copylines = sh < dh ? sh : dh;  
      
    const unsigned char * psrcline = (const unsigned char *)psrc;  
    const unsigned char * pend = psrcline + copylines * srclinesize;  
    unsigned char * pdstline = (unsigned char *)pdst;  
      
    while (psrcline < pend) {  
        memcpy(pdstline, psrcline, copylinesize);  
        psrcline += srclinesize;  
        pdstline += dstlinesize;  
    }  
}  
/*****************************************************************************
 函 数 名  : rbg565_copy
 功能描述  : rgb565拷贝
 输入参数  : const void * psrc  
             void * pdst        
             int sw             
             int sh             
             int dw             
             int dh             
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void rbg565_copy(const void * psrc, void * pdst, int sw, int sh, int dw, int dh)  
{  
    rgb_copy(psrc, pdst, sw, sh, dw, dh, 16);  
}  
/*****************************************************************************
 函 数 名  : rbg888_copy
 功能描述  : rgb888拷贝
 输入参数  : const void * psrc  
             void * pdst        
             int sw             
             int sh             
             int dw             
             int dh             
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void rbg888_copy(const void * psrc, void * pdst, int sw, int sh, int dw, int dh)  
{  
    rgb_copy(psrc, pdst, sw, sh, dw, dh, 24);  
}  
/*****************************************************************************
 函 数 名  : rbgx8888_copy
 功能描述  : rgbx8888拷贝
 输入参数  : const void * psrc  
             void * pdst        
             int sw             
             int sh             
             int dw             
             int dh             
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void rbgx8888_copy(const void * psrc, void * pdst, int sw, int sh, int dw, int dh)  
{  
    rgb_copy(psrc, pdst, sw, sh, dw, dh, 32);  
}  
/*****************************************************************************
 函 数 名  : line_reversal
 功能描述  : 行数据反转
 输入参数  : void * pdata  
             int w         
             int h         
             int bpp       
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void line_reversal(void * pdata, int w, int h, int bpp)  
{  
    int bytes     = UpAlign8(bpp) >> 3; // bpp / 8  
    int linesize  = UpAlign4(w * bytes);//4
    int copylines = h >> 1;  
  
    int i;  
    unsigned char * pline = NULL;  
    unsigned char * pline1 = NULL;  
    unsigned char * linebuffer = NULL;  
  
    if (pdata && w > 0 && h > 1) {
        linebuffer = (unsigned char *)malloc(linesize);  
        if (linebuffer) {  
            pline  = (unsigned char *)pdata;  
            pline1 = (unsigned char *)pdata + linesize * (h - 1);  
            for (i = 0; i < copylines; i++) {  
                memcpy(linebuffer, pline, linesize);  
                memcpy(pline, pline1, linesize);  
                memcpy(pline1, linebuffer, linesize);  
                pline  += linesize;  
                pline1 -= linesize;  
            }  
            free(linebuffer);  
        }  
    }  
}  
/*****************************************************************************
 函 数 名  : rgb565_line_reversal
 功能描述  : rgb565行反转
 输入参数  : void * p565  
             int w        
             int h        
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void rgb565_line_reversal(void * p565, int w, int h)  
{  
    line_reversal(p565, w, h, 16);  
}  
/*****************************************************************************
 函 数 名  : rgb888_line_reversal
 功能描述  : rgb888行反转
 输入参数  : void * p888  
             int w        
             int h        
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void rgb888_line_reversal(void * p888, int w, int h)  
{  
    line_reversal(p888, w, h, 24);  
}  
/*****************************************************************************
 函 数 名  : rgbx8888_line_reversal
 功能描述  : rgbx8888行数据反转
 输入参数  : void * p8888  
             int w         
             int h         
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void rgbx8888_line_reversal(void * p8888, int w, int h)  
{  
    line_reversal(p8888, w, h, 32);  
}  
/*****************************************************************************
 函 数 名  : rgb565_to_rgb888
 功能描述  : rgb565转换rgb888
 输入参数  : const void * psrc  
             int w              
             int h              
             void * pdst        
 输出参数  : 无
 返 回 值  : static
*****************************************************************************/
static int rgb565_to_rgb888(const void * psrc, int w, int h, void * pdst)  
{  
    int srclinesize = UpAlign4(w * 2);  
    int dstlinesize = UpAlign4(w * 3);  
  
    const unsigned char  * psrcline;  
    const unsigned short * psrcdot;  
    unsigned char  * pdstline;  
    unsigned char  * pdstdot;  
  
    int i,j;  
  
    if (!psrc || !pdst || w <= 0 || h <= 0) {  
        printf("rgb565_to_rgb888 : parameter error\n");  
        return -1;  
    }  
  
    psrcline = (const unsigned char *)psrc;  
    pdstline = (unsigned char *)pdst;  
    for (i=0; i<h; i++) {  
        psrcdot = (const unsigned short *)psrcline;  
        pdstdot = pdstline;  
        for (j=0; j<w; j++) {  
/*			
            //565 b|g|r -> 888 r|g|b  
            *pdstdot++ = (unsigned char)(((*psrcdot) >> 0 ) << 3);  
            *pdstdot++ = (unsigned char)(((*psrcdot) >> 5 ) << 2);  
            *pdstdot++ = (unsigned char)(((*psrcdot) >> 11) << 3);  
*/
			//565 r|g|b -> 888 r|g|b  
            *pdstdot++ = (unsigned char)(((*psrcdot) >> 11) << 3); 
            *pdstdot++ = (unsigned char)(((*psrcdot) >> 5 ) << 2);  
            *pdstdot++ = (unsigned char)(((*psrcdot) >> 0 ) << 3);  
            psrcdot++;  
        }  
        psrcline += srclinesize;  
        pdstline += dstlinesize;  
    }  
  
    return 0;  
}  
/*****************************************************************************
 函 数 名  : rgb888_to_rgb565
 功能描述  : rgb888转换为rgb565
 输入参数  : const void * psrc  
             int w              
             int h              
             void * pdst        
 输出参数  : 无
 返 回 值  : static
*****************************************************************************/
static int rgb888_to_rgb565(const void * psrc, int w, int h, void * pdst)  
{  
    int srclinesize = UpAlign4(w * 3);  
    int dstlinesize = UpAlign4(w * 2);  
      
    const unsigned char * psrcline;  
    const unsigned char * psrcdot;  
    unsigned char  * pdstline;  
    unsigned short * pdstdot;  
      
    int i,j;  
      
    if (!psrc || !pdst || w <= 0 || h <= 0) {  
        printf("rgb888_to_rgb565 : parameter error\n");  
        return -1;  
    }  
  
    psrcline = (const unsigned char *)psrc;  
    pdstline = (unsigned char *)pdst;  
    for (i=0; i<h; i++) {  
        psrcdot = psrcline;  
        pdstdot = (unsigned short *)pdstline;  
        for (j=0; j<w; j++) {  
            //888 r|g|b -> 565 b|g|r  
            *pdstdot =  (((psrcdot[0] >> 3) & 0x1F) << 0)//r  
                        |(((psrcdot[1] >> 2) & 0x3F) << 5)//g  
                        |(((psrcdot[2] >> 3) & 0x1F) << 11);//b  
            psrcdot += 3;  
            pdstdot++;  
        }  
        psrcline += srclinesize;  
        pdstline += dstlinesize;  
    }  
  
    return 0;  
}  
/*****************************************************************************
 函 数 名  : rgb565_to_rgbx8888
 功能描述  : rgb565转换为rgba8888
 输入参数  : const void * psrc  
             int w              
             int h              
             void * pdst        
 输出参数  : 无
 返 回 值  : static
*****************************************************************************/
static int rgb565_to_rgbx8888(const void * psrc, int w, int h, void * pdst)  
{  
    int srclinesize = UpAlign4(w * 2);  
    int dstlinesize = UpAlign4(w * 4);  
  
    const unsigned char  * psrcline;  
    const unsigned short * psrcdot;  
    unsigned char  * pdstline;  
    unsigned char  * pdstdot;  
  
    int i,j;  
  
    if (!psrc || !pdst || w <= 0 || h <= 0) {  
        printf("rgb565_to_rgbx8888 : parameter error\n");  
        return -1;  
    }  
  
    psrcline = (const unsigned char *)psrc;  
    pdstline = (unsigned char *)pdst;  
    for (i=0; i<h; i++) {  
        psrcdot = (const unsigned short *)psrcline;  
        pdstdot = pdstline;  
        for (j=0; j<w; j++) {  
            pdstdot++;  
            *pdstdot++ = (unsigned char)(((*psrcdot) >> 0 ) << 3);  
            *pdstdot++ = (unsigned char)(((*psrcdot) >> 5 ) << 2);  
            *pdstdot++ = (unsigned char)(((*psrcdot) >> 11) << 3);  
            psrcdot++;  
        }  
        psrcline += srclinesize;  
        pdstline += dstlinesize;  
    }  
      
    return 0;  
}  
/*****************************************************************************
 函 数 名  : rgbx8888_to_rgb565
 功能描述  : rgbx8888转换rgb65
 输入参数  : const void * psrc  
             int w              
             int h              
             void * pdst        
 输出参数  : 无
 返 回 值  : static
*****************************************************************************/
static int rgbx8888_to_rgb565(const void * psrc, int w, int h, void * pdst)  
{  
    int srclinesize = UpAlign4(w * 4);  
    int dstlinesize = UpAlign4(w * 2);  
      
    const unsigned char * psrcline;  
    const unsigned char * psrcdot;  
    unsigned char  * pdstline;  
    unsigned short * pdstdot;  
      
    int i,j;  
      
    if (!psrc || !pdst || w <= 0 || h <= 0) {  
        printf("rgbx8888_to_rgb565 : parameter error\n");  
        return -1;  
    }  
      
    psrcline = (const unsigned char *)psrc;  
    pdstline = (unsigned char *)pdst;  
    for (i=0; i<h; i++) {  
        psrcdot = psrcline;  
        pdstdot = (unsigned short *)pdstline;  
        for (j=0; j<w; j++) {  
            //888 r|g|b -> 565 b|g|r  
            *pdstdot =  (((psrcdot[1] >> 3) & 0x1F) << 0)//r  
                |(((psrcdot[2] >> 2) & 0x3F) << 5)//g  
                |(((psrcdot[3] >> 3) & 0x1F) << 11);//b  
            psrcdot += 4;  
            pdstdot++;  
        }  
        psrcline += srclinesize;  
        pdstline += dstlinesize;  
    }  
      
    return 0;  
}  
/*****************************************************************************
 函 数 名  : rgb888_to_rgbx8888
 功能描述  : rgb888转换为rgba8888
 输入参数  : const void * psrc  
             int w              
             int h              
             void * pdst        
 输出参数  : 无
 返 回 值  : static
*****************************************************************************/
static int rgb888_to_rgbx8888(const void * psrc, int w, int h, void * pdst)  
{  
    int srclinesize = UpAlign4(w * 3);  
    int dstlinesize = UpAlign4(w * 4);  
  
    const unsigned char * psrcline;  
    const unsigned char * psrcdot;  
    unsigned char  * pdstline;  
    unsigned char  * pdstdot;  
  
    int i,j;  
  
    if (!psrc || !pdst || w <= 0 || h <= 0) {  
        printf("rgb888_to_rgbx8888 : parameter error\n");  
        return -1;  
    }  
  
    psrcline = (const unsigned char *)psrc;  
    pdstline = (unsigned char *)pdst;  
    for (i=0; i<h; i++) {  
        psrcdot = psrcline;  
        pdstdot = pdstline;  
        for (j=0; j<w; j++) {  
            *pdstdot++ = 0;  
            *pdstdot++ = *psrcdot++;  
            *pdstdot++ = *psrcdot++;  
            *pdstdot++ = *psrcdot++;  
        }  
        psrcline += srclinesize;  
        pdstline += dstlinesize;  
    }  
      
    return 0;  
}  
/*****************************************************************************
 函 数 名  : rgbx8888_to_rgb888
 功能描述  : rgbx8888转换为rgb888
 输入参数  : const void * psrc  
             int w              
             int h              
             void * pdst        
 输出参数  : 无
 返 回 值  : static
*****************************************************************************/
static int rgbx8888_to_rgb888(const void * psrc, int w, int h, void * pdst)  
{  
    int srclinesize = UpAlign4(w * 4);  
    int dstlinesize = UpAlign4(w * 3);  
      
    const unsigned char * psrcline;  
    const unsigned char * psrcdot;  
    unsigned char  * pdstline;  
    unsigned char  * pdstdot;  
      
    int i,j;  
      
    if (!psrc || !pdst || w <= 0 || h <= 0) {  
        printf("rgbx8888_to_rgb888 : parameter error\n");  
        return -1;  
    }  
      
    psrcline = (const unsigned char *)psrc;  
    pdstline = (unsigned char *)pdst;  
    for (i=0; i<h; i++) {  
        psrcdot = psrcline;  
        pdstdot = pdstline;  
        for (j=0; j<w; j++) {  
            psrcdot++;  
            *pdstdot++ = *psrcdot++;  
            *pdstdot++ = *psrcdot++;  
            *pdstdot++ = *psrcdot++;  
        }  
        psrcline += srclinesize;  
        pdstline += dstlinesize;  
    }  
      
    return 0;  
}  
/*****************************************************************************
 函 数 名  : rgb565_to_rgb888_buffer
 功能描述  : rgb565转换为rgb888
 输入参数  : const void * psrc  
             int w              
             int h              
 输出参数  : 无
 返 回 值  : void
*****************************************************************************/
void * rgb565_to_rgb888_buffer(const void * psrc, int w, int h)  
{  
    int size = h * UpAlign4(w * 3);  
    void * pdst = NULL;  
    if (psrc && w > 0 && h > 0) {  
        pdst = malloc(size);  
        if (pdst) {  
            if (rgb565_to_rgb888(psrc, w, h, pdst)) {  
                free(pdst);  
                pdst = NULL;  
            }  
        }  
    }  
    return pdst;  
}  
/*****************************************************************************
 函 数 名  : rgb888_to_rgb565_buffer
 功能描述  : rgb888转换为rgb565
 输入参数  : const void * psrc  
             int w              
             int h              
 输出参数  : 无
 返 回 值  : void
*****************************************************************************/
void * rgb888_to_rgb565_buffer(const void * psrc, int w, int h)  
{  
    int size = h * UpAlign4(w * 2);  
    void * pdst = NULL;  
    if (psrc && w > 0 && h > 0) {  
        pdst = malloc(size);  
        if (pdst) {  
            if (rgb888_to_rgb565(psrc, w, h, pdst)) {  
                free(pdst);  
                pdst = NULL;  
            }  
        }  
    }  
    return pdst;  
}  
/*****************************************************************************
 函 数 名  : rgb565_to_rgbx8888_buffer
 功能描述  : rgb565转换为rgbx8888
 输入参数  : const void * psrc  
             int w              
             int h              
 输出参数  : 无
 返 回 值  : void
*****************************************************************************/
void * rgb565_to_rgbx8888_buffer(const void * psrc, int w, int h)  
{  
    int size = h * UpAlign4(w * 4);  
    void * pdst = NULL;  
    if (psrc && w > 0 && h > 0) {  
        pdst = malloc(size);  
        if (pdst) {  
            if (rgb565_to_rgbx8888(psrc, w, h, pdst)) {  
                free(pdst);  
                pdst = NULL;  
            }  
        }  
    }  
    return pdst;  
}  
/*****************************************************************************
 函 数 名  : rgbx8888_to_rgb565_buffer
 功能描述  : rgbx8888转换为rgb565
 输入参数  : const void * psrc  
             int w              
             int h              
 输出参数  : 无
 返 回 值  : void
*****************************************************************************/
void * rgbx8888_to_rgb565_buffer(const void * psrc, int w, int h)  
{  
    int size = h * UpAlign4(w * 2);  
    void * pdst = NULL;  
    if (psrc && w > 0 && h > 0) {  
        pdst = malloc(size);  
        if (pdst) {  
            if (rgbx8888_to_rgb565(psrc, w, h, pdst)) {  
                free(pdst);  
                pdst = NULL;  
            }  
        }  
    }  
    return pdst;  
}  
/*****************************************************************************
 函 数 名  : rgb888_to_rgbx8888_buffer
 功能描述  : fgb888转换为rgbx8888
 输入参数  : const void * psrc  
             int w              
             int h              
 输出参数  : 无
 返 回 值  : void
*****************************************************************************/
void * rgb888_to_rgbx8888_buffer(const void * psrc, int w, int h)  
{  
    int size = h * UpAlign4(w * 4);  
    void * pdst = NULL;  
    if (psrc && w > 0 && h > 0) {  
        pdst = malloc(size);  
        if (pdst) {  
            if (rgb888_to_rgbx8888(psrc, w, h, pdst)) {  
                free(pdst);  
                pdst = NULL;  
            }  
        }  
    }  
    return pdst;  
}  
/*****************************************************************************
 函 数 名  : rgbx8888_to_rgb888_buffer
 功能描述  : fgba8888转换rgb888
 输入参数  : const void * psrc  
             int w              
             int h              
 输出参数  : 无
 返 回 值  : void
*****************************************************************************/
void * rgbx8888_to_rgb888_buffer(const void * psrc, int w, int h)  
{  
    int size = h * UpAlign4(w * 3);  
    void * pdst = NULL;  
    if (psrc && w > 0 && h > 0) {  
        pdst = malloc(size);  
        if (pdst) {  
            if (rgbx8888_to_rgb888(psrc, w, h, pdst)) {  
                free(pdst);  
                pdst = NULL;  
            }  
        }  
    }  
    return pdst;  
}  
/*****************************************************************************
 函 数 名  : yuv420sp_to_uyvy
 功能描述  : yuv420sp转换为uyvy格式
 输入参数  : YUV_TYPE type              
             unsigned char* yuvbuffer   
             unsigned char* uyvybuffer  
             int width                  
             int height                 
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void yuv420sp_to_uyvy(YUV_TYPE type, unsigned char* yuvbuffer,unsigned char* uyvybuffer, int width,int height) 
{
    unsigned char  CY00, CY01, CY10, CY11, Cb, Cr;
	if (type == FMT_NV21)
	{
	    for (int j = 0;j < (height/2);j++)
		{
	        for (int i = 0;i < (width/2);i++)
			{
				CY00 = yuvbuffer[(j*2)*width + i*2 + 0];
				CY01 = yuvbuffer[(j*2)*width + i*2 + 1];
				CY10 = yuvbuffer[(j*2+1)*width + i*2 + 0];
				CY11 = yuvbuffer[(j*2+1)*width + i*2 + 1];
				
				Cr = yuvbuffer[height*width + j*width + i*2 + 0];
				Cb = yuvbuffer[height*width + j*width + i*2 + 1];

				uyvybuffer[(j*2)*width*2 + i*4 + 0] = Cb;
				uyvybuffer[(j*2)*width*2 + i*4 + 1] = CY00;
				uyvybuffer[(j*2)*width*2 + i*4 + 2] = Cr;
				uyvybuffer[(j*2)*width*2 + i*4 + 3] = CY01;
				uyvybuffer[(j*2+1)*width*2 + i*4 + 0] = Cb;
				uyvybuffer[(j*2+1)*width*2 + i*4 + 1] = CY10;
				uyvybuffer[(j*2+1)*width*2 + i*4 + 2] = Cr;
				uyvybuffer[(j*2+1)*width*2 + i*4 + 3] = CY11;
	        }
	    }
	}
	else if (type == FMT_NV12)
	{
	    for (int j = 0;j < (height/2);j++)
		{
	        for (int i = 0;i < (width/2);i++)
			{
				CY00 = yuvbuffer[(j*2)*width + i*2 + 0];
				CY01 = yuvbuffer[(j*2)*width + i*2 + 1];
				CY10 = yuvbuffer[(j*2+1)*width + i*2 + 0];
				CY11 = yuvbuffer[(j*2+1)*width + i*2 + 1];
				
				Cb = yuvbuffer[height*width + j*width + i*2 + 0];
				Cr = yuvbuffer[height*width + j*width + i*2 + 1];

				uyvybuffer[(j*2)*width*2 + i*4 + 0] = Cb;
				uyvybuffer[(j*2)*width*2 + i*4 + 1] = CY00;
				uyvybuffer[(j*2)*width*2 + i*4 + 2] = Cr;
				uyvybuffer[(j*2)*width*2 + i*4 + 3] = CY01;
				uyvybuffer[(j*2+1)*width*2 + i*4 + 0] = Cb;
				uyvybuffer[(j*2+1)*width*2 + i*4 + 1] = CY10;
				uyvybuffer[(j*2+1)*width*2 + i*4 + 2] = Cr;
				uyvybuffer[(j*2+1)*width*2 + i*4 + 3] = CY11;
	        }
	    }
	}
}
/*****************************************************************************
 函 数 名  : get_file_size
 功能描述  : 获得文件大小
 输入参数  : char* filename  
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
int get_file_size(char* filename)  
{  
    struct stat statbuf;  
    stat(filename,&statbuf);  
    int size=statbuf.st_size;  
  
    return size;  
} 
/*****************************************************************************
 函 数 名  : load_png24_file
 功能描述  : 装载png24位
 输入参数  : unsigned char * * out  
             unsigned * w           
             unsigned * h           
             const char * filename  
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void load_png24_file(unsigned char * * out, unsigned * w, unsigned * h, const char * filename)
{
	lodepng_decode24_file(out, w, h, filename);
}
/*****************************************************************************
 函 数 名  : save_png24_file
 功能描述  : 保存png23位
 输入参数  : unsigned char *  in    
             int w                  
             int h                  
             const char * filename  
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void save_png24_file(unsigned char *  in, int w, int h, const char * filename)
{
	lodepng_encode24_file(filename, in, w, h);
}
/*****************************************************************************
 函 数 名  : save_data
 功能描述  : 保存数据
 输入参数  : const unsigned char* buffer  
             size_t buffersize            
             const char* filename         
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
unsigned save_data(const unsigned char* buffer, int buffersize, const char* filename)
{
  FILE* file;
  file = fopen(filename, "wb" );
  if(!file) return 79;
  fwrite((char*)buffer , 1 , buffersize, file);
  fclose(file);
  return 0;
}
/*****************************************************************************
 函 数 名  : RGB_T_NV21
 功能描述  : rgb转换nv21格式
 输入参数  : unsigned int width   
             unsigned int height  
             unsigned char *rgb   
             unsigned char *nv21  
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void RGB_T_NV21(unsigned int width, unsigned int height, unsigned char *rgb, unsigned char *nv21)
{
	const int nv_start = width * height;
	unsigned int  i, j, rgb_index = 0;
	unsigned char r, g, b;
	float y, u, v;
	int nv_index = 0;

	printf("%s,%d, rgb=%p, nv21=%p\n",__func__,__LINE__, rgb,nv21);
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++) {
			nv_index = i / 2 * width + j - j % 2;

			r = rgb[rgb_index + 0];
			g = rgb[rgb_index + 1];
			b = rgb[rgb_index + 2];

			rgb_index += 3;			

			y = 0.299*r + 0.587*g + 0.114*b;
			u = -0.1687*r - 0.3313*g + 0.5*b + 128;
			v = 0.5*r - 0.4187*g - 0.0813*b + 128;

			if (y > 255)   y = 255;
			if (u > 255)   u = 255;
			if (v > 255)   v = 255;
			if (y < 0)     y = 0;
			if (u < 0)     u = 0;
			if (v < 0)     v = 0;

			nv21[rgb_index/3] = (unsigned char)y;
			nv21[nv_start + nv_index] = (unsigned char)v;
			nv21[nv_start + nv_index + 1] = (unsigned char)u;
		}
//		printf("%s,%d, i=%d\n",__func__,__LINE__, i);
	}
}
/*****************************************************************************
 函 数 名  : NV21_T_RGB
 功能描述  : nv21转换rgb
 输入参数  : unsigned int width   
             unsigned int height  
             unsigned char *nv21  
             unsigned char *rgb   
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void NV21_T_RGB(unsigned int width, unsigned int height, unsigned char *nv21, unsigned char *rgb)
{
	const int nv_start = width * height;
	unsigned int  i, j, index = 0, rgb_index = 0;
	unsigned char y, u, v;
	int r, g, b, nv_index = 0;


	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++) {
			nv_index = i / 2 * width + j - j % 2;

			y = nv21[rgb_index];
			u = nv21[nv_start + nv_index];
			v = nv21[nv_start + nv_index + 1];

			b = y + (140 * (v - 128)) / 100;  //r
			g = y - (34 * (u - 128)) / 100 - (71 * (v - 128)) / 100; //g
			r = y + (177 * (u - 128)) / 100; //b

			if (r > 255)   r = 255;
			if (g > 255)   g = 255;
			if (b > 255)   b = 255;
			if (r < 0)     r = 0;
			if (g < 0)     g = 0;
			if (b < 0)     b = 0;

			index = rgb_index % width + (height - i - 1) * width;
			rgb[index * 3 + 0] = b;
			rgb[index * 3 + 1] = g;
			rgb[index * 3 + 2] = r;
			rgb_index++;
		}
	}
}
/*****************************************************************************
 函 数 名  : png_to_420sp
 功能描述  : png转换yuv420sp
 输入参数  : char *path          
             unsigned char *yuv  
 输出参数  : 无
 返 回 值  : 
*****************************************************************************/
void png_to_420sp(char *path, unsigned char *yuv)
{
	unsigned int width, height;
	unsigned char *png_debug_data = NULL;
	load_png24_file(&png_debug_data, &width, &height, path);
	RGB_T_NV21(width, height, png_debug_data, yuv);
	free(png_debug_data);

}

void bmp_to_420sp(char *path, unsigned char *yuv)
{
	int width, height;
	int channels;
	unsigned char *bmp_data = NULL;
	bmp_data = SOIL_load_image(path, &width, &height, &channels, SOIL_LOAD_RGB);
	RGB_T_NV21(width, height, bmp_data, yuv);
	free(bmp_data);

}

unsigned int avm_save_file(const unsigned char* buffer, int buffersize, const char* filename)
{
  FILE* file;
  file = fopen(filename, "wb" );
  if(!file) return -1;
  fwrite((char*)buffer , 1 , buffersize, file);
  fclose(file);
  return 0;
}
#ifdef T3BOARD

#ifdef PNG_SAVE

void save_camera_data(void)
{
	int i = 0;
	int j = 0;
	int ret = 0;
	static int num = 0;
	char buft[128] = {'\0'};
	unsigned char *camera_data_rgb = NULL;
	unsigned char *camera_data_first = NULL;
	unsigned char *camera_data_second = NULL;
	unsigned char *camera_data_signle = NULL;
	
	camera_data_first = (unsigned char *)malloc(sizeof(unsigned char)*DMAFD_W*DMAFD_H*3/2);
	camera_data_second = (unsigned char *)malloc(sizeof(unsigned char)*DMAFD_W*DMAFD_H*3/2);
	camera_data_rgb = (unsigned char *)malloc(DMAFD_W*DMAFD_H*3*sizeof(unsigned char));

	camera_data_signle = (unsigned char *)malloc(DMAFD_W*DMAFD_H*3*sizeof(unsigned char));
	//first 左后 second 右前
    printf("start get camera data\n");
	get_camera_yuv_data(camera_data_first, camera_data_second);
	//camera_obj.get_camera_data(camera_data_first,camera_data_second);


	yuv420sp_to_rgb24(FMT_NV21, camera_data_first, camera_data_rgb, DMAFD_W, DMAFD_H);    

	for(i = 0;i < CAM_HEIGHT;i++)
	{
		for(j = 0;j < CAM_WIDTH;j++)
		{
			camera_data_signle[(CAM_WIDTH*i+j)*3+0] = camera_data_rgb[(DMAFD_W*i+j)*3+0];
			camera_data_signle[(CAM_WIDTH*i+j)*3+1] = camera_data_rgb[(DMAFD_W*i+j)*3+1];
			camera_data_signle[(CAM_WIDTH*i+j)*3+2] = camera_data_rgb[(DMAFD_W*i+j)*3+2];
		}
	}
	
	sprintf(buft, "/mnt/sdcard/mmcblk1p1/photo_%d.png",num);    
	
	
	ret = lodepng_encode24_file(buft, camera_data_signle, DMAFD_W, DMAFD_H);
	if (ret == 0)
	{
		printf("save left camera data.png!\n");
	}
	else
	{
		printf("ret is %d\r\n",num);
		return ;
	}
	num++;
	
	for(i = 0;i < CAM_HEIGHT;i++)
	{
		for(j = 0;j < CAM_WIDTH;j++)
		{
			camera_data_signle[(CAM_WIDTH*i+j)*3+0] = camera_data_rgb[(DMAFD_W*i+CAM_WIDTH+j)*3+0];
			camera_data_signle[(CAM_WIDTH*i+j)*3+1] = camera_data_rgb[(DMAFD_W*i+CAM_WIDTH+j)*3+1];
			camera_data_signle[(CAM_WIDTH*i+j)*3+2] = camera_data_rgb[(DMAFD_W*i+CAM_WIDTH+j)*3+2];
		}
	}
	
	
	sprintf(buft, "/mnt/sdcard/mmcblk1p1/photo_%d.png",num);                                                   
	ret = lodepng_encode24_file(buft, camera_data_signle, CAM_WIDTH, CAM_HEIGHT);
	if (ret == 0)
	{
		printf("save near camera data.png!\n");
	}
	else
	{
		printf("ret is %d\r\n",ret);
		return ;
	}
	num++;
	yuv420sp_to_rgb24(FMT_NV21, camera_data_second, camera_data_rgb, DMAFD_W, DMAFD_H);    


	for(i = 0;i < CAM_HEIGHT;i++)
	{
		for(j = 0;j < CAM_WIDTH;j++)
		{
			camera_data_signle[(CAM_WIDTH*i+j)*3+0] = camera_data_rgb[(DMAFD_W*i+j)*3+0];
			camera_data_signle[(CAM_WIDTH*i+j)*3+1] = camera_data_rgb[(DMAFD_W*i+j)*3+1];
			camera_data_signle[(CAM_WIDTH*i+j)*3+2] = camera_data_rgb[(DMAFD_W*i+j)*3+2];
		}
	}
	
	sprintf(buft, "/mnt/sdcard/mmcblk1p1/photo_%d.png",num);    
	
	
	ret = lodepng_encode24_file(buft, camera_data_signle, CAM_WIDTH, CAM_HEIGHT);
	if (ret == 0)
	{
		printf("save right camera data.png!\n");
	}
	else
	{
		printf("ret is %d\r\n",num);
		return ;
	}
	num++;
	
	for(i = 0;i < CAM_HEIGHT;i++)
	{
		for(j = 0;j < CAM_WIDTH;j++)
		{
			camera_data_signle[(CAM_WIDTH*i+j)*3+0] = camera_data_rgb[(DMAFD_W*i+CAM_WIDTH+j)*3+0];
			camera_data_signle[(CAM_WIDTH*i+j)*3+1] = camera_data_rgb[(DMAFD_W*i+CAM_WIDTH+j)*3+1];
			camera_data_signle[(CAM_WIDTH*i+j)*3+2] = camera_data_rgb[(DMAFD_W*i+CAM_WIDTH+j)*3+2];
		}
	}
	
	
	sprintf(buft, "/mnt/sdcard/mmcblk1p1/photo_%d.png",num);                                                   
	ret = lodepng_encode24_file(buft, camera_data_signle, CAM_WIDTH, CAM_HEIGHT);
	if (ret == 0)
	{
		printf("save front camera data.png!\n");
	}
	else
	{
		printf("ret is %d\r\n",ret);
		return ;
	}
	num++;
	printf("save camera data ok\r\n");
	free(camera_data_rgb);
	free(camera_data_first);
	free(camera_data_second);
	free(camera_data_signle);
	
}
#else

void save_camera_data(void)
{
	int i = 0;
	int j = 0;
	int ret = 0;
	static int num = 0;
	char buft[128] = {'\0'};
	unsigned char *camera_data_rgb = NULL;
	unsigned char *camera_data_first = NULL;
	unsigned char *camera_data_second = NULL;
	unsigned char *camera_data_signle = NULL;
	
	camera_data_first = (unsigned char *)malloc(sizeof(unsigned char)*DMAFD_W*DMAFD_H*3/2);
	camera_data_second = (unsigned char *)malloc(sizeof(unsigned char)*DMAFD_W*DMAFD_H*3/2);
	camera_data_rgb = (unsigned char *)malloc(DMAFD_W*DMAFD_H*3*sizeof(unsigned char));

	camera_data_signle = (unsigned char *)malloc(DMAFD_W*DMAFD_H*3*sizeof(unsigned char));
	//first 左后 second 右前
    printf("start get camera data\n");
	get_camera_yuv_data(camera_data_first, camera_data_second);
	//camera_obj.get_camera_data(camera_data_first,camera_data_second);


	yuv420sp_to_rgb24(FMT_NV21, camera_data_first, camera_data_rgb, DMAFD_W, DMAFD_H);    

	for(i = 0;i < CAM_HEIGHT;i++)
	{
		for(j = 0;j < CAM_WIDTH;j++)
		{
			camera_data_signle[(CAM_WIDTH*i+j)*3+0] = camera_data_rgb[(DMAFD_W*i+j)*3+0];
			camera_data_signle[(CAM_WIDTH*i+j)*3+1] = camera_data_rgb[(DMAFD_W*i+j)*3+1];
			camera_data_signle[(CAM_WIDTH*i+j)*3+2] = camera_data_rgb[(DMAFD_W*i+j)*3+2];
		}
	}
	
	sprintf(buft, "/mnt/sdcard/mmcblk1p1/photo_%d.bmp",num);    
	
	ret = RGB2BMP(camera_data_signle, CAM_WIDTH, CAM_HEIGHT, buft);
	if (ret == 0)
	{
		printf("save left camera data.png!\n");
	}
	else
	{
		printf("ret is %d\r\n",num);
		return ;
	}
	num++;
	
	for(i = 0;i < CAM_HEIGHT;i++)
	{
		for(j = 0;j < CAM_WIDTH;j++)
		{
			camera_data_signle[(CAM_WIDTH*i+j)*3+0] = camera_data_rgb[(DMAFD_W*i+CAM_WIDTH+j)*3+0];
			camera_data_signle[(CAM_WIDTH*i+j)*3+1] = camera_data_rgb[(DMAFD_W*i+CAM_WIDTH+j)*3+1];
			camera_data_signle[(CAM_WIDTH*i+j)*3+2] = camera_data_rgb[(DMAFD_W*i+CAM_WIDTH+j)*3+2];
		}
	}
	
	
	sprintf(buft, "/mnt/sdcard/mmcblk1p1/photo_%d.bmp",num);                                                   
	ret = RGB2BMP(camera_data_signle, CAM_WIDTH, CAM_HEIGHT, buft);
	if (ret == 0)
	{
		printf("save near camera data.png!\n");
	}
	else
	{
		printf("ret is %d\r\n",ret);
		return ;
	}
	num++;
	yuv420sp_to_rgb24(FMT_NV21, camera_data_second, camera_data_rgb, DMAFD_W, DMAFD_H);    


	for(i = 0;i < CAM_HEIGHT;i++)
	{
		for(j = 0;j < CAM_WIDTH;j++)
		{
			camera_data_signle[(CAM_WIDTH*i+j)*3+0] = camera_data_rgb[(DMAFD_W*i+j)*3+0];
			camera_data_signle[(CAM_WIDTH*i+j)*3+1] = camera_data_rgb[(DMAFD_W*i+j)*3+1];
			camera_data_signle[(CAM_WIDTH*i+j)*3+2] = camera_data_rgb[(DMAFD_W*i+j)*3+2];
		}
	}
	
	sprintf(buft, "/mnt/sdcard/mmcblk1p1/photo_%d.bmp",num);    
	
	
	ret = RGB2BMP(camera_data_signle, CAM_WIDTH, CAM_HEIGHT, buft);
	if (ret == 0)
	{
		printf("save right camera data.png!\n");
	}
	else
	{
		printf("ret is %d\r\n",num);
		return ;
	}
	num++;
	
	for(i = 0;i < CAM_HEIGHT;i++)
	{
		for(j = 0;j < CAM_WIDTH;j++)
		{
			camera_data_signle[(CAM_WIDTH*i+j)*3+0] = camera_data_rgb[(DMAFD_W*i+CAM_WIDTH+j)*3+0];
			camera_data_signle[(CAM_WIDTH*i+j)*3+1] = camera_data_rgb[(DMAFD_W*i+CAM_WIDTH+j)*3+1];
			camera_data_signle[(CAM_WIDTH*i+j)*3+2] = camera_data_rgb[(DMAFD_W*i+CAM_WIDTH+j)*3+2];
		}
	}
	
	
	sprintf(buft, "/mnt/sdcard/mmcblk1p1/photo_%d.bmp",num);                                                   
	ret = RGB2BMP(camera_data_signle, CAM_WIDTH, CAM_HEIGHT, buft);
	if (ret == 0)
	{
		printf("save front camera data.png!\n");
	}
	else
	{
		printf("ret is %d\r\n",ret);
		return ;
	}
	num++;
	printf("save camera data ok\r\n");
	free(camera_data_rgb);
	free(camera_data_first);
	free(camera_data_second);
	free(camera_data_signle);
	
}

#endif
#else
void save_camera_data(void)
{

}
#endif
