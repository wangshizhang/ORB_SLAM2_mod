#include "MIH_helper.h"
#include<bitset>

using namespace std;
int64_t des_mat2key(cv::Mat &des_mat)
{
    int64_t res = 0;

    res = des_mat.at<u_int8_t>(0,0);


    for(int i = 1;i < des_mat.cols;i ++)
    {
        //cout << bitset<64> (res) << endl;
        res = res << 8;// shift the res with at least 8 bits
        res += des_mat.at<u_int8_t>(0,i); 
    }
    
    /*
    //test
    for(int i = 0;i < des_mat.rows;i ++)
    {
        for(int j = 0;j < des_mat.cols;j ++)
        {
            int64_t test = des_mat.at<u_int8_t>(i,j);
            cout << test << ",";
        }
        cout << endl;
    }
    */

    //cout << bitset<64> (res) << endl;
    
    return res;
}