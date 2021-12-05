#pragma once

#include <cstdint>
#include <string>

namespace FMap {

using std::string;

// Base class that contains
class FileMapBase {
    public:
        FileMapBase(const std::string &filename, int flags);
        virtual void close();
        ~FileMapBase();
    protected:
        int file_id_ = -1; // Error file descriptor by default
        int64_t st_size_ = 0;
};

class FileMapRead : public FileMapBase {
    public:
        FileMapRead(const std::string &filename);
        const char* get_top() { return top_ptr_; }
        const char* get_bottom() { return bottom_ptr_; }
        int64_t bottom_position() { return bottom_position_; }
        int load_next_block();
        void close() override;
    protected:
        int64_t bottom_position_ = 0;
        int64_t allocated_size_ = 0;
        int64_t chunk_size_ = 0;
        char *bottom_ptr_ = nullptr;
        char *top_ptr_ = nullptr;
        void unmap_();
        void map_();
};

} // namespace FMap

