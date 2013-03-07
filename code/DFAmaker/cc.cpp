/*
c complier  - cc base defination
author:ghnjk
time:2013-2
*/
#include "cc.h"
void showerror(const char *err){
	fprintf(stderr,"lexical error: %s\n",err);
}

char lex_name[][15]={
	"<none>",
	"<keyword>",
	"<operator>",
	"<integer>",
	"<integer_0x>",
	"<integer_0>",
	"<long>",
	"<long_0x>",
	"<long_0>",
	"<real>",
	"<real_float>",
	"<macro>",
	"<ident>",
	"<string>",
	"<comment_//>",
	"<comment_/*>",
	"<character>",
	"<whitespace>"
};