#include"global.h"

using namespace std;

void Stringsplit(const string , const string , vector<string>&);

ofstream test_output;
void open_file(char *file_char,string mode)
{
    cout << "Tik tok start saving!"<<endl;

    string file_id = string(file_char);

    vector<string> res;
    
	Stringsplit(file_id,"/",res);

	res.pop_back();
	file_id = res.back();

    cout << file_id << endl;

    string file_name = "/home/wangshi/test/ORB_SLAM_TEST/" + file_id+ "_"+ mode+".csv"; 
	cout << file_name << endl;
    test_output.open(file_name,ios::out|ios::trunc);
    test_output<< "step 1: Preprocess" << "," << "step 2: Optimizer Insert  process"<<"," << "step 3: Good Feature Select"<< "," << "step 4: Optimizer Optimization" << "," << "step 5: Postprocess"<<endl;
}
void write_file(double step1_consm,double step_gfs1_consm,double step_gfs2_consm, double step2_consm,double step3_consm)
{
    //cout << step1_consm << "," << step2_consm << "," << step3_consm << endl;
    test_output << step1_consm << "," << step_gfs1_consm << "," << step_gfs2_consm << "," << step2_consm << "," << step3_consm << endl;
}

void close_file()
{
    cout << "Tik-Tok time saved!" <<endl;
    test_output.close();
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