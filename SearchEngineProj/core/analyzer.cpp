#include"..\core\analyzer.h"
using std::ofstream;
//��������

















//----����-------------------------------------------------------------------
Canalysis::Canalysis(string fname) :fin(fname){
	;
}
void Canalysis::ana_ct(int time = 1000){//�Ե��ֽ��м�����Ĭ��ͳ��һǧ��
	/*while (time--){
		string temp(fin.get_word());
		ct.plus(temp);
	}*/
}
void Canalysis::output(string fname){
	/*ofstream fout(fname);
	ct.output(fout);
	fout.close();*/
}
void Canalysis::w_index(int cut_num = 1000, int word_num = 20000){//!!�ٶ��� ��Ҫ�þ�����ʽʵ��
	//������ϵ��
	/*string wd = fin.get_word();
	word_table.push_back(wd);
	windex_table[wd] = 0;
	int last_id = 0;
	winfo_table.push_back(Canalysis::word_info());
	for (int i = 1, pos = 1; i < cut_num && pos < word_num; pos++){//pos����ǰ�ĵ�λ��
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
	*/
}
//----�ݲ�ʹ�ã�
#include<set>
using std::set;
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
					//cout << var << word_table[temp.connect_id[i]] << ":" << temp.times[i] << endl;
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
				is_table[wd] = vector<int>(doc_num, 0);
				is_table[wd][c]++;
			}
		}
	}
#ifdef DEBUG
	for each (auto var in is_table){
		cout << var.first << ":" << ends;
		for (int i = 0; i < is_table[var.first].size(); i++){
			cout << is_table[var.first][i] << " " << ends;
		}
		cout << endl;
	}
#endif
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
	fin.flush();
	//cut
	while (doc_ct < doc_num){
		if (thr_ct > thresh){
			if (filter.match(fin)){
				fin.flush();
				doc_ct++;
				thr_ct = 0;
			}
			else{
				buf = filter.getbuffer();
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
#ifdef DEBUG
	for (int i = 0; i < terms_doc_table.size(); i++)
	{
		auto&arr = terms_doc_table[i];
		cout << word_table[i] << ":";
		for each (auto num in arr.info)
		{
			cout << num.doc_id << "," << num.bia_pos << " ";
		}
		cout << endl;
	}
#endif
}

//-------------------------------------------------------------------------------------------

