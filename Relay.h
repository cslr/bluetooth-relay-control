

#include <stdio.h>
#include <string>
#include <vector>

#ifdef WINDOWS

#include <windows.h>

#endif



class Relay
{
 public:

  // opens connection or throws exception if error
  Relay(const std::string& port); // COM port, for example "COM7" in Windows

  // closes connection
  virtual ~Relay();

  // interface:
  
  std::string getVersion();

  bool getRelay(unsigned int relay);
  bool setRelay(unsigned int relay, bool value);

  bool getRelay(std::vector<bool>& relay);
  bool setRelay(const std::vector<bool>& relay);

  void reset(); // all relays to off
  

 private:

  std::string runCommand(std::string cmd);

#ifdef WINDOWS
  
  HANDLE hComPort = NULL;
  char cmdBuffer[64];
  char responseBuffer[64];
  DWORD numBytesWritten;
  DWORD numBytesRead;
  
#endif
  
};
