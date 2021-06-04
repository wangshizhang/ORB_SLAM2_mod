#include <iostream>
#include <streambuf> 
#include<time.h>
#include <fstream> 
#include <vector>

#include <string>

//using namespace std;

//extern std::ofstream test_output;


void open_file(std::string ,std::string);
void write_file(double,double,double,double,double);
void close_file();
void Stringsplit(const std::string , const std::string , std::vector<std::string>& );