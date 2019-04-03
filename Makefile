CC = gcc
CFLAGS = -g -Wall -Werror -ansi -pedantic

all: dirs
	$(CC) $(CFLAGS) -o ./build/scheduler ./src/scheduler.c

test: test_linkedList
	./build/test_linkedList
	valgrind ./build/test_linkedList

linkedList: dirs
	$(CC) $(CFLAGS) -o ./build/obj/linkedList.o -c ./src/linkedList.c

test_linkedList: linkedList
	$(CC) $(CFLAGS) -o ./build/obj/test_linkedList.o -c ./src/test_linkedList.c
	$(CC) ./build/obj/linkedList.o ./build/obj/test_linkedList.o -o ./build/test_linkedList

clean:
	$(RM) -rv ./build

dirs:
	if [ ! -e "./build" ]; then \
		mkdir ./build; \
	fi
	if [ ! -e "./build/obj" ]; then \
		mkdir ./build/obj; \
	fi
