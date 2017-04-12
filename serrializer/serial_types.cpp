//
//  serial_types.cpp
//  serrializer
//
//  Created by B Wigley on 12/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#include "serial_types.hpp"

#include "app_types.h"

template<typename T> fb_serial * createInstance() { return new T; }
typedef std::map<std::string, fb_serial*(*)()> map_type;
map_type g_map;


  fb_serial* serial_types::create(std::string& name)
  {
    g_map["gpsthing"] = &createInstance<gps_position>;    
    
    std::map<std::string, fb_serial*(*)()>::iterator it = g_map.find(name.c_str());
    if(it == g_map.end())
      return NULL;
    
    return (*it).second(); 
  }
