/*
c complier  - dfa definition
author:ghnjk
time:2013-2
*/
#include "DFA.h"
DFA::DFA(){
	this->m_IsMatch=true;
	this->_buffer.clear();
	this->_charset_cnt=0;
	this->_cur=this->_end=0;
	this->_is_ready=false;
	this->_line=0;
	this->_node_cnt=0;

	/*load lexical_dfa.dat*/
	FILE *fp=fopen("lexical_dfa.dat","r");
	int i,j;
	if(fp==NULL){
		showerror("load file lexical_dfa.dat error.");
		return;
	}
	fscanf(fp,"%d%d",&_node_cnt,&_charset_cnt);
	if(_node_cnt>FA_NODE_MAX){
		showerror("build dfa error,cannot alloc new node.");
		return;
	}
	for(i=0;i<_node_cnt;i++){
		_dfa[i].next=new int[_charset_cnt];
		if(_dfa[i].next==NULL){
			showerror("build dfa error,cannot alloc new node.");
			return;
		}
		fscanf(fp,"%d",&_dfa[i].type);
		for(j=0;j<_charset_cnt;j++)
			fscanf(fp,"%d",&_dfa[i].next[j]);
	}
	_is_ready=true;
	fclose(fp);
}
DFA::~DFA(){
}
int DFA::lexical()		/*词法分析，如果程序错误返回小于0的值 如果代码错误 返回大于0的值 匹配成功返回 0*/
{
	if(_is_ready==false)return -1;
	_line=1;
	_is_macro=false;
	_cur=_end=0;
	m_IsMatch=true;
	while(_peak_next_char()){
		_lstline=_line;
		_dfa_find(0);
	}
	if(m_IsMatch)return 0;
	return 1;
}
bool DFA::_dfa_find(int r,bool nd){
	bool ans=true;
	int ch;
	bool alt;
	switch(_dfa[r].type)
	{
	case LEX_KEYWORD:
	case LEX_OPERATOR:
	case LEX_INTEGER:
	case LEX_INTEGER_OX:
	case LEX_INTEGER_O:
	case LEX_LONG:
	case LEX_LONG_OX:
	case LEX_LONG_O:
	case LEX_REAL:
	case LEX_REAL_FLOAT:
	case LEX_IDENT:
		_unget_char();
		printf("%-9d",_lstline);
		if(_is_macro)printf("<macro>\t");
		printf("%-15s\t",lex_name[_dfa[r].type]);
		_output_cur();
		putchar('\n');
		break;
	case LEX_CHARACTER:
		alt=false;
		while(ch=_next_char_with_not_op()){
			if(alt==false && ch=='\'' )
				break;
			else if(ch=='\\')
				alt=!alt;
			else if(ch!='\\')
				alt=false;
			if(ch=='\n'){
				_is_macro=false;
				break;
			}
		}
		if(ch!='\''){
			ans=false;
		}
		else{
			printf("%-9d",_lstline);
			if(_is_macro)printf("<macro>\t");
			printf("%-15s\t",lex_name[_dfa[r].type]);
			_output_cur();
			putchar('\n');
		}
		break;
	case LEX_COMMENT_S:
		_unget_char();
		while( (ch=_peak_next_char() ) && ch !='\n' )
			_next_char();
	//	printf("%-9d",_lstline);
		_is_macro=false;
	//	printf("%-15s\t",lex_name[_dfa[r].type]);
		_output_cur(NULL);
	//	putchar('\n');
		break;
	case LEX_COMMENT_M:
		alt=false;
		while(ch=_peak_next_char()){
			if(alt==true && ch=='/' )
				break;
			else if(ch=='*')
				alt=true;
			else
				alt=false;
			if(ch=='\n')_is_macro=false;
			_next_char();
		}
		if(_peak_next_char()!='/')
			ans=false;
		else{
			_next_char();
		//	printf("%-9d",_lstline);
		//	printf("%-15s\t",lex_name[_dfa[r].type]);
			_output_cur(NULL);
		//	putchar('\n');
		}
		break;
	case LEX_STRING:
		alt=false;
		while(ch=_next_char_with_not_op()){
			if(alt==false && ch=='\"' )
				break;
			else if(ch=='\\')
				alt=!alt;
			else if(ch!='\\')
				alt=false;
			if(ch=='\n'){
				_is_macro=false;
				break;
			}
		}
		if(ch!='\"'){
			ans=false;
		}
		else{
			printf("%-9d",_lstline);
			if(_is_macro)printf("<macro>\t");
			printf("%-15s\t",lex_name[_dfa[r].type]);
			_output_cur();
			putchar('\n');
		}
		break;
	case LEX_MACRO:
		_unget_char();
		_is_macro=true;
		printf("%-9d",_lstline);
		printf("%-15s\t",lex_name[_dfa[r].type]);
		_output_cur();
		putchar('\n');
		break;
	case LEX_WS:
		_unget_char();
		ch=_peak_next_char();
		if(ch=='\n')_is_macro=false;
		if(ch)
			_next_char();
		_output_cur(NULL);
		break;
	case LEX_NONE:
		ans=false;
		ch=_peak_next_char();
		if(ch>=0 && ch<_charset_cnt && _dfa[r].next[ch]>=0){
			_next_char();
			ans=_dfa_find(_dfa[r].next[ch],false);
		}
		if(ans==false && nd){
			while(ch && ch!=' ' && ch!='\t' && ch!='\n')
				ch=_next_char();
			fprintf(stderr,"lexical error(%d): ",_lstline);
			_output_cur(stderr);
			fprintf(stderr,"\n");
			m_IsMatch=false;
		}
		break;
	}
	return ans;
}
int DFA::_next_char()				/*忽略\r 经过处理（将\\\n忽略掉*/
{
	int ch;
	while( (ch=_next_char_with_not_op())=='\\' ){
		if(_next_char_with_not_op()=='\n')
			continue;
		_cur--;
		return ch;
	}
	return ch;
}
int DFA::_peak_next_char()
{
	int ch=_next_char();
	_unget_char();
	return ch;
}
int DFA::_next_char_with_not_op()	/*忽略\r 未处理的*/
{
	int ch;
	while(true){
		if(_cur>=_end){
			if(_end>=(int)_buffer.size())
				_buffer.push_back(0);
			_buffer[_end]=getchar();
			if(_buffer[_end]==EOF)
				_buffer[_end]=0;
			_end++;
		}
		ch=_buffer[_cur++];
		if(ch==0)break;
		if(ch=='\r')continue;
		if(ch=='\n')_line++;
		return ch;
	}
	return ch;
}
void DFA::_unget_char(){
	int p;
	if(_cur<=0)return;
	p=_cur-1;
	if(_cur>=2 && _buffer[_cur-1]=='\n' &&_buffer[_cur-2]=='\\')
		p--;
	while(_cur>p){
		_cur--;
		if(_buffer[_cur]=='\n')
			_line--;
	}
}
void DFA::_output_cur(FILE *fp){
	int i;
	if(fp){
		//putchar('#');
		for(i=0;i<_cur;i++)
			fputc(_buffer[i],fp);
		//putchar('#');
	}
	for(i=_cur;i<_end;i++)
		_buffer[i-_cur]=_buffer[i];
	_end-=_cur;
	_cur=0;
}