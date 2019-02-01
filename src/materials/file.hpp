# ifndef FILE_HPP
# define FILE_HPP

#include <stdlib.h>
#include <fstream>
#include <iostream>

template<typename T>
class file {
    
public:

    static void save_to_file(std::string path, const T& obj)
    {
        std::stringstream ss;
        ss << obj;
        std::ofstream fh;
        fh.open(path, std::ios::binary);
        ss.rdbuf()->pubseekpos(0, std::ios_base::out);
        fh << ss.rdbuf();
        fh.flush();
        fh.close();
    }

    static void load_from_file(std::string path, T& objIn)
    {
        int count = 0;
        std::cout << ++count << std::endl;
        std::stringstream ss;
        std::ifstream fh(path, std::ios::binary);
        std::cout << ++count << std::endl;
        ss << fh.rdbuf();
        std::cout << ++count << std::endl;
        fh.close();
        std::cout << ++count << std::endl;
        ss.rdbuf()->pubseekpos(0, std::ios_base::in);
        std::cout << ++count << std::endl;
        ss >> objIn;
        std::cout << ++count << std::endl;
    }

    static void load_from_file(T& objIn, std::string dir, std::string file){
        std::string path = dir + "/" + file;
        load_from_file(path, objIn);
    }

    static void save_to_file(T& objIn, std::string dir, std::string file){
        std::string path = dir + "/" + file;
        save_to_file(path, objIn);
    }


    static bool is_file(std::string path)
    {
        std::ifstream infile(path);
        return infile.good();
    }

    static bool is_file(std::string dir, std::string file)
    {
        string file_path = dir + "/" + file;
        return is_file(file_path);
    }

};

# endif