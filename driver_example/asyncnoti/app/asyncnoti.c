#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include "poll.h"
#include "sys/select.h"
#include "sys/time.h"
#include "linux/ioctl.h"
#include "signal.h"

static int fd = 0;                          /* 文件描述 */

static void sigio_signal_func(int signum)
{
    int err = 0;
    unsigned int keyvalue = 0;
    err = read(fd,&keyvalue,sizeof(keyvalue));
    if(err < 0){
        /* 读取错误 */
    }else{
        printf("sigio signal! key value = %d\r\n",keyvalue);
    }
}

int main(int argc,char *argv[])
{
    int flags = 0;
    char *filename;

    if(argc != 2){
        printf("Error Usage!\r\n");
        return -1;
    }

    filename = argv[1];
    /* 打开key驱动 */
    fd = open(filename,O_RDWR );
    if(fd < 0){
        printf("File %s open failed!\r\n",argv[1]);
        return -1;
    }
    /* 设置信号SIGIO的处理函数 */
    signal(SIGIO,sigio_signal_func);

    fcntl(fd,F_SETOWN,getpid());            /* 将当前进程的进程号告诉给内核 */
    flags = fcntl(fd,F_GETFD);              /* 获取当前的进程状态 */
    fcntl(fd,F_SETFL,flags | FASYNC);       /* 设置进程启用异步通知功能 */

    while(1){
        sleep(2);
    }

    close(fd);/*关闭文件*/
        
    return 0;

}