//
//  serialize.h
//  serrializer
//
//  Created by B Wigley on 12/04/2017.
//  Copyright © 2017 fab. All rights reserved.
//

#ifndef serialize_h
#define serialize_h

#include <string>
#include <vector>
#include <list>

#define INVALID_DATA_SZ_POS (0)

/**
  The reader/writer classes need to implement these methods for
  supporting intrinsic types.  
 */
struct serial
{
  typedef uint32_t size_type;

  serial():data_sz_pos(INVALID_DATA_SZ_POS){}

  virtual size_type serialize( uint8_t& value)=0;
  virtual size_type serialize( uint16_t& value)=0;
  virtual size_type serialize( uint32_t& value)=0;
  virtual size_type serialize( uint64_t& value)=0;
  virtual size_type serialize( float& value)=0;
  virtual size_type serialize( uint8_t* value, uint32_t& sz)=0;
  virtual size_type serialize( std::string& value, uint16_t& len)=0;
  virtual size_type serialize(std::vector<uint8_t>& data)=0;
  virtual size_type serialize(std::vector<int8_t>& data)=0;
  virtual bool  unarchiver()=0;

  virtual void serialize_data_size_init()=0;
  virtual uint32_t serialize_data_size(const size_type size)=0;
  virtual bool verify_data_size(const size_type size)=0;

  protected:
  size_t data_sz_pos;
};

//template helpers



template <class t,class container>
uint32_t _serialize(class serial& s,container& list){
  uint32_t done = 0;
  
  // the count of instances must be serialized first - even if it's not an 
  // explicit data member as in this class 
  typename container::size_type list_size = list.size();
  done += s.serialize((uint32_t&)list_size);

  if(list_size>0 && s.unarchiver())
  {
    for(int c=0;c<list_size;++c)
      list.push_back(new t());
  }
  
  typename container::iterator it = list.begin();
  for(;it!=list.end();++it)
  {
    done += (*it)->serialize_payload(s);      
  }
  
  return done;
}

/**
 * when deserializing this has the side-effrect of allocating on the heap new 
 * instances in the list
 */
template <class t>
uint32_t serialize(class serial& s,std::vector<t*>& list){
  return ::_serialize<t>(s,list);
}    

/**
 * when deserializing this has the side-effrect of allocating on the heap new 
 * instances in the list
 */
template <class t>
uint32_t serialize(class serial& s,std::list<t*>& list){
  return ::_serialize<t>(s,list);
}    

/**
 * serialise a pointer to a custom type
 *
 * when deserializing this has the side-effrect of allocating on the heap a 
 * new instance (if present in the data buffer)
 */
template <class t>
uint32_t serialize(class serial& s, t** pp_inst)
{
  uint8_t present = *pp_inst != NULL ? 1 : 0;
  uint32_t done = s.serialize(present);    
  if(present==1) 
  {
    if(s.unarchiver())
    {
        delete *pp_inst;
        *pp_inst = new t();
    }
    done += (*pp_inst)->serialize_payload(s);
  }
  return done;
}     



#endif /* serialize_h */
