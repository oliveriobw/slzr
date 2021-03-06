    //
    //  serial_read.h
    //  serrializer
    //
    //  Created by B Wigley on 12/04/2017.
    //  Copyright © 2017 fab. All rights reserved.
    //

#ifndef serialize_read_h
#define serialize_read_h

#include "serialize.h"
#include <vector>

struct serial_read  : public serial{
    
        //memory buffer or file  
    serial_read(std::istream& ifs):_ifs(ifs){
        //to allow repeated unpack calls:
        _ifs.seekg(0);
    
    }
    
    std::istream& _ifs;
    
    float float_swap(float value){
        union v {
            float        f;
            unsigned int i;
        };
        
        union v val;
        unsigned int temp;
        
        val.f = value;
        temp = ntohl(val.i);
        
        return *(float*)&temp;
    }
    
    template <typename T> size_type serialize( T& value)
    {
    assert(0);
    return 0;
    }
    
    virtual size_type serialize(uint8_t& value)
    {
    _ifs.read((char*)&value, sizeof value);  
#ifdef DEBUG
    std::cout << "read uint8_t = " << value ;
    std::cout << " => " << value << std::endl;
#endif    
    return sizeof value;
    }
    
    virtual size_type serialize(uint16_t& value)
    {
    _ifs.read((char*)&value, sizeof value);  
#ifdef DEBUG
    std::cout << "read uint16_t = " << value ;
    value = ntohs(value);
    std::cout << " => " << value << std::endl;
#else    
    value = ntohs(value);
#endif    
    return sizeof value;
    }
    
    virtual size_type serialize(uint32_t& value)
    {
    _ifs.read((char*)&value, sizeof value);  
#ifdef DEBUG
    std::cout << "read uint32_t = " << value ;
    value = ntohl(value);
    std::cout << " => " << value << std::endl;
#else    
    value = ntohl(value);
#endif    
    return sizeof value;
    }
    
    virtual   size_type serialize(uint64_t& value)
    {
    _ifs.read((char*)&value, sizeof value);  
#ifdef DEBUG
    std::cout << "read uint64_t = " << value ;
    value = ntohll(value);
    std::cout << " => " << value << std::endl;
#else    
    value = ntohll(value);
#endif    
    return sizeof value;
    }
    
    virtual   size_type serialize(float& value)
    {
    _ifs.read((char*)&value, sizeof value);  
#ifdef DEBUG
    std::cout << "read float = " << value ;
    value = float_swap(value);
    std::cout << " => " << value << std::endl;
#else    
    value = float_swap(value);
#endif    
    return sizeof value;
    }
    
    size_type serialize( uint8_t* buffer, uint32_t& len)
    {
    serialize(len);
    
    _ifs.read((char*)buffer,len);
    
    if (!_ifs)
        {
        std::cout << "error: only \"" << _ifs.gcount() << " bytes \" could be read";
        return 0;
        }    
    
    return len;  
    }
    
    /**
     * variable buffers
     */
    size_type serialize(std::vector<uint8_t>& buf)
    {
    uint32_t len=0;
    serialize(len);
    if(len>0)
        {
        buf.resize(len);
        _ifs.read((char*)&buf[0],len);
        if (!_ifs)
            {
            std::cout << "error: only \"" << _ifs.gcount() << " bytes \" could be read";
            return 0;
            }    
        }  
    return len;
    }
    
    /**
     * variable buffers
     */
    size_type serialize(std::vector<int8_t>& buf)
    {
    uint32_t len=0;
    serialize(len);
    if(len>0)
        {
        buf.resize(len);
        _ifs.read((char*)&buf[0],len);
        if (!_ifs)
            {
            std::cout << "error: only \"" << _ifs.gcount() << " bytes \" could be read";
            return 0;
            }    
        }  
    return len;
    }
    

    
    
        //reads string size then string payload
        //treats incoming payload like a string - so zero terminated after unpacking  
    virtual size_type serialize( std::string& value)
    {
    uint16_t len=0;
    serialize(len);
    
    char buffer[len+1];
    memset(buffer,0,len+1);
    _ifs.read(&buffer[0],len);
    
    if (!_ifs)
        {
        std::cout << "error: only \"" << _ifs.gcount() << " bytes \" could be read";
        return 0;
        }    
    
    value = std::string(buffer);
    std::cout << "read " << len << " bytes="  << value << std::endl;//nb: as string
    
    return len;  
    }
    
    virtual size_type serialize( std::string& value, uint16_t& len)
    {
    serialize(len);
    
    char buffer[len+1];
    memset(buffer,0,len+1);
    _ifs.read(&buffer[0],len);
    
    if (!_ifs)
        {
        std::cout << "error: only \"" << _ifs.gcount() << " bytes \" could be read";
        return 0;
        }    
    
    value = std::string(buffer);
    std::cout << "read " << len << " bytes="  << value << std::endl;//nb: as string
    
    return len;  
    }
    
    virtual size_t position() 
    {
    size_t pos= _ifs.tellg();
    return pos;
    }
    
    virtual bool seek(const size_t pos)
    {
    return true;
    }
    
        // get the data size field from the current position
    virtual void serialize_data_size_init()
    {
    serialize(data_size); 
    }
    
        // no-op when reading  
    virtual uint32_t serialize_data_size(const size_type size)
    {
    return 0;
    }
    
        //ensire the reported size in the payload matches that required by the
        //class
    virtual bool verify_data_size(const size_type size)
    {
    return size == data_size;
    }
    
    virtual bool  unarchiver(){ return true;}
    
protected:
    size_type data_size;
};



#endif /* serialize_read_h */
