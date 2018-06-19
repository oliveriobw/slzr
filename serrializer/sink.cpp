//
//  sink.cpp
//  serrializer
//
//  Created by B Wigley on 12/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#include "sink.hpp"
#include <fstream>
#include <sstream>
#include "base64.h"
#ifdef __ANDROID__
#include <sys/endian.h>
#endif
using namespace std;

/**
 * Prepares buffer for unpacking from data buffer
 */
sink::sink(std::vector<uint8_t>& d):_ofsx(NULL),_ifs(NULL),_pack(false)                  //unpack from memory buffer
{
      _ifs = new stringstream(ios::in |ios::out| ifstream::binary);
      streambuf * pbuf = _ifs->rdbuf();
      pbuf->sputn ((const char*)d.data(),d.size());
}

/**
 * Prepares internal buffer for b64 unpacking
 */
sink::sink(const std::string& base64):_ofsx(NULL),_ifs(NULL),_pack(false)
{
  string base64_src = base64_decode(base64);
  if(base64_src.size()>0)
  {
    for(int c=0;c<base64_src.size();++c)
        _data.push_back(base64_src[c]);
  }

  _ifs = new stringstream(ios::in |ios::out| ifstream::binary);
  streambuf * pbuf = _ifs->rdbuf();
    pbuf->sputn ((const char*)_data.data(),_data.size());
    
}

/**
 * Prepares buffer for packing
 */
sink::sink():_ofsx(NULL),_ifs(NULL),_pack(true)
{
  _ofsx = new stringstream( ios::out |ios::in | ofstream::binary);
}

/**
 * Prepares file for packing/unpacking
 */
sink::sink(const char* file, bool pack):_ofsx(NULL),_ifs(NULL),_pack(pack)
{
  if(pack)
  {
    _ofsx = (  new ofstream(file,ios::out | ofstream::binary));
  }
  else
  {
    _ifs = (  new ifstream(file,ios::in | ofstream::binary));
  }  
}

sink::~sink()
{
  done();
  delete _ofsx;
  delete _ifs;   
}

/**
 * finalises file/buffer packing/unpacking
 */
long sink::done()
{
  long sz = no_err;
  if(_ofsx)
  {
    _ofsx->flush();

    ofstream* file = out_file();
    if(file)
    {
      cout << "closed serialised file" << endl;
      file->close();
    }

    stringstream* buf = out_buf();
    if(buf)
    {
      buf->seekp(0, ios::end);
      stringstream::pos_type offset = buf->tellp();
    _data.resize(offset);
        if(_data.size()<1)
          return buffer_sz_err;
          
        buf->seekg(0);

        buf->read((char*)&_data[0],_data.size());
        long __sz = buf->gcount();
        cout << "closed serialised buffer" << endl;
        if(__sz>_data.size())
          return buffer_sz_err;
    }    
  }
  
  if(_ifs)
  {  
    ofstream* file = in_file();
    if(file)
    {
        file->close();
        cout << "closed input file" << endl;
    }    
  }
  return sz;
}

/**
 *  Assigns new buffer (unpacking only)
 */
bool sink::set_buf(char* buffer, int len)
{
  if(!_pack)
  {
    if(_ifs)
      delete _ifs;
    _ifs = new stringstream(ios::in |ios::out| ifstream::binary);
    if(_ifs)
    {
      streambuf * pbuf = _ifs->rdbuf();
      pbuf->sputn (buffer,len);      
      _ifs->seekg(0);
      return true;
    }
  }
  return false;
}

/**
 * Gets the serializing file
 */
ofstream* sink::out_file()
{
  try
  {
    ofstream* file = dynamic_cast<ofstream*>(_ofsx);
    if(file)
      return file;
  }
  catch(...)
  {
        
  }
  return NULL;   
}

/**
 * Gets the serializing buffer
 */
stringstream* sink::out_buf()
{
  try
  {
    stringstream* b = dynamic_cast<stringstream*>(_ofsx);
    if(b)
      return b;
  }
  catch(...)
  {
    
  }
  return NULL;   
}

/**
 * Gets the deserializing file
 */
ofstream* sink::in_file()
{
  try
  {
    ofstream* file = dynamic_cast<ofstream*>(_ifs);
    if(file)
      return file;
  }
  catch(...)
  {
    
  }
  return NULL;   
}

/**
 * Gets the deserializing buffer
 */
stringstream* sink::in_buf()
{
  try
  {
    stringstream* b = dynamic_cast<stringstream*>(_ifs);
    if(b)
      return b;
  }
  catch(...)
  {
    
  }
  return NULL;   
}


