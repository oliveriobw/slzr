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
fs_sink::fs_sink(char* buffer, int len, bool pack):ofsx(NULL),ifs(NULL),_len(0),_buf(NULL),_pack(pack)
{
  if(pack)
  {
      ofsx = new std::stringstream( std::ios::out |std::ios::in | std::ofstream::binary);
      _buf=buffer;
      _len=len;
  }
  else
  {
      ifs = new std::stringstream(std::ios::in |std::ios::out| std::ifstream::binary);
      std::streambuf * pbuf = ifs->rdbuf();
      pbuf->sputn (buffer,len);
  }  
}

/**
 * Prepares file for packing/unpacking
 */
fs_sink::fs_sink(const char* file, bool pack):ofsx(NULL),ifs(NULL), _len(0),_buf(NULL),_pack(pack)
{
  if(pack)
  {
    //nb: deletes the file
    ofsx = (  new std::ofstream(file,std::ios::out | std::ofstream::binary));
  }
  else
  {
    ifs = (  new std::ifstream(file,std::ios::in | std::ofstream::binary));
  }  
}

fs_sink::~fs_sink()
{
  done();
  delete ofsx;
  delete ifs;   
}

/**
 * finalises file/buffer packing/unpacking
 */
void fs_sink::done()
{
  if(ofsx)
  {
    ofsx->flush();

    std::ofstream* file = out_file();
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
  
  if(ifs)
  {  
    std::ofstream* file = in_file();
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
    if(ifs)
      delete ifs;
    ifs = new std::stringstream(std::ios::in |std::ios::out| std::ifstream::binary);
    if(ifs)
    {
      std::streambuf * pbuf = ifs->rdbuf();
      pbuf->sputn (buffer,len);      
      ifs->seekg(0);
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
    std::ofstream* file = dynamic_cast<std::ofstream*>(ofsx);
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
    std::stringstream* b = dynamic_cast<std::stringstream*>(ofsx);
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
    std::ofstream* file = dynamic_cast<std::ofstream*>(ifs);
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
    std::stringstream* b = dynamic_cast<std::stringstream*>(ifs);
    if(b)
      return b;
  }
  catch(...)
  {
    
  }
  return NULL;   
}


