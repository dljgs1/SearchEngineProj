#pragma once
#include<iostream>
#include<string>
using std::cout;
using std::string;
void report(string str){
#ifdef DEBUG
	cout << "error:" << str << std::endl;
#endif
}