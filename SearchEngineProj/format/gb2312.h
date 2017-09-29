#pragma once
#ifdef DEBUG
#include"..\debugtool\report.h"
using std::cout;
#endif


#include<string>
using std::string;
//针对gb2312编码建立一个映射矩阵 减少检索时间(O(lgn) -> O(1))
#include"spell.h"
//gb2312映射矩阵, sting - int，由于id的不唯一，各个对象之间独立

//将字串编为单字
class gb2312_str{
	vector<string> wds;
public:
	gb2312_str(string str){
		for (int i = 0; i < str.length(); i++){
			if (str[i] < 0){
				wds.push_back(string(str, i, 2));
				i++;
			}
			else{
				wds.push_back(string(str, i, 1));
			}
		}
	}
	string&operator[](int i){
		return wds[i];
	}
	int length(){
		return wds.size();
	}
	
};

class gb2312_mat{
private:
	enum Artribute
	{
		len = 256,
		none  = -1
	};
	int mesh[len][len];//save ids 
public:
	gb2312_mat(){
		memset(mesh, none, len*len*sizeof(int));
	}

	int&operator[](string token){
		if (token.size() <= 1){
#ifdef DEBUG
			report(token);
#endif
			return mesh[0][token[0]];
		}
		int pos1 = (int)token[0] > 0 ? token[0] : -token[0];
		int pos2 = (int)token[1] > 0 ? token[1] : -token[1];
		
		return mesh[pos1][pos2];
	}
	bool is_chinese(string token){
		return token[0] >= 0 ? false : true;
	}
	int find(string token){
		return (*this)[token];
	}
	inline int end(){
		return none;
	}
};

//gb2313的拼音，拼音唯一，使用单例模式
#include<fstream>
using std::ifstream;

class gb2312_spell{
private:
	vector<charspell>table;
	gb2312_mat mesh;
	gb2312_spell(string fname = "data\\gb2312_spell.txt"){
		ifstream fin(fname);
		char buffer[128];
		while (fin.getline(buffer, 128)){
			if (fin.eof())break;
			string buf(buffer);
			stringstream sin(buf);
			string temp;
			sin >> temp;
			charspell temp_spell(sin);

			mesh[temp] = table.size();
			table.push_back(temp_spell);
		}
	}
	static gb2312_spell* gb2312_spell_ptr;
public:
	static gb2312_spell*get_ptr(){
		if (!gb2312_spell_ptr)
			gb2312_spell_ptr = new gb2312_spell();
		return gb2312_spell_ptr;
	}
	charspell&get_spell(string&word){
		if (mesh.find(word) != mesh.end()){
			return table[mesh[word]];
		}
		else{//error
			throw;
			return table[0];
		}
	}
};
gb2312_spell* gb2312_spell::gb2312_spell_ptr = nullptr;
