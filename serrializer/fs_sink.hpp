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

struct fs_sink{
  
    //memory buffer
  fs_sink(const char* buffer, int len);
  
    //file
  fs_sink(const char* file, bool append = false);
  ~fs_sink();
  
    //std::ostream* ofs;    
  std::ifstream* ifs;  
  std::ofstream* ofsx;    
  
    // write/read
    // types with specific sizes
    // network order etc
    // T - serialize_write / serialize_read
    // U - target out buffer class - needs to be a base class or void*
    // Sink - stream type
  template <typename T,class U, class Sink> void serialize_(U& out,Sink s)
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
  
  uint16_t protocol = 12;
    //std::cout << "pos=" <<  s.tellp() << std::endl;
  r->serialize(protocol);
    
    //tbd instanciate class of correct type here
  std::string name = out.name();
  uint16_t sz = name.length();
  r->serialize(sz);
    //std::cout << "pos=" <<  s->tellp() << std::endl;
  r->serialize(name,sz);
    //std::cout << "pos=" <<  s->tellp() << std::endl;

  fb_serial* p = NULL;
  if(T::unarchiver())
  {
    //create instance of out type
    p = serial_types::create(name);
    if(!p)
      return;    
  }
  else
    p=&out;
  
    //tbd
    //long p =  s->tellp();
  typename T::size_type datasz =0;
    //assert(sizeof datasz == 4);
    //s->seekp(p + sizeof datasz);    
  //datasz = p->serialize(r);
  
    //std::cout << "pos=" <<  s->tellp() << std::endl;
    //s->seekp(p);    
    //std::cout << "pos=" <<  s->tellp() << std::endl;
  datasz = r->serialize(datasz);
    //assert(datasz == 4);
  
    //std::cout << "pos=" <<  s->tellp() << std::endl;
    //s->seekp(0,std::ios_base::end);
    //std::cout << "pos=" <<  s->tellp() << std::endl;
  
    //assuming this is the end etc
    //tbd: close    
  return;
  }
  
  template <class T> void operator<< (T& out)
  {
     return serialize_<serialize_write,T,std::ofstream&>(out,*ofsx);
  }  
  
  template <class T> void operator>> (T& out)
  {
     return serialize_<serialize_read,T,std::ifstream&>(out,*ifs);
  }  
  
  
};





#endif /* fs_sink_hpp */
