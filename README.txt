Installation
This code has been developed and tested on Ubuntu 20.04 version.
Clone the code by running : 
Complete installation by following the instructions given in : 
	1) https://github.com/raulmur/ORB_SLAM2
		Errors faced could be due to the difference in the version of opencv that is being used.
		The code uses opencv 4.2.0
	2) https://github.com/divamgupta/image-segmentation-keras/blob/master/README.md
	3) https://cv.gluon.ai/install.html
	4) https://github.com/princeton-vl/RAFT


Execution
	Download the required sequence from : https://cvg.cit.tum.de/data/datasets/rgbd-dataset/download
	Navigate into the tools directory and run
		1) python3 semanticSegmentation.py <absolute-path-of-sequence> <name-of-sequence>
		  For example if the downloaded sequence fr3/walking-xyz which is stored in the location /home/user/walking_xyz 
		  the command should be
		  python3 semanticSegmentation.py /home/user/walking_xyz walking_xyz
		
		2) python3 objectDetection.py <absolute-path-of-sequence> <name-of-sequence>
		  For example if the downloaded sequence fr3/walking-xyz which is stored in the location /home/user/walking_xyz 
		  the command should be
		  python3 objectDetection.py /home/user/walking_xyz walking_xyz

		3) Navigate into the tools/RAFT directory and run
		  python3 runModel.py <absolute-path-of-sequence> <name-of-sequence>
		  For example if the downloaded sequence fr3/walking-xyz which is stored in the location /home/user/walking_xyz 
		  the command should be
		  python3 runModel.py  --model=models/raft-things.pth --path=/home/user/walking_xyz/rgb --dataset=walking_rpy
		  Here rgb is the folder within the /home/user/walking_xyz directory that contains the images.

		4) ./build.sh
		5) ./runBTP.bash <location-of-sequence> <name-of-sequence>
		   If the downloaded sequence is fr3/walking_xyz which is stored in the location /home/user/walking_xyz 
		   the command should be
		   ./runBTP.bash /home/user/walking_xyz walking_xyz
