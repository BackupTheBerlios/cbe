extern "C" {
#include <termios.h>
}

#define BAUDRATE B115200

class SerialClient {
public:
  SerialClient(const char * serial_device);
  ~SerialClient();

  void requestData();  // Requests and reads new eye-position
  void onlyRequestData(); // only in conjuction with onlyGetData()
  void onlyGetData(); // then the same semantics as requestData()
  bool isBlink(); // Checks for eye-blink
 
  // Position of eye (x and y values)
  int x;
  int y;
  bool blink;

  // Serial-Device is available
  bool isAvailable;

private:
  int devptr;
  struct termios newtio;  // for the new serialport settings
  bool stillBlink;
};
