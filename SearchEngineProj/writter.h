#include<iostream>
#include<fstream>
#include<windows.h>
#include<string>
#include<map>
#include<set>
#include<vector>
using namespace std;
#ifndef MYWRITTER
#define MYWRITTER
//对中文txt文本进行分析



//新字元：表示以GB2313编码的字符
class Cnewchar{
private:
	char szC[3];
	int type;
public:
	Cnewchar();
	Cnewchar(string str);
	bool operator==(const Cnewchar&);
	bool operator==(const string&);
	int get_byte_num(){ return type; }
	const char* get_caddr(){ return szC; }
	string get_str();
};


//汉字读取模块：可读取txt 返回单字（包括字符、字母）
class Creader{
private:
	ifstream fin;
	string buffer;
	int cur;
public:
	Creader(string fname);
	~Creader();
	string get_word();
	Cnewchar get_newchar();
	bool flush_new_line();
	const Creader& operator>>(string&str){ str = get_word(); return *this; }//default inline
	bool end();
};
//计数器:以一个特征为key 对其进行计数统计

template<typename T>
class Ccounter{
private:
	map<T, int> mesh;
public:
	void clear(const T&key){ mesh.erase(key); }
	void minus(const T&key){ touch_set(key, 0); mesh[key]--; }
	void plus(const T&key){ touch_set(key, 0); mesh[key]++; }
	void set(const T&key, int val){ mesh[key] = val; }
	void touch_set(const T&key, int val){ if (isthere(key))return; else set(key, val); }
	bool isthere(const T&key){ return mesh.find(key) != mesh.end(); }
	int get_time(const T&key){ return mesh[key]; }
	int get_key_num(){ return mesh.size(); }
	void output(ofstream&fout){
		for each (auto&var in mesh){
			fout << var.first << "\t" << var.second << endl;
		}
	}
};


//过滤器: 滤除匹配的模式
class Cfilter{
	enum ftype{
		OneTime = '|',
		OneMore = '*',
		Nothing = ' '
	};
	typedef struct{
		vector<string> wds;
		ftype type;
		bool match(const string&wd){ return find(wds.begin(), wds.end(), wd) != wds.end(); }
	}filter_info;
	vector<filter_info> info;
	char mode;//工作方式?
public:
	string buffer;//缓存（必须）
	Cfilter(string,char);
	bool match(Creader&);
	bool remove(string&);
};

//分析器
#include "gb2312.h"
class Canalysis{
public:
//结构
	typedef struct{
		vector<int> connect_id;//其后连接的字号
		vector<int> times;//出现次数
		void add(int id){
			auto ptr = find(connect_id.begin(), connect_id.end(), id);
			if (ptr == connect_id.end()){
				connect_id.push_back(id);
				times.push_back(1);
			}
			else{
				times[ptr - connect_id.begin()]++;
			}
		}
	}word_info;//表示字的拓扑信息
	typedef struct wd_term{
		int bia_pos;
		int doc_id; //(可用基数排序 先排偏置后排文档号)
		wd_term(int b,int d = 0):bia_pos(b),doc_id(d){ ; }
	}term_info;//词项信息(位置、文档号等等)
	typedef int text;
	typedef struct{
		vector<term_info> info;
		inline void push_back(term_info&tm){ info.push_back(tm); }
	}doc_table;//倒排表 暂时用vec 考虑用heap

private:
	Creader fin;
	Ccounter<string> ct;

	//----倒排索引：建立词/字和文档/片段之间的联系
	map<string,vector<text>> is_table;//倒排表

	gb2312_mat inv_table_mesh;//新倒排表-index
	vector<doc_table> terms_doc_table;//新倒排表-table

	//----前向索引 ： 从文档/片段 到字/词
	//----improve(划掉 对词信息进行收集
	//map<string, int> windex_table;
	gb2312_mat windex_table;
	vector<word_info> winfo_table;
	vector<string> word_table;

	vector<string> ban_word;//禁用词


public:
	Canalysis(string fname);
	void ana_ct(int time);
	void output(string fname);

	void inverted_index(int,int);//倒排索引 按一定字数将其分段

	void inverted_index(string,int, int);//添加滤波分割的倒排索引
	
	int inverted_and(vector<string>&);

	void w_index(int,int);//建立词组之间一次关系的索引
	void w_findover(int);//查找出现频率超过一定次数的词组
};



int writter_main();


#endif