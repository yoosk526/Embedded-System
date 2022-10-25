obj-m += dev_nr.o
RESULT = major_num_example
SRC = $(RESULT).c

all:
	make -C /home/vlsi-emb2022/working/kernel M=$(PWD) modules
	aarch64-linux-gnu-gcc -o $(RESULT) $(SRC)

clean:
	make -C $(HOME)/wokring/kernel M=$(PWD) clean
	rm -f $(RESULT)

