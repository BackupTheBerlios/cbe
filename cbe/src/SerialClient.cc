#include <cstdlib>
#include <iostream>
#include "SerialClient.hh"

using namespace std;

#ifndef _WIN32 // Linux Version
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
}

#include <cstdlib>
#include <cstring>
#include <iostream>
#include "SerialClient.hh"


SerialClient::SerialClient(const char * dev_name) {
  isAvailable=false;
  isRandom=false;
  change=0; // default to make no change
  status=1; // default to run the simulation
  wasChange=false; // default to "no change occured"
  x=0;
  y=0;
  if (strcmp(dev_name,"random")==0) {
    isRandom=true;
    cout << "Using random eye-events" << endl;
  }
  else {
    bzero(&newtio, sizeof(newtio)); // Make everything in newtio -> 0
    devptr=open(dev_name, O_RDWR | O_NOCTTY); // Open the device for read-write mode in no CTTY-mode -- RTFM
    if (devptr<0) {
      cerr << "Unable to open " << dev_name << endl;
      cerr << "Expect no further data from eye-position-server" << endl;
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
}

SerialClient::~SerialClient() {
}

// wrires "r" to the serialport as request for new data and waits for 4 chars (7 bit each)
void SerialClient::requestData() {
  onlyRequestData();
  onlyGetData();
}

// only requests data (for ues with onlyGetData() ONLY!!! -- you HAVE to pair them!!!)
void SerialClient::onlyRequestData() {
  if (isAvailable) {
    tcflush(devptr,TCIOFLUSH);
    if (wasChange==false)  // no change since last request
      write(devptr,"n",1); // request data with 'n'
    else {                 // change since last request
      write(devptr,"o",1); // request data with 'o'
      wasChange=false;     // reset flag (so changes are reported only once)
    }
  }
}

void SerialClient::onlyGetData() {
  char buffer[20];
  if (isAvailable) {
    read(devptr,buffer,10); // read 10 chars, blocking
                            // 4 chars x, 4 chars y, 1 status, 1 '\n'
    // Ok, data is here ...
    // calculate x and y values
    for (int i=2; i<10;i++) // this loop is needed as we must be able to interprete
      if (buffer[i]==' ')   // ' ' as 0  (eg.: '  57' = '0057')
	buffer[i]='0';
    x=(buffer[2]-48)*1000+(buffer[3]-48)*100+(buffer[4]-48)*10+(buffer[5]-48);
    y=(buffer[6]-48)*1000+(buffer[7]-48)*100+(buffer[8]-48)*10+(buffer[9]-48);
    change=buffer[1];
    status=buffer[0];
  }
}


#else // Windows Version, Dummy

#include "Random.h"

SerialClient::SerialClient(const char * dev_name)
{
    isRandom=true;
    cout << "Using random eye-events" << endl;
	resetRandom();
}

SerialClient::~SerialClient() {
}

void SerialClient::requestData() {
	x = rndInt( 32768 );
	y = rndInt( 32768 );
}

void SerialClient::onlyRequestData() {
}

void SerialClient::onlyGetData() {
	x = rndInt( 32768 );
	y = rndInt( 32768 );
}

#endif



// returns run-reset-flag
bool SerialClient::isRun() {
  if (status==0)
    return false;
  else
    return true;
}

void SerialClient::setChange() {
  wasChange=true;
}

// Returns x
int SerialClient::getX() {
  if (isRandom) {
    return rand()%4096;
  }
  else
    return x;
}

// Returns y
int SerialClient::getY() {
  if (isRandom) {
    return rand()%4096;
  }
  else
    return y;
}

// returns what to change
int SerialClient::getChange() {
  if (isRandom) {
    if (rand()%10==0) { // make change
      return rand()%10;
    }
    else
      return 0;
  }
  return change;
}
  