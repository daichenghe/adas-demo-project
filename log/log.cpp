#include <log.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>



/* Private variables */

static FILE *flog = NULL;

static pthread_mutex_t avm_log_mutex;


/*
Function process:
	+ Open log file
	+ init mutex
	+ recoder the time
*/
void log_open(void)
{
    time_t time_of_day;

    time_of_day = time( NULL );

    flog = fopen("./avm.log","a+");
    if(flog==0)
    {
 	  //sys_err("open avm.log file error\r\n");
    }

    fprintf(flog, "\n\nstart new log time: %s",ctime( &time_of_day ));

    pthread_mutex_init(&avm_log_mutex, NULL);
}

/*
Function process:
	+ write log information to log file
*/
void logg(log_level prio,const char *fmt, ...)
{
    char buf[LOG_BUF_SIZE]={0};
    struct timespec   abstime = {1,0};

	/* 函数可重入设计 */
    int ret = pthread_mutex_timedlock(&avm_log_mutex,&abstime);
    if(ret != 0 )
    {
        return;
    }
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    if(flog)
    {
        switch(prio)
        {
        case ERR:
        	fprintf(flog,"ERR:");
           // sys_err("%s",buf);
        	break;
        case WARNING:
        	fprintf(flog,"WARNING:");
          //  sys_err("%s",buf);
        	break;
        default:
        	fprintf(flog,"MESSAGE:");
          //  sys_inf("%s",buf);
        	break;
        }

        fprintf(flog,"%s",buf);
        fflush(flog);
    }
    pthread_mutex_unlock(&avm_log_mutex);
}

/*
Function process:
	+ close log file
*/
void log_close(void)
{
    fflush(flog);
    fclose(flog);
}



