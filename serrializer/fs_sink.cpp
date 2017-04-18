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

fs_sink::fs_sink(char* buffer, int len, bool write):ofsx(NULL),ifs(NULL),_len(0),_buf(NULL)
{
  if(write)
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
      
      
     // ifs->write(buffer,;len)
//      stringbuf* pbuf=(stringbuf*)ifs->rdbuf();
//      pbuf->sputn (buffer,len);  
  }  
}

fs_sink::fs_sink(const char* file, bool write):ofsx(NULL),ifs(NULL), _len(0),_buf(NULL)
{
  if(write)
  {
    //nb: deletes the file too
    ofsx = (  new std::ofstream(file,std::ios::out | std::ofstream::binary));
  }
  else
    ifs = (  new std::ifstream(file,std::ios::in | std::ofstream::binary));
  
  {
     std::ifstream f( file, std::ios::binary | std::ios::ate);
     int len  = f.tellg();
     cout << "current file size = " << len << endl;  
  }
}

fs_sink::~fs_sink()
{
   done();
}

void fs_sink::done()
{
  if(ofsx)
  {
    ofsx->flush();
    
    try{
      std::ofstream* file = dynamic_cast<std::ofstream*>(ofsx);
      if(file)
        file->close();
    }
    catch(...)
    {
      
    } 
    
    try{
      std::stringstream* buffer = dynamic_cast<std::stringstream*>(ofsx);
      if(buffer)
      {
        buffer->seekg(0);
        buffer->read(_buf,_len);
      }
    }
    catch(...)
    {
      
    } 
    
    delete ofsx;
    ofsx = NULL;
  }
  
  if(ifs)
  {  
    try{
      std::ifstream* file = dynamic_cast<std::ifstream*>(ofsx);
      if(file)
        file->close();
    }
    catch(...)
    {
      
    }
    
    delete ifs;
    ifs = NULL;
  }
}
