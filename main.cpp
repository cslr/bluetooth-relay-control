
#include "Relay.h"

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <windows.h> 


int main(int argc, char** argv)
{
  srand(time(0));

  if(argc != 2){
    printf("Usage: %s <com_port>\n", argv[0]);
    return -1;
  }

  Relay* r = NULL;

  try{
    printf("Opening Bluetooth relay on port: %s.\n", argv[1]);
    
    r = new Relay(argv[1]);
  }
  catch(std::exception& e){
    std::cout << "Exception occured (FAIL): " << e.what() << std::endl;
    return -1;
  }
    
  printf("Relay version: %s\n", r->getVersion().c_str());

  
#if 0
  r->setRelay(0, true);
  
  printf("Relay 0: %d\n", r->getRelay(0));

  return 0;

#endif

  

#if 1
  std::vector<bool> relay;

  while(1)
  {

    if(r->getRelay(relay)){
      printf("RELAY:\n");
      for(unsigned int i=0;i<relay.size();i++){
	if(relay[i]) printf("1\n");
	else printf("0\n");
      }
    }
    else{
      printf("Get relay FAILED.\n");
    }
    
    relay.resize(8);
    
    
    for(unsigned int i=0;i<relay.size();i++)
      relay[i] = (bool)(rand() & 1);
    
    if(r->setRelay(relay)){
      printf("Relay successfully set.\n");
    }
    else{
      printf("Setting relay FAILED.\n");
    }

    fflush(stdout);
    Sleep(500);
  }
#endif
  
  if(r)
    delete r;

  return 0;
}
