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
//#include <map>


class gps_position : public fb_serial
{
private:
  int degrees;
  int minutes;
  float seconds;
public:
  gps_position(){}
  gps_position(int d, int m, float s) :  degrees(d), minutes(m), seconds(s) {}
  
  std::string name() const {return "gpsthing";}  
  
  template <class T> uint32_t serialize(T& serializer) const
  {
  uint32_t written =0;
  written += serializer->serialize_(degrees);
  written += serializer->serialize_(minutes);
  written += serializer->serialize_(seconds);
  return written;
  }  
  
};



//struct serial_types 
//{
//  static fb_serial* create(std::string& name)
//  {
//    g_map["gpsthing"] = &createInstance<gps_position>;    
//    
//    //if(g_map.find(name.c_str()) == g_map.end())
//    {
//      return NULL;
//    }
//    return NULL; 
//  }
//};


#endif /* app_types_h */
