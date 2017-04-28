//
//  serial_types_map.h
//  serrializer
//
//  Created by B Wigley on 12/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#ifndef serial_types_map_h
#define serial_types_map_h

#include <map>
#include "serial_types_common.hpp"

typedef std::map<std::string, fb_serial_v1*(*)()> map_type;

struct serial_types 
{
  static map_type& get_type_map();
  static fb_serial_v1* create(std::string& name);
};


#endif /* serial_types_map_h */
