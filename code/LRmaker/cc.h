/*
c complier  - cc base defination
author:ghnjk
time:2013-2
*/
#include<stdio.h>

#ifndef __C_C_H__
#define __C_C_H__


/*base macro defination */
#define LEX_NONE		0x00	//none
#define LEX_KEYWORD		0x01	//keyword
#define LEX_OPERATOR	0x02	//operator
#define	LEX_INTEGER		0x03	//integer
#define	LEX_INTEGER_OX	0x04	//integer_0x
#define LEX_INTEGER_O	0x05	//integer_0
#define LEX_LONG		0x06	//long
#define LEX_LONG_OX		0x07	//long_0x
#define LEX_LONG_O		0X08	//long_0
#define LEX_REAL		0x09
#define LEX_REAL_FLOAT	0x0A
#define LEX_MACRO		0x0B	//macro
#define LEX_IDENT		0x0C	//ident
#define LEX_STRING		0x0D	//string
#define LEX_COMMENT_S	0x0E	//comment_//
#define LEX_COMMENT_M	0x0F	//comment_/*
#define LEX_CHARACTER	0x10	//character
#define LEX_WS			0x11	//white space
/*base const defination */

/* base variant*/
extern char lex_name[18][15];

/*base function defination*/
void showerror(const char *err);

#endif