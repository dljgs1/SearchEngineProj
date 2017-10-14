#include"..\format\gb2312.h"
#include"..\io\reader.h"
#include"term.h"
#include"..\marco.h"
#pragma once

#include<vector>
using std::vector;

//倒排索引: 
class Cinverted_table{
	friend class Cclass_data;
private:
	gb2312_mat mesh;
	vector<record_table> terms;
	int doc_count;
	//?
	vector<record_table> mem_tb;//记忆查询的结果
	vector<string>mem_str;
public:
	Cinverted_table():doc_count(0){
		mem_tb.push_back(record_table("找不到！"));//记忆表第一项为空表 表示空查询
	}
	bool cut_sentence_load(Creader&reader, int doc_id = -1){//改写doc_id规则，划分句为文档，并存储句中偏移情况 返回句划分情况
		if (doc_id < 0)
			doc_id = doc_count;
		string buf;
		int bias = 0;
		int ct = terms.size();//当前表数目
		while (!reader.end()){
			reader >> buf;
		}
	}

	//SPIMI算法
	bool load(Creader&reader,int doc_id = -1,cut_fun pf= nullptr){//需以输入的数据建表 并指定所在文档（默认以自动计数作为文档号）
		if (doc_id < 0)
			doc_id = doc_count;
		string buf;
		int bias = 0;
		int ct = terms.size();//当前表数目
		while (!reader.end()){
			int id;//字号

			term_record temp;
			if (!pf){
				reader >> buf;
				temp = { bias, doc_id };
			}
			else{
				buf = pf(reader, temp);
			}
			if (buf == "")
				continue;
			if (mesh.find(buf) != mesh.end()){
				id = mesh[buf];
				terms[id].push_back(temp);
			}
			else{
				id = ct++;
				mesh[buf] = id;
				terms.push_back(record_table(buf));
				terms.back().push_back(temp);
			}
			bias++;
		}
		doc_count++;
		return true;
	}
	record_table*search_char(string s){
		if (mesh.find(s) == mesh.end()){
			return nullptr;
		}
		return &terms[mesh[s]];
	}
	record_table*search_string(string s,int k = 1){//k距查找
		for (int i = 0; i < mem_tb.size(); i++){
			if (mem_tb[i].token == s)return &mem_tb[i-1];
		}
		gb2312_str str(s);
		vector<record_table*>ptrs;
		record_table result(s);

		for (int i = 0; i < str.length(); i++){
			ptrs.push_back(search_char(str[i]));
		}
		if (ptrs.size() < 2)return ptrs[0];
		//通过合并倒排表查词:排序方法 将文档号和偏移量数字化：32bit = [文档号 偏移量][31:16 15:0]（前提：文档不超过6w 增加则需要64位）进行整数排序\基数排序
		int wide = 1;
		//restemp.push_back(record_table(ptrs[0]->token));//先放入第一个空表
		for (int i = 0; i < ptrs.size() - 1; i++){
			record_table restemp(s);
			if (ptrs[i] && ptrs[i + 1]){
				bool flag;
				int pc1 = 0, pc2 = 0;
				auto&tb1 = ptrs[i]->table;
				auto&tb2 = ptrs[i + 1]->table;
				auto&pos1 = ptrs[i]->doc_index;
				auto&pos2 = ptrs[i + 1]->doc_index;

				int fdcount = 0;
				do{
					flag = false;
					//对齐doc_id
					for (; pc1 < pos1.size() && pc2<pos2.size();){
						int id1 = tb1[pos1[pc1]].doc_id;
						int id2 = tb2[pos2[pc2]].doc_id;
						if (id1>id2){
							pc2++;
						}
						else if (id1 < id2){
							pc1++;
						}
						else{
							flag = true;//尚未查完
							break;
						}
					}

					if (flag){
						int st1 = pos1[pc1];
						int st2 = pos2[pc2];
						int ed1 = pc1 + 1 >= pos1.size() ? tb1.size() : pos1[pc1 + 1];
						int ed2 = pc2 + 1 >= pos2.size() ? tb2.size() : pos2[pc2 + 1];

						while (st1 < ed1 &&st2 < ed2){
							if (/*abs(*/tb2[st2].bias_pos - tb1[st1].bias_pos/*)*/ == k /*&& tb2[st2].bias_pos - tb1[st1].bias_pos!=0*/){
								term_record term = { tb2[st2].bias_pos, tb2[st2].doc_id };//应保存第二项的信息
								restemp.push_back(term);
								//保存一条结果
								st1++; st2++;
								fdcount++;
							}
							else{
								tb2[st2].bias_pos > tb1[st1].bias_pos ? st1++ : st2++;
							}
						}
						//!用完后增加
						pc2++;
						pc1++;
					}
					else{
						//wide++;//未在同一doc找到 
					}
				} while (flag);
				//一对查完：
				//!要增加模糊查找的选项在此做决定（如同音）
				if (fdcount == 0){
					result = mem_tb[0];//k近查找无结果
					break;
				}
				if (i + 1 < ptrs.size()){
					result = restemp;
					restemp = record_table(s);
					ptrs[i + 1] = &result;//以此次结果为下次的条件
				}
				else{
					result = restemp;
				}
			}
			else{
				restemp = mem_tb[0];//单字查找无结果
				break;
			}
		}
		//处理结果：
		mem_str.push_back(s);
		mem_tb.push_back(result);
		for (int i = 0; i< mem_tb.back().table.size();i++)
		{
			auto&var = mem_tb.back().table[i];
			var.bias_pos -= (str.length() - 1)*k;
		}
		//cout << str.length() << " " << endl;
		return &mem_tb.back();
	}

	record_table*old_search(string&buf,int recount){
		int pos = mem_str.size() - recount;
		if (pos > 0){
			buf = mem_str[pos - 1];
			return &mem_tb[pos];
		}
		return nullptr;
	}
	gb2312_mat&get_word_mesh(){
		return mesh;
	}


	int size(){
		terms[0] = record_table("");//?
		return terms.size();
	}



	void data_save(ofstream&fout){
		sl::mem_save(fout, mesh);
		int len = terms.size();
		sl::mem_save(fout, len);
		for (int i = 0; i < len; i++){
			terms[i].data_save(fout);
		}
		sl::mem_save(fout, doc_count);
		sl::mem_save(fout, mem_str);

		len = mem_tb.size();
		sl::mem_save(fout, len);
		for (int i = 0; i < len; i++){
			mem_tb[i].data_save(fout);
		}

		/*int len[4];
		len[0] = terms.size();
		len[1] = doc_count;
		len[2] = mem_tb.size();
		len[3] = mem_str.size();
		MEM_WRITE(fout, len, 4 * sizeof(int));
		MEM_WRITE(fout, &mesh, sizeof(gb2312_mat));
		for (int i = 0; i < len[0]; i++){
			terms[i].data_save(fout);
		}
		for (int i = 0; i < len[2]; i++){
			mem_tb[i].data_save(fout);
		}
		for (int i = 0; i < len[3]; i++){
			int sz = mem_str[i].size();
			MEM_WRITE(fout, &sz, sizeof(int));
			fout.write(mem_str[i].c_str(), sizeof(char)* sz);
		}*/

	}
	void data_load(ifstream&fin){
		sl::mem_load(fin, mesh);
		int len;
		sl::mem_load(fin, len);
		terms = vector<record_table>(len);
		for (int i = 0; i < len; i++){
			terms[i].data_load(fin);
		}
		sl::mem_load(fin, doc_count);
		sl::mem_load(fin, mem_str);

		sl::mem_load(fin, len);
		mem_tb = vector<record_table>(len);
		for (int i = 0; i < len; i++){
			mem_tb[i].data_load(fin);
		}
		/*int len[4];
		MEM_READ(fin, len, 4*sizeof(int));
		MEM_READ(fin, &mesh, sizeof(gb2312_mat));

		terms = vector<record_table>(len[0], record_table());
		for (int i = 0; i < len[0]; i++){
			terms[i].data_load(fin);
		}
		doc_count = len[1];
		mem_tb = vector<record_table>(len[2], record_table());
		for (int i = 0; i < len[2]; i++){
			mem_tb[i].data_load(fin);
		}

		for (int i = 0; i < len[3]; i++){
			int sz;
			char buf[128];
			MEM_READ(fin, &sz, sizeof(int));
			MEM_READ(fin, buf, sizeof(char)* sz);
			mem_str.push_back(string(buf));
		}*/
		
	}
};
