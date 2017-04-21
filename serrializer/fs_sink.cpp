//
//  fs_sink.cpp
//  serrializer
//
//  Created by B Wigley on 12/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#include "fs_sink.hpp"
#include <fstream>
#include <sstream>

using namespace std;

/**
 * Prepares buffer for packing/unpacking
 */
fs_sink::fs_sink(char* buffer, int len, bool pack):_ofsx(NULL),_ifs(NULL),_len(0),_buf(NULL),_pack(pack)
{
  if(pack)
  {
      _ofsx = new stringstream( ios::out |ios::in | ofstream::binary);
      _buf=buffer;
      _len=len;
  }
  else
  {
      _ifs = new stringstream(ios::in |ios::out| ifstream::binary);
      streambuf * pbuf = _ifs->rdbuf();
      pbuf->sputn (buffer,len);
  }  
}

/**
 * Prepares file for packing/unpacking
 */
fs_sink::fs_sink(const char* file, bool pack):_ofsx(NULL),_ifs(NULL), _len(0),_buf(NULL),_pack(pack)
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

fs_sink::~fs_sink()
{
  done();
  delete _ofsx;
  delete _ifs;   
}

/**
 * finalises file/buffer packing/unpacking
 */
void fs_sink::done()
{
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
        buf->seekg(0);
        buf->read(_buf,_len);
        cout << "closed serialised buffer" << endl;
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
}

/**
 *  Assigns new buffer (unpacking only)
 */
bool fs_sink::set_buf(char* buffer, int len)
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
ofstream* fs_sink::out_file()
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
stringstream* fs_sink::out_buf()
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
ofstream* fs_sink::in_file()
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
stringstream* fs_sink::in_buf()
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


