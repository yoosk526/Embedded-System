#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv){
    char buz_state = '0';   // '1' : 부저 울림, '0' : 부저 꺼짐

    int i;
    int delay_time = 1;     // 1초 간격으로 부저가 울리게 함

    int dev = open("/dev/my_buzzer", O_WRONLY);
    
    if (dev == -1){
        printf("Buzzer device Opening was not possible!\n");
		return -1;
    }
    printf("Buzzer device opening was successfull!\n");
    
    // main 함수의 매개변수 -> command 입력 할 때 전달 가능 ($ sudo ./buzzer Hello)
    printf("argc = %d\n", argc);
    for (i = 0; i < argc; i++)
        printf("argv[%d] = %s\n", i, argv[i]);

    while (1) {
        write(dev, &buz_state, 1);      // 1초 간격으로 부저가 울림
        sleep(delay_time);
        if (buz_state == '0'){
            buz_state = '1';
        }
        else if (buz_state == '1'){
            buz_state = '0';
        }
        
    }
    

    close(dev);
    return 0;
}