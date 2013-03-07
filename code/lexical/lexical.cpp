/*
c complier - lexical.exe
author:ghnjk
time:2013-2
*/
#include<stdio.h>
#include<string.h>
#include<string>
#include<iostream>
#include<vector>
#include<algorithm>
#include<stdlib.h>
#include<time.h>
#include<math.h>

#include"cc.h"
#include"DFA.h"
/*cmd: lexical.exe [inputfile [outputfile] ] */
int main(int argc,char **argv){
	if(argc>=2)
		freopen(argv[1],"r",stdin);
	if(argc>=3)
		freopen(argv[2],"w",stdout);
	if(argc>=4)
		freopen(argv[3],"w",stderr);
	DFA dfa;
	return dfa.lexical();
}
