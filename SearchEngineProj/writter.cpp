#include"writter.h"

//��Ԫ����������
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

//��ȡ����
Creader::Creader(string fname):fin(fname),cur(0){
	fin >> buffer;
}
bool Creader::flush_new_line(){//�ĵ����������Ľ�
	if (fin >> buffer){
		cur = 0;
		return true;
	}
	else{
		cur = -1;
		return false;//��β��
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

//��������
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
bool Cfilter::match(Creader&stream){//��������ƥ��
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


//��������
Canalysis::Canalysis(string fname):fin(fname){
	;
}
void Canalysis::ana_ct(int time = 1000){//�Ե��ֽ��м�����Ĭ��ͳ��һǧ��
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

void Canalysis::w_index(int cut_num = 1000,int word_num = 20000){//!!�ٶ��� ��Ҫ�þ�����ʽʵ��
	//������ϵ��
	string wd = fin.get_word();
	word_table.push_back(wd);
	windex_table[wd] = 0;
	int last_id = 0;
	winfo_table.push_back(Canalysis::word_info());
	for (int i = 1,pos = 1; i < cut_num && pos < word_num;pos++){//pos����ǰ�ĵ�λ��
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
	//����ʱӦ�ý���һЩ�ʣ������Գ��ôʽ��д���ͬʱ���������Ƶ�ʵĴ���������
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
	//�ִ�ϵͳ ��ÿһ���ٽ����ֽ������˹�ϵ�����Ǹ��������Ӵ�����������
}

//������ѯ
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


//��������:��Ҫ��д ��gb2312mat��Ϊ�������ݽṹ������˲�����cut_num����Ϊ�ָ���ֵ��
void Canalysis::inverted_index(int doc_num = 10, int cut_num = 1000){//���󷽷���ȱ�� �ѿռ�
	//�������ű�
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
	//���:
	for each (auto var in is_table){
		cout << var.first << ":" << ends;
		for (int i = 0; i < is_table[var.first].size(); i++){
			cout << is_table[var.first][i] << " " << ends;
		}
		cout << endl;
	}
}

//gb2312,�˲��ָ�:9.19
void Canalysis::inverted_index(string filterfile, int doc_num, int thresh = 100){
	Cfilter filter(filterfile);
	int doc_ct = 0;
	int thr_ct = 0;//������Ϊ�ֽ���ֵ���� �����Ϊƫ��(λ��
	int word_ct = 0;

	string buf;
	//pre processing: �ӵ�һ�¿�ʼ
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
			//---�����ĵ���Ϣ:���޸�
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
	Canalysis rd("novel\\�����ǿ�.txt");
	rd.inverted_index(string("novel\\cut_reason.txt"), 20, 100);
	system("pause");
	system("pause");


	rd.w_index(4000,500000);
	rd.w_findover(12);
	system("pause");

	rd.inverted_index(100,10);
	vector<string> sstrs = { "��","��" };
	cout << rd.inverted_and(sstrs) << endl;
	system("pause");
	return 0;
	rd.ana_ct(100000);
	std::cout << "��������" << endl;
	rd.output("noveloutput.txt");
	std::cout << "�������" << endl;
	system("pause");
	/*while (1){
		cout << rd.get_word() << endl;
		system("pause");
	}*/
	char z[] = "��";
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