//Ҫʹ�ö�̬����Ҳ�ܱ��洢��ȡ ����Ҫ����ÿ����Ĵ洢�������ֶԳ� ���޶�̬���ݵ����ֱ�Ӵ洢
#include"..\debugtool\report.h"
#include"..\core\invtable.h"

#include<fstream>
using std::ifstream;
using std::ofstream;
using std::string;
namespace SAVE_LOAD{
//---save
	template<class T>
	T* data_save(T*p,string fname){
		ofstream fout(fname, std::ios::binary);
		fout.write((char*)p, sizeof(T));
		fin.close();
		return p;
	}

	//inv�ػ�
	Cinverted_table* data_save(Cinverted_table*p, string fname){
		ofstream fout(fname, std::ios::binary);
		p->data_save(fout);
		fout.close();
		return p;
	}
//---load
	template<class T>
	T* data_load(T*p, string fname){
		ifstream fin(fname, std::ios::binary);
		fin.read((char*)p, sizeof(T));
		fin.close();
	}

	Cinverted_table* data_load(Cinverted_table*p, string fname){
		ifstream fin(fname, std::ios::binary);
		p->data_load(fin);
		fin.close();
		return p;
	}
};
