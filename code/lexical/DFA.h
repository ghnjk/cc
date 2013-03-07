/*
c complier  - dfa definition
author:ghnjk
time:2013-2
*/
#include"cc.h"
#include<string.h>
#include<vector>
#include<stdio.h>
#include<iostream>
using namespace std;

#ifndef __DFA__H__
#define __DFA__H__

#define FA_NODE_MAX 1024

/*词法分析器的自动机*/
class DFA{
public:
	class DFANode{		//自动机中的节点
	public:
		int type;
		int *next;
		DFANode(){next=NULL;}
		virtual ~DFANode(){
			if(next)delete next;
		}
	};
	
public:
	DFA();
	virtual ~DFA();
	bool	m_IsMatch;	/*自动机匹配时是否合法*/
public:
	int lexical();		/*词法分析，如果程序错误返回小于0的值 如果代码错误 返回大于0的值 匹配成功返回 0*/
protected:
	bool _dfa_find(int r,bool nd=true);
	int _peak_next_char();
	int _next_char();				/*忽略\r 经过处理（将\\\n忽略掉*/
	int _next_char_with_not_op();	/*忽略\r 未处理的*/
	void _unget_char();
	void _output_cur(FILE *fp=stdout);
protected:
	bool			_is_ready;		/*自动机是否创建完毕*/
	vector<char>	_buffer;
	int				_lstline;
	int				_line;
	int				_cur;
	int				_end;
	bool			_is_macro;

	int				_node_cnt;
	int				_charset_cnt;
	DFANode			_dfa[FA_NODE_MAX];
};

#endif