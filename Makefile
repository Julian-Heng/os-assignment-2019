CC = gcc
CFLAGS = -Wall -Werror -ansi -pedantic

all: dirs
	$(CC) $(CFLAGS) -o ./build/scheduler ./src/scheduler.c

clean:
	$(RM) -rf ./build

dirs:
	if [ ! -e "./build" ]; then \
		mkdir ./build; \
	fi
