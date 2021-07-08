count=0;

loop_total=10;

#dataset name here
#{room,desk,desk2,xyz,360,plant}
dataset_name=$1;
while(($count < $loop_total));

do ~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/rgbd_tum ~/orb_slam2/ORB_SLAM2_mod/Vocabulary/ORBvoc.txt ~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/TUM1.yaml ~/dataset/TUM/rgbd_dataset_freiburg1_$dataset_name/ ~/dataset/TUM/rgbd_dataset_freiburg1_$dataset_name/associations.txt gfs=0 index=$count;

~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/rgbd_tum ~/orb_slam2/ORB_SLAM2_mod/Vocabulary/ORBvoc.txt ~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/TUM1.yaml ~/dataset/TUM/rgbd_dataset_freiburg1_$dataset_name/ ~/dataset/TUM/rgbd_dataset_freiburg1_$dataset_name/associations.txt gfs=1 index=$count;

((count=$count+1));

done;
