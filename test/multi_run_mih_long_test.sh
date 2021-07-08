count=0;

loop_total=10;

#dataset name here
#{room_long,room_roaming}
dataset_name=$1;
block=10;

while(($count < $loop_total));

do ~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/rgbd_tum ~/orb_slam2/ORB_SLAM2_mod/Vocabulary/ORBvoc.txt ~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/TUM1.yaml ~/dataset/TUM/$dataset_name/ ~/dataset/TUM/$dataset_name/associate_blender.txt gfs=0 mih=1 block=$block index=$count;

~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/rgbd_tum ~/orb_slam2/ORB_SLAM2_mod/Vocabulary/ORBvoc.txt ~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/TUM1.yaml ~/dataset/TUM/$dataset_name/ ~/dataset/TUM/$dataset_name/associate_blender.txt gfs=0 mih=4 block=$block index=$count;


~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/rgbd_tum ~/orb_slam2/ORB_SLAM2_mod/Vocabulary/ORBvoc.txt ~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/TUM1.yaml ~/dataset/TUM/$dataset_name/ ~/dataset/TUM/$dataset_name/associate_blender.txt gfs=0 mih=8 block=$block index=$count;

~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/rgbd_tum ~/orb_slam2/ORB_SLAM2_mod/Vocabulary/ORBvoc.txt ~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/TUM1.yaml ~/dataset/TUM/$dataset_name/ ~/dataset/TUM/$dataset_name/associate_blender.txt gfs=0 mih=11 block=$block index=$count;

~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/rgbd_tum ~/orb_slam2/ORB_SLAM2_mod/Vocabulary/ORBvoc.txt ~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/TUM1.yaml ~/dataset/TUM/$dataset_name/ ~/dataset/TUM/$dataset_name/associate_blender.txt gfs=0 mih=16 block=$block index=$count;

#~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/rgbd_tum ~/orb_slam2/ORB_SLAM2_mod/Vocabulary/ORBvoc.txt ~/orb_slam2/ORB_SLAM2_mod/Examples/RGB-D/TUM1.yaml ~/dataset/TUM/$dataset_name/ ~/dataset/TUM/$dataset_name/associate_blender.txt gfs=0 mih=32 block=$block index=$count;

((count=$count+1));

done;
