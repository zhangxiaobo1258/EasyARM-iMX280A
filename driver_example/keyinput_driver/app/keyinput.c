#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <linux/input.h>

/* 定义一个input_event变量，存放输入事件信息 */
static struct input_event inputevent;

int main(int argc,char *argv[])
{
    int fd;
    int err = 0;
    char *filename;

    if(argc != 2){
        printf("Error Usage!\r\n");
        return -1;
    }

    filename = argv[1];
    /* 打开key驱动 */
    fd = open(filename,O_RDWR);
    if(fd < 0){
        printf("File %s open failed!\r\n",argv[1]);
        return -1;
    }

    while(1)
    {
        err = read(fd,&inputevent,sizeof(inputevent));
        if(err > 0 ){
            switch (inputevent.type){
            case EV_KEY:
                if(inputevent.code < BTN_MISC){     /* 键盘键值 */
                    printf("Key %d %s\r\n",inputevent.code,inputevent.value ? "press" : "release");
                }else{
                    printf("Key %d %s\r\n",inputevent.code,inputevent.value ? "press" : "release");
                }
                break;
            case EV_REL:
                break;
            case EV_ABS:
                break;
            case EV_MSC:
                break;
            case EV_SW:
                break;
            default:
                break;
            }
        }else{
            printf("read data failed.\r\n");
        }
    }
    close(fd);
    return 0;

}