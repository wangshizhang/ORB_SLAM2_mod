# Summary Report for 2 improvements reproductions in Local BA

Wangshi in orbbec @ 2021.6.15

code github : https://github.com/wangshizhang/ORB_SLAM2_mod

## Introduction

根据苏丹哥提供的一篇paper "Good Graph to Optimize: Cost-Effective,Budget-Aware Bundle Adjustment in Visual SLAM"，其中做了三个改进。代码已开源，github地址https://github.com/ivalab/gf_orb_slam2

- 1、Good Feature Select。针对local BA中，对于observation也就是待优化图中edge的筛选。
- 2、Multi-index Hash。针对local BA中，由共视产生的特征点的进一步筛选。
- 3、Good Graph Select。针对local BA中，优化图的整体筛选。

现已对其中Good Feature Select以及Multi-index Hash进行了复现，结果有一定的参考性，将进行代码复现思路，复现结果的总结。

## Code Details Instructions
这一部分将对代码实现思路和细节上进行说明，所有代码都将基于原始Orb-slam2（ https://github.com/raulmur/ORB_SLAM2 ）进行改进。

### Good Feature Select

#### The ideas and detailed process

具体idea的理解过程请参考 “Good Feature Selection for Least Squares Pose Optimization in VO/VSLAM”这篇paper。
我将简单说明这个idea的主要思路如下。

- 1、从object function也就是误差的二阶模出发，我们可以得到其中(map points)地图特征点的误差对于相机位姿误差的影响，将体现为基于雅克比的推导式（参考paper的推导即可），其意义是map points的方差到特征点方差的传递。简单来说，有一些map points对camera pose的求解影响大，有一些影响小。因此我们需要筛去那些影响大的map points的观测，也就是待优化图中的edge。
- 2、为了衡量这个影响，我们对所推导出的矩阵需要设计一个指标进行衡量。paper中探讨了多种常见指标，最后选择了LogDet作为使用指标。
- 3、为了加速基于LogDet的筛选过程，论文设计了greedy policy，从而减小了search space。

以上就是这篇paper主要的实现过程，也就是代码实现的主要思路

#### Code Structure

代码主要过程实现在Local BA过程，更具体来说，实现在Optimizer.cc 的 Optimizer::LocalBundleAdjustment 函数中(Line 780+)。将在原始optimizer的图已经构建好之后，对每一条edge进行进一步的筛选，剔除不要的edge。具体过程分为两个部分。

- 1、计算paper中所推导的，基于每一次观测的H_c矩阵。基于这个矩阵和LogDet指标，计算出这条edge的score并存放。
- 2、根据每一条edge的score进行筛选，存放最后的结果

代码过程实现可参考源码中的注释，Mono和Stereo结构相似。

### Multi-index Hash

#### The ideas and detailed process

具体idea的理解过程请参考 “Low-latency Visual SLAM with Appearance-Enhanced Local MapBuilding” 这篇paper，我将简单说明这个idea的主要思路如下。

- 1、原始ORB-SLAM2在构建优化图的时候采用了共视（Covisibility），但共视图意味着随着KeyFrame数量的上升，比较靠前的KeyFrame的共视点对于当前KeyFrame的优化的帮助是有限的。因此paper提出，除了共视之外，再引入一个基于相似度的map points筛选，即我们希望LocalBA的map points要和当前KeyFrame更接近一点。
- 2、那么如何筛选出和当前KeyFrame相似的map points？这篇paper提出了基于hash的多段匹配，即将ORB的256位特征进行分段筛选，对当前KeyFrame的可观测到的map points进行分段匹配，每一个point都能通过Multi-index Hash匹配得到一些和它局部上相似的map points，然后求并集即可。
- 3、paper中进一步采用了和Good Feature Select相似的greedy策略，对得到的解进行了进一步筛选

以上就是这篇paper的主要思路，代码实现思路也是一致的。

#### Code Structure

代码主要过程实现在Local BA过程，更具体来说，实现在Optimizer.cc 的 Optimizer::LocalBundleAdjustment 函数中（Line 490+)。将在optimizer构建优化图之前，针对其基于共视的Local Map Points先进行筛选。具体过程分为三个部分。

- 1、对基于共视（Covisibility）的Local Map Points构建Multi-index Hash Table，就是把每一个ORB分为多段，放入每一段所对应的hash table。每一个Hash Table，key是256位被分为多段后的值（比方256位被分为32段，那么每一段就是8位，值在[0,255]之中），value是对应map points的索引，即如果有多个map points在某一段上是一致的，那么相对应这一段的hash value就是这一些map points的索引list。
- 2、对当前KeyFrame的map points做分段匹配查找，即每一个point都可以获得一些和它局部相似的点，把所有point的结果求并集、
- 3、存放最后所筛选得到的map points作为Local Map Points。

注意，我这里没有实现greedy的筛选策略，基于两个考虑。第一，Good Feature Select中，LogDet Greedy的效果不是很明显；第二，从结果看，特征点的筛选比例已经比较明显，因此没有做进一步的筛选

## Results of improvements

### Good Feature Selection

#### Time Comparison

这里展示的是采用了Good Feature Select和原始ORB-SLAM2在不同dataset上各阶段消耗时间的比值。可以看出，Good Feature Select对于整个过程的加速效果并不是十分显著（即比值小于1），在一些dataset上甚至更加耗时。并没有获得预期的加速效果。

![test_pics/rgbd_dataset_freiburg1_360_time_res_ratio_gfs.jpg](attachment:rgbd_dataset_freiburg1_360_time_res_ratio_gfs.jpg)

![rgbd_dataset_freiburg1_desk2_time_res_ratio_gfs.jpg](attachment:rgbd_dataset_freiburg1_desk2_time_res_ratio_gfs.jpg)

![rgbd_dataset_freiburg1_desk_time_res_ratio_gfs.jpg](attachment:rgbd_dataset_freiburg1_desk_time_res_ratio_gfs.jpg)

![rgbd_dataset_freiburg1_plant_time_res_ratio_gfs.jpg](attachment:rgbd_dataset_freiburg1_plant_time_res_ratio_gfs.jpg)

#### Error comparison
但我们不止希望改进是单纯的提速，否则我们只需要随意减小问题规模就可以。因此我们需要对精度进行衡量。

这里采用误差的平均和方差来画图，越靠近左下角意味着越好。

可以看出，Good Feature Select虽然牺牲了一定的精度，但并没有获得预期的速度上的提升。

![rgbd_dataset_freiburg1_360_error_std_res_gfs.jpg](attachment:rgbd_dataset_freiburg1_360_error_std_res_gfs.jpg)

![rgbd_dataset_freiburg1_desk2_error_std_res_gfs.jpg](attachment:rgbd_dataset_freiburg1_desk2_error_std_res_gfs.jpg)

![rgbd_dataset_freiburg1_desk_error_std_res_gfs.jpg](attachment:rgbd_dataset_freiburg1_desk_error_std_res_gfs.jpg)

![rgbd_dataset_freiburg1_plant_error_std_res_gfs.jpg](attachment:rgbd_dataset_freiburg1_plant_error_std_res_gfs.jpg)

![rgbd_dataset_freiburg1_room_error_std_res_gfs.jpg](attachment:rgbd_dataset_freiburg1_room_error_std_res_gfs.jpg)

### Multi-index Hash

这里我们将对比同一个dataset上，将256位orb特征分为不同数量的block，所运行的结果进行对比。

#### Time Comparison

可以看出，分为4段和8段对于特征点的筛选较强，显著加速了localBA的过程。而分为16段和32段，由于匹配时间的上升，不仅没有加速，反而降低了速度。

![rgbd_dataset_freiburg1_360_time_res_ratio_MIH.jpg](attachment:rgbd_dataset_freiburg1_360_time_res_ratio_MIH.jpg)

![rgbd_dataset_freiburg1_desk2_time_res_ratio_MIH.jpg](attachment:rgbd_dataset_freiburg1_desk2_time_res_ratio_MIH.jpg)

![rgbd_dataset_freiburg1_desk_time_res_ratio_MIH.jpg](attachment:rgbd_dataset_freiburg1_desk_time_res_ratio_MIH.jpg)

![rgbd_dataset_freiburg1_plant_time_res_ratio_MIH.jpg](attachment:rgbd_dataset_freiburg1_plant_time_res_ratio_MIH.jpg)

#### Points Selecting Comparison

这里我对localBA过程中，Map Points的数量做了对比。可以看出，分为4段和8段在特征点选取数量上是接近的，这也解释了为何上面的run time他们也有相似的表现。而分为16段和32段，map points数量和原始的就比较接近了，这也是为什么速度没有得到提升的原因，因为问题规模没有得到有效的减小。

![rgbd_dataset_freiburg1_360%20Feature%20Selection.jpg](attachment:rgbd_dataset_freiburg1_360%20Feature%20Selection.jpg)

![rgbd_dataset_freiburg1_desk2%20Feature%20Selection.jpg](attachment:rgbd_dataset_freiburg1_desk2%20Feature%20Selection.jpg)

![rgbd_dataset_freiburg1_desk%20Feature%20Selection.jpg](attachment:rgbd_dataset_freiburg1_desk%20Feature%20Selection.jpg)

![rgbd_dataset_freiburg1_plant%20Feature%20Selection.jpg](attachment:rgbd_dataset_freiburg1_plant%20Feature%20Selection.jpg)

#### Error comparison

同样的，我们需要对精度进行测试。从精度结果上看，虽然分为4段和8段的运行速度是相近的，但是在精度上，8段的MIH可以比4段保持更好的误差精度，这提示我们，虽然它们筛选出的特征点数量相似，但质量是不一样的，从而在多个dataset上都呈现着牺牲一定的精度但获取了更好的速度的结果。16段和32段，正如我们所预想的，由于没有对特征点进行有效的减小，精度和原始ORB-SLAM2并没有明显的区分。

![rgbd_dataset_freiburg1_360_error_std_res_MIH-2.jpg](attachment:rgbd_dataset_freiburg1_360_error_std_res_MIH-2.jpg)

![rgbd_dataset_freiburg1_desk2_error_std_res_MIH.jpg](attachment:rgbd_dataset_freiburg1_desk2_error_std_res_MIH.jpg)

![rgbd_dataset_freiburg1_desk_error_std_res_MIH.jpg](attachment:rgbd_dataset_freiburg1_desk_error_std_res_MIH.jpg)

![rgbd_dataset_freiburg1_plant_error_std_res_MIH.jpg](attachment:rgbd_dataset_freiburg1_plant_error_std_res_MIH.jpg)

![rgbd_dataset_freiburg1_room_error_std_res_MIH.jpg](attachment:rgbd_dataset_freiburg1_room_error_std_res_MIH.jpg)

## Prospection 

在这次paper的复现中，我主要复现了Good Feature Select以及Multi-index Hash两个改进。将结果总结如下。

- Good Feature Select 虽然从理论上有一定可行性，但是基于LogDet的greedy筛选策略看起来没有对结果进行有效的改进。其原因，从直觉上讲，可能是LogDet的筛选比例并没有很合适，需要对超参进行进一步的测试。
- Multi-index Hash则是获得了较好的符合预期的结果，即牺牲了一定的精度，获得了稳定的速度的提升。但和paper结果所提出的32段效果最好，结论不一致。其原因可能有两方面，第一，Multi-index Hash中特征点的取舍策略可能影响结果；第二，测试过程中发现4段和8段虽然筛选了特征点，但看起来已经达到了hash table所设置的最大特征点数量，这意味着随着特征点数量进一步上升，这样的Multi-index Hash很可能会丢失掉有效的特征点。

因此，对于Multi-index Hash的改进，我还会进行进一步的测试。同时，还将进行其他可行LocalBA的改进尝试。

# ORB-SLAM2
**Authors:** [Raul Mur-Artal](http://webdiis.unizar.es/~raulmur/), [Juan D. Tardos](http://webdiis.unizar.es/~jdtardos/), [J. M. M. Montiel](http://webdiis.unizar.es/~josemari/) and [Dorian Galvez-Lopez](http://doriangalvez.com/) ([DBoW2](https://github.com/dorian3d/DBoW2))

**13 Jan 2017**: OpenCV 3 and Eigen 3.3 are now supported.

**22 Dec 2016**: Added AR demo (see section 7).

ORB-SLAM2 is a real-time SLAM library for **Monocular**, **Stereo** and **RGB-D** cameras that computes the camera trajectory and a sparse 3D reconstruction (in the stereo and RGB-D case with true scale). It is able to detect loops and relocalize the camera in real time. We provide examples to run the SLAM system in the [KITTI dataset](http://www.cvlibs.net/datasets/kitti/eval_odometry.php) as stereo or monocular, in the [TUM dataset](http://vision.in.tum.de/data/datasets/rgbd-dataset) as RGB-D or monocular, and in the [EuRoC dataset](http://projects.asl.ethz.ch/datasets/doku.php?id=kmavvisualinertialdatasets) as stereo or monocular. We also provide a ROS node to process live monocular, stereo or RGB-D streams. **The library can be compiled without ROS**. ORB-SLAM2 provides a GUI to change between a *SLAM Mode* and *Localization Mode*, see section 9 of this document.

<a href="https://www.youtube.com/embed/ufvPS5wJAx0" target="_blank"><img src="http://img.youtube.com/vi/ufvPS5wJAx0/0.jpg" 
alt="ORB-SLAM2" width="240" height="180" border="10" /></a>
<a href="https://www.youtube.com/embed/T-9PYCKhDLM" target="_blank"><img src="http://img.youtube.com/vi/T-9PYCKhDLM/0.jpg" 
alt="ORB-SLAM2" width="240" height="180" border="10" /></a>
<a href="https://www.youtube.com/embed/kPwy8yA4CKM" target="_blank"><img src="http://img.youtube.com/vi/kPwy8yA4CKM/0.jpg" 
alt="ORB-SLAM2" width="240" height="180" border="10" /></a>


### Related Publications:

[Monocular] Raúl Mur-Artal, J. M. M. Montiel and Juan D. Tardós. **ORB-SLAM: A Versatile and Accurate Monocular SLAM System**. *IEEE Transactions on Robotics,* vol. 31, no. 5, pp. 1147-1163, 2015. (**2015 IEEE Transactions on Robotics Best Paper Award**). **[PDF](http://webdiis.unizar.es/~raulmur/MurMontielTardosTRO15.pdf)**.

[Stereo and RGB-D] Raúl Mur-Artal and Juan D. Tardós. **ORB-SLAM2: an Open-Source SLAM System for Monocular, Stereo and RGB-D Cameras**. *IEEE Transactions on Robotics,* vol. 33, no. 5, pp. 1255-1262, 2017. **[PDF](https://128.84.21.199/pdf/1610.06475.pdf)**.

[DBoW2 Place Recognizer] Dorian Gálvez-López and Juan D. Tardós. **Bags of Binary Words for Fast Place Recognition in Image Sequences**. *IEEE Transactions on Robotics,* vol. 28, no. 5, pp.  1188-1197, 2012. **[PDF](http://doriangalvez.com/php/dl.php?dlp=GalvezTRO12.pdf)**

# 1. License

ORB-SLAM2 is released under a [GPLv3 license](https://github.com/raulmur/ORB_SLAM2/blob/master/License-gpl.txt). For a list of all code/library dependencies (and associated licenses), please see [Dependencies.md](https://github.com/raulmur/ORB_SLAM2/blob/master/Dependencies.md).

For a closed-source version of ORB-SLAM2 for commercial purposes, please contact the authors: orbslam (at) unizar (dot) es.

If you use ORB-SLAM2 (Monocular) in an academic work, please cite:

    @article{murTRO2015,
      title={{ORB-SLAM}: a Versatile and Accurate Monocular {SLAM} System},
      author={Mur-Artal, Ra\'ul, Montiel, J. M. M. and Tard\'os, Juan D.},
      journal={IEEE Transactions on Robotics},
      volume={31},
      number={5},
      pages={1147--1163},
      doi = {10.1109/TRO.2015.2463671},
      year={2015}
     }

if you use ORB-SLAM2 (Stereo or RGB-D) in an academic work, please cite:

    @article{murORB2,
      title={{ORB-SLAM2}: an Open-Source {SLAM} System for Monocular, Stereo and {RGB-D} Cameras},
      author={Mur-Artal, Ra\'ul and Tard\'os, Juan D.},
      journal={IEEE Transactions on Robotics},
      volume={33},
      number={5},
      pages={1255--1262},
      doi = {10.1109/TRO.2017.2705103},
      year={2017}
     }

# 2. Prerequisites
We have tested the library in **Ubuntu 12.04**, **14.04** and **16.04**, but it should be easy to compile in other platforms. A powerful computer (e.g. i7) will ensure real-time performance and provide more stable and accurate results.

## C++11 or C++0x Compiler
We use the new thread and chrono functionalities of C++11.

## Pangolin
We use [Pangolin](https://github.com/stevenlovegrove/Pangolin) for visualization and user interface. Dowload and install instructions can be found at: https://github.com/stevenlovegrove/Pangolin.

## OpenCV
We use [OpenCV](http://opencv.org) to manipulate images and features. Dowload and install instructions can be found at: http://opencv.org. **Required at leat 2.4.3. Tested with OpenCV 2.4.11 and OpenCV 3.2**.

## Eigen3
Required by g2o (see below). Download and install instructions can be found at: http://eigen.tuxfamily.org. **Required at least 3.1.0**.

## DBoW2 and g2o (Included in Thirdparty folder)
We use modified versions of the [DBoW2](https://github.com/dorian3d/DBoW2) library to perform place recognition and [g2o](https://github.com/RainerKuemmerle/g2o) library to perform non-linear optimizations. Both modified libraries (which are BSD) are included in the *Thirdparty* folder.

## ROS (optional)
We provide some examples to process the live input of a monocular, stereo or RGB-D camera using [ROS](ros.org). Building these examples is optional. In case you want to use ROS, a version Hydro or newer is needed.

# 3. Building ORB-SLAM2 library and examples

Clone the repository:
```
git clone https://github.com/raulmur/ORB_SLAM2.git ORB_SLAM2
```

We provide a script `build.sh` to build the *Thirdparty* libraries and *ORB-SLAM2*. Please make sure you have installed all required dependencies (see section 2). Execute:
```
cd ORB_SLAM2
chmod +x build.sh
./build.sh
```

This will create **libORB_SLAM2.so**  at *lib* folder and the executables **mono_tum**, **mono_kitti**, **rgbd_tum**, **stereo_kitti**, **mono_euroc** and **stereo_euroc** in *Examples* folder.

# 4. Monocular Examples

## TUM Dataset

1. Download a sequence from http://vision.in.tum.de/data/datasets/rgbd-dataset/download and uncompress it.

2. Execute the following command. Change `TUMX.yaml` to TUM1.yaml,TUM2.yaml or TUM3.yaml for freiburg1, freiburg2 and freiburg3 sequences respectively. Change `PATH_TO_SEQUENCE_FOLDER`to the uncompressed sequence folder.
```
./Examples/Monocular/mono_tum Vocabulary/ORBvoc.txt Examples/Monocular/TUMX.yaml PATH_TO_SEQUENCE_FOLDER
```

## KITTI Dataset  

1. Download the dataset (grayscale images) from http://www.cvlibs.net/datasets/kitti/eval_odometry.php 

2. Execute the following command. Change `KITTIX.yaml`by KITTI00-02.yaml, KITTI03.yaml or KITTI04-12.yaml for sequence 0 to 2, 3, and 4 to 12 respectively. Change `PATH_TO_DATASET_FOLDER` to the uncompressed dataset folder. Change `SEQUENCE_NUMBER` to 00, 01, 02,.., 11. 
```
./Examples/Monocular/mono_kitti Vocabulary/ORBvoc.txt Examples/Monocular/KITTIX.yaml PATH_TO_DATASET_FOLDER/dataset/sequences/SEQUENCE_NUMBER
```

## EuRoC Dataset

1. Download a sequence (ASL format) from http://projects.asl.ethz.ch/datasets/doku.php?id=kmavvisualinertialdatasets

2. Execute the following first command for V1 and V2 sequences, or the second command for MH sequences. Change PATH_TO_SEQUENCE_FOLDER and SEQUENCE according to the sequence you want to run.
```
./Examples/Monocular/mono_euroc Vocabulary/ORBvoc.txt Examples/Monocular/EuRoC.yaml PATH_TO_SEQUENCE_FOLDER/mav0/cam0/data Examples/Monocular/EuRoC_TimeStamps/SEQUENCE.txt 
```

```
./Examples/Monocular/mono_euroc Vocabulary/ORBvoc.txt Examples/Monocular/EuRoC.yaml PATH_TO_SEQUENCE/cam0/data Examples/Monocular/EuRoC_TimeStamps/SEQUENCE.txt 
```

# 5. Stereo Examples

## KITTI Dataset

1. Download the dataset (grayscale images) from http://www.cvlibs.net/datasets/kitti/eval_odometry.php 

2. Execute the following command. Change `KITTIX.yaml`to KITTI00-02.yaml, KITTI03.yaml or KITTI04-12.yaml for sequence 0 to 2, 3, and 4 to 12 respectively. Change `PATH_TO_DATASET_FOLDER` to the uncompressed dataset folder. Change `SEQUENCE_NUMBER` to 00, 01, 02,.., 11. 
```
./Examples/Stereo/stereo_kitti Vocabulary/ORBvoc.txt Examples/Stereo/KITTIX.yaml PATH_TO_DATASET_FOLDER/dataset/sequences/SEQUENCE_NUMBER
```

## EuRoC Dataset

1. Download a sequence (ASL format) from http://projects.asl.ethz.ch/datasets/doku.php?id=kmavvisualinertialdatasets

2. Execute the following first command for V1 and V2 sequences, or the second command for MH sequences. Change PATH_TO_SEQUENCE_FOLDER and SEQUENCE according to the sequence you want to run.
```
./Examples/Stereo/stereo_euroc Vocabulary/ORBvoc.txt Examples/Stereo/EuRoC.yaml PATH_TO_SEQUENCE/mav0/cam0/data PATH_TO_SEQUENCE/mav0/cam1/data Examples/Stereo/EuRoC_TimeStamps/SEQUENCE.txt
```
```
./Examples/Stereo/stereo_euroc Vocabulary/ORBvoc.txt Examples/Stereo/EuRoC.yaml PATH_TO_SEQUENCE/cam0/data PATH_TO_SEQUENCE/cam1/data Examples/Stereo/EuRoC_TimeStamps/SEQUENCE.txt
```

# 6. RGB-D Example

## TUM Dataset

1. Download a sequence from http://vision.in.tum.de/data/datasets/rgbd-dataset/download and uncompress it.

2. Associate RGB images and depth images using the python script [associate.py](http://vision.in.tum.de/data/datasets/rgbd-dataset/tools). We already provide associations for some of the sequences in *Examples/RGB-D/associations/*. You can generate your own associations file executing:

  ```
  python associate.py PATH_TO_SEQUENCE/rgb.txt PATH_TO_SEQUENCE/depth.txt > associations.txt
  ```

3. Execute the following command. Change `TUMX.yaml` to TUM1.yaml,TUM2.yaml or TUM3.yaml for freiburg1, freiburg2 and freiburg3 sequences respectively. Change `PATH_TO_SEQUENCE_FOLDER`to the uncompressed sequence folder. Change `ASSOCIATIONS_FILE` to the path to the corresponding associations file.

  ```
  ./Examples/RGB-D/rgbd_tum Vocabulary/ORBvoc.txt Examples/RGB-D/TUMX.yaml PATH_TO_SEQUENCE_FOLDER ASSOCIATIONS_FILE
  ```

# 7. ROS Examples

### Building the nodes for mono, monoAR, stereo and RGB-D
1. Add the path including *Examples/ROS/ORB_SLAM2* to the ROS_PACKAGE_PATH environment variable. Open .bashrc file and add at the end the following line. Replace PATH by the folder where you cloned ORB_SLAM2:

  ```
  export ROS_PACKAGE_PATH=${ROS_PACKAGE_PATH}:PATH/ORB_SLAM2/Examples/ROS
  ```
  
2. Execute `build_ros.sh` script:

  ```
  chmod +x build_ros.sh
  ./build_ros.sh
  ```
  
### Running Monocular Node
For a monocular input from topic `/camera/image_raw` run node ORB_SLAM2/Mono. You will need to provide the vocabulary file and a settings file. See the monocular examples above.

  ```
  rosrun ORB_SLAM2 Mono PATH_TO_VOCABULARY PATH_TO_SETTINGS_FILE
  ```
  
### Running Monocular Augmented Reality Demo
This is a demo of augmented reality where you can use an interface to insert virtual cubes in planar regions of the scene.
The node reads images from topic `/camera/image_raw`.

  ```
  rosrun ORB_SLAM2 MonoAR PATH_TO_VOCABULARY PATH_TO_SETTINGS_FILE
  ```
  
### Running Stereo Node
For a stereo input from topic `/camera/left/image_raw` and `/camera/right/image_raw` run node ORB_SLAM2/Stereo. You will need to provide the vocabulary file and a settings file. If you **provide rectification matrices** (see Examples/Stereo/EuRoC.yaml example), the node will recitify the images online, **otherwise images must be pre-rectified**.

  ```
  rosrun ORB_SLAM2 Stereo PATH_TO_VOCABULARY PATH_TO_SETTINGS_FILE ONLINE_RECTIFICATION
  ```
  
**Example**: Download a rosbag (e.g. V1_01_easy.bag) from the EuRoC dataset (http://projects.asl.ethz.ch/datasets/doku.php?id=kmavvisualinertialdatasets). Open 3 tabs on the terminal and run the following command at each tab:
  ```
  roscore
  ```
  
  ```
  rosrun ORB_SLAM2 Stereo Vocabulary/ORBvoc.txt Examples/Stereo/EuRoC.yaml true
  ```
  
  ```
  rosbag play --pause V1_01_easy.bag /cam0/image_raw:=/camera/left/image_raw /cam1/image_raw:=/camera/right/image_raw
  ```
  
Once ORB-SLAM2 has loaded the vocabulary, press space in the rosbag tab. Enjoy!. Note: a powerful computer is required to run the most exigent sequences of this dataset.

### Running RGB_D Node
For an RGB-D input from topics `/camera/rgb/image_raw` and `/camera/depth_registered/image_raw`, run node ORB_SLAM2/RGBD. You will need to provide the vocabulary file and a settings file. See the RGB-D example above.

  ```
  rosrun ORB_SLAM2 RGBD PATH_TO_VOCABULARY PATH_TO_SETTINGS_FILE
  ```
  
# 8. Processing your own sequences
You will need to create a settings file with the calibration of your camera. See the settings file provided for the TUM and KITTI datasets for monocular, stereo and RGB-D cameras. We use the calibration model of OpenCV. See the examples to learn how to create a program that makes use of the ORB-SLAM2 library and how to pass images to the SLAM system. Stereo input must be synchronized and rectified. RGB-D input must be synchronized and depth registered.

# 9. SLAM and Localization Modes
You can change between the *SLAM* and *Localization mode* using the GUI of the map viewer.

### SLAM Mode
This is the default mode. The system runs in parallal three threads: Tracking, Local Mapping and Loop Closing. The system localizes the camera, builds new map and tries to close loops.

### Localization Mode
This mode can be used when you have a good map of your working area. In this mode the Local Mapping and Loop Closing are deactivated. The system localizes the camera in the map (which is no longer updated), using relocalization if needed. 

