CC 		= gcc
CFLAGS 	= -g -Wall -Werror -ansi -pedantic
BUILD 	= ./build
OBJ 	= $(BUILD)/obj
SRC 	= ./src
TEST 	= $(SRC)/test
REPORT 	= ./docs/report

all: $(BUILD) $(OBJ) scheduler
debug: $(BUILD) $(OBJ) scheduler_debug

scheduler: $(BUILD) queue file
	$(CC) $(CFLAGS) -o $(OBJ)/scheduler.o -c $(SRC)/scheduler.c
	$(CC) -pthread 	$(OBJ)/linkedList.o \
					$(OBJ)/queue.o \
					$(OBJ)/file.o \
					$(OBJ)/scheduler.o \
					-o $(BUILD)/scheduler

scheduler_debug: $(BUILD) queue file
	$(CC) $(CFLAGS) -DDEBUG=1 -o $(OBJ)/scheduler.o -c $(SRC)/scheduler.c
	$(CC) -pthread 	$(OBJ)/linkedList.o \
					$(OBJ)/queue.o \
					$(OBJ)/file.o \
					$(OBJ)/scheduler.o \
					-o $(BUILD)/scheduler

linkedList: $(OBJ)
	$(CC) $(CFLAGS) -o $(OBJ)/linkedList.o -c $(SRC)/linkedList.c

queue: linkedList
	$(CC) $(CFLAGS) -o $(OBJ)/queue.o -c $(SRC)/queue.c

file: $(OBJ)
	$(CC) $(CFLAGS) -o $(OBJ)/file.o -c $(SRC)/file.c

test: runtest_linkedList runtest_queue test_file runtest_scheduler

runtest_linkedList: test_linkedList
	$(BUILD)/test_linkedList
	valgrind $(BUILD)/test_linkedList

runtest_queue: test_queue
	$(BUILD)/test_queue
	valgrind $(BUILD)/test_queue

runtest_scheduler: scheduler_debug
	for i in 1 2 3 4 5 6 7 8 9 10; do \
		for tool in helgrind drd; do \
			valgrind --tool="$$tool" \
				./build/scheduler ./resources/small_tasks "$$i"; \
		done; \
	done

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
	$(CC) $(CFLAGS) -o $(OBJ)/test_file_read.o -c $(TEST)/test_file_read.c
	$(CC) $(CFLAGS) -o $(OBJ)/test_file_write.o -c $(TEST)/test_file_write.c
	$(CC)	$(OBJ)/file.o \
			$(OBJ)/linkedList.o \
			$(OBJ)/queue.o \
			$(OBJ)/test_file_read.o \
			-o $(BUILD)/test_file_read
	$(CC)	$(OBJ)/file.o \
			$(OBJ)/linkedList.o \
			$(OBJ)/queue.o \
			$(OBJ)/test_file_write.o \
			-o $(BUILD)/test_file_write

docs: $(REPORT)
	$(RM) -v *_log

	if [ ! -e "$(REPORT)/file_listing" ]; then \
		{ \
			printf "$$ %s\\n" "ls -Rl ."; \
			ls -Rl .; \
		} > $(REPORT)/file_listing; \
	fi

	make scheduler

	if [ ! -e "$(REPORT)/demo" ]; then \
		{ \
			printf "$$ %s\\n" \
				   "./build/scheduler"; \
			./build/scheduler 2>&1; \
			if command -v time > /dev/null 2>&1; then \
				printf "\\n$$ %s\\n" \
					   "time ./build/scheduler ./resources/small_tasks 5"; \
				time ./build/scheduler ./resources/small_tasks 5 2>&1; \
			else \
				printf "\\n$$ %s\\n" \
					   "./build/scheduler ./resources/small_tasks 5"; \
				./build/scheduler ./resources/small_tasks 5; \
			fi; \
			printf "\\n$$ %s\\n" "cat ./simulation_log"; \
			cat ./simulation_log; \
			if command -v valgrind > /dev/null 2>&1; then \
				for i in "memcheck" "helgrind" "drd"; do \
					printf "\\n$$ %s %s %s %s\\n" \
						   "valgrind" \
						   "--tool=$$i" \
						   "./build/scheduler" \
						   "./resources/small_tasks 5"; \
					valgrind --tool=$$i \
							 ./build/scheduler \
							 ./resources/small_tasks 5 2>&1; \
				done; \
			fi; \
		} > $(REPORT)/demo; \
	fi

	pdflatex -output-directory $(REPORT) ./docs/AssignmentDoc.tex
	pdflatex -output-directory $(REPORT) ./docs/AssignmentDoc.tex

dist: dist_clean docs
	mkdir -p $(HOME)/OS/assignment
	cp -r ./* $(HOME)/OS/assignment
	( \
		cd $(HOME)/OS/assignment; \
		$(RM) -rv ./build; \
		cd ..; \
		zip -r 19473701_OS_Assignment.zip ./assignment -x \*.swp; \
	)

dist_clean:
	$(RM) -rv $(HOME)/OS

clean:
	$(RM) -rv $(BUILD) $(REPORT) *_log

$(BUILD) $(OBJ) $(REPORT):
	if [ ! -e "$@" ]; then \
		mkdir -p $@; \
	fi

.PHONY: clean docs
