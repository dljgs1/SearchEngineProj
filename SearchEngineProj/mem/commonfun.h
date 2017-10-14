#pragma once
//level 2

#include<string>
#include<fstream>
#include<vector>
#include"..\marco.h"
//���ø������͵ĳ��ô�ȡ���� 
namespace sl{
	using std::ofstream;
	using std::ifstream;
	using std::string;
	using std::vector;
//ԭ����
	template<class T>
	void mem_save(ofstream&fout, T&temp){
		MEM_WRITE(fout, &temp, sizeof(T));
	}
	template<class T>
	void mem_load(ifstream&fin, T&temp){
		MEM_READ(fin, &temp, sizeof(T));
	}

//string��
	void mem_save(ofstream&fout,string&temp){
		int len = temp.length();
		MEM_WRITE(fout, &len, sizeof(int));
		MEM_WRITE(fout, temp.c_str(), len);
	}
	void mem_load(ifstream&fin, string&temp){//�����ַ������Ȳ�����128
		int len;
		char str[STR_MAX_LEN];
		MEM_READ(fin, &len, sizeof(int));
		MEM_READ(fin, str, len);
		str[len] = 0;
		temp = string(str);
	}
//ԭ��������
	template<class T>
	void mem_save(ofstream&fout, typename vector<T>&temp){
		int len = temp.size();
		MEM_WRITE(fout, &len, sizeof(int));
		for (int i = 0; i < len; i++){
			mem_save(fout, temp[i]);
		}
	}

	template<class T>
	void mem_load(ifstream&fin, typename vector<T>&temp){
		int len;
		MEM_READ(fin, &len, sizeof(int));
		temp = vector<T>(len);
		for (int i = 0; i < len; i++){
			mem_load(fin, temp[i]);
		}
	}
}