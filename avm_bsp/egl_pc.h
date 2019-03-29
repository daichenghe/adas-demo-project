#ifndef __EGL_PC_H__
#define __EGL_PC_H__

#ifndef T3BOARD
extern void fps_calc(void);
extern void egl_swap(void);
	extern void bsp_pc_init(void);
	extern int texture_update(void);
	extern void avm_User_Stub_Textures(GLint TexturesID, GLint *OutTexturesID);
#endif
#endif