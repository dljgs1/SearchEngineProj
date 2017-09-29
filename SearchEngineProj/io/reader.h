#pragma once

#ifndef C_READER

#include<fstream>
#include<string>
using std::ifstream;
using std::string;

//���ֶ�ȡģ�飺�ɶ�ȡtxt ���ص��֣������ַ�����ĸ��
//����ע���˴�ȫ��������������Ϊ���ܵķֲ�ʽ�Ƽ���ӿ���׼��
class Creader{
private:
	ifstream fin;
	string buffer;
	int cur;
public:
	Creader(string fname) :fin(fname), cur(0){
		fin >> buffer;
	}
	~Creader(){ ; }
	string get_word(){//����ʽ�ɴ˾���
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
			if (temp.back() >= 0){//
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
			return false;//��β��
		}
	}
	const Creader& operator>>(string&str){
		str = get_word();
		return *this;
	}
	bool end(){ return cur == -1; }
};



#endif