#pragma once

#ifndef C_READER

#include<fstream>
#include<string>
using std::ifstream;
using std::string;

//汉字读取模块：可读取txt 返回单字（包括字符、字母）
//——注：此处全部用流操作，是为可能的分布式云计算接口做准备
//二注：应建立缓冲区，对反复使用的文档进行存储
class Creader{
private:
	ifstream fin;
	string buffer;
	int cur;
public:
	Creader(string fname) :fin(fname), cur(0){
		if (!fin.fail()){
			fin >> buffer;
		}
		else{
			cur = -1;
		}
	}
	~Creader(){ ; }
	string get_word(){//捕获方式由此决定
		string temp;
		while (1){
			if (cur >= buffer.length()){
				if (flush() == false)
					return "";
			}
			if (buffer[cur] == ' '){
				cur++; continue;
			}
			temp += buffer[cur++];
			if (temp.back() >= 0){//ascii单字节码
				return temp;
			}
			else if (temp.length() == 2){
				return temp;
			}
		}
		return temp;
	}
	bool flush(){
		if (fin >> buffer){
			cur = 0;
			return true;
		}
		else{
			cur = -1;
			return false;//到尾了
		}
	}
	const Creader& operator>>(string&str){
		str = get_word();
		return *this;
	}
	bool end(){ return cur == -1; }
};



#endif