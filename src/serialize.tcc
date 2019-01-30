#ifndef SERIALIZE_TCC
#define SERIALIZE_TCC

#include <stdlib.h>
#include <fstream>
#include <iostream>


namespace file {

    template<typename T>
    void save_to_file(std::string& path, T& obj) {

        cout << "Saving to " << path << endl; 

        std::stringstream ss;
        ss << obj;
        std::ofstream fh;
        fh.open(path, std::ios::binary);
        ss.rdbuf()->pubseekpos(0, std::ios_base::out);
        fh << ss.rdbuf();
        fh.flush();
        fh.close();
    };

    template<typename T>
    void load_from_file(std::string& path, T& objIn) {

        cout << "Loading from " << path << endl; 

        std::stringstream ss;
        std::ifstream fh(path, std::ios::binary);
        ss << fh.rdbuf();
        fh.close();
        ss.rdbuf()->pubseekpos(0, std::ios_base::in);
        ss >> objIn;
    };

    bool is_file(std::string& path)
    {
        std::ifstream infile(path);
        return infile.good();
    }

}

#endif