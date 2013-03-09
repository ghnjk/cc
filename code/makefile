# makefile for jcc
# date 2013-3
###
VPATH = DFAmaker:lexical

CC=g++
CFLAGS=

include DFAmaker/makefile
include lexical/makefile

APP=$(DFA) $(LEX)
OBJECTS=$(DFA_OBJS) $(LEX_OBJS)
all:$(APP)

#clean
.PHONY : clean
clean:
	@- rm $(APP)
	@- rm $(OBJECTS)
install:all
	@if [ ! -e bin ]; \
	then \
		mkdir bin;\
	fi
	mv $(DFA) bin/
	mv $(LEX) bin/
