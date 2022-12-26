#include "nylon_file_writer.h"

#include <string>

NYLON_NAMESPACE_BEGIN

FileWriter::FileWriter(std::string const & filename)
    : file_(fopen(filename.c_str(), "w"))
{
}

void FileWriter::write(std::span<const char> bytes)
{
    fwrite(bytes.data(), sizeof(char), bytes.size(), file_.get());
}

NYLON_NAMESPACE_END
