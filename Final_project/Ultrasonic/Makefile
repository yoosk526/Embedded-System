obj-m += wave_driver.o buzzer_driver.o
KDIR = ~/working/kernel
RESULT = wave
SRC = $(RESULT).c
CCC = gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(CCC) -o $(RESULT) $(SRC)
	sudo insmod buzzer_driver.ko
	sudo insmod wave_driver.ko
	
clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(RESULT)
	sudo rmmod buzzer_driver.ko
	sudo rmmod wave_driver.ko
