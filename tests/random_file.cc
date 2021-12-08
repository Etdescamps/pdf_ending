#include "random_file.hh"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <system_error>

namespace RndFile {

RandomFile::RandomFile(const std::string &filename, size_t size) : FMap::FileMapBase(filename, O_CREAT | O_TRUNC | O_WRONLY) {
    fchmod(file_id_, S_IRUSR | S_IWUSR);
    if(ftruncate(file_id_, size) < 0)
        throw std::system_error(errno, std::generic_category());
    st_size_ = size;
}

void RandomFile::write_string(size_t offset, const std::string string) {
    lseek(file_id_, offset, SEEK_SET);
    write(file_id_, (void*) string.c_str(), string.length());
}

} // namespace RndFile
