#pragma once
#include "src/file_map.hh"

namespace RndFile {

class RandomFile : public FMap::FileMapBase {
    public:
        RandomFile(const std::string &filename, size_t size);
        void write_string(size_t offset, const std::string string);
};

} // namespace RndFile
