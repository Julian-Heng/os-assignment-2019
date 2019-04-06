CC = gcc
CFLAGS = -g -Wall -Werror -ansi -pedantic
BUILD = ./build
OBJ = $(BUILD)/obj
SRC = ./src
TEST = $(SRC)/test

all: dirs scheduler

scheduler: dirs queue file
	$(CC) $(CFLAGS) -pthread -o $(OBJ)/scheduler.o -c $(SRC)/scheduler.c
	$(CC)	$(OBJ)/linkedList.o \
			$(OBJ)/queue.o \
			$(OBJ)/file.o \
			$(OBJ)/scheduler.o \
			-o $(BUILD)/scheduler

linkedList: dirs
	$(CC) $(CFLAGS) -o $(OBJ)/linkedList.o -c $(SRC)/linkedList.c

queue: linkedList
	$(CC) $(CFLAGS) -o $(OBJ)/queue.o -c $(SRC)/queue.c

file: dirs
	$(CC) $(CFLAGS) -o $(OBJ)/file.o -c $(SRC)/file.c

test: runtest_linkedList runtest_queue test_file

runtest_linkedList: test_linkedList
	$(BUILD)/test_linkedList
	valgrind $(BUILD)/test_linkedList

runtest_queue: test_queue
	$(BUILD)/test_queue
	valgrind $(BUILD)/test_queue

test_linkedList: linkedList
	$(CC) $(CFLAGS) -o $(OBJ)/test_linkedList.o -c $(TEST)/test_linkedList.c
	$(CC) 	$(OBJ)/linkedList.o \
			$(OBJ)/test_linkedList.o \
			-o $(BUILD)/test_linkedList

test_queue: queue
	$(CC) $(CFLAGS) -o $(OBJ)/test_queue.o -c $(TEST)/test_queue.c
	$(CC) 	$(OBJ)/linkedList.o \
			$(OBJ)/queue.o \
			$(OBJ)/test_queue.o \
			-o $(BUILD)/test_queue

test_file: file queue
	$(CC) $(CFLAGS) -o $(OBJ)/test_file.o -c $(TEST)/test_file.c
	$(CC)	$(OBJ)/file.o \
			$(OBJ)/linkedList.o \
			$(OBJ)/queue.o \
			$(OBJ)/test_file.o \
			-o $(BUILD)/test_file

clean:
	$(RM) -rv $(BUILD)

dirs:
	if [ ! -e "$(BUILD)" ]; then \
		mkdir $(BUILD); \
	fi
	if [ ! -e "$(OBJ)" ]; then \
		mkdir $(OBJ); \
	fi
