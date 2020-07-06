#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char write_buf[100];
static char read_buf[100];
static char app_data[] = "This is app data!";

/*
 * @description		: main主程序
 * @param - argc 	: argv数组元素个数
 * @param - argv 	: 具体参数
 * @return 			: 0 成功;其他 失败
 */
int main(int argc, char *argv[])
{
    int fd, ret;
    char *filename;

    if(argc != 3){
        printf("Input Error!\r\n");
    }
    /* 打开设备文件，若失败直接退出并返回-1 */
    filename = argv[1];
    fd = open(filename, O_RDWR);
    if(fd < 0){
        printf("Can't open file %s !\r\n", filename);
        return -1;
    }

    /* 判断argv参数是否等于1,然后向设备发送数据 */
    if(atoi(argv[2]) == 1)
    {
        memcpy(write_buf, app_data, sizeof(app_data));
        ret = write(fd, write_buf, 50);
        if(ret < 0){
            printf("APP send data failed!\r\n");
        }else{
            printf("App send data ok!\r\n");
        }
    }
    
    /* 判断argv参数是否等于2,然后读取设备数据 */
    if(atoi(argv[2]) == 2)
    {
        ret = read(fd, read_buf, 50);
        if(ret < 0){
            printf("App receive data failed!\r\n");
        }else{
            printf("App receive data: %s\r\n", read_buf);
        }
    }

    /* 关闭设备文件 */
    ret = close(fd);
    if(ret < 0){
        printf("Can't Close file %s!\r\n", filename);
        return -1;
    }

    return 0;
}