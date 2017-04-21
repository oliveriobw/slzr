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

using namespace std;

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
  bool set_buf(char* buffer, int len);  
  
  // write/read
  // types with specific sizes
  // network order etc
  // T - serial_write / serial_read
  // Sink - ios::iostream stream type 
  template <typename SerializeType,class Sink> bool serialize(fb_serial_v1** out,Sink s)
  {
    SerializeType r(s);
    
    /*
     1             2               3               4
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |       Protocol Version      |        Class Name Size          |       
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |                      Class Name UTF8                          |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |      Class  Version         |          Data Size ->           |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |    <-- Data Size            |               data ->           |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |                          <-- data -->                         |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    
     */
    fb_serial_header header;
    if(*out)
      header = (*out)->_hdr;
    
    r.serialize(header._protocol_version);
    r.serialize(header._class_name,header._class_name_sz);
    
    fb_serial_v1* p = NULL;
    if(SerializeType::unarchiver())
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
    if(!_ofsx)
      return false;
    if(!out)
      return false;      
    return serialize<serial_write,ostream&>(&out,*_ofsx);
  }  

  //returns an object on the heap, for the caller to delete
  bool unpack (fb_serial_v1** out)  
  {
    if(!_ifs)
      return false;
  
    if(!out)
      return false;
  
    return serialize<serial_read,istream&>(out,*_ifs);
  }      
  
private:
  // private because we are not inheriting from this class. If that changes, then
  // the access specification can change at the same time, not before. 
  void done();
  ofstream* out_file();
  stringstream* out_buf();
  ofstream* in_file();
  stringstream* in_buf();
  
  //data members
  istream* _ifs;  
  ostream* _ofsx;      
  char* _buf;
  size_t _len;
  bool _pack;
};

#endif /* fs_sink_hpp */
