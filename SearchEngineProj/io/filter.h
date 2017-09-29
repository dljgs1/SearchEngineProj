#pragma once

#ifndef C_FILTER

#include"..\io\reader.h"
#include<vector>
#include<string>
#include<algorithm>
using std::vector;
using std::string;
using std::find;

//过滤器: 滤除匹配的模式
//构造函数：filename：配置文件 内含对模式的设置 md：工作模式
//注：可以使用regex代替部分功能 但无法很好的与Creader配合
class Cfilter{
	enum ftype{
		OneTime = '|',
		OneMore = '*',
		Nothing = ' '
	};
	typedef struct{
		vector<string> wds;
		ftype type;
		bool match(const string&wd){
			return find(wds.begin(), wds.end(), wd) != wds.end();
		}
	}filter_info;
	vector<filter_info> info;
	char mode;//工作方式?
	string buffer;//缓存（必须）
public:
	Cfilter(string filename, char md = 0){
		string temp;
		Creader reader(filename);
		filter_info cur;
		char state = 0;
		while (!reader.end()){
			temp = reader.get_word();
			if (temp == "!"){
				state = 1;
			}
			else if (temp == string("") + char(OneTime) || temp == string("") + char(OneMore)){
				reader.flush();
				cur.type = (ftype)temp[0];
				state = 0;
				info.push_back(cur);
				cur = filter_info();
			}
			else if (state){
				cur.wds.push_back(temp);
			}
		}
	}
	string&getbuffer(){
		return this->buffer;
	}
	bool match(Creader&stream){//匹配输入流
		int onemorect = 0;
		buffer = stream.get_word();
		for (int i = 0; i < info.size();){
			auto&var = info[i];
			switch (var.type){
			default:
			case OneTime:
				if (var.match(buffer)){
					i++;
					buffer = stream.get_word();
				}
				else return false;
				break;
			case OneMore:
				if (var.match(buffer)){
					onemorect++;
					buffer = stream.get_word();
				}
				else if (onemorect){
					onemorect = 0;
					i++;
				}
				else{
					return false;
				}
				break;
			}
		}
		return true;
	}
	bool remove(string&);
};



#endif