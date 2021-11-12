#include <fstream>
#include <iostream>
#include "VK_Util.h"

std::vector<char> readDataFromFile(const std::string &filename)
{
    std::vector<char> buffer;

    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "failed to open file:" << filename.data() << std::endl;
        return buffer;
    }

    size_t size = (size_t) file.tellg();
    buffer.resize(size);

    file.seekg(0);
    file.read(buffer.data(), size);
    file.close();
    return buffer;
}
