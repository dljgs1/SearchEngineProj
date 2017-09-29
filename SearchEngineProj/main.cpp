#include<iostream>
using namespace std;

#include"io\filter.h"
#include"core\analyzer.h"
#include"core\invtable.h"
#include"mem\dataman.h"

int main(){
	Cclass_data man;
	cout << "loading..." << endl;
	Cinverted_table*tb = man.inv_init();
	cout << "complete!" << endl;
	
	while (man.usr());
	system("pause");
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
