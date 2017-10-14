// level 4
#pragma once
class Cclass_data;
//�Զ������ݽ��й������ ��ֱ�Ӵ�ȡ���ʺϲ��ʺ϶�̬���� ����Ҫ�ڸ������������ô�ȡ����
//��ӹ��ܣ�дһ���࣬Cxxxx������save() load() ��������������Ԫ�� friend class Cclass_data;  �书����Cclass_man�н��й���
//�ɶ�����е������ԣ������Ҫ���ģ������ݣ���Ҫ��Tdata_man���Ӻ�������ýӿ�
//
#include <fstream>
#include <string>
#include"..\debugtool\report.h"
#include"..\core\invtable.h"
#include"..\core\analyzer.h"
using std::ifstream;
using std::ofstream;
using std::string;
#include"slmem.h"
#include"fconf.h"
template<class T>
class Tdata_man{
	T*ptr;
	string fname;
public:
	Tdata_man(string f="", T*p = nullptr) :fname(f), ptr(p){ ; }
	void bind_file(string f){ fname = f; }
	void bind_ptr(T*p){
		ptr = p;
	}
	string file(){
		return fname;
	}
	T* save(T*p = nullptr){
		if (p == nullptr)p = ptr;
		if (p){/*
			ofstream fout(fname,std::ios::binary);
			fout.write((char*)p, sizeof(T));
			fout.close();*/
			sl::data_save(p, fname);
		}
		else{
			report("δ������ָ�룡");
		}
		return p;
	}
	T* load(){
		ifstream fin(fname, std::ios::binary);
		if (fin.fail()){
			cout << "�ļ������ڣ�" << endl;
			report("�ļ������ڣ�");
		}
		else{
			/*ptr = new T();
			fin.read((char*)ptr, sizeof(T));
			fin.close();*/
			ptr = new T();
			ptr = sl::data_load(ptr, fname);
		}
		return ptr;
	}

};

//-----macro
#define DECLARE_CLASS_DATA(Cname) Cname##*p_##Cname=nullptr;\
	Tdata_man<typename Cname##> t_##Cname
#define LOAD_CLASS_DATA(Cname) if(p_##Cname##=t_##Cname##.load())return p_##Cname
#define SAVE_CLASS_DATA(Cname) t_##Cname##.save(p_##Cname##),t_##Cname##.file()
#define CLEAR_CLASS_DATA(Cname) delete p_##Cname
#define BIND_FILE_PATH(Cname) t_##Cname##.bind_file(config.get_path(#Cname))

#define FUN_INIT_HEAD(Cname) LOAD_CLASS_DATA(Cname);
#define FUN_INIT_END(Cname,ptr) t_##Cname##.bind_ptr(p_##Cname##);return p_##Cname = ptr


//-----�����ݡ����Խ�����ڴˣ�
class Cclass_data{
	DECLARE_CLASS_DATA(Cinverted_table);
	DECLARE_CLASS_DATA(Canalysis);
	DECLARE_CLASS_DATA(Csentence);
	Cfileconfig config;
public:
	Cclass_data(){
		BIND_FILE_PATH(Cinverted_table);
		BIND_FILE_PATH(Canalysis);
		BIND_FILE_PATH(Csentence);

		init();
	}
	~Cclass_data(){
		CLEAR_CLASS_DATA(Cinverted_table);
		CLEAR_CLASS_DATA(Csentence);
		CLEAR_CLASS_DATA(Canalysis);
	}
	//���ݳ�ʼ�� ���Զ�LOAD ��Ϊ�ڳ�û�����ݿ���LOAD
	void init(){
		sen_init();//sen��Ҫ��inv֮ǰ��ʼ��
		inv_init();
		ana_init();
	}
	void save(){
		SAVE_CLASS_DATA(Cinverted_table);
		SAVE_CLASS_DATA(Canalysis);
		SAVE_CLASS_DATA(Csentence);
	}
//init:
	Csentence* sen_init(){
		FUN_INIT_HEAD(Csentence);
		Csentence* sen_p = new Csentence();
		sen_p->init();
		FUN_INIT_END(Csentence, sen_p);
	}

	Canalysis* ana_init(){
		FUN_INIT_HEAD(Canalysis);
		Canalysis* ana_p = new Canalysis();
		ana_p->init();

		cout << "first analysis..." << endl;
		for (int i = 0; i < 100; i++){
			Creader rd(string("data\\novel\\") + std::to_string(i) + ".txt");
			ana_p->build_index(rd, i);
			cout << i << " ";
		}
		cout << endl;
		cout << "sorting..." << endl;
		ana_p->sort();
		cout << "complete!" << endl;
		FUN_INIT_END(Canalysis, ana_p);
	}

	Cinverted_table* inv_init(){
		FUN_INIT_HEAD(Cinverted_table);
		Cinverted_table*inv_p = new Cinverted_table();
		cout << "first init..." << endl;
		for (int i = 0; i < 100; i++){
			cout << i << " " << ends;
			Creader rd(string("data\\novel\\") + std::to_string(i) + ".txt");
			inv_p->load(rd, i, p_Csentence->get_cut_fun());
			p_Csentence->new_doc();//!!
		}
		cout << "complete!" << endl;
		FUN_INIT_END(Cinverted_table, inv_p);
	}

//fun:
	string str_record_result(record_table*pr){
		stringstream sout;
		auto&tb = pr->table;
		for (int i = 0; i < tb.size(); i++){
			sout << tb[i].doc_id << " " << p_Csentence->get_doc_id(tb[i].doc_id) << " " << tb[i].bias_pos << endl;
		}
		return sout.str();
	}

	string retrive(string str,int k = 1){
		stringstream sout;
		sout << str_record_result(p_Cinverted_table->search_string(str, k));
		return sout.str();
	}
	string history(){
		record_table*p = nullptr;
		stringstream sout;
		int ct = 0;
		string buf;
		do{
			p = p_Cinverted_table->old_search(buf, ct++);
			if (p){
				sout << buf << ":" << endl;
				sout << str_record_result(p) << endl;
			}
		} while (p);
		return sout.str();
	}

	string cluster(int k){
		return "";
	}

	int usr(){
		int input;
		int id_cho;
		string buf;
		std::cout << "ѡ��: 0 �˳���1 ������2 ���桢3 ������4 ��ʷ��5 ����" << endl;
		cin >> input;
		switch (input)
		{
		case 0:
			cout << "�˳���" << endl;
			cin >> buf;
			if (buf != "y")
				input = -1;
			break;
		case 1:
			cin >> buf >> id_cho;
			cout << retrive(buf, id_cho) << endl;
			break;
			/*
			cout << p_Cinverted_table->search_string(buf)->show() << endl;*/
		case 2:
			cout << "���棿" << endl;
			cin >> buf;
			if (buf == "y")
				save();
			break;
		case 3:
			cout << "�����ֺ�" << endl;
			cin >> id_cho;
			cout << p_Canalysis->show(id_cho) << endl;
			break;
		case 4:
			cout << history();
			break;
		case 5:
			cout << "k=?" << endl;
			cin >> id_cho;
			cout << cluster(id_cho) << endl;
		default:
			break;
		}
		return input;
	}
};