#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

static struct termios init_setting, new_setting;
char seg_num[10] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90 };
char seg_dnum[10] = { 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x58, 0x00, 0x10 };

#define D1 0x01
#define D2 0x02
#define D3 0x04
#define D4 0x08

void init_keyboard() 
{
	tcgetattr(STDIN_FILENO, &init_setting);
	new_setting = init_setting;
	new_setting.c_lflag &= ~ICANON;
	new_setting.c_lflag &= ~ECHO;
	new_setting.c_cc[VMIN] = 0;
	new_setting.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &new_setting);
}

void init_keyboard2()
{
	tcgetattr(STDIN_FILENO, &init_setting);
	new_setting = init_setting;
	new_setting.c_lflag &= ICANON;
	new_setting.c_lflag &= ECHO;
	new_setting.c_cc[VMIN] = 4;
	new_setting.c_cc[VTIME] = 10;
	tcsetattr(0, TCSANOW, &new_setting);
}

void close_keyboard()
{
	tcsetattr(0, TCSANOW, &init_setting);
}

char get_key()
{
	char ch = -1;

	if (read(STDIN_FILENO, &ch, 1) != 1)
		ch = -1;
	return ch;
}

void print_menu()
{
	printf("\n----------menu----------\n");
	printf("[q] : program exit\n");
	printf("[u] : up count\n");
	printf("[d] : down count\n");
	printf("[p] : put count num\n");
	printf("------------------------\n\n");
}

int main(int argc, char** argv)
{
	unsigned short data[4];
	char key;
	unsigned short buff;
	int tmp_n;
	int count = 0;
	int delay_time;
	
	char buff2[2];
	char tmp[2];
	char prev[2] = { 'r','r' };

	int dev_seg = open("/dev/my_segment", O_RDWR); // if you want read='O_RDONLY' write='O_WRONLY', read&write='O_RDWR'
	int dev_button = open("/dev/my_gpio", O_RDWR);

	if (dev_seg == -1) {
		printf("Segment device Opening was not possible!\n");
		return -1;
	}
	printf("Segment device opening was successfull!\n");

	if (dev_button == -1) {
		printf("Button device Opening was not possible!\n");
		return -1;
	}
	printf("Button device opening was successfull!\n");

	init_keyboard();
	print_menu();
	tmp_n = 0;
	delay_time = 1000000;

	data[0] = (seg_num[0] << 4) | D1;
	data[1] = (seg_num[0] << 4) | D2;
	data[2] = (seg_num[0] << 4) | D3;
	data[3] = (seg_num[0] << 4) | D4;

	while (1) {
		delay_time = 2000;
		key = get_key();
		if (key == 'q') {
			printf("exit this program.\n");
			break;
		}
		else if (key == 'u') {
			count++;
		}
		else if (key == 'd') {
			count--;
		}
		else if (key == 'p') {
			close_keyboard();
			init_keyboard2();
			printf("put num\n");
			scanf("%d", &count);
			close_keyboard();
			init_keyboard();
		}

		read(dev_button, buff2, 2);
		prev[0] = tmp[0];
		tmp[0] = buff2[0];

		prev[1] = tmp[1];
		tmp[1] = buff2[1];

		if (prev[0] != tmp[0])
		{
			if (tmp[0] == '1')
			{
				count++;
				printf("Up Button Pushed! \n");
			}
		}
		if (prev[1] != tmp[1])
		{
			if (tmp[1] == '1')
			{
				count--;
				printf("Down Button Pushed! \n");
			}

		}

		if (count < 0000)
			count = 9999;
		if (count > 9999)
			count = 0;

		data[0] = (seg_num[count / 1000] << 4) | D1;
		data[1] = (seg_num[count % 1000 / 100] << 4) | D2;
		data[2] = (seg_num[count % 100 / 10] << 4) | D3;
		data[3] = (seg_num[count % 10] << 4) | D4;

		write(dev_seg, &data[tmp_n++], 2);	// Sizing 2 bytes
		usleep(delay_time);
		
		write(dev_seg, &data[tmp_n++], 2); 
		usleep(delay_time);

		write(dev_seg, &data[tmp_n++], 2);
		usleep(delay_time);

		write(dev_seg, &data[tmp_n++], 2);
		usleep(delay_time);
;
		if (tmp_n > 3) {
			tmp_n = 0;
		}
	}

	close_keyboard();
	write(dev_seg, 0x0000, 2); 
	close(dev_seg);
	close(dev_button);
	return 0;
}

