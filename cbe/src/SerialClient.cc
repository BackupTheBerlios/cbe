extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
}
#include <cstdlib>
#include <iostream>
#include "SerialClient.hh"

SerialClient::SerialClient(const char * dev_name) {
  isAvailable=false;
  x=0;
  y=0;
  bzero(&newtio, sizeof(newtio)); // Make everything in newtio -> 0
  devptr=open(dev_name, O_RDWR | O_NOCTTY); // Open the device for read-write mode in no CTTY-mode RTFM
  if (devptr<0) {
    cout << "Unable to open " << dev_name << endl;
  }
  else {
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS7 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0; // input-mode: non-canonical, no echo ...
    newtio.c_cc[VTIME] = 0; // inter-character timer unused
    newtio.c_cc[VMIN] = 4; // blocking read until 4 chars recieved

    tcflush(devptr, TCIFLUSH);
    tcsetattr(devptr, TCSANOW, &newtio);
    isAvailable=true;
  }
}

SerialClient::~SerialClient() {
}

// wrires "r" to the serialport as request for new data and waits for 4 chars (7 bit each)
void SerialClient::requestNewData() {
  char buffer[4];
  if (isAvailable) {
    write(devptr,"r",1); // request data
    read(devptr,buffer,4); //read 4 chars, blocking
    // Ok, data is here ...
    x=buffer[0]*128+buffer[1];
    y=buffer[2]*128+buffer[3];
  }
}

bool SerialClient::isBlink() {
  if (x==0 || y==0)
    return true;
  else
    return false;
}
