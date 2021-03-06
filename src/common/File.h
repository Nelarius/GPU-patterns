#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

#include <string>
#include <sstream>
#include <fstream>
#include <sys/stat.h>

namespace narwhal {

/// @brief Check whether a file of the given name exists.
inline bool fileExists(const std::string& file) {
    struct stat buffer;
    return (stat(file.c_str(), &buffer) == 0);
}

/// @brief Get the contents of a file as a string.
inline std::string fileToString(const std::string& file) {
    std::ifstream fin;
    fin.open(file, std::ios::in);
    if (!fileExists(file)) {
        return "No such file!";
    }

    std::stringstream buffer;
    buffer << fin.rdbuf();
    return buffer.str();
}

}

#endif // FILE_H_INCLUDED
