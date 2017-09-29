#pragma once


//词项的数据结构
#include<string>
#include<vector>
#include<sstream>
using std::stringstream;
using std::vector;
using std::string;
//词项的倒排记录结构
struct term_record{
	int bias_pos;//偏移位置
	int doc_id;//所在文档编号
};
//倒排记录表项结构
struct record_table{//对于非分布式数据读入，数据必然是有序的
	string token;
	vector<term_record> table;
	vector<int>doc_index;
	//vector<int>word_feq;//在各个文档中的出现频率
	record_table(){ ; }
	record_table(string name):token(name){ ; }
	void push_back(const term_record&term){
		if (doc_index.size()){
			if (table[doc_index.back()].doc_id != term.doc_id){
				doc_index.push_back(table.size());
			}
		}
		else{
			doc_index.push_back(0);//第一条文档所在
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