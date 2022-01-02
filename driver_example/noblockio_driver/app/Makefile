COMPILE	  := /home/davinci/Projects/buildroot-2020.02.5/output/host/bin/arm-buildroot-linux-uclibcgnueabi-
EXEC = noblockio
OBJS = noblockio.o
SRC  = noblockio.c
CFLAGS = -Wall -g -o2 

all: clean $(EXEC) 
$(EXEC):$(OBJS)
	$(COMPILE)gcc $(CFLAGS)  $(SRC) -o $@
clean:
	rm -rf $(EXEC) *.o
