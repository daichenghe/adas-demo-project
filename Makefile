
#target PC or Board Default is PC
ifndef TG
TG=PC
AVMLIB = -L./AVM_lib/ -l$(TG)Davm 
else
TG=T3
AVMLIB = -L./AVM_lib/ -l$(TG)Davm 
endif
#target
BIN = avm_test_$(TG)
MAKE = make
SDKDEBUG_FLAGS=-g -O0

AVM_INC = -I./avm_bsp/ -I./common/ -I./log/ -I./AVM_lib/inc/ -I./AVM_lib/ -I./avm_app/ -I./tool/ -I./video_hal/inc/ -I./video_hal/ -I./avm_line/ -I./apa_line/

ifeq ($(TG),T3)

PWD=../sdk

CC            = $(LICHEE_TOOL_DIR)/bin/arm-linux-gnueabi-gcc
CPP           = $(LICHEE_TOOL_DIR)/bin/arm-linux-gnueabi-g++

LICHEE_DIR=/home/xxy/t3linux/v1.3.5_lichee
LICHEE_TOOL_DIR=$(LICHEE_DIR)/out/sun8iw11p1/linux/common/buildroot/external-toolchain
LICHEE_OUT_HOSTDIR=$(LICHEE_DIR)/out/sun8iw11p1/linux/common/buildroot/host/usr/
FFMPEG_LIB_DIR = /home/xxy/t3linux/v1.3.5_lichee/buildroot/target/user_rootfs_misc/t3_build/ffmpeg_build
H264_LIB_DIR = /home/xxy/t3linux/v1.3.5_lichee/buildroot/target/user_rootfs_misc/t3_build/build_t3/x264_build
JSON_PATH = /home/xxy/t3linux/v1.3.5_lichee/buildroot/target/user_rootfs_misc/t3_build/jsoncpp-src-0.5.0

CEDAR_MACORS = -D_GNU_SOURCE -DOPTION_CC_GNUEABIHF=1 -DOPTION_CC_GNUEABI=2 -DOPTION_CC_UCGNUEABI=3 -DOPTION_CC_LINUX_UCGNUEABI=4 -DOPTION_CC_LINUX_MUSLGNUEABI=5 -DOPTION_CC_LINUX_MUSLGNUEABI64=6 -DOPTION_OS_ANDROID=1 -DOPTION_OS_LINUX=2 -DOPTION_OS_VERSION_ANDROID_4_2=1 -DOPTION_OS_VERSION_ANDROID_4_4=2 -DOPTION_OS_VERSION_ANDROID_5_0=3 -DOPTION_OS_VERSION_ANDROID_6_0=4 -DOPTION_MEMORY_DRIVER_SUNXIMEM=1 -DOPTION_MEMORY_DRIVER_ION=2 -DOPTION_PRODUCT_PAD=1 -DOPTION_PRODUCT_TVBOX=2 -DOPTION_PRODUCT_OTT_CMCC=3 -DOPTION_PRODUCT_IPTV=4 -DOPTION_PRODUCT_DVB=5 -DOPTION_PRODUCT_LOUDSPEAKER=6 -DOPTION_CHIP_R8=1 -DOPTION_CHIP_R16=2 -DOPTION_CHIP_C500=3 -DOPTION_CHIP_R58=4 -DOPTION_CHIP_R18=5 -DOPTION_CHIP_T2=6 -DOPTION_CHIP_T3=7 -DOPTION_DRAM_INTERFACE_DDR1_16BITS=1 -DOPTION_DRAM_INTERFACE_DDR1_32BITS=2 -DOPTION_DRAM_INTERFACE_DDR2_16BITS=3 -DOPTION_DRAM_INTERFACE_DDR2_32BITS=4 -DOPTION_DRAM_INTERFACE_DDR3_16BITS=5 -DOPTION_DRAM_INTERFACE_DDR3_32BITS=6 -DOPTION_DRAM_INTERFACE_DDR3_64BITS=7 -DOPTION_CMCC_NO=0 -DOPTION_CMCC_YES=1 -DOPTION_DTV_NO=0 -DOPTION_DTV_YES=1 -DOPTION_ALI_YUNOS_NO=0 -DOPTION_ALI_YUNOS_YES=1 -DOPTION_IS_CAMERA_DECODER_NO=0 -DOPTION_IS_CAMERA_DECODER_YES=1 -DOPTION_VE_IPC_DISABLE=1 -DOPTION_VE_IPC_ENABLE=2 -DOPTION_NO_DEINTERLACE=0 -DOPTION_HW_DEINTERLACE=1 -DOPTION_SW_DEINTERLACE=2 -DLINUX_VERSION_3_4=1 -DLINUX_VERSION_3_10=2 -DDROP_DELAY_FRAME_NONE=0 -DDROP_DELAY_FRAME_720P=1 -DDROP_DELAY_FRAME_4K=2 -DZEROCOPY_PIXEL_FORMAT_NONE=0 -DZEROCOPY_PIXEL_FORMAT_YUV_MB32_420=1 -DZEROCOPY_PIXEL_FORMAT_YV12=2 -DZEROCOPY_PIXEL_FORMAT_NV21=3 -DGPU_Y16_C16_ALIGN=1 -DGPU_Y32_C16_ALIGN=2 -DGPU_Y16_C8_ALIGN=3 -DZEROCOPY_HAL_PIXEL_FORMAT_AW_YUV_PLANNER420=1 -DZEROCOPY_HAL_PIXEL_FORMAT_AW_YVU_PLANNER420=2 -DDEINTERLACE_FORMAT_MB32_12=1 -DDEINTERLACE_FORMAT_NV=2 -DDEINTERLACE_FORMAT_NV21=3 -DDEINTERLACE_FORMAT_NV12=4 -DOUTPUT_PIXEL_FORMAT_NV21=1 -DOUTPUT_PIXEL_FORMAT_YV12=2 -DOUTPUT_PIXEL_FORMAT_MB32=3 -DGPU_ALIGN_STRIDE_16=1 -DGPU_ALIGN_STRIDE_32=2 -DOPTION_HAVE_ZLIB=1 -DOPTION_NO_ZLIB=2 -DOPTION_HAVE_SSL=1 -DOPTION_NO_SSL=2 -DOPTION_HAVE_LIVE555=1 -DOPTION_NO_LIVE555=2 -DCONFIG_CC=4 -DCONFIG_OS=2 -DCONFIG_CHIP=7 -DCONFIG_ALI_YUNOS=0 -DCONFIG_OS_VERSION=2 -DCONFIG_TARGET_PRODUCT= -DCONFIG_PRODUCT=2 -DCONFIG_HAVE_ZLIB=1 -DCONFIG_HAVE_SSL=1 -DCONFIG_HAVE_LIVE555=2 -DCONFIG_MEMORY_DRIVER=2 -DCONFIG_DRAM_INTERFACE=6 -DCONFIG_VE_IPC=1 -DCONFIG_CMCC=0 -DCONFIG_DTV=0 -DCONFIG_IS_CAMERA_DECODER=0 -DCONFIG_DEINTERLACE=0 -DGPU_TYPE_MALI=1 -DCONFIG_ARM_VERSION= -DENABLE_SUBTITLE_DISPLAY_IN_CEDARX=0 -DDTV=no -DLINUX_VERSION=2 -DUSE_NEW_BDMV_STREAM=0 -DPLAYREADY_DRM_INVOKE=0 -DH265_4K_CHECK_SCALE_DOWN=1 -DNON_H265_4K_NOT_SCALE_DOWN=0 -DSUPPORT_H265=0 -DANTUTU_NOT_SUPPORT=1 -DENABLE_MEDIA_BOOST=1 -DDROP_DELAY_FRAME=2 -DROTATE_PIC_HW=0 -DVE_PHY_OFFSET=0x40000000 -DZEROCOPY_PIXEL_FORMAT=3 -DGPU_Y_C_ALIGN=1 -DSEND_3_BLACK_FRAME_TO_GPU=0 -DZEROCOPY_DYNAMIC_CHECK=1 -DGRALLOC_PRIV=0 -DVIDEO_DIRECT_ACCESS_DE=0 -DKEY_PARAMETER_GET=0 -DDISPLAY_CMD_SETVIDEOSIZE_POSITION=0 -DDEINTERLACE_IOWR=0 -DDEINTERLACE_FORMAT=4 -DNEW_DISPLAY_DOUBLE_STREAM_NEED_NV21=1 -DOUTPUT_PIXEL_FORMAT=1 -DNOT_DROP_FRAME=1 -DSOUND_DEVICE_SET_RAW_FLAG=0 -DNATIVE_WIN_DISPLAY_CMD_GETDISPFPS=0 -DIMG_NV21_4K_ALIGN=0 -DDEINTERLACE_ADDR_64=0 -DCONFIG_LOG_LEVEL=0

CEDAR_MACORS +=  -I$(PWD)/ -I$(PWD)/cedarx/include -I$(PWD)/cedarx/include/libcedarc/include -I$(PWD)/cedarx/include/DEMUX/MUXER/include/include/ \
                  -I$(PWD)/cedarx/include/DEMUX/BASE/include/include -I$(PWD)/cedarx/include/DEMUX/PARSER/include/include/ -I$(PWD)/cedarx/include/DEMUX/STREAM/include/include -I$(PWD)/cedarx/include/PLAYER/include/ \
				  	 -I$(PWD)/cedarx/include/libcore/muxer/include \
				  -I$(PWD)/cedarx/include/libcore/base/include/ \
				  -I$(PWD)/cedarx/include/libcore/parser/include \
				  -I$(PWD)/cedarx/include/libcore/stream/include \
				  -I$(PWD)/cedarx/include/libcore/playback/include/ \
				  -I$(PWD)/cedarx/include/libcore/common/iniparser/ \

DEFINES       = -DHAVE_PTHREADS -DSUPPORT_NEW_DRIVER -DHAVE_SYS_UIO_H -DANDROID_SMP=1 -D__ARM_HAVE_DMB -D__ARM_HAVE_LDREX_STREX -D__ARM_HAVE_LDREXD \
				-D__ARM_HAVE_VFP -D__ARM_HAVE_NEON -DWATERMARK_ENABLE -DHAVE_PRCTL -DSUPPORT_NEW_DRIVER -DWATERMARK -DADAS_ENABLE -DDMAFD_TEST -DCDX_V27 -DGL_GLEXT_PROTOTYPES -DT3BOARD

LDFLAGS := -rdynamic -L$(LICHEE_OUT_HOSTDIR)/arm-unknown-linux-gnueabi/sysroot/lib -L$(LICHEE_OUT_HOSTDIR)/arm-unknown-linux-gnueabi/sysroot/usr/lib -L$(LICHEE_TOOL_DIR)/arm-linux-gnueabi/libc/usr/lib/arm-linux-gnueabi -lpthread  -lasound -lpng

LDLIBS += -L$(PWD)/cedarx/lib/ -Wl,--no-undefined  -lcdx_playback -lcdx_common -laencoder -lawrecorder -lxplayer   -lcdx_muxer -lcdx_parser -lcdc_vdecoder -lcdx_stream   -lcdc_vencoder -lcdc_ve  -lcdc_sdecoder -lcdc_base -lcdc_memory -ladecoder -laencoder -lxmetadataretriever -lcdx_base -lrt -lpthread -lz -lcrypto -lssl $(LIBS) -L.  $(LDFLAGS) -ltinyalsa\
			-L$(FFMPEG_LIB_DIR)/lib -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswscale  -lswresample -lpostproc \
			-L$(H264_LIB_DIR)/lib -lx264 \
			-L./video_hal/ -lcameral_hal\
			-L./video_hal/ -lrecorder_hal  \
			-L./video_hal/ -ldecoder_hal
LIBS = -L$(PWD)/libs -lsdk_disp  -lsdk_player -lsdk_tr -L$(LICHEE_DIR)/tools/pack/chips/sun8iw11p1/hal/gpu/fbdev/lib/ -lGLESv2 -L./AVM_lib/ -lT3Davm  -L$(JSON_PATH) -ljson
LIBS += $(AVMLIB)
DEFINC = -I$(PWD)/include -I$(PWD)/include/disp2 -I$(PWD)/include/utils -I$(PWD)/media -I$(LICHEE_TOOL_DIR)/arm-linux-gnueabi/include/c++/4.6.3 -I$(LICHEE_TOOL_DIR)/arm-linux-gnueabi/include/c++/4.6.3/arm-linux-gnueabi -I$(LICHEE_TOOL_DIR)/arm-linux-gnueabi/include/c++/4.6.3/backward -I$(LICHEE_TOOL_DIR)/lib/gcc/arm-linux-gnueabi/4.6.3/include -I$(LICHEE_TOOL_DIR)/lib/gcc/arm-linux-gnueabi/4.6.3/include-fixed -I$(LICHEE_TOOL_DIR)/arm-linux-gnueabi/include -I$(LICHEE_OUT_HOSTDIR)/include/arm-linux-gnueabi -I$(LICHEE_OUT_HOSTDIR)/include -I$(PWD)/camera -I$(LICHEE_TOOL_DIR)/arm-linux-gnueabi/libc/usr/include/ \
-I$(LICHEE_TOOL_DIR)/arm-linux-gnueabi/libc/usr/include/arm-linux-gnueabi/  -I$(PWD)/storage/ -I$(PWD)/sound/  -I$(PWD)/misc/ -I$(PWD)/camera/allwinnertech/include -I$(PWD)/camera/allwinnertech/water_mark/ -I$(PWD)/camera/adas/  -I$(LICHEE_DIR)/tools/pack/chips/sun8iw11p1/hal/gpu/fbdev/include -I$(LICHEE_DIR)/out/sun8iw11p1/linux/common/buildroot/host/usr/arm-unknown-linux-gnueabi/sysroot/usr/include/drm/

DEFINC += $(AVM_INC)

CFLAGS= $(SDKDEBUG_FLAGS) -rdynamic -std=c99 -pipe -mfpu=neon -marm -mcpu=cortex-a7 --sysroot=$(PWD)/$(LICHEE_OUT_HOSTDIR)/arm-unknown-linux-gnueabi/sysroot  -Wall -W -D_REENTRANT  -fPIC $(DEFINES) $(INCS)
CPPFLAGS= $(SDKDEBUG_FLAGS) -rdynamic -pipe -marm -mfpu=neon -mcpu=cortex-a7 --sysroot=$(PWD)/$(LICHEE_OUT_HOSTDIR)/arm-unknown-linux-gnueabi/sysroot  -Wall -W -D_REENTRANT  -fPIC $(DEFINES) $(INCS)


else

CC            = gcc
CPP           = g++
DEFINES += -DPC_DEBUG_BMP -DEGL_USE_X11
DEFINC += $(AVM_INC)
LDFLAGS += -lstdc++ -lz -lm  -lGLESv2 -lEGL -lX11  -lpthread -lpng
LDLIBS += $(LDFLAGS)
LIBS += $(AVMLIB)
CFLAGS= -pipe 
CPPFLAGS = -pipe 
endif

SUBDIRS = $(shell find . -maxdepth 3 -type d)

CSRCS =  $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.c)) 
CPPSRCS = $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.cpp))
 

COBJS := $(CSRCS:.c=.o)
CPPOBJS := $(CPPSRCS:.cpp=.o)


MAKEDEPEND = $(CC) -MM -MT



CDEF = $(CSRCS:.c=.d)
CPPDEF = $(CPPSRCS:.cpp=.d)

all:$(BIN) 

$(CDEF) : %.d : %.c
	$(MAKEDEPEND) $(<:.c=.o) $< $(DEFINC) $(DEFINES) $(CEDAR_MACORS) > $@
$(CPPDEF) : %.d : %.cpp
	$(MAKEDEPEND) $(<:.cpp=.o) $< $(DEFINC) $(DEFINES)  $(CEDAR_MACORS) > $@

depend:
	rm $(CDEF)
	rm $(CPPDEF)
	$(MAKE) $(CDEF)
	$(MAKE) $(CPPDEF)

$(COBJS) : %.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(DEFINES) $(CEDAR_MACORS) $(DEFINC) -DSDKLIB_TEST
	@echo ""
	@echo "->>>>>>>> compile " $@
$(CPPOBJS) : %.o: %.cpp
	$(CPP) -c $< -o $@ $(CPPFLAGS) $(DEFINES) $(CEDAR_MACORS) $(DEFINC) -DSDKLIB_TEST
	@echo ""
	@echo "->>>>>>>> compile " $@

$(BIN) : $(COBJS) $(CPPOBJS) 
	$(CPP) -o $(BIN) $(COBJS) $(CPPOBJS) $(LIBS) $(LDLIBS) $(CEDAR_MACORS)
		
include $(CDEF)
include $(CPPDEF)
.PHONY:clean cleanall
clean:            
	rm $(BIN) $(COBJS) $(CPPOBJS) 
cleanall:
	rm $(CDEF) $(CPPDEF)
	rm $(BIN) $(COBJS) $(CPPOBJS) 
