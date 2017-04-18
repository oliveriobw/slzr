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

#define INVALID_DATA_SZ_POS (0)

/**
  The reader/writer classes need to implement these methods for
  supporting intrinsic types.  
 */
struct sink
{
  typedef uint32_t size_type;

  sink():data_sz_pos(INVALID_DATA_SZ_POS){}

  virtual size_type serialize_(  uint16_t& value)=0;
  virtual size_type serialize_(  uint32_t& value)=0;
  virtual size_type serialize_(  uint64_t& value)=0;
  virtual size_type serialize_(  float& value)=0;
  virtual size_type serialize_( std::string& value, uint16_t& len)=0;

  //for stream navigation
  virtual size_t position()=0;
  virtual bool seek(const size_t pos)=0;

  virtual void serialize_data_size_init()=0;
  virtual void serialize_data_size(const size_type size)=0;
  virtual bool verify_data_size(const size_type size)=0;

protected:
  size_t data_sz_pos;

};

#endif /* serialize_h */
