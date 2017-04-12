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

fs_sink::fs_sink(const char* buffer, int len)
{
//tbd memory buffer
    //ofs = new  std::stringstream(std::ios::in | std::ios::out | std::ofstream::binary);
  //ifs = new  std::stringstream(std::ios::in | std::ios::out | std::ifstream::binary);
  
  ofsx = NULL;//new  std::stringstream(std::ios::in | std::ios::out | std::ofstream::binary);
  
}

fs_sink::fs_sink(const char* file, bool write)
{
  if(write)
  {
    //deletes the file too
    ofsx = (  new std::ofstream(file,std::ios::out | std::ofstream::binary));
  }

  ifs = (  new std::ifstream(file,std::ios::in | std::ofstream::binary));
  
    //  if(write)
    //    {
    //    int r  =std::remove(file);
    //    cout << "remove res:" <<r <<endl;
    //    }  
  
    //ofs = new std::ofstream(file,std::ios::out | std::ofstream::binary);
  
    //  ofstream  _ofs(file,std::ios::out | std::ofstream::binary);
    //  int g;
    //  _ofs << &g;
    //  _ofs << "lol";
    //  _ofs.close();
  
    //  ifs = new std::ifstream(file,std::ios::in | std::ifstream::binary);
  
  {
  std::ifstream f( file, std::ios::binary | std::ios::ate);
  int len  = f.tellg();
  cout << "current file size = " << len << endl;
  
  }
}

fs_sink::~fs_sink()
{
    //((std::ofstream*)ofs)->flush();
    //((std::ifstream*)ifs)->flush();
  ofsx->flush();
  ofsx->close();

  //ifs->flush();
  ifs->close();
  
    //((std::ofstream*)ofs)->close();
    //((std::ifstream*)ifs)->close();
  
    delete ofsx;
    delete ifs;
  
}
