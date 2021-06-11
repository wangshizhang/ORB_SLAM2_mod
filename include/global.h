#include <iostream>
#include <streambuf> 
#include<time.h>
#include <fstream> 
#include <vector>

#include <string>

//using namespace std;

//extern std::ofstream test_output;


void open_file(std::string ,std::string);
void write_file(double,double,double,double,double,double);
void close_file();

void open_file_mih(std::string ,std::string);
void write_file_mih(double ,double );
void close_file_mih();

void Stringsplit(const std::string , const std::string , std::vector<std::string>& );