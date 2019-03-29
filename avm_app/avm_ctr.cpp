/*******************************************************************************
* File Name          : avm_ctr.c
* Author             : Daich
* Revision           : 2.1
* Date               : 03/07/2018
* Description        : Mode Judgment and switch
*
* HISTORY***********************************************************************
* 03/07/2018  | sDas Basic 2.0                       | Daich
*
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avm_config.h"
#include "log.h"
#include "app.h"
#include "avmlib_sdk.h"
#include "avm_ctr.h"
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tool.h"
#include <sys/ioctl.h>



#ifdef T3BOARD

#include "app.h"
#include "DecoderHal.h"//player_lib/player/
#include "egl_dvr.h"
#endif

//keyboard
#if !defined(T3BOARD)
#define  AVM_KEY_ESC    (0x1B) 
#define  AVM_KEY_F2  	(0x1B4F51)
#define  AVM_KEY_F3  	(0x1B4F52)
#define  AVM_KEY_F4  	(0x1B4F53)
#define  AVM_KEY_F5  	(0x5B31357F)
#define  AVM_KEY_F6  	(0x5B31377F)
#define  AVM_KEY_F7  	(0x5B31387F)
#define  AVM_KEY_F8  	(0x5B31397F)
#define  AVM_KEY_F9  	(0x5B32307F)
#define  AVM_KEY_F10  	(0x5B32317F)
#else
#define  AVM_KEY_ESC    (0x1B) 
#define  AVM_KEY_F2  	(0x1B4F51)
#define  AVM_KEY_F3  	(0x1B4F52)
#define  AVM_KEY_F4  	(0x1B4F53)
#define  AVM_KEY_F5  	(0x5b31357e)
#define  AVM_KEY_F6  	(0x5B31377E)
#define  AVM_KEY_F7  	(0x5B31387E)
#define  AVM_KEY_F8  	(0x5B31397E)
#define  AVM_KEY_F9  	(0x5B32307E)
#define  AVM_KEY_F10  	(0x5B32317E)
#endif

#define AVM_KEY_1      (0x31)
#define AVM_KEY_2      (0x32)
#define AVM_KEY_3      (0x33)
#define AVM_KEY_4      (0x34)
#define AVM_KEY_5      (0x35)
#define AVM_KEY_6      (0x36)
#define AVM_KEY_7      (0x37)
#define AVM_KEY_8      (0x38)
#define AVM_KEY_9      (0x39)
#define AVM_KEY_0      (0x30)

#define AVM_KEY_M	   (0X6D)
#define AVM_KEY_C	   (0X63)

#define AVM_INPUT_INVALID	(0)
#define AVM_INPUT_VALID		(1)

static AVM_ERR avm_key_input_process(char * input_buf,int input_len);
static void avm_uart_input_process(char *input_buf, int intput_len);
static void uart_open(int *fd);
static void uart_config(int fd);

view_keep_t g_view_keep;


#ifdef T3BOARD
void *player_thread(void * argv)
{
    DecoderHal *player_args = (DecoderHal *)argv;
    while(1)
    {
        {
            player_args->openFile("/mnt/sdcard/mmcblk1p1/test.mp4");
            DbgPrintf("openFile\n");
            sleep(5);
            player_args->pausePlay();
            DbgPrintf("pausePlay\n");
            sleep(3);
            player_args->startPlay();
            DbgPrintf("startPlay\n");
            sleep(3);
            player_args->seekTo(70);
            DbgPrintf("seekTo\n");
            sleep(5);
            player_args->stopPlay();
            DbgPrintf("stopPlay\n");
            sleep(1);
        }
    }
}

void dvr_test(void)
{
	 int result;
    pthread_t  play_thread;
    pthread_t  dbus_thread;
	DecoderHal *player;
	player = get_player();

    result = pthread_create(&play_thread,NULL, (void* (*)(void*))player_thread,static_cast<void*>(player));

    pthread_detach(play_thread);


}
#endif

void *avm_key_input_monitor_thread(void *arg)
{
	char input_buf[20];
	int input_index = 0;

	g_view_keep.mode_last = g_view_keep.mode;
	g_view_keep.mode = MODE_AVM_2D;
	g_view_keep.sub_mode_last = g_view_keep.sub_mode;
	g_view_keep.sub_mode = MODE_2D_FRONT;

	while(g_sysquit == AVM_STATE_ACTIVE)
	{
		input_index = 0;
		while(g_sysquit == AVM_STATE_ACTIVE)
		{
			input_buf[input_index] = getchar();
			input_index++;
			if(input_index >= 20)
			{
				input_index = 0;
			}
			else if(input_buf[input_index - 1] == '\n')
			{
				break;
			}
			
		}
		avm_key_input_process(input_buf,input_index-1);
		
	}
}
static AVM_ERR avm_key_input_process(char * input_buf,int input_len)
{
	int key_value = 0;
	int i;
	int input_new_flag = AVM_INPUT_INVALID;
	view_keep_t view_keep_tmp;
	
	
	for(i = 0; i < input_len ; i++)
	{
		key_value |=(input_buf[i] << (((input_len - i) -1) * 8));
	}
	memcpy(&view_keep_tmp,&g_view_keep,sizeof(view_keep_t));
	DbgPrintf("key_value:%x\n",key_value);
	
#ifdef T3BOARD

	dec_status_ player_state = get_player_state();
	DecoderHal *player = get_player();
	
	if ((player_state != DEC_STOP)&&(key_value != AVM_KEY_F7))
	{
	//	set_player_state(DEC_STOP);
		player->stopPlay();
	}
	
#endif	
	switch(key_value)
	{
		case AVM_KEY_F2:
			if(view_keep_tmp.mode != MODE_AVM_2D)
			{
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_AVM_2D;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_2D_FRONT;
				
				view_keep_tmp.fly_flag = AVM_FLY_OFF;
				input_new_flag = AVM_INPUT_VALID;
			}
			break;
		case AVM_KEY_F3:
			if(view_keep_tmp.mode != MODE_AVM_3D)
			{
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_AVM_3D;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_3D_ROTATE;//MODE_3D_BACK;
				
				view_keep_tmp.fly_flag = AVM_FLY_OFF;
				input_new_flag = AVM_INPUT_VALID;
			}
			break;
		case AVM_KEY_F5:
			if(view_keep_tmp.mode != MODE_ORI)
			{
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_ORI;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_ORI_SPLIT;
				input_new_flag = AVM_INPUT_VALID;

			}
			break;
		case AVM_KEY_F6:
			if(view_keep_tmp.mode != MODE_SVFS)
			{
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_SVFS;
				input_new_flag = AVM_INPUT_VALID;

			}
			break;
		case AVM_KEY_F8:
			if(view_keep_tmp.mode != MODE_PER)
			{
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_PER;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_PER_FRONT;
				input_new_flag = AVM_INPUT_VALID;
			}
			break;
		case AVM_KEY_F7:
			
#ifdef T3BOARD

			if(view_keep_tmp.mode != MODE_AVM_DVR)
			{
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_AVM_DVR;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_2D_FRONT;
				input_new_flag = AVM_INPUT_VALID;
			}
						
			switch (player_state)
			{
				case DEC_PAUSE:
				//	set_player_state(DEC_PLAY);
					 player->startPlay();
					break;
				case DEC_PLAY:
				//	set_player_state(DEC_PAUSE);
					 player->pausePlay();
					break;
				case DEC_STOP:
				//	set_player_state(DEC_PLAY);
					 player->openFile("/mnt/sdcard/mmcblk1p1/test.mp4");
					break;
								
			}
#endif
			break;
		case AVM_KEY_F10:
			if(view_keep_tmp.mode != MODE_BACKUP)
			{
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_BACKUP;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_BACKUP_FRONT;
				input_new_flag = AVM_INPUT_VALID;
			}
			break;
		case AVM_KEY_F4:
			if(view_keep_tmp.mode != MODE_AVM_LDWS)
			{
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_AVM_LDWS;
				input_new_flag = AVM_INPUT_VALID;
			}
			break;
		case AVM_KEY_F9:
			if(view_keep_tmp.mode != MODE_AVM_BSD)
			{
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_AVM_BSD;
				input_new_flag = AVM_INPUT_VALID;
			}

			break;
		case AVM_KEY_1:
			switch(view_keep_tmp.mode)
			{
				case MODE_AVM_3D:
					if(view_keep_tmp.sub_mode != MODE_3D_LEFTUP)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_3D_LEFTUP;
						view_keep_tmp.fly_flag = AVM_FLY_ON;
						input_new_flag = AVM_INPUT_VALID;

					}
					break;
				case MODE_ORI:
					if(view_keep_tmp.sub_mode != MODE_ORI_FRONT)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_ORI_FRONT;
						input_new_flag = AVM_INPUT_VALID;
					}

					break;
				case MODE_PER:
					if(view_keep_tmp.sub_mode != MODE_PER_FRONT)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_PER_FRONT;
						input_new_flag = AVM_INPUT_VALID;
					}

					break;
				case MODE_AVM_2D:
					if(view_keep_tmp.sub_mode != MODE_2D_FRONT)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_2D_FRONT;
						input_new_flag = AVM_INPUT_VALID;
					}

					break;
				case MODE_BACKUP:
					if(view_keep_tmp.sub_mode != MODE_BACKUP_FRONT)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_BACKUP_FRONT;
						input_new_flag = AVM_INPUT_VALID;
					}

					break;
			}
			break;

		case AVM_KEY_2:
			switch(view_keep_tmp.mode)
			{
				case MODE_AVM_3D:
					if(view_keep_tmp.sub_mode != MODE_3D_BACK)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_3D_BACK;
						view_keep_tmp.fly_flag = AVM_FLY_ON;
						input_new_flag = AVM_INPUT_VALID;
					}
					break;
				case MODE_ORI:
					if(view_keep_tmp.sub_mode != MODE_ORI_RIGHT)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_ORI_RIGHT;
						input_new_flag = AVM_INPUT_VALID;
					}

					break;
				case MODE_AVM_2D:
					if(view_keep_tmp.sub_mode != MODE_2D_RIGHT)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_2D_RIGHT;
						input_new_flag = AVM_INPUT_VALID;
					}

					break;
				case MODE_BACKUP:
					if(view_keep_tmp.sub_mode != MODE_BACKUP_RIGHT)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_BACKUP_RIGHT;
						input_new_flag = AVM_INPUT_VALID;
					}

					break;
			}
			break;
		case AVM_KEY_3:
			switch(view_keep_tmp.mode)
			{
				case MODE_AVM_3D:
					if(view_keep_tmp.sub_mode != MODE_3D_RIGHTUP)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_3D_RIGHTUP;
						view_keep_tmp.fly_flag = AVM_FLY_ON;
						input_new_flag = AVM_INPUT_VALID;

					}
					break;
				case MODE_ORI:
					if(view_keep_tmp.sub_mode != MODE_ORI_BACK)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_ORI_BACK;
						input_new_flag = AVM_INPUT_VALID;

					}

					break;
				case MODE_PER:
					if(view_keep_tmp.sub_mode != MODE_PER_BACK)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_PER_BACK;
						input_new_flag = AVM_INPUT_VALID;

					}

					break;
				case MODE_AVM_2D:
					if(view_keep_tmp.sub_mode != MODE_2D_BACK)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_2D_BACK;
						input_new_flag = AVM_INPUT_VALID;

					}

					break;
				case MODE_BACKUP:
					if(view_keep_tmp.sub_mode != MODE_BACKUP_BACK)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_BACKUP_BACK;
						input_new_flag = AVM_INPUT_VALID;
					}

					break;
			}
			break;
		case AVM_KEY_4:
			switch(view_keep_tmp.mode)
			{
				case MODE_AVM_3D:
					if(view_keep_tmp.sub_mode != MODE_3D_RIGHT)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_3D_RIGHT;
						view_keep_tmp.fly_flag = AVM_FLY_ON;
						input_new_flag = AVM_INPUT_VALID;

					}
					break;
				case MODE_ORI:
					if(view_keep_tmp.sub_mode != MODE_ORI_LEFT)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_ORI_LEFT;
						input_new_flag = AVM_INPUT_VALID;
					}

					break;
				case MODE_AVM_2D:
					if(view_keep_tmp.sub_mode != MODE_2D_LEFT)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_2D_LEFT;
						input_new_flag = AVM_INPUT_VALID;
					}

					break;
				case MODE_BACKUP:
					if(view_keep_tmp.sub_mode != MODE_BACKUP_LEFT)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_BACKUP_LEFT;
						input_new_flag = AVM_INPUT_VALID;
					}

					break;
			}
			break;
		case AVM_KEY_5:
			switch(view_keep_tmp.mode)
			{
				case MODE_AVM_3D:
					if(view_keep_tmp.sub_mode != MODE_3D_FRONTRUP)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_3D_FRONTRUP;
						view_keep_tmp.fly_flag = AVM_FLY_ON;
						input_new_flag = AVM_INPUT_VALID;
					}
					break;
				case MODE_ORI:
					if(view_keep_tmp.sub_mode != MODE_ORI_SPLIT)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_ORI_SPLIT;
						input_new_flag = AVM_INPUT_VALID;
						save_camera_data();
					}
					break;
			}
			break;
		case AVM_KEY_6:
			switch(view_keep_tmp.mode)
			{
				case MODE_AVM_3D:
					if(view_keep_tmp.sub_mode != MODE_3D_FRONT)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_3D_FRONT;
						view_keep_tmp.fly_flag = AVM_FLY_ON;
						input_new_flag = AVM_INPUT_VALID;
					}
					break;
			}
			break;
		case AVM_KEY_7:
			switch(view_keep_tmp.mode)
			{
				case MODE_AVM_3D:
					if(view_keep_tmp.sub_mode != MODE_3D_FRONTLUP)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_3D_FRONTLUP;
						view_keep_tmp.fly_flag = AVM_FLY_ON;
						input_new_flag = AVM_INPUT_VALID;
					}
					break;
			}
			break;
		case AVM_KEY_8:
			switch(view_keep_tmp.mode)
			{
				case MODE_AVM_3D:
					if(view_keep_tmp.sub_mode != MODE_3D_LEFT)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_3D_LEFT;
						view_keep_tmp.fly_flag = AVM_FLY_ON;
						input_new_flag = AVM_INPUT_VALID;
					}
					break;
			}
			break;
		case AVM_KEY_M:
			switch(view_keep_tmp.mode)
			{
				case MODE_ORI:
					if(view_keep_tmp.sub_mode != MODE_ORI_SAVE_IMAGE)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_ORI_SAVE_IMAGE;
						input_new_flag = AVM_INPUT_VALID;
					}
					break;
			}
			break;
		case AVM_KEY_C:
			switch(view_keep_tmp.mode)
			{
				case MODE_ORI:
					if(view_keep_tmp.sub_mode != MODE_ORI_CALI)
					{
						view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
						view_keep_tmp.sub_mode = MODE_ORI_CALI;
						input_new_flag = AVM_INPUT_VALID;
					}
					break;
			}
			break;
	}
	if(input_new_flag == AVM_INPUT_VALID)
	{
		memcpy(&g_view_keep,&view_keep_tmp,sizeof(view_keep_t));
		g_view_keep.winclear = AVM_SCREEN_CLEAN_ON;
	}

}
#if 1
void *avm_uart_input_monitor_thread(void *arg)
{
	char input_buf[50];
	int input_len = 0;
	int fd;
	g_view_keep.mode_last = g_view_keep.mode;
	g_view_keep.mode = MODE_AVM_2D;
	g_view_keep.sub_mode_last = g_view_keep.sub_mode;
	g_view_keep.sub_mode = MODE_2D_FRONT;
	
	uart_open(&fd);
	uart_config(fd);
	
	while(g_sysquit == AVM_STATE_ACTIVE && fd > 0)
	{
		input_len = read(fd,input_buf,50);
		if(input_len != 0)
		{
			avm_uart_input_process(input_buf,input_len);
		}
		usleep(10);
	}

}
static void uart_open(int *fd)
{
	*fd = open("/dev/ttymxc1",O_RDWR|O_NOCTTY);
	if(*fd <=0)
	{
		DbgPrintf("open uart err\n");
	}

	fcntl(*fd,F_SETFL,0);
}

static void uart_config(int fd)
{
	struct termios options; 
	int line_val = 0x8000;
	fcntl(fd, F_SETFL, 0);	
	tcgetattr(fd, &options);
					
	ioctl(fd, TIOCMSET, &line_val);
	ioctl(fd, TIOCMBIC, &line_val);

							
	cfsetispeed(&options, B19200);
	cfsetospeed(&options, B19200);

	options.c_cflag &= ~CSTOPB;	// stop 1
	options.c_cflag &= ~CSIZE;	
	options.c_cflag &= ~PARENB;	//check enable
	options.c_cflag &= ~PARODD;	// check odd
	options.c_cflag |= CS8;	// 8 bit
	options.c_cflag &= ~CRTSCTS;// flow ctr
												
	options.c_lflag &= ~(ICANON | IEXTEN | ISIG | ECHO);	
																
	options.c_oflag &= ~OPOST;	

	options.c_iflag &= ~(ICRNL | INPCK | ISTRIP | IXON | BRKINT );	
	//	options.c_cc[VMIN] = 1;	
	//	options.c_cc[VTIME] = 0;	
	options.c_cflag |= (CLOCAL | CREAD);
	if (tcsetattr(fd,   TCSANOW,   &options)<0) 
	{
		return;
	 }
	DbgPrintf("set parity finish\n");
}
static void avm_uart_input_process(char *input_buf, int intput_len)
{
	int input_new_flag = AVM_INPUT_INVALID;
	view_keep_t view_keep_tmp;
	
	
	memcpy(&view_keep_tmp,&g_view_keep,sizeof(view_keep_t));

	if(intput_len == 8)
	{
		switch((unsigned char )input_buf[7])
		{
			case 0xE2:
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_AVM_2D;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_2D_FRONT;
				input_new_flag = AVM_INPUT_VALID;
				break;
			case 0xE3:
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_AVM_2D;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_2D_BACK;
				input_new_flag = AVM_INPUT_VALID;
				break;
			case 0xE4:
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_AVM_2D;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_2D_LEFT;
				input_new_flag = AVM_INPUT_VALID;
				break;
			case 0xE5:
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_AVM_2D;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_2D_RIGHT;
				input_new_flag = AVM_INPUT_VALID;
				break;
			case 0xE6:
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_BACKUP;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_BACKUP_FRONT;
				input_new_flag = AVM_INPUT_VALID;
				break;
			case 0xE7:
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_PER;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_PER_FRONT;
				input_new_flag = AVM_INPUT_VALID;
				break;
			case 0xE8:
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_SVFS;
				input_new_flag = AVM_INPUT_VALID;
				break;
			case 0xE9:
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_BACKUP;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_BACKUP_BACK;
				input_new_flag = AVM_INPUT_VALID;
				break;
			case 0xEA:
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_PER;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_PER_BACK;
				input_new_flag = AVM_INPUT_VALID;		
				break;
			case 0xEB:
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_AVM_3D;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_3D_FRONT;
				
				view_keep_tmp.fly_flag = AVM_FLY_OFF;
				input_new_flag = AVM_INPUT_VALID;
				break;
			case 0xEC:
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_AVM_3D;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_3D_BACK;
				
				view_keep_tmp.fly_flag = AVM_FLY_ON;
				input_new_flag = AVM_INPUT_VALID;
				break;
			case 0xED:
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_AVM_3D;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_3D_LEFT;
				
				view_keep_tmp.fly_flag = AVM_FLY_ON;
				input_new_flag = AVM_INPUT_VALID;
				break;
			case 0xEE:
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_AVM_3D;
				view_keep_tmp.sub_mode_last = view_keep_tmp.sub_mode;
				view_keep_tmp.sub_mode = MODE_3D_RIGHT;
				
				view_keep_tmp.fly_flag = AVM_FLY_ON;
				input_new_flag = AVM_INPUT_VALID;
				break;
			case 0xEF:
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_AVM_LDWS;
				input_new_flag = AVM_INPUT_VALID;
				break;
			case 0xF0:
				view_keep_tmp.mode_last = view_keep_tmp.mode;
				view_keep_tmp.mode = MODE_AVM_BSD;
				input_new_flag = AVM_INPUT_VALID;
				break;
		}
	}
	if(input_new_flag == AVM_INPUT_VALID)
	{
		memcpy(&g_view_keep,&view_keep_tmp,sizeof(view_keep_t));
		g_view_keep.winclear = AVM_SCREEN_CLEAN_ON;
	}

}
#endif
