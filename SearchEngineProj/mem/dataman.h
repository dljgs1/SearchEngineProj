// level 4
#pragma once
class Cclass_data;
//对对象数据进行管理的类 但直接存取不适合不适合动态数据 故需要在各类中自行设置存取代码
//添加功能：写一个类，Cxxxx，具有save() load() 函数，且声明友元类 friend class Cclass_data;  其功能在Cclass_man中进行管理
//可对类进行单独测试，如果需要别的模块的数据，需要对Tdata_man增加函数，获得接口
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
			report("未绑定数据指针！");
		}
		return p;
	}
	T* load(){
		ifstream fin(fname, std::ios::binary);
		if (fin.fail()){
			cout << "文件不存在！" << endl;
			report("文件不存在！");
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


//-----类数据、调试界面均在此：
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
	//数据初始化 不自动LOAD 因为期初没有数据可以LOAD
	void init(){
		sen_init();//sen需要在inv之前初始化
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
		std::cout << "选择: 0 退出、1 检索、2 保存、3 分析、4 历史、5 聚类" << endl;
		cin >> input;
		switch (input)
		{
		case 0:
			cout << "退出？" << endl;
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
			cout << "保存？" << endl;
			cin >> buf;
			if (buf == "y")
				save();
			break;
		case 3:
			cout << "输入字号" << endl;
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