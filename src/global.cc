#include"global.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;

ofstream test_output;
ofstream  mih_output;
void open_file(string dir_path,string mode)
{
    cout << "Tik tok start saving!"<<endl;

	if(opendir(&dir_path[0]) == NULL)
	{
		cout << "Making new directory for this dataset !" << endl;
		mkdir(&dir_path[0],0777);
	}

    string file_name = dir_path+ mode+"_time.csv"; 
	cout << file_name << endl;
    test_output.open(file_name,ios::out|ios::trunc);
    test_output<< "step 1: Preprocess" << ","<< "step 2: Multi-index Hash Selecting" << ","  << "step 3: Optimizer Insert  process"<<"," << "step 4: Good Feature Select"<< "," << "step 5: Optimizer Optimization" << "," << "step 6: Postprocess"<<endl;
}
void write_file(double step1_consm,double step2_consm,double step3_consm, double step4_consm,double step5_consm,double step6_consm)
{
    //cout << step1_consm << "," << step2_consm << "," << step3_consm << endl;
    test_output << step1_consm << "," << step2_consm << "," << step3_consm << "," << step4_consm << "," << step5_consm << "," << step6_consm << endl;
}

void close_file()
{
    cout << "Tik-Tok time saved!" <<endl;
    test_output.close();
}

void open_file_mih(string dir_path,string file_name)
{
    cout << "MIH start saving!"<<endl;

	if(opendir(&dir_path[0]) == NULL)
	{
		cout << "Making new directory for this dataset !" << endl;
    	mih_output<< "Feature Covisibility"<< "," << "MIH Feature Selected"<<endl;
	}
void write_file_mih(double  cov_count,double mih_count)
{
    mih_output << cov_count <<"," << mih_count << endl;
}

void close_file_mih()
{
    cout << "MIH points data saved!" <<endl;
    mih_output.close();
}

void Stringsplit(const string str, const string splits, vector<string>& res)
{
	if (str == "")		return;
	//在字符串末尾也加入分隔符，方便截取最后一段
	string strs = str + splits;
	size_t pos = strs.find(splits);
	int step = splits.size();

	// 若找不到内容则字符串搜索函数返回 npos
	while (pos != strs.npos)
	{
		string temp = strs.substr(0, pos);
		res.push_back(temp);
		//去掉已分割的字符串,在剩下的字符串中进行分割
		strs = strs.substr(pos + step, strs.size());
		pos = strs.find(splits);
	}
}