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

struct serialize_write : public sink
{    
  //memory buffer or file  
  serialize_write(std::ostream& ofs):_ofs(ofs){}
  
  std::ostream& _ofs;
  
  float float_swap(float value)
  {
    union v 
    {
      float           f;
      unsigned int    i;
    };
    
    union v val;
    unsigned int temp;
    
    val.f = value;
    temp = htonl(val.i);
    
    return *(float*)&temp;
  }

  template <typename T> size_type serialize( T& value)
  {
    //unexpected type - use supported types
    assert(0);
  }

#ifdef DEBUG
  
  virtual size_type serialize(uint8_t& value)
  {
    size_t pos = position();
    std::cout << "write uint8_t = \"" << value ;
    std::cout << "\" at =" << pos << std::endl;
    _ofs.write((const char*)&value, sizeof value);  
    return sizeof value;
  }

  virtual size_type serialize(  uint16_t& value)
  {
    std::cout << "write uint16_t = " << value ;
    uint16_t tmp = htons(value);
    size_t pos = position();
    std::cout << " => " << tmp << ", at =" << pos << std::endl;
    _ofs.write((const char*)&tmp, sizeof value);  
    return sizeof tmp;
  }

  virtual size_type serialize(  uint32_t& value)
  {
    std::cout << "write uint32_t = " << value ;
    uint32_t tmp = htonl(value);
    size_t pos = position();
    std::cout << " => " << tmp << ", at =" << pos << std::endl;
    _ofs.write((const char*)&tmp, sizeof value);  
    return sizeof tmp;
  }
  
    virtual  size_type serialize(  uint64_t& value)
  {
    std::cout << "write uint64_t = " << value ;
    uint64_t tmp = htonll(value);
    size_t pos = position();
    std::cout << " => " << tmp << ", at =" << pos << std::endl;
    _ofs.write((const char*)&tmp, sizeof value);  
    return sizeof tmp;
  }

  virtual  size_type serialize(  float& value)
  {
    std::cout << "write float = " << value ;
    float tmp = float_swap(value);    
    size_t pos = position();
    std::cout << " => " << tmp << ", at =" << pos << std::endl;

    _ofs.write((const char*)&tmp, sizeof value);  
    return sizeof tmp;
  }
  
  /**
    serializes size folowed by string payload. 
  */
  size_type serialize( std::string& value, uint16_t& len)
  {
    //string::length treats contents as a string 
    //assert(value.length()==len);
  
    serialize(len);
            
    std::ostream::pos_type p = _ofs.tellp();
    _ofs.write(value.c_str(),len);
    std::ostream::pos_type newp = _ofs.tellp();
  std::cout << "wrote=\"" << value << "\"" << std::endl; //warn: treats it as string
    assert((newp-p) == len);
    return ((size_type)(newp-p)) + (size_type)sizeof len;  
  }

#else

  virtual size_type serialize(uint8_t& value)
  {
    size_t pos = position();
    _ofs.write((const char*)&value, sizeof value);  
    return sizeof value;
  }

  virtual size_type serialize(  uint16_t& value)
  {
    uint16_t tmp = htons(value);
    _ofs.write((const char*)&tmp, sizeof value);  
    return sizeof tmp;
  }
  
  virtual size_type serialize(  uint32_t& value)
  {
    uint32_t tmp = htonl(value);
    _ofs.write((const char*)&tmp, sizeof value);  
    return sizeof tmp;
  }
  
  virtual  size_type serialize(  uint64_t& value)
  {
    uint64_t tmp = htonll(value);
    _ofs.write((const char*)&tmp, sizeof value);  
    return sizeof tmp;
  }
  
  virtual  size_type serialize(  float& value)
  {
    float tmp = float_swap(value);    
    _ofs.write((const char*)&tmp, sizeof value);  
    return sizeof tmp;
  }
  
  /**
   serializes size folowed by string payload. 
   */
  size_type serialize( std::string& value, uint16_t& len)
  {
    serialize(len);  
    std::ostream::pos_type p = _ofs.tellp();
    _ofs.write(value.c_str(),len);
    std::ostream::pos_type newp = _ofs.tellp();
    return ((size_type)(newp-p)) + (size_type)sizeof len;  
  }  
#endif

  size_type serialize( uint8_t* data, uint32_t& len)
  {
//    std::string tmp;
//    tmp.resize(len);
//    tmp.replace(0, len, (char*)data,len);
    
    //like strings, buffers get the len prepended too
    serialize(len);
    
    std::ostream::pos_type p = _ofs.tellp();
    _ofs.write((char*)data,len);
    std::ostream::pos_type newp = _ofs.tellp();
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
    size_type sz = 0;
  
    //write 0 to increment pointer by desired amount
    serialize(sz);    
  }
  
  // write the data size field
  virtual void serialize_data_size(const size_type size)
  {
    if(data_sz_pos==INVALID_DATA_SZ_POS)
      return;
      
    seek(data_sz_pos);
    
    size_type sz = size;
    serialize(sz);    
  }

  // no-op when writing
  virtual bool verify_data_size(const size_type size)
  {
    return true;
  }
    
  static bool unarchiver(){ return false; }  
};


#endif /* serialize_write_h */
