#pragma once

#include<string>
#include<vector>
#include<algorithm>
#include<sstream>


using std::string;
using std::vector;
using std::min;
using std::stringstream;
//×ÖµÄÆ´Òô½á¹¹

struct charspell{
	vector<string> spells;//Æ´·¨>=1
	charspell(stringstream&sin){
		string temp;
		while (sin >> temp)spells.push_back(temp);

	}
	int edit_dis(charspell&other, int cho1 = 0, int cho2 = 0){//±à¼­¾àÀë
		string&str1 = spells[cho1];
		string&str2 = other.spells[cho2];
		int len1 = spells[cho1].length();
		int len2 = other.spells[cho2].length();
		vector<vector<int>> calcumat(len1+1, vector<int>(len2+1, 0));
		for (int i = 1; i <= len1; i++){
			calcumat[i][0] = i;
		}
		for (int j = 1; j <= len2; j++){
			calcumat[0][j] = j;
		}
		vector<int>temp = vector<int>(3,0);
		for (int i = 1; i <= len1; i++){
			for (int j = 1; j <= len2; j++){
				temp[0] = calcumat[i - 1][j] + 1;
				temp[1] = calcumat[i][j - 1] + 1;
				temp[2] = calcumat[i - 1][j - 1] + (str1[i - 1] == str2[j - 1] ? 0 : 1);
				int min = temp[0];
				for each (auto var in temp)
					if (var < min)min = var;
				calcumat[i][j] = min;
			}
		}
		return calcumat[len1][len2];
	}
};