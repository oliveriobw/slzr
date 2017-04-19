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
  
    //also checks an internal buffer of data 
    memset(g.random_buf,'\0',10);
    g.random_buf[0] = 'a';
    g.random_buf[1] = 'b';
    g.random_buf[9] = 'c'; //ensures nulls are ignored
    fs_sink oa(file,true);
    oa.pack(&g); 
    
      //recover
    fb_serial_v1* fb = NULL;
    fs_sink input(file,false);
    input.unpack(&fb);        
    gps_position* b = dynamic_cast<gps_position*>(fb);
    assert(b);
    if(b)
    {
      cout << "decoded gps_position instance:" << b->seconds << endl;
      assert(b->random_buf[0]=='a');
      assert(b->random_buf[1]=='b');
      assert(b->random_buf[9]=='c');
      delete b;
    }
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
    assert(b);
    if(b)
    {
      cout << "decoded text_im instance:" << b->_message << endl;
      delete b;
    }
  }
  
  {    
    const int bufsz=100;
    char buffer[bufsz];
    memset(buffer,'v',bufsz);
    char buffer2[bufsz];
    memset(buffer2,'v',bufsz);

    //unpack two types with one sink instance
    text_im g("12345678910 happy-day");
    gps_position gps(30, 25, 24.123f);
    gps.random_buf[0] = 'a';
    gps.random_buf[1] = 'b';
    gps.random_buf[9] = 'c';
    
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
    assert(b);
    if(b)
    {
      cout << "decoded text_im instance:" << b->_message << endl;
    }
    
    //if we don't have rtti
    if(fb->_hdr._class_name==text_im::name())
    {
      b = (text_im*)fb;
      cout << "decoded text_im instance:" << b->_message << endl;
    }
    delete fb;
    fb = NULL;

    //supply a new input stream
    net_stream_in.set_buf(buffer2,bufsz);
    net_stream_in.unpack(&fb);        
    gps_position* bg = dynamic_cast<gps_position*>(fb);
    assert(bg);
    if(bg)
    {
      cout << "decoded gps_position instance:" << bg->seconds << endl;
      assert(bg->random_buf[0]=='a');
      assert(bg->random_buf[1]=='b');
      assert(bg->random_buf[9]=='c');
    
      delete bg;
    }
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
    assert(b);
    if(b)
    {
      cout << "decoded compound_type instance with text_im:" << b->im._message << endl;
      cout << "decoded compound_type instance with gps_position:" << b->pos.seconds << endl;
      delete b;
    }
  }

  {
    //variable lists tests
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
    assert(d);
    if(d)
    {
      std::vector<text_im*>::iterator it = d->list.begin();
      for(;it!=d->list.end();++it)
      {
        cout << "decoded list_type instance:" << (*it)->_message << endl;
      }    
      delete d;
    }
  }

  
  {
    //all native types tests (max values)

    //the buffers could be larger in some instances but not here
    const int bufsz = sizeof(types_test); 
    char buffer[bufsz];
    memset(buffer,'v',bufsz);
    
    types_test g;
    g.a = INT8_MAX;
    g.b = INT16_MAX;
    g.d = INT32_MAX;
    g.e = INT64_MAX;
    g.ua = UINT8_MAX;
    g.ub = UINT16_MAX;
    g.ud = UINT32_MAX;
    g.ue = UINT64_MAX;
    
    //pack into buffer
    fs_sink net_stream(buffer,bufsz,true);
    net_stream.pack(&g);
        
    //decode buffer into type:
    fb_serial_v1* fb = NULL;
    fs_sink net_stream_in(buffer,bufsz,false);    
    net_stream_in.unpack(&fb);        
    
    types_test* b = dynamic_cast<types_test*>(fb);
    assert(b);
    if(b)
    {
      assert(b->a == INT8_MAX);
      assert(b->b == INT16_MAX);
      assert(b->d == INT32_MAX);
      assert(b->e == INT64_MAX);
      assert(b->ua == UINT8_MAX);
      assert(b->ub == UINT16_MAX);
      assert(b->ud == UINT32_MAX);
      assert(b->ue == UINT64_MAX);    
      cout << "decoded types_test instance:" <<  endl;
      delete b;
    }
  }  

  
  {
    //all native types tests (minimum values)
  
    //the buffers could be larger in some instances but not here
    const int bufsz = sizeof(types_test); 
    char buffer[bufsz];
    memset(buffer,'v',bufsz);
    
    types_test g;
    g.a = INT8_MIN;
    g.b = INT16_MIN;
    g.d = INT32_MIN;
    g.e = INT64_MIN;
    g.ua = 0;
    g.ub = 0;
    g.ud = 0;
    g.ue = 0;
    
    //pack into buffer
    fs_sink net_stream(buffer,bufsz,true);
    net_stream.pack(&g);
    
    //decode buffer into type:
    fb_serial_v1* fb = NULL;
    fs_sink net_stream_in(buffer,bufsz,false);    
    net_stream_in.unpack(&fb);        
    
    types_test* b = dynamic_cast<types_test*>(fb);
    assert(b);
    if(b)
    {
      assert(b->a == INT8_MIN);
      assert(b->b == INT16_MIN);
      assert(b->d == INT32_MIN);
      assert(b->e == INT64_MIN);
      assert(b->ua == 0);
      assert(b->ub == 0);
      assert(b->ud == 0);
      assert(b->ue == 0);    
      cout << "decoded types_test instance:" <<  endl;
      delete b;
    }
  }  
  return 0;
}
