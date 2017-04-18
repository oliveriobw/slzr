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

struct serialize_read  : public sink{
  
  
    //memory buffer or file  
  serialize_read(std::istream& ofs):_ofs(ofs){}
  
  std::istream& _ofs;

  float float_swap(float value){
    union v {
      float       f;
      unsigned int    i;
    };
    
    union v val;
    unsigned int temp;
    
    val.f = value;
    temp = ntohl(val.i);
    
    return *(float*)&temp;
  }

  
  template <typename T> size_type serialize_( T& value)
  {
    if(sizeof value == sizeof(uint16_t))
      return serialize_<const uint16_t>((const uint16_t&) value);
    if(sizeof value == sizeof(uint32_t))
      return serialize_<const uint32_t>((const uint32_t&) value);
    if(sizeof value == sizeof(uint64_t))
      return serialize_<const uint64_t>((const uint64_t&) value);
    
    assert(("cast the type to uint32_t or uint64_t",0));
  }
  
  virtual size_type serialize_(uint16_t& value)
  {
    value = 0;
    size_t pos = position();    
    _ofs.read((char*)&value, sizeof value);  
    std::cout << "read uint16_t = " << value <<std::endl;
    value = ntohs(value);
    std::cout << "read uint16_t as = " << value  << ", at=" << pos << std::endl;
    return sizeof value;
  }
  
  virtual size_type serialize_(uint32_t& value)
  {
    _ofs.read((char*)&value, sizeof value);  
    std::cout << value <<std::endl;
    value = ntohl(value);
    std::cout << value <<std::endl;
    return sizeof value;
  }

  virtual   size_type serialize_(uint64_t& value)
  {
    _ofs.read((char*)&value, sizeof value);  
    std::cout << value <<std::endl;
    value = ntohll(value);
    std::cout << value <<std::endl;
    return sizeof value;
  }

    //template <typename T>
  virtual   size_type serialize_(float& value)
  {
  //float tbd
    _ofs.read((char*)&value, sizeof value);  
    std::cout << value <<std::endl;
    value = float_swap(value);
    std::cout << value <<std::endl;
    return sizeof value;
  }

  //reads string size then string payload
  //treats incoming payload like a string - so zero terminated after unpacking  
  virtual size_type serialize_( std::string& value, uint16_t& len)
  {
    serialize_(len);
  
    char buffer[len+1];
    memset(buffer,0,len+1);
    _ofs.read(&buffer[0],len);
        
    if (!_ofs)
    {
      std::cout << "error: only \"" << _ofs.gcount() << " bytes \" could be read";
      return 0;
    }    
    
    value = std::string(buffer);
    return len;  
  }

  virtual size_t position() 
  {
    size_t pos= _ofs.tellg();
    return pos;
  }
  
  virtual bool seek(const size_t pos)
  {
    return true;
  }

  // get the data size field from the current position
  virtual void serialize_data_size_init()
  {
    serialize_(data_size); 
  }

  // no-op when reading  
  virtual void serialize_data_size(const size_type size)
  {
  }
  
  //ensire the reported size in the payload matches that required by the
  //class
  virtual bool verify_data_size(const size_type size)
  {
    return size == data_size;
  }

  static bool unarchiver(){ return true;}

  protected:
  size_type data_size;
};



#endif /* serialize_read_h */
