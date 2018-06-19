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

struct a{
    char* b;
    a(){
        b=new char[4];
    }
    ~a(){
        delete b;
    }
    a(a&& p){
        b=p.b;
        p.b=nullptr;
    }
};

#if defined IOS
int test()
#else
int main(int argc, const char * argv[]) 
#endif
{
    
    a s;  
    a d = std::move(s);
    
  const char* file = "/tmp/test_data.bin";  
  
  {
    coords g;
    g._lat = -10.67;
    g._lng = 53.3456;
  
    sink oa(file,true);    //archive file
    oa.pack(&g);            //persist to file
  
    //restore data
    sink input(file,false);
    fb_serial_v1* fb = input.unpack();        
    coords* b = dynamic_cast<coords*>(fb);
    assert(b);
    assert(b->_lat == g._lat);
    assert(b->_lng == g._lng);
  
    //test repeat unpack
    std::shared_ptr<coords> valid = input.unpack<coords>();    // new object
    assert(valid);

    std::shared_ptr<gps_position> invalid = input.unpack<gps_position>();    // new object
    assert(!invalid);

  std::shared_ptr<text_im> im = input.unpack<text_im>();    // new object
  assert(!im);

    valid = input.unpack<coords>();    // new object
    assert(valid);

  }
  
  {
    coords g;
    g._lat = -34.67;
    g._lng = 65.3456;
  
    //pack into internally managed buffer
    sink net_stream;
    net_stream.pack(&g);
  
    //decode buffer into type:
    sink input(net_stream.data());    
    fb_serial_v1* fb = input.unpack();        
 
    coords* b = dynamic_cast<coords*>(fb);
    assert(b);
    if(b)
    {
      cout << "decoded coords instance:" <<  endl;
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
    g.random_data();
    g.print();
  
    sink oa(file,true);
    oa.pack(&g); 
    
      //recover
    sink input(file,false);
    fb_serial_v1* fb = input.unpack();        
    gps_position* b = dynamic_cast<gps_position*>(fb);
    assert(b);
    if(b)
    {
      cout << "decoded gps_position instance:" << b->seconds << endl;
      assert(b->random_buf[0]=='a');
      assert(b->random_buf[1]=='b');
      assert(b->random_buf[9]=='c');
      assert(g.buf.size() == b->buf.size());
      for(int c=0;c< g.buf.size();c++)
         assert(g.buf[c] == b->buf[c]);

      assert(g.sbuf.size() == b->sbuf.size());
      for(int c=0;c< g.sbuf.size();c++)
        assert(g.sbuf[c] == b->sbuf[c]);

    }
  }
  
  {
    //serialize
    text_im g("12345");
    sink oa(file,true);
    oa.pack(&g);
    
    //deserialize
    sink input(file,false);
    fb_serial_v1* fb = input.unpack();        
    text_im* b = dynamic_cast<text_im*>(fb);
    assert(b);
    if(b)
    {
      assert(b->_message=="12345");
      cout << "decoded text_im instance:" << b->_message << endl;
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
    sink input(net_stream.data());    
    fb_serial_v1* fb = input.unpack();        

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
    fb = NULL;

#if 0
    //supply a new input stream of packed data
    input.set_buf(buffer2,bufsz);
    fb = input.unpack();        
    gps_position* bg = dynamic_cast<gps_position*>(fb);
    assert(bg);
    if(bg)
    {
      cout << "decoded gps_position instance:" << bg->seconds << endl;
      assert(bg->random_buf[0]=='a');
      assert(bg->random_buf[1]=='b');
      assert(bg->random_buf[9]=='c');
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
    sink input(file,false);
    fb_serial_v1* fb = input.unpack();        
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
    }
  }

  {
    //variable lists tests
    list_type l;
    text_im* a = new text_im("hello");
    text_im* b = new text_im("world");
    l.list.push_back(a);
    l.list.push_back(b);

    text_im* va = new text_im("hello vec");
    text_im* vb = new text_im("world vec");
    l.vec.push_back(va);
    l.vec.push_back(vb);
  
    sink oa(file,true);
    oa.pack(&l); 
    
    sink input(file,false);
    fb_serial_v1* fb = input.unpack();        
    list_type* d = dynamic_cast<list_type*>(fb);
    assert(d);
    if(d)
    {
      assert(d->vec.size()==2);
      assert(d->list.size()==2);
      list<text_im*>::iterator it = d->list.begin();
      for(;it!=d->list.end();++it)
      {
        cout << "decoded list_type instance:" << (*it)->_message << endl;
      }    

      vector<text_im*>::iterator vit = d->vec.begin();
      for(;vit!=d->vec.end();++vit)
      {
        cout << "decoded list_type instance:" << (*vit)->_message << endl;
      }    
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
    sink input(net_stream.data());    
    fb_serial_v1* fb = input.unpack();        
    
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
    sink input(net_stream.data());    
    fb_serial_v1* fb = input.unpack();        
    
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
    }
  }  
  
  
  {
    //using memory buffers not files => base 64 usage
    //unpack two types with one serial instance
    const char* cstr = "12345678910 this is the day my life will surely ...";
    text_im g(cstr);
    string tmp = g.tob64();
    
    //decode incoming b64 buffer into one of your types:
    sink input(tmp);    
    fb_serial_v1* fb = input.unpack();        
  
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
  }
  
  {
    //compound types
    has_pointer g;
    g.p_pos = new gps_position();
    g.p_pos->minutes=33;
    g.p_pos->seconds=3.3;
    g.p_pos->degrees=8;
  
    sink oa(file,true);
    oa.pack(&g);
  
    //deserialize
    sink input(file,false);
    fb_serial_v1* fb = input.unpack();        
    has_pointer* b = dynamic_cast<has_pointer*>(fb);
    assert(b);
    if(b)
    {
      assert(b->p_pos->minutes == g.p_pos->minutes);
      assert(b->p_pos->seconds == g.p_pos->seconds);
      assert(b->p_pos->degrees == g.p_pos->degrees);
    }
  }
  
  {
    //compound types
    has_pointer g;
  
    sink oa(file,true);
    oa.pack(&g);
  
    //deserialize
    sink input(file,false);
    fb_serial_v1* fb = input.unpack();        
    has_pointer* b = dynamic_cast<has_pointer*>(fb);
    assert(b);
    if(b)
    {
      assert(b->p_pos == NULL);
      assert(b->p_pos == g.p_pos);
    }
  }
    
    
    {
    //long types
    ax_state_msg g;
//    g.timestamps.push_back(time(NULL));
//    g.timestamps.push_back(time(NULL)+100);
//    g.timestamps.push_back(time(NULL)+1000);
    
    sink oa(file,true);
    oa.pack(&g);
    
        //deserialize
    sink input(file,false);
    fb_serial_v1* fb = input.unpack();        
    ax_state_msg* b = dynamic_cast<ax_state_msg*>(fb);
    assert(b);
    if(b)
     {
//     vector<uint64_t>::iterator i = g.timestamps.begin();
//     for(int f=0;i!=g.timestamps.end();++i,++f)
//         assert((*i)==b->timestamps[f]);
     }
    }
  

    {
        ax_usr_state g;
        g.setup();    
        sink oa(file,true);
        oa.pack(&g);
    
        //deserialize
        sink input(file,false);
        fb_serial_v1* fb = input.unpack();        
        ax_usr_state* b = dynamic_cast<ax_usr_state*>(fb);
        assert(b);
        if(b)
        {
        for(int f=0;f<32;++f){
            assert(g.rk[f]==b->rk[f]);
            assert(g.cks[f]==b->cks[f]);
            assert(g.ckr[f]==b->ckr[f]);
        }
        assert(g.last_sent_keyrequest==b->last_sent_keyrequest);
        assert(g.last_received_keyrequest==b->last_received_keyrequest);
        }    
    }

    /**
     *
     *
     *                   OBJECT SCOPE - unpack
     * 
     *
     */
    ax_usr_state* b = NULL;
    ax_usr_state state;
    {
        vector<uint8_t> data;
        state.setup();    
        struct sink mem_sink;
        long resut = mem_sink.pack(&state);
        data = mem_sink.data();
    
        sink net_stream_in(data);
        fb_serial_v1* fb = net_stream_in.unpack();    // new object
        b = dynamic_cast<ax_usr_state*>(fb);
        assert(b);
    }
    
    {
    if(b)
    {
        for(int f=0;f<32;++f){
            assert(state.rk[f]==b->rk[f]);
            assert(state.cks[f]==b->cks[f]);
            assert(state.ckr[f]==b->ckr[f]);
        }
        assert(state.last_sent_keyrequest==b->last_sent_keyrequest);
        assert(state.last_received_keyrequest==b->last_received_keyrequest);
        delete b; //we need to clear it
    }    
    
    }
    
    {
        ax_usr_state* b = new ax_usr_state;
        b->setup();    
        struct sink mem_sink;
        long resut = mem_sink.pack(b);
    }
    
    
    /**
     *
     *
     *                   OBJECT SCOPE - unpack to shared_ptr
     * 
     *
     */
    std::shared_ptr<ax_usr_state> bshr;
    {
        vector<uint8_t> data;
        state.setup();    
        struct sink mem_sink;
        long resut = mem_sink.pack(&state);
        data = mem_sink.data();
    
        sink net_stream_in(data);
    
        //attempt unpack into invalid type
        std::shared_ptr<has_pointer> wrong = net_stream_in.unpack<has_pointer>();    // new object
        assert(!wrong);
    
        bshr = net_stream_in.unpack<ax_usr_state>();    // new object
        assert(bshr);
    }
    
    {
    if(bshr)
        {
        for(int f=0;f<32;++f){
            assert(state.rk[f]==bshr->rk[f]);
            assert(state.cks[f]==bshr->cks[f]);
            assert(state.ckr[f]==bshr->ckr[f]);
        }
        assert(state.last_sent_keyrequest==bshr->last_sent_keyrequest);
        assert(state.last_received_keyrequest==bshr->last_received_keyrequest);
        // delete b; //no need
        }    
    
    }
    
    
    //TEXT IM ATTACHMENT
    {
        //serialize
        text_im g("12345");
        app_serial_file fl;
        fl.remote_filename_ = "/volume/plop";
        g.file_ =     std::make_shared<app_serial_file>(fl);    
        sink oa(file,true);
        oa.pack(&g);
    
        //deserialize
        sink input(file,false);
        fb_serial_v1* fb = input.unpack();        
        text_im* b = dynamic_cast<text_im*>(fb);
        assert(b);
        if(b)
        {
            assert(b->_message=="12345");
            assert(b->file_.get()->remote_filename_=="/volume/plop");
            cout << "decoded text_im instance:" << b->_message << endl;
            delete b;
        }    
    }    
  
    
    {    
        //serialize
        chat_serial g("stanza-id",chat_serial::echat_serial::echat_serial_decode_fail+4);
        sink oa(file,true);
        oa.pack(&g);
    
        //deserialize
        sink input(file,false);
        fb_serial_v1* fb = input.unpack();        
        chat_serial* c = dynamic_cast<chat_serial*>(fb);
        assert(c);
        if(c)
        {
            assert(c->_event==chat_serial::echat_serial::echat_serial_decode_fail+4);
            assert(c->_stanza_id == "stanza-id");
        }  
    }
  cout << "success: all tests completed" << endl;
  
  return 0;
}
