#pragma once

#include"..\mem\commonfun.h"
//��������ݽṹ
#include<string>
#include<vector>
#include<sstream>
#include<functional>
using std::stringstream;
using std::vector;
using std::string;
using namespace std::placeholders;
//����ĵ��ż�¼�ṹ
struct term_record{
	int bias_pos;//ƫ��λ��
	int doc_id;//�����ĵ����
};
//���ż�¼����ṹ
struct record_table{//���ڷǷֲ�ʽ���ݶ��룬���ݱ�Ȼ�������
	string token;
	vector<term_record> table;
	vector<int>doc_index;
	//vector<int>word_feq;//�ڸ����ĵ��еĳ���Ƶ��
	record_table(){ ; }
	record_table(string name):token(name){ ; }
	void push_back(const term_record&term){
		if (doc_index.size()){
			if (table[doc_index.back()].doc_id != term.doc_id){
				doc_index.push_back(table.size());
			}
		}
		else{
			doc_index.push_back(0);//��һ���ĵ�����
		}
		table.push_back(term);
	}
	string show(){
		stringstream buf;
		buf << token << ":" << endl;
		for (int i = 0; i < table.size(); i++){
			buf << "\t" << table[i].doc_id << " " << table[i].bias_pos << endl;
		}
		return buf.str();
	}
	term_record&operator[](int i){
		return table[i];
	}

	void clear(){
		token.clear();
		table.clear();
		doc_index.clear();
	}
	void data_save(ofstream&fout){
		int len[3];
		len[0] = token.length();
		len[1] = table.size();
		len[2] = doc_index.size();
		fout.write((char*)len, sizeof(int)* 3);
		fout.write(token.c_str(), len[0] * sizeof(char));
		for (int i = 0; i < len[1]; i++){
			fout.write((char*)&table[i], sizeof(term_record));
		}
		for (int i = 0; i < len[2]; i++){
			fout.write((char*)&doc_index[i], sizeof(int));
		}
	}
	void data_load(ifstream&fin){
		int len[3];
		fin.read((char*)len, sizeof(int)* 3);
		char c_str[128];
		fin.read(c_str, len[0] * sizeof(char));
		token = string(c_str);

		table = vector<term_record>(len[1], term_record());
		for (int i = 0; i < len[1]; i++){
			fin.read((char*)&table[i], sizeof(term_record));
		}
		doc_index = vector<int>(len[1], int());
		for (int i = 0; i < len[2]; i++){
			fin.read((char*)&doc_index[i], sizeof(int));
		}
	}

};


typedef std::function<string(Creader&, term_record&)> cut_fun;//���ӵĻ��ֺ���
//���ĵ�����: ���ĵ��־䣬�Ծ�����Ϊ�ĵ��������ڲ鵽������ٲ��������ĵ�
class Csentence{
	gb2312_mat mesh;//�ָ�̽���
	int doc_count = -1;
	vector<int>bounds;//�洢����߽�

	//--һ����temp
	int sen_id_count = 0;
	int bias_count = 0;
	const string path = "data\\sentence_config.txt";
public:
	Csentence(){
		;
	}
	void init(){
		Creader rd(path);
		string buf;
		while (!rd.end()){
			rd >> buf;
			mesh[buf] = 1;
		}
		new_doc();//����"����" ��ʱ�ɶ����
	}
	//--------build method------
	void new_doc(){
		bounds.push_back(sen_id_count/* + (doc_count>=0 ? 1 : 0)*/);//�߽�������ҿ��ģ�[left, right)
		doc_count++;
	}
	string cut(Creader&rd, term_record&tb){//�ָ�ֻ���ڲ��� �ⲿֻ��õ���һ������
		string temp;
		static bool last_cut = false;
		rd >> temp;
		while(mesh[temp] > 0){//��Ҫ���ָ�
			if (last_cut){
				last_cut = false;
				bias_count = 0;
				sen_id_count++;
			}
			if (rd.end())return "";
			rd >> temp;
		}
		last_cut = true;
		tb.bias_pos = bias_count++;
		tb.doc_id = sen_id_count;
		return temp;
	}
	//---------------------------------
	int get_doc_id(int sen_id){//ͨ�������ѯ�ĵ��� ���ַ�����
		int pos = (doc_count+1) / 2;
		int l = 0;
		int r = doc_count;
		while (1){
			if (pos > l && pos < r){
				if (sen_id >= bounds[pos - 1] && sen_id < bounds[pos]){
					return pos - 1;
				}
				if (sen_id < bounds[pos - 1]){//����
					r = pos;
				}
				else if(sen_id >= bounds[pos]){//����
					l = pos;
				}
				pos = (l + r) / 2;
			}
			else if (pos == l && r < doc_count){
				if (sen_id >= bounds[1] && sen_id < bounds[r]){
					return l;
				}
			}
			else if (pos == r - 1 && l >= 0){
				if (sen_id >= bounds[l] && sen_id < bounds[r]){
					return r-1;
				}
			}
			else{
				return -1;//δ�ҵ�
			}
		}
	}

	cut_fun get_cut_fun(){
		return std::bind(&Csentence::cut, this, _1,  _2);
	}


	void data_save(ofstream&fout){
		sl::mem_save(fout, mesh);
		sl::mem_save(fout, doc_count);
		sl::mem_save(fout, bounds);
	}
	void data_load(ifstream&fin){
		sl::mem_load(fin, mesh);
		sl::mem_load(fin, doc_count);
		sl::mem_load(fin, bounds);
	}

};