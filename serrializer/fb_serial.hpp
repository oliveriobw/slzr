//
//  fb_serial.hpp
//  serrializer
//
//  Created by B Wigley on 10/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#ifndef fb_serial_hpp
#define fb_serial_hpp

#include <stdio.h>
#include <istream>
#include <iostream>
#include <assert.h>


class gps_position;

struct serialize_write {

  typedef uint32_t size_type;
  
  //memory buffer or file  
  serialize_write(std::ostream* ofs):_ofs(ofs){}
    
  std::ostream* _ofs;
  
  template <typename T> size_type serialize_( T& protocol)
  {
  if(sizeof protocol == sizeof(uint16_t))
    return serialize_<const uint16_t>((const uint16_t&) protocol);
  if(sizeof protocol == sizeof(uint32_t))
    return serialize_<const uint32_t>((const uint32_t&) protocol);
  if(sizeof protocol == sizeof(uint64_t))
    return serialize_<const uint64_t>((const uint64_t&) protocol);
    
    assert(("cast the type to uint32_t or uint64_t",0));
 }
  
  template <typename T> 
  size_type serialize_( const uint16_t& protocol)
  {
       std::cout << protocol <<std::endl;
       T tmp = htons(protocol);
       std::cout << tmp <<std::endl;
       _ofs->write((const char*)&tmp, sizeof protocol);  
       return sizeof tmp;
  }
  
  template <typename T> 
  size_type serialize_(const  uint32_t& protocol)
  {
  std::cout << protocol <<std::endl;
  T tmp = htonl(protocol);
  std::cout << tmp <<std::endl;
  _ofs->write((const char*)&tmp, sizeof protocol);  
  return sizeof tmp;
  }


  template <typename T> 
  size_type serialize_(const  uint64_t& protocol)
  {
  std::cout << protocol <<std::endl;
  T tmp = htonll(protocol);
  std::cout << tmp <<std::endl;
  _ofs->write((const char*)&tmp, sizeof protocol);  
  return sizeof tmp;
  }
  
  size_type serialize( const char* protocol, const uint16_t len)
  {
      long p = _ofs->tellp();
      _ofs->write(protocol,len);
      long newp = _ofs->tellp();
      return newp-p;  
  }
  
  template <typename T> size_type serialize(const T& protocol)
  {
     return serialize_<T>(protocol);
  }
};


struct fs_sink{

  //memory buffer
  fs_sink(const char* buffer, int len);
  
  //file
  fs_sink(const char* file);
  
  std::ostream* ofs;  
  
  // write/read
  // types with specific sizes
  // network order etc
  template <class T,class U> std::ostream& serialize_(U& out)
  {
    T* r = new T(ofs);

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
  std::cout << "pos=" <<  ofs->tellp() << std::endl;
    r->serialize(protocol);

    std::string name = out.name();
    uint16_t sz = name.length();
    r->serialize(sz);
  std::cout << "pos=" <<  ofs->tellp() << std::endl;
    r->serialize(name.c_str(),sz);
  std::cout << "pos=" <<  ofs->tellp() << std::endl;

    long p =  ofs->tellp();
    typename T::size_type datasz =0;
    assert(sizeof datasz == 4);
    ofs->seekp(p + sizeof datasz);
    
    datasz = out.serialize(r);
    
    std::cout << "pos=" <<  ofs->tellp() << std::endl;
    ofs->seekp(p);    
    std::cout << "pos=" <<  ofs->tellp() << std::endl;
    datasz = r->serialize(datasz);
    assert(datasz == 4);
    
    std::cout << "pos=" <<  ofs->tellp() << std::endl;
    ofs->seekp(0,std::ios_base::end);
    std::cout << "pos=" <<  ofs->tellp() << std::endl;

    //assuming this is the end etc
    //tbd: close
    
    return *ofs;
  }
  
  template <class T> std::ostream& operator<< (T& out)
  {
     return serialize_<serialize_write>(out);
  }  


};

//struct net_sink{
//  net_sink(const char* buffer, int len);
//  std::ofstream* ofs;
//  
//    //
//  template <class T>
//  std::ofstream& operator<< (T& out){
//      // for all classes:
//      //   name/type
//      //   version
//    
//    return *ofs;
//  }
//};


struct fb_archiver
{
  
};

class gps_position
{
private:
  int degrees;
  int minutes;
  float seconds;
public:
  gps_position(){}
  gps_position(int d, int m, float s) :  degrees(d), minutes(m), seconds(s) {}

  std::string name() const{return "gpsthing";}  

  template <class T> uint32_t serialize(T& serializer) const
  {
  uint32_t written =0;
  written += serializer->serialize_(degrees);
     written += serializer->serialize_(minutes);
     written += serializer->serialize_(seconds);
    return written;
  }  
  
};



#endif /* fb_serial_hpp */
