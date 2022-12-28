#ifndef NYLON_FILE_PTR_H
#define NYLON_FILE_PTR_H

#include "../namespace.h"

#include <cstdio>
#include <memory>

NYLON_NAMESPACE_BEGIN

struct FileCloser
{
    void operator()(FILE* f)
    {
        fclose(f);
    }
};

using FilePtr = std::unique_ptr<FILE, FileCloser>;

NYLON_NAMESPACE_END

#endif // NYLON_FILE_PTR_H
