//
//  app_types.h
//  serrializer
//
//  Created by B Wigley on 12/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#ifndef app_types_h
#define app_types_h

#include "serial_types.hpp"
#include "serialize.h"

/*
 pgs coords class
 demonstrates how data members might be serialized 
*/
struct gps_position : public fb_serial_v1
{
  int degrees; //luckily the compiler objects to these non size specific types, so we'll have to cast
  int minutes;
  float seconds;
  
  gps_position():fb_serial_v1(2,gps_position::name()){}
  
  gps_position(int d, int m, float s) :  degrees(d), minutes(m), 
    seconds(s) ,fb_serial_v1(2,gps_position::name()){}
  
  static std::string name(){ return "gpsthing";}
  
  uint32_t serialize_payload(class sink& serializer) 
  {
    uint32_t type_size=
      sizeof  degrees +
      sizeof  minutes +
      sizeof  seconds ;
    
    // optionasl - if we're expecting POD types of specific total sizes
    // check it here
    if(!serializer.verify_data_size(type_size))
    {
      return 0;  
    }
    
    uint32_t done =0;
    done += serializer.serialize_((uint32_t&)degrees);
    done += serializer.serialize_((uint32_t&)minutes);
    done += serializer.serialize_((float&)seconds);
    return done;
  }  
  
};

/*
 IM Text coords class
 demonstrates how data members might be serialized 
 */
struct text_im : public fb_serial_v1
{
  //config for this class
  static std::string name(){ return "text_im";}
  static uint16_t version(){ return 1;}
  
  std::string _message;
  
  text_im():fb_serial_v1(text_im::version(),text_im::name()){}  
  text_im(std::string message) : _message(message) ,fb_serial_v1(text_im::version(),text_im::name()){}
    
  uint32_t serialize_payload(class sink& serializer) 
  {
    uint32_t done =0;
    uint16_t len=_message.length();
    done += serializer.serialize_(_message,len);
    return done;
  }  
  
};

#endif /* app_types_h */
