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

/**
 * Sink class for buffers/files
 *
 * Manages packing and unpacking of data types. 
 */
struct fs_sink
{  
    //memory buffer or file based packing/unpacking
  fs_sink(char* buffer, int len, bool pack = false);
  fs_sink(const char* file, bool pack = false);
  
  ~fs_sink();
  void done();
  std::ofstream* out_file();
  std::stringstream* out_buf();
  std::ofstream* in_file();
  std::stringstream* in_buf();
  bool set_buf(char* buffer, int len);
  
  std::istream* ifs;  
  char* _buf;
  size_t _len;
  bool _pack;
  std::ostream* ofsx;    
  
  // write/read
  // types with specific sizes
  // network order etc
  // T - serialize_write / serialize_read
  // Sink - ios::iostream stream type 
  template <typename T,class Sink> bool serialize_(fb_serial_v1** out,Sink s)
  {
    T r(s);
    
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
    
    r.serialize_(header._protocol_version);
    r.serialize_(header._class_name,header._class_name_sz);
    
    fb_serial_v1* p = NULL;
    if(T::unarchiver())
    {
        //create instance of unserializing out type
      p = serial_types::create(header._class_name);
      if(!p)
        return false;
      
      *out = p;
    }
    else
      p=*out;
    
      //base class is responsible for the data-size field as well
      //as the custom fields
    p->serialize(r);
    
    done();
    
    return true;
  }
  
  bool pack(fb_serial_v1* out)
  {
    if(!ofsx)
      return false;
    return serialize_<serialize_write,std::ostream&>(&out,*ofsx);
  }  
  
  bool unpack (fb_serial_v1** out)  
  {
    if(!ifs)
      return false;
    
    return serialize_<serialize_read,std::istream&>(out,*ifs);
  }      
};

#endif /* fs_sink_hpp */
