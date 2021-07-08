
__conda_setup="$('/home/wangshi/anaconda3/bin/conda' 'shell.bash' 'hook' 2> /dev/null)"
if [ $? -eq 0 ]; then
    eval "$__conda_setup"
else
    if [ -f "/home/wangshi/anaconda3/etc/profile.d/conda.sh" ]; then
        . "/home/wangshi/anaconda3/etc/profile.d/conda.sh"
    else
        export PATH="/home/wangshi/anaconda3/bin:$PATH"
    fi
fi
unset __conda_setup

conda activate workspace;

count_gfs=0;
loop_total=10;

#dataset name here
#{room_long,room_roaming}
dataset_name=$1;


cd ORB_SLAM_TEST/$dataset_name/;

while(($count_gfs < $loop_total));
do evo_ape tum groundtruth.tum CameraTrajectory_RGBD_nogfs_nomih_blocksize10_$count_gfs.txt -va --save_results error_res_nogfs_nomih_$count_gfs.zip;
rm -rf error_res_nogfs_nomih_$count_gfs;
mkdir error_res_nogfs_nomih_$count_gfs;
unzip -d error_res_nogfs_nomih_$count_gfs error_res_nogfs_nomih_$count_gfs.zip;
rm -f error_res_nogfs_nomih_$count_gfs.zip;

evo_ape tum groundtruth.tum CameraTrajectory_RGBD_nogfs_mih4_blocksize10_$count_gfs.txt -va --save_results error_res_nogfs_mih4_$count_gfs.zip;
rm -rf error_res_nogfs_mih4_$count_gfs;
mkdir error_res_nogfs_mih4_$count_gfs;
unzip -d error_res_nogfs_mih4_$count_gfs error_res_nogfs_mih4_$count_gfs.zip;
rm -f error_res_nogfs_mih4_$count_gfs.zip;


evo_ape tum groundtruth.tum CameraTrajectory_RGBD_nogfs_mih8_blocksize10_$count_gfs.txt -va --save_results error_res_nogfs_mih8_$count_gfs.zip;
rm -rf error_res_nogfs_mih8_$count_gfs;
mkdir error_res_nogfs_mih8_$count_gfs;
unzip -d error_res_nogfs_mih8_$count_gfs error_res_nogfs_mih8_$count_gfs.zip;
rm -f error_res_nogfs_mih8_$count_gfs.zip;


evo_ape tum groundtruth.tum CameraTrajectory_RGBD_nogfs_mih11_blocksize10_$count_gfs.txt -va --save_results error_res_nogfs_mih11_$count_gfs.zip;
rm -rf error_res_nogfs_mih11_$count_gfs;
mkdir error_res_nogfs_mih11_$count_gfs;
unzip -d error_res_nogfs_mih11_$count_gfs error_res_nogfs_mih11_$count_gfs.zip;
rm -f error_res_nogfs_mih11_$count_gfs.zip;

evo_ape tum groundtruth.tum CameraTrajectory_RGBD_nogfs_mih16_blocksize10_$count_gfs.txt -va --save_results error_res_nogfs_mih16_$count_gfs.zip;
rm -rf error_res_nogfs_mih16_$count_gfs;
mkdir error_res_nogfs_mih16_$count_gfs;
unzip -d error_res_nogfs_mih16_$count_gfs error_res_nogfs_mih16_$count_gfs.zip;
rm -f error_res_nogfs_mih16_$count_gfs.zip;

evo_ape tum groundtruth.tum CameraTrajectory_RGBD_nogfs_mih32_blocksize10_$count_gfs.txt -va --save_results error_res_nogfs_mih32_$count_gfs.zip;
rm -rf error_res_nogfs_mih32_$count_gfs;
mkdir error_res_nogfs_mih32_$count_gfs;
unzip -d error_res_nogfs_mih32_$count_gfs error_res_nogfs_mih32_$count_gfs.zip;
rm -f error_res_nogfs_mih32_$count_gfs.zip;



((count_gfs=$count_gfs+1));
done;


