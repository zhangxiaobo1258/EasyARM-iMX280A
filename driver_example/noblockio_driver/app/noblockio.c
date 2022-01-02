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

int main(int argc,char *argv[])
{
    int fd,ret = 0;
    char *filename;
    struct pollfd fds;
    fd_set readfds;
    struct timeval timeout;
    unsigned char data;

    if(argc != 2){
        printf("Error Usage!\r\n");
        return -1;
    }

    filename = argv[1];
    /* 打开key驱动 */
    fd = open(filename,O_RDWR | O_NONBLOCK);
    if(fd < 0){
        printf("File %s open failed!\r\n",argv[1]);
        return -1;
    }   
    fds.fd = fd;                /* 构造结构体 */
    fds.events = POLLIN;        
    
    while(1)
    {
        ret = poll(&fds,1,500);
        if(ret){
            ret = read(fd,&data,sizeof(data));
            if(ret >= 0 ){
                if(data){
                    printf("Key1 Press,value = 0x%X\r\n",data);
                }
            }
        }else if(ret == 0){     /* 超时 */
            /* 自定义超时处理 */
        }else if(ret < 0){      /* 错误 */
            /* 自定义错误处理 */
        }
    }
#if 0 
    while(1){
        FD_ZERO(&readfds);
        FD_SET(fd,&readfds);
        /* 构造超时时间 */
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;           /* 500ms */
        ret = select(fd+1,&readfds,NULL,NULL,&timeout);
        switch (ret)
        {
        case 0:             /* 超时 */
            /* 用户自定义超时处理 */
            break;
        case -1:
            /* 用户自定义错误处理 */
            break;
        default:
            if(FD_ISSET(fd,&readfds)){
                ret = read(fd,&data,sizeof(data));            
                if(ret >= 0 ){
                    if(data){
                        printf("Key1 Press,value = 0x%X\r\n",data);
                    }
                }
            }
            break;
        }
    }
#endif
    close(fd);/*关闭文件*/
        
    return ret;

}