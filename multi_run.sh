count=0;
while(($count < 2));
do ~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/rgbd_tum Vocabulary/ORBvoc.txt ~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/TUM1.yaml ~/dataset/TUM/rgbd_dataset_freiburg1_xyz/ ~/dataset/TUM/rgbd_dataset_freiburg1_xyz/associations.txt gfs=0 index=$count;
((count=$count+1));
done;s
