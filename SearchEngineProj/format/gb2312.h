#pragma once
#ifdef DEBUG
#include"..\debugtool\report.h"
using std::cout;
#endif


#include<string>
using std::string;
#include"spell.h"

//=========================GBK编码格式==================
//GB2312汉字区：B0A1 - F7FE
//GB2312非汉字符号区：A1A1 - A9FE
//-----
//GB13000.1汉字区：8140 - A0FE    AA40 - FEA0
//GB13000.1非汉字区：A840 - A9A0
//-----
//用户自定区：AAA1 - AFFE  F8A1 - FEFE  A140 - A7A0
//=====================================================
//string转short
inline unsigned short str_to_short(string&str){
	unsigned short t = str[0];
	return (unsigned short)((t << 8) + str[1]);
}
//判断是否为GBK的汉字 传入16位变量
inline bool is_gbk_chinese(unsigned short s){
	return ((s >= 0xB0A1 && s <= 0xF7FE) || (s >= 0x8140 && s <= 0xA0FE) || (s >= 0xAA40 && s <= 0xFEA0));
}
inline bool is_gbk_chinese(string&str){
	return is_gbk_chinese(str_to_short(str));
}
//判断是否为GBK的符号
inline bool is_gbk_symbol(string&str){
	unsigned short s = str_to_short(str);
	return ((s >= 0xA1A1 && s <= 0xA9FE) || (s >= 0xA840 && s <= 0xA9A0));
}

//判是否为GBK编码(仅区分于ascii
inline bool is_gbk_code(string&str){
	return str[0] < 0;
}


//将字串编为单字
class gb2312_str{
	vector<string> wds;
public:
	gb2312_str(string str){
		for (int i = 0; i < str.length(); i++){
			if (i + 1< str.length() && is_gbk_chinese(string(str, i, 2))){
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

//针对gb2312编码建立一个映射矩阵 减少检索时间(O(lgn) -> O(1))
//gb2312映射矩阵, sting - int，由于id的不唯一，各个对象之间独立
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

	int&operator[](string token){//取字号 此处无需考虑其是否为汉字 !修改后需重新加载
		/*if (token.size() == 1){
			return mesh[0][(unsigned short)token[0]];
		}
		int pos1 = (int)token[0] > 0 ? token[0] : -token[0];
		int pos2 = (int)token[1] > 0 ? token[1] : -token[1];*/
		if (token.size() < 1){ return mesh[0][0];}
		unsigned char pos1 = token[0];
		unsigned char pos2 = token[1];
		return mesh[pos1][pos2];
	}

	bool is_chinese(string token){
		return is_gbk_chinese(token);
		//return token[0] >= 0 ? false : true;//只适合gb2312 不适合gbk的判法
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
