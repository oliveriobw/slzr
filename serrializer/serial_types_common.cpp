//
//    serial_types.hpp serial_types_common.cpp
//  serrializer
//
//  Created by B Wigley on 28/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#include "serial_types_common.hpp"
#include "sink.hpp"
#include "base64.h"

std::string&  fb_serial_v1::tob64(){
  struct sink mem_sink;
  long resut = mem_sink.pack(this);

  b64="";
  if(resut==buffer_sz_err)
    return b64;
  data_t* d = mem_sink.data();
  b64 = string(base64_encode((unsigned char*)d->_data,(uint32_t)d->_len));    
  return b64;
}
