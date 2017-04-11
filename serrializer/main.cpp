//
//  main.cpp
//  serrializer
//
//  Created by B Wigley on 10/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#include "fb_serial.hpp"
#include "machine.h"
#include <fstream>

#include <sstream>

// include headers that implement a archive in simple text format



//see http://en.cppreference.com/w/cpp/types/integer
//struct L{
//  char c=[90];
//  float f;
//  std::string;
//  int8_t f;
//};

fs_sink::fs_sink(const char* buffer, int len)
{
  this->ofs = new  std::stringstream(std::ios::in | std::ios::out | std::ios::binary);
}

fs_sink::fs_sink(const char* file)
{
  this->ofs = new std::ofstream(file);
}


int main(int argc, const char * argv[]) {

   
  //create class instance - two different types, inherited
   
  //write to disk
    
  //unserialise both to type again
  
  //write it to network sink (via machine.h)
  
  //read it from network sink
  
  
    // create and open a character archive for output
  
    // create class instance
  const gps_position g(35, 59, 24.567f);
  
    // save data to archive
  {
    fs_sink oa("filename");
    oa << g; //write to the sink
    // archive and stream closed when destructors are called
  }
  
    // ... some time later restore the class instance to its orginal state
  gps_position newg;
  {
    // create and open an archive for input
//  std::ifstream ifs("filename");
//  boost::archive::text_iarchive ia(ifs);
//    // read class state from archive
//  ia >> newg;
//    // archive and stream closed when destructors are called
  }
  return 0;  
  
  
    return 0;
}
