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

fs_sink::fs_sink(const char* buffer, int len, bool write):ofsx(NULL),ifs(NULL)
{
//  if(write)
//  {
//      ofsx = new std::stringstream( std::ios::out | std::ofstream::binary);
//  }
//  else
//      ifs = new std::stringstream(std::ios::in | std::ifstream::binary);  
}

fs_sink::fs_sink(const char* file, bool write):ofsx(NULL),ifs(NULL)
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
  if(ofsx)
  {
    ofsx->flush();
    ofsx->close();
    delete ofsx;
  }

  if(ifs)
  {  
    ifs->close();
    delete ifs;
  }
}
