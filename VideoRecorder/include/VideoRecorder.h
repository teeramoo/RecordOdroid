//
// Created by teeramoo on 11/11/2560.
//

#ifndef RECORDVIDEO_RECORDVIDEO_H
#define RECORDVIDEO_RECORDVIDEO_H

#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdio.h>
#include <ctime>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace cv;
using namespace boost;
class VideoRecorder {
public:

    //function declaration
    VideoRecorder(bool _debug, int _imageWidth, int _imageHeight, double _videoFPS,
                  int _videoCodec, string _filePath, boost::posix_time::ptime const _referencedTime);
    ~VideoRecorder();

    bool findCameras();
    int findOneCamera(int _maxNum);
    void start();
    int record();
    void configCameras();
    bool getReadyStatus();
    int checkCameras();


    //variable declaration
private:
    //function declaration

    //variable declaration
    bool debug;
    VideoCapture camera1, camera2;
    Mat mImage1, mImage2;
    int imageWidth, imageHeight;
    VideoWriter videoWriter1, videoWriter2;
    string filePath;
    string videoTimestampFile;
    int videoCodec;
    double videoFPS;
    boost::posix_time::ptime const referencedTime;
    bool bReadyStatus;

};




#endif //RECORDVIDEO_RECORDVIDEO_H
