#include<iostream>
using namespace std;

#include"io\filter.h"
#include"core\analyzer.h"
#include"core\invtable.h"
#include"mem\dataman.h"

#include <functional>


int main(){

	/*vector<int>sort_table(2700,0);
	vector<Ctemp>arr(2700);
	for (int i = 0; i < 2700; i++){
		sort_table[i] = i;
		arr[i].sum = rand() % 100;
	}
	int flag = -1;
	cout << "start" << endl;
	vector<Ctemp>&temp = arr;
	::sort(sort_table.begin(), sort_table.end(), [temp, flag](int a, int b){return flag*temp[a].sum < flag*temp[b].sum; });

	system("pause");*/
	Cclass_data man;
	while (man.usr());
	return 0;

	/*·Ö¸îÎÄ¼þ²âÊÔ
	Cfilter fcut("data\\cut_config.txt");
	Creader reader("data\\ÍÌÊÉÐÇ¿Õ.txt");
	string foutdict("data\\novel\\");
	while (!fcut.match(reader));
	reader.flush();
	for (int doc_id = 0; doc_id < 100 && !reader.end(); doc_id++){
		string fname = foutdict + to_string(doc_id) + string(".txt");
		ofstream fout(fname);
		while (!fcut.match(reader)){
			fout << fcut.getbuffer();
		}
		reader.flush();
		fout.close();
		cout << "Ð´" << doc_id << endl;
	}
	*/

	/*²âÊÔ±à¼­¾àÀë
	gb2312_spell*sp = gb2312_spell::get_ptr();
	string s1 = "ÎÒ";
	string s2 = "ÍÛ";
	auto& var1 = sp->get_spell(s1);
	auto& var2 = sp->get_spell(s2);
	cout << var1.edit_dis(var2) << endl;
	system("pause");*/
}
