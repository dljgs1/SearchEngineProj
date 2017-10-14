//level 3
#pragma once
//核心代码与其他文件均有依赖关系

#include "..\format\gb2312.h"
#include "..\io\filter.h"
#include "..\io\reader.h"
#include "..\mem\commonfun.h"
#include "..\marco.h"

#include<map>
#include<functional>
using std::map;
//分析器
class Canalysis{
private:
	//树形结构
	typedef struct wdif{
		int id;//最初读进来时的id
		string token;
		vector<int> connect_id;//其后连接的字号

		vector<int> times;//出现次数 词频
		int sum = 0;//总出现次数 字频
		wdif(){ ; }
		wdif(string tk):token(tk){ ; }
		void add(int id){
			auto ptr = find(connect_id.begin(), connect_id.end(), id);
			if (ptr == connect_id.end()){
				connect_id.push_back(id);
				times.push_back(1);
			}
			else{
				times[ptr - connect_id.begin()]++;
			}
			sum++;
		}
		void sort(int flag = -1){
			vector<int> temp(times.size());
			for (int i = 0; i < temp.size(); i++)
				temp[i] = i;
			auto&tm = times;
			auto&cnt = connect_id;
			::sort(temp.begin(), temp.end(), [tm, flag](int l, int r){return flag*tm[l] < flag*tm[r]; });
			//auto fun = [temp, flag](int l, int r){return flag*temp[l] < flag*temp[r]; };
			vector<int> cnt_new(cnt.size());
			for (int i = 0; i < cnt.size(); i++)
				cnt_new[i] = cnt[temp[i]];
			vector<int> tm_new(tm.size());
			for (int i = 0; i < tm.size(); i++)
				tm_new[i] = tm[temp[i]];
			cnt = cnt_new;
			tm = tm_new;
		}
	//sl:
		void data_save(ofstream&fout){
			sl::mem_save(fout, id);
			sl::mem_save(fout, token);
			sl::mem_save(fout, connect_id);
			sl::mem_save(fout, times);
			sl::mem_save(fout, sum);
		}
		void data_load(ifstream&fin){
			sl::mem_load(fin, id);
			sl::mem_load(fin, token);
			sl::mem_load(fin, connect_id);
			sl::mem_load(fin, times);
			sl::mem_load(fin, sum);
		}
	}word_info;//表示一个字的拓扑信息  0~99 100~199 200~299

private:
	//----前向索引 ： 从文档/片段 到字/词
	gb2312_mat mesh;
	vector<word_info> winfo_table;
	vector<int> sort_table;

	vector<string> ban_word;//禁用词 暂时不用


	
	//分词 ： 用统计方法完成分词 并找出关键词
	gb2312_mat ban_words;
	const string ban_path = "data\\sentence_config.txt";

	//----聚类分析 : 以关键词为特征 建立特征向量
	float* *feature_vec;
	int vec_len = 0;
	int doc_num = 0;


public:
	Canalysis::Canalysis(){//需要加入对数据的读取?——由数据管理中心提供数据, 从改进Creader着手
		;
	}
	void init(){//初始化
		Creader rd(ban_path);
		string buf;
		while (!rd.end()){
			rd >> buf;
			ban_words[buf] = 1;
		}
	}



	void build_cluster_vec(Creader&fin, int doc_id){
		doc_num++;
	}


	//建立双词索引
	void build_index(Creader&fin,int doc_id){
		//init
		int last_id  = - 1;//!实现一次连接的关键
		int pos = 0;
		do{
			string wd = fin.get_word();
			if (ban_words[wd] == 1){
				last_id = -1;//中断记词
				continue;
			}
			int cur_id = -1;
			if (mesh.find(wd) == mesh.end()){
				mesh[wd] = cur_id = winfo_table.size();

				sort_table.push_back(winfo_table.size());
				winfo_table.push_back(Canalysis::word_info(wd));
			}
			if (last_id >= 0){
				winfo_table[last_id].add(cur_id = mesh[wd]);
			}
			last_id = cur_id;
		} while (!fin.end());
	}

	void sort(int flag = -1){//按照词频排序 先排字后排词 用排序表表示其顺序 默认降序排
		//! sort自定义函数中不可包含结构体 否则会出错
		vector<int>temp(winfo_table.size());
		for (int i = 0; i < temp.size(); i++)
			temp[i] = winfo_table[i].sum;
		::sort(sort_table.begin(), sort_table.end(), [temp,flag](int a, int b){return flag*temp[a] < flag*temp[b]; });
		for (int i = 0; i < winfo_table.size(); i++){
			cout << i << " ";
			winfo_table[i].sort(flag);
		}
	}


	string show(int i){//按排序
		stringstream ss;
		auto&goal = winfo_table[sort_table[i]];
		ss << goal.token << ":" << endl;
		for (int i = goal.connect_id.size()-1; i>=0 ; i--){
			//if (i>10 && goal.times[i] == 1)break;
			ss << goal.token << winfo_table[goal.connect_id[i]].token << ":" << goal.times[i] << endl;
		}
		return ss.str();
	}
//=======
	void data_save(ofstream&fout){
		sl::mem_save(fout, mesh);
		int len = winfo_table.size();
		sl::mem_save(fout, len);
		for (int i = 0; i < len; i++){
			winfo_table[i].data_save(fout);
		}
		sl::mem_save(fout, sort_table);

		sl::mem_save(fout, ban_words);
	}
	void data_load(ifstream&fin){
		sl::mem_load(fin, mesh);
		int len;
		sl::mem_load(fin, len);
		winfo_table = vector<word_info>(len);
		for (int i = 0; i < len; i++){
			winfo_table[i].data_load(fin);
		}
		sl::mem_load(fin, sort_table);

		sl::mem_load(fin, ban_words);
	}
};

//新：


