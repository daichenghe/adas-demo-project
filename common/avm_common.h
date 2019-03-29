/*******************************************************************************
* File Name          : avm_common.h
* Author             : Jerry
* Revision           : 2.1
* Date               : 15/07/2016
* Description        :
*
* HISTORY***********************************************************************
* 15/07/2016  | sDas Basic 2.0                                             | Jerry
*
*******************************************************************************/
#ifndef __AVM_COMMON_H__
#define __AVM_COMMON_H__

#define   PI                    (3.1415926f)        /* 圆周�?*/
#define   DEGREES_TO_RADIANS(a) (0.0174533f * (a))  /* 角度转弧�?*/


#if    defined(DEBUG)
  #define	GL_CHECK_ERRORS		do{GLenum retgl = glGetError();if(retgl != GL_NO_ERROR){fprintf(stderr,"File:<%s> Fun:[%s] Line:%d\nglGetError():%d\n",__FILE__, __FUNCTION__, __LINE__,retgl);}}while(0)
  #define sys_inf(fmt,args...)  fprintf(stderr,fmt, ##args)
  #define sys_err(fmt,args...)  {fprintf(stderr,"\nError:\nFile:<%s> Fun:[%s] Line:%d\n "fmt, __FILE__, __FUNCTION__, __LINE__, ##args);}//perror("");

 #else

  #define   sys_inf(fmt,args...) /*do nothing */
  #define	GL_CHECK_ERRORS   if(glGetError() != GL_NO_ERROR){return AVM_ERR_GL;}
  #define   sys_err(fmt,args...) /*do nothing */
#endif

#define assert(condition)  \
{\
	if(condition)\
	NULL;\
		else\
{\
	printf("\n[EXAM]Assert failed: %s,line %u\n",__FILE__,__LINE__);\
	while (1);\
}\
}

#endif
