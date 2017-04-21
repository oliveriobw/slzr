//
//  serial_read.h
//  serrializer
//
//  Created by B Wigley on 12/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#ifndef serialize_read_h
#define serialize_read_h

#include "serialize.h"

struct serial_read  : public serial{
    
  //memory buffer or file  
  serial_read(std::istream& ifs):_ifs(ifs){}
  
  std::istream& _ifs;

  float float_swap(float value){
    union v {
      float        f;
      unsigned int i;
    };
    
    union v val;
    unsigned int temp;
    
    val.f = value;
    temp = ntohl(val.i);
    
    return *(float*)&temp;
  }
  
  template <typename T> size_type serialize( T& value)
  {
    assert(0);
    return 0;
  }

#ifdef DEBUG
  virtual size_type serialize(uint8_t& value)
  {
    _ifs.read((char*)&value, sizeof value);  
    std::cout << "read uint8_t = " << value ;
    std::cout << " => " << value << std::endl;
    return sizeof value;
  }

  virtual size_type serialize(uint16_t& value)
  {
    _ifs.read((char*)&value, sizeof value);  
    std::cout << "read uint16_t = " << value ;
    value = ntohs(value);
    std::cout << " => " << value << std::endl;
    return sizeof value;
  }

  virtual size_type serialize(uint32_t& value)
  {
    _ifs.read((char*)&value, sizeof value);  
    std::cout << "read uint32_t = " << value ;
    value = ntohl(value);
    std::cout << " => " << value << std::endl;
    return sizeof value;
  }

  virtual   size_type serialize(uint64_t& value)
  {
    _ifs.read((char*)&value, sizeof value);  
    std::cout << "read uint64_t = " << value ;
    value = ntohll(value);
    std::cout << " => " << value << std::endl;
    return sizeof value;
  }

  virtual   size_type serialize(float& value)
  {
    _ifs.read((char*)&value, sizeof value);  
    std::cout << "read float = " << value ;
    value = float_swap(value);
    std::cout << " => " << value << std::endl;
    return sizeof value;
  }
#else
  virtual size_type serialize(uint8_t& value)
  {
    _ifs.read((char*)&value, sizeof value);  
    return sizeof value;
  }

  virtual size_type serialize(uint16_t& value)
  {
    _ifs.read((char*)&value, sizeof value);  
    value = ntohs(value);
    return sizeof value;
  }
  
  virtual size_type serialize(uint32_t& value)
  {
    _ifs.read((char*)&value, sizeof value);  
    value = ntohl(value);
    return sizeof value;
  }
  
  virtual   size_type serialize(uint64_t& value)
  {
    _ifs.read((char*)&value, sizeof value);  
    value = ntohll(value);
    return sizeof value;
  }
  
  virtual   size_type serialize(float& value)
  {
    _ifs.read((char*)&value, sizeof value);  
    value = float_swap(value);
    return sizeof value;
  }
#endif

  size_type serialize( uint8_t* buffer, uint32_t& len)
  {
    serialize(len);
    
    //char buffer[len+1];
    //memset(buffer,0,len+1);
    _ifs.read((char*)buffer,len);
    
    if (!_ifs)
      {
      std::cout << "error: only \"" << _ifs.gcount() << " bytes \" could be read";
      return 0;
      }    
    
//    value = std::string(buffer);
//    std::cout << "read " << len << " bytes="  << value << std::endl;//nb: as string
    
    return len;  
  }

  //reads string size then string payload
  //treats incoming payload like a string - so zero terminated after unpacking  
  virtual size_type serialize( std::string& value, uint16_t& len)
  {
    serialize(len);
  
    char buffer[len+1];
    memset(buffer,0,len+1);
    _ifs.read(&buffer[0],len);
        
    if (!_ifs)
    {
      std::cout << "error: only \"" << _ifs.gcount() << " bytes \" could be read";
      return 0;
    }    
    
    value = std::string(buffer);
  std::cout << "read " << len << " bytes="  << value << std::endl;//nb: as string
    
    return len;  
  }

  virtual size_t position() 
  {
    size_t pos= _ifs.tellg();
    return pos;
  }
  
  virtual bool seek(const size_t pos)
  {
    return true;
  }

  // get the data size field from the current position
  virtual void serialize_data_size_init()
  {
    serialize(data_size); 
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
