#include"..\format\gb2312.h"
#include"..\io\reader.h"
#include"term.h"
#include"..\marco.h"
#pragma once

#include<vector>
using std::vector;

//��������: 
class Cinverted_table{
	friend class Cclass_data;
private:
	gb2312_mat mesh;
	vector<record_table> terms;
	int doc_count;
	//?
	vector<record_table> mem_tb;//�����ѯ�Ľ��
	vector<string>mem_str;
public:
	Cinverted_table():doc_count(0){
		mem_tb.push_back(record_table("�Ҳ�����"));//������һ��Ϊ�ձ� ��ʾ�ղ�ѯ
	}
	bool cut_sentence_load(Creader&reader, int doc_id = -1){//��дdoc_id���򣬻��־�Ϊ�ĵ������洢����ƫ����� ���ؾ仮�����
		if (doc_id < 0)
			doc_id = doc_count;
		string buf;
		int bias = 0;
		int ct = terms.size();//��ǰ����Ŀ
		while (!reader.end()){
			reader >> buf;
		}
	}

	//SPIMI�㷨
	bool load(Creader&reader,int doc_id = -1,cut_fun pf= nullptr){//������������ݽ��� ��ָ�������ĵ���Ĭ�����Զ�������Ϊ�ĵ��ţ�
		if (doc_id < 0)
			doc_id = doc_count;
		string buf;
		int bias = 0;
		int ct = terms.size();//��ǰ����Ŀ
		while (!reader.end()){
			int id;//�ֺ�

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
	record_table*search_string(string s,int k = 1){//k�����
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
		//ͨ���ϲ����ű���:���򷽷� ���ĵ��ź�ƫ�������ֻ���32bit = [�ĵ��� ƫ����][31:16 15:0]��ǰ�᣺�ĵ�������6w ��������Ҫ64λ��������������\��������
		int wide = 1;
		//restemp.push_back(record_table(ptrs[0]->token));//�ȷ����һ���ձ�
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
					//����doc_id
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
							flag = true;//��δ����
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
								term_record term = { tb2[st2].bias_pos, tb2[st2].doc_id };//Ӧ����ڶ������Ϣ
								restemp.push_back(term);
								//����һ�����
								st1++; st2++;
								fdcount++;
							}
							else{
								tb2[st2].bias_pos > tb1[st1].bias_pos ? st1++ : st2++;
							}
						}
						//!���������
						pc2++;
						pc1++;
					}
					else{
						//wide++;//δ��ͬһdoc�ҵ� 
					}
				} while (flag);
				//һ�Բ��꣺
				//!Ҫ����ģ�����ҵ�ѡ���ڴ�����������ͬ����
				if (fdcount == 0){
					result = mem_tb[0];//k�������޽��
					break;
				}
				if (i + 1 < ptrs.size()){
					result = restemp;
					restemp = record_table(s);
					ptrs[i + 1] = &result;//�Դ˴ν��Ϊ�´ε�����
				}
				else{
					result = restemp;
				}
			}
			else{
				restemp = mem_tb[0];//���ֲ����޽��
				break;
			}
		}
		//��������
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
