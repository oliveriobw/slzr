//
//  serialize_write.h
//  serrializer
//
//  Created by B Wigley on 12/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#ifndef serialize_write_h
#define serialize_write_h

#include "serialize.h"

struct serialize_write : public serialize{
  
  typedef uint32_t size_type;
  
    //memory buffer or file  
  serialize_write(std::ofstream& ofs):_ofs(ofs){}
  
  std::ofstream& _ofs;
  
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
  
  template <typename T> size_type serialize_( const uint16_t& protocol)
  {
    std::cout << protocol <<std::endl;
    T tmp = htons(protocol);
    std::cout << tmp <<std::endl;
    _ofs.write((const char*)&tmp, sizeof protocol);  
    return sizeof tmp;
  }
  
  template <typename T>  size_type serialize_(const  uint32_t& protocol)
  {
    std::cout << protocol <<std::endl;
    T tmp = htonl(protocol);
    std::cout << tmp <<std::endl;
    _ofs.write((const char*)&tmp, sizeof protocol);  
    return sizeof tmp;
  }
  
  
  template <typename T>  size_type serialize_(const  uint64_t& protocol)
  {
    std::cout << protocol <<std::endl;
    T tmp = htonll(protocol);
    std::cout << tmp <<std::endl;
    _ofs.write((const char*)&tmp, sizeof protocol);  
    return sizeof tmp;
  }
  
  size_type serialize( std::string& protocol, uint16_t len)
  {
    size_type p = _ofs.tellp();
    //const uint16_t len = protocol.length();
    _ofs.write(protocol.c_str(),len);
    size_type newp = _ofs.tellp();
    return newp-p;  
  }
  
  template <typename T> size_type serialize(const T& protocol)
  {
    return serialize_<T>(protocol);
  }
  
  static bool unarchiver(){ return false;}
};


#endif /* serialize_write_h */
