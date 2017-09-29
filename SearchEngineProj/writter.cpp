#include"writter.h"

//字元：（暂弃）
Cnewchar::Cnewchar() :type(0){ szC[0] = szC[1] = szC[2] = 0; }
Cnewchar::Cnewchar(string str) :type(str.length()) {
	szC[0] = str[0];
	szC[1] = str[1];
	szC[type] = str[type];
}
string Cnewchar::get_str(){
	return string(szC);
}
bool Cnewchar::operator==(const Cnewchar&other){
	return type == other.type && szC[0] == other.szC[0] && szC[1] == other.szC[1];
}
Cnewchar Creader::get_newchar(){
	return Cnewchar(get_word());
}

//读取器：
Creader::Creader(string fname):fin(fname),cur(0){
	fin >> buffer;
}
bool Creader::flush_new_line(){//文档结束检测待改进
	if (fin >> buffer){
		cur = 0;
		return true;
	}
	else{
		cur = -1;
		return false;//到尾了
	}
}
Creader::~Creader(){ fin.close(); }
bool Creader::end(){ return cur == -1; }
string Creader::get_word(){
	string temp;
	while (1){
		if (cur >= buffer.length()){
			if (flush_new_line() == false)return "";
		}
		if (buffer[cur] == ' '){
			cur++; continue;
		}
		temp += buffer[cur++];
		if (temp.back() >= 0){
			return temp/* + '\0'*/;
		}
		else if (temp.length() == 2){
			return temp/* + '\0'*/;
		}
	}
}

//过滤器：
Cfilter::Cfilter(string filename,char md = 0){
	string temp;
	Creader reader(filename);
	filter_info cur;
	char state = 0;
	while (!reader.end()){
		temp = reader.get_word();
		if (temp == "!"){
			state = 1;
		}
		else if (temp == "" + OneTime || temp == "" + OneMore){
			reader.flush_new_line();
			cur.type = (ftype)temp[0];
			state = 0;
			info.push_back(cur);
			cur = filter_info();
		}
		else if(state){
			cur.wds.push_back(temp);
		}
	}
}
bool Cfilter::match(Creader&stream){//对流进行匹配
	buffer = stream.get_word();
	int onemorect = 0;
	for (int i = 0; i < info.size();){
		auto&var = info[i];
		switch (var.type){
		default:
			case OneTime:
				if (var.match(buffer))i++;
				else return false;
				break;
			case OneMore:
				if (var.match(buffer)){
					onemorect++;
				}
				else if(onemorect){
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


//分析器：
Canalysis::Canalysis(string fname):fin(fname){
	;
}
void Canalysis::ana_ct(int time = 1000){//对单字进行计数，默认统计一千字
	while (time--){
		string temp(fin.get_word());
		ct.plus(temp);
	}
}
void Canalysis::output(string fname){
	ofstream fout(fname);
	ct.output(fout);
	fout.close();
}

void Canalysis::w_index(int cut_num = 1000,int word_num = 20000){//!!速度慢 需要用矩阵形式实现
	//建立关系表：
	string wd = fin.get_word();
	word_table.push_back(wd);
	windex_table[wd] = 0;
	int last_id = 0;
	winfo_table.push_back(Canalysis::word_info());
	for (int i = 1,pos = 1; i < cut_num && pos < word_num;pos++){//pos代表当前文档位置
		wd = fin.get_word();
		if (windex_table.find(wd) != windex_table.end()){
			int temp = windex_table[wd];
			winfo_table[last_id].add(temp);
			last_id = temp;
		}
		else{//
			windex_table[wd] = i;
			winfo_table[last_id].add(i);
			winfo_table.push_back(word_info());

			word_table.push_back(wd);
			last_id = i;
			i++;
		}
	}
}

void Canalysis::w_findover(int feq = 10){
	set<string> sets;
	ifstream fin("novel\\banwords.txt");
	string tp;
	while (fin >> tp)sets.insert(tp);
	fin.close();
	//查找时应该禁用一些词，包括对常用词进行处理，同时对连续相近频率的词组进行组合
	for each (auto var in word_table){
		if (sets.find(var) == sets.end()){
			int id = windex_table[var];
			word_info&temp = winfo_table[id];
			for (int i = 0; i < temp.times.size(); i++){
				if (temp.times[i]>feq && sets.find(word_table[temp.connect_id[i]]) == sets.end()){
					cout << var << word_table[temp.connect_id[i]] << ":" << temp.times[i] << endl;
				}
			}
		}
	}
	//分词系统 对每一个临近的字建立拓扑关系，宾那个对其连接次数进行排序
}

//布尔查询
int Canalysis::inverted_and(vector<string>&strs){
	vector<int> temp;
	for each (auto var in strs)
	{
		if (temp.size() == 0){
			temp = is_table[var];
		}
		else{
			for (int i = 0; i < temp.size(); i++){
				if (temp[i] && is_table[var][i])temp[i] = 1;
			}
		}
	}
	int c = 0;
	for (int i = 0; i < temp.size(); i++){
		c += temp[i];
	}
	return c;
}


//倒排索引:需要重写 用gb2312mat作为基本数据结构，添加滤波器，cut_num将作为分割阈值，
void Canalysis::inverted_index(int doc_num = 10, int cut_num = 1000){//矩阵方法：缺点 费空间
	//建立倒排表：
	for (int c = 0; c < doc_num; c++){
		for (int i = 0; i < cut_num; i++){
			string wd = fin.get_word();
			if (is_table.find(wd) != is_table.end()){
				is_table[wd][c]++;
			}
			else{
				is_table[wd] = vector<int>(doc_num,0);
				is_table[wd][c]++;
			}
		}
	}
	//输出:
	for each (auto var in is_table){
		cout << var.first << ":" << ends;
		for (int i = 0; i < is_table[var.first].size(); i++){
			cout << is_table[var.first][i] << " " << ends;
		}
		cout << endl;
	}
}

//gb2312,滤波分割:9.19
void Canalysis::inverted_index(string filterfile, int doc_num, int thresh = 100){
	Cfilter filter(filterfile);
	int doc_ct = 0;
	int thr_ct = 0;//即可作为分界阈值计数 亦可作为偏置(位置
	int word_ct = 0;

	string buf;
	//pre processing: 从第一章开始
	while (!filter.match(fin));
	fin.flush_new_line();
	//cut
	while (doc_ct < doc_num){
		if (thr_ct > thresh){
			if (filter.match(fin)){
				fin.flush_new_line();
				doc_ct++;
				thr_ct = 0;
			}
			else{
				buf = filter.buffer;
			}
		}
		else{
			buf = fin.get_word();
		}
		if (inv_table_mesh.find(buf) == inv_table_mesh.end()){
			inv_table_mesh[buf] = word_ct;
			doc_table temp;
			//---存入文档信息:需修改
			temp.push_back(wd_term(thr_ct, doc_ct));
			terms_doc_table.push_back(temp);
			word_table.push_back(buf);
			word_ct++;
		}
		else{
			terms_doc_table[inv_table_mesh[buf]].push_back(wd_term(thr_ct, doc_ct));
		}

		thr_ct++;
	}
	
	//output
	for (int i = 0; i < terms_doc_table.size();i++)
	{
		auto&arr = terms_doc_table[i];
		cout << word_table[i] << ":";
		for each (auto num in arr.info)
		{
			cout << num.doc_id << "," << num.bia_pos << " ";
		}
		cout << endl;
	}
}


int writter_main(){
	Canalysis rd("novel\\吞噬星空.txt");
	rd.inverted_index(string("novel\\cut_reason.txt"), 20, 100);
	system("pause");
	system("pause");


	rd.w_index(4000,500000);
	rd.w_findover(12);
	system("pause");

	rd.inverted_index(100,10);
	vector<string> sstrs = { "罗","峰" };
	cout << rd.inverted_and(sstrs) << endl;
	system("pause");
	return 0;
	rd.ana_ct(100000);
	std::cout << "分析结束" << endl;
	rd.output("noveloutput.txt");
	std::cout << "输出结束" << endl;
	system("pause");
	/*while (1){
		cout << rd.get_word() << endl;
		system("pause");
	}*/
	char z[] = "和";
	for (int i = 1; i < 128; i++){
		for (int j = 1; j < 128; j++){
			z[0] = -i;
			z[1] = -j;
			cout << z;
		}
		system("pause");
		cout << endl;
	}/*
	 cout << (int)z[0] << endl;
	 cout << (int)z[1] << endl;*/
	system("pause");
	return 0;
}