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

/*�ʷ����������Զ���*/
class DFA{
public:
	class DFANode{		//�Զ����еĽڵ�
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
	bool	m_IsMatch;	/*�Զ���ƥ��ʱ�Ƿ�Ϸ�*/
public:
	int lexical();		/*�ʷ����������������󷵻�С��0��ֵ ���������� ���ش���0��ֵ ƥ��ɹ����� 0*/
protected:
	bool _dfa_find(int r,bool nd=true);
	int _peak_next_char();
	int _next_char();				/*����\r ����������\\\n���Ե�*/
	int _next_char_with_not_op();	/*����\r δ�����*/
	void _unget_char();
	void _output_cur(FILE *fp=stdout);
protected:
	bool			_is_ready;		/*�Զ����Ƿ񴴽����*/
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