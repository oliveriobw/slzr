//
//  serial_types.cpp
//  serializer
//
//  Created by B Wigley on 12/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#include "serial_types_common.hpp"
#include "serial_types_map.h"
#include "app_types.h"
#include <assert.h>

template<typename T> fb_serial_v1 * create_inst() { return new T; }

/*
 * gets the static map of serializable types we support in this build
 * the map enables persisted/streamed types to be unpacked into specific
 * class instances 
 */
map_type& serial_types::get_type_map()
{
  static map_type g_map;
  
  // every type registered here needs a unique name which is used to map
  // it to the appropriate constructor
  g_map[gps_position::name()] = &create_inst<gps_position>;
  g_map[text_im::name()] = &create_inst<text_im>;  
  g_map[compound_type::name()] = &create_inst<compound_type>;  
  g_map[list_type::name()] = &create_inst<list_type>;  
  g_map[types_test::name()] = &create_inst<types_test>;  
  
  return g_map;    
}

fb_serial_v1* serial_types::create(std::string& name)
{
  static map_type& m = serial_types::get_type_map();
  
  std::map<std::string, fb_serial_v1*(*)()>::iterator it = m.find(name.c_str());
  if(it == m.end())
  {
    assert(0);   //register your new type first
    return NULL;
  }
  
  return (*it).second(); 
}
