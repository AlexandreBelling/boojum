#ifndef SERIALIZE_SNARK_HPP
#define SERIALIZE_SNARK_HPP

#include <stdlib.h>
#include <iostream>

namespace file {

    template<typename T>
    extern void save_to_file(std::string& path, T& obj);

    template<typename T>
    extern void load_from_file(std::string& path, T& objIn);

    extern bool is_file(std::string& path);

    template<typename T>
    extern std::string to_string(T& obj); 

    template<typename T>
    extern void load_from_string(void* buffer, T& obj, int size);
        
    template<typename T>
    extern void load_pinput_from_string(void* buffer, vector<T>& obj, int size);

}

#include "serialize.tcc"
#endif