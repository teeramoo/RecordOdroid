//
// Created by teeramoo on 11/11/2560.
//

#include "VideoRecorder.h"
#include "mavlink_control.h"
#include "IMUrecorder.h"
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

//int main(int, char**)
int main(int argc, char **argv)
{
    if(argc != 6)
    {
        cout << "Usage : ./recordVideo <u-art_port> <baud_rate> <image_width> <image_height> <video_fps>" << endl;
        return -1;
    }

    // Setup paths and parameters
    createDirectory("Output_files/");
    pt::ptime now = pt::second_clock::local_time();
    string filePath = "Output_files/" + to_string(now) + "/";
    createDirectory(filePath);

    string uartPort = argv[1];
    int baudRate = stoi(argv[2]); // 57600 or 921600
    int imageWidth = stoi(argv[3]);
    int imageHeight = stoi(argv[4]);
    double fps = stod(argv[5]);
    boost::posix_time::ptime const referencedTime(boost::gregorian::date(2015, 1, 1));
    bool debug = true;
    int codec = CV_FOURCC('M', 'J', 'P', 'G');

//    string uartPort = "/dev/ttyUSB0";
//    int baudRate = 57600; //921600
//    bool debug = true;
//    int imageWidth = 640;
//    int imageHeight = 480;
//    double fps = 10.0;
//    int codec = CV_FOURCC('M', 'J', 'P', 'G');

    MAVLinkControl mavlinkControl(uartPort, baudRate, filePath, referencedTime);
    mavlinkControl.start();

    VideoRecorder vidRecorder(debug, imageWidth, imageHeight, fps, codec, filePath, referencedTime);
    boost::thread threadVideoRecorder(&VideoRecorder::start, vidRecorder);

    boost::posix_time::ptime checkTime = boost::posix_time::microsec_clock::local_time();

    while(!mavlinkControl.getReadyStatus() && !vidRecorder.getReadyStatus())
    {
        cout << "Waiting for both thread to be ready." << endl;
        usleep(500000);
        auto elapseTime = (boost::posix_time::microsec_clock::local_time() - checkTime).total_microseconds();

        if(elapseTime > 5*10E7)
        {
            cout << "Terminate after waiting for 5 seconds." << endl;
            if(!mavlinkControl.getReadyStatus())
                cout << " Mavlink is not ready" << endl;
            if(!vidRecorder.getReadyStatus())
                cout << " Video recorder is not ready" << endl;

            return -1;
        }
        cout << "Wait time : " << elapseTime << endl;
    }

    while(true);

//    IMUrecorder imuRecorder(uartPort, filePath, referencedTime);
//    boost::thread threadIMUrecorder(&IMUrecorder::start, boost::ref(imuRecorder));

//    while(true);

    return 0;
}

