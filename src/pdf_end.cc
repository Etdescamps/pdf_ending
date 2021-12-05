#include <iostream>
#include "file_map.hh"

int64_t find_offset(FMap::FileMapRead &pdfFile) {
    const char revert_trailer[] = "FOE%%";
    const char *pos = &revert_trailer[0];
    const char *top = pdfFile.get_top() - 1;
    const char *bottom = pdfFile.get_bottom();
    while(true) {
        if(*pos == 0) {
            // We got a complete "%%EOF"
            // we verify if there is an '\n' or an '\r' before
            if(*top == '\n' || *top == '\r')
                return pdfFile.bottom_position() + (top - bottom) + 1;
            // else
            pos = &revert_trailer[0];
            continue; // Reevaluate the iteration looking for 'F'
        }
        else if(*top == *pos)
            ++pos;
        else if (pos > &revert_trailer[0]) {
            pos = &revert_trailer[0];
            continue; // Reevaluate the iteration looking for 'F'
        }
        --top; // 
        if(top <= bottom) {
            if(pdfFile.load_next_block() == 0)
                return -1; // Begin of the file: no occurences
            top = pdfFile.get_top(); --top;
            bottom = pdfFile.get_bottom();
        }
    }
    return -1; // Avoid warning
}

int main(int argc, char **argv) {
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " filename.pdf" << std::endl;
        std::cerr << "Give the offset position of the file trailer of the PDF on the standard output" << std::endl;
        std::cerr << "Print -1 if this trailer is not present" << std::endl;
        return 1;
    }
    try {
        FMap::FileMapRead pdfFile(argv[1]);
        std::cout << find_offset(pdfFile) << std::endl;;
        pdfFile.close();
    }
    catch(const std::system_error &e) {
        std::cerr << "Error with file: " << argv[1] << std::endl;
        std::cerr << "System code (" << e.code() << "): " << e.what() << '\n';
    }
    return 0;
}
