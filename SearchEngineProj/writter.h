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
//������txt�ı����з���



//����Ԫ����ʾ��GB2313������ַ�
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


//���ֶ�ȡģ�飺�ɶ�ȡtxt ���ص��֣������ַ�����ĸ��
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
//������:��һ������Ϊkey ������м���ͳ��

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


//������: �˳�ƥ���ģʽ
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
	char mode;//������ʽ?
public:
	string buffer;//���棨���룩
	Cfilter(string,char);
	bool match(Creader&);
	bool remove(string&);
};

//������
#include "gb2312.h"
class Canalysis{
public:
//�ṹ
	typedef struct{
		vector<int> connect_id;//������ӵ��ֺ�
		vector<int> times;//���ִ���
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
	}word_info;//��ʾ�ֵ�������Ϣ
	typedef struct wd_term{
		int bia_pos;
		int doc_id; //(���û������� ����ƫ�ú����ĵ���)
		wd_term(int b,int d = 0):bia_pos(b),doc_id(d){ ; }
	}term_info;//������Ϣ(λ�á��ĵ��ŵȵ�)
	typedef int text;
	typedef struct{
		vector<term_info> info;
		inline void push_back(term_info&tm){ info.push_back(tm); }
	}doc_table;//���ű� ��ʱ��vec ������heap

private:
	Creader fin;
	Ccounter<string> ct;

	//----����������������/�ֺ��ĵ�/Ƭ��֮�����ϵ
	map<string,vector<text>> is_table;//���ű�

	gb2312_mat inv_table_mesh;//�µ��ű�-index
	vector<doc_table> terms_doc_table;//�µ��ű�-table

	//----ǰ������ �� ���ĵ�/Ƭ�� ����/��
	//----improve(���� �Դ���Ϣ�����ռ�
	//map<string, int> windex_table;
	gb2312_mat windex_table;
	vector<word_info> winfo_table;
	vector<string> word_table;

	vector<string> ban_word;//���ô�


public:
	Canalysis(string fname);
	void ana_ct(int time);
	void output(string fname);

	void inverted_index(int,int);//�������� ��һ����������ֶ�

	void inverted_index(string,int, int);//����˲��ָ�ĵ�������
	
	int inverted_and(vector<string>&);

	void w_index(int,int);//��������֮��һ�ι�ϵ������
	void w_findover(int);//���ҳ���Ƶ�ʳ���һ�������Ĵ���
};



int writter_main();


#endif