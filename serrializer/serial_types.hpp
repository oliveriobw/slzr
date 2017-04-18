//
//  serial_types.hpp
//  serrializer
//
//  Created by B Wigley on 12/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#ifndef serial_types_hpp
#define serial_types_hpp

#include <string>
#include <map>
#include "serialize.h"
//#include "app_types.h"

//in case we ever rewrite this protocol
#define FB_SERIAL_PROT_VER (0x01) 

class serialize_read;

/*
   Common header for all serialisable types
 */
struct fb_serial_header{
  uint16_t _protocol_version;
  uint16_t _class_name_sz;
  std::string _class_name;
  uint16_t _class_version;  //the nested class version
  //uint16_t _class_data_sz;  //class payload  
  
  fb_serial_header(){}
  
  fb_serial_header(uint16_t pro_ver,uint16_t ver, std::string class_id):
    _protocol_version(pro_ver),
    _class_version(ver),
    _class_name(class_id),
    _class_name_sz(class_id.length()) {}
};

/*
  Base class for all types needing serialize support
*/
struct fb_serial_v1{
  //common header for types
  fb_serial_header _hdr;
  
  //all classes need to specify their versions and unique names
  fb_serial_v1(uint16_t ver, std::string class_id): _hdr(FB_SERIAL_PROT_VER,ver,class_id) {}


  /**
   serialises custom payloads including the size field
  */
  virtual uint32_t serialize(sink& sink)
  {
    sink.serialize_data_size_init();
    uint32_t size = serialize_payload(sink);
    sink.serialize_data_size(size);
    return size;
    
  }

  protected:
  
  // classes to implement their own data members' serialisation using
  // sink class methods
  // Only the base class can invoke this, to ensure the datra size field is 
  // correctly managed
  virtual uint32_t serialize_payload(struct sink& serializer) = 0;

};




#endif /* serial_types_hpp */
