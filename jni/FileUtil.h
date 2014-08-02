#ifndef CPT_FILE_UTIL_H
#define CPT_FILE_UTIL_H

#include <string>

#include "Error.h"

// 移动文件。如果在不同的磁盘上，不进行拷贝。
errno_t RenameFile(const std::string& from, const std::string& to);

#endif /* CPT_FILE_UTIL_H */