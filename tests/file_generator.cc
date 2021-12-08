#include <iostream>
#include <cstdlib>
#include "tests/random_file.hh"

void create_randomfile(const std::string &filename, size_t size, size_t offset) {
    RndFile::RandomFile file(filename, size);
    std::string str("\n%%EOF");
    if(offset < size - 20) {
        int imax = int(str.length() - 1);
        for(int i = 1; i < imax; ++i)
            file.write_string((std::rand() % (size - offset - 16)) + offset + 6,str.substr(i));
    }
    file.write_string(offset-1, str);
    file.close();
}

int main(int argc, char **argv) {
    if(argc < 4) {
        std::cerr << "Usage: " << argv[0] << " filename size offset" << std::endl;
        return 1;
    }
    char *argvend;
    long long size = std::strtoll(argv[2], &argvend, 10);
    long long offset = std::strtoll(argv[3], &argvend, 10);
    std::string filename(argv[1]);
    try {
        create_randomfile(filename, size, offset);
    }
    catch(const std::system_error &e) {
        std::cerr << "Error with file: " << argv[1] << std::endl;
        std::cerr << "System code (" << e.code() << "): " << e.what() << '\n';
    }
    return 0;
}

