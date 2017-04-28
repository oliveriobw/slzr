//
//  main.cpp
//  serial
//
//  Demonstrates serialisation classes:
//
//    Types can be serialised/deserialised to/from files or 
//    buffers ready for network transmission. See app_types.h for sample classes. 
//
//    Data is serialised in network byte order and deserialised into host byte 
//    order. 
//
//  Created by O Wigley on 10/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#include "app_types.h"
#include "sink.hpp"
#include <fstream>
#include <sstream>

using namespace std;

int main(int argc, const char * argv[]) 
{
  const char* file = "/tmp/test_data.bin";  
  
  {
    coords g;
    g._lat = -10.67;
    g._lng = 53.3456;
  
    sink oa(file,true);    //archive file
    oa.pack(&g);            //persist to file
  
    //restore data
    fb_serial_v1* fb = NULL;
    sink input(file,false);
    input.unpack(&fb);        
    coords* b = dynamic_cast<coords*>(fb);
    assert(b);
    assert(b->_lat == g._lat);
    assert(b->_lng == g._lng);
    delete b;
  }
  
  {
    coords g;
    g._lat = -34.67;
    g._lng = 65.3456;
  
    //pack into internally managed buffer
    sink net_stream;
    net_stream.pack(&g);
  
    //decode buffer into type:
    fb_serial_v1* fb = NULL;
    sink net_stream_in(net_stream.data());    
    net_stream_in.unpack(&fb);        
  
    coords* b = dynamic_cast<coords*>(fb);
    assert(b);
    if(b)
    {
      cout << "decoded coords instance:" <<  endl;
      delete b;
    }  
  }
  // save data to archive
  {
    gps_position g(35, 59, 24.567f);
  
    //also checks an internal buffer of data 
    memset(g.random_buf,'\0',10);
    g.random_buf[0] = 'a';
    g.random_buf[1] = 'b';
    g.random_buf[9] = 'c'; //ensures nulls are ignored
    sink oa(file,true);
    oa.pack(&g); 
    
      //recover
    fb_serial_v1* fb = NULL;
    sink input(file,false);
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
    //serialize
    text_im g("12345");
    sink oa(file,true);
    oa.pack(&g);
    
    //deserialize
    fb_serial_v1* fb = NULL;
    sink input(file,false);
    input.unpack(&fb);        
    text_im* b = dynamic_cast<text_im*>(fb);
    assert(b);
    if(b)
    {
      assert(b->_message=="12345");
      cout << "decoded text_im instance:" << b->_message << endl;
      delete b;
    }
  }
  
  {
    //using memory buffers not files

    //unpack two types with one serial instance
    const char* cstr = "12345678910 this is the day my life will surely ...";
    text_im g(cstr);
    gps_position gps(30, 25, 24.123f);
    gps.random_buf[0] = 'a';
    gps.random_buf[1] = 'b';
    gps.random_buf[9] = 'c';
    
    //pack into internally managed buffer
    sink net_stream;
    net_stream.pack(&g);

    sink net_stream2;
    net_stream2.pack(&gps);
    
    //decode incoming buffers:
    fb_serial_v1* fb = NULL;
    sink net_stream_in(net_stream.data());    
    net_stream_in.unpack(&fb);        

    //two ways to establish the decoded type:
    text_im* b = dynamic_cast<text_im*>(fb);
    assert(b);
    if(b)
    {
      assert(b->_message==cstr);
      cout << "decoded text_im instance:" << b->_message << endl;
    }
    
    //if we don't have rtti
    if(fb->_hdr._class_name==text_im::name())
    {
      b = (text_im*)fb;
      assert(b->_message==cstr);
      cout << "decoded text_im instance:" << b->_message << endl;
    }
    delete fb;
    fb = NULL;

#if 0
    //supply a new input stream of packed data
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
#endif
  
  }

  {
    //compound types
    compound_type g;  
    g.pos.minutes=33;
    g.pos.seconds=3.3;
    g.pos.degrees=8;
    g.im._message="hi people";
  
    sink oa(file,true);
    oa.pack(&g);
    
    //deserialize
    fb_serial_v1* fb = NULL;
    sink input(file,false);
    input.unpack(&fb);        
    compound_type* b = dynamic_cast<compound_type*>(fb);
    assert(b);
    if(b)
    {
      assert(b->im._message == g.im._message);
      assert(b->pos.minutes == g.pos.minutes);
      assert(b->pos.seconds == g.pos.seconds);
      assert(b->pos.degrees == g.pos.degrees);
      cout << "decoded compound_type instance with text_im:" << b->im._message << endl;
      cout << "decoded compound_type instance with gps_position:" << b->pos.seconds << endl;
      delete b;
    }
  }

  {
    //variable lists tests
    list_type l;
    text_im* a = new text_im("hello");
    text_im* b = new text_im("world");
    l.list.push_back(a);
    l.list.push_back(b);
  
    sink oa(file,true);
    oa.pack(&l); 
    
    fb_serial_v1* fb = NULL;
    sink input(file,false);
    input.unpack(&fb);        
    list_type* d = dynamic_cast<list_type*>(fb);
    assert(d);
    if(d)
    {
      vector<text_im*>::iterator it = d->list.begin();
      for(;it!=d->list.end();++it)
      {
        cout << "decoded list_type instance:" << (*it)->_message << endl;
      }    
      delete d;
    }
  }

  
  {
    //all native types tests (max values)

    types_test g;
    g.a = INT8_MAX;
    g.b = INT16_MAX;
    g.d = INT32_MAX;
    g.e = INT64_MAX;
    g.ua = UINT8_MAX;
    g.ub = UINT16_MAX;
    g.ud = UINT32_MAX;
    g.ue = UINT64_MAX;
    
    //pack into internally managed buffer
    sink net_stream;
    net_stream.pack(&g);
        
    //decode buffer into type:
    fb_serial_v1* fb = NULL;
    sink net_stream_in(net_stream.data());    
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
      
    types_test g;
    g.a = INT8_MIN;
    g.b = INT16_MIN;
    g.d = INT32_MIN;
    g.e = INT64_MIN;
    g.ua = 0;
    g.ub = 0;
    g.ud = 0;
    g.ue = 0;
    
    //pack into internally managed buffer
    sink net_stream;
    net_stream.pack(&g);
    
    //decode buffer into type:
    fb_serial_v1* fb = NULL;
    sink net_stream_in(net_stream.data());    
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
  
  
  {
    //using memory buffers not files => base 64 usage
    //unpack two types with one serial instance
    const char* cstr = "12345678910 this is the day my life will surely ...";
    text_im g(cstr);
    string tmp = g.tob64();
    
    //decode incoming b64 buffer into one of your types:
    fb_serial_v1* fb = NULL;
    sink net_stream_in(tmp);    
    net_stream_in.unpack(&fb);        
  
    //two ways to establish the decoded type:
    text_im* b = dynamic_cast<text_im*>(fb);
    assert(b);
    if(b)
    {
      assert(b->_message==cstr);
      cout << "decoded text_im instance:" << b->_message << endl;
    }
  
    //if we don't have rtti
    if(fb->_hdr._class_name==text_im::name())
    {
      b = (text_im*)fb;
      assert(b->_message==cstr);
      cout << "decoded text_im instance:" << b->_message << endl;
    }
    delete fb;
    fb = NULL;  
  }
  
  cout << "success: all tests completed" << endl;
  
  return 0;
}
