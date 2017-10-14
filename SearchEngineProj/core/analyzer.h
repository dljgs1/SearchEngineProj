//level 3
#pragma once
//���Ĵ����������ļ�����������ϵ

#include "..\format\gb2312.h"
#include "..\io\filter.h"
#include "..\io\reader.h"
#include "..\mem\commonfun.h"
#include "..\marco.h"

#include<map>
#include<functional>
using std::map;
//������
class Canalysis{
private:
	//���νṹ
	typedef struct wdif{
		int id;//���������ʱ��id
		string token;
		vector<int> connect_id;//������ӵ��ֺ�

		vector<int> times;//���ִ��� ��Ƶ
		int sum = 0;//�ܳ��ִ��� ��Ƶ
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
	}word_info;//��ʾһ���ֵ�������Ϣ  0~99 100~199 200~299

private:
	//----ǰ������ �� ���ĵ�/Ƭ�� ����/��
	gb2312_mat mesh;
	vector<word_info> winfo_table;
	vector<int> sort_table;

	vector<string> ban_word;//���ô� ��ʱ����


	
	//�ִ� �� ��ͳ�Ʒ�����ɷִ� ���ҳ��ؼ���
	gb2312_mat ban_words;
	const string ban_path = "data\\sentence_config.txt";

	//----������� : �Թؼ���Ϊ���� ������������
	float* *feature_vec;
	int vec_len = 0;
	int doc_num = 0;


public:
	Canalysis::Canalysis(){//��Ҫ��������ݵĶ�ȡ?���������ݹ��������ṩ����, �ӸĽ�Creader����
		;
	}
	void init(){//��ʼ��
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


	//����˫������
	void build_index(Creader&fin,int doc_id){
		//init
		int last_id  = - 1;//!ʵ��һ�����ӵĹؼ�
		int pos = 0;
		do{
			string wd = fin.get_word();
			if (ban_words[wd] == 1){
				last_id = -1;//�жϼǴ�
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

	void sort(int flag = -1){//���մ�Ƶ���� �����ֺ��Ŵ� ��������ʾ��˳�� Ĭ�Ͻ�����
		//! sort�Զ��庯���в��ɰ����ṹ�� ��������
		vector<int>temp(winfo_table.size());
		for (int i = 0; i < temp.size(); i++)
			temp[i] = winfo_table[i].sum;
		::sort(sort_table.begin(), sort_table.end(), [temp,flag](int a, int b){return flag*temp[a] < flag*temp[b]; });
		for (int i = 0; i < winfo_table.size(); i++){
			cout << i << " ";
			winfo_table[i].sort(flag);
		}
	}


	string show(int i){//������
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

//�£�


