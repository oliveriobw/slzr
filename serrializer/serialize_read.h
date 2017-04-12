//
//  serialize_read.h
//  serrializer
//
//  Created by B Wigley on 12/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#ifndef serialize_read_h
#define serialize_read_h

#include "serialize.h"

struct serialize_read  : public serialize{
  
  typedef uint32_t size_type;
  
    //memory buffer or file  
  serialize_read(std::istream& ofs):_ofs(ofs){}
  
  std::istream& _ofs;
  
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
  
  template <typename T>  size_type serialize_(uint16_t& protocol)
  {
    protocol = 0;
    _ofs.read((char*)&protocol, sizeof protocol);  
    std::cout << protocol <<std::endl;
    protocol = ntohs(protocol);
    std::cout << protocol <<std::endl;
    return sizeof protocol;
  }
  
  template <typename T>  size_type serialize_(uint32_t& protocol)
  {
    _ofs.read((char*)&protocol, sizeof protocol);  
    std::cout << protocol <<std::endl;
    protocol = ntohl(protocol);
    std::cout << protocol <<std::endl;
    return sizeof protocol;
  }
  template <typename T>  size_type serialize_(uint64_t& protocol)
  {
    _ofs.read((char*)&protocol, sizeof protocol);  
    std::cout << protocol <<std::endl;
    protocol = ntohll(protocol);
    std::cout << protocol <<std::endl;
    return sizeof protocol;
  }
  
//  size_type serialize( char* protocol, const uint16_t len)
  size_type serialize( std::string& protocol, uint16_t len)
  {
    size_type p = _ofs.tellg();
    char buffer[len+1];
    memset(buffer,0,len+1);
    _ofs.read(&buffer[0],len);
        
    if (!_ofs)
    {
      std::cout << "error: only " << _ofs.gcount() << " could be read";
      return 0;
    }    
    
    protocol = std::string(buffer);
    size_type newp = _ofs.tellg();
    return len;  
  }
  
  template <typename T> size_type serialize( T& protocol)
  {
    return serialize_<T>(protocol);
  }
  static bool unarchiver(){ return true;}

};



#endif /* serialize_read_h */
