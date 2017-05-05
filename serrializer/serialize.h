//
//  serialize.h
//  serrializer
//
//  Created by B Wigley on 12/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#ifndef serialize_h
#define serialize_h

#include <string>
#include <vector>

#define INVALID_DATA_SZ_POS (0)

/**
  The reader/writer classes need to implement these methods for
  supporting intrinsic types.  
 */
struct serial
{
  typedef uint32_t size_type;

  serial():data_sz_pos(INVALID_DATA_SZ_POS){}

  virtual size_type serialize( uint8_t& value)=0;
  virtual size_type serialize( uint16_t& value)=0;
  virtual size_type serialize( uint32_t& value)=0;
  virtual size_type serialize( uint64_t& value)=0;
  virtual size_type serialize( float& value)=0;
  virtual size_type serialize( uint8_t* value, uint32_t& sz)=0;
  virtual size_type serialize( std::string& value, uint16_t& len)=0;
  virtual size_type serialize(std::vector<uint8_t>& data)=0;
  virtual size_type serialize(std::vector<int8_t>& data)=0;
  virtual bool  unarchiver()=0;

  virtual void serialize_data_size_init()=0;
  virtual uint32_t serialize_data_size(const size_type size)=0;
  virtual bool verify_data_size(const size_type size)=0;

  protected:
  size_t data_sz_pos;
};

#endif /* serialize_h */
