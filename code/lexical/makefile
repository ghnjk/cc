# makefile for jcc/lexical
# date 2013-2
###
#lexical projects
LEX=lex
LEX_OBJS=lexical.o cc.o dfa.o

$(LEX): $(LEX_OBJS)
	$(CC) $(CFLAGS) -o $(LEX) $(LEX_OBJS)

#由G++ -MM产生的依赖关系
cc.o: cc.cpp cc.h
	$(CC) $(CFLAGS) -c $^
dfa.o: dfa.cpp DFA.h cc.h
	$(CC) $(CFLAGS) -c $^
lexical.o: lexical.cpp cc.h DFA.h
	$(CC) $(CFLAGS) -c $^

