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

class serialize_read;

struct fb_serial{
  //template <class T> uint32_t serialize(T& serializer) const;  
  //virtual unsigned int serialize(serialize_read*&) =0;

};

struct serial_types 
{
  static fb_serial* create(std::string& name);
};


#endif /* serial_types_hpp */
