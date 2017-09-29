#pragma once

//对对象数据进行管理的类 但直接存取不适合不适合动态数据 故需要在各类中自行设置存取代码

#include <fstream>
#include <string>
#include"..\debugtool\report.h"
#include"..\core\invtable.h"
using std::ifstream;
using std::ofstream;
using std::string;

#include"slmem.h"


template<class T>
class Tdata_man{
	T*ptr;
	string fname;
public:
	Tdata_man(string f="", T*p = nullptr) :fname(f), ptr(p){ ; }
	void bindFile(string f){ fname = f; }
	void bindPtr(T*p){
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
			SAVE_LOAD::data_save(p, fname);
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
			ptr = SAVE_LOAD::data_load(ptr, fname);
		}
		return ptr;
	}

};

#define DECLARE_CLASS_DATA(Cname) Cname##*p_##Cname=nullptr;\
	Tdata_man<typename Cname##> t_##Cname

#define LOAD_CLASS_DATA(Cname) if(p_##Cname##=t_##Cname##.load())return p_##Cname,t_##Cname##.file()

#define SAVE_CLASS_DATA(Cname) t_##Cname##.save(p_##Cname##),t_##Cname##.file()




//-----类数据、调试界面均在此：
class Cclass_data{
	DECLARE_CLASS_DATA(Cinverted_table);
public:
	Cclass_data(){
		t_Cinverted_table.bindFile("data\\inv_data.data");
	}
	~Cclass_data(){
		inv_save();
		delete p_Cinverted_table;
	}
	Cinverted_table* inv_init(){
		if (p_Cinverted_table = t_Cinverted_table.load())
			return p_Cinverted_table;
		
		Cinverted_table*inv_p = new Cinverted_table();
		cout << "first init..." << endl;
		for (int i = 0; i < 100; i++){
			Creader rd(string("data\\novel\\") + std::to_string(i) + ".txt");
			inv_p->load(rd, i);
		}
		p_Cinverted_table = inv_p;
		t_Cinverted_table.bindPtr(inv_p);

		return p_Cinverted_table;
	}
	void inv_save(){
		SAVE_CLASS_DATA(Cinverted_table);
	}
	int usr(){
		int input;
		string buf;
		std::cout << "选择: 0 退出、1 检索" << endl;
		cin >> input;
		switch (input)
		{
		case 0:break;
		case 1:
			cin >> buf;
			cout << p_Cinverted_table->search_string(buf)->show() << endl;

		default:
			break;
		}
		return input;
	}
};