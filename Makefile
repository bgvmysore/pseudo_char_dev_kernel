obj-m += pseudodev.o

KSRC=/mnt/d/MyProjects2/c-cpp/qemu-emb-lnx/Embedded-Linux/linux-5.14.8

all:
	make -C $(KSRC) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- modules

clean:
	make -C $(KSRC) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- clean
