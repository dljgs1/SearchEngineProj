#pragma once
//核心代码与其他文件均有依赖关系


#include "..\format\gb2312.h"
#include "..\io\filter.h"
#include "..\io\reader.h"


#include<map>
using std::map;
//分析器
class Canalysis{
private:
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
	}word_info;//表示一个字的拓扑信息  0~99 100~199 200~299

	typedef struct wd_term{
		int bia_pos;
		int doc_id; //(可用基数排序 先排偏置后排文档号)
		wd_term(int b, int d = 0) :bia_pos(b), doc_id(d){ ; }
	}term_info;//词项信息(位置、文档号等等)
	typedef int text;
	typedef struct{
		vector<term_info> info;
		inline void push_back(term_info&tm){ info.push_back(tm); }
	}doc_table;//倒排表 暂时用vec 考虑用heap

private:
	//----前向索引 ： 从文档/片段 到字/词
	gb2312_mat mesh;
	vector<word_info> winfo_table;
	vector<string> word_table;
	vector<string> ban_word;//禁用词

public:
	Canalysis::Canalysis(){//需要加入对数据的读取?
		   ;
	}
	//建立一次拓扑
	void w_index(Creader&fin,int doc_id){
		string wd = fin.get_word();
		//init
		int last_id;//!实现一次连接的关键
		if (mesh.find(wd) != mesh.end()){
			last_id = mesh[wd];
		}
		else{
			mesh[wd] = word_table.size();
			last_id = word_table.size();
			word_table.push_back(wd);
			winfo_table.push_back(Canalysis::word_info());
		}

		for (int i = 1, pos = 1;; pos++){//pos代表当前文档位置
			wd = fin.get_word();
			if (mesh.find(wd) != mesh.end()){
				int temp = mesh[wd];//当前文字的id
				winfo_table[last_id].add(temp);//
				last_id = temp;
			}
			else{//
				mesh[wd] = i;
				winfo_table[last_id].add(i);
				winfo_table.push_back(word_info());
				last_id = i;
			}
			word_table.push_back(wd);
			i++;
		}
	}
	void w_findover(int);//查找出现频率超过一定次数的词组

};

//新：


