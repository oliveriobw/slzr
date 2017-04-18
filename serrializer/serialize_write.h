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


struct serialize_write : public sink{
  
  
    //memory buffer or file  
  serialize_write(std::ofstream& ofs):_ofs(ofs){}
  
  std::ofstream& _ofs;
  
  float float_swap(float value){
    union v {
      float       f;
      unsigned int    i;
    };
    
    union v val;
    unsigned int temp;
    
    val.f = value;
    temp = htonl(val.i);
    
    return *(float*)&temp;
  }


  template <typename T> size_type serialize_( T& value)
  {
    assert(0);//unexpected type
    if(sizeof value == sizeof(uint16_t))
      return serialize_<const uint16_t>((uint16_t&) value);
    if(sizeof value == sizeof(uint32_t))
      return serialize_<const uint32_t>((uint32_t&) value);
    if(sizeof value == sizeof(uint64_t))
      return serialize_< uint64_t>((uint64_t&) value);
    
    assert(("cast the type to uint32_t or uint64_t",0));
  }
  
  //template <typename T> size_type serialize_( const uint16_t& value)
  virtual size_type serialize_(  uint16_t& value)
  {
    std::cout << "write uint16_t = " << value <<std::endl;
    uint16_t tmp = htons(value);
    size_t pos = position();
    std::cout << "write uint16_t as = " << tmp << ", at=" << pos << std::endl;
    _ofs.write((const char*)&tmp, sizeof value);  
    return sizeof tmp;
  }
  
  //template <typename T>  
  virtual size_type serialize_(  uint32_t& value)
  {
    std::cout << value <<std::endl;
    uint32_t tmp = htonl(value);
    std::cout << tmp <<std::endl;
    _ofs.write((const char*)&tmp, sizeof value);  
    return sizeof tmp;
  }
  
  
  //template <typename T> 
  virtual  size_type serialize_(  uint64_t& value)
  {
    std::cout << value <<std::endl;
    uint64_t tmp = htonll(value);
    std::cout << tmp <<std::endl;
    _ofs.write((const char*)&tmp, sizeof value);  
    return sizeof tmp;
  }

  virtual  size_type serialize_(  float& value)
  {
    std::cout << value <<std::endl;    
    float tmp = float_swap(value);    
    std::cout << tmp <<std::endl;
    _ofs.write((const char*)&tmp, sizeof value);  
    return sizeof tmp;
  }
  
  /**
    serializes size folowed by string payload. 
  */
  size_type serialize_( std::string& value, uint16_t& len)
  {
    assert(value.length()==len);
    serialize_(len);
            
    std::ofstream::pos_type p = _ofs.tellp();
    _ofs.write(value.c_str(),len);
    std::ofstream::pos_type newp = _ofs.tellp();
    
    assert((newp-p) == len);
    return ((size_type)(newp-p)) + (size_type)sizeof len;  
  }

  virtual size_t position() 
  {
      size_t pos= _ofs.tellp();
      return pos;
  }
  
  virtual bool seek(const size_t pos)
  {
    _ofs.seekp(pos);
    return true;
  }

  // cache the position for the data size field
  virtual void serialize_data_size_init()
  {
    data_sz_pos = position();    
    size_type sz = 0;//write 0 to increment pointer...
    serialize_(sz);    
  }
  
  // write the data size field
  virtual void serialize_data_size(const size_type size)
  {
    if(data_sz_pos==INVALID_DATA_SZ_POS)
      return;
      
    seek(data_sz_pos);
    
    size_type sz = size;
    serialize_(sz);    
  }

  // no-op when writing
  virtual bool verify_data_size(const size_type size)
  {
    return true;
  }
  
  
  static bool unarchiver(){ return false; }
  
};


#endif /* serialize_write_h */
