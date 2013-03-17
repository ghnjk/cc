/*
c complier  - LRmaker
author:ghnjk
time:2013-3
*/
#include"cc.h"
#include<string>
#include<iostream>
#include<strstream>
#include<vector>
#include<set>
#include<iterator>
#include<stdio.h>
#include<map>
using namespace std;
map<string,int>signmap;
int getSymbolId(string &name);
class GrammerSymbol{			///文法符号
public:
	vector< vector<int> > pros;	//产生式列表
	int id;						//符号ID
	string name;				//符号名称
	bool isTerminal;			//是否为终结符
	set<int>		first;		//first集合
	set<int>		follow;		//follow集合
	int color;					//生成时用的变量
	bool scanf(){
		string buf;
		char str[2048];
		isTerminal=false;
		while(cin>>buf && buf!=";"){
			cin.getline(str,2048);
			istrstream sin(str);
			pros.push_back(vector<int>() );
			while(sin>>buf){
				int i=getSymbolId(buf);
				pros.back().push_back(i);
			}
		}
		return true;
	}
	void printf();
};
#define MAX_GRAMMER_SYMBOL 500
GrammerSymbol grammer[MAX_GRAMMER_SYMBOL];
int epsilonID,eofID;
void GrammerSymbol::printf(){
	cout<<"GrammerSymbol "<<id<<" : "<<name<<endl;
	cout<<"First Set:"<<endl;
	int i,j;
	set<int>::iterator iter;
	for(iter=first.begin();iter!=first.end();iter++)
		cout<<" "<<grammer[*iter].name;
	cout<<endl;
	cout<<"Follow Set:"<<endl;
	for(iter=follow.begin();iter!=follow.end();++iter)
		cout<<" "<<grammer[*iter].name;
	cout<<endl;
	if(isTerminal){
		cout<<"Terminate Symbol."<<endl;
		return;
	}
	cout<<"Productions: "<<endl;
	for(i=0;i<pros.size();i++){
		cout<<"\t -> \t";
		for(j=0;j<pros[i].size();j++)
			cout<<" "<<grammer[pros[i][j]].name;
		cout<<endl;
	}
}
int getSymbolId(string &name){
	int id;
	if(signmap.find(name)==signmap.end()){
		id=signmap.size();
		signmap[name]=id;
		grammer[id].id=id;
		grammer[id].name=name;
		grammer[id].isTerminal=true;
		grammer[id].pros.clear();
		grammer[id].first.clear();
		grammer[id].follow.clear();
	}
	else
		id=signmap[name];
	return id;
}
bool genFirst(int r){
	bool ans=false;
	if(grammer[r].color)return ans;
	grammer[r].color=1;
	set<int>::iterator iter;
	int i,j,bsize,k;
	bsize=grammer[r].first.size();
	if(grammer[r].isTerminal){
		grammer[r].first.insert(grammer[r].id);
	}
	else{
		for(i=0;i<grammer[r].pros.size();i++){
			if(grammer[r].pros[i].size()==0)continue;
			if(grammer[r].pros[i].size()==1 && grammer[r].pros[i][0]==epsilonID){
				grammer[r].first.insert(epsilonID);
				continue;
			}
			for(j=0;j<grammer[r].pros[i].size();j++){
				k=grammer[r].pros[i][j];
				ans|=genFirst(k);
				for(iter=grammer[k].first.begin();iter!=grammer[k].first.end();++iter)
				{
					if(*iter==epsilonID)continue;
					grammer[r].first.insert(*iter);
				}
				if(grammer[k].first.find(epsilonID)==grammer[k].first.end())break;
			}
			if(j>=grammer[r].pros[i].size())grammer[r].first.insert(epsilonID);
		}
	}
	if(bsize<grammer[r].first.size())
		ans|=true;
	return ans;
}
bool genFollow(){
	int i,j,k,t,allcnt=0,nowcnt=0;
	set<int>::iterator iter;
	for(i=0;i<signmap.size();i++)
		allcnt+=grammer[i].follow.size();
	vector<int> * pVec;
	///
	///为开始非终结符添加EOF标记
	grammer[eofID+1].follow.insert(eofID);
	///
	for(int ii=0;ii<signmap.size();ii++){
		for(int jj=0;jj<grammer[ii].pros.size();jj++){
			pVec=&(grammer[ii].pros[jj]);
			for(i=0;i<pVec->size();i++){
				k=(*pVec)[i];
				if(grammer[k].isTerminal)continue;
				for(j=i+1;j<pVec->size();j++){
					t=(*pVec)[j];
					for(iter=grammer[t].first.begin();iter!=grammer[t].first.end();++iter)
						grammer[k].follow.insert(*iter);
					if(grammer[t].first.find(epsilonID)==grammer[t].first.end())break;
				}
				if(j>=pVec->size()){
					for(iter=grammer[ii].follow.begin();iter!=grammer[ii].follow.end();++iter)
						grammer[k].follow.insert(*iter);
				}
				grammer[k].follow.erase(epsilonID);
			}
		}
	}
	for(i=0;i<signmap.size();i++)
		nowcnt+=grammer[i].follow.size();
	return nowcnt>allcnt;
}
void initGrammer(){
	int i;
	bool flag=true;
	while(flag){
		flag=false;
		for(i=0;i<signmap.size();i++)
			grammer[i].color=0;
		for(i=0;i<signmap.size();i++)
			flag|=genFirst(i);
	}
	while(genFollow())
		;
}
int main(int argc,char **argv){
	signmap.clear();
	epsilonID=getSymbolId((string)"<<epsilon>>");
	eofID=getSymbolId((string)"<<EOF>>");
	string buf;
	int i;
	while(cin>>buf){
		i=getSymbolId(buf);
		grammer[i].scanf();
	}
	initGrammer();
	for(i=0;i<signmap.size();i++)
		grammer[i].printf();
	return 0;
}