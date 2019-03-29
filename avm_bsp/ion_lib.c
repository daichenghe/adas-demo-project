/*************************************************************************
  > File Name: ion_lib.c
  > Author: zhaodq
  > Created Time: 2017-01-12
 ************************************************************************/
#ifdef T3BOARD

#include <string.h>
#include "ion_lib.h"
#include "log.h"

static ion_element_t s_ion_element;

int ion_open(void)
{
    int ion_fd=-1;
    if (ion_fd == -1)
    {
        ion_fd = open("/dev/ion", O_RDWR);
    }
    if (ion_fd < 0)
    {
        printf("open /dev/ion err.\n");
        return -1;
    }
    s_ion_element.ion_fd = ion_fd;
    return 0;
}

int ion_free(ion_buf_t *ion_buf)
{
    int size = 0;
    int addr_phy = 0;
    int addr_vir = 0;
    ion_allocation_data_t alloc_data;
    int ret = 0;
    addr_vir = ion_buf->ion_vir_addr;
    size = ion_buf->size;
    munmap(addr_vir, size);
    //free
    alloc_data.len = size;
    alloc_data.align = 4096;
    alloc_data.heap_id_mask = ION_HEAP_CARVEOUT_MASK;
    alloc_data.flags = 0;
    alloc_data.handle = 0;
    ret = ioctl(s_ion_element.ion_fd, ION_IOC_FREE, &alloc_data);
    if(ret)
    {
        printf("ION_IOC_FREE err\n");
        return -1;
    }
    ion_buf->dma_fd = 0;
    ion_buf->ion_element = 0;
    ion_buf->ion_phy_addr = 0;
    ion_buf->ion_vir_addr = 0;
    ion_buf->size = 0;
    return 0;
}



void ion_close(void)
{
    close(s_ion_element.ion_fd);
    s_ion_element.ion_fd = -1;
    return 0;
}


int ion_alloc(int size, ion_buf_t *ion_buf)
{
//    int rest_size = 0;
    int addr_phy = 0;
    int addr_vir = 0;
    ion_allocation_data_t alloc_data;
//    ion_handle_data_t handle_data;
    ion_custom_data_t custom_data;
    ion_fd_data_t fd_data;
    sunxi_phys_data phys_data;
    int ret = 0;

    if(size<=0)
    {
         printf("pamerater err\n");
        return -1;
    }
    //alloc
    alloc_data.len = size;
    alloc_data.align = 4096;
    alloc_data.heap_id_mask = ION_HEAP_CARVEOUT_MASK;
    alloc_data.flags = 0;
    alloc_data.handle = 0;
    ret = ioctl(s_ion_element.ion_fd, ION_IOC_ALLOC, &alloc_data);
    if(ret)
    {
         printf("ION_IOC_ALLOC err\n");
        return -1;
    }

    //get phy addr
    phys_data.handle = alloc_data.handle;
    custom_data.cmd = ION_IOC_SUNXI_PHYS_ADDR;
    custom_data.arg = (unsigned long)&phys_data;
    ret = ioctl(s_ion_element.ion_fd, ION_IOC_CUSTOM, &custom_data);
    if(ret)
    {
        printf("ION_IOC_CUSTOM err\n");
        return -1;
    }
    addr_phy = phys_data.phys_addr;

    //get dma buffer fd
    fd_data.handle = alloc_data.handle;
    ret = ioctl(s_ion_element.ion_fd, ION_IOC_MAP, &fd_data);
    if(ret)
    {
         printf("ION_IOC_MAP err\n");
        return -1;
    }

    addr_vir = (int)mmap(NULL, alloc_data.len, PROT_READ | PROT_WRITE, MAP_SHARED, fd_data.fd, 0);
    if((int)MAP_FAILED == addr_vir)
    {
         printf("mmap err\n");
        return -1;
    }

    ion_buf->dma_fd = fd_data.fd;
    ion_buf->ion_element = &s_ion_element;
    ion_buf->ion_phy_addr = addr_phy;
    ion_buf->ion_vir_addr = addr_vir;
    ion_buf->size = size;

    return 0;

}
#endif
