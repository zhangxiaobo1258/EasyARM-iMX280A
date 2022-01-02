#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"

#define LEDOFF  0
#define LEDON   1
#define BEEPOFF 2
#define BEEPON  3

int main(int argc,char *argv[])
{
    int fd,retvalue;
    int i;
/*    if(argc != 3){
        printf("Error Usage!\r\n");
        printf("Usage:\r\n");
        printf("./led [device] [option]\r\n");
        printf("l:led;b:beep.\r\n");
        printf("0:off;1:on\r\n");
        return -1;
    }*/

    fd = open("/dev/led",O_RDWR);
    if(fd < 0){
        printf("File '/dev/led' open failed!\r\n");
        return -1;
    }
    for( i = 0; i < 100; i++ ){
        retvalue = ioctl(fd,'b','1');
        if(retvalue < 0){
            printf("Beep control failed!\r\n");
            close(fd);
            return -1;
        }
        usleep(5);
        retvalue = ioctl(fd,'b','0');
        if(retvalue < 0){
            printf("Beep control failed!\r\n");
            close(fd);
            return -1;
        }
        //usleep(50);
    }
    
    while(1)
    {
        retvalue = ioctl(fd,'l','1');
        if(retvalue < 0){
            printf("Led control failed!\r\n");
            close(fd);
            return -1;
        }
        sleep(1);
        retvalue = ioctl(fd,'l','0');
        if(retvalue < 0){
            printf("Led control failed!\r\n");
            close(fd);
            return -1;
        }
        sleep(1);
    }
    return 0;

}