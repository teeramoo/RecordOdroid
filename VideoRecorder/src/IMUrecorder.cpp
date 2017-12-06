
#include "IMUrecorder.h"

using namespace std;
using namespace boost::posix_time;

//std::ofstream writeFile;
//int fd = -1;


void IMUrecorder::error(const char * msg)
{
    fprintf(stderr, "%s\n", msg);
}


int IMUrecorder::set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf("Error from tcgetattr: %s\n", strerror(errno));
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf("Error from tcsetattr: %s\n", strerror(errno));
                return -1;
        }
        return 0;
}

void IMUrecorder::set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf("Error from tcgetattr: %s\n", strerror(errno));
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf("Error from tcsetattr: %s\n", strerror(errno));
}


IMUrecorder::IMUrecorder(std::string _portName, std::string _outputFile,  boost::posix_time::ptime const _referencedTime)
        : referencedTime(_referencedTime), outputFile(_outputFile)
{
    portName = _portName;
    cout << "IMUrecorder created." << endl;
    cout << "PortName is : " << portName << endl;
}

IMUrecorder::~IMUrecorder() {}

void IMUrecorder::start()
{
    cout << "filePath is : " << outputFile << endl;
    string tt = portName;
    cout << "from start --> tt is : " << tt << endl;
    cout << "from start --> PortName is : " << portName << endl;
    openPort();
    recordIMU();
}


int IMUrecorder::openPort()
{
    fd = -1;
    cout << "portName is " << portName << endl;
    fd = open (portName.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
        //printf("Error opening %s: %s\n", portname, strerror(errno));
        cout << "Error opening " << portName << " with error: " << strerror(errno) << endl;
        return -1;
    }

    set_interface_attribs (fd, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking (fd, 0);                // set no blocking

    write (fd, "hello!\n", 7);           // send 7 character greeting

    usleep ((7 + 25) * 100);             // sleep enough to transmit the 7 plus
    // receive 25:  approx 100 uS per char transmit

    //   char buf [512];
    //   int i, j, k, n;




    for(i = 0; i < 50; i++)
    {
        n = read (fd, buf, sizeof buf);  // read up to 100 characters if ready to read
        printf("Reading init: %s\n",buf);
        usleep(100);
    }

    usleep(1000);

    cout << "Initializing done ... " << endl;

    return 1;
}

int IMUrecorder::recordIMU()
{
    writeFile.open (outputFile + "IMUarduino.txt", std::ofstream::out | std::ofstream::app);
    if(!writeFile.is_open())
    {
        cout << "Could not open output file stream: "<< endl;
        exit(-1);
    }

    double ax = 0;
    double ay = 0;
    double az = 0;

    double wx = 0;
    double wy = 0;
    double wz = 0;

    ptime timestampFirst = microsec_clock::local_time();
    ptime timestampCurrent;
    ptime timestampBase(boost::gregorian::date(2015,1,1));
    writeFile << "Start," << timestampFirst << endl;
    int timeNow;
    //for (timeNow = 0, i = 0;timeNow < 10;i++)
    while(true)
    {
        n = read (fd, buf, sizeof(buf));
        timestampCurrent = microsec_clock::local_time();
        if(n>0)
        {
            //printf("%s\n",buf);
            string data(buf,n);
            //vector<string> vtokens;
            //tokenize(data,vtokens);
            //cout << "Read data: " << data << " with tokens: " << vtokens.size() << endl;
            //if(vtokens.size() == 8)
            //cout << buf << endl;
            stringstream ss;
            ss << (timestampCurrent - referencedTime).total_microseconds() << "," << data;

            if(data.length() > 0)
            {
                //writeFile << (timestampCurrent - timestampBase).total_microseconds() << "," << data;
                //writeFile.flush();
                writeFile << ss.str() << flush;
                writeFile.flush();
                cout << ss.str()<<endl;
            }
        }

        //timeNow = (microsec_clock::local_time()-timestampFirst).total_seconds();
    }
    return 1;
}
