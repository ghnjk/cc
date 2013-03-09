# makefile for jcc/dfamaker
# date 2013-3
###

#dfa maker projects
DFA=dfamaker.out
DFA_OBJS=dfamaker.o cc.o


$(DFA): $(DFA_OBJS)
	$(CC) $(CFLAGS) -o $(DFA) $(DFA_OBJS)

#由G++ -MM产生的依赖关系
cc.o: cc.cpp cc.h
	$(CC) $(CFLAGS) -c $^
dfamaker.o: dfamaker.cpp cc.h
	$(CC) $(CFLAGS) -c $^

