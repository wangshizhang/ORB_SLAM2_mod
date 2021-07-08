
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
count_nogfs=0;
loop_total=10;

#dataset name here
#{room,desk,desk2,xyz,360,plant}
dataset_name=$1;


cd ORB_SLAM_TEST/rgbd_dataset_freiburg1_$dataset_name/;

while(($count_gfs < $loop_total));
do evo_ape tum groundtruth.tum CameraTrajectory_gfs_$count_gfs.txt -va --save_results error_res_gfs_$count_gfs.zip;
rm -rf error_res_gfs_$count_gfs;
mkdir error_res_gfs_$count_gfs;
unzip -d error_res_gfs_$count_gfs error_res_gfs_$count_gfs.zip;
rm -f error_res_gfs_$count_gfs.zip;
((count_gfs=$count_gfs+1));
done;

while(($count_nogfs < $loop_total));
do evo_ape tum groundtruth.tum CameraTrajectory_nogfs_$count_nogfs.txt -va --save_results error_res_nogfs_$count_nogfs.zip;
rm -rf error_res_nogfs_$count_nogfs;
mkdir error_res_nogfs_$count_nogfs;
unzip -d error_res_nogfs_$count_nogfs error_res_nogfs_$count_nogfs.zip;
rm -f error_res_nogfs_$count_nogfs.zip;
((count_nogfs=$count_nogfs+1));
done;

