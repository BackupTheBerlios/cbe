extern "C" {
#include <termios.h>
}

#define BAUDRATE B115200

class SerialClient {
public:
  SerialClient(const char * serial_device);
  ~SerialClient();

  void requestNewData();  // Requests and reads new eye-position
  bool isBlink(); // Checks for eye-blink

  // Position of eye (x and y values)
  int x;
  int y;
  
  // Serial-Device is available
  bool isAvailable;

private:
  int devptr;
  struct termios newtio;  // for the new serialport settings
};
