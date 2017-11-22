//
// Created by teeramoo on 11/11/2560.
//

#include "VideoRecorder.h"

namespace pt = boost::posix_time;

void createDirectory(string _directoryName)
{
    if(!boost::filesystem::exists(_directoryName))
    {
        boost::filesystem::create_directory(_directoryName);
        cout << "Created directory : " << _directoryName << endl;
    }

    else if(boost::filesystem::exists(_directoryName))
        cout << _directoryName << " exists." << endl;
    else
        cout << "Error creating directory : " << _directoryName << endl;

}


int main(int argc, char ** argv)
{

    if(argc != 5)
    {
        cout << "Usage: ./recordVideo file_path image_width image_height video_fps" << endl;
        exit(-1);
    }
    string filePath = string(argv[1]);
    int imageWidth = stoi(string(argv[2]));
    int imageHeight = stoi(string(argv[3]));
    double fps = stod(string(argv[4]));
    // load Output file path, uartPort
 //   createDirectory("../Output_files/");

    pt::ptime now = pt::second_clock::local_time();
//    string filePath = "../Output_files/" + to_string(now) + "/";
//    createDirectory(filePath);

//    string uartPort = "/dev/ttyUSB0";
//    int baudRate = 57600; //921600
    bool debug = true;
//    int imageWidth = 640;
//    int imageHeight = 480;
//    double fps = 10.0;
    int codec = CV_FOURCC('M', 'J', 'P', 'G');
    boost::posix_time::ptime const referencedTime(boost::gregorian::date(2015, 1, 1));

    VideoRecorder vidRecorder(debug, imageWidth, imageHeight, fps, codec, filePath, referencedTime);
    vidRecorder.start();

//    boost::thread threadVideoRecorder(&VideoRecorder::start, vidRecorder);
//    while(true);

    return 0;
}

