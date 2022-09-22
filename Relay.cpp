
#include "Relay.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <exception>
#include <stdexcept>


// opens connection or throws exception if error
Relay::Relay(const std::string& port) // COM port, for example "COM7" in Windows
{
#ifdef WINDOWS
  char PortName[256];

  // L"\\\\.\\COM37";
  // wsnprintf(PortName, 256, L"\\\\.\\%s", port.c_str());
  snprintf(PortName, 256, "\\\\.\\%s", port.c_str());

  hComPort = INVALID_HANDLE_VALUE;
  
  hComPort = CreateFile(PortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
  
  if(hComPort == INVALID_HANDLE_VALUE)
  {
    std::string error = "Error: Unable to open the specified port\n";
    throw std::logic_error(error);
  }

  cmdBuffer[0] = 0x0D;
  
  if(!WriteFile(hComPort, cmdBuffer, 1, &numBytesWritten, NULL))
  {
    CloseHandle(hComPort);
    hComPort = INVALID_HANDLE_VALUE;
    std::string error = "Error: Unable to write to the specified port\n";
    throw std::logic_error(error);
  }
  
#else
  // not implemented (Linux)
#endif
  
}

// closes connection
Relay::~Relay()
{
#ifdef WINDOWS
  
  /* Close the comm port handle */
  if(hComPort != INVALID_HANDLE_VALUE){
    CloseHandle(hComPort);
    hComPort = INVALID_HANDLE_VALUE;
  }
  
#else
  // NOT IMPLEMENTED (Linux)
#endif
  
}


std::string Relay::runCommand(std::string cmd)
{
#ifdef WINDOWS

  // printf("Info: Relay cmd: '%s' %d\n", cmd.c_str(), cmd.length());

  cmd += std::string("\r");

  cmdBuffer[0] = 0x0D;
  
  if(!WriteFile(hComPort, cmdBuffer, 1, &numBytesWritten, NULL))
  {
    //CloseHandle(hComPort);
    //printf("Error: Unable to write to the specified port\n");
    //return 1;
    return "";
  }
  
  /* Flush the Serial port's RX buffer. This is a very important step*/
  Sleep(200);
  PurgeComm(hComPort, PURGE_RXCLEAR|PURGE_RXABORT);

  /* Copy the command to the command buffer */
  strcpy(cmdBuffer, cmd.c_str());

  if(!WriteFile(hComPort, cmdBuffer, cmd.length()+1, &numBytesWritten, NULL))
  {
    // CloseHandle(hComPort);
    return ""; // FAILURE
  }

  Sleep(200);

  /*Read back the response*/
  if(!ReadFile(hComPort, responseBuffer, 30, &numBytesRead, NULL))
  {
    // printf("Error: Unable to write to the specified port\n");
    // return 1;
    return "";
  }

  /* Add a null character at the end of the response so we can use the buffer
     with string manipulation functions.
  */
  responseBuffer[numBytesRead] = '\0';

  
  //printf("Info: Relay RAW return = '%s' %d\n", responseBuffer, numBytesRead);
  
  std::string result;
  result.resize(numBytesRead+1);
  for(unsigned int i=0;i<(numBytesRead+1);i++){
    result[i] = responseBuffer[i];
  }


  return result;
  
#else
  // not implemented (Linux)
  return ""; // FAILURE
#endif
  
}



//////////////////////////////////////////////////////////////////////
// INTERFACE

#include <iostream>
#include <sstream>


std::string Relay::getVersion()
{
  std::string result = runCommand("ver");

  if(result.length() == 0) return ""; 
  
  result = result.substr(5);
  
  std::istringstream s(result);
  
  std::getline(s, result);
  
  return result;
}


bool Relay::getRelay(unsigned int relay)
{
  char buffer[80];

  snprintf(buffer, 80, "relay read %d", relay);

  std::string result = runCommand(buffer);

  if(result.length() == 0) return false;

  result = result.substr(14);

  std::istringstream s(result);
  
  std::getline(s, result);

  if(result == "on") return true;
  else return false;
}


bool Relay::setRelay(unsigned int relay, bool value)
{
  char buffer[80];

  if(value)
    snprintf(buffer, 80, "relay on %d", relay);
  else
    snprintf(buffer, 80, "relay off %d", relay);

  std::string result = runCommand(buffer);

  if(result.length() == 0) return false;

  return true;
}


bool str2int(unsigned int *out, const char *ss, int base) {
  char s[128];

  strncpy(s, ss, 128*sizeof(char));
  
  char *end;
  if (s[0] == '\0' || isspace(s[0]))
    return false;
  errno = 0;
  long l = strtol(s, &end, base);
  /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
  if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
    return false;
  if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
    return false;
  if (*end != '\0')
    return false;
  *out = (unsigned int)l;
  
  return true;
}


bool Relay::getRelay(std::vector<bool>& relay)
{
  std::string result = runCommand("relay readall");
  
  if(result.length() > 0){

    result = result.substr(15);
    
    std::istringstream s(result);
    
    std::getline(s, result);

    // std::cout << "PARSED: " << result << std::endl;
    
    // 8 bit value
    unsigned int out = 0;
    
    if(str2int(&out, result.c_str(), 16)){
      
      // printf("relay readall = 0x%x\n", out);
      
      relay.resize(8);
      unsigned int mask = 0x01;

      for(unsigned int i=0;i<8;i++){
	relay[i] = out & mask;
	mask <<= 1; 
      }

      return true; 
    }
    else{
      relay.resize(0);
      return false;
    }
  }
  else{
    relay.resize(0);
    return false;
  }

  return false;
}



bool Relay::setRelay(const std::vector<bool>& relay)
{
  char buffer[80];

  unsigned int mask = 0x01;

  unsigned int value = 0;

  for(unsigned int i=0;i<8&&i<relay.size();i++){
    if(relay[i]) value += mask;
    mask <<= 1;
  }

  snprintf(buffer, 80, "relay writeall %x", (((unsigned char)value)&0xFF));

  std::string result = runCommand(buffer);

  if(result.length() == 0) return false;
  else return true;
}


void Relay::reset() // all relays to off
{
  std::string result = runCommand("reset");
}
