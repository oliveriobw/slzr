//
//  chat_serial.h
//  fb_sdk
//
//  Created by B Wigley on 28/04/2017.
//  Copyright © 2017 fabric. All rights reserved.
//

#ifndef chat_serial_h
#define chat_serial_h

#include "serial_types_common.hpp"
#include "serialize.h"
#include <vector>

/*
 * Internal message: general purpose chat event/status
 */
struct chat_serial : public fb_serial_v1
{
    //config for this class
    static std::string name() { return "chat_serial";}
    static uint16_t version() { return 1; }
  
    enum echat_serial
    {
      echat_serial_decode_fail
    };
    
    //custom payload
    std::string _stanza_id;
    uint8_t  _event;  //echat_serial
    
    //constructors
    chat_serial():fb_serial_v1(chat_serial::version(),chat_serial::name()){}  
    chat_serial(std::string b,uint8_t event) : _stanza_id(b),_event(event), fb_serial_v1(chat_serial::version(),chat_serial::name()){}
  
    // serilisation of custom data using serial class methods
    uint32_t serialize_payload(class serial& s) 
    {
        uint16_t len  = _stanza_id.length();
        uint32_t done = s.serialize(_stanza_id,len);
        done += s.serialize(_event);
        return done;
    }    
};

#endif /* chat_serial_h */
