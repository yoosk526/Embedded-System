obj-m += buzzer_driver.o
KDIR = ~/working/kernel
RESULT1 = buzzer
SRC1 = $(RESULT1).c
CCC = gcc

all:
	make -C $(KDIR) M=$(PWD) modules
	$(CCC) -o $(RESULT1) $(SRC1)
	sudo insmod $(RESULT1)_driver.ko
	
clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(RESULT1)
	sudo rmmod $(RESULT1)_driver.ko
