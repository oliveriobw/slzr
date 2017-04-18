//
//  main.cpp
//  serializer
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

int main(int argc, const char * argv[]) 
{
    //tbd network buffer  
    const char* file = "/tmp/dohp11.bin";  
    // save data to archive
    {
       gps_position g(35, 59, 24.567f);
       fs_sink oa(file,true);
       oa.pack(&g); 
    }

    {
       //recover
       fb_serial_v1* fb = NULL;
       fs_sink oa(file,false);
       oa.unpack(&fb);        
       gps_position* b = dynamic_cast<gps_position*>(fb);
       if(b)
         cout << "the float is:" << b->seconds << endl;
    }
   
  file = "/tmp/im.bin";  
  {
    text_im g("12345");
    fs_sink oa(file,true);
    oa.pack(&g); 
  }
  
  {
    fb_serial_v1* fb = NULL;
    fs_sink oa(file,false);
    oa.unpack(&fb);        
    text_im* b = dynamic_cast<text_im*>(fb);
    if(b)
      cout << "the content is:" << b->_message << endl;
  }
   
   
   
   return 0;
}
