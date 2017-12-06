
#include "VideoRecorder.h"

bool VideoRecorder::getReadyStatus()
{
    return bReadyStatus;
}

VideoRecorder::~VideoRecorder() {}

VideoRecorder::VideoRecorder(bool _debug, int _imageWidth, int _imageHeight, double _fps,
                             int _videoCodec, string _filePath, boost::posix_time::ptime const _referencedTime)
        : debug(_debug), imageWidth(_imageWidth), imageHeight(_imageHeight),
          videoFPS(_fps), videoCodec(_videoCodec), filePath(_filePath), referencedTime(_referencedTime) {}

void VideoRecorder::configCameras()
{
    camera1.set(CAP_PROP_FRAME_WIDTH, imageWidth);
    camera1.set(CAP_PROP_FRAME_HEIGHT, imageHeight);
    camera1.set(CAP_PROP_FPS, videoFPS);

    camera2.set(CAP_PROP_FRAME_WIDTH, imageWidth);
    camera2.set(CAP_PROP_FRAME_HEIGHT, imageHeight);
    camera2.set(CAP_PROP_FPS, videoFPS);

    cout << "Seting up cameras with 640x480 quality at 30 FPS" << endl;

}

void VideoRecorder::start()
{
    if(!findCameras())
    {
        cerr << "Error opening cameras." <<endl;
        throw;
    }
    configCameras();

    if(checkCameras() != 1)
    {
        cout << "error checking camera" << endl;
        bReadyStatus = false;
    }


    record();
}

int VideoRecorder::checkCameras()
{
    // check if we succeeded
    if (!camera1.read(mImage1)) {
        cerr << "ERROR! blank frame grabbed from camera 1\n";
        return -1 ;
    }
    if (!camera2.read(mImage2)) {
        cerr << "ERROR! blank frame grabbed from camera 2\n";
        return -2;
    }
    mImage1.release();
    mImage2.release();
    return 1;
}

bool VideoRecorder::findCameras()
{
    int camIndex1 = findOneCamera(5); // Camera index 1

    if(camIndex1 == -1)
    {
        cout << "Cannot find any camera." <<endl;
        return false;
    }
    camera1 = VideoCapture(camIndex1);

    int camIndex2 = findOneCamera(camIndex1-1); //Camera index 2

    if(camIndex2 == -1)
    {
        cout << "Cannot find second camera." <<endl;
        return false;
    }
    camera2 = VideoCapture(camIndex2);

    return  true;
}

int VideoRecorder::findOneCamera(int _maxNum)
{
    for (int i = _maxNum; i >= 0; i--)
    {
        cout << "Try camera index : " << i << endl;
        VideoCapture checkCamera(i);
        if(checkCamera.isOpened())
        {
            cout << "Found camera index : " << i << endl;
            checkCamera.release();
            return i;
        }
        else
            checkCamera.release();
    }
    return -1;
}

int VideoRecorder::record()
{
    
    videoWriter1.open(filePath + "vid1.avi", videoCodec, videoFPS, Size(imageWidth,imageHeight));
    if (!videoWriter1.isOpened()) {
        cerr << "Could not open the output video file 1 for write\n";
        return -1;
    }
    videoWriter2.open(filePath + "vid2.avi", videoCodec, videoFPS, Size(imageWidth,imageHeight));
    if (!videoWriter2.isOpened()) {
        cerr << "Could not open the output video file 2 for write\n";
        return -1;
    }

    //--- GRAB AND WRITE LOOP
    cout << "Writing videofiles..." << endl;

    int frame = 0;
    ofstream vidTimeWriter;
    string timestampPath = filePath + "video_timestamp.txt";
    vidTimeWriter.open(timestampPath);

    for (;;)
    {

        // encode the frame into the videofile stream
    //    imshow("Live1", mImage1);
    //    imshow("Live2", mImage2);
        camera1.read(mImage1);
        camera2.read(mImage2);
        videoWriter1.write(mImage1);
        videoWriter2.write(mImage2);

        mImage1.release();
        mImage2.release();
        auto ms = (boost::posix_time::microsec_clock::local_time() - referencedTime).total_microseconds();
        vidTimeWriter << ms << "," << frame << endl;

        frame++;

        if (waitKey(5) >= 0)
            break;
    }
    vidTimeWriter.close();
}