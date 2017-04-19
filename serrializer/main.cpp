//
//  main.cpp
//  serializer
//
//  Created by B Wigley on 10/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#include "machine.h"
#include "app_types.h"
#include "fs_sink.hpp"
#include <fstream>
#include <sstream>

using namespace std;

int main(int argc, const char * argv[]) 
{
  // save data to archive
  {
    const char* file = "/tmp/dohp11.bin";  
    gps_position g(35, 59, 24.567f);
    fs_sink oa(file,true);
    oa.pack(&g); 
    
      //recover
    fb_serial_v1* fb = NULL;
    fs_sink input(file,false);
    input.unpack(&fb);        
    gps_position* b = dynamic_cast<gps_position*>(fb);
    if(b)
      cout << "decoded gps_position instance:" << b->seconds << endl;
  }
  
  {
    const char* file = "/tmp/d.bin";  
    //serialize
    text_im g("12345");
    fs_sink oa(file,true);
    oa.pack(&g);
    
    //deserialize
    fb_serial_v1* fb = NULL;
    fs_sink input(file,false);
    input.unpack(&fb);        
    text_im* b = dynamic_cast<text_im*>(fb);
    if(b)
      cout << "decoded text_im instance:" << b->_message << endl;
  }
  
  {    
    const int bufsz=100;
    char buffer[bufsz];
    memset(buffer,'v',bufsz);
    char buffer2[bufsz];
    memset(buffer2,'v',bufsz);

    //unpack two types with one sink instance
    text_im g("12345678910-happy-day");
    gps_position gps(30, 25, 24.123f);
    
    //memory buffer / network stream example
    fs_sink net_stream(buffer,bufsz,true);
    net_stream.pack(&g);

    fs_sink net_stream2(buffer2,bufsz,true);
    net_stream2.pack(&gps);
    
    //decode incoming buffers:
    fb_serial_v1* fb = NULL;
    fs_sink net_stream_in(buffer,bufsz,false);
    
    net_stream_in.unpack(&fb);        

    //two ways to establish the decoded type:
    text_im* b = dynamic_cast<text_im*>(fb);
    if(b)
      cout << "decoded text_im instance:" << b->_message << endl;
    
    //if we don't have rtti
    if(fb->_hdr._class_name==text_im::name())
    {
      b = (text_im*)fb;
      cout << "decoded text_im instance:" << b->_message << endl;
    }

    //supply a new input stream
    net_stream_in.set_buf(buffer2,bufsz);
    net_stream_in.unpack(&fb);        
    gps_position* bg = dynamic_cast<gps_position*>(fb);
    if(bg)
      cout << "decoded gps_position instance:" << bg->seconds << endl;
  }

  {
    //compound types
    const char* file = "/tmp/dddww.bin";  
    compound_type g;
    fs_sink oa(file,true);
    oa.pack(&g);
    
    //deserialize
    fb_serial_v1* fb = NULL;
    fs_sink input(file,false);
    input.unpack(&fb);        
    compound_type* b = dynamic_cast<compound_type*>(fb);
    if(b)
    {
      cout << "decoded compound_type instance with text_im:" << b->im._message << endl;
      cout << "decoded compound_type instance with gps_position:" << b->pos.seconds << endl;
    }
  }

  {
    const char* file = "/tmp/dohlist.bin";
    list_type l;
    text_im* a = new text_im("hello");
    text_im* b = new text_im("world");
    l.list.push_back(a);
    l.list.push_back(b);
  
    fs_sink oa(file,true);
    oa.pack(&l); 
    
    fb_serial_v1* fb = NULL;
    fs_sink input(file,false);
    input.unpack(&fb);        
    list_type* d = dynamic_cast<list_type*>(fb);
    if(d)
    {
      std::vector<text_im*>::iterator it = d->list.begin();
      for(;it!=d->list.end();++it)
      {
        cout << "decoded list_type instance:" << (*it)->_message << endl;
      }    
    }
  }

  return 0;
}
