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

using namespace std;

void LoadImages(const string &strAssociationFilename, vector<string> &vstrImageFilenamesRGB,
                vector<string> &vstrImageFilenamesD, vector<double> &vTimestamps);


string deriveSegmentName(string &s) {
    int len = s.size();
    string fin = s.substr(4,len-8);
    string suffix = "_segment.png";
    return (fin+suffix);
}

string deriveObjectName(string &s) {
    int len = s.size();
    string fin = s.substr(4,len-8);
    string suffix = "_objectDetect";
    return (fin+suffix);
}

string deriveFlowName(string &s) {
    int len = s.size();
    string fin = s.substr(4,len-8);
    string suffix = "_flow";
    return (fin+suffix);
}
void retrieveObjectsFromImage(const string &folder,string &name, vector<vector<float>> &dynamicObjects) {
    ifstream inputFile(folder+"/"+deriveObjectName(name));
    int t;
    inputFile >> t;
    cout<<"detected objects "<<t<<"\n";
    for(int i=0;i<t;++i) {
        vector<float> temp;
        for(int j=0;j<5;++j) {
            float f;
            inputFile >> f;
            temp.push_back(f);
        }
        dynamicObjects.push_back(temp);
    }
    inputFile.close();
}

bool isInPerson( float cx,float cy,  const std::vector<std::vector<float>> &detect_result)
{
  bool InPerson = false;

//   cout<<"size : "<<detect_result.size()<<"\n";
  for(auto temp: detect_result)
  {
      int sz = temp.size();
      
      if(sz == 0)
        break;
    //   assert(sz >= 5);
    //   assert(coordinate.y < 480);
    //   assert(temp[3] < 480);
    //   if(temp[3] > 480) {
    //     cout<<"wrong\n";
    //   }
    //   cout<<temp[1]<<" "<<temp[2]<<" "<<temp[3]<<" "<<temp[4]<<"\n";
      if((cx > temp[1])&& (cy > temp[2]) && (cx < temp[3]) && (cy < temp[4]))
      {
        InPerson = true;
        break;
      }
  }
  return InPerson;
}

void retrieveFlowDetails(const string &folder, string &name , vector<vector<pair<float,float>>> &flowResults, float &medianX, float &medianY,vector<vector<float>> &dynamicObjects) {
    ifstream inputFile(folder+"/"+deriveFlowName(name));
    float x, y;
    inputFile >> x >> y;
    medianX = x;
    medianY = y;
    float mx = 0.0;
    float my = 0.0;
    vector<float>sortedX,sortedY;
    int counter = 0;
    for(int j=0;j<480;++j) {
        vector<pair<float,float>> temp;
        for(int k=0;k<640;++k) {
            inputFile >> x >> y;
            temp.push_back({x,y});
            if(!isInPerson(k,j,dynamicObjects)) {
                sortedX.push_back(x);
                sortedY.push_back(y);
                counter++;
            }

                mx+=x;
                my+=y;
        }
        flowResults.push_back(temp);
    }

    // sort(sortedX.begin(),sortedX.end());
    // sort(sortedY.begin(),sortedY.end());


    // medianX = mx/(640*480);
    // medianY = my/(640*480);

    cout<<medianX<<" "<<medianY<<"\n";
    inputFile.close();
}

int main(int argc, char **argv)
{
    if(argc != 8)
    {
        cerr << endl << "Usage: ./rgbd_tum path_to_vocabulary path_to_settings path_to_sequence path_to_association" << endl;
        return 1;
    }

    for(int i=0;i<argc;++i) {
        cout<<i<<" "<<argv[i]<<"\n";
    }
    // Retrieve paths to images
    vector<string> vstrImageFilenamesRGB;
    vector<string> vstrImageFilenamesD;
    vector<double> vTimestamps;
    string strAssociationFilename = string(argv[4]);
    LoadImages(strAssociationFilename, vstrImageFilenamesRGB, vstrImageFilenamesD, vTimestamps);
    // return 1;

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
    ORB_SLAM2::System SLAM(argv[1],argv[2],ORB_SLAM2::System::RGBD,true);

    // Vector for tracking time statistics
    vector<float> vTimesTrack;
    vTimesTrack.resize(nImages);

    cout << endl << "-------" << endl;
    cout << "Start processing sequence ..." << endl;
    cout << "Images in the sequence: " << nImages << endl << endl;

    // Main loop
    cv::Mat imRGB, imD, segmentationOutput;
    // cv::Mat segmentedImage= cv::imread("/home/cse/segmentationTest/out.png" ,IMREAD_UNCHANGED);

    // cout<<segmentedImage.rows<<" "<<segmentedImage.cols<<" "<<segmentedImage.channels()<<"\n";

    


    // int cols = segmentedImage.cols-1;
    // for(int i=0;i<segmentedImage.rows;++i) {
    //     int r= segmentedImage.at<Vec3b>(i,cols)[0];
    //     int g= segmentedImage.at<Vec3b>(i,cols)[1];
    //     int b= segmentedImage.at<Vec3b>(i,cols)[2];
    //     cout<<r<<" "<<g<<" "<<b<<"\n";
    // }

    for(int ni=0; ni<nImages; ni++)
    {

        // cout<<(string(argv[3]) + "/" + vstrImageFilenamesRGB[ni])<<"\n";

        // Read image  and depthmap from file
        imRGB = cv::imread(string(argv[3])+"/"+vstrImageFilenamesRGB[ni],IMREAD_UNCHANGED);
        imD = cv::imread(string(argv[3])+"/"+vstrImageFilenamesD[ni],IMREAD_UNCHANGED);

        string nameOfSegmentedFile = deriveSegmentName(vstrImageFilenamesRGB[ni]);
        // cout<<vstrImageFilenamesRGB[ni]<<" "<<nameOfSegmentedFile<<" "<<(string(argv[5]) + "/" + nameOfSegmentedFile)<<"\n";
        // return 1;
        segmentationOutput = cv::imread(string(argv[5]) + "/" + nameOfSegmentedFile,IMREAD_UNCHANGED);

        if(segmentationOutput.empty()) {

            cout<<"Failed to load segmentation result at : " <<(string(argv[5]) + "/" + nameOfSegmentedFile)<<"\n";
            return 1;
        }

        // cout<<imRGB.rows<<" "<<imRGB.cols<<"\n";
        // return 1;

        vector<vector<float>> dynamicObjects;
        retrieveObjectsFromImage(string(argv[6]),vstrImageFilenamesRGB[ni],dynamicObjects);

        vector<vector<pair<float,float>>> flowResults;
        float medianX=-1,medianY=-1;
        if(ni != 0) {
            retrieveFlowDetails(string(argv[7]) , vstrImageFilenamesRGB[ni], flowResults, medianX, medianY,dynamicObjects);
            int rows = flowResults.size();
            int cols = flowResults[0].size();
            cout<<"dimension of flowResults is "<<rows<<" "<<cols<<"\n";
        }

        cout<<(segmentationOutput.rows)<<" "<<(segmentationOutput.cols)<<"\n";
        cout<<"processing : "<<vstrImageFilenamesRGB[ni]<<" segmentation file is : "<<(string(argv[5]) + "/" + nameOfSegmentedFile)<<"\n";
        int sz = dynamicObjects.size();
        // cout<<sz<<"\n";
        // for(int i=0;i<sz;++i) {
        //     for(auto &it : dynamicObjects[i]) {
        //         cout<<it<<" ";
        //     }
        //     cout<<"\n";
        // }


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



    int blueVal= segmentationOutput.at<Vec3b>(209,527)[0];
    int greenVal= segmentationOutput.at<Vec3b>(209,527)[1];
    int redVal= segmentationOutput.at<Vec3b>(209,527)[2];

    cout<<"output of pixel : 527 209 is "<<(blueVal)<<" "<<(greenVal)<<" "<<(redVal)<<"  \n";
        // Pass the image to the SLAM system
        SLAM.TrackRGBD(imRGB,imD,tframe,segmentationOutput,dynamicObjects,medianX,medianY,flowResults);
        // if(vstrImageFilenamesRGB[ni] == "rgb/1341846313.789969.png") {
        //     return 0;   
        // }
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
    SLAM.SaveTrajectoryTUM("CameraTrajectory.txt");
    SLAM.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");   

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
