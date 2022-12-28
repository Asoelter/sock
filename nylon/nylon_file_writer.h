#ifndef NYLON_FILE_WRITER_H
#define NYLON_FILE_WRITER_H

#include "namespace.h"

#include "util/file_ptr.h"

#include <cstdio>
#include <memory>
#include <span>

NYLON_NAMESPACE_BEGIN

class FileWriter
{
public:
    FileWriter(std::string const & filename);

    void write(std::span<const char> bytes);

private:
    FilePtr file_;
};

NYLON_NAMESPACE_END

#endif // NYLON_FILE_WRITER_H
