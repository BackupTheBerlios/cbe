#ifndef _WIN32
	extern "C" {
	#include <termios.h>
	}
#endif

#define BAUDRATE B115200

// some constants
#define REQUEST_NORMAL 0
#define REQUEST_CHANGED 1

#define CHANGE_NOTHING 0
#define CHANGE_HIDE_CAR 49
#define CHANGE_TOGGLE_BREAKLIGHTS 50

class SerialClient {
public:
  SerialClient(const char * serial_device);
  ~SerialClient();

  void requestData();  // Requests and reads new eye-position
  void onlyRequestData(); // only in conjuction with onlyGetData()
  void onlyGetData(); // then the same semantics as requestData()
  bool isRun(); // return true for "run" and false for "reset"
  int getChange(); // returns status of what to change (0:nothing, 1:hide_car ...) 
  void setChange(); // used to confirm changes to eye-position-server
  int getX(); // returns x-value of eye
  int getY(); // returns y-value of eye
  // Serial-Device is available
  bool isAvailable;

private:
  int devptr;
  #ifndef _WIN32
    struct termios newtio;  // for the new serialport settings
  #endif
  int change;
  int status;
  bool wasChange; // used to confirm changes back to the server
  bool isRandom; // possibility to create events randomly
  int x,y; // Position of eye (x and y values)
};
