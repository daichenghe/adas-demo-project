#ifdef T3BOARD

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/time.h>
#include <type_camera.h>
#include "ion.h"
#include <sc_interface.h>

#ifndef ION_HAL_H
#define ION_HAL_H

typedef struct ion_element_t_{
    int     ion_fd;
}ion_element_t;

typedef struct ion_buf_t_{            //内存管理
    int     size;
    int     dma_fd;
    int     ion_phy_addr;
    void   *ion_vir_addr;
    ion_element_t *ion_element;
}ion_buf_t;

#ifdef __cplusplus
extern "C" {
#endif
//export
int ion_open(void);
int ion_alloc(int size, ion_buf_t *ion_buf);

#ifdef __cplusplus
}
#endif

#endif

#endif
