#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>

static struct termios init_setting, new_setting;

void init_keyboard() {
	tcgetattr(STDIN_FILENO, &init_setting);
	new_setting = init_setting;
	new_setting.c_lflag &= ~ICANON;
	new_setting.c_lflag &= ~ECHO;
	new_setting.c_cc[VMIN] = 0;
	new_setting.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &new_setting);
}

void close_keyboard() {
	tcsetattr(0, TCSANOW, &init_setting);
}

char get_key() {
	char ch = -1;

	if (read(STDIN_FILENO, &ch, 1) != 1)
		ch = -1;
	return ch;
}

int main(int argc, char** argv){
    double distance = 0.0;
    clock_t start, end;

    char key;					// 키보드 입력을 받음			
    char prev = 'r';
    char echo;                  // echo 신호
    char trigger = '0';         // Trigger에 인가하는 신호
    char buz_state = '0';       // '1' : 부저 울림, '0' : 부저 꺼짐
    int flag = 0;

    int i;
    int obstacle = 0;           // 초음파 센서로 앞에 장애물이 있는지 판별
    int delay_time = 1;         // 0.5초 간격으로 부저가 울리게 함

    int dev_buzzer = open("/dev/my_buzzer", O_WRONLY);
    int dev_wave = open("/dev/my_wave", O_RDWR);
    
    if (dev_buzzer == -1){
        printf("Buzzer device Opening was not possible!\n");
		return -1;
    }
    printf("Buzzer device opening was successfull!\n");
    
    if (dev_wave == -1){
        printf("Ultrawave device Opening was not possible!\n");
		return -1;
    }
    printf("Ultrawave device opening was successfull!\n");

    init_keyboard();
    printf("[q] : program exit\n");
    
    // main 함수의 매개변수 -> command 입력 할 때 전달 가능 ($ sudo ./buzzer Hello)
    printf("argc = %d\n", argc);
    for (i = 0; i < argc; i++)
        printf("argv[%d] = %s\n", i, argv[i]);

    write(dev_wave, &trigger, 1);

    while (1) {
        // key = get_key();
        // if (key == 'q'){
        //     printf("Exit this program.\n");
        //     break;
        // }

        // // 먼저 Trigger 핀을 off 상태로 만듬
        // write(dev_wave, &trigger, 1);

        // // 10us 펄스 인가
        // trigger = '1';
        // write(dev_wave, &trigger, 1);
        // printf("trigger = %c\n", trigger);
        // sleep(0.00001);
        // trigger = '0';
        // write(dev_wave, &trigger, 1);
        // printf("trigger = %c\n", trigger);
        // sleep(1);
        
        // // Echo 핀이 High로 되는 순간 while문 벗어남 -> 시작 시간
        // while (1) {
        //     read(dev_wave, &echo, 1);
        //     printf("first loop : echo = %c\n", echo);
        //     sleep(0.00001);
        //     if (echo == '1'){
        //         start = clock();
        //         break;
        //     }
        // }

        // printf("echo should be 1 : %c", echo);
        
        // // Echo 핀이 Low로 되는 순간 while문 벗어남 -> 종료 시간
        // while (echo == '1'){
        //     printf("not end %c\n", echo);
        //     end = clock();
        //     read(dev_wave, &echo, 1);
        // }
        // printf("%c\n", echo);

        // distance = (((end - start)/CLOCKS_PER_SEC) * 34000.0) / 2.0;
        // printf("Distance : %5.1f cm\n", distance);



        trigger = '1';
        write(dev_wave, &trigger, 1);
        sleep(0.00001);
        trigger = '0';
        write(dev_wave, &trigger, 1);

        read(dev_wave, &echo, 1);
        printf("first loop : echo = %c\n", echo);
        if (echo == '1' && flag == 0){
            start = clock();
            flag = 1;
            printf("echo high\n");
        }
        else if (echo == '0' && flag == 1){
            end = clock();
            flag = 0;
            printf("echo low\n");
            distance = (((end - start)/CLOCKS_PER_SEC) * 34000.0) / 2.0;
            printf("Distance : %5.1f cm\n", distance);
        }

        
        if (distance <= 50){
            obstacle = 1;
        }

        // 앞에 장애물이 있는 경우
        if (obstacle == 1){
            for (i = 0; i < 3; i++){           // delay_time * 3 sec 동안
                write(dev_buzzer, &buz_state, 1);      // 0.5초 간격으로 부저가 울림
                sleep(delay_time);
                if (buz_state == '0'){
                    buz_state = '1';
                }
                else if (buz_state == '1'){
                    buz_state = '0';
                }
            }
        }
            
        obstacle = 0;       // 장애물 Flag 초기화
    }

    close_keyboard();
    close(dev_wave);
    close(dev_buzzer);
    
    return 0;
}