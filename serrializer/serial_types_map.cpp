//
//  serial_types.cpp
//  serial
//
//  Created by B Wigley on 12/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#include "serial_types_common.hpp"
#include "serial_types_map.h"
#include "app_types.h"
#include <assert.h>

using namespace std;

static map_type g_map;

template<typename t> fb_serial_v1 * create_inst() {
  return new t; 
}

template <typename t> void register_class(){
  g_map[t::name()] = &create_inst<t>;  
}

/*
 * gets the static map of serializable types we support in this build
 * the map enables persisted/streamed types to be unpacked into specific
 * class instances 
 */
map_type& serial_types::get_type_map()
{  
  // every type registered here needs a unique name which is used to map
  // it to the appropriate constructor
  register_class<gps_position>();
  register_class<text_im>();
  register_class<compound_type>();
  register_class<list_type>();
  register_class<types_test>();
  register_class<coords>();
  register_class<has_pointer>();
  
  return g_map;    
}

fb_serial_v1* serial_types::create(string& name)
{
  static map_type& m = serial_types::get_type_map();
  
  map<string, fb_serial_v1*(*)()>::iterator it = m.find(name.c_str());
  if(it == m.end())
  {
    assert(0);   //register your new type first
    return NULL;
  }
  
  return (*it).second(); 
}
