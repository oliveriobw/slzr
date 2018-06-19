  //
  //  sink.hpp
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
#include "serial_write.h"
#include "serial_read.h"
#include "app_types.h"
#include "serial_types_map.h"

#define no_err (0) 
#define buffer_sz_err (-1) 

using namespace std;

/**
 * Sink class for buffers/files
 *
 * Manages packing and unpacking of data types. 
 */
struct sink
{  
  //memory buffer or file based packing/unpacking
  sink(const char* file, bool pack = false); //file store - pack or unpack  
                                             //  sink(data_t* d);                  //unpack from memory buffer
  sink(std::vector<uint8_t>& d);                  //unpack from memory buffer
  sink(const std::string& base_64); //unpack from b64 string 
  sink();                           //write to memory buffer
  
  ~sink();
  
    std::vector<uint8_t>& data(){
        return _data;
  }
  bool set_buf(char* buffer, int len);  
  
  // write/read
  // types with specific sizes
  // network order etc
  // T - serial_write / serial_read
  // Sink - ios::iostream stream type 
  template <typename SerializeType,class Sink> long serialize(fb_serial_v1** out,Sink s)
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
    
    size_t sz = r.serialize(header._protocol_version);
    sz += r.serialize(header._class_name,header._class_name_sz);
    sz += r.serialize(header._class_version);

    fb_serial_v1* p = NULL;
    if(r.unarchiver())
    {
      //create instance of unserializing out type
      p = serial_types::create(header);
      if(!p)
        return -1;
      
      *out = p;
    }
    else
      p=*out;
    
    //base class is responsible for the data-size field as well
    //as the custom fields
    sz += p->serialize(r);    
    return done();    
  }
  
  long pack(fb_serial_v1* out)
  {
    if(!_ofsx)
      return false;
    if(!out)
      return false;      
    return serialize<serial_write,ostream&>(&out,*_ofsx);
  }  

  /**
   *
   *  returns an object new-ed on the heap - the application
   *  needs to delete it
   *
   */
  fb_serial_v1* unpack()  
  {
    if(!_ifs)
      return NULL;
    
    fb_serial_v1* p = NULL;
    serialize<serial_read,istream&>(&p,*_ifs);

    return p;
  }      

    /**
     *
     *  returns a shared object
     *
     */
    template <class t>
    std::shared_ptr<t> unpack()  
    {
        if(!_ifs)
            return NULL;
    
        fb_serial_v1* b = NULL;
        serialize<serial_read,istream&>(&b,*_ifs);
        t* p = dynamic_cast<t*>(b);
        if(p){
            std::shared_ptr<t> a = std::make_shared<t>(*p);
            delete p;    //we can free the original here
            return a;
        }
    
        return NULL;
    }     
    
    
private:
  // private because we are not inheriting from this class. If that changes, then
  // the access specification can change at the same time, not before. 
  long done();
  ofstream* out_file();
  stringstream* out_buf();
  ofstream* in_file();
  stringstream* in_buf();
  
  //data members
  istream* _ifs;  
  ostream* _ofsx;
  std::vector<uint8_t> _data;  // Because vector is guranteed to be contiguous in C++03
  bool _pack;
};

#endif /* fs_sink_hpp */
