/**
* This file is part of ORB-SLAM2.
*
* Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/raulmur/ORB_SLAM2>
*
* ORB-SLAM2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
*/


#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>

#include<opencv2/core/core.hpp>

#include<System.h>

#include"global.h"

using namespace std;

void LoadImages(const string &strAssociationFilename, vector<string> &vstrImageFilenamesRGB,
                vector<string> &vstrImageFilenamesD, vector<double> &vTimestamps);

int main(int argc, char **argv)
{
    if(argc != 9)
    {
        cerr << endl << "Usage: ./rgbd_tum path_to_vocabulary path_to_settings path_to_sequence path_to_association" << endl;
        return 1;
    }

    string dir_path = "/home/wangshi/test/ORB_SLAM_TEST/" ;
    string file_id = string(argv[3]);
    vector<string> res;
    
	Stringsplit(file_id,"/",res);
	res.pop_back();
	file_id = res.back();
    string file_path = dir_path + file_id + "/";

    char* gfs_str = argv[5];
    bool  gfs = strcmp(gfs_str+4,"1") >= 0;

    string MUL_INDEX_str = string(argv[6]);
    int MUL_INDEX_HASH_COUNT = stoi(MUL_INDEX_str.substr(4));
    //cout << MUL_INDEX_HASH_COUNT << "!" << endl;

    string MIH_BLOCK = string(argv[7]);
    int MIT_BLOCK_SIZE = stoi(MIH_BLOCK.substr(6));
    cout << MIT_BLOCK_SIZE << "!" << endl;

    string count_index = string(&(string(argv[8])[6]));

    string file_name = "RGBD_";
    
    if(gfs)
    {
        cout << "Start Good Feature Select!!" << endl;
        file_name += "gfs_";
    }
    else
    {
        file_name += "nogfs_";
    }

    if(MUL_INDEX_HASH_COUNT != 1)
    {
        cout << "Start Multi-index Hash Selecting!" << endl;
        file_name += "mih"+ to_string(MUL_INDEX_HASH_COUNT) +"_" ;
        open_file_mih(file_path,file_name+"blocksize" + to_string(MIT_BLOCK_SIZE) + "_" + count_index);
    }
    else
    {
        file_name += "nomih_";
    }

    file_name += "blocksize" + to_string(MIT_BLOCK_SIZE) + "_";

    file_name += count_index;
    open_file(file_path,file_name);

    // if(gfs)
    // {
    //     cout << "Start Good Feature Select!!" << endl;
    //     open_file(file_path,"RGBD_badgfs_"+count_index);
    // }
    // else
    // {
    //     //cout << argv[5] << endl;
    //     open_file(file_path,"RGBD_nogfs_"+count_index);
    // }

    // Retrieve paths to images
    vector<string> vstrImageFilenamesRGB;
    vector<string> vstrImageFilenamesD;
    vector<double> vTimestamps;
    string strAssociationFilename = string(argv[4]);
    LoadImages(strAssociationFilename, vstrImageFilenamesRGB, vstrImageFilenamesD, vTimestamps);

    // Check consistency in the number of images and depthmaps
    int nImages = vstrImageFilenamesRGB.size();
    if(vstrImageFilenamesRGB.empty())
    {
        cerr << endl << "No images found in provided path." << endl;
        return 1;
    }
    else if(vstrImageFilenamesD.size()!=vstrImageFilenamesRGB.size())
    {
        cerr << endl << "Different number of images for rgb and depth." << endl;
        return 1;
    }

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[1],argv[2],ORB_SLAM2::System::RGBD,true,gfs,MUL_INDEX_HASH_COUNT,MIT_BLOCK_SIZE);

    // Vector for tracking time statistics
    vector<float> vTimesTrack;
    vTimesTrack.resize(nImages);

    cout << endl << "-------" << endl;
    cout << "Start processing sequence ..." << endl;
    cout << "Images in the sequence: " << nImages << endl << endl;

    // Main loop
    cv::Mat imRGB, imD;
    for(int ni=0; ni<nImages; ni++)
    {
        // Read image and depthmap from file
        imRGB = cv::imread(string(argv[3])+"/"+vstrImageFilenamesRGB[ni],CV_LOAD_IMAGE_UNCHANGED);
        imD = cv::imread(string(argv[3])+"/"+vstrImageFilenamesD[ni],CV_LOAD_IMAGE_UNCHANGED);
        double tframe = vTimestamps[ni];

        if(imRGB.empty())
        {
            cerr << endl << "Failed to load image at: "
                 << string(argv[3]) << "/" << vstrImageFilenamesRGB[ni] << endl;
            return 1;
        }

#ifdef COMPILEDWITHC11
        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
#else
        std::chrono::monotonic_clock::time_point t1 = std::chrono::monotonic_clock::now();
#endif

        // Pass the image to the SLAM system
        SLAM.TrackRGBD(imRGB,imD,tframe);

#ifdef COMPILEDWITHC11
        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
#else
        std::chrono::monotonic_clock::time_point t2 = std::chrono::monotonic_clock::now();
#endif

        double ttrack= std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();

        vTimesTrack[ni]=ttrack;

        // Wait to load the next frame
        double T=0;
        if(ni<nImages-1)
            T = vTimestamps[ni+1]-tframe;
        else if(ni>0)
            T = tframe-vTimestamps[ni-1];

        if(ttrack<T)
            usleep((T-ttrack)*1e6);
    }

    // Stop all threads
    SLAM.Shutdown();

    //close logging
    close_file();
    close_file_mih();
    
    // Tracking time statistics
    sort(vTimesTrack.begin(),vTimesTrack.end());
    float totaltime = 0;
    for(int ni=0; ni<nImages; ni++)
    {
        totaltime+=vTimesTrack[ni];
    }
    cout << "-------" << endl << endl;
    cout << "median tracking time: " << vTimesTrack[nImages/2] << endl;
    cout << "mean tracking time: " << totaltime/nImages << endl;

    // Save camera trajectory


    SLAM.SaveTrajectoryTUM(file_path + "CameraTrajectory_"+file_name+".txt");
    SLAM.SaveKeyFrameTrajectoryTUM(file_path + "KeyFrameTrajectory_"+file_name+".txt");

    // if(gfs)
    // {
    //     SLAM.SaveTrajectoryTUM(file_path + "CameraTrajectory_badgfs_"+count_index+".txt");
    //     SLAM.SaveKeyFrameTrajectoryTUM(file_path + "KeyFrameTrajectory_badgfs_"+count_index+".txt");   
    // }
    // else
    // {
    //     SLAM.SaveTrajectoryTUM(file_path + "CameraTrajectory_nogfs_"+count_index+".txt");
    //     SLAM.SaveKeyFrameTrajectoryTUM(file_path + "KeyFrameTrajectory_nogfs_"+count_index+".txt");   
    // }

    return 0;
}

void LoadImages(const string &strAssociationFilename, vector<string> &vstrImageFilenamesRGB,
                vector<string> &vstrImageFilenamesD, vector<double> &vTimestamps)
{
    ifstream fAssociation;
    fAssociation.open(strAssociationFilename.c_str());
    while(!fAssociation.eof())
    {
        string s;
        getline(fAssociation,s);
        if(!s.empty())
        {
            stringstream ss;
            ss << s;
            double t;
            string sRGB, sD;
            ss >> t;
            vTimestamps.push_back(t);
            ss >> sRGB;
            vstrImageFilenamesRGB.push_back(sRGB);
            ss >> t;
            ss >> sD;
            vstrImageFilenamesD.push_back(sD);

        }
    }
}
