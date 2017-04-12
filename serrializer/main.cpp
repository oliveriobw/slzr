//
//  main.cpp
//  serrializer
//
//  Created by B Wigley on 10/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#include "machine.h"
#include "app_types.h"
#include "fs_sink.hpp"

#include <fstream>
#include <sstream>

using namespace std;



int main(int argc, const char * argv[]) {
const char* file = "/tmp/dohp.bin";
   
  //create class instance - two different types, inherited
   
  //write to disk
    
  //unserialise both to type again
  
  //write it to network sink (via machine.h)
  
  //read it from network sink
  
  
    // create and open a character archive for output
  
    // create class instance
   gps_position g(35, 59, 24.567f);
  
    // save data to archive
  {
    fs_sink oa(file,true);
    oa << g; //write to the sink
    // archive and stream closed when destructors are called
  }
  
    // ... some time later restore the class instance to its orginal state
  gps_position newg;
  {
     gps_position gp(0,0,0);
     fs_sink oa(file,false);
     oa >> gp; //write to the sink
  }
  return 0;  
  
  
    return 0;
}
