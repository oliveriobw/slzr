    //
  //  app_types.h
  //  serrializer
  //
  //  Created by B Wigley on 12/04/2017.
  //  Copyright © 2017 fab. All rights reserved.
  //

#ifndef app_types_h
#define app_types_h

#include <cinttypes>

#include "serial_types_common.hpp"
#include "serialize.h"
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <iostream>
#include <time.h>
#include "chat_serial.h"

struct app_serial_file : public fb_serial_v1
{
    //config for this class
    static std::string name() { return "file"; }
    static uint16_t version() { return 1; }
    
    //custom payload
    std::string remote_filename_;
    std::string filename_;
    std::vector<uint8_t> encryption_key_;
    
    //constructors
    app_serial_file():fb_serial_v1(app_serial_file::version(),app_serial_file::name()){}
    
    // serilisation of custom data using serial class methods
    uint32_t serialize_payload(class serial& s)
    {
    uint16_t len = filename_.length();
    uint32_t done = s.serialize(filename_, len);
    
    //need the remote location.
    len = remote_filename_.length();
    done += s.serialize(remote_filename_, len);
    
    done += s.serialize(encryption_key_);
    return done;
    }    
};


/*
 IM Text coords class
 Demonstrates how data members might be serialized 
 */
struct text_im : public fb_serial_v1
{
    //config for this class
    static std::string name() { return "message";}
    static uint16_t version() { return 1; }
    
    //custom payload
    std::string _message;
    
    std::shared_ptr<app_serial_file> file_;
    
    //constructors
    text_im():fb_serial_v1(text_im::version(),text_im::name()){}  
    text_im(const std::string text) : _message(text),fb_serial_v1(text_im::version(),text_im::name()){}
    
    // serilisation of custom data using serial class methods
    uint32_t serialize_payload(class serial& s) 
    {
    uint16_t len = _message.length();
    uint32_t done = s.serialize(_message,len);
    
    uint8_t attachment_present = file_ != nullptr ? 1 : 0;
    done += s.serialize(attachment_present);
    
    if(attachment_present==1 && s.unarchiver())
    {
    //      file_ = std::shared_ptr<app_serial_file>(new app_serial_file());
    file_ = std::make_shared<app_serial_file>();
    }
    
    if (file_)
        done += file_->serialize_payload(s);
    
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
  std::vector<int8_t>  sbuf;
  
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
  
    //fixed size buffer
    uint32_t sz = bufsz;
    done += s.serialize((uint8_t*)random_buf,sz);

    
    done += s.serialize(buf);
    done += s.serialize(sbuf);

    return done;
  }  

  void random_data(){
    srand(time(NULL));
    int len = 1200;
    buf.resize(len);
    sbuf.resize(len);
    for(int c=0;c<len;c++)
    {
      int r = rand();
      buf[c] = (uint8_t)r;
      sbuf[c] = (int8_t)r;
    }
  }

  void print(){
    std::cout << "buf=";
    for(int c=0;c<buf.size();c++) {
      std::cout <<  std::bitset<8>(buf[c]) << std::endl;
      std::cout <<  std::bitset<8>(sbuf[c]) << std::endl;
    }
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

// stl containers of supported types
struct list_type : public fb_serial_v1
{
    //config for this class
  static std::string name() { return "list_type";}
  static uint16_t version() { return 1; }
  
  std::vector<text_im*> vec;
  std::list<text_im*> list;
  
  list_type():fb_serial_v1(list_type::version(),list_type::name()){}
  ~list_type() {
    std::vector<text_im*>::iterator it = vec.begin();
    for( ; it!=vec.end() ; ++it) delete (*it);
    
    std::list<text_im*>::iterator lit = list.begin();
    for( ; lit!=list.end() ; ++lit) delete (*lit);  
  }  
  
  /**
   * demonstrates the linked-list scenario
   */
  uint32_t serialize_payload(class serial& s) 
  {
    uint32_t done = 0;
    done += ::serialize(s,list);
    done += ::serialize(s,vec);
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

//optional point to another supported type 
struct has_pointer : public fb_serial_v1
{
  //config for this class
  static std::string name() { return "has_pointer";}
  static uint16_t version() { return 1; }
  
  gps_position* p_pos; 
  
  has_pointer():fb_serial_v1(has_pointer::version(),has_pointer::name()),p_pos(NULL){}
  ~has_pointer(){ delete p_pos;}
  
  uint32_t serialize_payload(class serial& s)
  {
    uint32_t done = 0;
    done += ::serialize(s,&p_pos);
    return done;
  }   
};




/*
 * Axolotl state msg type
 */
struct ax_state_msg : public fb_serial_v1
{
    static std::string name() { return "ax_state_msg";}
    static uint16_t version() { return 1; }
    
    std::vector<uint8_t> skipped_mks;
    //std::vector<uint64_t> timestamps;
    
    ax_state_msg():fb_serial_v1(ax_state_msg::version(),ax_state_msg::name()){}  
    
    uint32_t serialize_payload(class serial& s) 
    {
    uint32_t done = 0;
    done += s.serialize(skipped_mks);
    // done += s.serialize(timestamps);
    return done;
    }    
};

struct ax_pend_msg : public fb_serial_v1
{
    static std::string name() { return "ax_pend_msg";}
    static uint16_t version() { return 1; }
        
    ax_pend_msg():fb_serial_v1(ax_pend_msg::version(),ax_pend_msg::name()){}  
    
    std::string _message;
    
    uint32_t serialize_payload(class serial& s) 
    {
        return s.serialize(_message);
    }    
};

/*
 * Axolotl state type
 */
#define KEY_SZ 32
struct ax_usr_state : public fb_serial_v1
{
    static std::string name() { return "ax_state";}
    static uint16_t version() { return 1; }
        
    uint8_t  rk[KEY_SZ];
    uint8_t  cks[KEY_SZ];
    uint8_t  ckr[KEY_SZ];
    uint8_t  dhir[KEY_SZ];
    uint8_t  dhrs[KEY_SZ];
    uint8_t  dhrspub[KEY_SZ];
    uint8_t  dhrr[KEY_SZ];
    uint16_t ns;
    uint16_t nr;
    uint16_t pns;
    uint8_t  ratchet_flag;
    uint8_t  zrtpverified; // 0 if dhir is unverified, 1 if dhir is verified
    uint8_t  status;       // enum AxolotlStatus 
    uint8_t  pending;      // number of pending messages

    std::string   last_sent_keyrequest;
    std::string   last_received_keyrequest;

//unused    
//    ax_state_msg skipped_mk;
//    std::vector<ax_pend_msg*> list;
        
    ax_usr_state():fb_serial_v1(ax_usr_state::version(),ax_usr_state::name()){}  
    ~ax_usr_state(){ _hdr._class_name=""; }
    uint32_t serialize_payload(class serial& s) 
    {
    uint32_t done = 0, len = KEY_SZ;
    
    done += s.serialize(rk,len);
    done += s.serialize(cks,len);
    done += s.serialize(ckr,len);
    done += s.serialize(dhir,len);
    done += s.serialize(dhrs,len);
    done += s.serialize(dhrspub,len);
    done += s.serialize(dhrr,len);
    
    done += s.serialize(ns);
    done += s.serialize(nr);
    done += s.serialize(pns);
    
    done += s.serialize(ratchet_flag);
    done += s.serialize(zrtpverified);
    done += s.serialize(status);
    done += s.serialize(pending);
    done += s.serialize(last_sent_keyrequest);
    done += s.serialize(last_received_keyrequest);

    
    return done;
    }    
    
    void setup(){
        for(int c=0;c<KEY_SZ;++c){
            rk[c] = rand();
            cks[c] = rand();
            ckr[c] = rand();
        }
        last_sent_keyrequest="?AXLDHSGDSHGDFJHSFD";
        last_received_keyrequest="";
    }
};


#endif /* app_types_h */







