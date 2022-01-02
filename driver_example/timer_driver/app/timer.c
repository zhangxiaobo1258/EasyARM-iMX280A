#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include <linux/ioctl.h>


#define CLOSE_CMD       1           /* 关闭定时器 */
#define OPEN_CMD        2           /* 打开定时器 */
#define SETPERIOD_CMD   3           /* 设置定时器周期 */

int main(int argc,char *argv[])
{
    int fd,retvalue;
    unsigned int cmd;
    unsigned char str[10];
    unsigned int timerperiod;
    int i;

    fd = open("/dev/timer",O_RDWR);
    if(fd < 0){
        printf("File '/dev/timer' open failed!\r\n");
        return -1;
    }
    
    while(1)
    {
        printf("Input CMD:");
        retvalue = scanf("%d",&cmd);
        if(retvalue != 1){              /* 如果参数输入有误 */
            gets(str);                  /* 防止卡死 */
        }
        switch(cmd)
        {
            case 1 :
                cmd = CLOSE_CMD;
            break;
            case 2:
                cmd = OPEN_CMD;
            break;
            case 3:
                cmd = SETPERIOD_CMD;
                printf("Input Timer Period:");
                retvalue = scanf("%d",&timerperiod);
                if(retvalue != 1){
                    gets(str);
                }
            break;
        }
        retvalue = ioctl(fd,cmd,timerperiod);
    }
    close(fd);

}