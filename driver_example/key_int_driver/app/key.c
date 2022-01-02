#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include "linux/ioctl.h"

int main(int argc,char *argv[])
{
    int fd,retvalue;
    unsigned char keyvalue;
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
        retvalue = read(fd,&keyvalue,sizeof(keyvalue));
        if(retvalue >= 0){
            if(keyvalue){
                keyvalue &= ~0x80;
                printf("Key1 Press,value = 0x%X\r\n",keyvalue);
            }
        }
    }

    retvalue = close(fd);/*关闭文件*/
        
    return retvalue;

}