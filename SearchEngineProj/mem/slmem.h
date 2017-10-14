//要使得动态数据也能被存储读取 必须要配置每个类的存储函数保持对称 对无动态数据的类可直接存储
#include"..\debugtool\report.h"
#include"..\core\invtable.h"

#include<fstream>
using std::ifstream;
using std::ofstream;
using std::string;
namespace sl{//save load 针对含有动态变量的类 纯结构体请使用mem_save/load
//---save
	template<class T>
	T* data_save(T*p,string fname){
		ofstream fout(fname, std::ios::binary);
		p->data_save(fout);
		/*fout.write((char*)p, sizeof(T));*/
		fout.close();
		return p;
	}
//---load
	template<class T>
	T* data_load(T*p, string fname){
		ifstream fin(fname, std::ios::binary);
		p->data_load(fin);
		fin.close();
		return p;
	}
};
