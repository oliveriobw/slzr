//
//  fs_sink.hpp
//  serrializer
//
//  Created by B Wigley on 12/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#ifndef fs_sink_hpp
#define fs_sink_hpp

#include <assert.h>
#include <iostream>
#include <fstream>
#include "serialize_write.h"
#include "serialize_read.h"
#include "app_types.h"
#include "serial_types_map.h"

struct fs_sink{
  
    //memory buffer
  fs_sink(const char* buffer, int len, bool write);
  
    //file
  fs_sink(const char* file, bool write = false);
  ~fs_sink();
  
    //std::ostream* ofs;    
  std::ifstream* ifs;  
  std::ofstream* ofsx;    
  
    // write/read
    // types with specific sizes
    // network order etc
    // T - serialize_write / serialize_read
    // Sink - ios::iostream stream type 
  template <typename T,class Sink> void serialize_(fb_serial_v1** out,Sink s)
  {
    T* r = new T(s);
  
  /*
   1             2               3               4
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |       Protocol Version      |        Class Name Size          |       
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                      Class Name UTF8                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |      Class  Version         |          Data Size              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |       Data Size             |               data              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                             data                              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    
   */
    fb_serial_header header;
    if(*out)
      header = (*out)->_hdr;
    
  r->serialize_(header._protocol_version);
  //r->serialize_(header._class_name_sz);
  r->serialize_(header._class_name,header._class_name_sz);

  fb_serial_v1* p = NULL;
  if(T::unarchiver())
  {
    //create instance of unserializing out type
    p = serial_types::create(header._class_name);
    if(!p)
      return;
      
    *out = p;
  }
  else
    p=*out;
    
    //base class is responsible for the data-size field as well
    //as the custom fields
    p->serialize(*r);
    
    return;
  }
  
  void pack(fb_serial_v1* out)
  {
     return serialize_<serialize_write,std::ofstream&>(&out,*ofsx);
  }  
  
  void unpack (fb_serial_v1** out)  
  {
    return serialize_<serialize_read,std::ifstream&>(out,*ifs);
  }      
};





#endif /* fs_sink_hpp */
