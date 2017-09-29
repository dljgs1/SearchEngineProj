#pragma once
//���Ĵ����������ļ�����������ϵ


#include "..\format\gb2312.h"
#include "..\io\filter.h"
#include "..\io\reader.h"


#include<map>
using std::map;
//������
class Canalysis{
private:
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
	}word_info;//��ʾһ���ֵ�������Ϣ  0~99 100~199 200~299

	typedef struct wd_term{
		int bia_pos;
		int doc_id; //(���û������� ����ƫ�ú����ĵ���)
		wd_term(int b, int d = 0) :bia_pos(b), doc_id(d){ ; }
	}term_info;//������Ϣ(λ�á��ĵ��ŵȵ�)
	typedef int text;
	typedef struct{
		vector<term_info> info;
		inline void push_back(term_info&tm){ info.push_back(tm); }
	}doc_table;//���ű� ��ʱ��vec ������heap

private:
	//----ǰ������ �� ���ĵ�/Ƭ�� ����/��
	gb2312_mat mesh;
	vector<word_info> winfo_table;
	vector<string> word_table;
	vector<string> ban_word;//���ô�

public:
	Canalysis::Canalysis(){//��Ҫ��������ݵĶ�ȡ?
		   ;
	}
	//����һ������
	void w_index(Creader&fin,int doc_id){
		string wd = fin.get_word();
		//init
		int last_id;//!ʵ��һ�����ӵĹؼ�
		if (mesh.find(wd) != mesh.end()){
			last_id = mesh[wd];
		}
		else{
			mesh[wd] = word_table.size();
			last_id = word_table.size();
			word_table.push_back(wd);
			winfo_table.push_back(Canalysis::word_info());
		}

		for (int i = 1, pos = 1;; pos++){//pos����ǰ�ĵ�λ��
			wd = fin.get_word();
			if (mesh.find(wd) != mesh.end()){
				int temp = mesh[wd];//��ǰ���ֵ�id
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
	void w_findover(int);//���ҳ���Ƶ�ʳ���һ�������Ĵ���

};

//�£�


