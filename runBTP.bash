echo "running main command"
python3 ./evaluationMetrics/associate.py $1/rgb.txt $1/depth.txt > $1/associations.txt
./Examples/RGB-D/rgbd_tum Vocabulary/ORBvoc.txt ./Examples/RGB-D/TUM3.yaml $1 $1/associations.txt ./$2_semanticSegmentationResults ./$2_objectDetectionResults $2
echo "done"
python3 ./evaluationMetrics/evaluate_rpe.py $1/groundtruth.txt ./CameraTrajectory$2.txt --verbose
python3 ./evaluationMetrics/evaluate_ate.py $1/groundtruth.txt ./CameraTrajectory$2.txt --verbose
echo "/home/cse/segmentationTest/segmentationResults"
echo "/home/cse/segmentationTest/objectDetectionResults"
echo "/home/cse/segmentationTest/raftFlowResults_$1 "
echo "132289"
