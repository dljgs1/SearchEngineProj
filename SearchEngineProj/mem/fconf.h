//文件配置
#include<map>
#include<fstream>
using std::map;
using std::fstream;
class Cfileconfig{
	map<string, string>mesh;
	const string path = "data\\fileconfig.txt";
public:
	Cfileconfig(){
		ifstream fin(path);
		string buf1, buf2;
		while (fin >> buf1 >> buf2){
			mesh[buf1] = buf2;
		}
		fin.close();
	}
	string get_path(string cname){//通过类名查找其数据文件路径
		if (mesh.find(cname) != mesh.end())
			return mesh[cname];
		return "";
	}
};