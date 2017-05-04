  //
  //  app_types.h
  //  serrializer
  //
  //  Created by B Wigley on 12/04/2017.
  //  Copyright © 2017 fab. All rights reserved.
  //

#ifndef app_types_h
#define app_types_h

#include "serial_types_common.hpp"
#include "serialize.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

/*
 IM Text coords class
 Demonstrates how data members might be serialized 
 */
struct text_im : public fb_serial_v1
{
  //config for this class
  static std::string name() { return "text_im";}
  static uint16_t version() { return 1; }
  
  //custom payload
  std::string _message;
  
  //constructors
  text_im():fb_serial_v1(text_im::version(),text_im::name()){}  
  text_im(std::string message) : _message(message),fb_serial_v1(text_im::version(),text_im::name()){}
  ~text_im(){}
  
  // serilisation of custom data using serial class methods
  uint32_t serialize_payload(class serial& s) 
  {
    uint16_t len = _message.length();
    uint32_t done = s.serialize(_message,len);
    return done;
  }    
    
};

/*
 pgs coords class
 demonstrates how data members might be serialized 
 */
const int  bufsz  = 10;
struct gps_position : public fb_serial_v1
{
  // the custom class data
  // helpfully the compiler objects to these non size specific types, 
  // so we'll be forced to cast or change to size-specific type
  int degrees;
  int minutes;
  float seconds;
  char dummy_char;
  
  //demonstrates using fixed size buffer
  uint8_t random_buf[bufsz];
  
  std::vector<uint8_t> buf;
  
  //constructors
  gps_position():fb_serial_v1(2,gps_position::name()){}  
  gps_position(int d, int m, float s) :  degrees(d), minutes(m), 
  seconds(s) ,fb_serial_v1(2,gps_position::name()){}
  
  static std::string name()
  { 
    // Also demonstrates utf8 usage
    // Chinese characters for "zhongwen" : ‘中文’
    const char utf8_classname[] = {-28, -72, -83, -26, -106, -121, 0};
    return utf8_classname;
  }
  
  uint32_t serialize_payload(class serial& s) 
  {
    uint32_t type_size=
      sizeof  degrees +
      sizeof  minutes +
      sizeof  seconds +
      sizeof  dummy_char + bufsz;

    //
    // optional - if we're expecting POD types of specific total sizes
    // check it here
    // 
    // if we ever wanted to allow future adding to this type without breaking
    // old clients we would have to drop this step
    //
    if(!s.verify_data_size(type_size))
    {
        //std::cout << "unexpected size deserializing" << std::endl;
        //return 0;  
    }

      // just serialize data members in a predetermined order
      // this is the same code for un/serializing
    uint32_t done = 0;
    done += s.serialize((uint32_t&) degrees);
    done += s.serialize((uint32_t&) minutes);
    done += s.serialize((float&)    seconds);
    done += s.serialize((uint8_t&)  dummy_char);
  
    uint32_t sz = bufsz;
    done += s.serialize((uint8_t*)random_buf,sz);

    done += s.serialize(buf);

    return done;
  }  

  void random_data(){
    int len = 20;
    buf.resize(len);
    for(int c=0;c<len;c++)
    {
      int r = rand();
      buf[c] = (uint8_t)r;
    }
  }

  void print(){
    std::cout << "buf=";
    for(int c=0;c<buf.size();c++)
      std::cout <<  std::bitset<32>(buf[c]) << std::endl;
    std::cout << std::endl;
  }
};

struct compound_type : public fb_serial_v1
{
    //config for this class
  static std::string name() { return "compound";}
  static uint16_t version() { return 1; }
  
  gps_position pos;
  text_im im;  
  
  compound_type():fb_serial_v1(compound_type::version(),compound_type::name()){}  
  
  uint32_t serialize_payload(class serial& s) 
  {
    uint32_t done = 0;
    
      //here we have some nested ojects to serialize
    done += pos.serialize_payload(s);
    done += im.serialize_payload(s);
    
    return done;
  }    
};


struct list_type : public fb_serial_v1
{
    //config for this class
  static std::string name() { return "list_type";}
  static uint16_t version() { return 1; }
  
  std::vector<text_im*> list;
  
  list_type():fb_serial_v1(list_type::version(),list_type::name()){}
  ~list_type()
  {
    std::vector<text_im*>::iterator it = list.begin();
    for( ; it!=list.end() ; ++it) {
      delete (*it);
    }
  }  
  
  /**
   * demonstrates the linked-list scenario
   */
  uint32_t serialize_payload(class serial& s) 
  {
    uint32_t done = 0;
    
    // the count of instances must be serialized first - even if it's not an 
    // explicit data member as in this class 
    std::vector<text_im*>::size_type  list_size = list.size();
    done += s.serialize((uint32_t&)list_size);
    
    if(list_size>0 && s.unarchiver())
    {
      for(int c=0;c<list_size;++c)
        list.push_back(new text_im());
    }
    
    std::vector<text_im*>::iterator it = list.begin();
    for(;it!=list.end();++it)
    {
      done += (*it)->serialize_payload(s);      
    }
    
    return done;
  }    
};

/*
 Demonstrates how data members might be serialized - all intrinsic types tested
 */
struct types_test : public fb_serial_v1
{
    //config for this class
  static std::string name() { return "types_test";}
  static uint16_t version() { return 1; }
  
  //custom payload
  int8_t  a;
  int16_t b;
  int32_t d;
  int64_t e;
  uint8_t  ua;
  uint16_t ub;
  uint32_t ud;
  uint64_t ue;
  
    //constructors
  types_test():fb_serial_v1(types_test::version(),types_test::name()){}  
  types_test(std::string message) : fb_serial_v1(types_test::version(),types_test::name()){}
  
    // serilisation of custom data using serial class methods
  uint32_t serialize_payload(class serial& s) 
  {
    uint32_t done = 0;
    done += s.serialize((uint8_t&)a);
    done += s.serialize((uint16_t&)b);
    done += s.serialize((uint32_t&)d);
    done += s.serialize((uint64_t&)e);
    done += s.serialize(ua);
    done += s.serialize(ub);
    done += s.serialize(ud);
    done += s.serialize(ue);
    return done;
  }    
};

struct coords : public fb_serial_v1
{
  //config for this class
  static std::string name() { return "coords";}
  static uint16_t version() { return 1; }
  
  //custom payload
  float _lat,_lng;
  
  //constructors
  coords():fb_serial_v1(coords::version(),coords::name()){}  
  
  // serilisation of custom data using serial class methods
  uint32_t serialize_payload(class serial& s) 
  {
    uint32_t done = 0;
    done += s.serialize(_lat);
    done += s.serialize(_lng);
    return done;
  }    
};


#endif /* app_types_h */
