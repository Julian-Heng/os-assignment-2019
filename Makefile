CC = gcc
CFLAGS = -g -Wall -Werror -ansi -pedantic

all: dirs
	$(CC) $(CFLAGS) -o ./build/scheduler ./src/scheduler.c

linkedList: dirs
	$(CC) $(CFLAGS) -o ./build/obj/linkedList.o -c ./src/linkedList.c

queue: linkedList
	$(CC) $(CFLAGS) -o ./build/obj/queue.o -c ./src/queue.c

test: runtest_linkedList runtest_queue

runtest_linkedList: test_linkedList
	./build/test_linkedList
	valgrind ./build/test_linkedList

runtest_queue: test_queue
	./build/test_queue
	valgrind ./build/test_queue

test_linkedList: linkedList
	$(CC) $(CFLAGS) -o ./build/obj/test_linkedList.o -c ./src/test_linkedList.c
	$(CC) ./build/obj/linkedList.o ./build/obj/test_linkedList.o -o ./build/test_linkedList

test_queue: queue
	$(CC) $(CFLAGS) -o ./build/obj/test_queue.o -c ./src/test_queue.c
	$(CC) ./build/obj/linkedList.o ./build/obj/queue.o ./build/obj/test_queue.o -o ./build/test_queue

clean:
	$(RM) -rv ./build

dirs:
	if [ ! -e "./build" ]; then \
		mkdir ./build; \
	fi
	if [ ! -e "./build/obj" ]; then \
		mkdir ./build/obj; \
	fi
