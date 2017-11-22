
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

using namespace std;
using namespace boost::posix_time;

std::ofstream writeFile;
int fd = -1;

void signalHandler( int signum ) 
{
   cout << "Interrupt signal (" << signum << ") received.\n";

   // cleanup and close up stuff here  
   // terminate program  
   if(writeFile.is_open())
   {
   		writeFile.close();
   		cout << "OUTPUT FILE CLOSED" << endl;
   }
   
   if(fd>=0)
   {
   		close(fd);
   }

   exit(signum);  

}

void tokenize(const string &str, vector<string> &vTokens)
{
    int iPos = 0;
    int iTokBeg = 0;
    while (iPos < (int) str.length())
    {
        if (str[iPos] == ',')
        {
            if (iTokBeg < iPos)
            {
                vTokens.push_back(str.substr(iTokBeg, iPos - iTokBeg));
                iTokBeg = iPos + 1;
            }
        }
        iPos++;
    }
    if (iTokBeg < (int) str.length())
        vTokens.push_back(str.substr(iTokBeg));
}

static void readline(char * line, FILE * fp)
{
    fgets(line, 1000, fp);
}

static void readdata(FILE * fp, double SV_Rho[3])
{
    char line[1000];

    readline(line, fp);

    char * p = strtok(line, ",");

    int j;

    for (j=0; j<3; ++j) {
        SV_Rho[j] = atof(p);
        p = strtok(NULL, ",");
    }
}


void error(const char * msg)
{
    fprintf(stderr, "%s\n", msg);
}


int set_interface_attribs (int fd, int speed, int parity)
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

void set_blocking (int fd, int should_block)
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


int main(int argc, char ** argv)
{    
	if(argc != 3)
	{
		cout << "Usage: ./readIMU usbPort outputFile" << endl;
		exit(-1);
	}
	
	signal(SIGINT, signalHandler);  
	
	string portname = string(argv[1]); //"/dev/ttyUSB0";
	string outputFile = string(argv[2]);
	//char *portname = "/dev/ttyUSB0";
	fd = open (portname.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0)
	{
		    //printf("Error opening %s: %s\n", portname, strerror(errno));
		    cout << "Error opening " << portname << " with error: " << strerror(errno) << endl;
		    return -1;
	}

	set_interface_attribs (fd, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
	set_blocking (fd, 0);                // set no blocking

	write (fd, "hello!\n", 7);           // send 7 character greeting

	usleep ((7 + 25) * 100);             // sleep enough to transmit the 7 plus
		                                 // receive 25:  approx 100 uS per char transmit
		                                 
	char buf [512];
	int i, j, k, n;
	
	/*
	void writeFile(const string &strDataFile, vector<stallInfo> vOutput)
{
	std::ofstream myfile;
	myfile.open (strDataFile.c_str());
	for(int i = 0; i < vOutput.size(); ++i)
	{
		std::stringstream outss;
		outss << "Case #"<< i+1 << ": ";
		outss << max(vOutput[i].llLeft,vOutput[i].llRight) << " ";
		outss << min(vOutput[i].llLeft,vOutput[i].llRight) << endl;
		myfile << outss.str();
	}
	myfile.close();
}*/
	
	writeFile.open (outputFile, std::ofstream::out | std::ofstream::app);
	if(!writeFile.is_open())
	{
		cout << "Could not open output file stream: "<< endl;
		exit(-1);
	}
	
	
	for(i = 0; i < 50; i++)
	{
		n = read (fd, buf, sizeof buf);  // read up to 100 characters if ready to read
		printf("Reading init: %s\n",buf);
		usleep(100);
	}
	
	usleep(1000);
	
	cout << "Initializing done ... " << endl;
	
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
    		ss << (timestampCurrent - timestampBase).total_microseconds() << "," << data;
    		
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
    ptime timestampLast = microsec_clock::local_time();
    timeNow = (timestampLast-timestampFirst).total_seconds();
	cout<<"Time taken: "<<timeNow<< endl;
	cout<<"Writing done"<<endl;
	writeFile.close();
    close(fd);
    return 0;
}
