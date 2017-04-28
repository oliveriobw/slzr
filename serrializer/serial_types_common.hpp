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

#define FB_SERIAL_PROT_VER (0x01) 

/*
 
 Protocol Specification
 
 
 
 1             2               3               4
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |       Protocol Version      |        Class Name Size          |       
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                      Class Name UTF8                          |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |      Class  Version         |          Data Size              |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |       Data Size             |               data              |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                             data                              |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 
 C++ Types:
 
 fb_serial_header manages these fields:
      Protocol Version
      Class Name Size
      Class Name UTF8
      Class Version
 
 fb_serial_v1
      Implements class managment for Protocol Version 1 (latest/only version)
      Custom types needing serialisation support inherit from this class. 
                 
*/


/**
 *   fb_serial_header represents the common binary header for all serialisable 
 *   types. 
 *   
 *   A traditional c struct POD type (not inherited)  
 *
 */
struct fb_serial_header{

  uint16_t     _protocol_version;
  uint16_t     _class_name_sz;
  std::string  _class_name;
  uint16_t     _class_version;  //the nested class version
  
  // Needed only for deserialisation
  fb_serial_header(){}
  
  fb_serial_header(uint16_t pro_ver,uint16_t ver, std::string class_id):
    _protocol_version(pro_ver),
    _class_version(ver),
    _class_name(class_id),
    _class_name_sz(class_id.length()) {}
};

/*
 *
 *  Base class for all types needing serialize support  
 *
*/
struct fb_serial_v1{

  //Common header
  fb_serial_header _hdr;
  
  //Classes provide versions and unique names 
  fb_serial_v1(uint16_t ver, std::string class_id):
     _hdr(FB_SERIAL_PROT_VER,ver,class_id) {}

  // FYI: We do not want a default constructor here as it is an abstract class
  // but more importantly it's essential the class is created with a specific
  // protocol version and class name
  
  // Will need delete via derived-class on heap usage:
  // FYI: There is no need to redeclare the destructors virtual in the derived 
  // classes or provide empty implementations. 
  virtual ~fb_serial_v1(){}

  /**
   * Serialises custom payloads including the size field
   */
  virtual uint32_t serialize(serial& serial)
  {
    serial.serialize_data_size_init();
    uint32_t size = serialize_payload(serial);
    size += serial.serialize_data_size(size);
    return size;
    
  }
  
  //packs into base64
  std::string& tob64();
  
  protected:
  
  // classes are to implement their own data members' serialisation using
  // serial class methods
  // Only the base class can invoke this, to ensure the datra size field is 
  // correctly managed
  // Must return size or data written/read
  virtual uint32_t serialize_payload(struct serial& serial) = 0;

  std::string b64;
};




#endif /* serial_types_hpp */
