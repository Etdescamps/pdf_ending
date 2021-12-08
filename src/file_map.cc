#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <system_error>
#include <cassert>
#include <cerrno>
#include <unistd.h>
#include <algorithm>
#include "file_map.hh"

namespace FMap {

long pageSize = -1; // Page size

inline static int close_(int i) { return close(i); }

// Open file and determine the size
FileMapBase::FileMapBase(const std::string &filename, int flags) {
    if(pageSize < 0)
        pageSize = sysconf(_SC_PAGESIZE); // Get page size from the OS
    file_id_ = open(filename.c_str(), flags);
    if(file_id_ < 0)
        throw std::system_error(errno, std::generic_category());
    struct stat statbuf;
    if(fstat(file_id_, &statbuf) < 0)
        throw std::system_error(errno, std::generic_category());
    st_size_ = statbuf.st_size;
}

// Close file handler
void FileMapBase::close() {
    int return_value = close_(file_id_);
    file_id_ = -1;
    if(return_value < 0)
        throw std::system_error(errno, std::generic_category());
}

FileMapBase::~FileMapBase() {
    // Had to call close before the destructor (avoid exception in destructor)
    assert(file_id_ < 0);
}

FileMapRead::FileMapRead(const std::string &filename) : FileMapBase(filename, O_RDONLY) {
    if(st_size_ == 0)
        return; // No contents in the file
    chunk_size_ = pageSize; // The allocation size is a multiple of the page size
    // Start with the smallest amount of data (one page or two):
    // avoid loading large chunks of data if %%EOF is near the end
    if(st_size_ <= chunk_size_)
        bottom_position_ = 0;
    else {
        bottom_position_ = (st_size_/pageSize)*pageSize;
        if(st_size_ - bottom_position_ < 30) // load at least 30 bytes from the file
            bottom_position_ -= pageSize;
    }
    allocated_size_ = st_size_ - bottom_position_;
    map_();
}

// Utility function for unmapping and error handling
void FileMapRead::unmap_() {
    // Accelerate memory freeing from the OS
    madvise(bottom_ptr_, allocated_size_, MADV_DONTNEED);
    if(munmap(bottom_ptr_, allocated_size_) < 0)
        throw std::system_error(errno, std::generic_category());
}

// Utility function for mapping into memory and error handling
void FileMapRead::map_() {
    bottom_ptr_ = (char *) mmap(nullptr, allocated_size_, PROT_READ, MAP_PRIVATE, file_id_, bottom_position_);
    if(bottom_ptr_ == MAP_FAILED)
        throw std::system_error(errno, std::generic_category());
    // Indicate to the OS to preload the chunk into memory (non blocking call)
    madvise(bottom_ptr_, allocated_size_, MADV_WILLNEED);
    top_ptr_ = bottom_ptr_ + allocated_size_;
}

// Load the next block of the file into memory
int FileMapRead::load_next_block() {
    if(bottom_position_ == 0) {
        if(bottom_ptr_) {
            unmap_();
            top_ptr_ = nullptr;
            bottom_ptr_ = nullptr;
        }
        return 0; // End of File
    }
    unmap_();
    // Increase progressively the chunk size (UP to 1 MB of memory)
    // Bigger chunk are better for readling large file without a %%EOF
    if(chunk_size_ < 1'000'000)
        chunk_size_ *= 2;
    long new_bottom = std::max(((bottom_position_ - chunk_size_)/pageSize)*pageSize, 0l);
    allocated_size_ = bottom_position_ - new_bottom;
    bottom_position_ = new_bottom;
    map_();
    new_bottom = std::max(((bottom_position_ - chunk_size_)/pageSize)*pageSize, 0l);
    // Preload next block
    if(new_bottom < bottom_position_)
        posix_fadvise64(file_id_, new_bottom, bottom_position_ - new_bottom, MADV_WILLNEED);
    return 1;
}

void FileMapRead::close() {
    // Function for closing file (for handling exceptions)
    if(bottom_ptr_)
        unmap_();
    FileMapBase::close();
}

} // namespace FMap

