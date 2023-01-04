~/BTP/Examples/RGB-D/rgbd_tum Vocabulary/ORBvoc.txt ~/BTP/Examples/RGB-D/TUM3.yaml /home/cse/Downloads/rgbd_dataset_freiburg3_$1 /home/cse/Downloads/rgbd_dataset_freiburg3_$1/associations.txt
python3 ~/BTPtools/evaluate_rpe.py /home/cse/Downloads/rgbd_dataset_freiburg3_$1/groundtruth.txt ~/BTP/CameraTrajectory.txt --verbose
python3 ~/BTPtools/evaluate_ate.py /home/cse/Downloads/rgbd_dataset_freiburg3_$1/groundtruth.txt ~/BTP/CameraTrajectory.txt --verbose
echo "/home/cse/segmentationTest/segmentationResults"
echo "/home/cse/segmentationTest/objectDetectionResults"

