//
// Created by teeramoo on 21/11/2560.
//

#ifndef RECORDVIDEO_READIMU_H
#define RECORDVIDEO_READIMU_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <csignal>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class IMUrecorder {
public:
    // function declaration
   IMUrecorder(std::string _portName, std::string _outputFile,  boost::posix_time::ptime const _referencedTime);
   ~IMUrecorder();
    int recordIMU();
    int openPort();
    void start();

    void tokenize(const std::string &str, std::vector<std::string> &vTokens);
    static void readline(char * line, FILE * fp);
    static void readdata(FILE * fp, double SV_Rho[3]);
    void error(const char * msg);
    int set_interface_attribs (int fd, int speed, int parity);
    void set_blocking (int fd, int should_block);


private:
    std::ofstream writeFile;
    int fd ;
    char buf [512];
    int i, j, k, n;
    boost::posix_time::ptime const referencedTime;
    std::string portName;
    std::string outputFile;
};

//void signalHandler( int signum );

#endif //RECORDVIDEO_READIMU_H
