/*
c complier - dfa maker.exe
input:	none
output:
	DFA_TABLE:	lexical_dfa.dat
author:ghnjk
time:2013-2
*/
#include<algorithm>
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<string>
#include<iostream>
#include<vector>
#include<set>
#include<map>
#include<time.h>
using namespace std;

#include"cc.h"
const char whitespace[]=" \t\n";
const char macro[][10]={
	"#define","#if","#else","#endif","#include",
	"#ifndef","#ifdef","#elseif","#undef"
};
const char keyword[][10]={
	"char","byte","short","unsigned","signed","int","long","float","double",
	"if","else","return","break","continue","for","while","do","goto","void",
	"inline","const","enum","register","static","extern","operator","struct","typedef",
	"sizeof","typename","this","virtual","bool","false","true","public","protected","private",
	"class","typename","union","new","delete","case","switch","default"
};
const char optword[][5]={
	"+","-","++","--","*","/","%","&","|","~","^","=","<<",">>",
	"+=","-=","*=","/=","%=","&=","|=","^=","<<=",">>=","&&","||",
	">","<","!=","==",">=","<=","(",")","[","]","{","}","!","?",",",".",
	"->",":","::",";","..."
};
#define FA_MAX_NODE	1000
/*NFA*/
struct NFANode{
	int type;
	vector<int>next[128];
	void init(){
		type=LEX_NONE;
		for(int i=0;i<128;i++)
			next[i].clear();
	}
};
static NFANode nfa[FA_MAX_NODE];
static int nfa_ed;
/*DFA*/
struct DFANode{	//the node of dfa
	int type;
	int next[128];
	vector<int>stat;	/* use when translating from nfa */
	void init(){
		type=LEX_NONE;
		for(int i=0;i<128;i++)
			next[i]=-1;
		stat.clear();
	}
};
static DFANode dfa[FA_MAX_NODE];
static int dfa_ed;
void add_macro_to_nfa(int root,const char *s){
	int id,ch;
	if(s[0]==0){
		id=nfa_ed++;
		nfa[id].init();
		nfa[id].type=LEX_MACRO;
		for(ch=0;ch<128;ch++){
			if(ch=='_' || isdigit(ch) || isalpha(ch) )
				continue;
			nfa[root].next[ch].push_back(id);
		}
	}
	else{
		id=nfa_ed++;
		nfa[id].init();
		nfa[root].next[s[0]].push_back(id);
		add_macro_to_nfa(id,s+1);
	}

}
void add_keyword_to_nfa(int root,const char *s){
	int id,ch;
	if(s[0]==0){
		id=nfa_ed++;
		nfa[id].init();
		nfa[id].type=LEX_KEYWORD;
		for(ch=0;ch<128;ch++){
			if(ch=='_' || isdigit(ch) || isalpha(ch) )
				continue;
			nfa[root].next[ch].push_back(id);
		}
	}
	else{
		id=nfa_ed++;
		nfa[id].init();
		nfa[root].next[s[0]].push_back(id);
		add_keyword_to_nfa(id,s+1);
	}
}
void add_operator_to_nfa(int root,const char *s,const char *op){
int id,ch;
	if(s[0]==0){
		id=nfa_ed++;
		nfa[id].init();
		nfa[id].type=LEX_OPERATOR;
		char tmp[5];
		bool has=false;
		strncpy(tmp,op,5);
		int i,ed=strlen(tmp);
		tmp[ed+1]=0;
		for(ch=0;ch<128;ch++){
			tmp[ed]=ch;
			for(has=false,i=0;i<sizeof(optword)/sizeof(optword[0]);i++){
				if(strncmp(optword[i],tmp,ed+1)==0
					|| strncmp("//",tmp,ed+1)==0
					|| strncmp("/*",tmp,ed+1)==0 ){
					has=true;
					break;
				}
			}
			if(*(s-1)=='.' && isdigit(ch) )
				continue;
			if(has==false || ch==0 )nfa[root].next[ch].push_back(id);
		}
	}
	else{
		id=nfa_ed++;
		nfa[id].init();
		nfa[root].next[s[0]].push_back(id);
		add_operator_to_nfa(id,s+1,op);
	}
}
/*init the lexical nfa*/
bool init_nfa(){
	int i,cnt,ch,id,id2,id3;
	nfa_ed=1;
	nfa[0].init();
	/* add white space to nfa*/
	id=nfa_ed++;
	nfa[id].init();
	nfa[id].type=LEX_WS;
	for(i=0;whitespace[i];i++)
		nfa[0].next[whitespace[i]].push_back(id);
	nfa[0].next[0].push_back(id);
	/* add macro to nfa*/
	cnt=sizeof(macro)/sizeof(macro[0]);
	for(i=0;i<cnt;i++)
		add_macro_to_nfa(0,macro[i]);
	/* add keyword to nfa*/
	cnt=sizeof(keyword)/sizeof(keyword[0]);
	for(i=0;i<cnt;i++)
		add_keyword_to_nfa(0,keyword[i]);
	/* add operator to nfa*/
	cnt=sizeof(optword)/sizeof(optword[0]);
	for(i=0;i<cnt;i++)
		add_operator_to_nfa(0,optword[i],optword[i]);
	/*add id to nfa*/
	id=nfa_ed++;
	nfa[id].init();
	for(ch=' ';ch<128;ch++){
		if(ch=='_' || isalpha(ch) )
			nfa[0].next[ch].push_back(id);
	}
	id2=nfa_ed++;
	nfa[id2].init();
	nfa[id2].type=LEX_IDENT;	
	for(ch=0;ch<128;ch++)
	{
		if(isdigit(ch) || ch=='_' || isalpha(ch)){
			nfa[id].next[ch].push_back(id);
		}
		else{
			nfa[id].next[ch].push_back(id2);
		}
	}
	/* add comment to nfa*/
	id=nfa_ed++;
	nfa[id].init();
	nfa[0].next['/'].push_back(id);
	id2=nfa_ed++;
	nfa[id2].init();
	nfa[id2].type=LEX_COMMENT_S;
	nfa[id].next['/'].push_back(id2);
	id2=nfa_ed++;
	nfa[id2].init();
	nfa[id2].type=LEX_COMMENT_M;
	nfa[id].next['*'].push_back(id2);
	/* add string to nfa*/
	id=nfa_ed++;
	nfa[id].init();
	nfa[id].type=LEX_STRING;
	nfa[0].next['\"'].push_back(id);
	/*add character to nfa*/
	id=nfa_ed++;
	nfa[id].init();
	nfa[id].type=LEX_CHARACTER;
	nfa[0].next['\''].push_back(id);
	/* add integer and long to nfa*/
	id=nfa_ed++;
	nfa[id].init();
	for(ch='1';ch<='9';ch++)
		nfa[0].next[ch].push_back(id);
	id2=nfa_ed++;
	nfa[id2].init();
	nfa[id2].type=LEX_INTEGER;
	id3=nfa_ed++;
	nfa[id3].init();/*L*/
	for(ch=0;ch<128;ch++){
		if(isdigit(ch))
			nfa[id].next[ch].push_back(id);
		else if(toupper(ch)=='L')
			nfa[id].next[ch].push_back(id3);
		else if(isalpha(ch) || ch=='_' || ch=='.' )
			continue;
		else
			nfa[id].next[ch].push_back(id2);
	}
	id=nfa_ed++;
	nfa[id].init();//LL
	for(ch=0;ch<128;ch++){
		if(toupper(ch)=='L')
			nfa[id3].next[ch].push_back(id);
		if(isdigit(ch) || isalpha(ch) || ch=='_' || ch=='.')
			continue;
		else
			nfa[id3].next[ch].push_back(id2);
	}
	id2=nfa_ed++;
	nfa[id2].init();
	nfa[id2].type=LEX_LONG;
	for(ch=0;ch<128;ch++){
		if(isdigit(ch) || isalpha(ch) || ch=='_' || ch=='.')
			continue;
		else
			nfa[id].next[ch].push_back(id2);
	}
	/* add integer_0 and long_0 to nfa*/
	id=nfa_ed++;
	nfa[id].init();/*0*/
	nfa[0].next['0'].push_back(id);
	for(ch='0';ch<='7';ch++)
		nfa[id].next[ch].push_back(id);
	id2=nfa_ed++;
	nfa[id2].init();
	nfa[id2].type=LEX_INTEGER_O;
	id3=nfa_ed++;
	nfa[id3].init();/*L*/
	for(ch=0;ch<128;ch++){
		if(toupper(ch)=='L')
			nfa[id].next[ch].push_back(id3);
		else if(isdigit(ch) || isalpha(ch) || ch=='_' || ch=='.')
			continue;
		else
			nfa[id].next[ch].push_back(id2);
	}
	id=nfa_ed++;
	nfa[id].init();//LL
	for(ch=0;ch<128;ch++){
		if(toupper(ch)=='L')
			nfa[id3].next[ch].push_back(id);
		if(isdigit(ch) || isalpha(ch) || ch=='_' || ch=='.')
			continue;
		else
			nfa[id3].next[ch].push_back(id2);
	}
	id2=nfa_ed++;
	nfa[id2].init();
	nfa[id2].type=LEX_LONG_O;
	for(ch=0;ch<128;ch++){
		if(isdigit(ch) || isalpha(ch) || ch=='_' || ch=='.')
			continue;
		else
			nfa[id].next[ch].push_back(id2);
	}
	/* add integer_0x and long_0x to nfa*/
	id=nfa_ed++;
	nfa[id].init();/*0*/
	nfa[0].next['0'].push_back(id);
	id2=nfa_ed++;
	nfa[id2].init();/*0x || 0X */
	nfa[id].next['x'].push_back(id2);
	nfa[id].next['X'].push_back(id2);
	id=nfa_ed++;
	nfa[id].init();
	for(ch='0';ch<='9';ch++){
		nfa[id2].next[ch].push_back(id);
		nfa[id].next[ch].push_back(id);
	}
	for(ch='a';ch<='f';ch++){
		nfa[id2].next[ch].push_back(id);
		nfa[id2].next[toupper(ch)].push_back(id);
		nfa[id].next[ch].push_back(id);
		nfa[id].next[toupper(ch)].push_back(id);
	}
	id2=nfa_ed++;
	nfa[id2].init();
	nfa[id2].type=LEX_INTEGER_OX;
	id3=nfa_ed++;
	nfa[id3].init();/*L*/
	for(ch=0;ch<128;ch++){
		if(toupper(ch)=='L')
			nfa[id].next[ch].push_back(id3);
		else if(isdigit(ch) || isalpha(ch) || ch=='_' || ch=='.')
			continue;
		else
			nfa[id].next[ch].push_back(id2);
	}
	id=nfa_ed++;
	nfa[id].init();//LL
	for(ch=0;ch<128;ch++){
		if(toupper(ch)=='L')
			nfa[id3].next[ch].push_back(id);
		if(isdigit(ch) || isalpha(ch) || ch=='_' || ch=='.')
			continue;
		else
			nfa[id3].next[ch].push_back(id2);
	}
	id2=nfa_ed++;
	nfa[id2].init();
	nfa[id2].type=LEX_LONG_OX;
	for(ch=0;ch<128;ch++){
		if(isdigit(ch) || isalpha(ch) || ch=='_' || ch=='.')
			continue;
		else
			nfa[id].next[ch].push_back(id2);
	}
	int st[10];
	/* add real and float D. to nfa*/
	for(i=0;i<9;i++){
		st[i]=nfa_ed++;
		nfa[st[i]].init();
	}
	nfa[st[7]].type=LEX_REAL;
	nfa[st[8]].type=LEX_REAL_FLOAT;
	//digit
	for(ch='0';ch<='9';ch++){
		nfa[0].next[ch].push_back(st[0]);
		nfa[st[0]].next[ch].push_back(st[0]);
		nfa[st[1]].next[ch].push_back(st[2]);
		nfa[st[2]].next[ch].push_back(st[2]);
		nfa[st[3]].next[ch].push_back(st[5]);
		nfa[st[4]].next[ch].push_back(st[5]);
		nfa[st[5]].next[ch].push_back(st[5]);
	}
	//.
	nfa[st[0]].next['.'].push_back(st[1]);
	//e/E
	nfa[st[2]].next['e'].push_back(st[3]);
	nfa[st[2]].next['E'].push_back(st[3]);
	nfa[st[0]].next['e'].push_back(st[3]);
	nfa[st[0]].next['E'].push_back(st[3]);
	nfa[st[1]].next['e'].push_back(st[3]);
	nfa[st[1]].next['E'].push_back(st[3]);
	//f/F
	nfa[st[0]].next['f'].push_back(st[6]);
	nfa[st[0]].next['F'].push_back(st[6]);
	nfa[st[5]].next['f'].push_back(st[6]);
	nfa[st[5]].next['F'].push_back(st[6]);
	nfa[st[1]].next['f'].push_back(st[6]);
	nfa[st[1]].next['F'].push_back(st[6]);
	nfa[st[2]].next['f'].push_back(st[6]);
	nfa[st[2]].next['F'].push_back(st[6]);
	//+/-
	nfa[st[3]].next['+'].push_back(st[4]);
	nfa[st[3]].next['-'].push_back(st[4]);
	// not d nor alpha nor _
	for(ch=0;ch<128;ch++){
		if(isdigit(ch) || isalpha(ch) || ch=='_' || ch=='.')
			continue;
		nfa[st[6]].next[ch].push_back(st[8]);
		nfa[st[5]].next[ch].push_back(st[7]);
		nfa[st[1]].next[ch].push_back(st[7]);
		nfa[st[2]].next[ch].push_back(st[7]);
	}
	/* add real and float .D to nfa*/
	for(i=0;i<8;i++){
		st[i]=nfa_ed++;
		nfa[st[i]].init();
	}
	nfa[st[6]].type=LEX_REAL;
	nfa[st[7]].type=LEX_REAL_FLOAT;
	//.
	nfa[0].next['.'].push_back(st[0]);
	//digit
	for(ch='0';ch<='9';ch++){
		nfa[st[0]].next[ch].push_back(st[1]);
		nfa[st[1]].next[ch].push_back(st[1]);
		nfa[st[2]].next[ch].push_back(st[4]);
		nfa[st[3]].next[ch].push_back(st[4]);
		nfa[st[4]].next[ch].push_back(st[4]);
	}
	//e/E
	nfa[st[1]].next['e'].push_back(st[2]);
	nfa[st[1]].next['E'].push_back(st[2]);
	//f/F
	nfa[st[4]].next['f'].push_back(st[5]);
	nfa[st[4]].next['F'].push_back(st[5]);
	nfa[st[1]].next['f'].push_back(st[5]);
	nfa[st[1]].next['F'].push_back(st[5]);
	//+/-
	nfa[st[2]].next['+'].push_back(st[3]);
	nfa[st[2]].next['-'].push_back(st[3]);
	// not d nor alpha nor _
	for(ch=0;ch<128;ch++){
		if(isdigit(ch) || isalpha(ch) || ch=='_' || ch=='.')
			continue;
		nfa[st[5]].next[ch].push_back(st[7]);
		nfa[st[4]].next[ch].push_back(st[6]);
		nfa[st[1]].next[ch].push_back(st[6]);
	}

	printf("nfa node count=%d\n",nfa_ed);
	return true;
}
/* stat in dfa*/
map<string,int>statemap;
int getdfaid(vector<int>&stat){
	int i;
	string str="";
	char cmd[100];
	sort(stat.begin(),stat.end());
	for(i=0;i<(int)stat.size();i++){
		if(i && stat[i]==stat[i-1] )
			continue;
		sprintf(cmd,"_%d",stat[i]);
		str+=cmd;
	}
	i=statemap[str];
	if(i==0){
		dfa[dfa_ed].init();
		dfa[dfa_ed].stat=stat;
		statemap[str]=++dfa_ed;
	}
	i=statemap[str]-1;
	return i;
}
bool dfa_vis[FA_MAX_NODE];
/* init dfa*/
void dfa_build(int root){
	unsigned int i,j;
	int ch,id,to,nd;
	vector<int>s[128];
	if(dfa_vis[root])return;
	dfa_vis[root]=true;
	for(ch=0;ch<128;ch++)
		s[ch].clear();
	for(i=0;i<dfa[root].stat.size();i++){
		id=dfa[root].stat[i];
		if(nfa[id].type!=LEX_NONE){
			if(dfa[root].type==LEX_IDENT && nfa[id].type==LEX_KEYWORD)
				dfa[root].type=nfa[id].type;
			if(! (dfa[root].type==LEX_KEYWORD && nfa[id].type==LEX_IDENT) ){
				if(dfa[root].type!=LEX_NONE && dfa[root].type!=nfa[id].type)
					showerror("translating nfa to dfa error: multi type in one node.");
				dfa[root].type=nfa[id].type;
			}
		}
		for(ch=0;ch<128;ch++){
			for(j=0;j<nfa[id].next[ch].size();j++){
				to=nfa[id].next[ch][j];
				s[ch].push_back(to);
			}
		}
	}
	for(ch=0;ch<128;ch++){
		if(s[ch].size()==0)continue;
		nd=getdfaid(s[ch]);
		dfa[root].next[ch]=nd;
		dfa_build(dfa[root].next[ch]);
	}
}
bool init_dfa(){
	int r;
	vector<int>s;
	memset(dfa_vis,0,sizeof(dfa_vis));
	statemap.clear();
	dfa_ed=0;
	s.clear();
	s.push_back(0);
	r=getdfaid(s);
	dfa_build(r);
	printf("dfa node count=%d\n",dfa_ed);
	return true;
}
void outputDFA(const char *filename){
	int i,j,cnt;
	FILE *fp=fopen(filename,"w");
	if(fp==NULL){
		showerror("open file error.");
		return;
	}
	cnt=sizeof(dfa[0].next)/sizeof(dfa[0].next[0]);
	fprintf(fp,"%d %d\n",dfa_ed,cnt);
	/*out put dfa nodes*/
	for(i=0;i<dfa_ed;i++){
		fprintf(fp,"%d\n",dfa[i].type);
		for(j=0;j<cnt;j++)
			fprintf(fp,"%d ",dfa[i].next[j]);
		fprintf(fp,"\n");
	}
	fclose(fp);
}
/*init the lexical fa*/
bool init_fa(){
	if(init_nfa()==false)return false;
	if(init_dfa()==false)return false;
	outputDFA("lexical_dfa.dat");
	return true;
}
/*debug: find in nfa*/
char * current_point;
bool is_macro;
bool nfa_find(int r,char *s,char *p,bool newwd=true){
	current_point=NULL;
	bool ans=true;
	int ch,i;
	bool alt;
	switch(nfa[r].type)
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
		s--;
		ch=*s;
		*s=0;
		if(is_macro)printf("<macro>\t");
		printf("%-15s\t%s\n",lex_name[nfa[r].type],p);
		*s=ch;
		if(*s)current_point=s;
		break;
	case LEX_CHARACTER:
		alt=false;
		while(*s){
			if(alt==false && *s=='\'' )
				break;
			else if(*s=='\\')
				alt=!alt;
			else if(*s!='\\')
				alt=false;
			if(*s=='\n'){
				is_macro=false;
				break;
			}
			s++;
		}
		if(*s!='\'')
			ans=false;
		else{
			s++;
			ch=*s;
			*s=0;
			if(is_macro)printf("<macro>\t");
			printf("%-15s\t%s\n",lex_name[nfa[r].type],p);
			*s=ch;
			if(*s)current_point=s;
		}
		break;
	case LEX_COMMENT_S:
		s--;
		while( *s && *s!='\n' )
			s++;
		ch=*s;
		*s=0;
		is_macro=false;
		printf("%-15s\t%s\n",lex_name[nfa[r].type],p);
		*s=ch;
		if(*s)current_point=s;
		break;
	case LEX_COMMENT_M:
		alt=false;
		while(*s){
			if(alt==true && *s=='/' )
				break;
			else if(*s=='*')
				alt=true;
			else
				alt=false;
			if(*s=='\n')is_macro=false;
			s++;
		}
		if(*s!='/')
			ans=false;
		else{
			s++;
			ch=*s;
			*s=0;
			printf("%-15s\t%s\n",lex_name[nfa[r].type],p);
			*s=ch;
			if(*s)current_point=s;
		}
		break;
	case LEX_STRING:
		alt=false;
		while(*s){
			if(alt==false && *s=='\"' )
				break;
			else if(*s=='\\')
				alt=!alt;
			else if(*s!='\\')
				alt=false;
			if(*s=='\n'){
				is_macro=false;
				break;
			}
			s++;
		}
		if(*s!='\"')
			ans=false;
		else{
			s++;
			ch=*s;
			*s=0;
			if(is_macro)printf("<macro>\t");
			printf("%-15s\t%s\n",lex_name[nfa[r].type],p);
			*s=ch;
			if(*s)current_point=s;
		}
		break;
	case LEX_MACRO:
		s--;
		is_macro=true;
		ch=*s;
		*s=0;
		printf("<macro>\t\t%s\n",p);
		*s=ch;
		if(*s)current_point=s;
		break;
	case LEX_WS:
		if(*(s-1)=='\n')is_macro=false;
		if(*(s-1))
			current_point=s;
		break;
	case LEX_NONE:
		ans=false;
		for(i=0;ans==false && i<(int)nfa[r].next[*s].size();i++){
			ans|=nfa_find(nfa[r].next[*s][i],s+1,p,false);
		}
		if(ans==false && newwd){
			while(*s && *s!=' ' && *s!='\t' && *s!='\n')
				s++;
			ch=*s;
			*s=0;
			printf("lexical error:\t%s\n",p);
			*s=ch;
			if(*s)current_point=s;
		}
		break;
	}
	return ans;
}
/**debug dfa*/
bool dfa_find(int r,char *s,char *p,bool newwd=true){
	current_point=NULL;
	bool ans=true;
	int ch,i;
	bool alt;
	switch(dfa[r].type)
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
		s--;
		ch=*s;
		*s=0;
		if(is_macro)printf("<macro>\t");
		printf("%-15s\t%s\n",lex_name[dfa[r].type],p);
		*s=ch;
		if(*s)current_point=s;
		break;
	case LEX_CHARACTER:
		alt=false;
		while(*s){
			if(alt==false && *s=='\'' )
				break;
			else if(*s=='\\')
				alt=!alt;
			else if(*s!='\\')
				alt=false;
			if(*s=='\n'){
				is_macro=false;
				break;
			}
			s++;
		}
		if(*s!='\'')
			ans=false;
		else{
			s++;
			ch=*s;
			*s=0;
			if(is_macro)printf("<macro>\t");
			printf("%-15s\t%s\n",lex_name[dfa[r].type],p);
			*s=ch;
			if(*s)current_point=s;
		}
		break;
	case LEX_COMMENT_S:
		s--;
		while( *s && *s!='\n' )
			s++;
		ch=*s;
		*s=0;
		is_macro=false;
		printf("%-15s\t%s\n",lex_name[dfa[r].type],p);
		*s=ch;
		if(*s)current_point=s;
		break;
	case LEX_COMMENT_M:
		alt=false;
		while(*s){
			if(alt==true && *s=='/' )
				break;
			else if(*s=='*')
				alt=true;
			else
				alt=false;
			if(*s=='\n')is_macro=false;
			s++;
		}
		if(*s!='/')
			ans=false;
		else{
			s++;
			ch=*s;
			*s=0;
			printf("%-15s\t%s\n",lex_name[dfa[r].type],p);
			*s=ch;
			if(*s)current_point=s;
		}
		break;
	case LEX_STRING:
		alt=false;
		while(*s){
			if(alt==false && *s=='\"' )
				break;
			else if(*s=='\\')
				alt=!alt;
			else if(*s!='\\')
				alt=false;
			if(*s=='\n'){
				is_macro=false;
				break;
			}
			s++;
		}
		if(*s!='\"')
			ans=false;
		else{
			s++;
			ch=*s;
			*s=0;
			if(is_macro)printf("<macro>\t");
			printf("%-15s\t%s\n",lex_name[dfa[r].type],p);
			*s=ch;
			if(*s)current_point=s;
		}
		break;
	case LEX_MACRO:
		s--;
		is_macro=true;
		ch=*s;
		*s=0;
		printf("<macro>\t\t%s\n",p);
		*s=ch;
		if(*s)current_point=s;
		break;
	case LEX_WS:
		if(*(s-1)=='\n')is_macro=false;
		if(*(s-1))
			current_point=s;
		break;
	case LEX_NONE:
		ans=false;
		//for(i=0;ans==false && i<(int)dfa[r].next[*s].size();i++){
		//	ans|=dfa_find(dfa[r].next[*s][i],s+1,p,false);
		//}
		if(dfa[r].next[*s]>=0)
			ans=dfa_find(dfa[r].next[*s],s+1,p,false);
		if(ans==false && newwd){
			while(*s && *s!=' ' && *s!='\t' && *s!='\n')
				s++;
			ch=*s;
			*s=0;
			printf("lexical error:\t%s\n",p);
			*s=ch;
			if(*s)current_point=s;
		}
		break;
	}
	return ans;
}

char cmd[655350];
int main(int argc,char **argv){
	freopen("in.txt","r",stdin);
	time_t ti=clock();
	printf("keyword count is:%d\noperator count is:%d\n",
		sizeof(keyword)/sizeof(keyword[0]),
		sizeof(optword)/sizeof(optword[0]) );
	printf("init the lexical fa...\n");
	if(init_fa()==false){
		showerror("build the lexical fa error!");
		return -1;
	}
	printf("building time: %d ms\n",clock()-ti);
	char ch;
	int i;
	while(true){
		ch=getchar();
		if(ch==EOF)break;
		cmd[0]=ch;
		for(i=1;(ch=getchar())!=EOF;i++)
			cmd[i]=ch;
		cmd[i]=0;
		freopen("out_nfa.txt","w",stdout);
		ti=clock();
		for(current_point=cmd;current_point;)
			nfa_find(0,current_point,current_point);
		fprintf(stderr,"nfa lexical use time: %d ms\n",clock()-ti);
		freopen("out_dfa.txt","w",stdout);
		ti=clock();
		for(current_point=cmd;current_point;)
			dfa_find(0,current_point,current_point);
		fprintf(stderr,"dfa lexical use time: %d ms\n",clock()-ti);
	}
	return 0;
}