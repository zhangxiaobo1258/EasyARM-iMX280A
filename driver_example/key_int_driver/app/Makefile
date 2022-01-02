COMPILE	  := /home/davinci/Projects/buildroot-2020.02.5/output/host/bin/arm-buildroot-linux-uclibcgnueabi-
EXEC = key
OBJS = key.o
SRC  = key.c
CFLAGS = -Wall -g -o2 

all: clean $(EXEC) 
$(EXEC):$(OBJS)
	$(COMPILE)gcc $(CFLAGS)  $(SRC) -o $@
clean:
	rm -rf $(EXEC) *.o
